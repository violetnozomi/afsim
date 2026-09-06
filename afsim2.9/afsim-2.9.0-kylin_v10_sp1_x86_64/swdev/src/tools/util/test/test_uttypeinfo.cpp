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

#include "UtTypeInfo.hpp"

namespace xyz
{
struct test_struct
{
   struct nested
   {
   };
};
class test_class
{
public:
   virtual ~test_class() = default;
};
union test_union
{
};

class test_derived : public test_class
{
};

template<typename T>
class test_templ
{
};

template<typename... Ts>
class test_pack
{
};

enum named_enum
{
   cNAMED_ENUM
};

enum class scoped_enum
{
   cSCOPED_ENUM
};
} // namespace xyz

namespace
{
struct abc
{
};
} // namespace

TEST(UtTypeInfo, Primitives)
{
   EXPECT_EQ(ut::TypeNameOf<int>(), "int");
   EXPECT_EQ(ut::TypeNameOf<const int>(), "int");
   EXPECT_EQ(ut::TypeNameOf<int&>(), "int");
   EXPECT_EQ(ut::TypeNameOf<const int&>(), "int");

   EXPECT_EQ(ut::TypeNameOf<int*>(), "int*");
   EXPECT_EQ(ut::TypeNameOf<const int*>(), "int const*");
   EXPECT_EQ(ut::TypeNameOf<int**>(), "int**");

   // Note: on MSVC, typeid(int[]) == typeid(int[0]).
   EXPECT_EQ(ut::TypeNameOf<int[]>(), "int [0]");
   EXPECT_EQ(ut::TypeNameOf<int[10]>(), "int [10]");

   EXPECT_EQ(ut::TypeNameOf<void>(), "void");
   EXPECT_EQ(ut::TypeNameOf<bool>(), "bool");
   EXPECT_EQ(ut::TypeNameOf<char>(), "char");
   EXPECT_EQ(ut::TypeNameOf<signed char>(), "signed char");
   EXPECT_EQ(ut::TypeNameOf<unsigned char>(), "unsigned char");
   EXPECT_EQ(ut::TypeNameOf<short>(), "short");
   EXPECT_EQ(ut::TypeNameOf<unsigned short>(), "unsigned short");
   EXPECT_EQ(ut::TypeNameOf<unsigned>(), "unsigned int");
   EXPECT_EQ(ut::TypeNameOf<long>(), "long");
   EXPECT_EQ(ut::TypeNameOf<unsigned long>(), "unsigned long");
   EXPECT_EQ(ut::TypeNameOf<long long>(), "long long");
   EXPECT_EQ(ut::TypeNameOf<unsigned long long>(), "unsigned long long");

   EXPECT_EQ(ut::TypeNameOf<float>(), "float");
   EXPECT_EQ(ut::TypeNameOf<double>(), "double");
   EXPECT_EQ(ut::TypeNameOf<long double>(), "long double");
}

TEST(UtTypeInfo, UserTypes)
{
   EXPECT_EQ(ut::TypeNameOf<xyz::test_struct>(), "xyz::test_struct");
   EXPECT_EQ(ut::TypeNameOf<xyz::test_struct*>(), "xyz::test_struct*");

   EXPECT_EQ(ut::TypeNameOf<xyz::test_class>(), "xyz::test_class");
   EXPECT_EQ(ut::TypeNameOf<xyz::test_class*>(), "xyz::test_class*");

   EXPECT_EQ(ut::TypeNameOf<xyz::test_union>(), "xyz::test_union");
   EXPECT_EQ(ut::TypeNameOf<xyz::test_union*>(), "xyz::test_union*");

   EXPECT_EQ(ut::TypeNameOf<abc>(), "`anonymous`::abc");

   EXPECT_EQ(ut::TypeNameOf<xyz::test_struct::nested>(), "xyz::test_struct::nested");

   EXPECT_EQ(ut::TypeNameOf<xyz::named_enum>(), "xyz::named_enum");
   EXPECT_EQ(ut::TypeNameOf<xyz::named_enum*>(), "xyz::named_enum*");

   EXPECT_EQ(ut::TypeNameOf<xyz::scoped_enum>(), "xyz::scoped_enum");
   EXPECT_EQ(ut::TypeNameOf<xyz::scoped_enum*>(), "xyz::scoped_enum*");
}

TEST(UtTypeInfo, TemplateTypes)
{
   EXPECT_EQ(ut::TypeNameOf<xyz::test_templ<void>>(), "xyz::test_templ<void>");
   EXPECT_EQ(ut::TypeNameOf<xyz::test_templ<int&>>(), "xyz::test_templ<int&>");
   EXPECT_EQ(ut::TypeNameOf<xyz::test_templ<xyz::test_templ<void>>>(), "xyz::test_templ<xyz::test_templ<void> >");
   EXPECT_EQ((ut::TypeNameOf<xyz::test_pack<int, int>>()), "xyz::test_pack<int,int>");
}

TEST(UtTypeInfo, FunctionTypes)
{
   EXPECT_EQ(ut::TypeNameOf<void()>(), "void ()");
   EXPECT_EQ(ut::TypeNameOf<void (*)()>(), "void (*)()");
   EXPECT_EQ(ut::TypeNameOf<void(int, int)>(), "void (int,int)");
   EXPECT_EQ(ut::TypeNameOf<void() noexcept>(), "void ()");

   EXPECT_EQ(ut::TypeNameOf<xyz::test_struct()>(), "xyz::test_struct ()");

   EXPECT_EQ(ut::TypeNameOf<void (xyz::test_class::*)()>(), "void (xyz::test_class::*)()");
   EXPECT_EQ(ut::TypeNameOf<int xyz::test_class::*>(), "int xyz::test_class::*");

   EXPECT_EQ(ut::TypeNameOf<void (xyz::test_class::*)() const>(), "void (xyz::test_class::*)() const");
   EXPECT_EQ(ut::TypeNameOf<const int xyz::test_class::*>(), "int const xyz::test_class::*");
}

TEST(UtTypeInfo, Polymorphism)
{
   xyz::test_class   c;
   xyz::test_derived d;

   EXPECT_EQ(ut::TypeNameOf(c), "xyz::test_class");
   EXPECT_EQ(ut::TypeNameOf(d), "xyz::test_derived");

   // IILE
   [](xyz::test_class& aClass) { EXPECT_EQ(ut::TypeNameOf(aClass), "xyz::test_class"); }(c);
   // IILE
   [](xyz::test_class& aClass) { EXPECT_EQ(ut::TypeNameOf(aClass), "xyz::test_derived"); }(d);
}
