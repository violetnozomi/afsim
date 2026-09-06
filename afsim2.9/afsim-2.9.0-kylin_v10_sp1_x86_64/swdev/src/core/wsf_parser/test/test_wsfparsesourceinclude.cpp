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

#include "WsfParseSourceInclude.hpp"

TEST(WsfParseSourceInclude, DefaultConstructor)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
}

TEST(WsfParseSourceInclude, GetRoot)
{
   WsfParseSourceInclude testParentInclude;
   EXPECT_TRUE(testParentInclude.mEntries.empty());
   EXPECT_EQ(testParentInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testParentInclude.mParentPtr, nullptr);
   EXPECT_EQ(testParentInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testParentInclude.mIncludeCount, 1);
   auto* testChildIncludePtr = new WsfParseSourceInclude;
   // This does not call any WsfParseSourceInclude::Add method.
   // For this reason, set the parent WsfParseSourceInclude manually.
   testChildIncludePtr->mParentPtr = &testParentInclude;
   EXPECT_EQ(testChildIncludePtr->mParentPtr, &testParentInclude);
   WsfParseSourceInclude::Entry e;
   e.mIncludePtr = std::unique_ptr<WsfParseSourceInclude>(testChildIncludePtr);
   testParentInclude.mEntries.emplace_back(std::move(e));
   EXPECT_EQ(testChildIncludePtr->GetRoot(), &testParentInclude);
}

TEST(WsfParseSourceInclude, GetId)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   WsfParseSourceIncludeId testId{testInclude.GetId()};
   EXPECT_EQ(testId.first, nullptr);
   EXPECT_EQ(testId.second, testInclude.mIncludeCount);
}

TEST(WsfParseSourceInclude, AddLineComment)
{
   WsfParseSourceInclude testInclude;
   UtTextRange           testRange;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   testInclude.AddComment(testRange, true);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cLINE_COMMENT);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddDuplicateLineComment)
{
   WsfParseSourceInclude testInclude;
   UtTextRange           testRange;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   testInclude.AddComment(testRange, true);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cLINE_COMMENT);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
   // Attempt to add a duplicate comment.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddComment(testRange, true);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateEntry = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateEntry.mType, WsfParseSourceInclude::EntryType::cLINE_COMMENT);
   EXPECT_FALSE(testDuplicateEntry.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateEntry.mLocation, testRange);
   EXPECT_EQ(testDuplicateEntry.mTargetSource, nullptr);
   EXPECT_EQ(testDuplicateEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddBlockComment)
{
   WsfParseSourceInclude testInclude;
   UtTextRange           testRange;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   testInclude.AddComment(testRange, false);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cBLOCK_COMMENT);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddDuplicateBlockComment)
{
   WsfParseSourceInclude testInclude;
   UtTextRange           testRange;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   testInclude.AddComment(testRange, false);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cBLOCK_COMMENT);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
   // Attempt to add a duplicate comment.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddComment(testRange, false);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateEntry = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateEntry.mType, WsfParseSourceInclude::EntryType::cBLOCK_COMMENT);
   EXPECT_FALSE(testDuplicateEntry.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateEntry.mLocation, testRange);
   EXPECT_EQ(testDuplicateEntry.mTargetSource, nullptr);
   EXPECT_EQ(testDuplicateEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddIncludeUtTextDocument)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange    testRange;
   UtTextDocument testDocument;
   testInclude.AddInclude(testRange, &testDocument);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, &testDocument);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddDuplicateIncludeUtTextDocument)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange    testRange;
   UtTextDocument testDocument;
   testInclude.AddInclude(testRange, &testDocument);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, &testDocument);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
   // Attempt to add a duplicate include.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddInclude(testRange, &testDocument);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateEntry = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_FALSE(testDuplicateEntry.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateEntry.mLocation, testRange);
   EXPECT_EQ(testDuplicateEntry.mTargetSource, &testDocument);
   EXPECT_EQ(testDuplicateEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddIncludeWsfParseSourceInclude)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange            testRange;
   WsfParseSourceInclude* testIncludePtr = new WsfParseSourceInclude;
   testInclude.AddInclude(testRange, testIncludePtr);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), testIncludePtr);
}

TEST(WsfParseSourceInclude, AddDuplicateIncludeWsfParseSourceInclude)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange            testRange;
   WsfParseSourceInclude* testIncludePtr = new WsfParseSourceInclude;
   testInclude.AddInclude(testRange, testIncludePtr);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), testIncludePtr);
   // Attempt to add a duplicate include.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddInclude(testRange, testIncludePtr);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateInclude = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateInclude.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_FALSE(testDuplicateInclude.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateInclude.mLocation, testRange);
   EXPECT_EQ(testDuplicateInclude.mTargetSource, nullptr);
   EXPECT_EQ(testDuplicateInclude.mIncludePtr.get(), testIncludePtr);
}

TEST(WsfParseSourceInclude, AddVirtualInclude)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   WsfParseSourceInclude* testIncludePtr = new WsfParseSourceInclude;
   testInclude.AddVirtualInclude(testIncludePtr);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_TRUE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, WsfParseSourceInclude::cVIRTUAL_RANGE);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), testIncludePtr);
}

TEST(WsfParseSourceInclude, AddDuplicateVirtualInclude)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   WsfParseSourceInclude* testIncludePtr = new WsfParseSourceInclude;
   testInclude.AddVirtualInclude(testIncludePtr);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_TRUE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, WsfParseSourceInclude::cVIRTUAL_RANGE);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), testIncludePtr);
   // Attempt to add a duplicate virtual include.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddVirtualInclude(testIncludePtr);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateEntry = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateEntry.mType, WsfParseSourceInclude::EntryType::cINCLUDE_COMMAND);
   EXPECT_TRUE(testDuplicateEntry.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateEntry.mLocation, WsfParseSourceInclude::cVIRTUAL_RANGE);
   EXPECT_EQ(testDuplicateEntry.mTargetSource, nullptr);
   EXPECT_EQ(testDuplicateEntry.mIncludePtr.get(), testIncludePtr);
}

TEST(WsfParseSourceInclude, AddPreprocessorDefine)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange testRange;
   testInclude.AddPreprocessorEntry(WsfParseSourceInclude::EntryType::cPREPROCESSOR_DEFINE, testRange);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cPREPROCESSOR_DEFINE);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddDuplicatePreprocessorDefine)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange testRange;
   testInclude.AddPreprocessorEntry(WsfParseSourceInclude::EntryType::cPREPROCESSOR_DEFINE, testRange);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cPREPROCESSOR_DEFINE);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
   // Attempt to add a duplicate preprocessor define.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddPreprocessorEntry(WsfParseSourceInclude::EntryType::cPREPROCESSOR_DEFINE, testRange);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateEntry = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateEntry.mType, WsfParseSourceInclude::EntryType::cPREPROCESSOR_DEFINE);
   EXPECT_FALSE(testDuplicateEntry.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateEntry.mLocation, testRange);
   EXPECT_EQ(testDuplicateEntry.mTargetSource, nullptr);
   EXPECT_EQ(testDuplicateEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddPreprocessorVariable)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange testRange;
   testInclude.AddPreprocessorEntry(WsfParseSourceInclude::EntryType::cPREPROCESSOR_VARIABLE, testRange);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cPREPROCESSOR_VARIABLE);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
}

TEST(WsfParseSourceInclude, AddDuplicatePreprocessorVariable)
{
   WsfParseSourceInclude testInclude;
   EXPECT_TRUE(testInclude.mEntries.empty());
   EXPECT_EQ(testInclude.mLocation, UtTextRange{});
   EXPECT_EQ(testInclude.mParentPtr, nullptr);
   EXPECT_EQ(testInclude.mSourcePtr, nullptr);
   EXPECT_EQ(testInclude.mIncludeCount, 1);
   UtTextRange testRange;
   testInclude.AddPreprocessorEntry(WsfParseSourceInclude::EntryType::cPREPROCESSOR_VARIABLE, testRange);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testEntry = testInclude.mEntries[0];
   EXPECT_EQ(testEntry.mType, WsfParseSourceInclude::EntryType::cPREPROCESSOR_VARIABLE);
   EXPECT_FALSE(testEntry.mIsVirtualInclude);
   EXPECT_EQ(testEntry.mLocation, testRange);
   EXPECT_EQ(testEntry.mTargetSource, nullptr);
   EXPECT_EQ(testEntry.mIncludePtr.get(), nullptr);
   // Attempt to add a duplicate preprocessor variable expansion.
   // The include object should ignore the attempt.
   // The mEntries should stay the same.
   testInclude.AddPreprocessorEntry(WsfParseSourceInclude::EntryType::cPREPROCESSOR_VARIABLE, testRange);
   ASSERT_TRUE(testInclude.mEntries.size() == 1);
   const WsfParseSourceInclude::Entry& testDuplicateEntry = testInclude.mEntries[0];
   EXPECT_EQ(testDuplicateEntry.mType, WsfParseSourceInclude::EntryType::cPREPROCESSOR_VARIABLE);
   EXPECT_FALSE(testDuplicateEntry.mIsVirtualInclude);
   EXPECT_EQ(testDuplicateEntry.mLocation, testRange);
   EXPECT_EQ(testDuplicateEntry.mTargetSource, nullptr);
   EXPECT_EQ(testDuplicateEntry.mIncludePtr.get(), nullptr);
}
