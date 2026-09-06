// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VACALLBACK_HPP
#define VACALLBACK_HPP

#include "vespatk_export.h"

//! A 'simplified' type-safe callback mechanism.
//!
//! This class defines templates that provides type-safe callbacks. Callback objects are registered
//! with callback lists. Connections between callback lists and registered callback objects are
//! automatically broken if either is deleted.
//!
//! The implementation is pretty funky, but drastic measures were required to
//! make it work with older compilers. Some of the stuff in here is very similar
//! to the signal/slot mechanism that is present in 'boost'.

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

#include "UtBinder.hpp"
#include "UtLog.hpp"
#include "VaObject.hpp"
#include "VaUniqueId.hpp"

class UtWallClock;

namespace vespa
{
class VaCallback;
class VaCallbackList;

//! A helper class to link the callback with the callback list.
class VESPATK_EXPORT VaCallbackLink
{
public:
   VaCallbackLink(VaCallbackList* aCallbackListPtr)
      : mCallbackListPtr(aCallbackListPtr)
   {
   }
   virtual ~VaCallbackLink() {} // To avoid warnings about virtual methods without virtual destructor
   virtual void Disconnect(VaCallback* aCallbackPtr)         = 0;
   virtual void Block(VaCallback* aCallbackPtr, bool aBlock) = 0;

protected:
   VaCallbackList* mCallbackListPtr;
};

class VESPATK_EXPORT VaCallbackGroupId
{
public:
   VaCallbackGroupId()
      : mId(0)
      , mString()
   {
   }
   VaCallbackGroupId(int aId)
      : mId(aId)
      , mString()
   {
   }
   VaCallbackGroupId(const std::string& aString)
      : mId(0)
      , mString(aString)
   {
   }
   VaCallbackGroupId(const char* aStringPtr)
      : mId(0)
      , mString(aStringPtr)
   {
   }

   ~VaCallbackGroupId() {}

   bool operator<(const VaCallbackGroupId& aRHS) const
   {
      if (mString < aRHS.mString)
         return true;
      else if (mString == aRHS.mString)
         return (mId < aRHS.mId);
      else
         return false;
   }
   operator const std::string&() const { return mString; }
   operator int() const { return mId; }

private:
   int         mId;
   std::string mString;
};


//! The non-templated base class for a callback.
//! The lowest level of the callback maintains the connection to the non-templated callback list.
//! No knowledge of the derived callback list type is required because the callback list pointer is
//! only used to invoke the Disconnect method, which doesn't require the derived type.
class VESPATK_EXPORT VaCallback : public VaUniqueId
{
public:
   friend class VaCallbackList;

   VaCallback();
   VaCallback(const VaCallback& /*aSrc*/);
   virtual ~VaCallback();

   //! Disconnect from the callback list to which this is connected.
   //! This does nothing if not currently connected.
   //!
   //! This is declared inline after VaCallbackList so it can invoke VaCallbackList::Disconnect()
   void Disconnect();

   //! Sets the callback to blocked or unblocked.
   //! Blocking the callback will temporarily prevent it from being invoked.
   //! Unblocking the callback resumes normal behavior.
   void Block(bool aBlock = true)
   {
      if (mIsBlocked != aBlock && mCallbackLinkPtr != nullptr)
      {
         mCallbackLinkPtr->Block(this, aBlock);
         mIsBlocked = aBlock;
      }
   }

   //! Unblocks the callback.  See Block().
   void Unblock() { Block(false); }

   //! Returns true if this callback is currently blocked.
   bool IsBlocked() const { return mIsBlocked; }

   void                     SetGroupId(VaCallbackGroupId aGroupId) { mGroupId = aGroupId; }
   const VaCallbackGroupId& GetGroupId() const { return mGroupId; }

   //! Returns the current group id. This is useful to subscribers to who need
   //! to know which callback group signaled them.
   static const VaCallbackGroupId& GetCurrentGroupId() { return sCurrentGroupId; }

   void SetDisconnectFlag(bool aRemoveFlag) { mDisconnectFlag = aRemoveFlag; }
   bool GetDisconnectFlag() const { return mDisconnectFlag; }

   void SetDeleteFlag(bool aDeleteFlag) { mDeleteFlag = aDeleteFlag; }
   bool GetDeleteFlag() const { return mDeleteFlag; }

   static VaCallbackGroupId sCurrentGroupId; // Only the VaCallbackList should touch this.

private:
   //! Assignment operator declared but not define to prevent use.
   VaCallback& operator=(const VaCallback&);

   //! The pointer to the VaCallbackLink that links the callback with its callback list.
   VaCallbackLink*   mCallbackLinkPtr;
   VaCallbackGroupId mGroupId;
   bool              mIsBlocked;
   bool              mDisconnectFlag;
   bool              mDeleteFlag;
};

//! The non-templated base class for the callback list.
//! The base class VaCallback maintains a pointer to an instance of this and it uses it to invoke
//! the Disconnect method.
class VESPATK_EXPORT VaCallbackList : public VaObject
{
public:
   class VESPATK_EXPORT CallbackLink : public VaCallbackLink
   {
   public:
      CallbackLink(VaCallbackList* aCallbackList)
         : VaCallbackLink(aCallbackList)
      {
      }
      ~CallbackLink() override {} // To avoid warnings about virtual methods without virtual destructor
      void Disconnect(VaCallback* aCallbackPtr) override { mCallbackListPtr->Disconnect(aCallbackPtr); }
      void Block(VaCallback* aCallbackPtr, bool aBlock) override
      {
         if (aBlock)
         {
            EraseCallback(mCallbackListPtr->mCallbackList, aCallbackPtr);
            mCallbackListPtr->mBlockedCallbackList[aCallbackPtr->GetGroupId()].insert(aCallbackPtr);
         }
         else
         {
            EraseCallback(mCallbackListPtr->mBlockedCallbackList, aCallbackPtr);
            mCallbackListPtr->mCallbackList[aCallbackPtr->GetGroupId()].insert(aCallbackPtr);
         }
      }
   };
   friend class CallbackLink;

   // typedef STD__FAST_MULTIMAP<int, VaCallback*>           MapType;
   // typedef STD__FAST_MULTIMAP<int, VaCallback*>::iterator IterType;
   typedef std::map<VaCallbackGroupId, std::set<VaCallback*>> MapType;
   typedef std::vector<VaCallback*>                           ListType;

   explicit VaCallbackList();
   explicit VaCallbackList(const std::string& aType, const std::string& aName);
   explicit VaCallbackList(const VaCallbackList& aSrc);
   ~VaCallbackList() override;

   VaCallbackList& operator[](VaCallbackGroupId aCallbackGroupId);

   void Disconnect(VaCallback* aCallbackPtr);
   void DisconnectAll();

   //! Returns 'true' if the callback list contains no callbacks
   bool IsEmpty() const { return mCallbackList.empty(); }

   static bool EraseCallback(MapType& aList, VaCallback* aCallbackPtr);

   //! A node in a tree structure that keeps track of each
   //! callback list's performance data.
   struct ProfileNode
   {
      ProfileNode(int aCallbackUniqueId, const std::string& aCallbackType, const std::string& aCallbackNameId);
      ProfileNode();
      ~ProfileNode();

      ProfileNode* AddChild(ProfileNode* aProfileNodePtr);
      ProfileNode* FindChild(int aCallbackUniqueId);

      double CalculateSelfTime() const;

      void Print(const std::string& aIndent, std::ostream& aOut) const;

      typedef std::vector<ProfileNode*> Children;
      Children                          mChildren;

      int          mCallbackUniqueId;
      std::string  mCallbackType;
      std::string  mCallbackName;
      unsigned int mNumberCalls;
      unsigned int mNumberCallbacks;
      double       mTotalTime;
      UtWallClock* mTimerPtr;
   };
   typedef std::stack<ProfileNode*> ProfileQueueStack;

   //! Static interface for enabling and printing callback performance data.
   static void SetEnableProfile(bool aEnableProfile);
   static bool GetEnableProfile();
   static void PrintProfile(const std::string& aFilename = "");
   static void PrintProfile(std::ostream& aOut);

   const std::string& GetCallbackType() { return mCallbackType; }
   void               SetCallbackType(const std::string& aType) { mCallbackType = aType; }

protected:
   //! Connect the indicated callback to the callback list.
   //! If the callback is currently attached to a callback list then it will be
   //! disconnected from that list before connecting to this list.
   //! @param aCallbackPtr The callback to be connected.
   void ConnectP(VaCallback* aCallbackPtr);

   void BeginProfile();
   void EndProfile();
   void BeginCallbackProfile();
   void EndCallbackProfile();

   static void CleanUpProfile();

   //! Returns the root node of the profile tree and will
   //! auto generate one if not present.
   static ProfileNode* GetProfileTree();

   static const int         cMAX_RECURSION;
   static int               sInstanceCount;
   static bool              sEnableProfile;
   static ProfileQueueStack sProfileStack;
   static ProfileNode*      sProfileTreePtr;
   static std::string       sProfileFileName;
   static std::string       sProfileDateTime;

   MapType           mCallbackList;
   MapType           mTempCallbackList;
   VaCallbackGroupId mCallbackGroupId;
   int               mCallbackDepth;

private:
   MapType mBlockedCallbackList;
   //! Assignment operator declared but not defined to prevent use.
   VaCallbackList& operator=(const VaCallbackList&);
   std::string     mCallbackType;
};

#include "VaCallbackN.hpp"

} // end namespace vespa
#endif
