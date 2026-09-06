// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtDictionary.hpp"
#include "WsfStandardApplication.hpp"

// Test that a global dictionary can persist across application lifecycles.
TEST(WsfApplication, GlobalDictionaryPersistence)
{
   const int cNumApps = 2;

   UtDictionary* dictBefore = &UtDictionary::GetDictionary();

   // Sequentially, create and destroy apps.
   for (int i = 0; i < cNumApps; ++i)
   {
      {
         // Creating this application in its own scope allowing its
         // destructor to be called once this scope is left.
         WsfStandardApplication app("app", 0, {});
      }
   }

   UtDictionary* dictAfter = &UtDictionary::GetDictionary();

   // Ensure the global dictionary persisted after applications
   // were created and destroyed by checking the memory address
   // of the global dictionary now is the same as the address
   // before creating and destroying applications.
   EXPECT_TRUE(dictBefore == dictAfter);
}
