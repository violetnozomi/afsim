@echo off
rem ****************************************************************************
rem CUI
rem
rem The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
rem
rem The use, dissemination or disclosure of data in this file is subject to
rem limitation or restriction. See accompanying README and LICENSE for details.
rem ****************************************************************************

REM ****************************************************************************
REM UNCLASSIFIED//FOUO
rem
REM The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
REM
REM Distribution authorized to the Department of Defense and U.S. DoD contractors
REM REL AUS, CAN, UK, NZ. You may not use this file except in compliance with the
REM terms and conditions of 48 C.F.R. 252.204-7000 (Disclosure of Information),
REM 48 C.F.R. 252.227-7025 (Limitations on the Use or Disclosure of Government-
REM Furnished Information Marked with Restrictive Legends), and the AFSIM
REM Memorandum of Understanding or Information Transfer Agreement as applicable.
REM All requests for this software must be referred to the Air Force Research
REM Laboratory Aerospace Systems Directorate, 2130 8th St., Wright-Patterson AFB,
REM OH 45433. This software is provided "as is" without warranties of any kind.
REM
REM This information is furnished on the condition that it will not be released
REM to another nation without specific authority of the Department of the Air Force
REM of the United States, that it will be used for military purposes only, that
REM individual or corporate rights originating in the information, whether patented
REM or not, will be respected, that the recipient will report promptly to the
REM United States any known or suspected compromise, and that the information will
REM be provided substantially the same degree of security afforded it by the
REM Department of Defense of the United States. Also, regardless of any other
REM markings on the document, it will not be downgraded or declassified without
REM written approval from the originating U.S. agency.
REM
REM WARNING - EXPORT CONTROLLED
REM This document contains technical data whose export is restricted by the
REM Arms Export Control Act (Title 22, U.S.C. Sec 2751 et seq.) or the Export
REM Administration Act of 1979, as amended, Title 50 U.S.C., App. 2401 et seq.
REM Violations of these export laws are subject to severe criminal penalties.
REM Disseminate in accordance with provisions of DoD Directive 5230.25.
REM ****************************************************************************

REM ******************************************************************************
REM *** Windows run script
REM ******************************************************************************

setlocal

echo run %*

set APPNAME=mission
set APPROOT=..\..\..\%APPNAME%
set CONFIG=Release

if not exist %APPNAME%.exe goto Development
%APPNAME%.exe %*
goto Exit

: Development

set APPEXE=%CONFIG%\%APPNAME%.exe

REM Try VS6
if not exist %APPROOT%\%APPEXE% goto Try_VS2003
echo Using %APPROOT%\%APPEXE% from Visual Studio 6
%APPROOT%\%APPEXE% %*
goto Exit

: Try_VS2003
REM Try Visual Studio 2003
if not exist %APPROOT%\vs2003\%APPEXE% goto Try_VS2005
echo Using %APPROOT%\vs2003\%APPEXE% from Visual Studio 2003
%APPROOT%\vs2003\%APPEXE% %*
goto Exit

: Try_VS2005
REM Try Visual Studio 2005
if not exist %APPROOT%\vs2005\%APPEXE% goto Error
echo Using %APPROOT%\vs2005\%APPEXE% from Visual Studio 2005
%APPROOT%\vs2005\%APPEXE% %*
goto Exit

: Error
echo Could not find %APPNAME% executable!

: Exit
endlocal

