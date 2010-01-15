:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: Contact: Nokia Corporation (qt-info@nokia.com)
::
:: This file is part of the tools applications of the Qt Toolkit.
::
:: $QT_BEGIN_LICENSE:LGPL$
:: No Commercial Usage
:: This file contains pre-release code and may not be distributed.
:: You may use this file in accordance with the terms and conditions
:: contained in the Technology Preview License Agreement accompanying
:: this package.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 as published by the Free Software
:: Foundation and appearing in the file LICENSE.LGPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU Lesser General Public License version 2.1 requirements
:: will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: In addition, as a special exception, Nokia gives you certain additional
:: rights.  These rights are described in the Nokia Qt LGPL Exception
:: version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
::
:: If you have questions regarding the use of this file, please contact
:: Nokia at qt-info@nokia.com.
::
::
::
::
::
::
::
::
:: $QT_END_LICENSE$
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
call :%1 %2
goto END

:dest
  set IWMAKE_OUTDIR=%IWMAKE_ROOT%\%~1
  if not exist %IWMAKE_OUTDIR% mkdir %IWMAKE_OUTDIR%
goto :eof

:src
  set IWMAKE_SRCDIR=%IWMAKE_ROOT%\%~1
  if not exist %IWMAKE_SRCDIR% mkdir %IWMAKE_SRCDIR%
goto :eof

:destAbs
  set IWMAKE_OUTDIR=%1
  if not exist %IWMAKE_OUTDIR% mkdir %IWMAKE_OUTDIR%
goto :eof

:srcAbs
  set IWMAKE_SRCDIR=%1
  if not exist %IWMAKE_SRCDIR% mkdir %IWMAKE_SRCDIR%
goto :eof

:extsync
  if exist %IWMAKE_ROOT%\%~1 rd /S /Q %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
  xcopy /H /Y /Q /I /R /E %IWMAKE_EXTERNAL%\%~1 %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
goto :eof

:all
  xcopy /H /Y /Q /I /R /S %IWMAKE_SRCDIR%\%~1 %IWMAKE_OUTDIR%\ >> %IWMAKE_LOGFILE%
goto :eof

:filesEx
  for /F "tokens=1,2*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n& set IWMAKE_TMP3=%%~o
  echo %IWMAKE_TMP3% > %IWMAKE_ROOT%\iwmake_exclude
  xcopy /H /Y /Q /I /R /S /EXCLUDE:%IWMAKE_ROOT%\iwmake_exclude %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
goto :eof

:file
  set IWMAKE_TMP=%~1
  set IWMAKE_TMP2=%~nx1
  echo set IWMAKE_TMP3=%%IWMAKE_TMP:%IWMAKE_TMP2%=%%>"%IWMAKE_ROOT%\iwmake_tmp.bat"
  call %IWMAKE_ROOT%\iwmake_tmp.bat
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%"
  xcopy /H /Y /Q /I /R %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP3% >> %IWMAKE_LOGFILE%
goto :eof

:fileAndRename
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n& set IWMAKE_TMP4=%%~nxn
  echo set IWMAKE_TMP3=%%IWMAKE_TMP2:%IWMAKE_TMP4%=%%>"%IWMAKE_ROOT%\iwmake_tmp.bat"
  call %IWMAKE_ROOT%\iwmake_tmp.bat
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%"
  echo > %IWMAKE_OUTDIR%\%IWMAKE_TMP2%
  xcopy /H /Y /Q /R %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
  set IWMAKE_TMP4=
goto :eof

:files
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP2%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP2%"
  xcopy /H /Y /Q /I /R /S %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
goto :eof

:runtime
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if "%IWMAKE_TMP2%" == "" set IWMAKE_TMP2=bin
  xcopy /H /Y /Q /I /R %SystemRoot%\system32\msvcr%IWMAKE_TMP%.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
  xcopy /H /Y /Q /I /R %SystemRoot%\system32\msvcp%IWMAKE_TMP%.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
goto :eof

:syncqt
  pushd %IWMAKE_OUTDIR%
  if exist "include" rd /S /Q include
  if not "%QTDIR%"=="" set IWMAKE_OLD_QTDIR=%QTDIR%
  set QTDIR=%IWMAKE_OUTDIR%
  "%IWMAKE_PERLPATH%\perl" %QTDIR%\bin\syncqt -copy >> %IWMAKE_LOGFILE% 2>&1
  if not "%IWMAKE_OLD_QTDIR%"=="" set QTDIR=%IWMAKE_OLD_QTDIR%
  popd
goto :eof

:END
