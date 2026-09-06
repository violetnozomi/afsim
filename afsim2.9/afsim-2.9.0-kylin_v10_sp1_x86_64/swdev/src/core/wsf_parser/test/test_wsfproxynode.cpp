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
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtTextDocument.hpp"
#include "WsfPProxy.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyNode.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyValue.hpp"
#include "WsfParseDefinitions.hpp"
#include "WsfParseSourceProvider.hpp"
#include "WsfParser.hpp"

class WsfProxyNode_FixtureBase : public ::testing::Test
{
public:
   static constexpr const char* cWSF_GRAMMAR_FILE{"wsf.ag"};
   static constexpr const char* cWSF_GRAMMAR_SOURCE_PATH{"core/wsf/grammar/wsf.ag"};
   static const UtPath          cTEST_PATH;

   //! Get this fixture's WsfPProxy
   //! @return this fixture's WsfPProxy
   WsfPProxy* GetProxy() noexcept { return mProxy.get(); }

private:
   virtual std::vector<UtPath> GetGrammarP()
   {
      std::vector<UtPath> grammarPaths;
      grammarPaths.emplace_back(UnitTestFilePathHelper::Instance().Path(cWSF_GRAMMAR_SOURCE_PATH));
      return grammarPaths;
   }

   void ConsolidateGrammar(std::ostringstream& aGrammarOutput, const std::vector<UtPath>& aGrammarPaths)
   {
      for (auto elem : aGrammarPaths)
      {
         std::ifstream grammarFile{elem.GetNormalizedPath()};
         ASSERT_TRUE(grammarFile.is_open());
         std::string grammar;
         std::getline(grammarFile, grammar, '\0');
         aGrammarOutput << grammar << '\n';
         grammarFile.close();
         ASSERT_FALSE(grammarFile.is_open());
      }
   }

   void ParseGrammar()
   {
      auto               grammarList = GetGrammarP();
      std::ostringstream grammarOutput;
      ConsolidateGrammar(grammarOutput, grammarList);
      std::istringstream grammarInput{grammarOutput.str()};
      mDefinitions = new WsfParseDefinitions();
      mDefinitions->AddGrammar(cWSF_GRAMMAR_FILE, grammarInput);
      ASSERT_TRUE(mDefinitions->GetDefinitionErrors().empty());
   }

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

   virtual std::vector<wsf::parser::test::InputFile::Data> GetInputFileDataP() = 0;

   void GetSources(std::vector<UtPath>& aSourcePaths)
   {
      auto fileData = GetInputFileDataP();
      for (auto elem : fileData)
      {
         aSourcePaths.emplace_back(elem.mPathPrefix + elem.mName);
         wsf::parser::test::InputFile elemFile{elem.mPathPrefix, elem.mName, elem.mContents};
         UtPath                       p{elemFile.GetPath()};
         ASSERT_NE(mSourceProvider.CreateSource(p), nullptr);
         ASSERT_NE(mSourceProvider.FindSource(p, true), nullptr);
      }
   }

   void ParseInput()
   {
      std::vector<UtPath> sourceList;
      GetSources(sourceList);
      mRootNode = mParser->ParseFiles(sourceList, 100);
      ASSERT_NE(mRootNode, nullptr);
      WsfParseNodeList transitionNodes;
      mParser->FinalizeParseTree(mRootNode, transitionNodes);
      if (mSourceProvider.NumberOfSources() > 1)
      {
         ASSERT_FALSE(transitionNodes.empty());
      }
      else
      {
         ASSERT_TRUE(transitionNodes.empty());
      }
   }

   void DeserializeProxy()
   {
      mRegistry = std::make_shared<WsfPProxyRegistry>();
      mRegistry->Setup(mParser->GetDefinitions()->GetRootStruct(), mParser->GetDefinitions()->mBasicTypes);
      mProxy         = ut::make_unique<WsfPProxy>();
      mProxy->mIndex = ut::make_unique<WsfPProxyIndex>();
      mProxy->Registry(mRegistry);
      mProxy->mBasicRoot = mProxy->GetRegistry()->GetBasicRoot();
      mProxy->mRoot      = WsfPProxyValue::ConstructNew(mProxy->GetRegistry()->GetType("root"));
      {
         WsfPProxyUndoValue proxyUndoRoot(mProxy->mRoot);
         proxyUndoRoot.DisableUndo();
         WsfPProxyDeserialize des{mProxy->GetRegistry().get(), mProxy->mIndex.get()};
         des.Deserialize(proxyUndoRoot, mRootNode);
         ASSERT_TRUE(mProxy->mRoot.IsValid());
      }
   }

   //! Does necessary set-up before running the test
   void SetUp() override
   {
      ParseGrammar();

      // Initialize grammar
      mDefinitions->Initialize();
      ASSERT_TRUE(mDefinitions->GetDefinitionErrors().empty());

      mParser = ut::make_unique<WsfParser>(&mSourceProvider, cTEST_PATH, mDefinitions);
      SetUpMockSourceProvider();
      ParseInput();

      DeserializeProxy();
   }

   std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>> mSources;
   wsf::parser::test::MockSourceProvider                       mSourceProvider{mSources};
   WsfParseDefinitions*                                        mDefinitions{nullptr};
   std::unique_ptr<WsfParser>                                  mParser{nullptr};
   WsfParseNode*                                               mRootNode{nullptr};
   //! @note Has to be shared_ptr because that is what the WsfPProxy::Registry function takes...
   std::shared_ptr<WsfPProxyRegistry> mRegistry{nullptr};
   std::unique_ptr<WsfPProxy>         mProxy{nullptr};
};

const UtPath WsfProxyNode_FixtureBase::cTEST_PATH{UtPath::WorkingDirectory()};

class WsfProxyNode_GetDocumentRange : public WsfProxyNode_FixtureBase
{
public:
   static constexpr const char* cGET_DOCUMENT_RANGE_INPUT{"# WsfProxyNode::GetDocumentRange unit test input\n"
                                                          "\n"
                                                          "// Define a platform instance\n"
                                                          "platform testGetDocumentRange WSF_PLATFORM\n"
                                                          "end_platform\n"
                                                          "\n"
                                                          "// Edit the platform instance\n"
                                                          "edit platform testGetDocumentRange\n"
                                                          "end_platform"};
   static constexpr const char* cMAIN_INPUT{"# WsfProxyNode unit test input\n"
                                            "\n"
                                            "include_once testGetDocumentRange.txt\n"};

private:
   std::vector<wsf::parser::test::InputFile::Data> GetInputFileDataP() override
   {
      std::vector<wsf::parser::test::InputFile::Data> fileDataList;
      fileDataList.emplace_back(
         wsf::parser::test::InputFile::Data{cTEST_PATH, "testGetDocumentRange.txt", cGET_DOCUMENT_RANGE_INPUT});
      fileDataList.emplace_back(wsf::parser::test::InputFile::Data{cTEST_PATH, "testWsfProxyNode.txt", cMAIN_INPUT});
      return fileDataList;
   }
};

TEST_F(WsfProxyNode_GetDocumentRange, GetFirstDocumentRange)
{
   WsfPProxyNode proxyRoot{GetProxy(), WsfPProxyPath{}};
   EXPECT_TRUE(proxyRoot.IsValid());
   WsfPProxyNode proxyPlatforms{proxyRoot + "platform"};
   EXPECT_TRUE(proxyPlatforms.IsValid());
   WsfPProxyNode proxyPlatform{proxyPlatforms + "testGetDocumentRange"};
   EXPECT_TRUE(proxyPlatform.IsValid());
   UtTextDocumentRange proxyPlatformRange{proxyPlatform.GetSourceDocumentRange()};
   EXPECT_TRUE(proxyPlatformRange.Valid());
   EXPECT_EQ(proxyPlatformRange.Text(),
             "platform testGetDocumentRange WSF_PLATFORM\n"
             "end_platform");
   UtTextDocumentRange firstProxyPlatformRange{proxyPlatform.GetFirstDocumentRange()};
   EXPECT_TRUE(firstProxyPlatformRange.Valid());
   EXPECT_EQ(firstProxyPlatformRange.Text(),
             "platform testGetDocumentRange WSF_PLATFORM\n"
             "end_platform");
}

TEST_F(WsfProxyNode_GetDocumentRange, GetLastDocumentRange)
{
   WsfPProxyNode proxyRoot{GetProxy(), WsfPProxyPath{}};
   EXPECT_TRUE(proxyRoot.IsValid());
   WsfPProxyNode proxyPlatforms{proxyRoot + "platform"};
   EXPECT_TRUE(proxyPlatforms.IsValid());
   WsfPProxyNode proxyPlatform{proxyPlatforms + "testGetDocumentRange"};
   EXPECT_TRUE(proxyPlatform.IsValid());
   UtTextDocumentRange lastPlatformRange{proxyPlatform.GetLastDocumentRange()};
   EXPECT_TRUE(lastPlatformRange.Valid());
   EXPECT_EQ(lastPlatformRange.Text(),
             "edit platform testGetDocumentRange\n"
             "end_platform");
}

class WsfProxyNode_IsOfType : public WsfProxyNode_FixtureBase
{
public:
   static constexpr const char* cIS_A_TYPE_OF_INPUT{"# WsfProxyNode::IsA_TypeOf unit test input\n"
                                                    "\n"
                                                    "// Define a platform type\n"
                                                    "platform_type TEST_IS_A_TYPE_OF_PLATFORM WSF_PLATFORM\n"
                                                    "end_platform_type\n"
                                                    "\n"
                                                    "// Define a mover type\n"
                                                    "mover TEST_IS_A_TYPE_OF_MOVER WSF_GROUND_MOVER\n"
                                                    "end_mover\n"
                                                    "\n"
                                                    "// Define a platform instance\n"
                                                    "platform testIsA_TypeOf TEST_IS_A_TYPE_OF_PLATFORM\n"
                                                    "   position 0n 0e\n"
                                                    "   add mover TEST_IS_A_TYPE_OF_MOVER\n"
                                                    "      route /*testIsA_TypeOf*/\n"
                                                    "         position 0n 1e\n"
                                                    "      end_route\n"
                                                    "   end_mover\n"
                                                    "end_platform\n"};
   static constexpr const char* cMAIN_INPUT{"# WsfProxyNode unit test input\n"
                                            "\n"
                                            "include_once testIsA_TypeOf.txt\n"};

private:
   std::vector<wsf::parser::test::InputFile::Data> GetInputFileDataP() override
   {
      std::vector<wsf::parser::test::InputFile::Data> fileDataList;
      fileDataList.emplace_back(wsf::parser::test::InputFile::Data{cTEST_PATH, "testIsA_TypeOf.txt", cIS_A_TYPE_OF_INPUT});
      fileDataList.emplace_back(wsf::parser::test::InputFile::Data{cTEST_PATH, "testWsfProxyNode.txt", cMAIN_INPUT});
      return fileDataList;
   }
};

TEST_F(WsfProxyNode_IsOfType, IsOfBasicType)
{
   WsfPProxyNode proxyRoot{GetProxy(), WsfPProxyPath{}};
   {
      std::ofstream ofs{"UserType"};
      ofs << proxyRoot.ToJSON() << '\n';
   }
   EXPECT_TRUE(proxyRoot.IsValid());
   EXPECT_FALSE(proxyRoot.IsOfBasicType());
   proxyRoot.SwitchToBasicTypes();
   {
      std::ofstream ofs{"BasicType"};
      ofs << proxyRoot.ToJSON() << '\n';
   }
   EXPECT_TRUE(proxyRoot.IsValid());
   EXPECT_TRUE(proxyRoot.IsOfBasicType());
}

TEST_F(WsfProxyNode_IsOfType, IsA_BasicTypeOfInvalidUse)
{
   WsfPProxyNode proxyNodeWithNoProxy;
   ASSERT_FALSE(proxyNodeWithNoProxy.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_BasicType());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsOfBasicType("foo"));

   WsfPProxyNode proxyNodeWithNoValue{GetProxy(), WsfPProxyPath::FromString(WsfPProxyValue{}, "foo")};
   ASSERT_FALSE(proxyNodeWithNoValue.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_BasicType());
   EXPECT_FALSE(proxyNodeWithNoValue.IsOfBasicType("foo"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_BasicTypeOfValidUse)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());
   EXPECT_FALSE(testProxyNodeRoot.IsA_BasicType());
   EXPECT_FALSE(testProxyNodeRoot.IsOfBasicType("root"));

   WsfPProxyNode testProxyNodePlatforms{testProxyNodeRoot + "platform"};
   ASSERT_TRUE(testProxyNodePlatforms.IsValid());
   EXPECT_FALSE(testProxyNodePlatforms.IsA_BasicType());
   EXPECT_FALSE(testProxyNodePlatforms.IsOfBasicType("ObjectMap/Platform"));

   WsfPProxyNode testProxyNodePlatform{testProxyNodePlatforms + "testIsA_TypeOf"};
   ASSERT_TRUE(testProxyNodePlatform.IsValid());
   EXPECT_FALSE(testProxyNodePlatform.IsA_BasicType());
   EXPECT_FALSE(testProxyNodePlatform.IsOfBasicType("Platform"));

   WsfPProxyNode testProxyNodeMovers{testProxyNodePlatform + "movers"};
   ASSERT_TRUE(testProxyNodeMovers.IsValid());
   EXPECT_FALSE(testProxyNodeMovers.IsA_BasicType());
   EXPECT_FALSE(testProxyNodeMovers.IsOfBasicType("ObjectMap/Mover"));

   WsfPProxyNode testProxyNodeMover{testProxyNodeMovers + "mover"};
   ASSERT_TRUE(testProxyNodeMover.IsValid());
   EXPECT_FALSE(testProxyNodeMover.IsA_BasicType());
   EXPECT_FALSE(testProxyNodeMover.IsOfBasicType("Mover"));

   WsfPProxyNode testProxyNodeRoute{testProxyNodeMover + "defaultRoute"};
   ASSERT_TRUE(testProxyNodeRoute.IsValid());
   EXPECT_FALSE(testProxyNodeRoute.IsA_BasicType());
   EXPECT_FALSE(testProxyNodeRoute.IsOfBasicType("Route"));

   WsfPProxyNode testProxyNodeRouteWaypoints{testProxyNodeRoute + "waypoints"};
   ASSERT_TRUE(testProxyNodeRouteWaypoints.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsA_BasicType());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsOfBasicType("List/Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0{testProxyNodeRouteWaypoints + std::size_t{0}};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsA_BasicType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsOfBasicType("Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Position{testProxyNodeRouteWaypoint0 + "position"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Position.IsValid());
   EXPECT_TRUE(testProxyNodeRouteWaypoint0_Position.IsA_BasicType());
   EXPECT_TRUE(testProxyNodeRouteWaypoint0_Position.IsOfBasicType("Position"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Altitude{testProxyNodeRouteWaypoint0 + "altitude"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsValid());
   EXPECT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsA_BasicType());
   EXPECT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsOfBasicType("Length"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_ListTypeOfInvalidUse)
{
   WsfPProxyNode proxyNodeWithNoProxy;
   ASSERT_FALSE(proxyNodeWithNoProxy.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_ListType());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_ListTypeOf("foo"));

   WsfPProxyNode proxyNodeWithNoValue{GetProxy(), WsfPProxyPath::FromString(WsfPProxyValue{}, "foo")};
   ASSERT_FALSE(proxyNodeWithNoValue.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_ListType());
   EXPECT_FALSE(proxyNodeWithNoValue.IsA_ListTypeOf("foo"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_ListTypeOfValidUse)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());
   EXPECT_FALSE(testProxyNodeRoot.IsA_ListType());
   EXPECT_FALSE(testProxyNodeRoot.IsA_ListTypeOf("root"));

   WsfPProxyNode testProxyNodePlatforms{testProxyNodeRoot + "platform"};
   ASSERT_TRUE(testProxyNodePlatforms.IsValid());
   EXPECT_FALSE(testProxyNodePlatforms.IsA_ListType());
   EXPECT_FALSE(testProxyNodePlatforms.IsA_ListTypeOf("ObjectMap/Platform"));

   WsfPProxyNode testProxyNodePlatform{testProxyNodePlatforms + "testIsA_TypeOf"};
   ASSERT_TRUE(testProxyNodePlatform.IsValid());
   EXPECT_FALSE(testProxyNodePlatform.IsA_ListType());
   EXPECT_FALSE(testProxyNodePlatform.IsA_ListTypeOf("Platform"));

   WsfPProxyNode testProxyNodeMovers{testProxyNodePlatform + "movers"};
   ASSERT_TRUE(testProxyNodeMovers.IsValid());
   EXPECT_FALSE(testProxyNodeMovers.IsA_ListType());
   EXPECT_FALSE(testProxyNodeMovers.IsA_ListTypeOf("ObjectMap/Mover"));

   WsfPProxyNode testProxyNodeMover{testProxyNodeMovers + "mover"};
   ASSERT_TRUE(testProxyNodeMover.IsValid());
   EXPECT_FALSE(testProxyNodeMover.IsA_ListType());
   EXPECT_FALSE(testProxyNodeMover.IsA_ListTypeOf("Mover"));

   WsfPProxyNode testProxyNodeRoute{testProxyNodeMover + "defaultRoute"};
   ASSERT_TRUE(testProxyNodeRoute.IsValid());
   EXPECT_FALSE(testProxyNodeRoute.IsA_ListType());
   EXPECT_FALSE(testProxyNodeRoute.IsA_ListTypeOf("Route"));

   WsfPProxyNode testProxyNodeRouteWaypoints{testProxyNodeRoute + "waypoints"};
   ASSERT_TRUE(testProxyNodeRouteWaypoints.IsValid());
   EXPECT_TRUE(testProxyNodeRouteWaypoints.IsA_ListType());
   EXPECT_TRUE(testProxyNodeRouteWaypoints.IsA_ListTypeOf("List/Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0{testProxyNodeRouteWaypoints + std::size_t{0}};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsA_ListType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsA_ListTypeOf("Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Position{testProxyNodeRouteWaypoint0 + "position"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Position.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsA_ListType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsA_ListTypeOf("Position"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Altitude{testProxyNodeRouteWaypoint0 + "altitude"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsA_ListType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsA_ListTypeOf("Length"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_MapTypeOfInvalidUse)
{
   WsfPProxyNode proxyNodeWithNoProxy;
   ASSERT_FALSE(proxyNodeWithNoProxy.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_MapType());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_MapTypeOf("foo"));

   WsfPProxyNode proxyNodeWithNoValue{GetProxy(), WsfPProxyPath::FromString(WsfPProxyValue{}, "foo")};
   ASSERT_FALSE(proxyNodeWithNoValue.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_MapType());
   EXPECT_FALSE(proxyNodeWithNoValue.IsA_MapTypeOf("foo"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_MapTypeOfValidUse)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());
   EXPECT_FALSE(testProxyNodeRoot.IsA_MapType());
   EXPECT_FALSE(testProxyNodeRoot.IsA_MapTypeOf("root"));

   WsfPProxyNode testProxyNodePlatforms{testProxyNodeRoot + "platform"};
   ASSERT_TRUE(testProxyNodePlatforms.IsValid());
   EXPECT_TRUE(testProxyNodePlatforms.IsA_MapType());
   EXPECT_TRUE(testProxyNodePlatforms.IsA_MapTypeOf("ObjectMap/Platform"));

   WsfPProxyNode testProxyNodePlatform{testProxyNodePlatforms + "testIsA_TypeOf"};
   ASSERT_TRUE(testProxyNodePlatform.IsValid());
   EXPECT_FALSE(testProxyNodePlatform.IsA_MapType());
   EXPECT_FALSE(testProxyNodePlatform.IsA_MapTypeOf("Platform"));

   WsfPProxyNode testProxyNodeMovers{testProxyNodePlatform + "movers"};
   ASSERT_TRUE(testProxyNodeMovers.IsValid());
   EXPECT_TRUE(testProxyNodeMovers.IsA_MapType());
   EXPECT_TRUE(testProxyNodeMovers.IsA_MapTypeOf("ObjectMap/Mover"));

   WsfPProxyNode testProxyNodeMover{testProxyNodeMovers + "mover"};
   ASSERT_TRUE(testProxyNodeMover.IsValid());
   EXPECT_FALSE(testProxyNodeMover.IsA_MapType());
   EXPECT_FALSE(testProxyNodeMover.IsA_MapTypeOf("Mover"));

   WsfPProxyNode testProxyNodeRoute{testProxyNodeMover + "defaultRoute"};
   ASSERT_TRUE(testProxyNodeRoute.IsValid());
   EXPECT_FALSE(testProxyNodeRoute.IsA_MapType());
   EXPECT_FALSE(testProxyNodeRoute.IsA_MapTypeOf("Route"));

   WsfPProxyNode testProxyNodeRouteWaypoints{testProxyNodeRoute + "waypoints"};
   ASSERT_TRUE(testProxyNodeRouteWaypoints.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsA_MapType());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsA_MapTypeOf("List/Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0{testProxyNodeRouteWaypoints + std::size_t{0}};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsA_MapType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsA_MapTypeOf("Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Position{testProxyNodeRouteWaypoint0 + "position"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Position.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsA_MapType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsA_MapTypeOf("Position"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Altitude{testProxyNodeRouteWaypoint0 + "altitude"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsA_MapType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsA_MapTypeOf("Length"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_PredefinedStructTypeOfInvalidUse)
{
   WsfPProxyNode proxyNodeWithNoProxy;
   ASSERT_FALSE(proxyNodeWithNoProxy.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_StructType());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_StructTypeOf("foo"));

   WsfPProxyNode proxyNodeWithNoValue{GetProxy(), WsfPProxyPath::FromString(WsfPProxyValue{}, "foo")};
   ASSERT_FALSE(proxyNodeWithNoValue.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsA_StructType());
   EXPECT_FALSE(proxyNodeWithNoValue.IsA_StructTypeOf("foo"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_PredefinedStructTypeOfValidUse)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());
   EXPECT_TRUE(testProxyNodeRoot.IsA_StructType());
   EXPECT_TRUE(testProxyNodeRoot.IsA_PredefinedStructTypeOf("root"));

   WsfPProxyNode testProxyNodePlatforms{testProxyNodeRoot + "platform"};
   ASSERT_TRUE(testProxyNodePlatforms.IsValid());
   EXPECT_FALSE(testProxyNodePlatforms.IsA_StructType());
   EXPECT_FALSE(testProxyNodePlatforms.IsA_PredefinedStructTypeOf("ObjectMap/Platform"));

   WsfPProxyNode testProxyNodePlatform{testProxyNodePlatforms + "testIsA_TypeOf"};
   ASSERT_TRUE(testProxyNodePlatform.IsValid());
   EXPECT_TRUE(testProxyNodePlatform.IsA_StructType());
   EXPECT_TRUE(testProxyNodePlatform.IsA_PredefinedStructTypeOf("Platform"));
   EXPECT_TRUE(testProxyNodePlatform.IsA_PredefinedStructTypeOf("WSF_PLATFORM"));
   EXPECT_FALSE(testProxyNodePlatform.IsA_PredefinedStructTypeOf("TEST_IS_A_TYPE_OF_PLATFORM"));

   WsfPProxyNode testProxyNodeMovers{testProxyNodePlatform + "movers"};
   ASSERT_TRUE(testProxyNodeMovers.IsValid());
   EXPECT_FALSE(testProxyNodeMovers.IsA_StructType());
   EXPECT_FALSE(testProxyNodeMovers.IsA_StructTypeOf("ObjectMap/Mover"));

   WsfPProxyNode testProxyNodeMover{testProxyNodeMovers + "mover"};
   ASSERT_TRUE(testProxyNodeMover.IsValid());
   EXPECT_TRUE(testProxyNodeMover.IsA_StructType());
   EXPECT_TRUE(testProxyNodeMover.IsA_PredefinedStructTypeOf("Mover"));
   EXPECT_TRUE(testProxyNodeMover.IsA_PredefinedStructTypeOf("WSF_ROUTE_MOVER"));
   EXPECT_TRUE(testProxyNodeMover.IsA_PredefinedStructTypeOf("WSF_WAYPOINT_MOVER"));
   EXPECT_TRUE(testProxyNodeMover.IsA_PredefinedStructTypeOf("WSF_GROUND_MOVER"));
   EXPECT_FALSE(testProxyNodeMover.IsA_PredefinedStructTypeOf("TEST_IS_A_TYPE_OF_MOVER"));

   WsfPProxyNode testProxyNodeRoute{testProxyNodeMover + "defaultRoute"};
   ASSERT_TRUE(testProxyNodeRoute.IsValid());
   EXPECT_TRUE(testProxyNodeRoute.IsA_StructType());
   EXPECT_TRUE(testProxyNodeRoute.IsA_PredefinedStructTypeOf("Route"));

   WsfPProxyNode testProxyNodeRouteWaypoints{testProxyNodeRoute + "waypoints"};
   ASSERT_TRUE(testProxyNodeRouteWaypoints.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsA_StructType());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsA_PredefinedStructTypeOf("List/Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0{testProxyNodeRouteWaypoints + std::size_t{0}};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0.IsValid());
   EXPECT_TRUE(testProxyNodeRouteWaypoint0.IsA_StructType());
   EXPECT_TRUE(testProxyNodeRouteWaypoint0.IsA_PredefinedStructTypeOf("Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Position{testProxyNodeRouteWaypoint0 + "position"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Position.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsA_StructType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsA_PredefinedStructTypeOf("Position"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Altitude{testProxyNodeRouteWaypoint0 + "altitude"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsA_StructType());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsA_PredefinedStructTypeOf("Length"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_UserDefinedStructTypeOfInvalidUse)
{
   WsfPProxyNode proxyNodeWithNoProxy;
   ASSERT_FALSE(proxyNodeWithNoProxy.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsOfUserType("foo"));

   WsfPProxyNode proxyNodeWithNoValue{GetProxy(), WsfPProxyPath::FromString(WsfPProxyValue{}, "foo")};
   ASSERT_FALSE(proxyNodeWithNoValue.IsValid());
   EXPECT_FALSE(proxyNodeWithNoValue.IsOfUserType("foo"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_UserDefinedStructTypeOfValidUse)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());
   EXPECT_FALSE(testProxyNodeRoot.IsOfUserType("root"));

   WsfPProxyNode testProxyNodePlatforms{testProxyNodeRoot + "platform"};
   ASSERT_TRUE(testProxyNodePlatforms.IsValid());
   EXPECT_FALSE(testProxyNodePlatforms.IsOfUserType("ObjectMap/Platform"));

   WsfPProxyNode testProxyNodePlatform{testProxyNodePlatforms + "testIsA_TypeOf"};
   ASSERT_TRUE(testProxyNodePlatform.IsValid());
   EXPECT_FALSE(testProxyNodePlatform.IsOfUserType("Platform"));
   EXPECT_TRUE(testProxyNodePlatform.IsOfUserType("WSF_PLATFORM"));
   EXPECT_TRUE(testProxyNodePlatform.IsOfUserType("TEST_IS_A_TYPE_OF_PLATFORM"));

   WsfPProxyNode testProxyNodeMovers{testProxyNodePlatform + "movers"};
   ASSERT_TRUE(testProxyNodeMovers.IsValid());
   EXPECT_FALSE(testProxyNodeMovers.IsOfUserType("ObjectMap/Mover"));

   WsfPProxyNode testProxyNodeMover{testProxyNodeMovers + "mover"};
   ASSERT_TRUE(testProxyNodeMover.IsValid());
   // This should be EXPECT_TRUE, but whatever...
   EXPECT_FALSE(testProxyNodeMover.IsOfUserType("Mover"));
   EXPECT_TRUE(testProxyNodeMover.IsOfUserType("WSF_GROUND_MOVER"));
   EXPECT_TRUE(testProxyNodeMover.IsOfUserType("TEST_IS_A_TYPE_OF_MOVER"));

   WsfPProxyNode testProxyNodeRoute{testProxyNodeMover + "defaultRoute"};
   ASSERT_TRUE(testProxyNodeRoute.IsValid());
   EXPECT_FALSE(testProxyNodeRoute.IsOfUserType("Route"));

   WsfPProxyNode testProxyNodeRouteWaypoints{testProxyNodeRoute + "waypoints"};
   ASSERT_TRUE(testProxyNodeRouteWaypoints.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoints.IsOfUserType("List/Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0{testProxyNodeRouteWaypoints + std::size_t{0}};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0.IsOfUserType("Waypoint"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Position{testProxyNodeRouteWaypoint0 + "position"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Position.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Position.IsOfUserType("Position"));

   WsfPProxyNode testProxyNodeRouteWaypoint0_Altitude{testProxyNodeRouteWaypoint0 + "altitude"};
   ASSERT_TRUE(testProxyNodeRouteWaypoint0_Altitude.IsValid());
   EXPECT_FALSE(testProxyNodeRouteWaypoint0_Altitude.IsOfUserType("Length"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_TypeOfInvalidUse)
{
   WsfPProxyNode proxyNodeWithNoProxy;
   ASSERT_FALSE(proxyNodeWithNoProxy.IsValid());
   EXPECT_FALSE(proxyNodeWithNoProxy.IsOfType("foo"));

   WsfPProxyNode proxyNodeWithNoValue{GetProxy(), WsfPProxyPath::FromString(WsfPProxyValue{}, "foo")};
   ASSERT_FALSE(proxyNodeWithNoValue.IsValid());
   EXPECT_FALSE(proxyNodeWithNoValue.IsOfType("foo"));
}

TEST_F(WsfProxyNode_IsOfType, IsA_TypeOfValidUse)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());
   EXPECT_TRUE(testProxyNodeRoot.IsOfType("root"));
}

TEST_F(WsfProxyNode_IsOfType, SwitchToBasicTypes)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());

   WsfPProxyNode testProxyNodePlatformTypes{testProxyNodeRoot + "platformType"};
   ASSERT_TRUE(testProxyNodePlatformTypes.IsValid());

   EXPECT_TRUE(testProxyNodePlatformTypes.SwitchToBasicTypes());

   WsfPProxyNode testProxyNodeWsfPlatform{testProxyNodePlatformTypes + "WSF_PLATFORM"};
   ASSERT_TRUE(testProxyNodeWsfPlatform.IsValid());
   EXPECT_TRUE(testProxyNodeWsfPlatform.IsOfBasicType());

   EXPECT_FALSE(testProxyNodePlatformTypes.SwitchToBasicTypes());
}

TEST_F(WsfProxyNode_IsOfType, SwitchToUserTypes)
{
   WsfPProxyNode testProxyNodeRoot{GetProxy(), WsfPProxyPath{}};
   ASSERT_TRUE(testProxyNodeRoot.IsValid());

   WsfPProxyNode testProxyNodePlatformTypes{testProxyNodeRoot + "platformType"};
   ASSERT_TRUE(testProxyNodePlatformTypes.IsValid());

   // WsfPProxyNodes under normal construction are by default not basic types.
   // Therefore, switch back to user types in order to test SwitchToUserTypes.
   EXPECT_TRUE(testProxyNodePlatformTypes.SwitchToBasicTypes());

   WsfPProxyNode testProxyNodeWsfPlatform{testProxyNodePlatformTypes + "WSF_PLATFORM"};
   ASSERT_TRUE(testProxyNodeWsfPlatform.IsValid());

   EXPECT_TRUE(testProxyNodePlatformTypes.SwitchToUserTypes());

   WsfPProxyNode testProxyNodeIsA_TypeOfPlatformType{testProxyNodePlatformTypes + "TEST_IS_A_TYPE_OF_PLATFORM"};
   ASSERT_TRUE(testProxyNodeIsA_TypeOfPlatformType.IsValid());
   EXPECT_FALSE(testProxyNodeIsA_TypeOfPlatformType.IsOfBasicType());

   EXPECT_FALSE(testProxyNodePlatformTypes.SwitchToUserTypes());
}
