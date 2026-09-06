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
#include "UtRandom.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptDataPack.hpp"

static constexpr const char* scriptClassName = "TestScriptAccessible";

class TestScriptAccessible : public UtScriptAccessible
{
public:
   TestScriptAccessible(int aId)
      : mId(aId)
   {
   }

   const char* GetScriptClassName() const override { return scriptClassName; }

   friend bool operator==(const TestScriptAccessible& aLhs, const TestScriptAccessible& aRhs)
   {
      return aLhs.mId == aRhs.mId;
   }

private:
   int mId = 0;
};

class TestScriptClass : public UtScriptClass
{
public:
   TestScriptClass(UtScriptTypes* aScriptTypesPtr)
      : UtScriptClass(scriptClassName, aScriptTypesPtr)
   {
      SetClassName(scriptClassName);
      mIsScriptAccessible = true;
   }
};

TEST(UtScriptDataPack, PackAndUnpack)
{
   UtDictionary  dict;
   UtScriptTypes types{dict};
   types.Register(ut::make_unique<TestScriptClass>(&types));
   UtScriptDataPacker scriptArgs{&types};
   ut::Random         rand;

   const bool     bpack = rand.Bernoulli();
   const unsigned upack =
      rand.Uniform<unsigned>(0, std::numeric_limits<int>::max()); // must cap at max int to pack into script data
   const int         ipack = rand.Uniform<int>();
   const double      dpack = rand.Uniform<double>();
   const std::string spack = "stringpack";
   std::vector<int>  vpack(10);
   std::generate(vpack.begin(), vpack.end(), [&rand]() { return rand.Uniform<int>(); });
   const TestScriptAccessible sapack{rand.Uniform<int>()};
   UtScriptDataPack(scriptArgs, bpack, upack, ipack, dpack, spack, vpack, sapack);

   bool                  bunpack;
   unsigned              uunpack;
   int                   iunpack;
   double                dunpack;
   std::string           sunpack;
   std::vector<int>      vunpack;
   TestScriptAccessible* saunpack = nullptr;
   EXPECT_TRUE(UtScriptDataUnpack(scriptArgs.mDataList, bunpack, uunpack, iunpack, dunpack, sunpack, vunpack, saunpack));

   EXPECT_EQ(bpack, bunpack);
   EXPECT_EQ(upack, uunpack);
   EXPECT_EQ(ipack, iunpack);
   EXPECT_EQ(dpack, dunpack);
   EXPECT_EQ(spack, sunpack);
   EXPECT_EQ(vpack, vunpack);
   EXPECT_EQ(sapack, *saunpack);
}
