// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "WsfCoverageAccessInterval.hpp"

TEST(WsfCoverageAccessInterval, DefaultAccessInterval)
{
   wsf::coverage::AccessInterval interval{};
   EXPECT_TRUE(interval.GetDuration() < 0.0);
   EXPECT_FALSE(interval.IsValid());
   EXPECT_FALSE(interval.HasStart());
   EXPECT_FALSE(interval.HasEnd());
}

TEST(WsfCoverageAccessInterval, Construction)
{
   constexpr double cSTART{0.0};
   constexpr double cEND{20.0};
   constexpr double cTOLERANCE{1.0e-12};

   wsf::coverage::AccessInterval interval{cSTART, cEND};
   EXPECT_TRUE(interval.IsValid());
   EXPECT_TRUE(interval.HasStart());
   EXPECT_TRUE(interval.HasEnd());
   EXPECT_NEAR(interval.GetDuration(), cEND - cSTART, cTOLERANCE);
   EXPECT_NEAR(interval.GetStart(), cSTART, cTOLERANCE);
   EXPECT_NEAR(interval.GetEnd(), cEND, cTOLERANCE);

   wsf::coverage::AccessInterval other{interval};
   EXPECT_TRUE(other.IsValid());
   EXPECT_TRUE(other.HasStart());
   EXPECT_TRUE(other.HasEnd());
   EXPECT_NEAR(other.GetDuration(), cEND - cSTART, cTOLERANCE);
   EXPECT_NEAR(other.GetStart(), cSTART, cTOLERANCE);
   EXPECT_NEAR(other.GetEnd(), cEND, cTOLERANCE);

   interval.SetStart(cEND);
   EXPECT_TRUE(interval.IsValid());
   EXPECT_TRUE(interval.HasStart());
   EXPECT_TRUE(interval.HasEnd());
   EXPECT_NEAR(interval.GetStart(), cEND, cTOLERANCE);
   EXPECT_NEAR(interval.GetDuration(), 0.0, cTOLERANCE);

   other.SetEnd(cSTART);
   EXPECT_TRUE(other.IsValid());
   EXPECT_TRUE(other.HasStart());
   EXPECT_TRUE(other.HasEnd());
   EXPECT_NEAR(other.GetEnd(), cSTART, cTOLERANCE);
   EXPECT_NEAR(other.GetDuration(), 0.0, cTOLERANCE);
}

TEST(WsfCoverageAccessInterval, StartEndTests)
{
   constexpr double cSTART{0.0};
   constexpr double cEND{20.0};

   wsf::coverage::AccessInterval interval{};
   EXPECT_TRUE(interval.GetDuration() < 0.0);
   EXPECT_FALSE(interval.IsValid());
   EXPECT_FALSE(interval.HasStart());
   EXPECT_FALSE(interval.HasEnd());

   interval.SetStart(cSTART);
   EXPECT_TRUE(interval.GetDuration() < 0.0);
   EXPECT_FALSE(interval.IsValid());
   EXPECT_TRUE(interval.HasStart());
   EXPECT_FALSE(interval.HasEnd());

   interval.SetEnd(cEND);
   EXPECT_TRUE(interval.GetDuration() > 0.0);
   EXPECT_TRUE(interval.IsValid());
   EXPECT_TRUE(interval.HasStart());
   EXPECT_TRUE(interval.HasEnd());

   interval = wsf::coverage::AccessInterval{};
   interval.SetEnd(cEND);
   EXPECT_TRUE(interval.GetDuration() > 0.0);
   EXPECT_TRUE(interval.IsValid());
   EXPECT_FALSE(interval.HasStart());
   EXPECT_TRUE(interval.HasEnd());
}

TEST(WsfCoverageAccessInterval, EqualityOperator)
{
   wsf::coverage::AccessInterval a{0.0, 10.0};
   wsf::coverage::AccessInterval b{0.0, 12.0};
   wsf::coverage::AccessInterval c{1.0, 10.0};
   wsf::coverage::AccessInterval d{0.0, 10.0};

   EXPECT_NE(a, b);
   EXPECT_NE(a, c);
   EXPECT_EQ(a, d);
}

TEST(WsfCoverageAccessInterval, Overlap)
{
   wsf::coverage::AccessInterval a{0.0, 10.0};
   wsf::coverage::AccessInterval b{11.0, 21.0};
   wsf::coverage::AccessInterval c{5.0, 15.0};
   wsf::coverage::AccessInterval d{0.0, 5.0};
   wsf::coverage::AccessInterval e{15.0, 25.0};
   wsf::coverage::AccessInterval f{3.0, 5.0};

   EXPECT_FALSE(a.HasOverlapWith(b));
   EXPECT_FALSE(b.HasOverlapWith(a));
   EXPECT_TRUE(a.HasOverlapWith(c));
   EXPECT_TRUE(c.HasOverlapWith(a));
   EXPECT_TRUE(b.HasOverlapWith(c));
   EXPECT_TRUE(c.HasOverlapWith(b));

   EXPECT_TRUE(c.HasOverlapWith(d));
   EXPECT_TRUE(e.HasOverlapWith(c));
   EXPECT_TRUE(a.HasOverlapWith(f));
   EXPECT_TRUE(f.HasOverlapWith(a));
}

TEST(WsfCoverageAccessInterval, UnionWith)
{
   wsf::coverage::AccessInterval a{0.0, 10.0};
   wsf::coverage::AccessInterval b{11.0, 21.0};
   wsf::coverage::AccessInterval c{5.0, 15.0};
   wsf::coverage::AccessInterval d{0.0, 5.0};
   wsf::coverage::AccessInterval e{15.0, 25.0};

   // aub = 0
   wsf::coverage::AccessInterval auc{0.0, 15.0};
   // aud = a
   // aue = 0
   wsf::coverage::AccessInterval buc{5.0, 21.0};
   // bud = 0
   wsf::coverage::AccessInterval bue{11.0, 25.0};
   wsf::coverage::AccessInterval cud{0.0, 15.0};
   wsf::coverage::AccessInterval cue{5.0, 25.0};
   // due = 0

   EXPECT_FALSE(a.UnionWith(b).IsValid());
   EXPECT_EQ(a.UnionWith(c), auc);
   EXPECT_EQ(a.UnionWith(d), a);
   EXPECT_FALSE(a.UnionWith(e).IsValid());

   EXPECT_EQ(b.UnionWith(c), buc);
   EXPECT_FALSE(b.UnionWith(d).IsValid());
   EXPECT_EQ(b.UnionWith(e), bue);

   EXPECT_EQ(c.UnionWith(d), cud);
   EXPECT_EQ(c.UnionWith(e), cue);

   EXPECT_FALSE(d.UnionWith(e).IsValid());
}

TEST(WsfCoverageAccessInterval, IntersectionWith)
{
   wsf::coverage::AccessInterval a{0.0, 10.0};
   wsf::coverage::AccessInterval b{11.0, 21.0};
   wsf::coverage::AccessInterval c{5.0, 15.0};
   wsf::coverage::AccessInterval d{0.0, 5.0};
   wsf::coverage::AccessInterval e{15.0, 25.0};

   // a ^ b = 0
   wsf::coverage::AccessInterval apc{5.0, 10.0};
   wsf::coverage::AccessInterval apd{0.0, 5.0};
   // a ^ e = 0
   wsf::coverage::AccessInterval bpc{11.0, 15.0};
   // b ^ d = 0
   wsf::coverage::AccessInterval bpe{15.0, 21.0};
   wsf::coverage::AccessInterval cpd{5.0, 5.0};
   wsf::coverage::AccessInterval cpe{15.0, 15.0};
   // d ^ e = 0

   EXPECT_FALSE(a.IntersectionWith(b).IsValid());
   EXPECT_EQ(a.IntersectionWith(c), apc);
   EXPECT_EQ(a.IntersectionWith(d), apd);
   EXPECT_FALSE(a.IntersectionWith(e).IsValid());

   EXPECT_EQ(b.IntersectionWith(c), bpc);
   EXPECT_FALSE(b.IntersectionWith(d).IsValid());
   EXPECT_EQ(b.IntersectionWith(e), bpe);

   EXPECT_EQ(c.IntersectionWith(d), cpd);
   EXPECT_EQ(c.IntersectionWith(e), cpe);

   EXPECT_FALSE(d.IntersectionWith(e).IsValid());
}

TEST(WsfCoverageAccessInterval, Contains)
{
   wsf::coverage::AccessInterval a{0.0, 10.0};

   EXPECT_FALSE(a.Contains(a.GetStart() - 1.0));
   EXPECT_TRUE(a.Contains(a.GetStart()));
   EXPECT_TRUE(a.Contains(a.GetEnd()));
   EXPECT_TRUE(a.Contains((a.GetStart() + a.GetEnd()) / 2.0));
   EXPECT_FALSE(a.Contains(a.GetEnd() + 1.0));
}
