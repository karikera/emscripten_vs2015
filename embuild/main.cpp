#include "stdafx.h"
#include "main.h"
#include "tlog.h"

#include <KR3/io/bufferedstream.h>
#include <KR3/io/selfbufferedstream.h>
#include <KRUtil/text/template.h>
#include <KRUtil/shell/process.h>
#include <KRUtil/fs/path.h>
#include <KRUtil/envvar.h>
#include <KRUtil/parameter.h>

#include <sstream>
#include <fstream>

#pragma comment(lib, "KR3.lib")
#pragma comment(lib, "KRUtil.lib")

using namespace kr;

void splitFileName(TextW path, TextW * dir, TextW * name) noexcept
{
	TextW find = path.find_ry(L"\\/");
	if (find == nullptr)
	{
		if (dir) *dir = L"";
		if (name) *name = path;
	}
	else
	{
		find++;
		if (dir) *dir = path.cut(find);
		if (name) *name = find;
	}
}

enum class ConfigurationType
{
	None,
	Object,
	Executable,
	StaticLibrary,
};

enum class LogOption
{
	FileName,
	CommandLine
};

namespace
{
	ATextW ex_arguments = L" -std=c++14 -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1 -s FULL_ES2=1";
	Array<ATextW> javascripts;
	Path path;
	ConfigurationType confType = ConfigurationType::None;
	LogOption logOption = LogOption::FileName;
	bool tlogMinTest = false;
	ATextW tlogDirectory;
	bool tlogEnable = false;
	bool noLogo = false;
	ATextW cmdline;

	ATextW INCLUDE;
	ATextW KEN_DIR;
	TLog tlogCommand;
	TLog tlogRead;
	TLog tlogWrite;
}

dword compile(TextW output, TextW filepath, TSZW & fullPath) noexcept;
dword lib(ATextW &output, WRefArray<ATextW> inputs) noexcept;
dword link(ATextW &output, WRefArray<ATextW> inputs) noexcept;
void makeCmdLine(int argn, wchar_t ** args) noexcept;
void loadTLog(TextW commandTag, TextW readTag, TextW writeTag) noexcept;
bool checkSourceModified(TextW output, TSZW & fullPath);
bool checkExeModified(ATextW &outputjs, WRefArray<ATextW> inputs, bool exportDefExists, bool assetExists) noexcept;
template <typename LAMBDA>
bool readDependency(File * file, const LAMBDA & lambda);

int CT_CDECL wmain(int argn, wchar_t ** args, wchar_t** envp)
{
	std::wcout.imbue(std::locale("Korean"));

	TextW exeDir = (TextW)args[0];
	exeDir = exeDir.cut(exeDir.find_r(L'\\')+1);

	{
		KEN_DIR = EnviromentVariableW(L"KEN_DIR");
	}

	ATextW output;
	Array<ATextW> inputs;
	
	
	{
		ParameterW param = {
			{ ParamPrefix, L"Xo" },
			{ ParamPrefix, L"Lo"},
			{ ParamPrefix, L"Fo"},
			{ ParamPrefix, L"g" },
			{ ParamPrefix, L"O" },
			{ ParamValue, L"D" },
			{ ParamPrefix, L"I"},
			{ ParamValue, L"log" },
			{ ParamValue, L"charset" },
			{ ParamNoValue, L"NOLOGO"},
			{ ParamValue, L"tlog_directory"},
			{ ParamNoValue, L"tlog_mintest"},
			{ ParamNoValue, L"tlog_enable"}
		};
		param.start(argn, args);
		bool res = param.foreach([&](TextW name, TextW value) {
			if (name == L"")
			{
				ATextW newpath = value;
				newpath.change(L'\\', L'/');
				inputs.emplace(newpath);
			}
			else if (name == L"Xo")
			{
				confType = ConfigurationType::Executable;
				output = value;
			}
			else if (name == L"Lo")
			{
				confType = ConfigurationType::StaticLibrary;
				output = value;
			}
			else if (name == L"Fo")
			{
				confType = ConfigurationType::Object;
				output = value;
			}
			else if (name == L"log")
			{
				if (value == L"filename")
					logOption = LogOption::FileName;
				else if (value == L"cmdline")
					logOption = LogOption::CommandLine;
				else
					wcerr << L"Invalid log option /" << name << L':' << value << L'\n';
			}
			else if (name == L"g")
			{
				if (value == L"1" || value == L"2" || value == L"3" || value == L"4")
					ex_arguments << L" -g" << value;
				else
					wcerr << L"Invalid debug infomation option /" << name << value << L'\n';
			}
			else if (name == L"D")
				ex_arguments << L" -D" << value;
			else if (name == L"I")
			{
				ex_arguments << L" -I\"" << resolve(value) << L"\"";
			}
			else if (name == L"charset")
				ex_arguments << L" -finput-charset=" << value;
			else if (name == L"O")
			{
				if (value == L"0" || value == L"s" || value == L"fast" || value == L"3" || value == L"g")
					ex_arguments << L" -"<< name << value;
				else
					wcerr << L"Invalid optimization option /" << name << value << L'\n';
			}
			else if (name == L"NOLOGO")
			{
				noLogo = true;
			}
			else if (name == L"tlog_directory")
			{
				tlogDirectory = value;
			}
			else if (name == L"tlog_mintest")
			{
				tlogMinTest = true;
			}
			else if (name == L"tlog_enable")
			{
				tlogEnable = true;
			}
		});
		if(!res)
			return EINVAL;
	}

	if(!noLogo) wcout << L"emscripten_vs2015 beta\n";
	if (tlogEnable)
	{
		File::createFullDirectory(tlogDirectory);
	}

	dword res;

	output.change(L'\\', L'/');
	switch (confType)
	{
	case ConfigurationType::Object:
	{
		{
			wchar_t ** arg = args + 1;
			wchar_t ** arge = args + argn;
			for (; arg != arge; arg++)
			{
				TextW argtx = (TextW)*arg;
				for (TextW input : inputs)
				{
					if (argtx != input) continue;
					goto _ignore;
				}
				cmdline << argtx << L' ';
			_ignore:;
			}
		}

		{
			ATextW VEM_EXCLUDE = EnviromentVariableW(L"VEM_EXCLUDE");
			VEM_EXCLUDE.change(L'\\', L'/');
			ReferenceMap<TextW, Empty> excludeMap;
			for (TextW exclude : VEM_EXCLUDE.splitIterable(L';'))
			{
				excludeMap.insert(exclude, Empty());
			}

			INCLUDE = EnviromentVariableW(L"INCLUDE");
			INCLUDE.change(L'\\', L'/');
			for (TextW include : INCLUDE.reverseSplitIterable(L";"))
			{
				if (include.empty()) continue;
				if (excludeMap.find(include) != excludeMap.end()) continue;
				ex_arguments << L" -I\"" << resolve(include) << L"\"";
			}
		}

		size_t cmdEndIdx = cmdline.size();
		size_t endIdx = output.size();
		bool outputIsDirectory = (output.endsWith(L'\\') || output.endsWith(L'/'));

		if (tlogEnable)
		{
			loadTLog(L"CL", L"CL", L"CL");

			TLogGroup group = tlogWrite.reset(inputs);

			if (outputIsDirectory)
			{
				for (TextW filepath : inputs)
				{
					TextW filename;
					splitFileName(filepath, nullptr, &filename);
					output.cut_self(endIdx);
					output << filename << L".bc";
					group.putPath(output);
					output.resize(output.size() - 3);
					output << L".d";
					group.putPath(output);
				}
			}
			else
			{
				group.putPath(output);
			}
		}

		for (TextW filepath : inputs)
		{
			TSZW fullPath = resolve(filepath);

			TextW filename;
			splitFileName(filepath, nullptr, &filename);

			cmdline.cut_self(cmdEndIdx);
			cmdline << fullPath;
			tlogCommand.reset(fullPath).put(cmdline);

			if (outputIsDirectory)
			{
				output.cut_self(endIdx);
				output << filename;
			}

			res = compile(output, filepath, fullPath);
			if (res != 0) break;
		}
		break;
	}
	case ConfigurationType::StaticLibrary:
	{
		makeCmdLine(argn, args);
		res = lib(output, inputs);
		break;
	}
	case ConfigurationType::Executable:
	{
		makeCmdLine(argn, args);
		res = link(output, inputs);
		break;
	}
	default:
		wcerr << L"Unknown Build State\n";
		res = EINVAL;
		break;
	}
	if (tlogEnable)
	{
		tlogCommand.save();
		tlogRead.save();
		tlogWrite.save();
	}
	return res;
}

dword compile(TextW output, TextW filepath, TSZW & fullPath) noexcept
{
	TSZW fullOutputPath = resolve(output);
	TSZW depfile;
	depfile << fullOutputPath << L".d";
	
	bool modified;
	try
	{
		modified = checkSourceModified(output, fullPath);
	}
	catch (FileException&)
	{
		return ERROR_FILE_NOT_FOUND;
	}

	if (!modified)
	{
		wcout << L"Skip " << filepath << L'\n';
		wcout.flush();
	}
	else
	{
		if (logOption == LogOption::FileName)
		{
			wcout << filepath << L'\n';
			wcout.flush();
		}

		Process process;
		{
			TSZW command;
			command << L"emcc";
			command << ex_arguments;
			if (fullPath.endsWith(L".js"))
				command << L" --js-library \"" << fullPath << L'\"';
			else
				command << L" \"" << fullPath << L'\"';

			command << L" -MMD -c -o \"" << fullOutputPath << L".bc\"";
			if (logOption == LogOption::CommandLine)
			{
				wcout << command << L'\n';
				wcout.flush();
			}
			TTextW curdir = currentDirectory;
			process.shell(command, TSZW() << curdir[0] << L":\\"); // Avoide emscripten sourcemap bug
		}

		while (!process.eof())
		{
			TSZ line = process.readTo('\n');
			if (line.empty())
				continue;

			if (line.endsWith("\r"))
				line.cut_self(line.endIndex() - 1);

			{

				Text firstColon = line.find(':');
				if (firstColon == nullptr)
					goto _skip;

				Text filePath;
				bool includePath;
				if (line.subarr(0, 22) == "In file included from ")
				{
					filePath = line + 22;
					includePath = true;
				}
				else
				{
					filePath = line;
					includePath = false;
				}

				if (firstColon - filePath <= 1)
				{
					firstColon = (firstColon + 1).find(':');
					if (firstColon == nullptr)
						goto _skip;
				}

				Text secondColumn = (firstColon + 1).find(':');
				if (secondColumn == nullptr)
					goto _skip;

				Text lineNumber = (firstColon + 1).cut(secondColumn);
				if (!lineNumber.numberonly())
					goto _skip;

				Text message;
				if (includePath)
					message = ": included";
				else
				{
					message = (secondColumn + 1).find(':');
					if (message == nullptr)
						message = "";
				}

				cout << filePath.cut(firstColon) << '(' << lineNumber << ')' << message << '\n';
				cout.flush();
				continue;
			}
		_skip:
			cout << line << '\n';
			cout.flush();
		}

		dword exitCode = process.getExitCode();
		if (exitCode != S_OK)
		{
			File::remove(depfile);
			return exitCode;
		}
	}

	if (tlogEnable)
	{
		TLogGroup group = tlogRead.reset(fullPath);

		try
		{
			Must<File> file = File::open(depfile);
			readDependency(file, [&](Text dependfile) {
				group.putPath(TSZW() << (Utf8ToWide)(dependfile));
				return false;
			});
		}
		catch (FileException&)
		{
			wcerr << depfile << L" not found\n";
		}
	}

	return S_OK;
}
dword lib(ATextW &output, WRefArray<ATextW> inputs) noexcept
{
	if (tlogEnable)
	{
		loadTLog(L"lib", L"Lib-link", L"Lib-link");
		tlogCommand.reset(inputs).put(cmdline);
		TLogGroup group = tlogWrite.reset(inputs);
		group.put(resolve(output));
		group = tlogRead.reset(inputs);
		group.putPath(inputs);
	}

	filetime_t destTime;
	bool exists;
	try
	{
		{
			Must<File> check = File::open(output.c_str());
			destTime = check->getLastModifiedTime();
		}
		exists = true;
	}
	catch (FileException&)
	{
		destTime = 0;
		exists = false;
	}

	for (ATextW &input : inputs)
	{
		filetime_t srcTime;
		try
		{
			if (exists)
			{
				Must<File> file = File::open(input.c_str());
				srcTime = file->getLastModifiedTime();
				if (srcTime > destTime)
				{
					File::remove(TSZW() << output);
					exists = false;
				}
			}
		}
		catch (FileException&)
		{
			return ERROR_FILE_NOT_FOUND;
		}
	}

	if (exists)
	{
		wcout << L"Skip linking\n";
		wcout.flush();
		return 0;
	}

	Process process;

	{
		TSZW command;
		command << L"emcc";
		command << ex_arguments;

		for (ATextW &input : inputs)
		{
			if (input.endsWith(L".js"))
				command << L" --js-library \"" << input << L'\"';
			else
				command << L" \"" << input << L"\"";
		}

		command << L" -o \"" << output << L"\"";

		wcout << L"Linking...\n";
		wcout.flush();

		if (logOption == LogOption::CommandLine)
		{
			wcout << command << L'\n';
			wcout.flush();
		}
		process.shell(command);
	}

	while (!process.eof())
	{
		TSZ line = process.readTo('\n');

		if (line.empty()) continue;

		if (line.endsWith("\r"))
			line.cut_self(line.endIndex() - 1);

		cout << line << '\n';
		cout.flush();
	}
	return process.getExitCode();
}
dword link(ATextW &output, WRefArray<ATextW> inputs) noexcept
{
	bool isExportDefExists = File::isFile(L"exports.def");
	bool isAssetExists = File::isDirectory(L"asset");

	ATextW outputjs;
	TextW outputExt = output.find_r('.');
	if (outputExt == nullptr) outputExt = output.endIndex();
	TextW outputName = output.cut(outputExt);
	outputjs << outputName << L".js";

	if (tlogEnable)
	{
		loadTLog(L"link", L"link", L"link");
		tlogCommand.reset(inputs).put(cmdline);

		TLogGroup group = tlogWrite.reset(inputs);
		TSZW outputFull = resolve(outputName);
		pcwstr extptr = outputFull.end();
		outputFull << outputExt;
		group.put(outputFull);
		outputFull.cut_self(extptr);
		outputFull << L".js";
		group.put(outputFull);
		outputFull << L".map";
		group.put(outputFull);

		group = tlogRead.reset(inputs);
		group.putPath(inputs);
	}

	if (!checkExeModified(outputjs, inputs, isExportDefExists, isAssetExists))
	{
		wcout << L"Skip linking\n";
		wcout.flush();
		return 0;
	}
	
	if (logOption == LogOption::FileName)
	{
		wcout << L"Linking...\n";
		wcout.flush();
	}

	Process process;

	{
		TSZW command;
		command << L"emcc";
		command << ex_arguments;

		for (TextW input : inputs)
		{
			if (input.endsWith(L".js"))
				command << L" --js-library \"" << input << L'\"';
			else
				command << L" \"" << input << L"\"";
		}

		if (isExportDefExists)
		{
			command << L" -s EXPORTED_FUNCTIONS=\"";
			command << (AcpToWide)File::openAsArrayTemp<char>(L"exports.def");
			command << '\"';
		}
		if (isAssetExists)
		{
			command << L" --embed-file asset";
		}

		command << L" -o \"" << outputjs << L"\"";
		if (logOption == LogOption::CommandLine)
		{
			wcout << command << L'\n';
			wcout.flush();
		}
		process.shell(command);
	}

	while (!process.eof())
	{
		TSZ line = process.readTo('\n');

		if (line.empty()) continue;

		if (line.endsWith("\r"))
			line.cut_self(line.endIndex() - 1);

		cout << line << '\n';
		cout.flush();
	}

	int exitCode = process.getExitCode();
	if (exitCode == 0)
	{
		TextW compilerdir, outdir;
		ModuleName<wchar> exepath;
		exepath.change(L'\\', L'/');
		splitFileName(exepath, &compilerdir, nullptr);
		splitFileName(output, &outdir, nullptr);

		try
		{
			io::FOStream<char> fos = File::create(output.c_str());
			TemplateWriter<io::FOStream<char>> writer(&fos, "{{", "}}");
			writer.put("script", TSZ() << (WideToUtf8)outputjs.subarr(outputjs.pos_r(L'/') + 1));

			MappedFile srcfile(TSZW() << compilerdir << L"/template.html");
			writer.write(srcfile.cast<char>());
		}
		catch (FileException&)
		{
			cerr << "Cannot generate html" << endl;
		}
		try
		{
			io::FOStream<char> fos = File::create(TSZW() << outdir << L"/package.json");
			TemplateWriter<io::FOStream<char>> writer(&fos, "{{", "}}");
			writer.put("entry", TSZ() << (WideToUtf8)output.subarr(output.pos_r(L'/') + 1));

			MappedFile srcfile(TSZW() << compilerdir << L"/package.json");
			writer.write(srcfile.cast<char>());
		}
		catch (FileException&)
		{
			cerr << "Cannot generate package.json" << endl;
		}
	}

	return exitCode;
}

void makeCmdLine(int argn, wchar_t ** args) noexcept
{
	wchar_t ** arg = args + 1;
	wchar_t ** arge = args + argn;
	for (; arg != arge; arg++)
	{
		cmdline << *arg << L' ';
	}
	cmdline.resize(cmdline.size() - 1);
}
void loadTLog(TextW commandTag, TextW readTag, TextW writeTag) noexcept
{
	tlogCommand.load(TSZW() << tlogDirectory << commandTag << L".command.1.tlog");
	tlogRead.load(TSZW() << tlogDirectory << readTag << L".read.1.tlog");
	tlogWrite.load(TSZW() << tlogDirectory << writeTag << L".write.1.tlog");
}
bool checkSourceModified(TextW output, TSZW & fullPath)
{
	filetime_t srcTime = File::getLastModifiedTime(fullPath);
	filetime_t destTime;
	try
	{
		TSZW deppath;
		deppath << output << L".d";
		Must<File> file = File::open(deppath);
		destTime = file->getLastModifiedTime();
		if (srcTime <= destTime)
		{
			if (!readDependency(file, [&](Text checkfile) {
				return File::getLastModifiedTime(TSZW() << (Utf8ToWide)checkfile) > destTime;
			}))
				return false;
		}
		File::remove(deppath);
	}
	catch (FileException&)
	{
	}
	return true;
}
bool checkExeModified(ATextW &outputjs, WRefArray<ATextW> inputs, bool exportDefExists, bool assetExists) noexcept
{
	filetime_t outfiletime = File::getLastModifiedTime(outputjs.c_str());
	for (ATextW & js : javascripts)
	{
		if (File::getLastModifiedTime(js.c_str()) > outfiletime)
			return true;
	}

	for (ATextW & input : inputs)
	{
		if (File::getLastModifiedTime(input.c_str()) > outfiletime)
			return true;
	}

	if (exportDefExists && File::getLastModifiedTime(L"exports.def") > outfiletime)
		return true;

	if (assetExists && File::isDirectoryModified(L"asset", outfiletime))
		return true;

	return false;
}
template <typename LAMBDA>
bool readDependency(File * file, const LAMBDA & lambda)
{
	try
	{
		io::FIStream<char, false> fis = file;
		fis.skipto(": ");

		Text line = fis.readLine();

		auto getFile = [&] {
			for (;;)
			{
				line.readto_ny(Text::WHITE_SPACE);
				if (line.empty())
					throw EofException();
				if (line == "\\")
				{
					line = fis.readLine();
					continue;
				}

				Text checkfile = line.readto_ye(Text::WHITE_SPACE);
				line.readto_ny(Text::WHITE_SPACE);
				return checkfile;
			}
		};

		// ignore first file
		getFile();

		for (;;)
		{
			if (lambda(getFile()))
				return true;
		}
	}
	catch (EofException&)
	{
		return false;
	}
}
