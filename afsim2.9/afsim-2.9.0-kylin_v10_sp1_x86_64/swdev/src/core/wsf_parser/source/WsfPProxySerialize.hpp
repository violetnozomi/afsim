// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPPROXYSERIALIZE_HPP
#define WSFPPROXYSERIALIZE_HPP

#include "wsf_parser_export.h"

#include <string>
class WsfPProxyNode;
#include "WsfPProxyFileWriter.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyRuleTrace.hpp"
#include "WsfPProxyUndo.hpp"

class WsfPProxyRegistry;
class WsfPProxyDeserializeTracer;

class WSF_PARSER_EXPORT WsfPProxySerialize
{
public:
   WsfPProxySerialize();
   ~WsfPProxySerialize();
   void Initialize(WsfPProxyRegistry* aProxyRegistry, WsfParser* aParserPtr, const std::vector<UtPath>& aFiles);

   void SetRenameMap(const std::map<WsfPProxyPath, std::string>& aRenameMap) { mRenameMap = aRenameMap; }
   void SetSaveFiles(const std::map<WsfPProxyPath, std::string>& aSaveFiles);
   void PreSerialize(std::vector<UtTextDocument*>& aChangedFiles);
   void Serialize(WsfPProxyValue aNewValue);

   void GetChangedFiles(std::vector<UtTextDocument*>& aTextDocumentList);
   void Finalize();

private:
   WsfPProxyNode Lookup(const WsfPProxyNode& aNode, const WsfParseTypeName& aTypeName, WsfParseNode* aParseNodePtr);

   //      bool SalvageCommand(const WsfPProxyPath& aValuePath, WsfParseNode* aNodePtr);

   using RenameMap = std::map<WsfPProxyPath, std::string>;
   RenameMap mRenameMap;

   WsfPProxyDeserializeTracer* mDeserializeTracer;
   WsfPParseRuleTracer*        mTracer;
   WsfPProxyIndex*             mProxyIndex;
   // WsfPProxyUndo*             mOldProxy;
   WsfPProxyUndoValue mOldProxy;
   // WsfPProxyValue             mOldProxy;
   WsfPProxyValue mNewProxy;
   WsfParser*     mParserPtr;

   WsfPProxyRegistry*  mProxyRegistry;
   WsfPProxyFileWriter mFileWriter;
   std::vector<UtPath> mFiles;
};
#endif
