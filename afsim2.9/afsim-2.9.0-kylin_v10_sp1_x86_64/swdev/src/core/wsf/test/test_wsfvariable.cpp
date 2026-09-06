// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtInputString.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardApplication.hpp"
#include "WsfVariable.hpp"

// This test evaluates the Read, assignment operator, and resolution of references in WsfVariable
TEST(WsfVariable, Equivalence)
{
   WsfStandardApplication app("WsfVariableUnitTestApp", 0, {}, {""});
   WsfScenario            scen(app);
   scen.GetScriptContext()->DeclareVariable("double", "fromScriptVar");                  // create a script_variable
   scen.GetScriptContext()->GetContext().FindGlobalVar("fromScriptVar")->SetDouble(2.7); // set the value

   // TEST 1: a WsfVariable with a value directly applied
   WsfVariable<double> v1;
   UtInput             input1;
   input1.PushInput(ut::make_unique<UtInputString>("1.7"));
   v1.ReadValue(input1);
   EXPECT_FALSE(v1.IsReference()); // does a straight value show as non-reference
   EXPECT_TRUE(v1.Initialize("", nullptr, nullptr, *scen.GetScriptContext()));
   EXPECT_EQ(v1.GetValue(), 1.7); // does a straight value return correctly

   // TEST 2: a WsfVariable with a reference to a script_variable
   WsfVariable<double> v2;
   UtInput             input2;
   input2.PushInput(ut::make_unique<UtInputString>("/variable fromScriptVar"));
   v2.ReadValue(input2);
   EXPECT_TRUE(v2.IsReference()); // does a reference value show as a reference
   EXPECT_TRUE(v2.Initialize("", nullptr, nullptr, *scen.GetScriptContext())); // does it resolve
   EXPECT_EQ(v2.GetValue(), 2.7); // does it resolve to the script_variable's value

   // TEST 3: a WsfVariable with a reference to a script_variable, but explicitly overwritten before resolution
   WsfVariable<double> v3;
   UtInput             input3;
   input3.PushInput(ut::make_unique<UtInputString>("/variable fromScriptVar"));
   v3.ReadValue(input3);
   EXPECT_TRUE(v3.IsReference());                                              // it starts as a reference
   v3 = 3.7;                                                                   // overwrite
   EXPECT_FALSE(v3.IsReference());                                             // it is no longer a reference
   EXPECT_TRUE(v3.Initialize("", nullptr, nullptr, *scen.GetScriptContext())); // it should process fine
   EXPECT_EQ(v3.GetValue(), 3.7);                                              // make sure the overwrite took

   // TEST 4: a WsfVariable with a bad reference
   WsfVariable<double> v4;
   UtInput             input4;
   input4.PushInput(ut::make_unique<UtInputString>("/variable badName"));
   v4.ReadValue(input4);
   EXPECT_FALSE(v4.Initialize("", nullptr, nullptr, *scen.GetScriptContext())); // bad names shouldn't resolve

   // TEST 5: a WsfVariable with a bad reference and a default value
   WsfVariable<double> v5;
   UtInput             input5;
   input5.PushInput(ut::make_unique<UtInputString>("/variable badName /default 4.7"));
   v5.ReadValue(input5);
   EXPECT_TRUE(v5.Initialize("", nullptr, nullptr, *scen.GetScriptContext())); // default value saves it
   EXPECT_EQ(v5.GetValue(), 4.7); // make sure it falls back to the given default
}
