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

set EXEBASE=sensor_plot
set CONFIG=Debug

if not exist %EXEBASE%.exe goto Development
%EXEBASE%.exe %*
goto Exit

: Development

set DEVEXE=%CONFIG%\%EXEBASE%.exe

rem Try VS6
if not exist ..\%DEVEXE% goto Try_VS2003
echo Using %DEVEXE% from Visual Studio 6
..\%DEVEXE% %*
goto Exit

: Try_VS2003
rem Try Visual Studio 2003
if not exist ..\vs2003\%DEVEXE% goto Try_VS2005
echo Using %DEVEXE% from Visual Studio 2003
..\vs2003\%DEVEXE% %*
goto Exit

: Try_VS2005
rem Try Visual Studio 2005
if not exist ..\vs2005\%DEVEXE% goto Error
echo Using %DEVEXE% from Visual Studio 2005
..\vs2005\%DEVEXE% %*
goto Exit

: Error
echo Could not find %EXEBASE% executable!

: Exit
endlocal