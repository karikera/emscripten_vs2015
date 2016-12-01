#include "stdafx.h"
#include "tlog.h"

constexpr wchar BOM_UTF16 = (wchar)0xfeff;

TLogGroup::TLogGroup(Map<TextW, Empty> * group) noexcept
	:m_group(group)
{
}
void TLogGroup::put(TextW value) noexcept
{
	m_group->insert(value, Empty());
}
void TLogGroup::putPath(TextW value) noexcept
{
	m_group->insert(resolve(value), Empty());
}
void TLogGroup::putPath(RefArray<ATextW> inputs) noexcept
{
	for (TextW text : inputs)
	{
		putPath(text);
	}
}

void TLogInputs::put(TextW value) noexcept
{
	m_inputs << resolve(value) << L"|";
}

TLog::TLog() noexcept
{
}
void TLog::load(pcwstr path) noexcept
{
	m_filepath.clear();
	m_filepath << path;
	m_map.clear();

	try
	{
		int line = 0;
		TTextW file = File::openAsArrayTemp<wchar>(path);
		TextW reader = file;
		if (*reader == BOM_UTF16)
			reader++;

		auto readLine = [&] {
			for (;;)
			{
				line++;
				if (reader.empty()) throw FileException();
				TextW line = reader.readto_e(L"\r\n");;
				if (line.empty()) continue;
				return line;
			}
		};

		Map<TextW, Empty> *list = nullptr;
		for (;;)
		{
			TextW value = readLine();
			if (*value == L'^')
			{
				list = &m_map[value+1];
				continue;
			}
			if (list == nullptr)
			{
				wcerr << path << L'(' << line << L"): error : Must starts with ^ character.\n";
				continue;
			}
			list->emplace(value, Empty());
		}
	}
	catch (FileException&)
	{
	}
}
void TLog::save() noexcept
{
	if (m_filepath == nullptr) return;

	try
	{
		Must<File> file = File::create(m_filepath.c_str());
		TTextW output;
		constexpr size_t FLUSH_SIZE = 4096;

		auto flushTest = [&] {
			if (output.size() >= FLUSH_SIZE)
			{
				file->write(output.data(), FLUSH_SIZE * sizeof(wchar));
				output.remove(0, FLUSH_SIZE);
			}
		};

		output.write(BOM_UTF16);
		for (auto & pair : m_map)
		{
			output << L'^' << pair.first.cast<wchar>() << L"\r\n";
			flushTest();
			for (auto & pair2 : pair.second)
			{
				output << pair2.first.cast<wchar>() << L"\r\n";
				flushTest();
			}
		}
		file->write(output.data(), output.sizeBytes());
	}
	catch (FileException&)
	{
		wcerr << m_filepath << L": error : Cannot save.\n";
	}
}
TLogGroup TLog::reset(TextW input) noexcept
{
	Map<TextW, Empty>* group = &m_map[input];
	group->clear();
	return group;
}
TLogGroup TLog::reset(RefArray<ATextW> inputs) noexcept
{
	TSZW inputTexts;
	for (TextW input : inputs)
	{
		resolve(&inputTexts, input);
		inputTexts << L"|";
	}
	inputTexts.resize(inputTexts.size() - 1);
	return reset(inputTexts);
}
TLogGroup TLog::reset(const TLogInputs& inputs) noexcept
{
	_assert(!inputs.m_inputs.empty());
	return reset(inputs.m_inputs.cut(inputs.m_inputs.size() - 1));
}
void TLog::put(TextW input, TextW value) noexcept
{
	m_map[input].insert(value, Empty());
}
