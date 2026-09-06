@echo off
rem ****************************************************************************
rem CUI
rem
rem The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
rem
rem The use, dissemination or disclosure of data in this file is subject to
rem limitation or restriction. See accompanying README and LICENSE for details.
rem ****************************************************************************
setlocal
set CME_PATH=c:\cme\win
set CME_SIMNAME=multicast:224.2.25.54:192.168.10:3225
rem %CME_PATH%\conmain -connect %CME_SIMNAME%
rem %CME_PATH%\clouseau -connectTo %CME_SIMNAME% -autostartOff -audioOff -preferenceFile test.prf

start run.bat shooter-rt.txt
start run.bat target-rt.txt

endlocal

