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

#include "WsfPProxyDeserializeContext.hpp"

#include "UtColor.hpp"
#include "UtLog.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfPProxyVisitor.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseTypeLoadRule.hpp"

using ReturnValue = WsfPProxyDeserializeContext::ReturnValue;

WsfPProxyDeserializeContext::WsfPProxyDeserializeContext(WsfPProxyDeserializeContext& aBase)
   : mParent(&aBase)
   , mDeserializer(aBase.mDeserializer)
   , mContainer(aBase.mContainer)
   , mSkip(aBase.mSkip)
{
}

WsfPProxyDeserializeContext::WsfPProxyDeserializeContext(WsfPProxyDeserialize*     aDeserializer,
                                                         const WsfPProxyUndoValue& aContainer)
   : mParent(nullptr)
   , mDeserializer(aDeserializer)
   , mContainer(aContainer)
   , mSkip(false)
{
}

void WsfPProxyDeserializeContext::BeginNode(WsfParseNode* aNodePtr)
{
   WsfPProxyIndex* index = mDeserializer->GetIndex();
   if (index && index->mRecordCurrentPath && mParent && aNodePtr)
   {
      index->SetCurrentProxyPath(CurrentPath(), aNodePtr);
   }
   if (mDeserializer->mTraceBuilder)
   {
      mDeserializer->mTraceBuilder->BeginNode(aNodePtr, CurrentPath());
   }
}

void WsfPProxyDeserializeContext::EndNode(WsfParseNode* aNodePtr)
{
   if (mDeserializer->mTraceBuilder)
   {
      mDeserializer->mTraceBuilder->EndNode(aNodePtr);
   }
}
WsfPProxyUndoValue WsfPProxyDeserializeContext::Lookup(const WsfParseActionAddress& aAddr,
                                                       const WsfPProxyUndoValue&    aBaseContainer)
{
   if (aAddr.mAttrIndices.empty())
   {
      return aBaseContainer;
   }
   if (aAddr.mAttrIndices[0].mType == WsfParseActionAddress::EntryType::cROOT)
   {
      WsfPProxyUndoValue rval = LookupP(aAddr, mDeserializer->GetRoot());
      if (rval.Exists())
      {
         return rval;
      }
      rval = LookupP(aAddr, BasicRoot());
      if (rval.Exists())
      {
         return rval;
      }
   }
   else
   {
      WsfPProxyUndoValue rval = LookupP(aAddr, aBaseContainer);
      if (rval.Exists())
      {
         return rval;
      }
   }
   // Lookup failed
   return WsfPProxyUndoValue();
}

WsfPProxyUndoValue WsfPProxyDeserializeContext::LookupRoot(const WsfParseTypePath& aLookupPath)
{
   WsfPProxyUndoValue rval = BasicRoot().GetAttr(aLookupPath);
   if (rval.Exists())
   {
      return rval;
   }
   rval = mDeserializer->GetRoot().GetAttr(aLookupPath);
   return rval;
}

ReturnValue WsfPProxyDeserializeContext::GetOrdValue(size_t aOrd)
{
   if (mNodesReturnValues.size() > aOrd && mNodesReturnValues[aOrd])
   {
      return mNodesReturnValues[aOrd];
   }
   WsfParseNode* parent = mCurrentNodePtr->GetParent();
   WsfParseNode* n      = parent->GetChild(aOrd);
   return NodeValue(n);
}

std::string WsfPProxyDeserializeContext::GetOrdString(size_t aOrd)
{
   WsfParseNode* parent = mCurrentNodePtr->GetParent();
   WsfParseNode* n      = parent->GetChild(aOrd);
   return NodeValue(n);
}

std::string WsfPProxyDeserializeContext::NodeValue(const WsfParseNode* aValue) const
{
   std::string rval;
   if (aValue)
   {
      const auto* last = aValue->Next(false);
      for (const auto* n = aValue; n != last; n = n->Next())
      {
         if (n->mValue.Valid())
         {
            if (!rval.empty())
            {
               rval += ' ';
            }
            rval += n->mValue.Text();
         }
      }
   }
   return rval;
}

void WsfPProxyDeserializeContext::AddToIndex(const WsfPProxyPath&      aPath,
                                             WsfParseNode*             aNodePtr,
                                             WsfPProxyIndex::EntryType aEntryType)
{
   if (mDeserializer->GetIndex())
   {
      WsfPProxyIndexNode& node = mDeserializer->GetIndex()->Get(aPath);
      node.AddLocation(aNodePtr, aEntryType);
   }
}

namespace
{
void MarkAllInherited(WsfPProxyValue aProxyValue, bool aInherited)
{
   auto markInheritedFunc = [=](WsfPProxyValue aValue, const WsfPProxyValue&)
   {
      aValue.SetInherited(aInherited);
      return true;
   };

   aProxyValue.SetInherited(aInherited);
   WsfPProxyVisitor::Visit(markInheritedFunc, aProxyValue);
}
} // namespace

ReturnValue WsfPProxyDeserializeContext::Eval(WsfParseValueType* aLHS_TypePtr, WsfParseActionPart& aExpr)
{
   switch (aExpr.mActionPartType)
   {
   case WsfParseActionPart::cNODE_REFERENCE:
   {
      ReturnValue                  rval;
      WsfParseActionNodeReference* ref = (WsfParseActionNodeReference*)&aExpr;

      if (ref->mValueTypePtr)
      {
         WsfPProxyType* typePtr = GetRegistry()->GetType(aLHS_TypePtr);
         if (typePtr && typePtr->IsBasicType())
         {
            rval             = GetOrdValue(ref->mIndex);
            rval.mProxyValue = rval.mProxyValue.Copy();
            return rval;
         }
      }

      // If LHS is string, we just take the string that exists in the input file:
      if (aLHS_TypePtr != nullptr && aLHS_TypePtr->GetTypeName() == "String")
      {
         rval.mTextValue = GetOrdString(ref->mIndex);
      }
      else
      {
         rval             = GetOrdValue(ref->mIndex);
         rval.mProxyValue = rval.mProxyValue.Copy();
      }

      return rval;
   }
   case WsfParseActionPart::cLITERAL:
   {
      ReturnValue            rval;
      WsfParseActionLiteral* lit = (WsfParseActionLiteral*)&aExpr;
      if (aLHS_TypePtr)
      {
         WsfPProxyType* lhsTypePtr = GetRegistry()->GetType(aLHS_TypePtr);
         if (lhsTypePtr && lhsTypePtr->IsBasicType())
         {
            WsfPProxyBasicType* lhsBasicType = (WsfPProxyBasicType*)lhsTypePtr;
            WsfPProxyBasicValue basicValue   = WsfPProxyValue::ConstructNew(lhsBasicType);
            if (lit->mText.empty())
            {
               basicValue.SetUnset();
            }
            else
            {
               basicValue.SetValue(lit->mText);
            }
            rval = basicValue;
         }
      }
      else
      {
         rval = lit->mText;
      }
      return rval;
   }
   case WsfParseActionPart::cPUSH:
   {
      WsfParseActionPush* pushPtr      = (WsfParseActionPush*)&aExpr;
      WsfPProxyUndoValue  newContainer = Lookup(pushPtr->mAttributeAddr, Container());
      SetContainer(newContainer);
      newContainer.SetUnset(false);
      return ReturnValue();
   }
   case WsfParseActionPart::cCOPY:
   {
      WsfParseActionCopy* copyPtr  = (WsfParseActionCopy*)&aExpr;
      WsfPProxyUndoValue  dstValue = Lookup(copyPtr->mDstAddr, mContainer);
      WsfPProxyUndoValue  srcValue = Lookup(copyPtr->mSrcAddr, mContainer);

      WsfPProxyDeserializeObserver* obs = Observer();
      if (obs)
      {
         obs->BeforeCopy(srcValue.GetPath());
      }

      WsfPProxyValue srcVal = srcValue.Get();
      if (srcVal)
      {
         // TODO: In this case, the RHS type should be verified in WsfParseDefinitions.
         WsfPProxyValue newVal = srcVal.Copy(WsfProxy::cCOPY_INHERITED);
         if (newVal.IsStruct())
         {
            WsfPProxyStructValue newInst(newVal);
            newInst.SetBase(srcValue.GetPath());
            newInst.SetInherited(true);
         }
         if (Observer())
         {
            Observer()->BeforeAssignment(dstValue.GetPath(), newVal);
         }
         dstValue.Assign(newVal);
         AddToIndex(dstValue.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cASSIGNED);
      }
      return ReturnValue();
   }
   case WsfParseActionPart::cDEBUG_PRINT:
   {
      WsfParseActionPrint* printPtr = (WsfParseActionPrint*)&aExpr;
      auto                 logger   = ut::log::debug() << "WsfPProxyDeserializeContext: Debug Print:";
      logger.AddNote() << "Message: " << printPtr->mString;
      return ReturnValue();
   }
   case WsfParseActionPart::cASSIGN:
   {
      WsfParseActionAssign* assignPtr = (WsfParseActionAssign*)&aExpr;
      WsfPProxyUndoValue    lhsValue  = Lookup(assignPtr->mLHS_Address, Container());
      if (lhsValue)
      {
         if (!assignPtr->mLHS_Address.IsEmpty() && lhsValue) // don't record assignment to temporary variables
         {
            AddToIndex(lhsValue.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cASSIGNED);
         }

         ReturnValue rhsVal = Eval(assignPtr->mLHS_Type, *assignPtr->mRHS);
         if (rhsVal.mProxyValue)
         {
            // TODO: In this case, the RHS type should be verified in WsfParseDefinitions.
            rhsVal.mProxyValue.SetInherited(false);
            if (Observer())
            {
               Observer()->BeforeAssignment(lhsValue.GetPath(), rhsVal.mProxyValue);
            }
            lhsValue.Assign(rhsVal.mProxyValue);
         }
         else
         {
            // TODO: In this case, we should be pre-processing all action assignments from string literals.
            //       So that if we type "isAlive=fulse", we can detect the error
            WsfPProxyValue lhsVal = lhsValue.Get();
            if (lhsVal.IsBasicType())
            {
               if (rhsVal.mTextValue.empty())
               {
                  WsfPProxyValue newVal = lhsVal.Copy();
                  newVal.SetUnset();
                  newVal.SetInherited(false);
                  if (Observer())
                  {
                     Observer()->BeforeAssignment(lhsValue.GetPath(), newVal);
                  }
                  lhsValue.Assign(newVal);
               }
               else
               {
                  WsfPProxyValue newVal = lhsVal.Copy();
                  WsfPProxyBasicValue(newVal).SetValue(rhsVal.mTextValue);
                  newVal.SetInherited(false);
                  if (Observer())
                  {
                     Observer()->BeforeAssignment(lhsValue.GetPath(), newVal);
                  }
                  lhsValue.Assign(newVal);
               }
            }
            else if (rhsVal.mTextValue.empty())
            {
               auto newVal = WsfPProxyValue::ConstructNew(lhsVal.GetType());
               if (Observer())
               {
                  Observer()->BeforeAssignment(lhsValue.GetPath(), newVal);
               }
               lhsValue.Assign(newVal);
            }
         }
      }

      return ReturnValue();
   }
   case WsfParseActionPart::cLIST_OP:
   {
      WsfParseActionListOp* op = (WsfParseActionListOp*)&aExpr;
      if (op->mOperation == WsfParseActionListOp::cPUSH_BACK)
      {
         WsfPProxyUndoValue listValue = Lookup(op->mAttributeAddr, Container());
         WsfPProxyValue     listVal   = listValue.Get();
         WsfPProxyList*     list      = listVal.GetList();
         if (list)
         {
            if (Observer())
            {
               Observer()->BeforeListPush(listValue.GetPath());
            }
            listValue.ListPush();
            size_t newIndex = list->Size() - 1;
            listValue.MoveToAttr(newIndex);
            if (op->mPushedValuePtr)
            {
               ReturnValue rhs = Eval(nullptr, *op->mPushedValuePtr);
               if (rhs.mProxyValue)
               {
                  if (Observer())
                  {
                     Observer()->BeforeAssignment(listValue.GetPath(), rhs.mProxyValue);
                  }
                  listValue.Assign(rhs.mProxyValue);
               }
               else
               {
                  WsfPProxyBasicValue val = WsfPProxyBasicValue(listValue.Get());
                  if (val)
                  {
                     auto newVal = WsfPProxyValue::ConstructNew(val.GetType());
                     newVal.SetValue(rhs.mTextValue);
                     if (Observer())
                     {
                        Observer()->BeforeAssignment(listValue.GetPath(), newVal);
                     }
                     listValue.Assign(newVal);
                  }
               }
            }
            AddToIndex(listValue.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cEDITED);
            SetContainer(listValue);
            listVal.SetInherited(false);
         }
      }
      return ReturnValue();
   }
   case WsfParseActionPart::cOBJECT_MAP_OP:
   {
      WsfParseActionObjectMapOp* op = (WsfParseActionObjectMapOp*)&aExpr;
      if (op->mOperation == WsfParseActionObjectMapOp::cAPPLY)
      {
         WsfParseTypeInfoData* typeInfoPtr = (WsfParseTypeInfoData*)mCurrentNodePtr->GetAuxiliaryValue();
         if (typeInfoPtr)
         {
            // For all operations, we load a type:
            WsfPProxyUndoValue loadType;
            if (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_LOAD)
            {
               loadType = mContainer.GetAttr(typeInfoPtr->mLoadKey);
            }
            else
            {
               loadType = LookupRoot(typeInfoPtr->mLoadKey);
            }

            if (Observer())
            {
               Observer()->AfterLoad(loadType.GetPath());
            }

            if (typeInfoPtr->mOperation == WsfParseTypeInfoData::cCREATE_TYPE)
            {
               WsfPProxyValue loadValue = loadType.Get();
               if (!loadValue)
               {
                  WsfParseTypeLoadRule* loadRule = (WsfParseTypeLoadRule*)mCurrentNodePtr->mRulePtr;
                  if (!loadRule->mBackupLoadType.empty())
                  {
                     WsfPProxyUndoValue backupLoadType  = LookupRoot(loadRule->mBackupLoadType);
                     WsfPProxyValue     backupLoadValue = backupLoadType.Get();
                     if (backupLoadValue)
                     {
                        loadValue = backupLoadValue;
                        loadType  = backupLoadType;
                     }
                  }
               }
               if (loadValue)
               {
                  if (Observer())
                  {
                     Observer()->BeforeCopy(loadType.GetPath());
                  }

                  WsfPProxyStructValue newValue = loadValue.Copy(WsfProxy::cCOPY_INHERITED);
                  MarkAllInherited(newValue, true);
                  WsfParseTypePath saveKey = typeInfoPtr->mSaveKey;
                  saveKey.pop_back();
                  WsfPProxyUndoValue destMap;
                  // WsfPProxyPath destMapPath = CurrentPath();
                  if (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_SAVE)
                  {
                     destMap = mContainer.GetAttr(saveKey);
                  }
                  else
                  {
                     destMap = mContainer.GetAttr(saveKey);
                  }
                  WsfPProxyValue destMapVal = destMap.Get();
                  if (destMapVal.GetObjectMap())
                  {
                     const std::string& typeName = typeInfoPtr->mSaveKey.back().Get();
                     // newValue.SetBase(destMap + typeName);
                     newValue.SetBase(loadType.GetPath());
                     if (Observer())
                     {
                        Observer()->BeforeMapInsert(destMap.GetPath(), typeName, newValue);
                     }
                     destMap.MapInsert(typeName, newValue);
                     WsfPProxyUndoValue newValuePos = destMap.GetAttr(typeName);
                     AddToIndex(newValuePos.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cADDED);
                     // TODO: what do we do if there is a failure
                     SetContainer(newValuePos);
                     // destMapVal.SetInherited(false);
                     assert(newValue.IsInherited());
                  }
                  else if (destMapVal.IsStruct())
                  {
                     const std::string& typeName = typeInfoPtr->mSaveKey.back().Get();
                     WsfPProxyUndoValue destAttr = destMap.GetAttr(typeName);
                     if (destAttr.Exists())
                     {
                        // destAttr.GetPath
                        newValue.SetBase(loadType.GetPath());
                        WsfPProxyValue newValCopy = newValue.Copy(WsfProxy::cCOPY_INHERITED);
                        if (newValCopy.IsStruct())
                        {
                           WsfPProxyStructValue(newValCopy).SetBase(loadType.GetPath());
                        }
                        if (Observer())
                        {
                           Observer()->BeforeAssignment(destAttr.GetPath(), newValCopy);
                        }
                        destAttr.Assign(newValCopy);
                        AddToIndex(destAttr.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cASSIGNED);
                        SetContainer(destAttr);
                     }
                     else
                     {
                        newValue.Delete();
                     }
                  }
                  else
                  {
                     // Silently accept failure
                     // TODO: Log errors
                     newValue.Delete();
                  }
               }
            }
            else if (typeInfoPtr->mOperation == WsfParseTypeInfoData::cLOAD_TYPE)
            {
               AddToIndex(loadType.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cEDITED);
               SetContainer(loadType);
            }
            else if (typeInfoPtr->mOperation == WsfParseTypeInfoData::cDELETE_TYPE)
            {
               WsfPProxyValue loadVal = loadType.Get();
               if (loadVal)
               {
                  WsfPProxyUndoValue mapVal = loadType.GetParent();
                  AddToIndex(loadType.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cREMOVED);
                  const std::string& key = loadType.GetPath().Back().GetMapKey();
                  if (Observer())
                  {
                     Observer()->BeforeMapDelete(mapVal.GetPath(), key);
                  }
                  mapVal.MapDelete(key);
               }
            }
            else
            {
               assert(false);
            }
         }
      }
      else // Map operation NEW and GET
      {
         WsfPProxyUndoValue  mapPos   = Lookup(op->mAttributeAddr, Container());
         WsfPProxyValue      mapValue = mapPos.Get();
         WsfPProxyObjectMap* objMap   = mapValue.GetObjectMap();
         if (objMap)
         {
            if (op->mOperation == WsfParseActionObjectMapOp::cNEW || op->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
            {
               ReturnValue objName = Eval(nullptr /*aRegistry.GetStringType()*/, *op->mObjectName);
               std::string key     = objName.mTextValue;
               if (objMap->FindAt(key) == nullptr)
               {
                  auto newValue = WsfPProxyValue::ConstructNew(objMap->GetDataType());

                  if (Observer())
                  {
                     Observer()->BeforeMapInsert(mapPos.GetPath(), key, newValue);
                  }
                  mapPos.MapInsert(key, newValue);

                  WsfPProxyUndoValue newEntry = mapPos.GetAttr(key);
                  AddToIndex(newEntry.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cADDED);
                  SetContainer(newEntry);
                  return ReturnValue();
               }
               else if (op->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
               {
                  SetContainer(mapPos.GetAttr(key));
                  return ReturnValue();
               }
               else
               {
                  return ReturnValue();
               }
            }
            else if (op->mOperation == WsfParseActionObjectMapOp::cGET)
            {
               ReturnValue objName = Eval(nullptr /*aRegistry.GetStringType()*/, *op->mObjectName);
               std::string key     = objName.mTextValue;
               mapPos.MoveToAttr(key);
               if (mapPos.Exists())
               {
                  SetContainer(mapPos);
                  AddToIndex(mapPos.GetPath(), mCurrentNodePtr, WsfPProxyIndex::cEDITED);
               }
            }
            else if (op->mOperation == WsfParseActionObjectMapOp::cDELETE)
            {
               ReturnValue objName = Eval(nullptr /*aRegistry.GetStringType()*/, *op->mObjectName);
               std::string key     = objName.mTextValue;
               if (Observer())
               {
                  Observer()->BeforeMapDelete(mapPos.GetPath(), key);
               }
               if (mapPos.MapDelete(key))
               {
                  WsfPProxyPath path = mapPos.GetPath();
                  path += key;
                  AddToIndex(path, mCurrentNodePtr, WsfPProxyIndex::cREMOVED);
               }
            }
         }
      }
      return ReturnValue();
   }
   case WsfParseActionPart::cNEGATE:
   {
      WsfParseActionNegate* neg = (WsfParseActionNegate*)&aExpr;
      ReturnValue           val = Eval(aLHS_TypePtr, *neg->mSubPart);
      if (val.mTextValue.empty() && val.mProxyValue.IsBasicType())
      {
         WsfPProxyBasicValue basicVal(val.mProxyValue);
         basicVal.Negate();
         return basicVal;
      }
      return ReturnValue();
   }
   case WsfParseActionPart::cSKIP:
      SetContainer(WsfPProxyValue());
      SetSkip(true);
      break;
   case WsfParseActionPart::cCALL:
   {
      WsfParseActionCall*      callPtr = (WsfParseActionCall*)&aExpr;
      std::vector<ReturnValue> args(callPtr->mArgs.size());
      for (size_t i = 0; i < callPtr->mArgs.size(); ++i)
      {
         args[i] = Eval(callPtr->mFunctionPtr->mSignature[i + 1], *callPtr->mArgs[i]);
      }
      if (callPtr->mFunctionName == "MakeLength")
      {
         WsfParseValueType* rType = callPtr->mFunctionPtr->mSignature[0];
         if (rType)
         {
            WsfPProxyType*      rTypePtr = GetRegistry()->GetType(rType);
            WsfPProxyBasicValue rval     = WsfPProxyValue::ConstructNew(rTypePtr);
            std::string         realVal  = args[0].ToString();
            std::string         unitVal  = args[1].ToString();
            rval.SetValue(realVal + ' ' + unitVal);
            return rval;
         }
      }
      if (callPtr->mFunctionName == "MakeAngle")
      {
         WsfParseValueType* rType = callPtr->mFunctionPtr->mSignature[0];
         if (rType)
         {
            WsfPProxyType*      rTypePtr = GetRegistry()->GetType(rType);
            WsfPProxyBasicValue rval     = WsfPProxyValue::ConstructNew(rTypePtr);
            std::string         realVal  = args[0].ToString();
            std::string         unitVal  = args[1].ToString();
            rval.SetValue(realVal + ' ' + unitVal);
            return rval;
         }
      }
      if ((callPtr->mFunctionName == "NormalizeColor3") || (callPtr->mFunctionName == "NormalizeColor4"))
      {
         auto rType = callPtr->mFunctionPtr->mSignature[0];
         if (rType)
         {
            WsfPProxyType*       rTypePtr = GetRegistry()->GetType(rType);
            WsfPProxyStructValue rval     = WsfPProxyValue::ConstructNew(rTypePtr);
            auto                 r        = static_cast<int>(std::stof(args[0].ToString()) * 255);
            auto                 g        = static_cast<int>(std::stof(args[1].ToString()) * 255);
            auto                 b        = static_cast<int>(std::stof(args[2].ToString()) * 255);
            rval.GetAttr("red").SetIntegerValue(r);
            rval.GetAttr("green").SetIntegerValue(g);
            rval.GetAttr("blue").SetIntegerValue(b);
            if (callPtr->mFunctionName == "NormalizeColor4")
            {
               auto a = static_cast<int>(std::stof(args[3].ToString()) * 255);
               rval.GetAttr("alpha").SetIntegerValue(a);
            }
            return rval;
         }
      }
      if (callPtr->mFunctionName == "HexStringToColor")
      {
         auto rType = callPtr->mFunctionPtr->mSignature[0];
         if (rType)
         {
            WsfPProxyType*       rTypePtr = GetRegistry()->GetType(rType);
            WsfPProxyStructValue rval     = WsfPProxyValue::ConstructNew(rTypePtr);
            if (UtColor::IsHexColor(args[0].ToString()))
            {
               float rgba[4] = {0.F};
               UtColor::HexStringToRGBA(args[0].ToString(), rgba);
               rval.GetAttr("red").SetIntegerValue(static_cast<int>(rgba[0] * 255));
               rval.GetAttr("green").SetIntegerValue(static_cast<int>(rgba[1] * 255));
               rval.GetAttr("blue").SetIntegerValue(static_cast<int>(rgba[2] * 255));
               rval.GetAttr("alpha").SetIntegerValue(static_cast<int>(rgba[3] * 255));
            }
            return rval;
         }
      }
   }
   break;

   default:          // we should have a case for each action part
      assert(false); // todo!
   }
   return ReturnValue();
}

bool WsfPProxyDeserializeContext::DoActions(WsfParseAction* aActions, WsfParseNode* aNodePtr)
{
   if (!aActions || IsSkip())
   {
      return false;
   }

   size_t historyPos = mContainer.GetHistorySize();

   std::vector<WsfParseActionPart*>& acts = aActions->mSubActions;
   for (WsfParseActionPart* act : acts)
   {
      ReturnValue rval = Eval(nullptr, *act);
      rval.mProxyValue.Delete();
   }
   if (aNodePtr)
   {
      if (CanContinue(aNodePtr->GetParent(), aNodePtr) == WsfPProxyDeserializeTracerI::cDA_ROLLBACK)
      {
         if (mContainer.GetUndo())
         {
            mContainer.Rollback(historyPos);
            RollbackComplete(aNodePtr->GetParent());
         }
      }
   }
   return !acts.empty();
}

WsfPProxyValue WsfPProxyDeserializeContext::CreateNodeOutput(WsfParseNode* aNodePtr)
{
   // traverse rule references to arrive at the real rule
   WsfParseRule* ctx = aNodePtr->mRulePtr;
   while (ctx)
   {
      if (ctx->Type() == WsfParseRule::cRULE_REFERENCE)
      {
         ctx = ((WsfParseRuleReference*)ctx)->GetRulePtr();
      }
      else
      {
         break;
      }
   }

   WsfPProxyValue subContainer;
   // If this node is supposed to create a value, do it now
   if (ctx)
   {
      if (ctx->IsNamedRule() && ((WsfParseNamedRule*)ctx)->mOutputTypePtr)
      {
         WsfParseNamedRule* ctxNamed = (WsfParseNamedRule*)ctx;
         WsfParseValueType* typePtr  = ctxNamed->mOutputTypePtr->GetType();
         if (typePtr)
         {
            WsfPProxyType* proxyTypePtr = GetRegistry()->GetType(typePtr);
            if (proxyTypePtr)
            {
               subContainer = WsfPProxyValue::ConstructNew(proxyTypePtr);
               // for basic values, there are no actions to set the value.  The value is taken from the node now.
               // Note: this is not entirely true.  We should probably be checking that the rule is a value rule.
               // Other rules may output basic types, but set the values with actions.
               WsfPProxyBasicValue basic(subContainer);
               if (basic)
               {
                  basic.SetValue(aNodePtr);
               }
            }
            else
            {
               assert(false); // todo: handle errors
            }
         }
         else
         {
            assert(false); // todo: handle errors
         }
      }
   }
   return subContainer;
}

bool WsfPProxyDeserializeContext::ApplyActions()
{
   bool hasActions = false;
   if (IsSkip() || IsAborting())
   {
      return false;
   }

   WsfPProxyDeserializeContext newCtx(*this);
   // This is a DFS traversal of the parse tree.
   //
   // Node structure:
   //                                   parent-node
   //                                    |   |   |
   //              +---------------------+   |   +-------------------+
   //              |                         |                       |
   //        CurrentNodePtr       -        next1       -           next2
   // [pre-action]     [CurrentNodePtr->action]   [next1->action]        [next2->action]
   //
   // Here, the current node is the first child.  We walk right to traverse siblings in this loop.
   // mContainer is initially inherited by parent-node
   // Actions may change mContainer which propagates to the next sibling's children.
   // After a node's children are done executing, the container is reset to the original value.
   WsfParseSequence* sequencePtr = nullptr;
   if (mCurrentNodePtr->GetParent())
   {
      WsfParseRule* parentRulePtr = mCurrentNodePtr->GetParent()->mRulePtr;
      if (parentRulePtr && parentRulePtr->Type() == WsfParseRule::cSEQUENCE)
      {
         sequencePtr = (WsfParseSequence*)parentRulePtr;
      }
   }
   size_t        childIndex = 0;
   WsfParseNode* n          = mCurrentNodePtr;

   if (n && (n->GetFlags() & WsfParseNode::cDELAY_LOAD) && !Deserializer().IsDelayLoading())
   {
      WsfPProxyDeserialize::DelayNode dn(((WsfParseDelayLoadData*)n->GetAuxiliaryValue())->mLoadOrder, n);
      Deserializer().mDelayNodes.push_back(dn);
      return false;
   }

   newCtx.BeginNode(mCurrentNodePtr->GetParent());

   if (sequencePtr)
   {
      while (n)
      {
         newCtx.mCurrentNodePtr = n;
         if (n->Left() == nullptr)
         {
            hasActions |= newCtx.DoActions(sequencePtr->GetActionBefore(childIndex), n);
         }
         WsfParseNode* c = n->GetChild(0);
         if (c)
         {
            newCtx.mCurrentNodePtr = c;
            // Most nodes just operate on the current container.  However, nodes that have 'output types' do not operate
            // on any inputs and instead create a value to operate on.
            // TODO: This is fairly inefficient for a couple reasons:
            //    1. Returned values may not be used in an assignment, in which case they shouldn't be created.
            //    2. Each value is created on the heap, read in, and then finally copied upon assignment.
            //       Most cases could be reduced to reading directly into the destination value.
            //       At a minimum we could eliminate heap allocations.
            WsfPProxyValue outVal = CreateNodeOutput(n);
            if (outVal)
            {
               newCtx.SetContainer(outVal);
            }
            hasActions = newCtx.ApplyActions() || hasActions;
            newCtx.mNodesReturnValues.push_back(outVal);
         }
         else
         {
            newCtx.BeginNode(n);
            newCtx.EndNode(n);
            newCtx.mNodesReturnValues.emplace_back();
         }
         newCtx.mCurrentNodePtr = n;
         /////////////////////////////
         // TODO: This is insufficient to record the current type
         ///////////////
         newCtx.SetContainer(Container());
         hasActions = newCtx.DoActions(sequencePtr->GetActionBefore(childIndex + 1), n) || hasActions;
         ++childIndex;
         n = n->Right();
      }
   }
   else
   {
      while (n)
      {
         newCtx.mCurrentNodePtr = n;
         WsfParseNode* c        = n->GetChild(0);
         if (c)
         {
            newCtx.mCurrentNodePtr = c;
            WsfPProxyValue outVal  = CreateNodeOutput(n);
            if (outVal)
            {
               newCtx.SetContainer(outVal);
               newCtx.Deserializer().AddValue(outVal);
            }
            size_t historyPos = newCtx.mContainer.GetHistorySize();
            hasActions |= newCtx.ApplyActions();
            if (CanContinue(n, c) == WsfPProxyDeserializeTracerI::cDA_ROLLBACK)
            {
               newCtx.mContainer.Rollback(historyPos);
               newCtx.RollbackComplete(n);
            }
         }
         newCtx.mCurrentNodePtr = n;
         n                      = n->Right();
      }
   }

   newCtx.EndNode(mCurrentNodePtr->GetParent());
   return hasActions;
}

WsfPProxyUndoValue WsfPProxyDeserializeContext::LookupP(const WsfParseActionAddress& aAddr,
                                                        const WsfPProxyUndoValue&    aBaseContainer)
{
   WsfPProxyUndoValue val = aBaseContainer;
   for (size_t i = 0; i < aAddr.mAttrIndices.size() && val; ++i)
   {
      const WsfParseActionAddress::Entry& e = aAddr.mAttrIndices[i];
      if (e.mType == WsfParseActionAddress::EntryType::cROOT)
      {
         continue; // Handled by caller, ignore root element.
      }
      else if (e.IsNodeIndex())
      {
         std::string lookupName = GetOrdValue(e.GetNodeIndex()).mTextValue;
         val.MoveToAttr(lookupName);
      }
      else if (e.IsString())
      {
         val.MoveToAttr(e.mName);
      }
      else
      {
         val.MoveToAttr(e.mIndex);
      }
   }
   if (val.Exists())
   {
      return val;
   }
   return WsfPProxyUndoValue();
}
