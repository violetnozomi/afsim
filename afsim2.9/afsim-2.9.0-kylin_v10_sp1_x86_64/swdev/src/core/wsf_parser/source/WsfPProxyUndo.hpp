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

#ifndef WSFPPROXYUNDO_HPP
#define WSFPPROXYUNDO_HPP

#include "wsf_parser_export.h"

#include <cassert>
#include <vector>

#include "UtReferenceCount.hpp"
#include "WsfPProxyPath.hpp"
#include "WsfPProxyValue.hpp"
#include "WsfParseType.hpp"

// Provides feedback when a proxy object is modified
class WSF_PARSER_EXPORT WsfPProxyDeserializeObserver
{
public:
   virtual ~WsfPProxyDeserializeObserver() {}

   virtual void BeforeCopy(const WsfPProxyPath& aCopyFrom) = 0;

   virtual void BeforeAssignment(const WsfPProxyPath& aPath, WsfPProxyValue& aNewValue)                           = 0;
   virtual void BeforeMapInsert(const WsfPProxyPath& aMapPath, const std::string& aMapKey, WsfPProxyValue aValue) = 0;

   virtual void BeforeMapDelete(const WsfPProxyPath& aMapPath, const std::string& aMapKey) = 0;

   virtual void BeforeListPush(const WsfPProxyPath& aListPath) = 0;

   virtual void AfterLoad(const WsfPProxyPath& aLoadPath) = 0;
};

class WsfPProxyUndo_ActionAssign;
class WsfPProxyUndo_ActionMapOp;
class WsfPProxyUndo_ActionListOp;

// Implements an undo system for the proxy.
// Changes to the proxy can be reverted to a previous state
class WSF_PARSER_EXPORT WsfPProxyUndo
{
public:
   friend class WsfPProxyUndo_ActionAssign;
   friend class WsfPProxyUndo_ActionMapOp;
   friend class WsfPProxyUndo_ActionListOp;

   WsfPProxyUndo(WsfPProxyValue aRoot)
      : mRoot(aRoot)
      , mKeepUndoData(true)
   {
   }
   WsfPProxyUndo(const WsfPProxyUndo&) = delete;
   WsfPProxyUndo& operator=(const WsfPProxyUndo&) = delete;

   ~WsfPProxyUndo() { Commit(); }
   class ActionBase
   {
   public:
      virtual ~ActionBase() {}
      virtual void Undo(WsfPProxyUndo* aThis) = 0;
   };

   size_t GetHistorySize() { return mActions.size(); }

   void Rollback(size_t aHistoryIndex);
   void Commit(size_t aHistoryIndex = 0);
   bool Assign(const WsfPProxyPath& aPath, WsfPProxyValue aNewValue);
   bool MapInsert(const WsfPProxyPath& aMapPath, const std::string& aMapKey, WsfPProxyValue aValue);
   bool MapDelete(const WsfPProxyPath& aMapPath, const std::string& aMapKey);
   bool ListPush(const WsfPProxyPath& aListPath);

   const WsfPProxyValue& Root() const { return mRoot; }

   bool SetUnset(const WsfPProxyPath& aAttributePath, bool aIsUnset = true);


   void DisableUndo() { mKeepUndoData = false; }

private:
   void PushAction(std::unique_ptr<ActionBase> aAction);

   std::vector<std::unique_ptr<ActionBase>> mActions;
   WsfPProxyValue                           mRoot;
   bool                                     mKeepUndoData;
   UT_MEMORY_DEBUG_MARKER(cMDB_PARSE_UNDO);
};

// Wraps a WsfPProxyValue object so that changes may be undone
class WSF_PARSER_EXPORT WsfPProxyUndoValue
{
public:
   WsfPProxyUndoValue()
      : mUndoPtr(nullptr)
      , mReferenceCount(nullptr)
   {
   }
   explicit WsfPProxyUndoValue(WsfPProxyValue aValue)
   {
      mUndoPtr        = new WsfPProxyUndo(aValue);
      mReferenceCount = new UtReferenceCount(1);
   }
   WsfPProxyUndoValue(const WsfPProxyUndoValue& aSrc)
      : mPath(aSrc.mPath)
      , mUndoPtr(aSrc.mUndoPtr)
      , mReferenceCount(aSrc.mReferenceCount)
   {
      if (mReferenceCount)
      {
         mReferenceCount->AddStrongRef();
         assert(mUndoPtr);
      }
      else
      {
         assert(!mUndoPtr);
      }
   }
   void DisableUndo()
   {
      if (mUndoPtr)
      {
         mUndoPtr->DisableUndo();
      }
   }
   ~WsfPProxyUndoValue() { Cleanup(); }
   WsfPProxyUndoValue& operator=(const WsfPProxyUndoValue& aRhs)
   {
      if (this == &aRhs)
      {
         return *this;
      }
      Cleanup();
      mPath           = aRhs.mPath;
      mUndoPtr        = aRhs.mUndoPtr;
      mReferenceCount = aRhs.mReferenceCount;
      if (mReferenceCount)
      {
         mReferenceCount->AddStrongRef();
      }
      return *this;
   }

   //! Assigns a new value.  aNewValue's memory is now maintained by WsfPProxyUndo and should not be used directly.
   bool Assign(WsfPProxyValue aNewValue) { return mUndoPtr->Assign(mPath, aNewValue); }

   //! Inserts a new value into a map.
   //! aValue's memory is now maintained by WsfPProxyUndo and should not be used directly.
   bool MapInsert(const std::string& aMapKey, WsfPProxyValue aValue)
   {
      return mUndoPtr ? mUndoPtr->MapInsert(mPath, aMapKey, aValue) : false;
   }
   bool MapDelete(const std::string& aMapKey) { return mUndoPtr ? mUndoPtr->MapDelete(mPath, aMapKey) : false; }
   bool ListPush() { return mUndoPtr ? mUndoPtr->ListPush(mPath) : false; }
   bool MoveToAttr(size_t aIndex)
   {
      if (*this)
      {
         mPath += aIndex;
         return true;
      }
      return false;
   }
   bool MoveToAttr(const std::string& aKey)
   {
      bool ok = mPath.Push(Get(), aKey);
      if (!ok)
      {
         Cleanup();
      }
      return ok;
   }
   bool MoveToAttr(const WsfPProxyKey& aKey)
   {
      if (aKey.IsString())
      {
         return MoveToAttr(aKey.GetMapKey());
      }
      else
      {
         return MoveToAttr(aKey.GetIndex());
      }
   }
   bool MoveToAttr(const WsfPProxyPath& aPath)
   {
      for (size_t i = 0; i < aPath.size(); ++i)
      {
         if (!MoveToAttr(aPath[i]))
         {
            return false;
         }
      }
      return true;
   }

   bool MoveToAttr(const WsfParseTypePath& aLookupPath)
   {
      bool ok = true;
      for (size_t i = 0; i < aLookupPath.size() && ok; ++i)
      {
         const std::string& addr = aLookupPath[i].Get();
         ok                      = MoveToAttr(addr);
      }
      return ok;
   }

   WsfPProxyUndoValue GetAttr(const WsfParseTypePath& aLookupPath) const
   {
      WsfPProxyUndoValue rval = *this;
      if (rval.MoveToAttr(aLookupPath))
      {
         return rval;
      }
      return WsfPProxyUndoValue();
   }

   bool               SetUnset(bool aIsUnset) { return mUndoPtr ? mUndoPtr->SetUnset(mPath, aIsUnset) : false; }
   WsfPProxyUndoValue GetAttr(const std::string& aKey) const
   {
      WsfPProxyUndoValue rval(*this);
      rval.MoveToAttr(aKey);
      return rval;
   }
   WsfPProxyUndoValue GetAttr(size_t aIndex) const
   {
      WsfPProxyUndoValue rval(*this);
      rval.MoveToAttr(aIndex);
      return rval;
   }
   WsfPProxyUndoValue GetAttr(const WsfPProxyPath& aPath) const
   {
      WsfPProxyUndoValue rval(*this);
      rval.MoveToAttr(aPath);
      return rval;
   }
   void               MoveToParent() { mPath.Pop(); }
   WsfPProxyUndoValue GetParent() const
   {
      WsfPProxyUndoValue rval = *this;
      rval.MoveToParent();
      return rval;
   }

   WsfPProxyValue Get() const
   {
      if (mUndoPtr)
      {
         return mUndoPtr->Root().Lookup(mPath);
      }
      return WsfPProxyValue();
   }
   bool Exists() const { return (bool)Get(); }
   void Rollback()
   {
      if (mUndoPtr)
      {
         mUndoPtr->Rollback(0);
      }
   }
   void Rollback(size_t aHistoryIndex)
   {
      if (aHistoryIndex <= GetHistorySize())
      {
         if (mUndoPtr)
         {
            mUndoPtr->Rollback(aHistoryIndex);
         }
      }
      else
      {
         assert(!"INVALID INDEX");
      }
   }
   void Commit(size_t aHistoryIndex = 0)
   {
      assert(mUndoPtr != nullptr);
      mUndoPtr->Commit(aHistoryIndex);
   }
   size_t      GetHistorySize() const { return mUndoPtr ? mUndoPtr->GetHistorySize() : 0; }
               operator bool() const { return mUndoPtr != nullptr; }
   std::string DebugAddr() const;

   const WsfPProxyPath& GetPath() const { return mPath; }

   WsfPProxyUndo* GetUndo() const { return mUndoPtr; }

protected:
   void Cleanup()
   {
      if (mReferenceCount)
      {
         if (mReferenceCount->RemoveStrongRef())
         {
            mUndoPtr->Commit();
            delete mUndoPtr;
         }
      }
      mUndoPtr        = nullptr;
      mReferenceCount = nullptr;
   }
   WsfPProxyPath     mPath;
   WsfPProxyUndo*    mUndoPtr;
   UtReferenceCount* mReferenceCount;
};

#endif
