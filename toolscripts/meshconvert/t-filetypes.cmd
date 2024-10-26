@echo off
call startup.cmd %0

echo.
echo -nologo -o %OUTTESTDIR%\cup.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -o %OUTTESTDIR%\cup.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ft sdkmesh -o %OUTTESTDIR%\cup2.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -ft sdkmesh -o %OUTTESTDIR%\cup2.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ft sdkmesh2 -o %OUTTESTDIR%\cupPBR.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -ft sdkmesh2 -o %OUTTESTDIR%\cupPBR.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ft cmo -o %OUTTESTDIR%\cup.cmo %REFDIR%\cup._obj
"%RUN%" -nologo -ft cmo -o %OUTTESTDIR%\cup.cmo %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ft vbo -o %OUTTESTDIR%\cup.vbo %REFDIR%\cup._obj
"%RUN%" -nologo -ft vbo -o %OUTTESTDIR%\cup.vbo %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ft obj -o %OUTTESTDIR%\cup2._obj %REFDIR%\cup._obj
"%RUN%" -nologo -ft obj -o %OUTTESTDIR%\cup2._obj %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )
