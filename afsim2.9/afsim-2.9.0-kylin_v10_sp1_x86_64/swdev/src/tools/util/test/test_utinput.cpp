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

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLexicalCast.hpp"
#include "UtLonPos.hpp"
#include "UtRandom.hpp"
#include "UtReal.hpp"

namespace
{
UtInput    input;
ut::Random random;

namespace detail
{
template<typename T>
ut::enable_if_t<std::is_integral<T>::value> CompareValues(T& expected, T& actual)
{
   EXPECT_EQ(expected, actual);
}

template<typename T>
ut::enable_if_t<!std::is_integral<T>::value> CompareValues(T& expected, T& actual)
{
   EXPECT_NEAR(expected, actual, 2.0e-5);
}
} // namespace detail

template<typename T, typename RT = T>
void TestReadValue()
{
   T expected = random.Uniform<RT>();
   input.PushInputString(ut::lexical_cast<std::string>(expected));
   T actual;
   input.ReadValue(actual);
   detail::CompareValues(expected, actual);
}

template<typename T, typename U = double>
void TestReadValueOfType(UtInput::ValueType aValueType)
{
   T                  expected = random.Uniform<T>(0, 90);
   U                  temp{static_cast<U>(expected)};
   std::ostringstream oss;
   oss << std::fixed << temp;
   input.PushInputString(oss.str());
   T actual;
   input.ReadValueOfType(actual, aValueType);
   detail::CompareValues(expected, actual);
}
} // namespace

TEST(UtInput, ReadValue)
{
   UtReal::SetDefaultFormat(6);

   TestReadValue<double>();
   TestReadValue<float>();
   TestReadValue<int>();
   TestReadValue<unsigned>();
   TestReadValue<UtLatPos, double>();
   TestReadValue<UtLonPos, double>();
   TestReadValue<UtReal, double>();
}

TEST(UtInput, ReadValueInvalid)
{
   std::string command;
   input.PushInputString("command");
   input.ReadCommand(command);

   double   d;
   float    f;
   int      i;
   unsigned u;
   UtLatPos lat;
   UtLonPos lon;
   UtReal   real;

   // Nothing to read
   EXPECT_THROW(input.ReadValue(d), UtInput::EndOfData);

   // Float out of range
   input.PushInputString(std::to_string(std::numeric_limits<double>::max()));
   EXPECT_THROW(input.ReadValue(f), UtInput::BadValue);

   // Int out of range
   input.PushInputString(std::to_string(std::numeric_limits<unsigned>::max()));
   EXPECT_THROW(input.ReadValue(i), UtInput::BadValue);

   // Float string to integer
   input.PushInputString("10.42");
   EXPECT_THROW(input.ReadValue(i), UtInput::BadValue);
   input.PushInputString("1e4");
   EXPECT_THROW(input.ReadValue(i), UtInput::BadValue);

   // Unsigned out of range
   input.PushInputString(std::to_string(ut::npos));
   ;
   EXPECT_THROW(input.ReadValue(u), UtInput::BadValue);
   input.PushInputString("-1");
   EXPECT_THROW(input.ReadValue(u), UtInput::BadValue);

   // Latitude out of range
   input.PushInputString("91n");
   EXPECT_THROW(input.ReadValue(lat), UtInput::BadValue);

   // Longitude out of range
   input.PushInputString("181w");
   EXPECT_THROW(input.ReadValue(lon), UtInput::BadValue);

   // Invalid real number
   input.PushInputString("abc");
   EXPECT_THROW(input.ReadValue(real), UtInput::BadValue);
}

TEST(UtInput, ReadValueOfType)
{
   UtLength::SetDefaultFormat(UtLength::FmtM + 6);

   TestReadValueOfType<double, UtLatPos>(UtInput::cLATITUDE);
   TestReadValueOfType<double, UtLonPos>(UtInput::cLONGITUDE);
   TestReadValueOfType<double>(UtInput::cNON_DIMENSIONAL);
   TestReadValueOfType<double, UtLength>(UtInput::cLENGTH);

   TestReadValueOfType<float, UtLatPos>(UtInput::cLATITUDE);
   TestReadValueOfType<float, UtLonPos>(UtInput::cLONGITUDE);
   TestReadValueOfType<float>(UtInput::cNON_DIMENSIONAL);
   TestReadValueOfType<float, UtLength>(UtInput::cLENGTH);

   TestReadValueOfType<int, UtLatPos>(UtInput::cLATITUDE);
   TestReadValueOfType<int, UtLonPos>(UtInput::cLONGITUDE);
   TestReadValueOfType<int, int>(UtInput::cNON_DIMENSIONAL);
   TestReadValueOfType<int, UtLength>(UtInput::cLENGTH);
}

TEST(UtInput, ReadValueOfTypeInvalid)
{
   std::string command;
   input.PushInputString("command");
   input.ReadCommand(command);

   double d;
   // Latitude out of range
   input.PushInputString("91n");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cLATITUDE), UtInput::BadValue);
   // Invalid latitude suffix
   input.PushInputString("90e");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cLATITUDE), UtInput::BadValue);

   // Longitude out of range
   input.PushInputString("181w");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cLONGITUDE), UtInput::BadValue);
   // Invalid longitude suffix
   input.PushInputString("180n");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cLONGITUDE), UtInput::BadValue);

   input.PushInputString("non-dimensional");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cNON_DIMENSIONAL), UtInput::BadValue);

   // Missing units
   input.PushInputString("1");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cLENGTH), UtInput::EndOfData);
   // Invalid length units
   input.PushInputString("1 m/s");
   EXPECT_THROW(input.ReadValueOfType(d, UtInput::cLENGTH), UtInput::BadValue);

   // Float out of range
   float f;
   input.PushInputString(std::to_string(std::numeric_limits<double>::max()) + " m");
   EXPECT_THROW(input.ReadValueOfType(f, UtInput::cLENGTH), UtInput::BadValue);

   // Int out of range
   int i;
   input.PushInputString(std::to_string(std::numeric_limits<unsigned>::max()) + " m");
   EXPECT_THROW(input.ReadValueOfType(i, UtInput::cLENGTH), UtInput::BadValue);
}

TEST(UtInput, ReadValueUtCalendar)
{
   UtCalendar epoch;
   input.PushInputString("unk");
   EXPECT_THROW(input.ReadValue(epoch), UtInput::BadValue);

   input.PushInputString("jan 55");
   EXPECT_THROW(input.ReadValue(epoch), UtInput::EndOfData);

   input.PushInputString("jan 55 2020");
   EXPECT_THROW(input.ReadValue(epoch), UtInput::EndOfData);

   input.PushInputString("jan 55 2020 12");
   EXPECT_THROW(input.ReadValue(epoch), UtInput::BadValue);

   input.PushInputString("jan 55 2020 12:12:12.000");
   EXPECT_THROW(input.ReadValue(epoch), UtInput::BadValue);

   input.PushInputString("jan 12 2020 12:90:95.000");
   EXPECT_THROW(input.ReadValue(epoch), UtInput::BadValue);

   input.PushInputString("jan 2 2020 12:13:14.00");
   EXPECT_NO_THROW(input.ReadValue(epoch));

   EXPECT_EQ(epoch.GetMonth(), 1);
   EXPECT_EQ(epoch.GetDay(), 2);
   EXPECT_EQ(epoch.GetYear(), 2020);
   EXPECT_EQ(epoch.GetHour(), 12);
   EXPECT_EQ(epoch.GetMinute(), 13);
   EXPECT_NEAR(epoch.GetSecond(), 14.0, 1.0e-6);
}
