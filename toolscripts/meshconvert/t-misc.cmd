@echo off
call startup.cmd %0

echo.
echo -nologo -o %OUTTESTDIR%\cup.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -o %OUTTESTDIR%\cup.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -n -o %OUTTESTDIR%\cupn.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -n -o %OUTTESTDIR%\cupn.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -na -o %OUTTESTDIR%\cupna.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -na -o %OUTTESTDIR%\cupna.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ne -o %OUTTESTDIR%\cupne.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -ne -o %OUTTESTDIR%\cupne.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -cw -o %OUTTESTDIR%\cupcw.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -cw -o %OUTTESTDIR%\cupcw.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -ib32 -o %OUTTESTDIR%\cupib32.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -ib32 -o %OUTTESTDIR%\cupib32.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -flip -o %OUTTESTDIR%\cupflip.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -flip -o %OUTTESTDIR%\cupflip.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )

echo.
echo -nologo -n -fn r11g11b10 -o %OUTTESTDIR%\cupfn.sdkmesh %REFDIR%\cup._obj
"%RUN%" -nologo -n -fn r11g11b10 -o %OUTTESTDIR%\cupfn.sdkmesh %REFDIR%\cup._obj
if %ERRORLEVEL% NEQ 0 ( call failure.cmd "Failed" )
