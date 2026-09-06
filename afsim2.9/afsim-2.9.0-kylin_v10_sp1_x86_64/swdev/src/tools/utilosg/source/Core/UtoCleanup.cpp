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

#include <algorithm>

#ifdef _WIN32
#include <tchar.h>
#endif

#include "UtoCleanup.hpp"
#include "UtoObjectManager.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"

UtoCleanup* UtoCleanup::m_pMe = nullptr;

UtoCleanup* UtoCleanup::Instance()
{
   if (m_pMe == nullptr)
   {
      m_pMe = new UtoCleanup;
   }
   return m_pMe;
}

UtoCleanup::UtoCleanup()
   : m_bExitCalled(false)
{
}

UtoCleanup::~UtoCleanup() {}

int UtoCleanup::Init()
{
   ::atexit(exit_handler);

   return 0;
}

void UtoCleanup::exit_handler()
{
   if (!m_pMe->m_bExitCalled)
      m_pMe->Exit();
   delete m_pMe;
}

int UtoCleanup::Exit()
{
   UtoObjectManager::Instance()->DestroyAllObjects();

   m_bExitCalled = true;

   return 0;
}
