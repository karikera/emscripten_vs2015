#include "stdafx.h"
#include "tlog.h"
#include <KRUtil/fs/path.h>

constexpr char16 BOM_UTF16 = (char16)0xfeff;

TLogGroup::TLogGroup(Set<Text16> * group) noexcept
	:m_group(group)
{
}
bool TLogGroup::put(Text16 value) noexcept
{
	auto res = m_group->insert(value);
	return res.second;
}
bool TLogGroup::putPath(Text16 value) noexcept
{
	auto res = m_group->insert(path16.resolve(value));
	return res.second;
}
bool TLogGroup::putPath(View<AText16> inputs) noexcept
{
	bool res = true;
	for (Text16 text : inputs)
	{
		res = putPath(text) && res;
	}
	return res;
}

void TLogInputs::put(Text16 value) noexcept
{
	m_inputs << path16.resolve(value) << u"|";
}

TLog::TLog() noexcept
{
}
void TLog::load(pcstr16 path) noexcept
{
	m_filepath.clear();
	m_filepath << path;
	m_map.clear();

	try
	{
		int line = 0;
		TText16 file = File::openAsArrayTemp<char16>(path);
		Text16 reader = file;
		if (*reader == BOM_UTF16)
			reader++;

		auto readLine = [&] {
			for (;;)
			{
				line++;
				if (reader.empty()) throw Error();
				Text16 line = reader.readwith_e(u"\r\n");;
				if (line.empty()) continue;
				return line;
			}
		};

		Set<Text16> *list = nullptr;
		for (;;)
		{
			Text16 value = readLine();
			if (*value == u'^')
			{
				list = &m_map[value+1];
				continue;
			}
			if (list == nullptr)
			{
				ucerr << path << u'(' << line << u"): error : Must starts with ^ character.\n";
				continue;
			}
			list->insert(value);
		}
	}
	catch (Error&)
	{
	}
}
void TLog::save() noexcept
{
	if (m_filepath == nullptr) return;

	try
	{
		Must<io::FileStream<void>> file = File::create(m_filepath.c_str())->stream<void>();
		TText16 output;
		constexpr size_t FLUSH_SIZE = 4096;

		auto flushTest = [&] {
			if (output.size() >= FLUSH_SIZE)
			{
				file->write(output.data(), FLUSH_SIZE * sizeof(char16));
				output.remove(0, FLUSH_SIZE);
			}
		};
		memcheck();
		output.write(BOM_UTF16);
		for (auto & pair : m_map)
		{
			memcheck();
			output << u'^' << pair.first.cast<char16>() << u"\r\n";
			flushTest();
			for (auto & pair2 : pair.second)
			{
				memcheck();
				output << pair2.cast<char16>() << u"\r\n";
				flushTest();
			}
			memcheck();
		}
		file->write(output.data(), output.sizeBytes());
	}
	catch (Error&)
	{
		ucerr << m_filepath << u": error : Cannot save.\n";
	}
}
TLogGroup TLog::get(Text16 input) noexcept
{
	return &m_map[input];
}
TLogGroup TLog::get(View<AText16> inputs) noexcept
{
	TSZ16 inputTexts;
	for (Text16 input : inputs)
	{
		path16.resolveAppend(&inputTexts, input);
		inputTexts << u"|";
	}
	inputTexts.resize(inputTexts.size() - 1);
	return &m_map[inputTexts];
}
TLogGroup TLog::get(const TLogInputs& inputs) noexcept
{
	_assert(!inputs.m_inputs.empty());
	return &m_map[inputs.m_inputs.cut(inputs.m_inputs.size() - 1)];
}
TLogGroup TLog::reset(Text16 input) noexcept
{
	Set<Text16>* group = &m_map[input];
	group->clear();
	return group;
}
TLogGroup TLog::reset(View<AText16> inputs) noexcept
{
	TLogGroup group = get(inputs);
	group.m_group->clear();
	return group;
}
TLogGroup TLog::reset(const TLogInputs& inputs) noexcept
{
	TLogGroup group = get(inputs);
	group.m_group->clear();
	return group;
}
bool TLog::reset(View<AText16> inputs, Set<Text16> values) noexcept
{
	TLogGroup group = get(inputs);
	if (*group.m_group == values) return false;
	*group.m_group = move(values);
	return true;
}
void TLog::put(Text16 input, Text16 value) noexcept
{
	m_map[input].insert(value);
}

Set<Text16> TLog::resolve(View<AText16> values) noexcept
{
	Set<Text16> sets;
	for (Text16 text : values)
	{
		sets.insert(text);
	}
	return sets;
}