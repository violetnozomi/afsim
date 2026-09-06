// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/////////////////////////////////////////////////////////////////////////////
// UtoCleanup.hpp : header file

#if !defined(_UTOCLEANUP_HPP_)
#define _UTOCLEANUP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>

/////////////////////////////////////////////////////////////////////////////
// UtoCleanup

class UtoCleanup
{
public:
   static UtoCleanup* Instance();
   int                Init();
   int                Exit();

private:
   UtoCleanup();
   ~UtoCleanup();
   UtoCleanup(const UtoCleanup&);
   UtoCleanup& operator=(const UtoCleanup&);

   static void exit_handler();

   static UtoCleanup* m_pMe;

   bool m_bExitCalled;
};

#endif // !defined(_UTOCLEANUP_HPP_)
