@echo off
rem ****************************************************************************
rem CUI
rem
rem The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
rem
rem The use, dissemination or disclosure of data in this file is subject to
rem limitation or restriction. See accompanying README and LICENSE for details.
rem ****************************************************************************
rem ******************************************************************************
rem *** Windows run script
rem ******************************************************************************

setlocal

set APPNAME=sage
set APPROOT=..\..\..\%APPNAME%
set CONFIG=Debug

if not exist %APPNAME%.exe goto Development
%APPNAME%.exe %*
goto Exit

: Development

set APPEXE=%CONFIG%\%APPNAME%.exe

rem Try VS6
if not exist %APPROOT%\%APPEXE% goto Try_VS2003
echo Using %APPROOT%\%APPEXE% from Visual Studio 6
%APPROOT%\%APPEXE% %*
goto Exit

: Try_VS2003
rem Try Visual Studio 2003
if not exist %APPROOT%\vs2003\%APPEXE% goto Try_VS2005
echo Using %APPROOT%\vs2003\%APPEXE% from Visual Studio 2003
%APPROOT%\vs2003\%APPEXE% %*
goto Exit

: Try_VS2005
rem Try Visual Studio 2005
if not exist %APPROOT%\vs2005\%APPEXE% goto Error
echo Using %APPROOT%\vs2005\%APPEXE% from Visual Studio 2005
%APPROOT%\vs2005\%APPEXE% %*
goto Exit

: Error
echo Could not find %APPNAME% executable!

: Exit
endlocal

