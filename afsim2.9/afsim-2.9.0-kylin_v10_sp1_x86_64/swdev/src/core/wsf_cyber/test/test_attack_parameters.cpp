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

#include "WsfCyberAttackParameters.hpp"
#include "WsfTrack.hpp"

TEST(AttackParameterObject, Construction)
{
   wsf::cyber::AttackParameterObject object;
   auto&                             auxdata = object.GetAuxData();

   ASSERT_FALSE(auxdata.HasAttributes());
}

TEST(AttackParameterObject, AuxData)
{
   wsf::cyber::AttackParameterObject object1;
   wsf::cyber::AttackParameterObject object2;
   auto&                             auxdata1 = object1.GetAuxData();
   auto&                             auxdata2 = object2.GetAuxData();

   std::string namedEffect1 = "named_effect";
   std::string namedEffect2 = "other_named_effect";
   int         integerData  = 42;
   WsfTrack    trackData;
   trackData.SetTargetName("target");

   auxdata1.Assign(namedEffect1, integerData);
   auxdata2.Assign(namedEffect2, trackData);

   std::vector<std::string> attributeNames;
   object1.GetAuxData().GetAttributeNames(attributeNames);

   ASSERT_EQ(1U, attributeNames.size());
   ASSERT_TRUE(attributeNames.front() == namedEffect1);

   attributeNames.clear();
   object2.GetAuxData().GetAttributeNames(attributeNames);

   ASSERT_EQ(1U, attributeNames.size());
   ASSERT_TRUE(attributeNames.front() == namedEffect2);

   WsfTrack attributeTrack;
   ASSERT_TRUE(object1.GetAuxData().GetAttribute(namedEffect1).GetInt() == integerData);
   object2.GetAuxData().GetAttribute(namedEffect2).Get<WsfTrack>(attributeTrack);
   ASSERT_TRUE(attributeTrack.GetTargetIndex() == trackData.GetTargetIndex());
}

TEST(AttackParameters, Construction)
{
   wsf::cyber::AttackParameters parameters;

   auto effectNames = parameters.GetEffectNames();
   ASSERT_TRUE(effectNames.empty());
}

TEST(AttackParameters, AddParameter)
{
   wsf::cyber::AttackParameterObject object1;
   wsf::cyber::AttackParameterObject object2;
   wsf::cyber::AttackParameterObject object3;
   auto&                             auxdata1 = object1.GetAuxData();
   auto&                             auxdata2 = object2.GetAuxData();
   auto&                             auxdata3 = object3.GetAuxData();

   std::string namedEffect1 = "named_effect";
   std::string namedEffect2 = "other_named_effect";
   int         integerData  = 42;
   WsfTrack    trackData;
   trackData.SetTargetName("target");
   std::string stringData = "effect_string_data";

   auxdata1.Assign(namedEffect1, integerData);
   auxdata2.Assign(namedEffect2, trackData);
   auxdata3.Assign(namedEffect1, stringData);

   wsf::cyber::AttackParameters parameters;

   parameters.AddParameter(object1);
   parameters.AddParameter(object2);
   parameters.AddParameter(object3);

   auto effectNames = parameters.GetEffectNames();
   ASSERT_EQ(2U, effectNames.size());

   auto effectEntries1 = parameters.GetEffectEntries(namedEffect1);
   auto effectEntries2 = parameters.GetEffectEntries(namedEffect2);
   ASSERT_TRUE(effectEntries1);
   ASSERT_TRUE(effectEntries2);
   ASSERT_EQ(2U, effectEntries1->size());
   ASSERT_EQ(1U, effectEntries2->size());
   ASSERT_EQ(2U, parameters.GetEffectEntryCount(namedEffect1));
   ASSERT_EQ(1U, parameters.GetEffectEntryCount(namedEffect2));

   auto parObjPtr = parameters.GetEffectEntryByIndex(namedEffect1, 0);
   ASSERT_TRUE(parObjPtr);
   ASSERT_EQ(42, parObjPtr->GetAuxData().GetAttribute(namedEffect1).GetInt());

   parObjPtr = parameters.GetEffectEntryByIndex(namedEffect1, 1);
   ASSERT_TRUE(parObjPtr);
   ASSERT_EQ("effect_string_data", parObjPtr->GetAuxData().GetAttribute(namedEffect1).GetString());

   parObjPtr = parameters.GetEffectEntryByIndex(namedEffect2, 0);
   ASSERT_TRUE(parObjPtr);
   WsfTrack tempTrack;
   parObjPtr->GetAuxData().GetAttribute(namedEffect2).Get<WsfTrack>(tempTrack);
   ASSERT_EQ("target", tempTrack.GetTargetName());

   parObjPtr = parameters.GetEffectEntryByIndex(namedEffect2, 1);
   ASSERT_FALSE(parObjPtr);
}
