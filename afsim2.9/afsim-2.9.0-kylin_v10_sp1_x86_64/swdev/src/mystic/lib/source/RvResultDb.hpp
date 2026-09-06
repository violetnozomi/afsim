// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVWSFRESULTDB_HPP
#define RVWSFRESULTDB_HPP

#include <cassert>
#include <cfloat>
#include <deque>
#include <iostream>
#include <map>

#include <QAtomicInt>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>

#include "RvEventPipeClasses.hpp"
#include "RvExport.hpp"
#include "RvResultMessageArray.hpp"
#include "RvWsfPipe_FileStreamer.hpp"
#include "UtCallback.hpp"
#include "UtCircularBuffer.hpp"
class UtPackSchema;

namespace RvWsfPipe
{
class MessageCache;
}

namespace rv
{
class EventThread;
class InteractionDb;
class ResultPlatform;
class TrackDb;
class MsgWeaponFired;
class MsgWeaponTerminated;

class ResultMessage
{
public:
   template<bool IsLess>
   struct CompareInitialTime
   {
      bool operator()(const ResultMessage& aLhs, const ResultMessage& aRhs) const
      {
         return IsLess == (aLhs.mInitialTime < aRhs.mInitialTime);
      }
   };

   ResultMessage()
      : mInitialTime(0.0f)
      , mExpirationTime(std::numeric_limits<float>::max())
      , mMessagePtr(nullptr)
   {
   }
   ResultMessage(MsgBase* aMessagePtr)
      : mInitialTime(aMessagePtr->simTime())
      , mExpirationTime(std::numeric_limits<float>::max())
      , mMessagePtr(aMessagePtr)
   {
   }
   static ResultMessage MakeTransient(MsgBase* aMsg)
   {
      ResultMessage m;
      m.mExpirationTime = m.mInitialTime = aMsg->simTime();
      m.mMessagePtr                      = aMsg;
      return m;
   }
   ResultMessage(MsgBase* aMessagePtr, float aExpireTime)
      : mInitialTime(aMessagePtr->simTime())
      , mExpirationTime(aExpireTime)
      , mMessagePtr(aMessagePtr)
   {
   }

   float        GetDuration() const { return mExpirationTime - mInitialTime; }
   float        GetInitialTime() const { return mInitialTime; }
   float        GetExpirationTime() const { return mExpirationTime; }
   unsigned int GetMessageIndex() const { return mMessagePtr ? mMessagePtr->GetMessageIndex() : 0; }
   bool         IsLasting() const { return mInitialTime == mExpirationTime; }

   bool operator==(const ResultMessage& aRhs) const { return mMessagePtr == aRhs.mMessagePtr; }

   float    mInitialTime;
   float    mExpirationTime;
   MsgBase* mMessagePtr;
   // unsigned int            mMessageIndex;
};

// Container for data related to a QuantumTaskerMatrixMessage
class ResultQuantumTaskerMatrixObject //: public ResultObject
{
public:
   ResultQuantumTaskerMatrixObject() {}

   ResultQuantumTaskerMatrixObject(int                aRow,
                                   int                aColumn,
                                   float              aValue,
                                   const std::string& aTaskName,
                                   const std::string& aAssetName,
                                   const std::string& aResourceName,
                                   bool               aWinner)
      : mRow(aRow)
      , mColumn(aColumn)
      , mValue(aValue)
      , mTaskName(aTaskName)
      , mAssetName(aAssetName)
      , mResourceName(aResourceName)
      , mWinner(aWinner)
   {
   }

   virtual ~ResultQuantumTaskerMatrixObject() = default;

   int         GetRow() const { return mRow; }
   int         GetColumn() const { return mColumn; }
   float       GetValue() const { return mValue; }
   std::string GetTaskName() const { return mTaskName; }
   std::string GetAssetName() const { return mAssetName; }
   std::string GetResourceName() const { return mResourceName; }
   bool        IsTaskWinner() const { return mWinner; }

   void SetRow(int aRow) { mRow = aRow; }
   void SetColumn(int aCol) { mColumn = aCol; }
   void SetValue(float aVal) { mValue = aVal; }
   void SetTaskName(const std::string& aName) { mTaskName = aName; }
   void SetAssetName(const std::string& aName) { mAssetName = aName; }
   void SetResourceName(const std::string& aName) { mResourceName = aName; }
   void SetTaskWinner(bool aVal) { mWinner = aVal; }

private:
   int         mRow;
   int         mColumn;
   float       mValue;
   std::string mTaskName;
   std::string mAssetName;
   std::string mResourceName;
   bool        mWinner;
};

// Processes event data from a Wsf event pipe file
// Because processing of data is done in another thread,
// Locks must be used when accessing data
class RV_EXPORT ResultDb : public QObject
{
   Q_OBJECT
public:
   friend class RvWsfPipe::MessageCache;

   ResultDb(const std::string& aEventSource, unsigned int aPageLimit);
   ~ResultDb() override;

   ResultPlatform* FindPlatform(unsigned int aIndex);

   std::string  GetPlatformName(unsigned int aIndex);
   unsigned int GetPlatformCount();

   float GetMaxTime() const { return mMaxTime; }
   float GetMaxRealTime() const { return mMaxTime + mClock.GetClock(); }

   void SetCurrentTime(float aSimTime);
   void GetTimeRangeInCache(double& aMinTime, double& aMaxTime);

   const MessageBaseArray& LockMessagesOfType(int aMessageTypeId)
   {
      assert(mMessagesByTypeMap.find(aMessageTypeId) != mMessagesByTypeMap.end());
      MessageTypeList& mtl = mMessagesByTypeMap[aMessageTypeId];
      mtl.mMutex.lock();
      return mtl.mMessageArray;
   }

   bool HasMessagesOfType(int aMessageTypeId) const
   {
      return mMessagesByTypeMap.find(aMessageTypeId) != mMessagesByTypeMap.end();
   }

   void UnlockMessagesOfType(int aMessageTypeId)
   {
      MessageTypeList& mtl = mMessagesByTypeMap[aMessageTypeId];
      mtl.mMutex.unlock();
   }

   const MessageBaseArray& LockTrackStateMessages()
   {
      mTrackStateMessages.mMutex.lock();
      return mTrackStateMessages.mMessageArray;
   }
   void UnlockTrackStateMessages() { mTrackStateMessages.mMutex.unlock(); }

   const TrackDb* LockTrackDb()
   {
      mTrackDbMutex.lock();
      return mTrackDb;
   }
   void UnlockTrackDb() { mTrackDbMutex.unlock(); }

   InteractionDb* LockInteractionDb()
   {
      mInteractionDbMutex.lock();
      return mInteractionDb;
   }
   void UnlockInteractionDb() { mInteractionDbMutex.unlock(); }

   void ProcessOneTimeMessages(std::vector<rv::MsgBase*>& aMessages);

   struct MessageStats
   {
      std::map<size_t, size_t> mMessageCountByType;
      std::map<size_t, size_t> mMessageBytesByType;
   };
   MessageStats GetMessageStats();

   const UtPackSchema* GetSchema() const { return mSchemaPtr; }

   ResultPlatform* FindPlatformByName(const std::string& aName);
   ResultPlatform* FindPlatformByName(const QString& aName);
   int             FindPlatformIndexByName(const std::string& aName);

   std::multimap<float, ResultQuantumTaskerMatrixObject*> GetQuantumTaskerMatrix()
   {
      return mQuantumTaskerMatrixMessages;
   }

   MessageBaseArray& GetAllPagedMessageList() { return mAllMessageArray.mMessageArray; }
   MessageBaseArray& GetAllOneTimeMessageList() { return mAllOneTimeMessageArray.mMessageArray; }

   // this will allocate the aMsgPtr, it is the responsibility of the caller to clean up
   void FindWeaponTerminationByWeaponId(int aId, MsgWeaponTerminated*& aMsgPtr);
   // this will allocate the aMsgPtr, it is the responsibility of the caller to clean up
   void FindWeaponFireByWeaponId(int aId, float& aMaxTime, MsgWeaponFired*& aMsgPtr);
   // this will allocate into the aEventList, it is the responsibility of the caller to clean up
   void                   TraceTrackId(const Track_Id&                                 aTrackId,
                                       unsigned int                                    aResultId,
                                       std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                       float                                           aMaxTime);
   RvWsfPipe::StreamState GetStreamState();

private:
   void SetTimeRangeInCache(float aMinTime, float aMaxTime)
   {
      mTimeInCacheMutex.lock();
      mMinTimeInCache = aMinTime;
      mMaxTimeInCache = aMaxTime;
      mTimeInCacheMutex.unlock();
   }

   void SetSchema(const UtPackSchema* aSchema) { mSchemaPtr = aSchema; }

   ResultPlatform* GetPlatform(unsigned int aIndex, const std::string& aName, double aTime);

   /*
      void MessageOutOfScope(ViEventPipeMessage* aMessagePtr,
                             bool                        aAtBegin);

      void MessageInScope(ViEventPipeMessage* aMessagePtr,
                          bool                        aAtBegin);

      void MessageInTimeRange(ViEventPipeMessage* aMessagePtr,
                              bool                aAtBegin);
      void MessageOutOfTimeRange(ViEventPipeMessage* aMessagePtr,
                                 bool                aAtBegin);
   */
   void Push(MsgBase* aMessagePtr, bool aInFront);
   void Pop(MsgBase* aMessagePtr, bool aInFront);
   void PushBack(MsgBase* aMessagePtr) { Push(aMessagePtr, false); }
   void PopBack(MsgBase* aMessagePtr) { Pop(aMessagePtr, false); }

   void PushFront(MsgBase* aMessagePtr) { Push(aMessagePtr, true); }
   void PopFront(MsgBase* aMessagePtr) { Pop(aMessagePtr, true); }
   void SetMaxTime(float aMaxTime)
   {
      mClock.ResetClock();
      mMaxTime = aMaxTime;
   }

   void ProcessOneTimeMessage(MsgBase* aMessagePtr);


   void TraceTrackIdPrivate(const Track_Id&                                 aTrackId,
                            unsigned int                                    aResultId,
                            std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                            float                                           aMaxTime,
                            std::set<std::pair<std::string, int>>&          aTraversalSet) const;
   void FindTasksByTrackId(const Track_Id&                                 aTrackId,
                           unsigned int                                    aResultId,
                           std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                           float                                           aMaxTime) const;
   void FindLocalTrackCreatedByTrackId(const Track_Id&                                 aTrackId,
                                       unsigned int                                    aResultId,
                                       std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                       float                                           aMaxTime) const;
   void FindSensorTrackCreatedByTrackId(const Track_Id&                                 aTrackId,
                                        unsigned int                                    aResultId,
                                        std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                        float                                           aMaxTime) const;
   void FindLocalTrackDecorrelationByTrackId(const Track_Id&                                 aTrackId,
                                             unsigned int                                    aResultId,
                                             std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                             float                                           aMaxTime) const;
   void FindAndTraceLocalTrackCorrelationByTrackId(const Track_Id&                                 aTrackId,
                                                   unsigned int                                    aResultId,
                                                   std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                                   float                                           aMaxTime,
                                                   std::set<std::pair<std::string, int>>&          aTraversalSet) const;

   EventThread*                 mDataThread;
   QMutex                       mPlatformsLock;
   std::vector<ResultPlatform*> mPlatforms;
   QMutex                       mPlatformNameToIndexMutex;
   std::map<std::string, int>   mPlatformNameToIndex;
   float                        mMaxTime;
   QMutex                       mTimeInCacheMutex;
   float                        mMinTimeInCache;
   float                        mMaxTimeInCache;

   struct MessageTypeList
   {
      // int                                 mMessageTypeId;
      QMutex           mMutex;
      MessageBaseArray mMessageArray;
   };

   std::map<int, MessageTypeList> mMessagesByTypeMap;

   MessageTypeList mTrackStateMessages;

   QMutex                                                 mTrackDbMutex;
   TrackDb*                                               mTrackDb;
   QMutex                                                 mInteractionDbMutex;
   InteractionDb*                                         mInteractionDb;
   QMutex                                                 mOneTimeMessageMutex;
   std::vector<MsgBase*>                                  mOneTimeMessages;
   std::multimap<float, ResultQuantumTaskerMatrixObject*> mQuantumTaskerMatrixMessages;
   const UtPackSchema*                                    mSchemaPtr;

   MessageTypeList mAllMessageArray;
   MessageTypeList mAllOneTimeMessageArray;
   UtWallClock     mClock;
};
} // namespace rv
#endif
