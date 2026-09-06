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

#include <string>

#include <gtest/gtest.h>

#include "UtOutPtr.hpp"

namespace test_ut_outptr
{
void MockGet(int** aPtr, int* aValue)
{
   *aPtr = aValue;
}

void MockGetVoid(void** aPtr, int* aValue)
{
   *reinterpret_cast<int**>(aPtr) = aValue;
}

struct MockSmartPointer
{
   using pointer = int*;
   MockSmartPointer(std::string& aHistory)
      : mHistory(aHistory)
   {
      mHistory += "c";
   }
   ~MockSmartPointer() { mHistory += "d"; }

   void reset(int* aPtr, const std::string& aHistory = std::string())
   {
      if (aPtr)
      {
         mValue = *aPtr;
         mHistory += "r(";
         mHistory += std::to_string(*aPtr);
         if (!aHistory.empty())
         {
            mHistory += ",";
            mHistory += aHistory;
         }
         mHistory += ")";
      }
      else
      {
         mValue = 0;
         mHistory += "R(";
         mHistory += aHistory;
         mHistory += ")";
      }
   }

   std::string& mHistory;
   int          mValue = 0;
};
} // namespace test_ut_outptr

TEST(OutPtr, MockSmartPointer)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   {
      test_ut_outptr::MockSmartPointer ptr2{history};
      EXPECT_EQ(history, "cc");
   }
   EXPECT_EQ(history, "ccd");

   history.clear();

   ptr.reset(nullptr);
   EXPECT_EQ(history, "R()");
   ptr.reset(nullptr, "xyz");
   EXPECT_EQ(history, "R()R(xyz)");

   history.clear();

   int x = 5;
   ptr.reset(&x);
   EXPECT_EQ(history, "r(5)");
   ptr.reset(&x, "xyz");
   EXPECT_EQ(history, "r(5)r(5,xyz)");
}

TEST(OutPtr, Constructor)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*> op{ptr};
      EXPECT_EQ(history, "c");
   }
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, GetNull)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*> op{ptr};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      test_ut_outptr::MockGet(op, nullptr);
   }
   EXPECT_EQ(ptr.mValue, 0);
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, GetNonNull)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*> op{ptr};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      int x = 5;
      test_ut_outptr::MockGet(op, &x);
   }
   EXPECT_EQ(ptr.mValue, 5);
   EXPECT_EQ(history, "cr(5)");
}

TEST(OutPtr, GetNullArg)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*, std::string> op{ptr, "xyz"};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      test_ut_outptr::MockGet(op, nullptr);
   }
   EXPECT_EQ(ptr.mValue, 0);
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, GetNonNullArg)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*, std::string> op{ptr, "xyz"};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      int x = 5;
      test_ut_outptr::MockGet(op, &x);
   }
   EXPECT_EQ(ptr.mValue, 5);
   EXPECT_EQ(history, "cr(5,xyz)");
}

TEST(OutPtr, GetVoidNull)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*> op{ptr};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      test_ut_outptr::MockGetVoid(op, nullptr);
   }
   EXPECT_EQ(ptr.mValue, 0);
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, GetVoidNonNull)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*> op{ptr};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      int x = 5;
      test_ut_outptr::MockGetVoid(op, &x);
   }
   EXPECT_EQ(ptr.mValue, 5);
   EXPECT_EQ(history, "cr(5)");
}

TEST(OutPtr, GetVoidNullArg)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*, std::string> op{ptr, "xyz"};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      test_ut_outptr::MockGetVoid(op, nullptr);
   }
   EXPECT_EQ(ptr.mValue, 0);
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, GetVoidNonNullArg)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   // RAII block
   {
      ut::out_ptr_t<test_ut_outptr::MockSmartPointer, int*, std::string> op{ptr, "xyz"};
      EXPECT_EQ(ptr.mValue, 0);
      EXPECT_EQ(history, "c");
      int x = 5;
      test_ut_outptr::MockGetVoid(op, &x);
   }
   EXPECT_EQ(ptr.mValue, 5);
   EXPECT_EQ(history, "cr(5,xyz)");
}

TEST(OutPtr, OutPtrNull)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   test_ut_outptr::MockGet(ut::out_ptr(ptr), nullptr);
   EXPECT_EQ(ptr.mValue, 0);
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, OutPtrNonNull)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   int x = 5;
   test_ut_outptr::MockGet(ut::out_ptr(ptr), &x);
   EXPECT_EQ(ptr.mValue, 5);
   EXPECT_EQ(history, "cr(5)");
}

TEST(OutPtr, OutPtrNullArg)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   test_ut_outptr::MockGet(ut::out_ptr(ptr, "xyz"), nullptr);
   EXPECT_EQ(ptr.mValue, 0);
   EXPECT_EQ(history, "c");
}

TEST(OutPtr, OutPtrNonNullArg)
{
   std::string                      history;
   test_ut_outptr::MockSmartPointer ptr{history};
   EXPECT_EQ(history, "c");
   int x = 5;
   test_ut_outptr::MockGet(ut::out_ptr(ptr, "xyz"), &x);
   EXPECT_EQ(ptr.mValue, 5);
   EXPECT_EQ(history, "cr(5,xyz)");
}
