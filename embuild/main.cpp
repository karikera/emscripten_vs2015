#include "stdafx.h"
#include "main.h"
#include "tlog.h"

#include <KR3/io/bufferedstream.h>
#include <KR3/io/selfbufferedstream.h>
#include <KR3/util/process.h>
#include <KR3/util/wide.h>
#include <KR3/util/envvar.h>
#include <KR3/wl/windows.h>
#include <KRUtil/text/template.h>
#include <KRUtil/text/slash.h>
#include <KRUtil/fs/path.h>
#include <KRUtil/parameter.h>

#include <sstream>
#include <fstream>

#pragma comment(lib, "KR3.lib")
#pragma comment(lib, "KRUtil.lib")

using namespace kr;

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
	CommandLineEMCC,
	CommandLineEMBuild
};

enum class LanguageType
{
	Auto,
	C,
	CPP
};

namespace
{
	AText16 ex_arguments;
	bool useWasmRead = false;
	Array<AText16> javascripts;
	Path path;
	ConfigurationType confType = ConfigurationType::None;
	LanguageType langType = LanguageType::Auto;
	AText16 langC;
	AText16 langCPP;
	LogOption logOption = LogOption::FileName;
	bool tlogMinTest = false;
	AText16 tlogDirectory;
	bool tlogEnable = false;
	bool noLogo = false;
	bool enableToolBar = false;
	AText16 cmdline;

	AText nwjsName;
	AText nwjsDesc;
	AText nwjsVersion = "0.1.0";
	AText nwjsBugs;
	AText nwjsChromiumArgs;
	Array<AText> nwjsKeywords;

	AText nwjsTitle;
	int nwjsWidth = 0, nwjsHeight = 0;
	bool nwjsToolbar = false;
	AText nwjsIcon;
	AText nwjsPosition;
	bool nwjsResizable = true;
	bool nwjsAlwaysOnTop = false;
	bool nwjsFullscreen = false;
	bool nwjsShowInTaskbar = true;
	bool nwjsFrame = true;
	bool nwjsShow = true;
	bool nwjsKiosk = false;
	bool nwjstransparent = false;

	TLog tlogCommand;
	TLog tlogRead;
	TLog tlogWrite;

	Set<Text16> libpath;
}

dword compile(Text16 output, Text16 filepath, TSZ16 & fullPath) noexcept;
dword lib(AText16 &output, WRefArray<AText16> inputs) noexcept;
dword link(AText16 &output, WRefArray<AText16> inputs) noexcept;
void makeCmdLine() noexcept;
void loadTLog(Text16 commandTag, Text16 readTag, Text16 writeTag) noexcept;
bool checkSourceModified(Text16 output, TSZ16 & fullPath);
bool checkExeModified(AText16 &outputjs, WRefArray<AText16> inputs, bool exportDefExists, bool assetExists) noexcept;
template <typename LAMBDA>
bool readDependency(File * file, const LAMBDA & lambda);

char jsslashrule(char chr) noexcept
{
	switch (chr)
	{
	case '\r': return 'r';
	case '\n': return 'n';
	case '\t': return 't';
	case '\0': return '0';
	case '\"': return '\"';
	case '\\': return '\\';
	default: return '\0';
	}
}

auto jsaddslashes(Text text) noexcept
{
	return addSlashesCustom(text, jsslashrule);
}

class Demangler
{
private:
	Array<AText> m_identities;
	Text m_input;
	Text m_original;

public:
	Demangler(Text input) noexcept
		: m_input(input), m_original(input)
	{
	}

	Text readIdentifier() throws(EofException, InvalidSourceException)
	{
		Text idlen_text = m_input.readto_L([](char chr) { return chr < '0' || chr > '9'; });
		if (idlen_text.empty()) throw InvalidSourceException();
		int idlen = idlen_text.to_uint();
		return m_input.read(idlen);
	}

	AText getType() throws(EofException, InvalidSourceException)
	{
		if (m_input.empty()) return "[]";
		char chr = *m_input++;
		switch (chr)
		{
		case 'v': return "void";
		case 'w': return "wchar_t";
		case 'b': return "bool";
		case 'c': return "char";
		case 'a': return "signed char";
		case 'h': return "unsigned char";
		case 's': return "short";
		case 't': return "unsigned short";
		case 'i': return "int";
		case 'j': return "unsigned int";
		case 'l': return "long";
		case 'm': return "unsigned long";
		case 'x': return "long long";
		case 'y': return "unsigned long long";
		case 'n': return "__int128";
		case 'o': return "unsigned __int128";
		case 'f': return "float";
		case 'd': return "double";
		case 'e': return "long double";
		case 'g': return "__float128";
		case 'z': return "...";
		case 'D':
			chr = *m_input++;
			switch (chr)
			{
			case 'd': return "IEEE_754r_float64";
			case 'e': return "IEEE_754r_float128";
			case 'f': return "IEEE_754r_float32";
			case 'h': return "IEEE_754r_float16";
			case 'F':  {
				AText out;
				out << m_input.readto_L([](char chr) { return chr < '0' || chr > '9'; });
				return out;
			}
			case 'i': return "char32_t";
			case 's': return "char16_t";
			case 'a': return "auto";
			case 'c': return "decltype(auto)";
			case 'n': return "nullptr_t";
			}
			throw InvalidSourceException();
		case 'u':
			return readIdentifier();
		case 'N': // nested
		{
			AText out;
			for (;;)
			{
				chr = *m_input++;
				switch (chr)
				{
				case 'C': // constructor
				{
					chr = *m_input++;
					switch (chr)
					{
					case 'I': {
						out << "::inherited ctor";
						out << '(' << getType() << ')';
						break;
					}
					case '2': out << "::base ctor"; break;
					case '4': out << "::charge ctor"; break;
					case '1': out << "::complete ctor"; break;
					default: throw InvalidSourceException();
					}
					break;
				}
				case 'E': goto _endloop;
				case 'S': // subtitution
				{
					Text idx = m_input.readwith('_');
					size_t nidx = idx.empty() ? 0 : idx.to_uint() + 1;
					out << m_identities[nidx];
					break;
				}
				case 'I': // template
					out << '<';
					out << getType();
					for (;;)
					{
						if (*m_input == 'E') break;
						out << ',';
						out << getType();
					}
					m_input++;
					out << '>';
					break;
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
				{
					m_input--;
					out << "::" << readIdentifier();
					m_identities.push(out);
					break;
				}
				default: throw InvalidSourceException();
				}
			}
		_endloop:
			//_ZN 2kr 3ary 5_pri_ 8WrapImpl I N S0_ 4data 13AllocatedData I c N S_ 5Empty E E E c E C1Ev
			/*
			_ZN
			2kr 3ary 5_pri_ 8WrapImpl I
			N
			S0_ 4data 13AllocatedData I
			c N
			S_ 5Empty
			E
			E
			E c
			E C1
			E v
			*/
			return out;
		}
		case 'R': // reference
		{
			AText out = getType();
			out << '&';
			return out;
		}
		default:
			throw InvalidSourceException();
		}
	}

	AText getFunctionType() noexcept
	{
		AText out;
		try
		{
			if (m_input.startsWith("__"))
			{
				Text retType = m_input.find_r('_');
				Text funcname = m_input.cut(retType);
				m_input = retType;
				out << getType() << ' ' << funcname;
			}
			else if (m_input.startsWith("_Z"))
			{
				m_input += 2;
				AText funcbody = getType();
				out << getType() << ' ' << funcbody;
			}
			out << " [" << m_original << ']';
		}
		catch (EofException&)
		{
			out << " [" << m_original << ']';
		}
		catch (InvalidSourceException&)
		{
			out << m_original.cut(m_input) << " [ERR] " << m_input;
		}
		return out;
	}

};

void readIncludePath() noexcept
{
	TText16 VEM_EXCLUDE = EnviromentVariable16(u"VEM_EXCLUDE");
	VEM_EXCLUDE.change(u'\\', u'/');
	ReferenceSet<Text16> excludeMap;
	for (Text16 exclude : VEM_EXCLUDE.splitIterable(u';'))
	{
		if (exclude.endsWith(u'/')) exclude.cut_self(exclude.end() - 1);
		excludeMap.insert(exclude);
	}

	TText16 INCLUDE = EnviromentVariable16(u"INCLUDE");
	INCLUDE.change(u'\\', u'/');
	for (Text16 include : INCLUDE.reverseSplitIterable(u";"))
	{
		if (include.empty()) continue;
		if (include.endsWith(u'/')) include.cut_self(include.end() - 1);
		if (!excludeMap.insert(include).second) continue;
		ex_arguments << u" -I\"" << path16.resolve(include) << u"\"";
	}
}

void readLibraryPath(Array<AText16> * inputs) noexcept
{
	TText16 LIB = EnviromentVariable16(u"LIB");
	LIB.change(u'\\', u'/');
	for (Text16 include : LIB.reverseSplitIterable(u";"))
	{
		if (include.empty()) continue;
		if (include.endsWith(u'/')) include.cut_self(include.end() - 1);
		libpath.insert(path16.resolve(include));
	}

	for (AText16 & input : *inputs)
	{
		pcstr16 szinput = input.c_str();
		if (File::exists(szinput)) continue;

		for (Text16 lib : libpath)
		{
			TSZ16 joined = path16.join(lib, input);
			if (File::exists(joined))
			{
				input = joined;
				break;
			}
		}
	}
}

int CT_CDECL wmain(int argn, wchar_t ** args, wchar_t** envp)
{
	std::wcout.imbue(std::locale("Korean"));
	
	Text16 exeDir = path16.dirname((Text16)unwide(args[0]));
	
	AText16 output;
	Array<AText16> inputs;

	{
		Parameter16 param = {
			{ ParamValue, u"D" },
			{ ParamValue, u"log" },
			{ ParamValue, u"stdc" },
			{ ParamValue, u"stdcpp" },
			{ ParamNoValue, u"bind" },
			{ ParamValue, u"wasm" },

			{ ParamNoValue, u"safe_heap" }, 
			{ ParamNoValue, u"legacy_gl_emulation" },
			{ ParamNoValue, u"gl_unsafe_opts" },
			{ ParamNoValue, u"gl_ffp_only" },
			{ ParamNoValue, u"enable_exception_catching" },
			{ ParamNoValue, u"demangle_support" },
			{ ParamNoValue, u"full_es2" },
			{ ParamValue, u"use_sdl" },
			{ ParamNoValue, u"fetch" },
			{ ParamNoValue, u"allow_memory_growth" },
			{ ParamNoValue, u"aborting_malloc" },
			{ ParamValue, u"total_memory" },
			{ ParamValue, u"outlining_limit"},
			
			{ ParamValue, u"charset" },
			{ ParamNoValue, u"NOLOGO"},
			{ ParamValue, u"tlog_directory"},
			{ ParamNoValue, u"tlog_mintest"},
			{ ParamNoValue, u"tlog_enable"},

			{ ParamValue, u"nwjs_name" },
			{ ParamValue, u"nwjs_desc" },
			{ ParamValue, u"nwjs_version" },
			{ ParamValue, u"nwjs_keyword" },
			{ ParamValue, u"nwjs_bugs" },
			{ ParamValue, u"nwjs_chromium_args" },
			{ ParamValue, u"nwjs_title" },
			{ ParamValue, u"nwjs_width" },
			{ ParamValue, u"nwjs_height" },
			{ ParamNoValue, u"nwjs_toolbar" },
			{ ParamValue, u"nwjs_icon" },
			{ ParamValue, u"nwjs_position" },
			{ ParamNoValue, u"nwjs_noresizable" },
			{ ParamNoValue, u"nwjs_always_on_top" },
			{ ParamNoValue, u"nwjs_fullscreen" },
			{ ParamNoValue, u"nwjs_hide_in_taskbar" },
			{ ParamNoValue, u"nwjs_noframe" },
			{ ParamNoValue, u"nwjs_hide" },
			{ ParamNoValue, u"nwjs_kiosk" },
			{ ParamNoValue, u"nwjs_transparent" },

			{ ParamPrefix, u"Xo" },
			{ ParamPrefix, u"Lo"},
			{ ParamPrefix, u"Fo"},
			{ ParamPrefix, u"g" },
			{ ParamPrefix, u"O" },
			{ ParamPrefix, u"I"},
			{ ParamValue, u"LIBPATH"},
			{ ParamPrefix, u"W"},

			{ ParamPrefix, u"TC"},
			{ ParamPrefix, u"TP"},
		};
		param.start(argn, unwide(args));
		bool res = param.foreach([&](Text16 name, Text16 value) {
			if (name == u"")
			{
				AText16 newpath = value;
				newpath.change(u'\\', u'/');
				inputs.push(newpath);
			}
			else if (name == u"Xo")
			{
				confType = ConfigurationType::Executable;
				output = value;
			}
			else if (name == u"Lo")
			{
				confType = ConfigurationType::StaticLibrary;
				output = value;
			}
			else if (name == u"Fo")
			{
				confType = ConfigurationType::Object;
				output = value;
			}
			else if (name == u"TC")
			{
				langType = LanguageType::C;
			}
			else if (name == u"TP")
			{
				langType = LanguageType::CPP;
			}
			else if (name == u"log")
			{
				if (value == u"filename")
					logOption = LogOption::FileName;
				else if (value == u"emcc")
					logOption = LogOption::CommandLineEMCC;
				else if (value == u"embuild")
					logOption = LogOption::CommandLineEMBuild;
				else
					ucerr << u"Invalid log option /" << name << u':' << value << u'\n';
			}
			else if (name == u"stdc")
			{
				langC << u" -std=" << value;
			}
			else if (name == u"stdcpp")
			{
				langCPP << u" -std=" << value;
			}
			else if (name == u"safe_heap") ex_arguments << u" -s SAFE_HEAP=1";
			else if (name == u"legacy_gl_emulation") ex_arguments << u" -s LEGACY_GL_EMULATION=1";
			else if (name == u"gl_unsafe_opts") ex_arguments << u" -s GL_UNSAFE_OPTS=1";
			else if (name == u"gl_ffp_only") ex_arguments << u" -s GL_FFP_ONLY=1";
			else if (name == u"enable_exception_catching") ex_arguments << u" -s DISABLE_EXCEPTION_CATCHING=0";
			else if (name == u"demangle_support") ex_arguments << u" -s DEMANGLE_SUPPORT=1";
			else if (name == u"full_es2") ex_arguments << u" -s FULL_ES2=1";
			else if (name == u"fetch") ex_arguments << u" -s FETCH=1";
			else if (name == u"use_sdl") ex_arguments << u" -s USE_SDL=" << value;
			else if (name == u"allow_memory_growth") ex_arguments << u" -s ALLOW_MEMORY_GROWTH=1";
			else if (name == u"aborting_malloc") ex_arguments << u" -s ABORTING_MALLOC=1";
			else if (name == u"total_memory") ex_arguments << u" -s TOTAL_MEMORY=" << value;
			else if (name == u"outlining_limit") ex_arguments << u" -s OUTLINING_LIMIT=" << value;
			else if (name == u"bind") ex_arguments << u" --bind";
			else if (name == u"wasm")
			{
				ex_arguments << u" -s WASM=1 -s \"BINARYEN_METHOD='" << value << u"'\"";
				if (value != u"native-wasm") useWasmRead = true;
			}
			else if (name == u"g")
			{
				if (value == u"0" || value == u"1" || value == u"2" || value == u"3" || value == u"4")
					ex_arguments << u" -g" << value;
				else
					ucerr << u"Invalid debug infomation option /" << name << value << u'\n';
			}
			else if (name == u"D")
				ex_arguments << u" -D" << value;
			else if (name == u"I")
			{
				ex_arguments << u" -I\"" << path16.resolve(value) << u"\"";
			}
			else if (name == u"LIBPATH")
			{
				libpath.insert(path16.resolve(value));
			}
			else if (name == u"W")
			{
				ex_arguments << u" -W" << value;
			}
			else if (name == u"charset")
				ex_arguments << u" -finput-charset=" << value;
			else if (name == u"O")
			{
				if (value == u"0" || value == u"1" || value == u"2" || value == u"3" || value == u"s" || value == u"z")
					ex_arguments << u" -"<< name << value;
				else
					ucerr << u"Invalid optimization option /" << name << value << u'\n';
			}
			else if (name == u"NOLOGO") noLogo = true;
			else if (name == u"tlog_directory") tlogDirectory = value;
			else if (name == u"tlog_mintest") tlogMinTest = true;
			else if (name == u"tlog_enable") tlogEnable = true;
			else if (name == u"nwjs_name") nwjsName = toUtf8(value);
			else if (name == u"nwjs_desc") nwjsDesc = toUtf8(value);
			else if (name == u"nwjs_version") nwjsVersion = toUtf8(value);
			else if (name == u"nwjs_keyword") nwjsKeywords.push((AText)toUtf8(value));
			else if (name == u"nwjs_bugs") nwjsBugs = toUtf8(value);
			else if (name == u"nwjs_chromium_args") nwjsChromiumArgs = toUtf8(value);
			else if (name == u"nwjs_title") nwjsTitle = toUtf8(value);
			else if (name == u"nwjs_width")
			{
				if (value.empty()) return;
				if (!value.numberonly()) return;
				nwjsWidth = value.to_uint();
			}
			else if (name == u"nwjs_height")
			{
				if (value.empty()) return;
				if (!value.numberonly()) return;
				nwjsHeight = value.to_uint();
			}
			else if (name == u"nwjs_toolbar") nwjsToolbar = true;
			else if (name == u"nwjs_icon") nwjsIcon = toUtf8(value);
			else if (name == u"nwjs_position") nwjsPosition = toUtf8(value);
			else if (name == u"nwjs_noresizable") nwjsResizable = false;
			else if (name == u"nwjs_always_on_top") nwjsAlwaysOnTop = true;
			else if (name == u"nwjs_fullscreen") nwjsFullscreen = true;
			else if (name == u"nwjs_hide_in_taskbar") nwjsShowInTaskbar = false;
			else if (name == u"nwjs_noframe") nwjsFrame = false;
			else if (name == u"nwjs_hide") nwjsShow = false;
			else if (name == u"nwjs_kiosk") nwjsKiosk = true;
			else if (name == u"nwjs_transparent") nwjstransparent = true;
		});
		if(!res)
			return EINVAL;
	}

	if (logOption == LogOption::CommandLineEMBuild)
	{
		for (wchar_t * arg : View<wchar_t*>(args, argn))
		{
			ucout << unwide(arg) << u' ';
		}
		ucout << u'\n';
		ucout.flush();
	}

	if(!noLogo) ucout << u"emscripten_vs2015 beta" << endl;
	if (tlogEnable)
	{
		File::createFullDirectory(tlogDirectory);
	}

	dword res;
	output.change(u'\\', u'/');
	switch (confType)
	{
	case ConfigurationType::Object:
	{
		{
			wchar_t ** arg = args + 1;
			wchar_t ** arge = args + argn;
			for (; arg != arge; arg++)
			{
				Text16 argtx = (Text16)unwide(*arg);
				for (Text16 input : inputs)
				{
					if (argtx != input) continue;
					goto _ignore;
				}
				cmdline << argtx << u' ';
			_ignore:;
			}
		}

		readIncludePath();

		cmdline << u' ';
		size_t cmdEndIdx = cmdline.size();
		size_t endIdx = output.size();
		bool outputIsDirectory = path16.endsWithSeperator(output);

		if (tlogEnable)
		{
			loadTLog(u"CL", u"CL", u"CL");

			TLogGroup group = tlogWrite.reset(inputs);

			if (outputIsDirectory)
			{
				for (Text16 filepath : inputs)
				{
					output.cut_self(endIdx);
					output << path16.basename(filepath) << u".bc";
					group.putPath(output);
					output.resize(output.size() - 3);
					output << u".d";
					group.putPath(output);
				}
			}
			else
			{
				group.putPath(output);
			}
		}

		for (Text16 filepath : inputs)
		{
			TSZ16 fullPath = path16.resolve(filepath);
			fullPath << nullterm;

			Text16 filename = path16.basename(filepath);

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
		//_ZN 2kr 3ary 5_pri_ 8WrapImpl I N S0_ 4data 13AllocatedData I c N S_ 5Empty E E E c E C1Ev
		//I N S0_ 4data 13AllocatedData I c N S_ 5Empty EEEcEC1Ev
		break;
	}
	case ConfigurationType::StaticLibrary:
	{
		readLibraryPath(&inputs);
		makeCmdLine();
		inputs.removeMatchAllL([](Text16 input) { return input.endsWith_i(u".lib"); });
		inputs.removeMatchAllL([](Text16 input) { return input.endsWith_i(u".dll"); });
		res = lib(output, inputs);
		break;
	}
	case ConfigurationType::Executable:
	{
		readLibraryPath(&inputs);
		makeCmdLine();
		inputs.removeMatchAllL([](Text16 input) { return input.endsWith_i(u".lib"); });
		inputs.removeMatchAllL([](Text16 input) { return input.endsWith_i(u".dll"); });
		res = link(output, inputs);
		break;
	}
	default:
		ucerr << u"Unknown Build State\n";
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

dword compile(Text16 output, Text16 filepath, TSZ16 & fullPath) noexcept
{
	LanguageType thisLangType;

	{
		AText16 ext = path16.extname(output);
		ext.toUpperCase();

		if (ext == u".C")
		{
			if (langType == LanguageType::Auto)
			{
				thisLangType = LanguageType::C;
			}
		}
		else if (ext == u".CXX" || ext == u".CPP")
		{
			if (langType == LanguageType::Auto)
			{
				thisLangType = LanguageType::CPP;
			}
		}
		else
		{
			ucerr << u"invalid file ext: " << ext << u"(" << filepath << u")\n";
			ucerr.flush();
			return 0;
		}
		if (langType != LanguageType::Auto)
		{
			thisLangType = langType;
		}
	}


	TSZ16 fullOutputPath = path16.resolve(output);

	TSZ16 depfile;
	depfile << fullOutputPath << u".d" << nullterm;
	
	bool modified;
	try
	{
		modified = checkSourceModified(output, fullPath);
	}
	catch (Error&)
	{
		return ENOENT;
	}


	if (!modified)
	{
		ucout << u"Skip " << filepath << u'\n';
		ucout.flush();
	}
	else
	{
		if (logOption == LogOption::FileName)
		{
			ucout << filepath << u'\n';
			ucout.flush();
		}

		Process process;
		{
			TSZ16 command;
			command << u"emcc";
			command << ex_arguments;

			switch (thisLangType)
			{
			case LanguageType::C:
				command << langC;
				break;
			case LanguageType::CPP:
				command << langCPP;
				break;
			}

			if (fullPath.endsWith(u".js"))
				command << u" --js-library \"" << fullPath << u'\"';
			else
				command << u" \"" << fullPath << u'\"';

			command << u" -MMD -c -o \"" << fullOutputPath << u".bc\"";
			if (logOption == LogOption::CommandLineEMCC)
			{
				ucout << command << u'\n';
				ucout.flush();
			}
			TText16 curdir = currentDirectory;
			process.shell(command, TSZ16() << curdir[0] << u":\\"); // Avoide emscripten sourcemap bug
		}

		try
		{
			io::BufferedIStream<Process> bis(&process);
			for (;;)
			{
				TSZ line = bis.readLine();
				if (line.empty())
					continue;

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
		}
		catch (EofException&)
		{
		}

		dword exitCode = process.getExitCode();
		if (exitCode != 0)
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
				group.putPath(TSZ16() << (Utf8ToUtf16)(dependfile));
				return false;
			});
		}
		catch (Error&)
		{
			ucerr << depfile << u" not found\n";
		}
	}

	return 0;
}
dword lib(AText16 &output, WRefArray<AText16> inputs) noexcept
{
	if (tlogEnable)
	{
		loadTLog(u"lib", u"Lib-link", u"Lib-link");
		tlogCommand.reset(inputs).put(cmdline);
		TLogGroup group = tlogWrite.reset(inputs);
		group.put(path16.resolve(output));
		group = tlogRead.reset(inputs);
		group.putPath(inputs);
	}

	try
	{
		filetime_t destTime = File::getLastModifiedTime(output.c_str());
		for (AText16 &input : inputs)
		{
			try
			{
				filetime_t srcTime = File::getLastModifiedTime(input.c_str());
				if (srcTime > destTime)
				{
					File::remove(output.c_str());
					goto _continueLinking;
				}
			}
			catch (Error&)
			{
				ucout << u"File not found: " << input << endl;
				return ENOENT;
			}
		}
		ucout << u"Skip linking" << endl;
		ucout.flush();
		return 0;
	}
	catch (Error&)
	{
	}
_continueLinking:

	Process process;

	{
		TSZ16 command;
		command << u"emcc";
		command << ex_arguments;

		for (AText16 &input : inputs)
		{
			if (input.endsWith(u".js"))
				command << u" --js-library \"" << input << u'\"';
			else
				command << u" \"" << input << u"\"";
		}

		command << u" -o \"" << output << u"\"";

		ucout << u"Linking...\n";
		ucout.flush();

		if (logOption == LogOption::CommandLineEMCC)
		{
			ucout << command << u'\n';
			ucout.flush();
		}
		process.shell(command);
	}

	io::BufferedIStream<Process> bis(&process);
	try
	{
		for (;;)
		{
			TSZ line = bis.readLine();
			if (line.empty()) continue;

			cout << line << '\n';
			cout.flush();
		}
	}
	catch (EofException&)
	{
	}
	return process.getExitCode();
}
dword link(AText16 &output, WRefArray<AText16> inputs) noexcept
{
	bool isExportDefExists = File::isFile(u"exports.def");
	bool isAssetExists = File::isDirectory(u"asset");

	AText16 outputjs;
	Text16 outputExt = path16.extname(output);
	Text16 outputName = output.cut(outputExt);
	outputjs << outputName << u".js";

	if (tlogEnable)
	{
		loadTLog(u"link", u"link", u"link");
		tlogCommand.reset(inputs).put(cmdline);

		TLogGroup group = tlogWrite.reset(inputs);
		TSZ16 outputFull = path16.resolve(outputName);
		pcstr16 extptr = outputFull.end();
		outputFull << outputExt;
		group.put(outputFull);
		outputFull.cut_self(extptr);
		outputFull << u".js";
		group.put(outputFull);
		outputFull << u".map";
		group.put(outputFull);

		group = tlogRead.reset(inputs);
		group.putPath(inputs);
	}

	// karikera: Need to check properties or project settings, but maybe visual studio do check
	//if (!checkExeModified(outputjs, inputs, isExportDefExists, isAssetExists))
	//{
	//	ucout << u"Skip linking\n";
	//	ucout.flush();
	//	return 0;
	//}
	
	if (logOption == LogOption::FileName)
	{
		ucout << u"Linking...\n";
		ucout.flush();
	}

	Process process;

	{
		TSZ16 command;
		command << u"emcc";
		command << ex_arguments;

		for (Text16 input : inputs)
		{
			if (input.endsWith(u".js"))
				command << u" --js-library \"" << input << u'\"';
			else
				command << u" \"" << input << u"\"";
		}

		if (isExportDefExists)
		{
			AText exports = File::openAsArray<char>(u"exports.def");
			exports.change('\r', ' ');
			exports.change('\n', ' ');
			exports.change('"', '\'');
			Text reader = exports;
			
			for (;;)
			{
				Text next_equals = reader.readwith('=');
				if (next_equals == nullptr) break;
				Text type = next_equals.trim();
				command << u" -s " << (AcpToUtf16)type << u"=\"";
				command << (AcpToUtf16)reader.readwith_e(';').trim();
				command << u'\"';
			}
		}
		if (isAssetExists)
		{
			command << u" --embed-file asset";
		}

		command << u" -o \"" << outputjs << u"\"";
		if (logOption == LogOption::CommandLineEMCC)
		{
			ucout << command << u'\n';
			ucout.flush();
		}
		process.shell(command);
	}

	static const Text unresolvedSymbolPrefix = "warning: unresolved symbol: ";

	io::BufferedIStream<Process> bis(&process);
	try
	{
		for (;;)
		{
			TSZ line = bis.readLine();

			if (line.empty()) continue;

			if (line.startsWith(unresolvedSymbolPrefix))
			{
				Text symbol = line.subarr(unresolvedSymbolPrefix.size());
				cout << unresolvedSymbolPrefix;
				Demangler mangler(symbol);
				cout << mangler.getFunctionType();
				cout << '\n';
			}
			else
			{
				cout << line << '\n';
			}
			//

			cout.flush();
		}
	}
	catch (EofException&)
	{
	}

	int exitCode = process.getExitCode();
	if (exitCode == 0)
	{
		ModuleName<char16> exepath;
		exepath.change(u'\\', u'/');
		output.c_str();
		Text16 compilerdir = path16.dirname(exepath);
		Text16 outdir = path16.dirname(output);
		Text16 htmlfilename = path16.basename(output);
		Text16 jsfilename = path16.basename(outputjs);
		AText startScript;
		
		try
		{
			io::FOStream<char> fos = File::create(output.data());
			TemplateWriter<io::FOStream<char>> writer(&fos, "{{", "}}");
			writer.put("script", TSZ() << toUtf8(jsfilename));
			if (useWasmRead)
			{
				startScript <<
					"fetch('" << toUtf8(path16.basename(outputName)) << ".wasm').then(v = > v.arrayBuffer())\n"
					".then(v=>{\n"
					"Module.wasmBinary = new Uint8Array(v);\n"
					"start();\n"
					"});\n"
					"}\n";
			}
			else
			{
				startScript << "start();";
			}
			writer.put("start", startScript);
			writer.put("title", nwjsTitle);

			MappedFile srcfile(TSZ16() << compilerdir << u"template.html");
			writer.write(srcfile.cast<char>());
		}
		catch (Error&)
		{
			ErrorCode err = ErrorCode::getLast();
			cerr << "Cannot generate html" << endl;
			cerr << "ERROR: " << err.getMessage<char>() << endl;
		}
		try
		{
			io::FOStream<char> fos = File::create(TSZ16() << outdir << u"package.json");
				
			fos << "{";
			fos << "\n\t\"main\": \"" << toUtf8(htmlfilename) << "\"";
			fos << ",\n\t\"name\": \"" << jsaddslashes(nwjsName) << "\"";
			if (!nwjsDesc.empty()) fos << ",\n\t\"description\": \"" << jsaddslashes(nwjsDesc) << "\"";
			fos << ",\n\t\"version\": \"" << jsaddslashes(nwjsVersion) << "\"";
			if (!nwjsKeywords.empty())
			{
				fos << ",\n\t\"keywords\": [\n";
				fos << "\t\t\"" << jsaddslashes(nwjsKeywords[0]) << "\"";
				for (size_t i=1;i<nwjsKeywords.size();i++)
				{
					fos << ",\n\t\t\"" << jsaddslashes(nwjsKeywords[i]) << "\"";
				}
				fos << "\n\t]";
			}
			if (!nwjsBugs.empty()) fos << ",\n\t\"bugs\": \"" << jsaddslashes(nwjsBugs) << "\"\n";
			if (!nwjsChromiumArgs.empty()) fos << ",\n\t\"chromium-args\": \"" << jsaddslashes(nwjsChromiumArgs) << "\"";
			fos << ",\n\t\"window\": {";
			fos << "\n\t\t\"title\": \"" << jsaddslashes(nwjsTitle) << "\"";
			if (nwjsWidth) fos << ",\n\t\t\"width\": " << nwjsWidth;
			if (nwjsHeight) fos << ",\n\t\t\"height\": " << nwjsHeight;
			if (nwjsToolbar) fos << ",\n\t\t\"toolbar\": true";
			if (!nwjsIcon.empty()) fos << ",\n\t\t\"icon\": \"" << jsaddslashes(nwjsIcon) << "\"";
			if (!nwjsPosition.empty()) fos << ",\n\t\t\"position\": \"" << jsaddslashes(nwjsPosition) << "\"";
			if (!nwjsResizable) fos << ",\n\t\t\"resizable\": false";
			if (nwjsAlwaysOnTop) fos << ",\n\t\t\"alwaysOnTop\": true";
			if (nwjsFullscreen) fos << ",\n\t\t\"fullscreen\": true";
			if (!nwjsShowInTaskbar) fos << ",\n\t\t\"show_in_taskbar\": false";
			if (!nwjsFrame) fos << ",\n\t\t\"frame\": false";
			if (!nwjsShow) fos << ",\n\t\t\"show\": false";
			if (nwjsKiosk) fos << ",\n\t\t\"kiosk\": true";
			if (nwjstransparent) fos << ",\n\t\t\"transparent\": true";
			fos << "\n\t}\n}";
		}
		catch (Error&)
		{
			ErrorCode err = ErrorCode::getLast();
			cerr << "Cannot generate package.json" << endl;
			cerr << "ERROR: " << err.getMessage<char>() << endl;
		}
	}

	return exitCode;
}

void makeCmdLine() noexcept
{
	cmdline = (Text16)unwide(GetCommandLineW());
}
void loadTLog(Text16 commandTag, Text16 readTag, Text16 writeTag) noexcept
{
	tlogCommand.load(TSZ16() << tlogDirectory << commandTag << u".command.1.tlog");
	tlogRead.load(TSZ16() << tlogDirectory << readTag << u".read.1.tlog");
	tlogWrite.load(TSZ16() << tlogDirectory << writeTag << u".write.1.tlog");
}
bool checkSourceModified(Text16 output, TSZ16 & fullPath)
{
	filetime_t srcTime = File::getLastModifiedTime(fullPath);
	filetime_t destTime;
	try
	{
		TSZ16 deppath;
		deppath << output << u".d";
		Must<File> file = File::open(deppath);
		destTime = file->getLastModifiedTime();
		if (srcTime <= destTime)
		{
			if (!readDependency(file, [&](Text checkfile) {
				return File::getLastModifiedTime(TSZ16() << (Utf8ToUtf16)checkfile) > destTime;
			}))
				return false;
		}
		File::remove(deppath);
	}
	catch (Error&)
	{
	}
	return true;
}
bool checkExeModified(AText16 &outputjs, WRefArray<AText16> inputs, bool exportDefExists, bool assetExists) noexcept
{
	filetime_t outfiletime = File::getLastModifiedTime(outputjs.c_str());
	for (AText16 & js : javascripts)
	{
		if (File::getLastModifiedTime(js.c_str()) > outfiletime)
			return true;
	}

	for (AText16 & input : inputs)
	{
		if (File::getLastModifiedTime(input.c_str()) > outfiletime)
			return true;
	}

	if (exportDefExists && File::getLastModifiedTime(u"exports.def") > outfiletime)
		return true;

	if (assetExists && File::isDirectoryModified(u"asset", outfiletime))
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

				Text checkfile = line.readwith_ye(Text::WHITE_SPACE);
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
