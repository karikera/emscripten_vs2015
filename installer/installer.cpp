#include <KR3/main.h>
#include <KR3/util/process.h>
#include <KR3/util/envvar.h>
#include <KRUtil/fs/installer.h>
#include <KRUtil/fs/file.h>
#include <KRUtil/fs/path.h>
#include <KRUtil/parser/jsonparser.h>
#pragma comment(lib, "KR3.lib")
#pragma comment(lib, "KRUtil.lib")

#include <conio.h>

using namespace kr;

int CT_CDECL main()
{
	try
	{
		{
			ModuleName<char16> moduleName(nullptr);
			moduleName[moduleName.pos_r(u'\\') + 1] = '\0';
			currentDirectory.set(moduleName.data());
		}

		Process vswhere(u"vswhere.exe", u"vswhere.exe -format json");
		JsonParser parser(&vswhere);
		
		parser.array([&](size_t idx) {
			bool skip = false;
			AText path;
			int version = 0;
			parser.object([&](Text field) {
				if (skip)
				{
					parser.skipValue();
					return;
				}
				if (field == "installationVersion")
				{
					TText text = parser.ttext();
					version = ((Text)text).readwith('.').to_int();
					switch (version)
					{
					case 14: break;
					case 15: break;
					case 16: break;
					default: skip = true; break;
					}
				}
				else if (field == "installationPath")
				{
					path = parser.text();
				}
				else
				{
					parser.skipValue();
				}
			});
			if (skip) return;

			switch (version)
			{
			case 14:
			{
				EnviromentVariable16 pf_x86(u"ProgramFiles(x86)");
				EnviromentVariable16 pf(u"ProgramFiles");
				TSZ16 dest;
				if (pf_x86.exists()) dest << pf_x86;
				else dest << pf;
				dest << u"\\MSBuild\\Microsoft.Cpp\\v4.0\\V140\\Platforms\\Emscripten\\";
				kr::Installer(dest, u"Emscripten\\").all();
				dest << u"PlatformToolSets\\v140\\";
				kr::Installer(dest, u"toolset_inner\\").all();
				break;
			}
			case 15:
			{
				TSZ16 tsz;
				tsz << acpToUtf16(path) << u"\\Common7\\IDE\\VC\\VCTargets\\Platforms\\Emscripten\\";
				kr::Installer(tsz, u"Emscripten\\").all();

				size_t restorePoint = tsz.size();
				tsz << u"PlatformToolSets\\v140\\";
				kr::Installer(tsz, u"toolset_inner\\").all();

				tsz.resize(restorePoint);
				tsz << u"PlatformToolSets\\v141\\";
				kr::Installer(tsz, u"toolset_inner\\").all();
				break;
			}
			case 16:
			{
				TSZ16 tsz;
				tsz << acpToUtf16(path) << u"\\MSBuild\\Microsoft\\VC\\v160\\Platforms\\Emscripten\\";
				kr::Installer(tsz, u"Emscripten\\").all();

				tsz << u"PlatformToolSets\\v142\\";
				kr::Installer(tsz, u"toolset_inner\\").all();
				break;
			}
			}
		});
	}
	catch (Error&)
	{
		ucerr << u"Error: " << ErrorCode::getLast().getMessage<char16>() << endl;
	}
	return 0;
}
