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
set CONFIG=Release

if not exist %EXEBASE%.exe goto Development
%EXEBASE%.exe %*
goto Exit

: Development

set DEVEXE=%CONFIG%\%EXEBASE%.exe

: Try_VS2005
rem Try Visual Studio 2005
if not exist ..\vs2005\%DEVEXE% goto Try_VS2008
echo Using %DEVEXE% from Visual Studio 2005
..\vs2005\%DEVEXE% %*
goto Exit

: Try_VS2008
rem Try Visual Studio 2008
if not exist ..\vs2008\%DEVEXE% goto Try_VS2010
echo Using %DEVEXE% from Visual Studio 2008
..\vs2008\%DEVEXE% %*
goto Exit

: Try_VS2010
rem Try Visual Studio 2010
if not exist ..\vs2010\%DEVEXE% goto Error
echo Using %DEVEXE% from Visual Studio 2010
..\vs2010\%DEVEXE% %*
goto Exit

: Error
echo Could not find %EXEBASE% executable!

: Exit
endlocal