@echo off

for /f "tokens=*" %%i in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do set MSBUILD=%%i
 
"%MSBUILD%" ../CSE-16-ASM.sln /p:Configuration=Release
 
pause
 