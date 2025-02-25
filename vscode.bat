@echo off
SET CurrentWorkingDirectory=%CD%
SET VSDEVCMD_ROOT=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools
CALL "%VSDEVCMD_ROOT%\LaunchDevCmd.bat"
CD /D %CurrentWorkingDirectory%