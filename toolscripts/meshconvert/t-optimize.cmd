@echo off
call startup.cmd %0

echo.
echo -nologo -c -op -o %OUTTESTDIR%\cup.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -c -op -o %OUTTESTDIR%\cup.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -c -oplru -o %OUTTESTDIR%\cuplru.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -c -oplru -o %OUTTESTDIR%\cuplru.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -c -op -o %OUTTESTDIR%\Head_Big_Ears.sdkmesh %REFDIR%\Head_Big_Ears._obj
"%RUN%" -nologo -c -op -o %OUTTESTDIR%\Head_Big_Ears.sdkmesh %REFDIR%\Head_Big_Ears._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -c -oplru -o %OUTTESTDIR%\Head_Big_Earslru.sdkmesh %REFDIR%\Head_Big_Ears._obj
"%RUN%" -nologo -c -oplru -o %OUTTESTDIR%\Head_Big_Earslru.sdkmesh %REFDIR%\Head_Big_Ears._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -c -oplru -wf -o %OUTTESTDIR%\Head_Big_Earslru._obj %REFDIR%\Head_Big_Ears._obj
"%RUN%" -nologo -c -oplru -wf -o %OUTTESTDIR%\Head_Big_Earslru._obj %REFDIR%\Head_Big_Ears._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

