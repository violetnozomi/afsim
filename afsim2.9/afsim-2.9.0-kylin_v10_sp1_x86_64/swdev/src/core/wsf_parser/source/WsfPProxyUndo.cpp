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

#include "WsfPProxyUndo.hpp"

#include "UtMemory.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyPath.hpp"

class WsfPProxyUndo_ActionAssign : public WsfPProxyUndo::ActionBase
{
public:
   WsfPProxyPath  mPath;
   WsfPProxyValue mOldValue;
   ~WsfPProxyUndo_ActionAssign() override { mOldValue.Delete(); }
   void Undo(WsfPProxyUndo* aThis) override
   {
      if (mPath.Empty())
      {
         aThis->mRoot.Copy(mOldValue);
         mOldValue.Delete();
      }
      else
      {
         WsfPProxyValue base = aThis->Root().Lookup(mPath, mPath.size() - 1);
         if (base)
         {
            base.SwapAttr(mPath.Back(), mOldValue);
            mOldValue.Delete();
         }
      }
   }
};

class WsfPProxyUndo_ActionMapOp : public WsfPProxyUndo::ActionBase
{
public:
   enum RestoreStep
   {
      cREMOVE_KEY,
      cREPLACE_OLD_VALUE
   };
   ~WsfPProxyUndo_ActionMapOp() override { mOldValue.Delete(); }
   void Undo(WsfPProxyUndo* aThis) override
   {
      WsfPProxyValue      mapVal = aThis->Root().Lookup(mMapPath);
      WsfPProxyObjectMap* mapPtr = mapVal.GetObjectMap();
      if (mapPtr)
      {
         if (mRestoreStep == cREMOVE_KEY)
         {
            mapPtr->Erase(mKey);
         }
         else if (mRestoreStep == cREPLACE_OLD_VALUE)
         {
            WsfPProxyValue& valInMap = mapPtr->GetValues()[mKey];
            valInMap.Swap(mOldValue);
            mapPtr->UserUndelete(mKey);
            mOldValue.Delete();
         }
      }
   }

   WsfPProxyPath  mMapPath;
   std::string    mKey;
   WsfPProxyValue mOldValue;
   RestoreStep    mRestoreStep;
};

class WsfPProxyUndo_ActionListOp : public WsfPProxyUndo::ActionBase
{
public:
   enum RestoreStep
   {
      cPOP_BACK,
      cRESTORE_AT
   };
   ~WsfPProxyUndo_ActionListOp() override { mOldValue.Delete(); }
   void Undo(WsfPProxyUndo* aThis) override
   {
      WsfPProxyValue listVal = aThis->Root().Lookup(mListPath);
      WsfPProxyList* listPtr = listVal.GetList();
      if (listPtr)
      {
         if (mRestoreStep == cPOP_BACK)
         {
            listPtr->PopBack();
         }
         else if (mRestoreStep == cRESTORE_AT)
         {
            WsfPProxyValue valInList = listPtr->Get(mIndex);
            valInList.Delete();
            listPtr->mValues[mIndex] = mOldValue.GetDataPtr();
            mOldValue                = WsfPProxyValue();
         }
      }
   }
   WsfPProxyPath  mListPath;
   size_t         mIndex;
   WsfPProxyValue mOldValue;
   RestoreStep    mRestoreStep;
};

bool WsfPProxyUndo::MapInsert(const WsfPProxyPath& aMapPath, const std::string& aMapKey, WsfPProxyValue aValue)
{
   WsfPProxyValue      mapVal = mRoot.Lookup(aMapPath);
   WsfPProxyObjectMap* mapPtr = mapVal.GetObjectMap();
   if (mapPtr)
   {
      assert(aValue.GetType()->IsTypeOf(mapPtr->GetDataType()));
      if (mKeepUndoData)
      {
         auto action      = ut::make_unique<WsfPProxyUndo_ActionMapOp>();
         action->mMapPath = aMapPath;
         action->mKey     = aMapKey;
         auto iter        = mapPtr->GetValues().find(aMapKey);
         if (iter != mapPtr->GetValues().end())
         {
            // action->mRestoreStep = WsfPProxyUndo_ActionMapOp::cREPLACE_OLD_VALUE;
            // iter->second.Swap(aValue);
            action->mRestoreStep = WsfPProxyUndo_ActionMapOp::cREPLACE_OLD_VALUE;
            action->mOldValue    = aValue;
            iter->second.Swap(action->mOldValue);
         }
         else
         {
            action->mRestoreStep = WsfPProxyUndo_ActionMapOp::cREMOVE_KEY;
            mapPtr->AddNew(aMapKey, aValue);
         }
         PushAction(std::move(action));
      }
      else
      {
         WsfPProxyValue& val = mapPtr->GetValues()[aMapKey];
         val.Delete();
         val = aValue;
      }
      return true;
   }
   return false;
}

bool WsfPProxyUndo::MapDelete(const WsfPProxyPath& aMapPath, const std::string& aMapKey)
{
   WsfPProxyValue      mapVal = mRoot.Lookup(aMapPath);
   WsfPProxyObjectMap* mapPtr = mapVal.GetObjectMap();
   if (mapPtr)
   {
      WsfPProxyValue val = mapPtr->GetAt(aMapKey);
      if (val)
      {
         if (mKeepUndoData)
         {
            auto action          = ut::make_unique<WsfPProxyUndo_ActionMapOp>();
            action->mMapPath     = aMapPath;
            action->mOldValue    = val;
            action->mKey         = aMapKey;
            action->mRestoreStep = WsfPProxyUndo_ActionMapOp::cREPLACE_OLD_VALUE;
            PushAction(std::move(action));
            // mapPtr->Erase(aMapKey);
            mapPtr->UserEraseNoDelete(aMapKey);
            return true;
         }
         else
         {
            mapPtr->UserErase(aMapKey);
            // val.Delete();
            return true;
         }
      }
      else
      {
         return false;
      }
   }
   return false;
}

bool WsfPProxyUndo::ListPush(const WsfPProxyPath& aListPath)
{
   WsfPProxyValue listVal = mRoot.Lookup(aListPath);
   WsfPProxyList* listPtr = listVal.GetList();
   if (listPtr)
   {
      if (mKeepUndoData)
      {
         auto action          = ut::make_unique<WsfPProxyUndo_ActionListOp>();
         action->mListPath    = aListPath;
         action->mRestoreStep = WsfPProxyUndo_ActionListOp::cPOP_BACK;
         PushAction(std::move(action));
         listPtr->PushBack();
         return true;
      }
      else
      {
         listPtr->PushBack();
         return true;
      }
   }
   return false;
}

bool WsfPProxyUndo::Assign(const WsfPProxyPath& aPath, WsfPProxyValue aNewValue)
{
   if (aPath.Empty())
   {
      if (mKeepUndoData)
      {
         auto action       = ut::make_unique<WsfPProxyUndo_ActionAssign>();
         action->mOldValue = mRoot.Copy();
         PushAction(std::move(action));
      }
      mRoot.Copy(aNewValue);
      aNewValue.Delete();
   }
   else
   {
      WsfPProxyValue base = mRoot.Lookup(aPath, aPath.size() - 1);
      if (base)
      {
         if (base.SwapAttr(aPath.Back(), aNewValue))
         {
            if (mKeepUndoData)
            {
               auto action       = ut::make_unique<WsfPProxyUndo_ActionAssign>();
               action->mPath     = aPath;
               action->mOldValue = aNewValue;
               PushAction(std::move(action));
               return true;
            }
            else
            {
               aNewValue.Delete();
               return true;
            }
         }
      }
   }
   return false;
}
//
// bool WsfPProxyUndo::Assign(const WsfPProxyPath& aPath,
//                            const std::string&   aNewValue,
//                            WsfPProxyBasicValue& aNewProxyValue)
// {
//    WsfPProxyValue base = mRoot.Lookup(aPath, (int)aPath.size() - 1);
//    if (base)
//    {
//       WsfPProxyValue current = base.GetAttr(aPath.Back());
//       if (current.IsBasicType())
//       {
//          WsfPProxyBasicValue newVal(current.GetType()->ConstructNew());
//          newVal.SetValue(aNewValue);
//          //if (mObserverPtr) mObserverPtr->BeforeAssignment(aPath, newVal);
//          if (base.SwapAttr(aPath.Back(), newVal))
//          {
//             if (mKeepUndoData)
//             {
//                WsfPProxyUndo_ActionAssign* action = new WsfPProxyUndo_ActionAssign;
//                action->mPath = aPath;
//                action->mOldValue = aNewProxyValue;
//                newVal = WsfPProxyValue();
//                aNewProxyValue = base.GetAttr(aPath.Back());
//                PushAction(action);
//                return true;
//             }
//             else
//             {
//                aNewProxyValue = base.GetAttr(aPath.Back());
//                return true;
//             }
//          }
//       }
//    }
//    return false;
// }
void WsfPProxyUndo::PushAction(std::unique_ptr<ActionBase> aAction)
{
   assert(mKeepUndoData);
   mActions.push_back(std::move(aAction));
}

void WsfPProxyUndo::Commit(size_t aHistoryIndex)
{
   while (mActions.size() > aHistoryIndex)
   {
      mActions.pop_back();
   }
}

void WsfPProxyUndo::Rollback(size_t aHistoryIndex)
{
   while (mActions.size() > aHistoryIndex)
   {
      mActions.back()->Undo(this);
      mActions.pop_back();
   }
}
class ActionToggleUnset : public WsfPProxyUndo::ActionBase
{
public:
   WsfPProxyPath mPath;
   ~ActionToggleUnset() override {}
   void Undo(WsfPProxyUndo* aThis) override
   {
      WsfPProxyValue val = aThis->Root().Lookup(mPath);
      if (val)
      {
         if (val.IsUnset())
         {
            val.ClearUnset();
         }
         else
         {
            val.SetUnset();
         }
      }
   }
};

bool WsfPProxyUndo::SetUnset(const WsfPProxyPath& aAttributePath, bool aIsUnset /*=true*/)
{
   WsfPProxyValue val = mRoot.Lookup(aAttributePath);
   if (val)
   {
      if (val.IsUnset() != aIsUnset)
      {
         if (aIsUnset)
         {
            val.SetUnset();
         }
         else
         {
            val.ClearUnset();
         }
         if (mKeepUndoData)
         {
            auto action   = ut::make_unique<ActionToggleUnset>();
            action->mPath = aAttributePath;
            mActions.push_back(std::move(action));
         }
      }
      return true;
   }
   return false;
}

std::string WsfPProxyUndoValue::DebugAddr() const
{
   if (mUndoPtr)
   {
      return mPath.ToString(mUndoPtr->Root());
   }
   else
   {
      return "<null>";
   }
}
