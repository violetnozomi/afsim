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

#ifndef WSFPARSETYPELOADRULE_HPP
#define WSFPARSETYPELOADRULE_HPP

#include "UtTextDocument.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseType.hpp"
#include "WsfParseTypeInfoData.hpp"
class WsfParseIndex;

class WsfParseTypeLoadRule : public WsfParseRule
{
public:
   using Operation = WsfParseTypeInfoData::Operation;

   WsfParseTypeLoadRule(WsfParseRuleMemory& aMem, Operation aOperation, bool aAllowReplace = false);

   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;

   void Undo(WsfParser& aParser) override;
   void Resolve(WsfParser& aParser, WsfParseNode* aNodePtr) override;

   Operation               GetOperation() const { return mOperation; }
   const WsfParseTypeName& GetSaveType() const { return mSaveType; }
   const WsfParseTypeName& GetLoadType() const { return mLoadType; }
   const WsfParseTypePath& GetBackupType() const { return mBackupLoadType; }

   bool CreateTypePath(WsfParseTypePath&       aPath,
                       WsfParser&              aParser,
                       WsfParseIndex&          aIndex,
                       const WsfParseTypeName& aKey,
                       UtTextDocumentRange&    aInputRange);

   WsfParseTypeName mSaveType;
   WsfParseTypeName mLoadType;
   WsfParseTypePath mBackupLoadType;

protected:
   UtStringRef   GenerateName(WsfParser& aParser, bool aIsConflict);
   WsfParseType* FindLoadType(WsfParser& aParser, WsfParseIndex& aIndex, const WsfParseTypePath& aKey, bool aNested);

   WsfParseTypeInfoData* CreateAuxData(const WsfParseTypePath* aSaveType, const WsfParseTypePath* aLoadType);
   void                  SaveTypeToNode(WsfParseType* aTypePtr, WsfParseNode* aNode);
   UtStringRef           GetLoadType(WsfParser& aParser, const WsfParseTypeName& aLoadType);
   UtStringRef           GetSaveType(WsfParser& aParser);

   Operation mOperation;

public:
   bool mAllowRedefinition;
   bool mGenerateNameInConflict;

private:
   bool                mPushedType;
   bool                mPushedNestedType;
   UtTextDocumentRange mNameRange;
   UtTextDocumentRange mTypeRange;
};

#endif
