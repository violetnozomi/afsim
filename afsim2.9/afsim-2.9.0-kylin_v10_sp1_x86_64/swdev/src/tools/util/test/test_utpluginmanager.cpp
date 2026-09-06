// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtPluginManager.hpp"
#include "UtStringUtil.hpp"

TEST(UtPluginManager, CompilerVersionCompatibility)
{
   UtPluginManager manager{UT_PLUGIN_API_MAJOR_VERSION, UT_PLUGIN_API_MINOR_VERSION, UT_PLUGIN_API_COMPILER_STRING};
   std::string     matchVersion = UtPluginCompilerVersionString();
   UtPluginVersion pluginVersion{UT_PLUGIN_API_MAJOR_VERSION, UT_PLUGIN_API_MINOR_VERSION, matchVersion.c_str()};
   auto            pluginCompatible = [&]() { return manager.ValidateCompilerVersion(pluginVersion); };

   // Plugin and manager version match exactly. Expect success.
   EXPECT_TRUE(pluginCompatible());

   // gcc 4.8 does not implement <regex>. When linux compiler support is moved forward, this implementation
   // could be simplified using std::regex_replace
   auto testReplacement = [&](size_t aPos, const std::string& aReplacement, bool aExpectSuccess)
   {
      auto newVersionStringList      = UtStringUtil::Split(matchVersion, '_');
      newVersionStringList[aPos]     = aReplacement;
      auto newVersionString          = UtStringUtil::Join("_", newVersionStringList);
      pluginVersion.mCompilerVersion = newVersionString.c_str();
      EXPECT_EQ(pluginCompatible(), aExpectSuccess);
   };

   // Plugin platform does not match manager. Expect failure.
   testReplacement(0, "wrongplatform", false);

#ifdef _WIN32
   // On windows, only first two digits of compiler version need to match.
   // Note: This test may need to be updated as newer compilers are supported.
#if _MSC_VER >= 1900 && _MSC_VER < 2000
   testReplacement(1, "1800", false);
   testReplacement(1, "2000", false); // hypothetical future version
   for (auto version : {"1900", "1910", "1916", "1920", "1923"})
   {
      testReplacement(1, version, true);
   }
#else
   // Unsupported or future compiler version.
#endif
#elif defined(__linux__)
   testReplacement(1, std::to_string(ut::system::compiler + 100), false);
   testReplacement(1, std::to_string(ut::system::compiler - 100), true);
#endif

   // Plugin word size does not match manager. Expect failure.
   testReplacement(2, "wrongbits", false);

   // Plugin build type does not match manager. Expect failure.
   testReplacement(3, "wrongbuild", false);
}
