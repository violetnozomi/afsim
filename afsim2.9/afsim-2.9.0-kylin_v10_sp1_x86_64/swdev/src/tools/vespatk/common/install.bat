@echo off
rem ****************************************************************************
rem CUI
rem
rem The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
rem
rem The use, dissemination or disclosure of data in this file is subject to
rem limitation or restriction. See accompanying README and LICENSE for details.
rem ****************************************************************************
rem Installation script to copy to release area

setlocal
pushd %~dp0

call ..\misc\install_setup %*
if errorlevel 1 goto Exit

mkdir %RELETC% 2>nul

REM make sure the script is running in its local directory for these copies
copy  AAA_PROPRIETARY_NOTICE.TXT      %RELROOT%
copy  AAA_RELEASE_NOTES.TXT           %RELROOT%
copy  AAA_README_MSWIN.TXT            %RELROOT%
rem Copy all data files into the application specific etc folder.
copy  beam_parameters.txt             %RELETC%
copy  dis_enums.txt                   %RELETC% 
xcopy /I /Y fonts                     %RELETC%\fonts
REM pop back to previous location, before we changed it

popd

:Exit
endlocal
