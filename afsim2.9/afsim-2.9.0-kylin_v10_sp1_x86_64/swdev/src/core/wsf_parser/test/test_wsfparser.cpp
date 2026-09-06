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

#include <fstream>
#include <unordered_map>

#include <gtest/gtest.h>

#include "MockSourceProvider.hpp"
#include "TestInputFile.hpp"
#include "UnitTestFileHelper.h"
#include "UtPath.hpp"
#include "UtTextDocument.hpp"
#include "WsfParseSourceInclude.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParser.hpp"

//! Test fixture to test WsfParser::ReadWord
class WsfParser_ReadWord : public ::testing::Test
{
public:
   static constexpr const char* cWSF_GRAMMAR_FILE{"wsf.ag"};
   static constexpr const char* cWSF_GRAMMAR_SOURCE_PATH{"core/wsf/grammar/wsf.ag"};
   static const UtPath          cTEST_PATH;

   static constexpr const char* cEMPTY_FILE_INPUT{// Empty file (for include purposes)
                                                  ""};
   static constexpr const char* cINVALID_INPUT{// The WsfParser will treat any white-space-delimited ASCII token as a
                                               // valid range. So, the only invalid ASCII input would be white-space.
                                               " "};
   static constexpr const char* cNUMBER_SIGN_LINE_COMMENT_INPUT{
      "# This is a line comment delimited by the number sign.\n"};
   static constexpr const char* cSLASH_LINE_COMMENT_INPUT{
      "// This is a line comment delimited by two forward-slashes.\n"};
   static constexpr const char* cBLOCK_COMMENT_INPUT{
      "/*This is a block comment.\n"
      "The comment starts with a forward-slash followed by an asterisk.\n"
      "The comment ends with an asterisk followed by a forward-slash.*/\n"};
   static constexpr const char* cPREPROCESSOR_DEFINE_INPUT{"$define HELLO_WORLD Hello World!\n"};
   static constexpr const char* cPREPROCESSOR_VARIABLE_INPUT{"$<HELLO_WORLD>$\n"};
   static constexpr const char* cINCLUDE_ONCE_EMPTY_FILE_INPUT{"include_once testEmptyFile.txt\n"};
   static constexpr const char* cINCLUDE_ONCE_INPUT{"include_once testValidInput.txt\n"};
   static constexpr const char* cINCLUDE_EMPTY_FILE_INPUT{"include testEmptyFile.txt\n"
                                                          "include testEmptyFile.txt\n"};
   static constexpr const char* cINCLUDE_INPUT{"include testValidInput.txt\n"
                                               "include testValidInput.txt\n"};
   static constexpr const char* cVALID_INPUT{// This is valid in the eyes of the WsfParser (the lexer).
                                             // This may not be valid in the eyes of the WsfParseRules (the parser).
                                             "WsfParser\n"};

   //! Get this fixture's WsfParser
   //! @return this fixture's WsfParser
   WsfParser* GetParser() noexcept { return mParser.get(); }
   //! Get this fixture's WsfParseSourceProvider
   //! @return this fixture's WsfParseSourceProvider
   wsf::parser::test::MockSourceProvider& GetSourceProvider() noexcept { return mSourceProvider; }

private:
   void SetUpMockSourceProvider()
   {
      EXPECT_CALL(mSourceProvider, CreateSource(::testing::A<const UtPath&>()))
         .Times(::testing::AnyNumber())
         .WillRepeatedly(::testing::Invoke([this](const UtPath& aPath) -> UtTextDocument*
                                           { return mSourceProvider.SourceProvider::CreateSource(aPath); }));
      EXPECT_CALL(mSourceProvider, FindSource(::testing::A<const UtPath&>(), ::testing::A<bool>()))
         .Times(::testing::AnyNumber())
         .WillRepeatedly(::testing::Invoke([this](const UtPath& aPath, bool aReadAccess) -> UtTextDocument*
                                           { return mSourceProvider.SourceProvider::FindSource(aPath, aReadAccess); }));
      EXPECT_CALL(mSourceProvider, NumberOfSources())
         .Times(::testing::AnyNumber())
         .WillRepeatedly(
            ::testing::Invoke([this]() -> std::size_t { return mSourceProvider.SourceProvider::NumberOfSources(); }));
   }

   //! Does necessary set-up before running the test
   void SetUp() override
   {
      UtPath        grammarFile{UnitTestFilePathHelper::Instance().Path(cWSF_GRAMMAR_SOURCE_PATH)};
      std::ifstream grammarStream{grammarFile.GetNormalizedPath()};
      ASSERT_TRUE(grammarStream.is_open());
      auto* definitions = new WsfParseDefinitions();
      definitions->AddGrammar(cWSF_GRAMMAR_FILE, grammarStream);
      ASSERT_TRUE(definitions->GetDefinitionErrors().empty());
      definitions->Initialize();
      ASSERT_TRUE(definitions->GetDefinitionErrors().empty());
      grammarStream.close();
      ASSERT_FALSE(grammarStream.is_open());
      mParser = ut::make_unique<WsfParser>(&mSourceProvider, cTEST_PATH, definitions);
      SetUpMockSourceProvider();
   }

   std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>> mSources;
   wsf::parser::test::MockSourceProvider                       mSourceProvider{mSources};
   std::unique_ptr<WsfParser>                                  mParser;
};

const UtPath WsfParser_ReadWord::cTEST_PATH{UtPath::WorkingDirectory()};

TEST_F(WsfParser_ReadWord, ReadWordInvalid)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testInvalidInput{cTEST_PATH, "testInvalidInput.txt", cINVALID_INPUT};
   UtPath                       testInvalidInputPath{testInvalidInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInvalidInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInvalidInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInvalidInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange invalidWord = GetParser()->ReadWord();
   ASSERT_FALSE(invalidWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordIncludeOnceEmptyFile)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testIncludeOnceEmptyFileInput{cTEST_PATH,
                                                              "testIncludeOnceEmptyFileInput.txt",
                                                              cINCLUDE_ONCE_EMPTY_FILE_INPUT};
   UtPath                       testIncludeOnceEmptyFileInputPath{testIncludeOnceEmptyFileInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testIncludeOnceEmptyFileInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testIncludeOnceEmptyFileInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   wsf::parser::test::InputFile testEmptyFileInput{cTEST_PATH, "testEmptyFileInput.txt", cEMPTY_FILE_INPUT};
   UtPath                       testEmptyFileInputPath{testEmptyFileInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testEmptyFileInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testEmptyFileInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testIncludeOnceEmptyFileInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    includeOnceWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testIncludeOnceEmptyFileInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mIncludeCount, 1);
   ASSERT_EQ(sourcePtr->mEntries.size(), 1);
   ASSERT_FALSE(includeOnceWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordIncludeOnce)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testIncludeOnceInput{cTEST_PATH, "testIncludeOnceInput.txt", cINCLUDE_ONCE_INPUT};
   UtPath                       testIncludeOnceInputPath{testIncludeOnceInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testIncludeOnceInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testIncludeOnceInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   wsf::parser::test::InputFile testValidInput{cTEST_PATH, "testValidInput.txt", cVALID_INPUT};
   UtPath                       testValidInputPath{testValidInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testValidInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testValidInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testIncludeOnceInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    includeOnceWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testValidInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mIncludeCount, 1);
   ASSERT_TRUE(sourcePtr->mEntries.empty());
   ASSERT_TRUE(includeOnceWord.Valid());
   ASSERT_EQ(includeOnceWord.Text(), "WsfParser");
}

TEST_F(WsfParser_ReadWord, ReadWordIncludeEmptyFile)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testIncludeEmptyFileInput{cTEST_PATH,
                                                          "testIncludeEmptyFileInput.txt",
                                                          cINCLUDE_EMPTY_FILE_INPUT};
   UtPath                       testIncludeEmptyFileInputPath{testIncludeEmptyFileInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testIncludeEmptyFileInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testIncludeEmptyFileInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   wsf::parser::test::InputFile testEmptyFileInput{cTEST_PATH, "testEmptyFileInput.txt", cEMPTY_FILE_INPUT};
   UtPath                       testEmptyFileInputPath{testEmptyFileInput.GetPath()};
   std::string                  testEmptyFileInputPathStr{testEmptyFileInputPath.GetNormalizedPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testEmptyFileInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testEmptyFileInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testIncludeEmptyFileInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    includeWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testIncludeEmptyFileInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mIncludeCount, 1);
   ASSERT_EQ(sourcePtr->mEntries.size(), 2);
   ASSERT_FALSE(includeWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordInclude)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testIncludeInput{cTEST_PATH, "testIncludeInput.txt", cINCLUDE_INPUT};
   UtPath                       testIncludeInputPath{testIncludeInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testIncludeInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testIncludeInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   wsf::parser::test::InputFile testValidInput{cTEST_PATH, "testValidInput.txt", cVALID_INPUT};
   UtPath                       testValidInputPath{testValidInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testValidInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testValidInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testIncludeInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    includeWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testValidInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mIncludeCount, 1);
   ASSERT_TRUE(sourcePtr->mEntries.empty());
   ASSERT_TRUE(includeWord.Valid());
   ASSERT_EQ(includeWord.Text(), "WsfParser");
}

TEST_F(WsfParser_ReadWord, ReadWordPreprocessorDefine)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testPreprocessorDefineInput{cTEST_PATH,
                                                            "testPreprocessorDefineInput.txt",
                                                            cPREPROCESSOR_DEFINE_INPUT};
   UtPath                       testPreprocessorDefineInputPath{testPreprocessorDefineInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testPreprocessorDefineInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testPreprocessorDefineInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testPreprocessorDefineInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    preprocessorDefineWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testPreprocessorDefineInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mEntries.size(), 1);
   ASSERT_FALSE(preprocessorDefineWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordPreprocessorVariable)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testPreprocessorVariableInput{cTEST_PATH,
                                                              "testPreprocessorVariableInput.txt",
                                                              cPREPROCESSOR_VARIABLE_INPUT};
   UtPath                       testPreprocessorVariableInputPath{testPreprocessorVariableInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testPreprocessorVariableInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testPreprocessorVariableInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testPreprocessorVariableInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    preprocessorVariableWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testPreprocessorVariableInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mEntries.size(), 1);
   ASSERT_FALSE(preprocessorVariableWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordNumberSignLineComment)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testNumberSignLineCommentInput{cTEST_PATH,
                                                               "testNumberSignLineCommentInput.txt",
                                                               cNUMBER_SIGN_LINE_COMMENT_INPUT};
   UtPath                       testNumberSignLineCommentInputPath{testNumberSignLineCommentInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testNumberSignLineCommentInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testNumberSignLineCommentInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testNumberSignLineCommentInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    numberSignLineCommentWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testNumberSignLineCommentInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mEntries.size(), 1);
   ASSERT_FALSE(numberSignLineCommentWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordSlashLineComment)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testSlashLineCommentInput{cTEST_PATH,
                                                          "testSlashLineCommentInput.txt",
                                                          cSLASH_LINE_COMMENT_INPUT};
   UtPath                       testSlashLineCommentInputPath{testSlashLineCommentInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testSlashLineCommentInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testSlashLineCommentInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testSlashLineCommentInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    slashLineCommentWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testSlashLineCommentInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mEntries.size(), 1);
   ASSERT_FALSE(slashLineCommentWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordBlockComment)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testBlockCommentInput{cTEST_PATH, "testBlockCommentInput.txt", cBLOCK_COMMENT_INPUT};
   UtPath                       testBlockCommentInputPath{testBlockCommentInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testBlockCommentInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testBlockCommentInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testBlockCommentInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange    blockCommentWord = GetParser()->ReadWord();
   WsfParseSourceInclude* sourcePtr{GetParser()->GetCurrentInclude()};
   ASSERT_NE(sourcePtr, nullptr);
   {
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testBlockCommentInputPath, true);
      ASSERT_EQ(sourcePtr->mSourcePtr, foundInputDocPtr);
   }
   ASSERT_EQ(sourcePtr->mEntries.size(), 1);
   ASSERT_FALSE(blockCommentWord.Valid());
}

TEST_F(WsfParser_ReadWord, ReadWordValid)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   wsf::parser::test::InputFile testValidInput{cTEST_PATH, "testValidInput.txt", cVALID_INPUT};
   UtPath                       testValidInputPath{testValidInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testValidInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testValidInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testValidInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange validWord = GetParser()->ReadWord();
   ASSERT_TRUE(validWord.Valid());
   ASSERT_EQ(validWord.Text(), "WsfParser");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_Null)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string            cINPUT{"a\0b", 3};
   wsf::parser::test::InputFile testInput{cTEST_PATH, "testInput.txt", cINPUT};
   UtPath                       testInputPath{testInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_LineFeed)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string            cINPUT{"a\fb", 3};
   wsf::parser::test::InputFile testInput{cTEST_PATH, "testInput.txt", cINPUT};
   UtPath                       testInputPath{testInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_Newline)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string            cINPUT{"a\nb", 3};
   wsf::parser::test::InputFile testInput{cTEST_PATH, "testInput.txt", cINPUT};
   UtPath                       testInputPath{testInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_CarriageReturn)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string cINPUT{"a\rb", 3};
   UtPath            testInputPath{cTEST_PATH + "testInput.txt"};
   {
      // Cannot use the InputFile object because UtTextDocument will normalize line endings when reading from an input
      // stream. Instead, create a "virtual" UtTextDocument as the input file and insert the text into it.
      auto* createdInputDoc = GetSourceProvider().CreateVirtualSource(testInputPath);
      ASSERT_NE(createdInputDoc, nullptr);
      auto* foundInputDoc = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDoc, nullptr);
      foundInputDoc->Insert(0, cINPUT);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_Tab)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string            cINPUT{"a\tb", 3};
   wsf::parser::test::InputFile testInput{cTEST_PATH, "testInput.txt", cINPUT};
   UtPath                       testInputPath{testInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_VerticalTab)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string            cINPUT{"a\vb", 3};
   wsf::parser::test::InputFile testInput{cTEST_PATH, "testInput.txt", cINPUT};
   UtPath                       testInputPath{testInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}

TEST_F(WsfParser_ReadWord, ReadWordWhitespace_NormalSpace)
{
   GetParser()->Reset();
   GetParser()->SetWorkingDirectory(cTEST_PATH);
   const std::string            cINPUT{"a b", 3};
   wsf::parser::test::InputFile testInput{cTEST_PATH, "testInput.txt", cINPUT};
   UtPath                       testInputPath{testInput.GetPath()};
   {
      auto* createdInputDocPtr = GetSourceProvider().CreateSource(testInputPath);
      ASSERT_NE(createdInputDocPtr, nullptr);
      auto* foundInputDocPtr = GetSourceProvider().FindSource(testInputPath, true);
      ASSERT_NE(foundInputDocPtr, nullptr);
   }
   GetParser()->PushSource(testInputPath.GetNormalizedPath(), true);
   UtTextDocumentRange word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "a");
   word = GetParser()->ReadWord();
   ASSERT_TRUE(word.Valid());
   ASSERT_EQ(word.Text(), "b");
}
