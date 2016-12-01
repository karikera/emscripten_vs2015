@net session >nul 2>&1
@if %errorLevel% neq 0 (
@Elevate\Elevate.exe install.bat
@goto :eof
)
@echo %~dp0
@cd /d %~dp0
@if defined ProgramFiles(x86^) (set "Dest=%ProgramFiles(x86)%"
)else set "Dest=%ProgramFiles%"
@xcopy /SCIY "emscripten" "%Dest%\MSBuild\Microsoft.Cpp\v4.0\V140\Platforms\emscripten"
@echo Install complete
@pause