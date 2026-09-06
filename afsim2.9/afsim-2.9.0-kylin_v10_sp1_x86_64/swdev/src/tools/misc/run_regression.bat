@echo off
rem ****************************************************************************
rem CUI
rem
rem The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
rem
rem Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
rem
rem The use, dissemination or disclosure of data in this file is subject to
rem limitation or restriction. See accompanying README and LICENSE for details.
rem ****************************************************************************
python regression_test.py -l ..\..\..\demos\regression_tests\mission_list.txt -o ..\..\..\demos\regression_tests\current -x ..\..\..\BUILD_WIN\wsf_install\bin\mission_win_x.exe
