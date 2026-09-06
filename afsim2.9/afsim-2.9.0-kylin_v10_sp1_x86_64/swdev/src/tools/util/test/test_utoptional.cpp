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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "UtOptional.hpp"

struct TestHelper
{
   static void Clear() { ss.str(std::string()); }

   TestHelper()
   {
      ss << "dc"; // Default Construct
   }

   TestHelper(int)
   {
      ss << "vc"; // Value Construct
   }

   explicit TestHelper(const char*)
   {
      ss << "sc"; // String Construct
   }

   TestHelper(const TestHelper&)
   {
      ss << "cc"; // Copy Construct
   }

   TestHelper(TestHelper&&)
   {
      ss << "mc"; // Move Construct
   }

   ~TestHelper()
   {
      ss << "~h"; // ~Helper (destruct)
   }

   TestHelper& operator=(const TestHelper&)
   {
      ss << "ca"; // Copy Assignment
      return *this;
   }

   TestHelper& operator=(TestHelper&&)
   {
      ss << "ma"; // Move Assignment
      return *this;
   }

   void foo()
   {
      ss << "ff"; // Foo
   }

   void foo() const
   {
      ss << "cf"; // Const Foo
   }

   static const std::string str() noexcept { return ss.str(); }

   static std::stringstream ss;
};

std::stringstream TestHelper::ss;

////////////////////////////////////////////////////////////////////
//                                                                //
//                        CONSTRUCTORS                            //
//                                                                //
////////////////////////////////////////////////////////////////////

TEST(UtOptional, DefaultConstruct)
{
   TestHelper::Clear();

   {
      ut::optional<TestHelper> opt;
      EXPECT_FALSE(opt.has_value());
   }

   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, NulloptConstruct)
{
   TestHelper::Clear();

   {
      ut::optional<TestHelper> opt{ut::nullopt};
      EXPECT_FALSE(opt.has_value());
   }

   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, CopyValueConstruct)
{
   TestHelper::Clear();

   TestHelper helper;
   {
      ut::optional<TestHelper> opt{helper};
      EXPECT_TRUE(opt.has_value());
   }

   EXPECT_EQ(TestHelper::str(), "dccc~h");
}

TEST(UtOptional, MoveValueConstruct)
{
   TestHelper::Clear();

   TestHelper helper;
   {
      ut::optional<TestHelper> opt{std::move(helper)};
      EXPECT_TRUE(opt.has_value());
   }

   EXPECT_EQ(TestHelper::str(), "dcmc~h");
}

TEST(UtOptional, OtherValueConstruct)
{
   TestHelper::Clear();

   {
      ut::optional<TestHelper> opt{0};
      EXPECT_TRUE(opt.has_value());
   }

   EXPECT_EQ(TestHelper::str(), "vc~h");
}

TEST(UtOptional, EmptyCopyConstruct)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   EXPECT_FALSE(opt.has_value());

   ut::optional<TestHelper> cpy{opt};
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, EmptyMoveConstruct)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   EXPECT_FALSE(opt.has_value());

   ut::optional<TestHelper> mov{std::move(opt)};
   EXPECT_FALSE(mov.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, ValueCopyConstruct)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{0};
   EXPECT_TRUE(opt.has_value());

   ut::optional<TestHelper> cpy{opt};
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "vccc");
}

TEST(UtOptional, ValueMoveConstruct)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{0};
   EXPECT_TRUE(opt.has_value());

   ut::optional<TestHelper> cpy{std::move(opt)};
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "vcmc");
}

TEST(UtOptional, InPlaceConstruct)
{
   TestHelper::Clear();

   {
      ut::optional<TestHelper> opt{ut::in_place};
      EXPECT_TRUE(opt.has_value());
   }

   {
      ut::optional<TestHelper> opt{ut::in_place, 0};
      EXPECT_TRUE(opt.has_value());
   }
   EXPECT_EQ(TestHelper::str(), "dc~hvc~h");
}

TEST(UtOptional, EmptyConvertibleConstruct)
{
   TestHelper::Clear();

   ut::optional<int> opt;
   EXPECT_FALSE(opt.has_value());

   ut::optional<TestHelper> cpy{opt};
   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, ConvertibleConstruct)
{
   TestHelper::Clear();

   ut::optional<int> opt{0};
   EXPECT_TRUE(opt.has_value());

   ut::optional<TestHelper> cpy{opt};
   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "vc");
}

////////////////////////////////////////////////////////////////////
//                                                                //
//                         ASSIGNMENT                             //
//                                                                //
////////////////////////////////////////////////////////////////////

TEST(UtOptional, EmptyBraceAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{0};
   EXPECT_TRUE(opt.has_value());

   opt = {};
   EXPECT_FALSE(opt.has_value());
   EXPECT_EQ(TestHelper::str(), "vc~h");
}

TEST(UtOptional, NulloptAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{0};
   EXPECT_TRUE(opt.has_value());

   opt = ut::nullopt;
   EXPECT_FALSE(opt.has_value());
   EXPECT_EQ(TestHelper::str(), "vc~h");
}

//////////////////////////////////////
//          COPY ASSIGNMENT         //
//////////////////////////////////////

TEST(UtOptional, EmptyToEmptyCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   ut::optional<TestHelper> cpy;

   cpy = opt;

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, EmptyToFullCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = opt;

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dc~h");
}

TEST(UtOptional, FullToEmptyCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   ut::optional<TestHelper> cpy;

   cpy = opt;

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dccc");
}

TEST(UtOptional, FullToFullCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = opt;

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dcdcca");
}

//////////////////////////////////////
//          MOVE ASSIGNMENT         //
//////////////////////////////////////

TEST(UtOptional, EmptyToEmptyMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   ut::optional<TestHelper> cpy;

   cpy = std::move(opt);

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, EmptyToFullMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = std::move(opt);

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dc~h");
}

TEST(UtOptional, FullToEmptyMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   ut::optional<TestHelper> cpy;

   cpy = std::move(opt);

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dcmc");
}

TEST(UtOptional, FullToFullMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = std::move(opt);

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dcdcma");
}

//////////////////////////////////////
//    CONVERT + COPY ASSIGNMENT     //
//////////////////////////////////////

TEST(UtOptional, ConvertEmptyToEmptyCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt;
   ut::optional<TestHelper> cpy;

   cpy = opt;

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, ConvertEmptyToFullCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt;
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = opt;

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dc~h");
}

TEST(UtOptional, ConvertFullToEmptyCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt{ut::in_place};
   ut::optional<TestHelper> cpy;

   cpy = opt;

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "vc");
}

TEST(UtOptional, ConvertFullToFullCopyAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt{ut::in_place};
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = opt;

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dcvcma~h");
}

//////////////////////////////////////
//    CONVERT + MOVE ASSIGNMENT     //
//////////////////////////////////////

TEST(UtOptional, ConvertEmptyToEmptyMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt;
   ut::optional<TestHelper> cpy;

   cpy = std::move(opt);

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, ConvertEmptyToFullMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt;
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = std::move(opt);

   EXPECT_FALSE(opt.has_value());
   EXPECT_FALSE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dc~h");
}

TEST(UtOptional, ConvertFullToEmptyMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt{ut::in_place};
   ut::optional<TestHelper> cpy;

   cpy = std::move(opt);

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "vc");
}

TEST(UtOptional, ConvertFullToFullMoveAssignment)
{
   TestHelper::Clear();

   ut::optional<int>        opt{ut::in_place};
   ut::optional<TestHelper> cpy{ut::in_place};

   cpy = std::move(opt);

   EXPECT_TRUE(opt.has_value());
   EXPECT_TRUE(cpy.has_value());
   EXPECT_EQ(TestHelper::str(), "dcvcma~h");
}

//////////////////////////////////////
//         VALUE ASSIGNMENT         //
//////////////////////////////////////

TEST(UtOptional, ValueToEmptyCopyAssignment)
{
   TestHelper::Clear();

   TestHelper               val;
   ut::optional<TestHelper> opt;

   opt = val;

   EXPECT_TRUE(opt.has_value());
   EXPECT_EQ(TestHelper::str(), "dccc");
}

TEST(UtOptional, ValueToFullCopyAssignment)
{
   TestHelper::Clear();

   TestHelper               val;
   ut::optional<TestHelper> opt{ut::in_place};

   opt = val;

   EXPECT_TRUE(opt.has_value());
   EXPECT_EQ(TestHelper::str(), "dcdcca");
}

TEST(UtOptional, ValueToEmptyMoveAssignment)
{
   TestHelper::Clear();

   TestHelper               val;
   ut::optional<TestHelper> opt;

   opt = std::move(val);

   EXPECT_TRUE(opt.has_value());
   EXPECT_EQ(TestHelper::str(), "dcmc");
}

TEST(UtOptional, ValueToFullMoveAssignment)
{
   TestHelper::Clear();

   TestHelper               val;
   ut::optional<TestHelper> opt{ut::in_place};

   opt = std::move(val);

   EXPECT_TRUE(opt.has_value());
   EXPECT_EQ(TestHelper::str(), "dcdcma");
}


////////////////////////////////////////////////////////////////////
//                                                                //
//                          ACCESSORS                             //
//                                                                //
////////////////////////////////////////////////////////////////////

//////////////////////////////////////
//         ACCESS OPERATORS         //
//////////////////////////////////////

TEST(UtOptional, ArrowOperator)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   opt->foo();

   const auto& ref = opt;
   ref->foo();

   EXPECT_EQ(TestHelper::str(), "dcffcf");
}

TEST(UtOptional, IndirectionOperator)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   (*opt).foo();

   const auto& ref = opt;
   (*ref).foo();

   EXPECT_EQ(TestHelper::str(), "dcffcf");
}

TEST(UtOptional, FalseOperatorBool)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;
   if (opt)
   {
      GTEST_FAIL();
   }

   if (!opt)
   {
      // do nothing
   }
   else
   {
      GTEST_FAIL();
   }
}

TEST(UtOptional, TrueOperatorBool)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};
   if (opt)
   {
      // do nothing
   }
   else
   {
      GTEST_FAIL();
   }

   if (!opt)
   {
      GTEST_FAIL();
   }
}

//////////////////////////////////////
//          ACCESS METHODS          //
//////////////////////////////////////

TEST(UtOptional, Value)
{
   ut::optional<TestHelper> opt;

   EXPECT_ANY_THROW(opt.value());

   ut::optional<TestHelper> opt2{ut::in_place};

   EXPECT_NO_THROW(opt2.value());
}

TEST(UtOptional, ValueOr)
{
   ut::optional<int> opt;

   EXPECT_EQ(opt.value_or(1), 1);

   opt = 2;

   EXPECT_EQ(opt.value_or(1), 2);
}


////////////////////////////////////////////////////////////////////
//                                                                //
//                           MUTATORS                             //
//                                                                //
////////////////////////////////////////////////////////////////////

//////////////////////////////////////
//         ACCESS OPERATORS         //
//////////////////////////////////////

TEST(UtOptional, EmptyEmptySwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1;
   ut::optional<TestHelper> opt2;

   EXPECT_FALSE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   opt1.swap(opt2);

   EXPECT_FALSE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "");
}

TEST(UtOptional, FullEmptySwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1{ut::in_place};
   ut::optional<TestHelper> opt2;

   EXPECT_TRUE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   opt1.swap(opt2);

   EXPECT_FALSE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "dcmc~h");
}

TEST(UtOptional, EmptyFullSwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1;
   ut::optional<TestHelper> opt2{ut::in_place};

   EXPECT_FALSE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   opt1.swap(opt2);

   EXPECT_TRUE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "dcmc~h");
}

TEST(UtOptional, FullFullSwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1{ut::in_place};
   ut::optional<TestHelper> opt2{ut::in_place};

   EXPECT_TRUE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   opt1.swap(opt2);

   EXPECT_TRUE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "dcdcmcmama~h");
}

TEST(UtOptional, EmptyEmptyStdSwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1;
   ut::optional<TestHelper> opt2;

   EXPECT_FALSE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   std::swap(opt1, opt2);

   EXPECT_FALSE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "");
}

// In theory, this is the only case where the custom std::swap
//    functionality should differ from the default behavior.
TEST(UtOptional, FullEmptyStdSwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1{ut::in_place};
   ut::optional<TestHelper> opt2;

   EXPECT_TRUE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   std::swap(opt1, opt2);

   EXPECT_FALSE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "dcmc~h");
}

TEST(UtOptional, EmptyFullStdSwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1;
   ut::optional<TestHelper> opt2{ut::in_place};

   EXPECT_FALSE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   std::swap(opt1, opt2);

   EXPECT_TRUE(opt1.has_value());
   EXPECT_FALSE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "dcmc~h");
}

TEST(UtOptional, FullFullStdSwap)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt1{ut::in_place};
   ut::optional<TestHelper> opt2{ut::in_place};

   EXPECT_TRUE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   std::swap(opt1, opt2);

   EXPECT_TRUE(opt1.has_value());
   EXPECT_TRUE(opt2.has_value());

   EXPECT_EQ(TestHelper::str(), "dcdcmcmama~h");
}

TEST(UtOptional, Emplace)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt;

   opt.emplace();
   EXPECT_TRUE(opt.has_value());
   opt.emplace(0);
   EXPECT_TRUE(opt.has_value());

   EXPECT_EQ(TestHelper::str(), "dc~hvc");
}

TEST(UtOptional, Reset)
{
   TestHelper::Clear();

   ut::optional<TestHelper> opt{ut::in_place};

   opt.reset();
   opt.reset();

   EXPECT_EQ(TestHelper::str(), "dc~h");
}

TEST(UtOptional, Hash)
{
   ut::optional<int> opt;
   EXPECT_EQ(std::hash<ut::optional<int>>()(opt), 0);

   for (int i = 0; i < 1000; i++)
   {
      ut::optional<int> opti{i};
      EXPECT_EQ(std::hash<ut::optional<int>>()(opti), std::hash<int>()(i));
   }
}

TEST(UtOptional, MakeOptional)
{
   TestHelper::Clear();

   // Using explicit constructor
   auto opt = ut::make_optional<TestHelper>("");

   EXPECT_TRUE(opt.has_value());

   const volatile int cvi = 42;

   auto opti = ut::make_optional(cvi);

   static_assert(std::is_same<decltype(opti), ut::optional<int>>::value, "Failed ut::make_optional test.");

   EXPECT_TRUE(opti.has_value());
   EXPECT_EQ(*opti, 42);
}

template<typename TypeA, typename TypeB>
void TestOptionalEquality()
{
   ut::optional<TypeA> emptyA;
   ut::optional<TypeA> lowA{static_cast<TypeA>(0)};
   ut::optional<TypeA> highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_TRUE(emptyA == emptyB);
   EXPECT_FALSE(emptyA == lowB);
   EXPECT_FALSE(emptyA == highB);

   EXPECT_FALSE(lowA == emptyB);
   EXPECT_TRUE(lowA == lowB);
   EXPECT_FALSE(lowA == highB);

   EXPECT_FALSE(highA == emptyB);
   EXPECT_FALSE(highA == lowB);
   EXPECT_TRUE(highA == highB);
}

template<typename TypeA, typename TypeB>
void TestOptionalInequality()
{
   ut::optional<TypeA> emptyA;
   ut::optional<TypeA> lowA{static_cast<TypeA>(0)};
   ut::optional<TypeA> highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_FALSE(emptyA != emptyB);
   EXPECT_TRUE(emptyA != lowB);
   EXPECT_TRUE(emptyA != highB);

   EXPECT_TRUE(lowA != emptyB);
   EXPECT_FALSE(lowA != lowB);
   EXPECT_TRUE(lowA != highB);

   EXPECT_TRUE(highA != emptyB);
   EXPECT_TRUE(highA != lowB);
   EXPECT_FALSE(highA != highB);
}

template<typename TypeA, typename TypeB>
void TestOptionalLessThan()
{
   ut::optional<TypeA> emptyA;
   ut::optional<TypeA> lowA{static_cast<TypeA>(0)};
   ut::optional<TypeA> highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_FALSE(emptyA < emptyB);
   EXPECT_TRUE(emptyA < lowB);
   EXPECT_TRUE(emptyA < highB);

   EXPECT_FALSE(lowA < emptyB);
   EXPECT_FALSE(lowA < lowB);
   EXPECT_TRUE(lowA < highB);

   EXPECT_FALSE(highA < emptyB);
   EXPECT_FALSE(highA < lowB);
   EXPECT_FALSE(highA < highB);
}

template<typename TypeA, typename TypeB>
void TestOptionalLessThanOrEqual()
{
   ut::optional<TypeA> emptyA;
   ut::optional<TypeA> lowA{static_cast<TypeA>(0)};
   ut::optional<TypeA> highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_TRUE(emptyA <= emptyB);
   EXPECT_TRUE(emptyA <= lowB);
   EXPECT_TRUE(emptyA <= highB);

   EXPECT_FALSE(lowA <= emptyB);
   EXPECT_TRUE(lowA <= lowB);
   EXPECT_TRUE(lowA <= highB);

   EXPECT_FALSE(highA <= emptyB);
   EXPECT_FALSE(highA <= lowB);
   EXPECT_TRUE(highA <= highB);
}

template<typename TypeA, typename TypeB>
void TestOptionalGreaterThan()
{
   ut::optional<TypeA> emptyA;
   ut::optional<TypeA> lowA{static_cast<TypeA>(0)};
   ut::optional<TypeA> highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_FALSE(emptyA > emptyB);
   EXPECT_FALSE(emptyA > lowB);
   EXPECT_FALSE(emptyA > highB);

   EXPECT_TRUE(lowA > emptyB);
   EXPECT_FALSE(lowA > lowB);
   EXPECT_FALSE(lowA > highB);

   EXPECT_TRUE(highA > emptyB);
   EXPECT_TRUE(highA > lowB);
   EXPECT_FALSE(highA > highB);
}

template<typename TypeA, typename TypeB>
void TestOptionalGreaterThanOrEqual()
{
   ut::optional<TypeA> emptyA;
   ut::optional<TypeA> lowA{static_cast<TypeA>(0)};
   ut::optional<TypeA> highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_TRUE(emptyA >= emptyB);
   EXPECT_FALSE(emptyA >= lowB);
   EXPECT_FALSE(emptyA >= highB);

   EXPECT_TRUE(lowA >= emptyB);
   EXPECT_TRUE(lowA >= lowB);
   EXPECT_FALSE(lowA >= highB);

   EXPECT_TRUE(highA >= emptyB);
   EXPECT_TRUE(highA >= lowB);
   EXPECT_TRUE(highA >= highB);
}

template<typename TypeA, typename TypeB>
void TestValueEquality()
{
   TypeA lowA{static_cast<TypeA>(0)};
   TypeA highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_FALSE(lowA == emptyB);
   EXPECT_TRUE(lowA == lowB);
   EXPECT_FALSE(lowA == highB);

   EXPECT_FALSE(highA == emptyB);
   EXPECT_FALSE(highA == lowB);
   EXPECT_TRUE(highA == highB);

   EXPECT_FALSE(emptyB == lowA);
   EXPECT_FALSE(emptyB == highA);

   EXPECT_TRUE(lowB == lowA);
   EXPECT_FALSE(lowB == highA);

   EXPECT_FALSE(highB == lowA);
   EXPECT_TRUE(highB == highA);
}

template<typename TypeA, typename TypeB>
void TestValueInequality()
{
   TypeA lowA{static_cast<TypeA>(0)};
   TypeA highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_TRUE(lowA != emptyB);
   EXPECT_FALSE(lowA != lowB);
   EXPECT_TRUE(lowA != highB);

   EXPECT_TRUE(highA != emptyB);
   EXPECT_TRUE(highA != lowB);
   EXPECT_FALSE(highA != highB);

   EXPECT_TRUE(emptyB != lowA);
   EXPECT_TRUE(emptyB != highA);

   EXPECT_FALSE(lowB != lowA);
   EXPECT_TRUE(lowB != highA);

   EXPECT_TRUE(highB != lowA);
   EXPECT_FALSE(highB != highA);
}

template<typename TypeA, typename TypeB>
void TestValueLessThan()
{
   TypeA lowA{static_cast<TypeA>(0)};
   TypeA highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_FALSE(lowA < emptyB);
   EXPECT_FALSE(lowA < lowB);
   EXPECT_TRUE(lowA < highB);

   EXPECT_FALSE(highA < emptyB);
   EXPECT_FALSE(highA < lowB);
   EXPECT_FALSE(highA < highB);

   EXPECT_TRUE(emptyB < lowA);
   EXPECT_TRUE(emptyB < highA);

   EXPECT_FALSE(lowB < lowA);
   EXPECT_TRUE(lowB < highA);

   EXPECT_FALSE(highB < lowA);
   EXPECT_FALSE(highB < highA);
}

template<typename TypeA, typename TypeB>
void TestValueLessThanOrEqual()
{
   TypeA lowA{static_cast<TypeA>(0)};
   TypeA highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_FALSE(lowA <= emptyB);
   EXPECT_TRUE(lowA <= lowB);
   EXPECT_TRUE(lowA <= highB);

   EXPECT_FALSE(highA <= emptyB);
   EXPECT_FALSE(highA <= lowB);
   EXPECT_TRUE(highA <= highB);

   EXPECT_TRUE(emptyB <= lowA);
   EXPECT_TRUE(emptyB <= highA);

   EXPECT_TRUE(lowB <= lowA);
   EXPECT_TRUE(lowB <= highA);

   EXPECT_FALSE(highB <= lowA);
   EXPECT_TRUE(highB <= highA);
}

template<typename TypeA, typename TypeB>
void TestValueGreaterThan()
{
   TypeA lowA{static_cast<TypeA>(0)};
   TypeA highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_TRUE(lowA > emptyB);
   EXPECT_FALSE(lowA > lowB);
   EXPECT_FALSE(lowA > highB);

   EXPECT_TRUE(highA > emptyB);
   EXPECT_TRUE(highA > lowB);
   EXPECT_FALSE(highA > highB);

   EXPECT_FALSE(emptyB > lowA);
   EXPECT_FALSE(emptyB > highA);

   EXPECT_FALSE(lowB > lowA);
   EXPECT_FALSE(lowB > highA);

   EXPECT_TRUE(highB > lowA);
   EXPECT_FALSE(highB > highA);
}

template<typename TypeA, typename TypeB>
void TestValueGreaterThanOrEqual()
{
   TypeA lowA{static_cast<TypeA>(0)};
   TypeA highA{static_cast<TypeA>(1)};

   ut::optional<TypeB> emptyB;
   ut::optional<TypeB> lowB{static_cast<TypeB>(0)};
   ut::optional<TypeB> highB{static_cast<TypeB>(1)};

   EXPECT_TRUE(lowA >= emptyB);
   EXPECT_TRUE(lowA >= lowB);
   EXPECT_FALSE(lowA >= highB);

   EXPECT_TRUE(highA >= emptyB);
   EXPECT_TRUE(highA >= lowB);
   EXPECT_TRUE(highA >= highB);

   EXPECT_FALSE(emptyB >= lowA);
   EXPECT_FALSE(emptyB >= highA);

   EXPECT_TRUE(lowB >= lowA);
   EXPECT_FALSE(lowB >= highA);

   EXPECT_TRUE(highB >= lowA);
   EXPECT_TRUE(highB >= highA);
}

template<typename TypeA, typename TypeB>
void TestComparison()
{
   TestOptionalEquality<TypeA, TypeB>();
   TestOptionalInequality<TypeA, TypeB>();
   TestOptionalLessThan<TypeA, TypeB>();
   TestOptionalLessThanOrEqual<TypeA, TypeB>();
   TestOptionalGreaterThan<TypeA, TypeB>();
   TestOptionalGreaterThanOrEqual<TypeA, TypeB>();

   TestValueEquality<TypeA, TypeB>();
   TestValueInequality<TypeA, TypeB>();
   TestValueLessThan<TypeA, TypeB>();
   TestValueLessThanOrEqual<TypeA, TypeB>();
   TestValueGreaterThan<TypeA, TypeB>();
   TestValueGreaterThanOrEqual<TypeA, TypeB>();
}

TEST(UtOptional, OptionalComparison)
{
   TestComparison<int, int>();
   TestComparison<int, float>();
   TestComparison<float, int>();
   TestComparison<float, float>();
}

TEST(UtOptional, NulloptLeftComparison)
{
   ut::optional<int> empty;
   ut::optional<int> full{ut::in_place};

   EXPECT_TRUE(ut::nullopt == empty);
   EXPECT_FALSE(ut::nullopt == full);

   EXPECT_FALSE(ut::nullopt != empty);
   EXPECT_TRUE(ut::nullopt != full);

   EXPECT_FALSE(ut::nullopt < empty);
   EXPECT_TRUE(ut::nullopt < full);

   EXPECT_TRUE(ut::nullopt <= empty);
   EXPECT_TRUE(ut::nullopt <= full);

   EXPECT_FALSE(ut::nullopt > empty);
   EXPECT_FALSE(ut::nullopt > full);

   EXPECT_TRUE(ut::nullopt >= empty);
   EXPECT_FALSE(ut::nullopt >= full);
}

TEST(UtOptional, NulloptRightComparison)
{
   ut::optional<int> empty;
   ut::optional<int> full{ut::in_place};

   EXPECT_TRUE(empty == ut::nullopt);
   EXPECT_FALSE(full == ut::nullopt);

   EXPECT_FALSE(empty != ut::nullopt);
   EXPECT_TRUE(full != ut::nullopt);

   EXPECT_FALSE(empty < ut::nullopt);
   EXPECT_FALSE(full < ut::nullopt);

   EXPECT_TRUE(empty <= ut::nullopt);
   EXPECT_FALSE(full <= ut::nullopt);

   EXPECT_FALSE(empty > ut::nullopt);
   EXPECT_TRUE(full > ut::nullopt);

   EXPECT_TRUE(empty >= ut::nullopt);
   EXPECT_TRUE(full >= ut::nullopt);
}
