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

#include "RvResultDb.hpp"

#include <functional>

#include <QMutexLocker>

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvEventThread.hpp"
#include "RvInteractionDb.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "RvResultPlatform.hpp"
#include "RvTrackDb.hpp"
#include "RvWsfPipe.hpp"
#include "WkfGeneralPrefObject.hpp"

//#define ASSERT_MESSAGE_QUEUE
#ifdef ASSERT_MESSAGE_QUEUE
std::set<Message*> messagesInQueue;
#endif

rv::ResultDb::ResultDb(const std::string& aEventSource, const unsigned int aPageLimit)
   : mPlatformsLock(QMutex::Recursive)
   , mMaxTime(0.0)
   , mMinTimeInCache(0)
   , mMaxTimeInCache(1)
{
   mClock.ResetClock();
   mDataThread = new rv::EventThread(this, aPageLimit);
   mDataThread->start();

   mDataThread->SetEventSource(aEventSource);

   //   void ViResRecord_register_default_record_sources(ViWsfResultDb* aDb); // JEM
   //   ViResRecord_register_default_record_sources(this); // JEM

   mTrackDb       = new rv::TrackDb;
   mInteractionDb = new rv::InteractionDb;
}

rv::ResultDb::~ResultDb()
{
   mDataThread->QuitThread();
   if (mDataThread->wait())
   {
      delete mTrackDb;
      delete mInteractionDb;
      delete mDataThread;
   }
   for (auto& it : mPlatforms)
   {
      rv::ResultPlatform* plat = it;
      if (plat)
      {
         delete plat;
      }
   }
   for (auto& message : mQuantumTaskerMatrixMessages)
   {
      rv::ResultQuantumTaskerMatrixObject* curObj = message.second;
      if (curObj != nullptr)
      {
         delete curObj;
      }
   }
   mQuantumTaskerMatrixMessages.clear();
   //   for (RecordSourceMap::iterator i = mRecordSources.begin(); i != mRecordSources.end(); ++i) // JEM
   //   { // JEM
   //      delete i->second; // JEM
   //   } // JEM
}

rv::ResultPlatform* rv::ResultDb::FindPlatform(unsigned int aIndex)
{
   QMutexLocker lock(&mPlatformsLock);
   if (mPlatforms.size() > aIndex)
   {
      rv::ResultPlatform* platformPtr = mPlatforms[aIndex];
      assert(!platformPtr || platformPtr->GetPlatformIndex() == aIndex);
      return platformPtr;
   }
   return nullptr;
}

std::string rv::ResultDb::GetPlatformName(unsigned int aIndex)
{
   QMutexLocker lock(&mPlatformsLock);
   if (mPlatforms.size() > aIndex)
   {
      rv::ResultPlatform* platformPtr = mPlatforms[aIndex];
      if (platformPtr)
      {
         return platformPtr->GetName();
      }
   }
   return std::string();
}


unsigned int rv::ResultDb::GetPlatformCount()
{
   QMutexLocker lock(&mPlatformsLock);
   return static_cast<unsigned int>(mPlatforms.size());
}

rv::ResultPlatform* rv::ResultDb::GetPlatform(unsigned int aIndex, const std::string& aName, double aTime)
{
   QMutexLocker lock(&mPlatformsLock);

   if (mPlatforms.size() > aIndex)
   {
      rv::ResultPlatform* platformPtr = mPlatforms[aIndex];
      if (platformPtr)
      {
         return platformPtr;
      }
   }
   while (mPlatforms.size() <= aIndex)
   {
      mPlatforms.push_back(nullptr);
   }
   return mPlatforms[aIndex] = new rv::ResultPlatform(aName, aIndex, aTime);
}

// this is where a message gets addressed as it enters into our buffer
void rv::ResultDb::Push(rv::MsgBase* aMessagePtr, bool aAtBegin)
{
#ifdef ASSERT_MESSAGE_QUEUE
   bool ok = messagesInQueue.insert(aMessagePtr).second;
   assert(ok);
#endif

   int msgId = aMessagePtr->GetMessageId();

   if (rv::IsEventMessage(aMessagePtr))
   {
      MessageTypeList& mtl = mMessagesByTypeMap[msgId];
      mtl.mMutex.lock();
      mtl.mMessageArray.push(aMessagePtr, aAtBegin);
      mtl.mMutex.unlock();
   }
   mAllMessageArray.mMutex.lock();
   mAllMessageArray.mMessageArray.push(aMessagePtr, aAtBegin);
   mAllMessageArray.mMutex.unlock();

   switch (msgId)
   {
   case MsgEntityState::cMESSAGE_ID:
   {
      MsgEntityState*     msgPtr  = (MsgEntityState*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform((unsigned int)msgPtr->state().platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         msgPtr->state().platformIndex(0);
      }
   }
   break;
   case MsgDetectAttempt::cMESSAGE_ID:
   {
      MsgDetectAttempt*   msgPtr  = (MsgDetectAttempt*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform(msgPtr->sensorPlatformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         msgPtr->sensorPlatformIndex(0);
      }
   }
   break;
   case MsgLocalTrackCreated::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage((MsgLocalTrackCreated*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->AddMessage((MsgLocalTrackCreated*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      mTrackStateMessages.mMutex.lock();
      mTrackStateMessages.mMessageArray.push(aMessagePtr, aAtBegin);
      mTrackStateMessages.mMutex.unlock();
      break;
   }
   case MsgSensorTrackCreated::cMESSAGE_ID:
   {
      MsgSensorTrackCreated* msgPtr = (MsgSensorTrackCreated*)aMessagePtr;

      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage(msgPtr, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->AddMessage((MsgSensorTrackCreated*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      if (msgPtr->targetIndexValid())
      {
         rv::ResultPlatform* platPtr = FindPlatform(msgPtr->targetIndex());
         if (platPtr)
         {
            platPtr->BeginWrite();
            platPtr->AddMessage(msgPtr, aAtBegin);
            platPtr->EndWrite();
         }
         else
         {
            msgPtr->targetIndex(0);
         }
      }
      break;
   }
   case MsgLocalTrackDrop::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage((MsgLocalTrackDrop*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->AddMessage((MsgLocalTrackDrop*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      mTrackStateMessages.mMutex.lock();
      mTrackStateMessages.mMessageArray.push(aMessagePtr, aAtBegin);
      mTrackStateMessages.mMutex.unlock();
      break;
   }
   case MsgSensorTrackDrop::cMESSAGE_ID:
   {
      MsgSensorTrackDrop* msgPtr = (MsgSensorTrackDrop*)aMessagePtr;

      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage(msgPtr, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->AddMessage((MsgSensorTrackCreated*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      if (msgPtr->targetIndexValid())
      {
         rv::ResultPlatform* platPtr = FindPlatform(msgPtr->targetIndex());
         if (platPtr)
         {
            platPtr->BeginWrite();
            platPtr->AddMessage(msgPtr, aAtBegin);
            platPtr->EndWrite();
         }
         else
         {
            msgPtr->targetIndex(0);
         }
      }
      break;
   }
   case MsgLocalTrackUpdate::cMESSAGE_ID:
      mTrackDbMutex.lock();
      mTrackDb->AddMessage((MsgLocalTrackUpdate*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      break;
   case MsgSensorTrackUpdate::cMESSAGE_ID:
      mTrackDbMutex.lock();
      mTrackDb->AddMessage((MsgSensorTrackUpdate*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      break;
   case MsgPartStatus::cMESSAGE_ID:
   {
      MsgPartStatus*      msg     = (MsgPartStatus*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msg, msg->partType(), msg->partName(), aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->platformIndex(0);
      }
      break;
   }
   case MsgPlatformAppearanceChanged::cMESSAGE_ID:
   {
      auto* msg     = static_cast<MsgPlatformAppearanceChanged*>(aMessagePtr);
      auto* platPtr = FindPlatform(msg->ownerIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->ownerIndex(0);
      }
      break;
   }
   case MsgPartArticulationStatus::cMESSAGE_ID:
   {
      MsgPartArticulationStatus* msg     = (MsgPartArticulationStatus*)aMessagePtr;
      rv::ResultPlatform*        platPtr = FindPlatform(msg->ownerIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msg, msg->partType(), msg->partName(), aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->ownerIndex(0);
      }
      break;
   }
   case MsgSensorDetectionChange::cMESSAGE_ID:
   {
      MsgSensorDetectionChange* msg = (MsgSensorDetectionChange*)aMessagePtr;

      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage(msg, aAtBegin);
      mInteractionDbMutex.unlock();

      rv::ResultPlatform* platPtr = FindPlatform(msg->targetIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->targetIndex(0);
      }
      break;
   }
   case MsgJammingRequestInitiated::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage((MsgJammingRequestInitiated*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgJammingRequestCanceled::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage((MsgJammingRequestCanceled*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   /*   case MsgWeaponFired::cMESSAGE_ID:
      {
         mInteractionDbMutex.lock();
         mInteractionDb->AddMessage((MsgWeaponFired*)aMessagePtr, aAtBegin);
         mInteractionDbMutex.unlock();
         break;
      }

      case MsgWeaponTerminated::cMESSAGE_ID:
      {
         mInteractionDbMutex.lock();
         mInteractionDb->AddMessage((MsgWeaponTerminated*)aMessagePtr, aAtBegin);
         mInteractionDbMutex.unlock();
         break;
      }*/
   case MsgQuantumTaskerUpdate::cMESSAGE_ID:
   {
      MsgQuantumTaskerUpdate* QTMsg = (MsgQuantumTaskerUpdate*)aMessagePtr;

      float                                theTime = aMessagePtr->simTime();
      rv::ResultQuantumTaskerMatrixObject* QtObj   = new rv::ResultQuantumTaskerMatrixObject(QTMsg->row(),
                                                                                           QTMsg->column(),
                                                                                           QTMsg->value(),
                                                                                           QTMsg->taskName(),
                                                                                           QTMsg->assetName(),
                                                                                           QTMsg->resourceName(),
                                                                                           QTMsg->taskWinner());
      mQuantumTaskerMatrixMessages.insert(std::pair<float, rv::ResultQuantumTaskerMatrixObject*>(theTime, QtObj));

      break;
   }
   case MsgTaskUpdate::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage((MsgTaskUpdate*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgMessageReceived::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage((MsgMessageReceived*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgMessageHop::cMESSAGE_ID:
   {
      QMutexLocker ml(&mInteractionDbMutex);
      mInteractionDb->AddMessage((MsgMessageHop*)aMessagePtr, aAtBegin);
      break;
   }
   case MsgScriptData::cMESSAGE_ID:
   {
      MsgScriptData* msg = (MsgScriptData*)aMessagePtr;

      rv::ResultPlatform* platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddStringKeyedMessage(msg, msg->key(), aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->platformIndex(0);
      }
      break;
   }
   case MsgBehaviorTreeState::cMESSAGE_ID:
   {
      MsgBehaviorTreeState* msg     = dynamic_cast<MsgBehaviorTreeState*>(aMessagePtr);
      rv::ResultPlatform*   platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->platformIndex(0);
      }
      break;
   }
   case MsgAuxData::cMESSAGE_ID:
   {
      MsgAuxData* msg = (MsgAuxData*)aMessagePtr;

      rv::ResultPlatform* platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->platformIndex(0);
      }
      break;
   }
   case MsgStateMachineState::cMESSAGE_ID:
   {
      MsgStateMachineState* msg     = static_cast<MsgStateMachineState*>(aMessagePtr);
      rv::ResultPlatform*   platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         // make sure we don't try to pop this message
         msg->platformIndex(0);
      }
      break;
   }
   default:
   {
      for (auto& ext : rvEnv.GetExtensions())
      {
         if (ext->HandlesMessage(aMessagePtr->GetMessageId()))
         {
            ext->AddMessage(*this, aMessagePtr, aAtBegin);
         }
      }
      break;
   }
   }
}

// this is where a message gets addressed as it leaves our buffer
void rv::ResultDb::Pop(rv::MsgBase* aMessagePtr, bool aAtBegin)
{
#ifdef ASSERT_MESSAGE_QUEUE
   size_t ec = messagesInQueue.erase(aMessagePtr);
   assert(ec);
#endif

   int msgId = aMessagePtr->GetMessageId();

   if (rv::IsEventMessage(aMessagePtr))
   {
      assert(mMessagesByTypeMap.find(msgId) != mMessagesByTypeMap.end());
      MessageTypeList& mtl = mMessagesByTypeMap[msgId];
      mtl.mMutex.lock();
      mtl.mMessageArray.pop(aMessagePtr, aAtBegin);
      mtl.mMutex.unlock();
   }
   mAllMessageArray.mMutex.lock();
   mAllMessageArray.mMessageArray.pop(aMessagePtr, aAtBegin);
   mAllMessageArray.mMutex.unlock();

   switch (msgId)
   {
   case MsgEntityState::cMESSAGE_ID:
   {
      MsgEntityState*     msgPtr  = (MsgEntityState*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform((unsigned int)msgPtr->state().platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
   }
   break;

   case MsgDetectAttempt::cMESSAGE_ID:
   {
      MsgDetectAttempt*   msgPtr  = (MsgDetectAttempt*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform(msgPtr->sensorPlatformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
   }
   break;

   case MsgLocalTrackCreated::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage((MsgLocalTrackCreated*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackStateMessages.mMutex.lock();
      mTrackStateMessages.mMessageArray.pop(aMessagePtr, aAtBegin);
      mTrackStateMessages.mMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->RemoveMessage((MsgLocalTrackCreated*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      break;
   }
   case MsgSensorTrackCreated::cMESSAGE_ID:
   {
      MsgSensorTrackCreated* msg = (MsgSensorTrackCreated*)aMessagePtr;
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage(msg, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->RemoveMessage((MsgSensorTrackCreated*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      rv::ResultPlatform* platPtr = FindPlatform(msg->targetIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgLocalTrackDrop::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage((MsgLocalTrackDrop*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackStateMessages.mMutex.lock();
      mTrackStateMessages.mMessageArray.pop(aMessagePtr, aAtBegin);
      mTrackStateMessages.mMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->RemoveMessage((MsgLocalTrackDrop*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      break;
   }
   case MsgSensorTrackDrop::cMESSAGE_ID:
   {
      MsgSensorTrackDrop* msg = (MsgSensorTrackDrop*)aMessagePtr;
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage(msg, aAtBegin);
      mInteractionDbMutex.unlock();

      mTrackDbMutex.lock();
      mTrackDb->RemoveMessage((MsgSensorTrackDrop*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      rv::ResultPlatform* platPtr = FindPlatform(msg->targetIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgLocalTrackUpdate::cMESSAGE_ID:
      mTrackDbMutex.lock();
      mTrackDb->RemoveMessage((MsgLocalTrackUpdate*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      break;
   case MsgSensorTrackUpdate::cMESSAGE_ID:
      mTrackDbMutex.lock();
      mTrackDb->RemoveMessage((MsgSensorTrackUpdate*)aMessagePtr, aAtBegin);
      mTrackDbMutex.unlock();
      break;
   case MsgPartStatus::cMESSAGE_ID:
   {
      MsgPartStatus*      msg     = (MsgPartStatus*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemovePartMessage(msg, msg->partType(), msg->partName(), aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgPlatformAppearanceChanged::cMESSAGE_ID:
   {
      auto* msg     = static_cast<MsgPlatformAppearanceChanged*>(aMessagePtr);
      auto* platPtr = FindPlatform(msg->ownerIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgSensorDetectionChange::cMESSAGE_ID:
   {
      MsgSensorDetectionChange* msg = (MsgSensorDetectionChange*)aMessagePtr;
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage(msg, aAtBegin);
      mInteractionDbMutex.unlock();

      rv::ResultPlatform* platPtr = FindPlatform(msg->targetIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgPartArticulationStatus::cMESSAGE_ID:
   {
      MsgPartArticulationStatus* msg     = (MsgPartArticulationStatus*)aMessagePtr;
      rv::ResultPlatform*        platPtr = FindPlatform(msg->ownerIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemovePartMessage(msg, msg->partType(), msg->partName(), aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   /*   case MsgWeaponFired::cMESSAGE_ID:
      {
         mInteractionDbMutex.lock();
         mInteractionDb->RemoveMessage((MsgWeaponFired*)aMessagePtr, aAtBegin);
         mInteractionDbMutex.unlock();
         break;
      }
      case MsgWeaponTerminated::cMESSAGE_ID:
      {
         mInteractionDbMutex.lock();
         mInteractionDb->RemoveMessage((MsgWeaponTerminated*)aMessagePtr, aAtBegin);
         mInteractionDbMutex.unlock();
         break;
      }*/
   case MsgJammingRequestInitiated::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage((MsgJammingRequestInitiated*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgJammingRequestCanceled::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage((MsgJammingRequestCanceled*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgTaskUpdate::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage((MsgTaskUpdate*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgMessageReceived::cMESSAGE_ID:
   {
      mInteractionDbMutex.lock();
      mInteractionDb->RemoveMessage((MsgMessageReceived*)aMessagePtr, aAtBegin);
      mInteractionDbMutex.unlock();
      break;
   }
   case MsgMessageHop::cMESSAGE_ID:
   {
      QMutexLocker ml(&mInteractionDbMutex);
      mInteractionDb->RemoveMessage((MsgMessageHop*)aMessagePtr, aAtBegin);
      break;
   }
   case MsgScriptData::cMESSAGE_ID:
   {
      MsgScriptData*      msgPtr  = (MsgScriptData*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform((unsigned int)msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveStringKeyedMessage(msgPtr, msgPtr->key(), aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgBehaviorTreeState::cMESSAGE_ID:
   {
      MsgBehaviorTreeState* msg     = dynamic_cast<MsgBehaviorTreeState*>(aMessagePtr);
      rv::ResultPlatform*   platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgAuxData::cMESSAGE_ID:
   {
      MsgAuxData*         msg     = dynamic_cast<MsgAuxData*>(aMessagePtr);
      rv::ResultPlatform* platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   case MsgStateMachineState::cMESSAGE_ID:
   {
      MsgStateMachineState* msg     = static_cast<MsgStateMachineState*>(aMessagePtr);
      rv::ResultPlatform*   platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msg, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }
   default:
   {
      for (auto& ext : rvEnv.GetExtensions())
      {
         if (ext->HandlesMessage(aMessagePtr->GetMessageId()))
         {
            ext->RemoveMessage(*this, aMessagePtr, aAtBegin);
         }
      }
      break;
   }
   }
}

void rv::ResultDb::SetCurrentTime(float aSimTime)
{
   mDataThread->SetCurrentTime(aSimTime);
}

RvWsfPipe::StreamState rv::ResultDb::GetStreamState()
{
   return mDataThread->GetStreamState();
}

void rv::ResultDb::GetTimeRangeInCache(double& aMinTime, double& aMaxTime)
{
   aMinTime = mMinTimeInCache;
   aMaxTime = mMaxTimeInCache;
}

void rv::ResultDb::ProcessOneTimeMessages(std::vector<rv::MsgBase*>& aMessages)
{
   mOneTimeMessageMutex.lock();
   mOneTimeMessages.swap(aMessages);
   mOneTimeMessageMutex.unlock();
}

void rv::ResultDb::ProcessOneTimeMessage(rv::MsgBase* aMessagePtr)
{
   aMessagePtr->Ref();
   mAllOneTimeMessageArray.mMutex.lock();
   mAllOneTimeMessageArray.mMessageArray.push(aMessagePtr, false);
   mAllOneTimeMessageArray.mMutex.unlock();

   int msgId = aMessagePtr->GetMessageId();
   if (rv::IsEventMessage(aMessagePtr))
   {
      MessageTypeList& mtl = mMessagesByTypeMap[msgId];
      mtl.mMutex.lock();
      mtl.mMessageArray.push(aMessagePtr, false);
      mtl.mMutex.unlock();
   }


   mOneTimeMessageMutex.lock();
   if (aMessagePtr->GetMessageId() == MsgDrawCommand::cMESSAGE_ID)
   {
      MsgDrawCommand* msgPtr = dynamic_cast<MsgDrawCommand*>(aMessagePtr);
      rvEnv.AddDrawMessage(*this, *msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgComment::cMESSAGE_ID)
   {
      MsgComment* msgPtr = dynamic_cast<MsgComment*>(aMessagePtr);
      rvEnv.AddComment(*this, *msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgExecData::cMESSAGE_ID)
   {
      MsgExecData* msgPtr = dynamic_cast<MsgExecData*>(aMessagePtr);
      rvEnv.AddMetaData(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgScenarioData::cMESSAGE_ID)
   {
      MsgScenarioData* msgPtr = dynamic_cast<MsgScenarioData*>(aMessagePtr);

      Vec3f color = msgPtr->classificationColor();

      wkfEnv.GetPreferenceObject<wkf::GeneralPrefObject>()->SetOverlayBannerText(msgPtr->classification(),
                                                                                 UtColor(color.x(), color.y(), color.z()));
   }
   else if (aMessagePtr->GetMessageId() == MsgEmitterModeDefinition::cMESSAGE_ID)
   {
      MsgEmitterModeDefinition* msgPtr  = dynamic_cast<MsgEmitterModeDefinition*>(aMessagePtr);
      int                       index   = msgPtr->ownerIndex();
      rv::ResultPlatform*       platPtr = FindPlatform(index);
      if (platPtr)
      {
         platPtr->BeginWrite();
         if (msgPtr->partType() == rv::PartType::sensor)
         {
            platPtr->AddSensorModeDefinition(msgPtr);
         }
         else if (msgPtr->partType() == rv::MilPartType::weapon)
         {
            platPtr->AddWeaponModeDefinition(msgPtr);
         }
         platPtr->EndWrite();
      }
      if (msgPtr->partType() == rv::PartType::sensor)
      {
         rvEnv.AddSensorModeDefinition(*msgPtr);
      }
      else if (msgPtr->partType() == rv::MilPartType::weapon)
      {
         rvEnv.AddWeaponModeDefinition(*msgPtr);
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgSensorModeChange::cMESSAGE_ID)
   {
      MsgSensorModeChange* msg     = dynamic_cast<MsgSensorModeChange*>(aMessagePtr);
      rv::ResultPlatform*  platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msg, rv::PartType::sensor, msg->sensorName(), false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgWeaponModeChange::cMESSAGE_ID)
   {
      MsgWeaponModeChange* msg     = dynamic_cast<MsgWeaponModeChange*>(aMessagePtr);
      rv::ResultPlatform*  platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msg, rv::MilPartType::weapon, msg->weaponName(), false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgWeaponQuantityChange::cMESSAGE_ID)
   {
      MsgWeaponQuantityChange* msg     = dynamic_cast<MsgWeaponQuantityChange*>(aMessagePtr);
      rv::ResultPlatform*      platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msg, rv::MilPartType::weapon, msg->weaponName(), false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgVisualPartDefinition::cMESSAGE_ID)
   {
      MsgVisualPartDefinition* msgPtr  = dynamic_cast<MsgVisualPartDefinition*>(aMessagePtr);
      int                      index   = msgPtr->ownerIndex();
      rv::ResultPlatform*      platPtr = FindPlatform(index);
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msgPtr, rv::PartType::visual, msgPtr->partName(), false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgPartStatus::cMESSAGE_ID)
   {
      MsgPartStatus*      msg     = dynamic_cast<MsgPartStatus*>(aMessagePtr);
      rv::ResultPlatform* platPtr = FindPlatform(msg->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddPartMessage(msg, msg->partType(), msg->partName(), false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgPlatformInfo::cMESSAGE_ID)
   {
      MsgPlatformInfo*    msgPtr  = dynamic_cast<MsgPlatformInfo*>(aMessagePtr);
      rv::ResultPlatform* platPtr = GetPlatform(msgPtr->platformIndex(), msgPtr->name(), msgPtr->simTime());
      if (mPlatformNameToIndex.find(msgPtr->name()) == mPlatformNameToIndex.end())
      {
         mPlatformNameToIndexMutex.lock();
         mPlatformNameToIndex[msgPtr->name()] = msgPtr->platformIndex();
         mPlatformNameToIndexMutex.unlock();
      }
      if (platPtr)
      {
         platPtr->BeginWrite();
         bool newPlatform = !platPtr->HasMessageOfType(MsgPlatformInfo::cMESSAGE_ID);
         platPtr->AddBaseMessage(msgPtr, false);
         platPtr->EndWrite();
         if (newPlatform)
         {
            rvEnv.AddPlatform(*this, *msgPtr);
         }
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgWeaponFired::cMESSAGE_ID)
   {
      MsgWeaponFired* msgPtr = dynamic_cast<MsgWeaponFired*>(aMessagePtr);
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage(msgPtr, false);
      mInteractionDbMutex.unlock();
      if (msgPtr->targetPlatformIndexValid())
      {
         rv::ResultPlatform* platPtr = FindPlatform(msgPtr->targetPlatformIndex());
         if (platPtr)
         {
            platPtr->BeginWrite();
            platPtr->AddMessage(msgPtr);
            platPtr->EndWrite();
         }
      }
      rvEnv.AddWeaponFiredEvent(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgWeaponTerminated::cMESSAGE_ID)
   {
      MsgWeaponTerminated* msgPtr = dynamic_cast<MsgWeaponTerminated*>(aMessagePtr);
      mInteractionDbMutex.lock();
      mInteractionDb->AddMessage(msgPtr, false);
      mInteractionDbMutex.unlock();
      if (msgPtr->targetPlatformIndexValid())
      {
         rv::ResultPlatform* platPtr = FindPlatform(msgPtr->targetPlatformIndex());
         if (platPtr)
         {
            platPtr->BeginWrite();
            platPtr->AddMessage(msgPtr);
            platPtr->EndWrite();
         }
      }
      rvEnv.AddWeaponTerminatedEvent(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgSetDate::cMESSAGE_ID)
   {
      MsgSetDate* msgPtr = dynamic_cast<MsgSetDate*>(aMessagePtr);
      rvEnv.SetDate(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgResource::cMESSAGE_ID)
   {
      MsgResource* msgPtr = dynamic_cast<MsgResource*>(aMessagePtr);
      rvEnv.AddResource(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgBehaviorTree::cMESSAGE_ID)
   {
      MsgBehaviorTree*    msgPtr  = dynamic_cast<MsgBehaviorTree*>(aMessagePtr);
      rv::ResultPlatform* platPtr = FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msgPtr, false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgStateMachine::cMESSAGE_ID)
   {
      MsgStateMachine*    msgPtr  = static_cast<MsgStateMachine*>(aMessagePtr);
      rv::ResultPlatform* platPtr = FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msgPtr, false);
         platPtr->EndWrite();
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgBookmark::cMESSAGE_ID)
   {
      MsgBookmark* msgPtr = (MsgBookmark*)aMessagePtr;
      rvEnv.AddBookmark(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgPlatformStatus::cMESSAGE_ID)
   {
      MsgPlatformStatus*  msgPtr  = (MsgPlatformStatus*)aMessagePtr;
      rv::ResultPlatform* platPtr = FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msgPtr, false);
         platPtr->EndWrite();
      }
      else
      {
         msgPtr->platformIndex(0);
      }
      rvEnv.AddPlatformStatusEvent(*msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgRouteChanged::cMESSAGE_ID)
   {
      MsgRouteChanged*    msgPtr  = static_cast<MsgRouteChanged*>(aMessagePtr);
      rv::ResultPlatform* platPtr = FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msgPtr, false);
         platPtr->EndWrite();
      }
      else
      {
         msgPtr->platformIndex(0);
      }
   }
   else if (aMessagePtr->GetMessageId() == MsgCircularZone::cMESSAGE_ID ||
            aMessagePtr->GetMessageId() == MsgEllipticalZone::cMESSAGE_ID ||
            aMessagePtr->GetMessageId() == MsgSphericalZone::cMESSAGE_ID ||
            aMessagePtr->GetMessageId() == MsgPolygonalZone::cMESSAGE_ID ||
            aMessagePtr->GetMessageId() == MsgZoneSet::cMESSAGE_ID)
   {
      rvEnv.AddZone(*aMessagePtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgCommInfo::cMESSAGE_ID)
   {
      MsgCommInfo* msgPtr = dynamic_cast<MsgCommInfo*>(aMessagePtr);
      rvEnv.AddComm(*this, *msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgNetworkInfo::cMESSAGE_ID)
   {
      MsgNetworkInfo* msgPtr = dynamic_cast<MsgNetworkInfo*>(aMessagePtr);
      rvEnv.AddNetwork(*this, *msgPtr);
   }
   else if (aMessagePtr->GetMessageId() == MsgRouterInfo::cMESSAGE_ID)
   {
      MsgRouterInfo* msgPtr = dynamic_cast<MsgRouterInfo*>(aMessagePtr);
      rvEnv.AddRouter(*this, *msgPtr);
   }

   else
   {
      for (auto& ext : rvEnv.GetExtensions())
      {
         if (ext->HandlesMessage(aMessagePtr->GetMessageId()))
         {
            ext->AddMessage(*this, aMessagePtr);
         }
      }
   }
   mOneTimeMessageMutex.unlock();
}

rv::ResultDb::MessageStats rv::ResultDb::GetMessageStats()
{
   MessageStats             stats;
   RvWsfPipe::MessageCache* cachePtr = mDataThread->LockCache();
   stats.mMessageCountByType         = cachePtr->GetMessageCountByType();
   stats.mMessageBytesByType         = cachePtr->GetMessageBytesByType();
   mDataThread->UnlockCache();
   return stats;
}

rv::ResultPlatform* rv::ResultDb::FindPlatformByName(const std::string& aName)
{
   int index = FindPlatformIndexByName(aName);
   if (index)
   {
      return mPlatforms[index];
   }
   return nullptr;
}

rv::ResultPlatform* rv::ResultDb::FindPlatformByName(const QString& aName)
{
   return FindPlatformByName(aName.toStdString());
}

int rv::ResultDb::FindPlatformIndexByName(const std::string& aName)
{
   int index = 0;
   mPlatformNameToIndexMutex.lock();
   std::map<std::string, int>::const_iterator i = mPlatformNameToIndex.find(aName);
   if (i != mPlatformNameToIndex.end())
   {
      index = i->second;
   }
   mPlatformNameToIndexMutex.unlock();
   return index;
}

void rv::ResultDb::FindWeaponTerminationByWeaponId(int aId, MsgWeaponTerminated*& aMsgPtr)
{
   constexpr int mwt  = MsgWeaponTerminated::cMESSAGE_ID;
   aMsgPtr            = nullptr;
   unsigned int found = 0;
   mMessagesByTypeMap[mwt].mMutex.lock();
   MsgWeaponTerminated* msg = nullptr;
   for (auto&& it : mMessagesByTypeMap[mwt].mMessageArray)
   {
      MsgWeaponTerminated* wt = static_cast<MsgWeaponTerminated*>(it);
      if (wt)
      {
         if (aId >= 0 && wt->weaponPlatformIndex() == static_cast<unsigned int>(aId))
         {
            msg = wt;
            found++;
         }
      }
   }
   if (msg)
   {
      aMsgPtr = new MsgWeaponTerminated(*msg);
   }
   mMessagesByTypeMap[mwt].mMutex.unlock();
   // is it a good assumption that a weapon id will only be used once?  Perhaps not when there is no munition platform.
   assert(found <= 1);
}

void rv::ResultDb::FindWeaponFireByWeaponId(int aId, float& aMaxTime, MsgWeaponFired*& aMsgPtr)
{
   constexpr int mwf  = MsgWeaponFired::cMESSAGE_ID;
   aMsgPtr            = nullptr;
   unsigned int found = 0;
   mMessagesByTypeMap[mwf].mMutex.lock();
   MsgWeaponFired* msg = nullptr;
   for (auto&& it : mMessagesByTypeMap[mwf].mMessageArray)
   {
      MsgWeaponFired* wt = static_cast<MsgWeaponFired*>(it);
      if (wt)
      {
         if (wt->simTime() > aMaxTime)
         {
            break;
         }
         if (aId >= 0 && wt->weaponPlatformIndex() == static_cast<unsigned int>(aId))
         {
            aMaxTime = wt->simTime();
            msg      = wt;
            found++;
         }
      }
   }
   if (msg)
   {
      aMsgPtr = new MsgWeaponFired(*msg);
   }
   mMessagesByTypeMap[mwf].mMutex.unlock();
   // is it a good assumption that a weapon id will only be used once?  Perhaps not when there is no munition platform.
   assert(found <= 1);
}

void rv::ResultDb::TraceTrackId(const Track_Id&                                 aTrackId,
                                unsigned int                                    aResultId,
                                std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                float                                           aMaxTime)
{
   constexpr int                         mtu   = MsgTaskUpdate::cMESSAGE_ID;
   constexpr int                         mltc  = MsgLocalTrackCreated::cMESSAGE_ID;
   constexpr int                         mstc  = MsgSensorTrackCreated::cMESSAGE_ID;
   constexpr int                         mltd  = MsgLocalTrackDecorrelation::cMESSAGE_ID;
   constexpr int                         mltco = MsgLocalTrackCorrelation::cMESSAGE_ID;
   std::set<std::pair<std::string, int>> traversalSet;
   mMessagesByTypeMap[mtu].mMutex.lock();
   mMessagesByTypeMap[mltc].mMutex.lock();
   mMessagesByTypeMap[mstc].mMutex.lock();
   mMessagesByTypeMap[mltd].mMutex.lock();
   mMessagesByTypeMap[mltco].mMutex.lock();
   TraceTrackIdPrivate(aTrackId, aResultId, aEventList, aMaxTime, traversalSet);
   mMessagesByTypeMap[mltco].mMutex.unlock();
   mMessagesByTypeMap[mltd].mMutex.unlock();
   mMessagesByTypeMap[mstc].mMutex.unlock();
   mMessagesByTypeMap[mltc].mMutex.unlock();
   mMessagesByTypeMap[mtu].mMutex.unlock();
}

void rv::ResultDb::TraceTrackIdPrivate(const Track_Id&                                 aTrackId,
                                       unsigned int                                    aResultId,
                                       std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                       float                                           aMaxTime,
                                       std::set<std::pair<std::string, int>>&          aTraversalSet) const
{
   // make sure every trackId is only visited once
   bool inserted = aTraversalSet.emplace(aTrackId.owner(), aTrackId.localTrackNumber()).second;
   if (inserted)
   {
      FindTasksByTrackId(aTrackId, aResultId, aEventList, aMaxTime);
      FindLocalTrackCreatedByTrackId(aTrackId, aResultId, aEventList, aMaxTime);
      FindSensorTrackCreatedByTrackId(aTrackId, aResultId, aEventList, aMaxTime);
      FindLocalTrackDecorrelationByTrackId(aTrackId, aResultId, aEventList, aMaxTime);
      FindAndTraceLocalTrackCorrelationByTrackId(aTrackId, aResultId, aEventList, aMaxTime, aTraversalSet);
   }
}

void rv::ResultDb::FindTasksByTrackId(const Track_Id&                                 aTrackId,
                                      unsigned int                                    aResultId,
                                      std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                      float                                           aMaxTime) const
{
   constexpr int                                  mtu = MsgTaskUpdate::cMESSAGE_ID;
   std::map<int, MessageTypeList>::const_iterator mit = mMessagesByTypeMap.find(mtu);
   if (mit != mMessagesByTypeMap.end())
   {
      for (auto&& it : mit->second.mMessageArray)
      {
         MsgTaskUpdate* task = static_cast<MsgTaskUpdate*>(it);
         if (task)
         {
            if (task->simTime() > aMaxTime)
            {
               break;
            }
            if ((task->trackId().localTrackNumber() == aTrackId.localTrackNumber()) &&
                (task->trackId().owner() == aTrackId.owner()))
            {
               aEventList.push_back(std::pair<MsgBase*, unsigned int>(new MsgTaskUpdate(*task), aResultId));
            }
         }
      }
   }
}

void rv::ResultDb::FindLocalTrackCreatedByTrackId(const Track_Id&                                 aTrackId,
                                                  unsigned int                                    aResultId,
                                                  std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                                  float                                           aMaxTime) const
{
   constexpr int                                  mltc = MsgLocalTrackCreated::cMESSAGE_ID;
   std::map<int, MessageTypeList>::const_iterator mit  = mMessagesByTypeMap.find(mltc);
   if (mit != mMessagesByTypeMap.end())
   {
      for (auto&& it : mit->second.mMessageArray)
      {
         MsgLocalTrackCreated* ltc = static_cast<MsgLocalTrackCreated*>(it);
         if (ltc)
         {
            if (ltc->simTime() > aMaxTime)
            {
               break;
            }
            if ((ltc->trackId().localTrackNumber() == aTrackId.localTrackNumber()) &&
                (ltc->trackId().owner() == aTrackId.owner()))
            {
               aEventList.push_back(std::pair<MsgBase*, unsigned int>(new MsgLocalTrackCreated(*ltc), aResultId));
            }
         }
      }
   }
}

void rv::ResultDb::FindSensorTrackCreatedByTrackId(const Track_Id&                                 aTrackId,
                                                   unsigned int                                    aResultId,
                                                   std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                                   float                                           aMaxTime) const
{
   constexpr int                                  mstc = MsgSensorTrackCreated::cMESSAGE_ID;
   std::map<int, MessageTypeList>::const_iterator mit  = mMessagesByTypeMap.find(mstc);
   if (mit != mMessagesByTypeMap.end())
   {
      for (auto&& it : mit->second.mMessageArray)
      {
         MsgSensorTrackCreated* stc = static_cast<MsgSensorTrackCreated*>(it);
         if (stc)
         {
            if (stc->simTime() > aMaxTime)
            {
               break;
            }
            if ((stc->trackId().localTrackNumber() == aTrackId.localTrackNumber()) &&
                (stc->trackId().owner() == aTrackId.owner()))
            {
               aEventList.push_back(std::pair<MsgBase*, unsigned int>(new MsgSensorTrackCreated(*stc), aResultId));
            }
         }
      }
   }
}

void rv::ResultDb::FindLocalTrackDecorrelationByTrackId(const Track_Id&                                 aTrackId,
                                                        unsigned int                                    aResultId,
                                                        std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                                        float                                           aMaxTime) const
{
   constexpr int                                  mltd = MsgLocalTrackDecorrelation::cMESSAGE_ID;
   std::map<int, MessageTypeList>::const_iterator mit  = mMessagesByTypeMap.find(mltd);
   if (mit != mMessagesByTypeMap.end())
   {
      for (auto&& it : mit->second.mMessageArray)
      {
         MsgLocalTrackDecorrelation* ltd = static_cast<MsgLocalTrackDecorrelation*>(it);
         if (ltd)
         {
            if (ltd->simTime() > aMaxTime)
            {
               break;
            }
            if ((ltd->trackId().localTrackNumber() == aTrackId.localTrackNumber()) &&
                (ltd->trackId().owner() == aTrackId.owner()))
            {
               aEventList.push_back(std::pair<MsgBase*, unsigned int>(new MsgLocalTrackDecorrelation(*ltd), aResultId));
            }
         }
      }
   }
}

void rv::ResultDb::FindAndTraceLocalTrackCorrelationByTrackId(const Track_Id&                                 aTrackId,
                                                              unsigned int                                    aResultId,
                                                              std::vector<std::pair<MsgBase*, unsigned int>>& aEventList,
                                                              float                                           aMaxTime,
                                                              std::set<std::pair<std::string, int>>& aTraversalSet) const
{
   constexpr int                                  mltc = MsgLocalTrackCorrelation::cMESSAGE_ID;
   std::map<int, MessageTypeList>::const_iterator mit  = mMessagesByTypeMap.find(mltc);
   if (mit != mMessagesByTypeMap.end())
   {
      for (auto&& it : mit->second.mMessageArray)
      {
         MsgLocalTrackCorrelation* ltc = static_cast<MsgLocalTrackCorrelation*>(it);
         if (ltc)
         {
            if (ltc->simTime() > aMaxTime)
            {
               break;
            }
            if ((ltc->trackId().localTrackNumber() == aTrackId.localTrackNumber()) &&
                (ltc->trackId().owner() == aTrackId.owner()))
            {
               aEventList.push_back(std::pair<MsgBase*, unsigned int>(new MsgLocalTrackCorrelation(*ltc), aResultId));
               // I changed this from ltc->simTime to maxTime since even thing that happen after the correlation on the
               // source track matter. note that we need to make sure we don't operate on a given track Id more than once
               TraceTrackIdPrivate(ltc->addTrackId(), ltc->GetMessageIndex(), aEventList, aMaxTime, aTraversalSet);
            }
         }
      }
   }
}
