@echo off
call startup.cmd %0

echo.
echo -nologo -t -o %OUTTESTDIR%\cuptans.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -t -o %OUTTESTDIR%\cuptans.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -tb -o %OUTTESTDIR%\cuptanbin.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -tb -o %OUTTESTDIR%\cuptanbin.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )
