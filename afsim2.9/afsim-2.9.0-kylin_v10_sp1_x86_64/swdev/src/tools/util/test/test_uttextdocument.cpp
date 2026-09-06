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

#include <chrono>
#include <iostream>

#include <gtest/gtest.h>

#include "UtRandom.hpp"
#include "UtStringRef.hpp"
#include "UtTextDocument.hpp"

//=================================================================================
// Commit message:  added test coverage for UtTextDocument and related classes
//=================================================================================

std::string sampleText{
   "// ****************************************************************************               \n"
   "//                                                                                            \n"
   "// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)                   \n"
   "//                                                                                            \n"
   "// ****************************************************************************                "};

const std::string newLineInSampleText{
   "//                                                                                             "};

UtPath sampleTextPath{"test_uttextdocument_sampleText.txt"};
UtPath loremIpsumPath{"test_uttextdocument_loremIpsum.txt"};

TEST(UtTextDocument, UtTextRange)
{
   UtTextRange range;
   EXPECT_EQ(range.GetBegin(), 0);
   EXPECT_EQ(range.GetEnd(), ut::npos);
   EXPECT_EQ(range.Length(), 0);
   range.SetBegin(2);
   EXPECT_EQ(range.GetBegin(), 2);
   range.SetEnd(3);
   EXPECT_EQ(range.GetEnd(), 3);
   range.SetRange(3, 5);
   EXPECT_EQ(range.GetBegin(), 3);
   EXPECT_EQ(range.GetEnd(), 5);
   range.Extend(2);
   EXPECT_EQ(range.GetBegin(), 2);
   EXPECT_EQ(range.GetEnd(), 5);
   EXPECT_TRUE(range.Contains(3));
   range.Move(4);
   EXPECT_EQ(range.GetBegin(), 6);
   EXPECT_EQ(range.GetEnd(), 9);

   UtTextRange noIntersect(3, 5);
   EXPECT_FALSE(range.Intersects(noIntersect));
   UtTextRange yesIntersect(5, 9);
   EXPECT_TRUE(range.Intersects(yesIntersect));

   UtTextRange offsetRange{UtTextRange::Offset(7, 7)};
   EXPECT_EQ(offsetRange.GetBegin(), 7);
   EXPECT_EQ(offsetRange.GetEnd(), 13);
   EXPECT_EQ(offsetRange.Length(), 7);
   offsetRange.Extend(range);
   EXPECT_TRUE(offsetRange.Contains(range.GetBegin()));
   EXPECT_TRUE(offsetRange.Contains(range.GetEnd()));

   UtTextRange copyRange{range};
   EXPECT_TRUE(copyRange == range);
   EXPECT_FALSE(offsetRange == range);
}

TEST(UtTextDocument, UtTextDocument)
{
   const std::string loremIpsum{
      "// Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor          \n"
      "// incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis              \n"
      "// nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.           \n"
      "// Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu         \n"
      "// fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in          \n"
      "// culpa qui officia deserunt mollit anim id est laborum.                                   "};

   UtTextDocument defaultSource{};
   EXPECT_EQ(defaultSource.GetLineCount(), 1);
   EXPECT_EQ(defaultSource.GetLineLength(0), 0);
   EXPECT_EQ(*defaultSource.Begin(), '\0');
   EXPECT_EQ(defaultSource[0], '\0');

   std::istringstream inFile{sampleText};
   UtTextDocument     sourceFromStream{inFile};
   EXPECT_EQ(sourceFromStream.GetPointer(), sampleText);
   EXPECT_EQ(sourceFromStream.GetLineCount(), 5);
   EXPECT_EQ(sourceFromStream.Size(), 476);
   EXPECT_EQ(sourceFromStream.GetLineLength(0), 95);
   EXPECT_EQ(sourceFromStream.GetLineLength(4), 95);
   EXPECT_EQ(sourceFromStream.GetLineString(2),
             "// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)                   \n");
   UtTextDocument::TextRange afsimRangeIters{sourceFromStream.GetLineRange(3)};
   UtTextRange               afsimRange{afsimRangeIters.first, afsimRangeIters.second};
   EXPECT_EQ(afsimRange.GetBegin(), 285);
   EXPECT_EQ(afsimRange.GetEnd(), 380);
   EXPECT_EQ(afsimRange.Length(), 96);

   ut::Random randGen;
   randGen.SetSeed(static_cast<unsigned int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
   size_t randPos{randGen.Uniform<size_t>(0, sampleText.size())};
   auto   randPosChar = sourceFromStream[randPos];
   size_t row;
   size_t col;
   sourceFromStream.PositionToLineColumn(randPos, row, col);
   auto rowColChar = sourceFromStream.GetLineString(row)[col];
   EXPECT_EQ(randPosChar, rowColChar);
   sourceFromStream.ValidateLines();

   std::ofstream outFile(sampleTextPath.GetNormalizedPath());
   outFile << sampleText;
   outFile.close();
   outFile.open(loremIpsumPath.GetNormalizedPath());
   outFile << loremIpsum;
   outFile.close();

   UtTextDocument sampleTextSource{sampleTextPath};
   UtTextDocument loremIpsumSource;
   loremIpsumSource.ReadFile(loremIpsumPath);
   loremIpsumSource.WriteFile(outFile);

   EXPECT_STREQ(sourceFromStream.GetPointer(), sampleTextSource.GetPointer());
   EXPECT_TRUE(sourceFromStream != loremIpsumSource);
   std::swap(sampleTextSource, loremIpsumSource);

   sourceFromStream.Clear();
   EXPECT_EQ(sourceFromStream.Size(), 0);
   EXPECT_EQ(sourceFromStream.GetLineCount(), 1);
   sampleTextSource.FreeMemory();
   sampleTextPath.RemoveFile();
   loremIpsumPath.RemoveFile();
}

TEST(UtTextDocument, UtTextDocumentLocation)
{
   UtTextDocumentLocation defaultLoc;
   EXPECT_EQ(defaultLoc.mSource, nullptr);
   EXPECT_EQ(defaultLoc.mPosition, 0);

   std::istringstream     inFile{sampleText};
   UtTextDocument         sourceFromStream{inFile};
   UtTextDocumentLocation loc{&sourceFromStream, 7};
   EXPECT_EQ(loc.mSource, &sourceFromStream);
   EXPECT_EQ(loc.mPosition, 7);

   EXPECT_TRUE(defaultLoc < loc);
   EXPECT_FALSE(defaultLoc == loc);
}

TEST(UtTextDocument, UtTextDocumentRange)
{
   UtTextDocumentRange defaultRange;
   EXPECT_EQ(defaultRange.mSource, nullptr);

   std::istringstream  inFile{sampleText};
   UtTextDocument      sourceFromStream{inFile};
   UtTextDocumentRange indexRange{&sourceFromStream, 7, 42};
   EXPECT_EQ(indexRange.mSource, &sourceFromStream);
   EXPECT_EQ(indexRange.GetBegin(), 7);
   EXPECT_EQ(indexRange.GetEnd(), 42);
   EXPECT_EQ(indexRange.Length(), 36);

   UtTextRange range{8, 24};
   EXPECT_EQ(range.GetBegin(), 8);
   EXPECT_EQ(range.GetEnd(), 24);
   EXPECT_EQ(range.Length(), 17);
   UtTextDocumentRange rangeRange{&sourceFromStream, range};
   EXPECT_EQ(rangeRange.mSource, &sourceFromStream);
   EXPECT_EQ(rangeRange.GetBegin(), 8);
   EXPECT_EQ(rangeRange.GetEnd(), 24);
   EXPECT_EQ(rangeRange.Length(), 17);

   UtTextDocumentLocation loc{&sourceFromStream, 31};
   EXPECT_EQ(loc.mSource, &sourceFromStream);
   EXPECT_EQ(loc.mPosition, 31);
   UtTextDocumentRange locRange{loc, 15};
   EXPECT_EQ(locRange.mSource, &sourceFromStream);
   EXPECT_EQ(locRange.GetBegin(), loc.mPosition);
   EXPECT_EQ(locRange.GetEnd(), 45);
   EXPECT_EQ(locRange.Length(), 15);

   std::string indexRangeTextChars{"************************************"};
   auto        indexRangeTextStrFromPtr  = std::string(indexRange.Pointer());
   auto indexRangeTextStrFromPtrValidStr = indexRangeTextStrFromPtr.substr(indexRange.GetBegin(), indexRange.Length());
   EXPECT_EQ(indexRangeTextStrFromPtrValidStr, indexRangeTextChars);
   auto indexRangeText = indexRange.Text();
   EXPECT_EQ(indexRangeText, indexRangeTextChars);
   UtStringRef indexRangeTextRef;
   indexRange.Text(indexRangeTextRef);
   EXPECT_EQ(indexRangeTextRef.Get(), indexRangeTextChars);
   std::string indexRangeTextStr;
   indexRange.Text(indexRangeTextStr);
   EXPECT_EQ(indexRangeTextStr, indexRangeTextChars);

   UtStringRef defaultRangeTextRef;
   defaultRange.Text(defaultRangeTextRef);
   EXPECT_EQ(defaultRangeTextRef.Get(), "");
   std::string defaultRangeTextStr;
   defaultRange.Text(defaultRangeTextStr);
   EXPECT_EQ(defaultRangeTextStr, "");

   EXPECT_FALSE(defaultRange.Valid());
   EXPECT_TRUE(indexRange.Valid());

   EXPECT_TRUE(defaultRange == "");
   EXPECT_TRUE(indexRange == indexRangeTextChars.data());
   EXPECT_TRUE(indexRange == indexRangeTextChars);
   EXPECT_FALSE(indexRange == rangeRange);

   auto indexRangeLoc = indexRange.GetPosition();
   EXPECT_EQ(indexRangeLoc.mSource, indexRange.mSource);
   EXPECT_EQ(indexRangeLoc.mPosition, indexRange.GetBegin());
}

TEST(UtTextDocument, UtTextDocumentAutoUpdateRange)
{
   UtTextDocumentAutoUpdateRange defaultRangeAU;

   std::istringstream            inFile{sampleText};
   UtTextDocument                sourceFromStream{inFile};
   UtTextDocumentAutoUpdateRange indexRangeAU{&sourceFromStream, 3, 7};
   EXPECT_EQ(indexRangeAU.mSource, &sourceFromStream);
   EXPECT_EQ(indexRangeAU.GetBegin(), 3);
   EXPECT_EQ(indexRangeAU.GetEnd(), 7);
   EXPECT_EQ(indexRangeAU.Length(), 5);

   UtTextRange range{7, 13};
   EXPECT_EQ(range.GetBegin(), 7);
   EXPECT_EQ(range.GetEnd(), 13);
   EXPECT_EQ(range.Length(), 7);
   UtTextDocumentAutoUpdateRange rangeRangeAU{&sourceFromStream, range};
   EXPECT_EQ(rangeRangeAU.mSource, &sourceFromStream);
   EXPECT_EQ(rangeRangeAU.GetBegin(), 7);
   EXPECT_EQ(rangeRangeAU.GetEnd(), 13);
   EXPECT_EQ(rangeRangeAU.Length(), 7);

   UtTextDocumentLocation location{&sourceFromStream, 42};
   EXPECT_EQ(location.mSource, &sourceFromStream);
   UtTextDocumentAutoUpdateRange locationRangeAU{location, 13};
   EXPECT_EQ(locationRangeAU.mSource, location.mSource);
   EXPECT_EQ(locationRangeAU.GetBegin(), location.mPosition);
   EXPECT_EQ(locationRangeAU.GetEnd(), 54);
   EXPECT_EQ(locationRangeAU.Length(), 13);

   UtTextDocumentAutoUpdateRange copyRangeAU{indexRangeAU};
   EXPECT_EQ(copyRangeAU.mSource, indexRangeAU.mSource);
   EXPECT_EQ(copyRangeAU.GetBegin(), indexRangeAU.GetBegin());
   EXPECT_EQ(copyRangeAU.GetEnd(), indexRangeAU.GetEnd());
   EXPECT_EQ(copyRangeAU.Length(), indexRangeAU.Length());

   UtTextDocumentRange docRange{&sourceFromStream, 7, 21};
   EXPECT_EQ(docRange.mSource, &sourceFromStream);
   EXPECT_EQ(docRange.GetBegin(), 7);
   EXPECT_EQ(docRange.GetEnd(), 21);
   EXPECT_EQ(docRange.Length(), 15);
   UtTextDocumentAutoUpdateRange copyDocRangeAU{docRange};
   EXPECT_EQ(copyDocRangeAU.mSource, &sourceFromStream);
   EXPECT_EQ(copyDocRangeAU.GetBegin(), docRange.GetBegin());
   EXPECT_EQ(copyDocRangeAU.GetEnd(), docRange.GetEnd());
   EXPECT_EQ(copyDocRangeAU.Length(), docRange.Length());

   auto indexRange = indexRangeAU.GetRange();
   EXPECT_EQ(indexRange.mSource, indexRangeAU.mSource);
   EXPECT_EQ(indexRange.GetBegin(), indexRangeAU.GetBegin());
   EXPECT_EQ(indexRange.GetEnd(), indexRangeAU.GetEnd());
   EXPECT_EQ(indexRange.Length(), indexRangeAU.Length());
}
