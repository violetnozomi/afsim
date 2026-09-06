// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "WsfPProxyPath.hpp"

TEST(ProxyPathTest, Construction)
{
   WsfPProxyPath testPath;
   ASSERT_TRUE(testPath.Empty());
   ASSERT_EQ(size_t{0}, testPath.MemoryUsage());
   ASSERT_EQ(size_t{0}, testPath.size());
}

TEST(ProxyPathTest, Addition)
{
   WsfPProxyPath testPath;
   ASSERT_TRUE(testPath.Empty());
   testPath += "someString";
   testPath += 1;
   testPath += WsfPProxyKey("keyString");
   testPath += WsfPProxyKey(42);
   ASSERT_EQ(size_t{4}, testPath.size());
   ASSERT_EQ(WsfPProxyKey("someString"), testPath[0]);
   ASSERT_EQ(WsfPProxyKey(1), testPath[1]);
   ASSERT_EQ(WsfPProxyKey("keyString"), testPath[2]);
   ASSERT_EQ(WsfPProxyKey(42), testPath[3]);
}
