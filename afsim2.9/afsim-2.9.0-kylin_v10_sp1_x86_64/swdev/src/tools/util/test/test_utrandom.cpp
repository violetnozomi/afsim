// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <random>

#include <gtest/gtest.h>

#include "UtRandom.hpp"

TEST(UtRandom, DefaultRepeatability)
{
   constexpr double    comparisons = 10000;
   ut::Random          random{};
   std::vector<double> first;
   for (size_t i = 0; i < comparisons; ++i)
   {
      first.emplace_back(random.Uniform<double>());
   }
   random = ut::Random{};
   for (const auto& value : first)
   {
      ASSERT_EQ(value, random.Uniform<double>());
   }
}

TEST(UtRandom, NonDefaultRepeatability)
{
   constexpr double comparisons = 10000;
   auto             seed        = std::default_random_engine{}();
   ut::Random       random{};
   random.SetSeed(seed);
   std::vector<double> first;
   for (size_t i = 0; i < comparisons; ++i)
   {
      first.emplace_back(random.Uniform<double>());
   }
   random = ut::Random{};
   random.SetSeed(seed);
   for (const auto& value : first)
   {
      ASSERT_EQ(value, random.Uniform<double>());
   }
}

TEST(UtRandom, DefaultMatchesStandard)
{
   ut::Random                             random{};
   std::mt19937                           generator{random.GetSeed()};
   std::uniform_real_distribution<double> realDistribution;
   for (size_t i = 0; i < 10000; ++i)
   {
      ASSERT_EQ(realDistribution(generator), random.Uniform<double>());
   }
}

TEST(UtRandom, NonDefaultMatchesStandard)
{
   auto       randomSeed = std::default_random_engine{}();
   ut::Random random{};
   random.SetSeed(randomSeed);
   std::mt19937                           generator{randomSeed};
   std::uniform_real_distribution<double> realDistribution;
   for (size_t i = 0; i < 10000; ++i)
   {
      ASSERT_EQ(realDistribution(generator), random.Uniform<double>());
   }
}
