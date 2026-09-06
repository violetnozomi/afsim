@echo off
rem ****************************************************************************
rem CUI//REL TO USA ONLY
rem
rem The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
rem
rem The use, dissemination or disclosure of data in this file is subject to
rem limitation or restriction. See accompanying README and LICENSE for details.
rem ****************************************************************************
remstart ..\..\..\..\BUILD_panel\iwars_panel_install\iwars_panel.exe -all
..\bin\iwars_exec_win.exe controller_and_target.txt
pause