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

#ifndef WSFSPACETESTUTILS_HPP
#define WSFSPACETESTUTILS_HPP

#include <string>

#include <gtest/gtest.h>

template<typename ProcessInputType>
void TestBadValueThrow(ProcessInputType& aProcessInputObject, const std::string& aInputString)
{
   UtInput input{};
   input.PushInputString(aInputString);
   EXPECT_THROW(aProcessInputObject.ProcessInput(input), UtInput::BadValue);
}

template<typename ProcessInputType>
void TestEndOfDataThrow(ProcessInputType& aProcessInputObject, const std::string& aInputString)
{
   UtInput input{};
   input.PushInputString(aInputString);
   EXPECT_THROW(aProcessInputObject.ProcessInput(input), UtInput::EndOfData);
}

template<typename ProcessInputType>
void TestFalseReturn(ProcessInputType& aProcessInputObject, const std::string& aInputString)
{
   UtInput input{};
   input.PushInputString(aInputString);
   EXPECT_FALSE(aProcessInputObject.ProcessInput(input));
}

#endif // WSFSPACETESTUTILS_HPP
