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

#include "WsfParseTypeLoadRule.hpp"

#include <sstream>

#include "WsfParseIndex.hpp"
#include "WsfParseNode.hpp"
#include "WsfParser.hpp"

WsfParseTypeLoadRule::WsfParseTypeLoadRule(WsfParseRuleMemory& aMem, Operation aOperation, bool aAllowReplace)
   : WsfParseRule(aMem, cTYPE_LOAD)
   , mOperation(aOperation)
   , mAllowRedefinition(aAllowReplace)
   , mGenerateNameInConflict(false)
{
   // Since this creates side-effects, we need to clean them up after the command is complete.
   mReaderFlags = cCAN_RESOLVE | cCAN_UNDO;

   mPushedType       = false;
   mPushedNestedType = false;
}

bool WsfParseTypeLoadRule::CreateTypePath(WsfParseTypePath&       aPath,
                                          WsfParser&              aParser,
                                          WsfParseIndex&          aIndex,
                                          const WsfParseTypeName& aKey,
                                          UtTextDocumentRange&    aInputRange)
{
   aPath = aKey.mPath;

   if (aKey.mOrdinal != ut::npos)
   {
      WsfParseNode* seq = aParser.GetCurrentSequence();
      if (seq)
      {
         seq = seq->Down();
      }

      for (size_t i = 0; i < aKey.mOrdinal; ++i)
      {
         seq = seq->Right();
      }

      if (seq && seq->mValue.Valid())
      {
         UtStringRef s;
         seq->mValue.Text(s);
         aPath.back() = s;
         aInputRange  = seq->mValue;
      }
      else
      {
         return false;
      }
   }
   if (aPath.back() == "generated_name")
   {
      aPath.back() = GenerateName(aParser, false);
   }
   return true;
}

WsfParseType* WsfParseTypeLoadRule::FindLoadType(WsfParser&              aParser,
                                                 WsfParseIndex&          aIndex,
                                                 const WsfParseTypePath& aKey,
                                                 bool                    aNested)
{
   WsfParseType* loadTypePtr = nullptr;
   if (aNested)
   {
      WsfParseType* currentType = aIndex.GetCurrentType();
      if (currentType)
      {
         loadTypePtr = currentType->FindNestedSymbol(aKey);
      }
      if (!loadTypePtr)
      {
         // We once allowed a nested lookup to try to load from the global type lists as a backup
         // I do not think this is necessary anymore, and is more trouble than its worth
         // loadTypePtr = const_cast<WsfParseType*>(aIndex.mUserTypes.FindTypeLocal(aKey));
         // if (! loadTypePtr)
         //{
         // loadTypePtr = aParser.GetDefinitions()->mBasicTypes->FindType(aKey);
         //}
      }
   }
   else
   {
      loadTypePtr = const_cast<WsfParseType*>(aIndex.mUserTypes.FindType(aKey));
      if (!loadTypePtr)
      {
         loadTypePtr = aParser.GetDefinitions()->mBasicTypes->FindType(aKey);
      }
   }
   return loadTypePtr;
}

bool WsfParseTypeLoadRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   // This is a special type of reader which does not actually read anything, but
   // only changes the state of the parser.
   mPushedType          = false;
   mPushedNestedType    = false;
   aNode                = nullptr;
   bool           match = false;
   WsfParseIndex& index = *aParser.GetParseIndex();

   WsfParseTypePath    loadKey;
   UtTextDocumentRange loadInputRange;
   if (!CreateTypePath(loadKey, aParser, index, mLoadType, loadInputRange))
   {
      return false;
   }

   if (mOperation == WsfParseTypeInfoData::cDELETE_TYPE)
   {
      WsfParseType* currentType = index.GetCurrentType();
      if (currentType && currentType->HasSymbols())
      {
         if (currentType->RemoveType(loadKey))
         {
            aNode = aParser.NewNode(this, "LoadType");
            aNode->mFlags |= WsfParseNode::cLOAD_TYPE_NODE;
            aNode->SetAuxiliaryValue(CreateAuxData(nullptr, &loadKey));
            aParser.AddAuxiliaryValue(aNode->GetAuxiliaryValue());
            match = true;
         }
      }
   }
   else
   {
      if (aParser.IsDelayLoading() && mOperation == WsfParseTypeInfoData::cCREATE_TYPE && !mLoadType.mNestedLookup &&
          loadKey.size() == 2)
      {
         aParser.Requires(loadKey[0], loadKey[1]);
      }

      WsfParseType* loadTypePtr = FindLoadType(aParser, index, loadKey, mLoadType.mNestedLookup);
      bool          typeError   = false;
      if (!loadTypePtr)
      {
         typeError   = true;
         loadTypePtr = FindLoadType(aParser, index, mBackupLoadType, false);
      }

      if (mOperation == WsfParseTypeInfoData::cCREATE_TYPE)
      {
         if (loadTypePtr)
         {
            bool duplicateNameError = false;

            UtTextDocumentRange saveInputRange;
            WsfParseTypePath    saveKey;
            CreateTypePath(saveKey, aParser, index, mSaveType, saveInputRange);

            if (!mSaveType.mNestedLookup)
            {
               const WsfParseType* foundTypePtr = index.mUserTypes.FindType(saveKey);
               bool                canDefine    = foundTypePtr == nullptr;
               if (foundTypePtr)
               {
                  if (mAllowRedefinition)
                  {
                     index.mUserTypes.RemoveType(saveKey);
                     canDefine = true;
                  }
                  else if (mGenerateNameInConflict)
                  {
                     saveKey.back()     = GenerateName(aParser, true);
                     duplicateNameError = true;
                     canDefine          = true;
                  }
               }

               if (canDefine)
               {
                  WsfParseType* newTypePtr = new WsfParseType(saveKey.back(), *loadTypePtr, nullptr);
                  newTypePtr               = &index.mUserTypes.AddType(saveKey, newTypePtr);
                  newTypePtr->SetSourceType(loadTypePtr);
                  index.LoadCurrentType(newTypePtr);
                  aNode = aParser.NewNode(this, "CreateType");
                  aNode->mFlags |= WsfParseNode::cLOAD_TYPE_NODE;
                  WsfParseTypeInfoData* auxPtr = CreateAuxData(&saveKey, &loadKey);
                  if (typeError)
                  {
                     auxPtr->mFlags |= WsfParseTypeInfoData::cINVALID_TYPE;
                     aNode->mFlags |= WsfParseNode::cERROR_INCOMPLETE;
                  }
                  if (duplicateNameError)
                  {
                     auxPtr->mFlags |= WsfParseTypeInfoData::cDUPLICATE_NAME;
                     aNode->mFlags |= WsfParseNode::cERROR_INCOMPLETE;
                  }
                  aNode->SetAuxiliaryValue(auxPtr);
                  aParser.AddAuxiliaryValue(aNode->GetAuxiliaryValue());
                  mPushedType = true;
                  mNameRange  = saveInputRange;
                  mTypeRange  = loadInputRange;
                  match       = true;
               }
            }
            else
            {
               WsfParseType* currentType = index.GetCurrentType();
               if (currentType)
               {
                  WsfParseType* tablePtr     = currentType;
                  WsfParseType* foundTypePtr = tablePtr->FindTypeLocal(saveKey);
                  bool          canDefine    = foundTypePtr == nullptr;
                  if (foundTypePtr)
                  {
                     if (mAllowRedefinition)
                     {
                        tablePtr->RemoveType(saveKey);
                        canDefine = true;
                     }
                     else if (mGenerateNameInConflict)
                     {
                        saveKey.back()     = GenerateName(aParser, true);
                        canDefine          = true;
                        duplicateNameError = true;
                     }
                  }
                  if (canDefine)
                  {
                     WsfParseType* newTypePtr = new WsfParseType(saveKey.back(), *loadTypePtr, nullptr);
                     newTypePtr               = &tablePtr->AddType(saveKey, newTypePtr);
                     newTypePtr->SetSourceType(foundTypePtr);
                     assert(newTypePtr);
                     index.LoadCurrentType(newTypePtr);
                     aNode = aParser.NewNode(this, "CreateType");
                     aNode->mFlags |= WsfParseNode::cLOAD_TYPE_NODE;
                     WsfParseTypeInfoData* auxPtr = CreateAuxData(&saveKey, &loadKey);
                     if (typeError)
                     {
                        auxPtr->mFlags |= WsfParseTypeInfoData::cINVALID_TYPE;
                        aNode->mFlags |= WsfParseNode::cERROR_INCOMPLETE;
                     }
                     if (duplicateNameError)
                     {
                        auxPtr->mFlags |= WsfParseTypeInfoData::cDUPLICATE_NAME;
                        aNode->mFlags |= WsfParseNode::cERROR_INCOMPLETE;
                     }
                     aNode->SetAuxiliaryValue(auxPtr);
                     aParser.AddAuxiliaryValue(aNode->GetAuxiliaryValue());
                     mNameRange        = saveInputRange;
                     mTypeRange        = loadInputRange;
                     mPushedType       = true;
                     mPushedNestedType = false;
                     match             = true;
                  }
               }
            }
         }
      }
      else if (mOperation == WsfParseTypeInfoData::cLOAD_TYPE)
      {
         if (loadTypePtr)
         {
            if (mLoadType.mNestedLookup)
            {
               index.LoadCurrentType(loadTypePtr);
               // WsfParseTypeRef& typeRef = index.GetCurrentType();
               // typeRef.push_back(loadTypePtr);
               mPushedNestedType = false;
            }
            else
            {
               index.LoadCurrentType(loadTypePtr);
               mPushedNestedType = false;
            }

            aNode = aParser.NewNode(this, "LoadType");
            aNode->mFlags |= WsfParseNode::cLOAD_TYPE_NODE;
            aNode->SetAuxiliaryValue(CreateAuxData(nullptr, &loadKey));
            aParser.AddAuxiliaryValue(aNode->GetAuxiliaryValue());
            mTypeRange  = loadInputRange;
            mPushedType = true;
            match       = true;
         }
      }
   }
   return match;
}

void WsfParseTypeLoadRule::Resolve(WsfParser& aParser, WsfParseNode* aNodePtr)
{
   // WsfParseTypeLoadRule is the reason Resolve exists.  Resolve is called when the command
   // is finished reading.  This methods ensures that the current type stack is cleaned up
   // after the command is done.  Additionally, nodes which mark the type name or base type
   // are flagged for IDEAS.
   if (mPushedType)
   {
      if (aNodePtr)
      {
         if (mNameRange.Valid())
         {
            WsfParseNode* child = aNodePtr->Down();
            while (child)
            {
               if (child->mValue == mNameRange)
               {
                  child->mFlags |= WsfParseNode::cTYPE_NAME_NODE;
                  break;
               }
               child = child->Right();
            }
         }
         if (mTypeRange.Valid())
         {
            WsfParseNode* child = aNodePtr->Down();
            while (child)
            {
               if (child->mValue == mTypeRange)
               {
                  child->mFlags |= WsfParseNode::cTYPE_REFERENCE_NODE;
                  break;
               }
               child = child->Right();
            }
         }
      }
      if (false /*mPushedNestedType*/)
      {
         // aParser.GetParseIndex()->GetCurrentType().pop_back();
      }
      else
      {
         aParser.GetParseIndex()->UnloadCurrentType();
      }
   }
   mPushedType        = false;
   mNameRange.mSource = nullptr;
   mTypeRange.mSource = nullptr;
}

void WsfParseTypeLoadRule::Undo(WsfParser& aParser)
{
   if (mPushedType)
   {
      if (false /*mPushedNestedType*/)
      {
         // aParser.GetParseIndex()->GetCurrentType().pop_back();
      }
      else
      {
         aParser.GetParseIndex()->UnloadCurrentType();
      }
   }
   mPushedType = false;
}

UtStringRef WsfParseTypeLoadRule::GenerateName(WsfParser& aParser, bool aIsConflict)
{
   std::stringstream ss;
   if (aIsConflict)
   {
      ss << "__conflict__" << aParser.GetParseIndex()->GenerateName();
   }
   else
   {
      // Want these names to be string-sortable
      // so fill to 5 digits
      ss << "__auto_name_";
      int nameNumber = aParser.GetParseIndex()->GenerateName();
      int div        = 100000;
      while (div)
      {
         ss << (nameNumber / div) % 10;
         div /= 10;
      }
   }
   return ss.str();
}

void WsfParseTypeLoadRule::SaveTypeToNode(WsfParseType* aTypePtr, WsfParseNode* aNode)
{
   aNode->mFlags |= WsfParseNode::cLOAD_TYPE_NODE;
}

WsfParseTypeInfoData* WsfParseTypeLoadRule::CreateAuxData(const WsfParseTypePath* aSaveType,
                                                          const WsfParseTypePath* aLoadType)
{
   WsfParseTypeInfoData* dataPtr = new WsfParseTypeInfoData(mOperation);
   if (aSaveType)
   {
      dataPtr->mSaveKey = *aSaveType;
   }
   if (aLoadType)
   {
      dataPtr->mLoadKey = *aLoadType;
   }
   if (mLoadType.mNestedLookup)
   {
      dataPtr->mFlags |= WsfParseTypeInfoData::cNESTED_LOAD;
   }
   if (mSaveType.mNestedLookup)
   {
      dataPtr->mFlags |= WsfParseTypeInfoData::cNESTED_SAVE;
   }
   return dataPtr;
}
