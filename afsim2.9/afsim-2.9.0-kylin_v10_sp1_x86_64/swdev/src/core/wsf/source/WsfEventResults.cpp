// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEventResults.hpp"

#include <iomanip>
#include <iostream>

#include "WsfBehaviorTreeNode.hpp"
#include "WsfCallback.hpp"
#include "WsfComm.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommResult.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfEventUtils.hpp"
#include "WsfFuel.hpp"
#include "WsfImage.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMessage.hpp"
#include "WsfMover.hpp"
#include "WsfProcessor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfTankedFuel.hpp"
#include "WsfTask.hpp"

namespace wsf
{
namespace event
{

void BehaviorTreeNodeChildren::Print(std::ostream& aStream) const
{
#undef GetTimeFormat // Avoid conflict with Windows macro
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   // clang-format off
   aStream << "BTREE_NODE_CHILDREN "
           << "\"" << mNodePtr->FilePath() << "\""
           << " " << mNodePtr->OwningPlatform()->GetName()
           << " " << mNodePtr->Id()
           << " " << mNodePtr->GetType()
           << " " << mNodePtr->Children().size();
   // clang-format on
   for (auto child : mNodePtr->Children())
   {
      aStream << " " << child->Id();
   }
   aStream << "\n";
};

void BehaviorTreeNodeChildren::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "BTREE_NODE_CHILDREN"
           << ',' << mNodePtr->FilePath()
           << ',' << mNodePtr->OwningPlatform()->GetName()    //platform name
           << ',' << mNodePtr->Id()                           //globally unique behavior tree node ID
           << ',' << mNodePtr->GetType()                      //behavior names are object types & vise versa
           << ',' << mNodePtr->Children().size();                    //number of child IDs to follow
   // clang-format on
   for (auto child : mNodePtr->Children())
   {
      aStream << ',' << child->Id();
   }
   if (mNodePtr->Children().empty())
   {
      aStream << ',';
   }
   aStream << '\n';
};

void BehaviorTreeNodeExec::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "BTREE_NODE_EXEC " << mNodePtr->OwningPlatform()->GetName() << " " << mNodePtr->Id() << " "
           << mNodePtr->GetType() << " " << mNodePtr->ExecutedLastRun();
   if (mNodePtr->ExecutedLastRun() == false && !mNodePtr->FailureReason().empty())
   {
      aStream << " \"" << mNodePtr->FailureReason() << "\"";
   }
   aStream << "\n";
};

void BehaviorTreeNodeExec::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "BTREE_NODE_EXEC"
           << ',' << mNodePtr->OwningPlatform()->GetName()   //platform name
           << ',' << mNodePtr->OwningPlatform()->GetSide()   //platform side
           << ',' << mNodePtr->Id()                          //globally unique behavior tree node ID
           //<< ',' << aNodePtr->GetName()                     //generic node name (sometimes descriptive of node function)
           << ',' << mNodePtr->GetType()                     //behavior names are object types & vise versa
           << ',' << mNodePtr->ExecutedLastRun();            //whether or not this node executed during last update
   // clang-format on
   aStream << ',';
   if (mNodePtr->ExecutedLastRun() == false && !mNodePtr->FailureReason().empty())
   {
      aStream << mNodePtr->FailureReason();
   }
   aStream << '\n';
};

void Comment::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "COMMENT " << mPlatformPtr->GetName() << ' ' << utils::MakePrintableString(mComment) << '\n';
}

void Comment::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "COMMENT" << ',' << mPlatformPtr->GetName() << ',' << mPlatformPtr->GetSide() << ',' << mComment
           << '\n';
}

void CommAddedToManager::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "COMM_ADDED_TO_MANAGER ";
   utils::PrintCommData(aStream, mCommPtr, false);
   aStream << '\n';
}

void CommAddedToManager::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "COMM_ADDED_TO_MANAGER" << ',' << mCommPtr->GetPlatform()->GetName() << ',' << mCommPtr->GetName()
           << '\n';
}

void CommRemovedFromManager::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "COMM_REMOVED_FROM_MANAGER ";
   utils::PrintCommData(aStream, mCommPtr, false);
   aStream << '\n';
}

void CommRemovedFromManager::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "COMM_REMOVED_FROM_MANAGER" << ',' << mCommPtr->GetPlatform()->GetName() << ','
           << mCommPtr->GetName() << '\n';
}

void CommAddedToLocal::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "COMM_ADDED_TO_LOCAL ";
   utils::PrintRouterData(aStream, mLocalRouterPtr);
   aStream << " Protocol Type: " << mProtocolPtr->GetType() << " added: " << *mAddedAddressPtr << '\n';
}

void CommAddedToLocal::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "COMM_ADDED_TO_LOCAL" << ',' << mLocalRouterPtr->GetPlatform()->GetName() << ','
           << mLocalRouterPtr->GetName() << ',' << mProtocolPtr->GetType() << ',' << *mAddedAddressPtr << '\n';
}

void CommRemovedFromLocal::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "COMM_REMOVED_FROM_LOCAL ";
   utils::PrintRouterData(aStream, mLocalRouterPtr);
   aStream << " Protocol Type: " << mProtocolPtr->GetType() << " removed: " << *mRemovedAddressPtr << '\n';
}

void CommRemovedFromLocal::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "COMM_REMOVED_FROM_LOCAL" << ',' << mLocalRouterPtr->GetPlatform()->GetName() << ','
           << mLocalRouterPtr->GetName() << ',' << mProtocolPtr->GetType() << ',' << *mRemovedAddressPtr << '\n';
}

void CommBroken::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_BROKEN ", " Comm: ", mCommPtr, mSettings);
}

void CommBroken::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_BROKEN", "Comm", mCommPtr);
}

void CommFrequencyChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "COMM_FREQUENCY_CHANGED " << mCommPtr->GetPlatform()->GetName() << " Comm: " << mCommPtr->GetName()
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " ";
   utils::PrintLocationData(aStream, mCommPtr->GetPlatform(), mSettings);

   // Frequency
   comm::ComponentHW* commHW_Ptr = comm::ComponentHW::Find(*mCommPtr);
   if (commHW_Ptr != nullptr)
   {
      double frequency = commHW_Ptr->GetEM_Xmtr(0).GetFrequency();
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "  Frequency Id: " << commHW_Ptr->GetEM_Xmtr(0).GetCurrentAlternateFrequencyId();
      if (frequency >= 1.0E+9)
      {
         aStream << " Frequency: " << frequency * 1.0E-9 << " ghz";
      }
      else if (frequency >= 1.0E+6)
      {
         aStream << " Frequency: " << frequency * 1.0E-6 << " mhz";
      }
      else if (frequency >= 1.0E+3)
      {
         aStream << " Frequency: " << frequency * 1.0E-3 << " khz";
      }
      else
      {
         aStream << " Frequency: " << frequency << " hz";
      }
   }
   aStream << '\n';
}

void CommFrequencyChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "COMM_FREQUENCY_CHANGED" << ',' << mCommPtr->GetPlatform()->GetName() << ','
           << mCommPtr->GetPlatform()->GetSide() << ',' << mCommPtr->GetName();
   utilsCSV::PrintLocationData(aStream, mCommPtr->GetPlatform());

   // Frequency
   comm::ComponentHW* commHW_Ptr = comm::ComponentHW::Find(*mCommPtr);
   if (commHW_Ptr != nullptr)
   {
      auto frequency = commHW_Ptr->GetEM_Xmtr(0).GetFrequency();
      aStream << ',' << commHW_Ptr->GetEM_Xmtr(0).GetCurrentAlternateFrequencyId();
      aStream << ',' << frequency;
   }
   aStream << '\n';
}

void CommNonOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_NON_OPERATIONAL ", " Comm: ", mCommPtr, mSettings);
}

void CommNonOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_NON_OPERATIONAL", "Comm", mCommPtr);
}

void CommOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_OPERATIONAL ", " Comm: ", mCommPtr, mSettings);
}

void CommOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_OPERATIONAL", "Comm", mCommPtr);
}

void CommTurnedOff::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_TURNED_OFF ", " Comm: ", mCommPtr, mSettings);
}

void CommTurnedOff::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_TURNED_OFF", " Comm", mCommPtr);
}

void CommTurnedOn::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_TURNED_ON ", " Comm: ", mCommPtr, mSettings);
}

void CommTurnedOn::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "COMM_TURNED_ON", " Comm", mCommPtr);
}

void CrashedIntoGround::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "CRASHED_INTO_GROUND " << mPlatformPtr->GetName() << '\n';
}

void CrashedIntoGround::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "CRASHED_INTO_GROUND" << ',' << mPlatformPtr->GetName() << ',' << mPlatformPtr->GetSide() << '\n';
}

void ExchangeCompleted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   size_t       callerIndex       = mQuery.OwningIndex();
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "<unknown>";
   WsfPlatform* requesterPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "<unknown>";
   WsfPlatform* responderPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }

   std::string eventName = (callerIndex == requesterIndex) ? "EXCHANGE_REQUEST_COMPLETED" : "EXCHANGE_RESPONSE_COMPLETED";
   aStream << eventName << " from " << responderPlatName << " to " << requesterPlatName << " ID=" << mQuery.ItemId()
           << " DESIRED=" << mQuery.DesiredQuantity() << " OFFERED=" << mQuery.OfferedQuantity()
           << " TRANSFERRED=" << mQuery.NegotiatedQuantity() << " STATUS=" << mQuery.GetResultString()
           << " CONTAINER=" << mTransactor.ContainerNameId()
           << " QUANTITY=" << mTransactor.ContainerPtr()->CurrentQuantity() << std::endl;
}

void ExchangeCompleted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   size_t       callerIndex    = mQuery.OwningIndex();
   std::string  callerPlatName = "UNKNOWN";
   WsfPlatform* callerPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(callerIndex);
   if (callerPlatPtr != nullptr)
   {
      callerPlatName = callerPlatPtr->GetName();
   }
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "UNKNOWN";
   WsfPlatform* requesterPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "UNKNOWN";
   WsfPlatform* responderPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }
   // clang-format off
   aStream << ',' << "EXCHANGE_COMPLETED"
           << ',' << mQuery.EventId()
           << ',' << callerPlatName
           << ',' << requesterPlatName
           << ',' << responderPlatName
           << ',' << mQuery.RequestId()
           << ',' << mQuery.ItemId()
           << ',' << mQuery.DesiredQuantity()
           << ',' << mQuery.OfferedQuantity()
           << ',' << mQuery.NegotiatedQuantity()
           << ',' << mTransactor.ContainerNameId()
           << ',' << mTransactor.ContainerPtr()->CurrentQuantity()
           << '\n';
   // clang-format on
}

void ExchangeNegotiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   size_t       callerIndex    = mQuery.OwningIndex();
   std::string  callerPlatName = "<unknown>";
   WsfPlatform* callerPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(callerIndex);
   if (callerPlatPtr != nullptr)
   {
      callerPlatName = callerPlatPtr->GetName();
   }
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "<unknown>";
   WsfPlatform* requesterPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "<unknown>";
   WsfPlatform* responderPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }

   std::string eventName = (callerIndex == requesterIndex) ? "EXCHANGE_REQUEST_NEGOTIATED" : "EXCHANGE_RESPONSE_NEGOTIATED";
   std::string queryTarget = (callerIndex == requesterIndex) ? responderPlatName : requesterPlatName;
   aStream << eventName << " from " << callerPlatName << " to " << queryTarget << " EVENT=" << mQuery.EventId()
           << " N=" << mQuery.RequestId() << " ID=" << mQuery.ItemId() << " DESIRED=" << mQuery.DesiredQuantity()
           << " OFFERED=" << mQuery.OfferedQuantity() << " NEGOTIATED=" << mQuery.NegotiatedQuantity()
           << " DESIRED_RATE=" << mQuery.DesiredRate() << " OFFERED_RATE=" << mQuery.OfferedRate()
           << " NEGOTIATED_RATE=" << mQuery.NegotiatedRate() << " STATUS=" << mQuery.GetResultString() << std::endl;
}

void ExchangeNegotiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   size_t       callerIndex    = mQuery.OwningIndex();
   std::string  callerPlatName = "UNKNOWN";
   WsfPlatform* callerPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(callerIndex);
   if (callerPlatPtr != nullptr)
   {
      callerPlatName = callerPlatPtr->GetName();
   }
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "UNKNOWN";
   WsfPlatform* requesterPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "UNKNOWN";
   WsfPlatform* responderPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }
   // clang-format off
   aStream << ',' << "EXCHANGE_NEGOTIATED"
           << ',' << mQuery.EventId()
           << ',' << callerPlatName
           << ',' << requesterPlatName
           << ',' << responderPlatName
           << ',' << mQuery.RequestId()
           << ',' << mQuery.ItemId()
           << ',' << mQuery.DesiredQuantity()
           << ',' << mQuery.OfferedQuantity()
           << ',' << mQuery.NegotiatedQuantity()
           << '\n';
   // clang-format on
}

void ExchangeQueried::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   size_t       callerIndex    = mQuery.OwningIndex();
   std::string  callerPlatName = "<unknown>";
   WsfPlatform* callerPlatPtr  = mQuery.GetSimulation()->GetPlatformByIndex(callerIndex);
   if (callerPlatPtr != nullptr)
   {
      callerPlatName = callerPlatPtr->GetName();
   }
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "<unknown>";
   WsfPlatform* requesterPlatPtr  = mQuery.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "<unknown>";
   WsfPlatform* responderPlatPtr  = mQuery.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }

   std::string eventName   = (callerIndex == requesterIndex) ? "EXCHANGE_REQUEST_QUERIED" : "EXCHANGE_RESPONSE_QUERIED";
   std::string queryTarget = (callerIndex == requesterIndex) ? responderPlatName : requesterPlatName;
   aStream << eventName << " from " << callerPlatName << " to " << queryTarget << " EVENT=" << mQuery.EventId()
           << " N=" << mQuery.RequestId() << " ID=" << mQuery.ItemId() << " DESIRED=" << mQuery.DesiredQuantity()
           << " OFFERED=" << mQuery.OfferedQuantity() << " NEGOTIATED=" << mQuery.NegotiatedQuantity()
           << " DESIRED_RATE=" << mQuery.DesiredRate() << " OFFERED_RATE=" << mQuery.OfferedRate()
           << " NEGOTIATED_RATE=" << mQuery.NegotiatedRate() << " STATUS=" << mQuery.GetResultString() << std::endl;
}

void ExchangeQueried::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   size_t       callerIndex    = mQuery.OwningIndex();
   std::string  callerPlatName = "UNKNOWN";
   WsfPlatform* callerPlatPtr  = mQuery.GetSimulation()->GetPlatformByIndex(callerIndex);
   if (callerPlatPtr != nullptr)
   {
      callerPlatName = callerPlatPtr->GetName();
   }
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "UNKNOWN";
   WsfPlatform* requesterPlatPtr  = mQuery.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "UNKNOWN";
   WsfPlatform* responderPlatPtr  = mQuery.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }
   // clang-format off
   aStream << ',' << "EXCHANGE_QUERIED"
           << ',' << mQuery.EventId()
           << ',' << callerPlatName
           << ',' << requesterPlatName
           << ',' << responderPlatName
           << ',' << mQuery.RequestId()
           << ',' << mQuery.ItemId()
           << ',' << mQuery.DesiredQuantity()
           << ',' << mQuery.OfferedQuantity()
           << ',' << mQuery.NegotiatedQuantity()
           << '\n';
   // clang-format on
}

void ExchangeRequestFailed::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "<unknown>";
   WsfPlatform* requesterPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "<unknown>";
   WsfPlatform* responderPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }

   std::string eventName = "EXCHANGE_REQUEST_FAILED";
   aStream << eventName << " from " << requesterPlatName << " to " << responderPlatName << " ID=" << mQuery.ItemId()
           << " DESIRED=" << mQuery.DesiredQuantity() << " STATUS=" << mQuery.GetResultString() << std::endl;
}

void ExchangeRequestFailed::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   size_t       requesterIndex    = mQuery.RequestingIndex();
   std::string  requesterPlatName = "UNKNOWN";
   WsfPlatform* requesterPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(requesterIndex);
   if (requesterPlatPtr != nullptr)
   {
      requesterPlatName = requesterPlatPtr->GetName();
   }
   size_t       responderIndex    = mQuery.RespondingIndex();
   std::string  responderPlatName = "UNKNOWN";
   WsfPlatform* responderPlatPtr  = mTransactor.GetSimulation()->GetPlatformByIndex(responderIndex);
   if (responderPlatPtr != nullptr)
   {
      responderPlatName = responderPlatPtr->GetName();
   }
   // clang-format off
   aStream << ',' << "EXCHANGE_REQUEST_FAILED"
           << ',' << mQuery.EventId()
           << ',' << requesterPlatName
           << ',' << responderPlatName
           << ',' << mQuery.RequestId()
           << ',' << mQuery.ItemId()
           << ',' << mQuery.DesiredQuantity()
           << '\n';
   // clang-format on
}

void ExecuteCallback::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "EXECUTE_CALLBACK " << mPlatformPtr->GetName() << ' ' << mCallback.GetName() << '\n';
}

void ExecuteCallback::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "EXECUTE_CALLBACK" << ',' << mPlatformPtr->GetName() << ',' << mPlatformPtr->GetSide() << ','
           << mCallback.GetName() << '\n';
}

void FuelEvent::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "FUEL_EVENT " << mFuelPtr->GetPlatform()->GetName() << ' ' << mEventNameId << '\n';
}

void FuelEvent::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "FUEL_EVENT" << ',' << mFuelPtr->GetPlatform()->GetName() << ','
           << mFuelPtr->GetPlatform()->GetSide() << ',' << mEventNameId.GetString() << '\n';
}

void ImageCreated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "IMAGE_CREATED " << mSensorPtr->GetPlatform()->GetName() << " Sensor: " << mSensorPtr->GetName();

   if (!mImagePtr->GetSensorModeId().IsNull())
   {
      aStream << " Mode: " << mImagePtr->GetSensorModeId();
   }
   else
   {
      aStream << " Mode: <none>";
   }
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Creation_Time: " << mImagePtr->GetImageTime();
   aStream << " Number: " << mImagePtr->GetImageNumber();
   aStream << " Size: " << mImagePtr->GetMessageLength() << " bits";

   double originLocWCS[3] = {0.0, 0.0, 0.0};
   double lat;
   double lon;
   double alt;
   mImagePtr->GetOriginatorLocationWCS(originLocWCS);
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Originator:";
   if (mSettings.PrintLLA_Locations())
   {
      UtEntity::ConvertWCSToLLA(originLocWCS, lat, lon, alt);
      utils::PrintLocationDataLLA(aStream, lat, lon, alt, mSettings.GetLatLonFormat());
   }
   else
   {
      // todo eci
   }

   double centerLocWCS[3];
   mImagePtr->GetCenterLocationWCS(centerLocWCS);
   if (!UtVec3d::Equals(centerLocWCS, 0.0))
   {
      UtEntity::ConvertWCSToLLA(centerLocWCS, lat, lon, alt);
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Center:";
      utils::PrintLocationDataLLA(aStream, lat, lon, alt, mSettings.GetLatLonFormat());
      // todo eci
   }

   if (((mImagePtr->GetWidth() > 0.0) && (mImagePtr->GetHeight() > 0.0)) ||
       ((mImagePtr->GetWidthResolution() > 0.0) && (mImagePtr->GetHeightResolution() > 0.0)))
   {
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
      if ((mImagePtr->GetWidth() > 0.0) && (mImagePtr->GetHeight() > 0.0))
      {
         aStream << "  Width: " << mImagePtr->GetWidth() << " Height: " << mImagePtr->GetHeight();
      }
      if ((mImagePtr->GetWidthResolution() > 0.0) && (mImagePtr->GetHeightResolution() > 0.0))
      {
         aStream << "  Width_Resolution: " << mImagePtr->GetWidthResolution() << " m"
                 << " Height_Resolution: " << mImagePtr->GetHeightResolution() << " m";
      }
      aStream << "  Collection_Factor: " << mImagePtr->GetCollectionFactor();
   }

   if ((mImagePtr->GetNoiseLevel() > 0.0) || (mImagePtr->GetBackgroundLevel() > 0.0) ||
       (mImagePtr->GetMinimumLevel() > 0.0) || (mImagePtr->GetMaximumLevel() > 0.0))
   {
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
      if (mImagePtr->GetNoiseLevel() > 0.0)
      {
         aStream << "  Noise_Level: " << UtMath::LinearToDB(mImagePtr->GetNoiseLevel()) << " dB";
      }
      if (mImagePtr->GetBackgroundLevel() > 0.0)
      {
         aStream << "  Background_Level: " << UtMath::LinearToDB(mImagePtr->GetBackgroundLevel()) << " dB";
      }
      if (mImagePtr->GetMinimumLevel() > 0.0)
      {
         aStream << "  Minimum_Level: " << UtMath::LinearToDB(mImagePtr->GetMinimumLevel()) << " dB";
      }
      if (mImagePtr->GetMaximumLevel() > 0.0)
      {
         aStream << "  Maximum_Level: " << UtMath::LinearToDB(mImagePtr->GetMaximumLevel()) << " dB";
      }
   }

   const WsfImage::ObjectList& imageObjects    = mImagePtr->GetObjects();
   size_t                      numberOfObjects = imageObjects.size();
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Object_Count: " << numberOfObjects;
   for (size_t i = 0; i < numberOfObjects; ++i)
   {
      size_t      truthIndex = imageObjects[i].GetTruthIndex();
      WsfStringId truthName  = mSensorPtr->GetSimulation()->GetPlatformNameId(truthIndex);
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Object: Name: " << truthName
              << " Type: " << imageObjects[i].GetTruthType() << " Side: " << imageObjects[i].GetTruthSide();

      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << "    Pixel_Count: " << imageObjects[i].GetPixelCount();
      if (imageObjects[i].GetSignalLevel() > 0.0)
      {
         aStream << " Signal_Level: " << UtMath::LinearToDB(imageObjects[i].GetSignalLevel()) << " dB";
      }
      aStream << " Intensity: " << imageObjects[i].GetPixelIntensity();

      double locWCS[3] = {0.0, 0.0, 0.0};
      imageObjects[i].GetLocationWCS(locWCS);
      UtEntity::ConvertWCSToLLA(locWCS, lat, lon, alt);
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "    Perceived_Location:";
      utils::PrintLocationDataLLA(aStream, lat, lon, alt, mSettings.GetLatLonFormat());

      double relLocWCS[3] = {0.0, 0.0, 0.0};
      UtVec3d::Subtract(relLocWCS, originLocWCS, locWCS);
      double range = UtVec3d::Normalize(relLocWCS);
      utils::PrintEM_Range(aStream, " \\\n                       ", range, mSettings.PrintSingleLinePerEvent(), false);

      WsfPlatform* platformPtr = mSensorPtr->GetSimulation()->GetPlatformByIndex(truthIndex);
      if (platformPtr != nullptr)
      {
         // Make sure the target's platform location is up-to-date.
         // Making the call here ensures that there is no extra processing if
         // we do not use the event output.
         platformPtr->Update(mSimTime);
         double platformToTgtAz;
         double platformToTgtEl;
         platformPtr->ComputeAspect(relLocWCS, platformToTgtAz, platformToTgtEl);
         aStream << " Brg: " << utils::PrintableAngle(UtMath::NormalizeAngle0_TwoPi(platformToTgtAz)) << " deg"
                 << " El: " << utils::PrintableAngle(platformToTgtEl) << " deg";
      }
   }
   aStream << '\n';
}

void ImageCreated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "IMAGE_CREATED" << ',' << mSensorPtr->GetPlatform()->GetName() << ',' << mSensorPtr->GetName();
   aStream << ',';
   if (!mImagePtr->GetSensorModeId().IsNull())
   {
      aStream << mImagePtr->GetSensorModeId();
   }
   else
   {
      aStream << "none";
   }
   aStream << ',' << mImagePtr->GetImageTime();
   aStream << ',' << mImagePtr->GetImageNumber();
   aStream << ',' << mImagePtr->GetMessageLength();

   double originLocWCS[3] = {0.0, 0.0, 0.0};
   double lat;
   double lon;
   double alt;
   mImagePtr->GetOriginatorLocationWCS(originLocWCS);
   aStream << ',';
   /*if (PrintLLA_Locations())
   {*/
   UtEntity::ConvertWCSToLLA(originLocWCS, lat, lon, alt);
   utilsCSV::PrintLocationDataLLA(aStream, lat, lon, alt);
   /*}
   else
   {*/
   aStream << ",,,"; // TODO ECI
   //}

   double centerLocWCS[3];
   mImagePtr->GetCenterLocationWCS(centerLocWCS);
   if (!UtVec3d::Equals(centerLocWCS, 0.0))
   {
      UtEntity::ConvertWCSToLLA(centerLocWCS, lat, lon, alt);
      utilsCSV::PrintLocationDataLLA(aStream, lat, lon, alt);
      aStream << ",,,"; // TODO ECI
   }
   else
   {
      aStream << ",,,,,,";
   }

   if (((mImagePtr->GetWidth() > 0.0) && (mImagePtr->GetHeight() > 0.0)) ||
       ((mImagePtr->GetWidthResolution() > 0.0) && (mImagePtr->GetHeightResolution() > 0.0)))
   {
      aStream << ',';
      if ((mImagePtr->GetWidth() > 0.0) && (mImagePtr->GetHeight() > 0.0))
      {
         aStream << mImagePtr->GetWidth() << ',' << mImagePtr->GetHeight();
      }
      else
      {
         aStream << ',';
      }
      aStream << ',';
      if ((mImagePtr->GetWidthResolution() > 0.0) && (mImagePtr->GetHeightResolution() > 0.0))
      {
         aStream << mImagePtr->GetWidthResolution() << ',' << mImagePtr->GetHeightResolution();
      }
      else
      {
         aStream << ',';
      }
      aStream << ',' << mImagePtr->GetCollectionFactor();
   }
   else
   {
      aStream << ",,,,,";
   }

   if ((mImagePtr->GetNoiseLevel() > 0.0) || (mImagePtr->GetBackgroundLevel() > 0.0) ||
       (mImagePtr->GetMinimumLevel() > 0.0) || (mImagePtr->GetMaximumLevel() > 0.0))
   {
      aStream << ',';
      if (mImagePtr->GetNoiseLevel() > 0.0)
      {
         aStream << UtMath::LinearToDB(mImagePtr->GetNoiseLevel());
      }
      aStream << ',';
      if (mImagePtr->GetBackgroundLevel() > 0.0)
      {
         aStream << UtMath::LinearToDB(mImagePtr->GetBackgroundLevel());
      }
      aStream << ',';
      if (mImagePtr->GetMinimumLevel() > 0.0)
      {
         aStream << UtMath::LinearToDB(mImagePtr->GetMinimumLevel());
      }
      aStream << ',';
      if (mImagePtr->GetMaximumLevel() > 0.0)
      {
         aStream << UtMath::LinearToDB(mImagePtr->GetMaximumLevel());
      }
   }
   else
   {
      aStream << ",,,,";
   }

   const WsfImage::ObjectList& imageObjects    = mImagePtr->GetObjects();
   size_t                      numberOfObjects = imageObjects.size();
   aStream << ',' << numberOfObjects;

   for (size_t i = 0; i < numberOfObjects; ++i)
   {
      size_t      truthIndex = imageObjects[i].GetTruthIndex();
      WsfStringId truthName  = mSensorPtr->GetSimulation()->GetPlatformNameId(truthIndex);
      aStream << ',' << truthName << ',' << imageObjects[i].GetTruthType() << ',' << imageObjects[i].GetTruthSide();

      aStream << ',' << imageObjects[i].GetPixelCount() << ',';
      if (imageObjects[i].GetSignalLevel() > 0.0)
      {
         aStream << UtMath::LinearToDB(imageObjects[i].GetSignalLevel());
      }
      aStream << ',' << imageObjects[i].GetPixelIntensity();

      double locWCS[3] = {0.0, 0.0, 0.0};
      imageObjects[i].GetLocationWCS(locWCS);
      UtEntity::ConvertWCSToLLA(locWCS, lat, lon, alt);
      utilsCSV::PrintLocationDataLLA(aStream, lat, lon, alt);

      double relLocWCS[3] = {0.0, 0.0, 0.0};
      UtVec3d::Subtract(relLocWCS, originLocWCS, locWCS);
      double range = UtVec3d::Normalize(relLocWCS);
      utilsCSV::PrintEM_Range(aStream, range, false);

      WsfPlatform* platformPtr = mSensorPtr->GetSimulation()->GetPlatformByIndex(truthIndex);
      aStream << ',';
      if (platformPtr != nullptr)
      {
         // Make sure the target's platform location is up-to-date.
         // Making the call here ensures that there is no extra processing if
         // we do not use the event output.
         platformPtr->Update(mSimTime);
         double platformToTgtAz;
         double platformToTgtEl;
         platformPtr->ComputeAspect(relLocWCS, platformToTgtAz, platformToTgtEl);
         aStream << UtMath::NormalizeAngle0_TwoPi(platformToTgtAz) << ',' << platformToTgtEl;
      }
      else
      {
         aStream << ',';
      }
   }
   aStream << '\n';
}

void LinkAddedToManager::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_ADDED_TO_MANAGER ";
   utils::PrintCommData(aStream, mSourceCommPtr);
   aStream << " linked to: ";
   utils::PrintCommData(aStream, mDestinationCommPtr);
   aStream << '\n';
}

void LinkAddedToManager::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_ADDED_TO_MANAGER"
           << ',' << mSourceCommPtr->GetPlatform()->GetName()
           << ',' << mSourceCommPtr->GetName()
           << ',' << mSourceCommPtr->GetAddress()
           << ',' << mDestinationCommPtr->GetPlatform()->GetName()
           << ',' << mDestinationCommPtr->GetName()
           << ',' << mDestinationCommPtr->GetAddress()
           << '\n';
   // clang-format on
}

void LinkDisabledOnManager::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_DISABLED_ON_MANAGER ";
   utils::PrintCommData(aStream, mSourceCommPtr);
   aStream << " linked to: ";
   utils::PrintCommData(aStream, mDestinationCommPtr);
   aStream << '\n';
}

void LinkDisabledOnManager::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_DISABLED_ON_MANAGER"
           << ',' << mSourceCommPtr->GetPlatform()->GetName()
           << ',' << mSourceCommPtr->GetName()
           << ',' << mSourceCommPtr->GetAddress()
           << ',' << mDestinationCommPtr->GetPlatform()->GetName()
           << ',' << mDestinationCommPtr->GetName()
           << ',' << mDestinationCommPtr->GetAddress()
           << '\n';
   // clang-format on
}

void LinkEnabledOnManager::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_ENABLED_ON_MANAGER ";
   utils::PrintCommData(aStream, mSourceCommPtr);
   aStream << " linked to: ";
   utils::PrintCommData(aStream, mDestinationCommPtr);
   aStream << '\n';
}

void LinkEnabledOnManager::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_ENABLED_ON_MANAGER"
           << ',' << mSourceCommPtr->GetPlatform()->GetName()
           << ',' << mSourceCommPtr->GetName()
           << ',' << mSourceCommPtr->GetAddress()
           << ',' << mDestinationCommPtr->GetPlatform()->GetName()
           << ',' << mDestinationCommPtr->GetName()
           << ',' << mDestinationCommPtr->GetAddress()
           << '\n';
   // clang-format on
}

void LinkRemovedFromManager::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_REMOVED_FROM_MANAGER ";
   utils::PrintCommData(aStream, mSourceCommPtr);
   aStream << " linked to: ";
   utils::PrintCommData(aStream, mDestinationCommPtr);
   aStream << '\n';
}

void LinkRemovedFromManager::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_REMOVED_FROM_MANAGER"
           << ',' << mSourceCommPtr->GetPlatform()->GetName()
           << ',' << mSourceCommPtr->GetName()
           << ',' << mSourceCommPtr->GetAddress()
           << ',' << mDestinationCommPtr->GetPlatform()->GetName()
           << ',' << mDestinationCommPtr->GetName()
           << ',' << mDestinationCommPtr->GetAddress()
           << '\n';
   // clang-format on
}

void LinkAddedToLocal::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_ADDED_TO_LOCAL ";
   utils::PrintRouterData(aStream, mLocalRouterPtr);
   aStream << " Protocol Type: " << mProtocolPtr->GetType();
   aStream << " added: " << *mSourceAddressPtr;
   aStream << " linked to: " << *mDestinationAddressPtr;
   aStream << '\n';
}

void LinkAddedToLocal::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_ADDED_TO_LOCAL"
           << ',' << mLocalRouterPtr->GetPlatform()->GetName()
           << ',' << mLocalRouterPtr->GetName()
           << ',' << mProtocolPtr->GetType()
           << ',' << *mSourceAddressPtr
           << ',' << *mDestinationAddressPtr
           << '\n';
   // clang-format on
}

void LinkDisabledOnLocal::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_DISABLED_ON_LOCAL ";
   utils::PrintRouterData(aStream, mLocalRouterPtr);
   aStream << " Protocol Type: " << mProtocolPtr->GetType();
   aStream << " disabled: " << *mSourceAddressPtr;
   aStream << " linked to: " << *mDestinationAddressPtr;
   aStream << '\n';
}

void LinkDisabledOnLocal::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_DISABLED_ON_LOCAL"
           << ',' << mLocalRouterPtr->GetPlatform()->GetName()
           << ',' << mLocalRouterPtr->GetName()
           << ',' << mProtocolPtr->GetType()
           << ',' << *mSourceAddressPtr
           << ',' << *mDestinationAddressPtr
           << '\n';
   // clang-format on
}

void LinkEnabledOnLocal::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_ENABLED_ON_LOCAL ";
   utils::PrintRouterData(aStream, mLocalRouterPtr);
   aStream << " Protocol Type: " << mProtocolPtr->GetType();
   aStream << " enabled: " << *mSourceAddressPtr;
   aStream << " linked to: " << *mDestinationAddressPtr;
   aStream << '\n';
}

void LinkEnabledOnLocal::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_ENABLED_ON_LOCAL"
           << ',' << mLocalRouterPtr->GetPlatform()->GetName()
           << ',' << mLocalRouterPtr->GetName()
           << ',' << mProtocolPtr->GetType()
           << ',' << *mSourceAddressPtr
           << ',' << *mDestinationAddressPtr
           << '\n';
   // clang-format on
}

void LinkRemovedFromLocal::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LINK_REMOVED_FROM_LOCAL ";
   utils::PrintRouterData(aStream, mLocalRouterPtr);
   aStream << " Protocol Type: " << mProtocolPtr->GetType();
   aStream << " removed: " << *mSourceAddressPtr;
   aStream << " linked to: " << *mDestinationAddressPtr;
   aStream << '\n';
}

void LinkRemovedFromLocal::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LINK_REMOVED_FROM_LOCAL"
           << ',' << mLocalRouterPtr->GetPlatform()->GetName()
           << ',' << mLocalRouterPtr->GetName()
           << ',' << mProtocolPtr->GetType()
           << ',' << *mSourceAddressPtr
           << ',' << *mDestinationAddressPtr
           << '\n';
   // clang-format on
}

void LocalTrackCorrelation::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LOCAL_TRACK_CORRELATION " << mPlatformPtr->GetName();
   aStream << " Local_TrackId: " << mLocalTrackId;
   aStream << " NonLocal_TrackId: " << mNonLocalTrackId << '\n';
}

void LocalTrackCorrelation::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LOCAL_TRACK_CORRELATION"
           << ',' << mPlatformPtr->GetName()
           << ',' << mPlatformPtr->GetSide()
           << ',' << mLocalTrackId
           << ',' << mNonLocalTrackId
           << '\n';
   // clang-format on
}

void LocalTrackDecorrelation::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LOCAL_TRACK_DECORRELATION " << mPlatformPtr->GetName();
   aStream << " Local_TrackId: " << mLocalTrackId;
   aStream << " NonLocal_TrackId: " << mNonLocalTrackId << '\n';
}

void LocalTrackDecorrelation::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "LOCAL_TRACK_DECORRELATION"
           << ',' << mPlatformPtr->GetName()
           << ',' << mPlatformPtr->GetSide()
           << ',' << mLocalTrackId
           << ',' << mNonLocalTrackId
           << '\n';
   // clang-format on
}

void LocalTrackDropped::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LOCAL_TRACK_DROPPED " << mPlatformPtr->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mPlatformPtr->GetSimulation());
   aStream << " TrackId: " << mTrackPtr->GetTrackId() << '\n';
}

void LocalTrackDropped::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);

   aStream << ',' << "LOCAL_TRACK_DROPPED";
   aStream << ',' << mPlatformPtr->GetName();
   aStream << ',' << mPlatformPtr->GetSide();
   aStream << ',' << mTrackPtr->GetTrackId();
   utilsCSV::PrintTrackTargetName(aStream, mTrackPtr, *mPlatformPtr->GetSimulation());
   aStream << ',';
   if (mTrackPtr->GetTargetType() != 0)
   {
      aStream << mTrackPtr->GetTargetType();
   }
   aStream << '\n';
}

void LocalTrackInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LOCAL_TRACK_INITIATED " << mPlatformPtr->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mPlatformPtr->GetSimulation());
   aStream << " TrackId: " << mTrackPtr->GetTrackId();
   utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mPlatformPtr->GetSimulation(), mSettings);
   if (mSourcePtr != nullptr)
   {
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << " Source_TrackId: " << mSourcePtr->GetTrackId();
      if (mSourcePtr->GetSensorNameId() != 0)
      {
         aStream << " Sensor: " << mSourcePtr->GetSensorNameId() << " Type: " << mSourcePtr->GetSensorTypeId()
                 << " Mode: " << mSourcePtr->GetSensorModeId();
      }
      aStream << " Update_Time: " << UtTime(mSourcePtr->GetUpdateTime(), mSettings.GetTimeFormat());
      aStream << " Update_Count: " << mSourcePtr->GetUpdateCount();
      aStream << " Quality: " << mSourcePtr->GetTrackQuality();
   }
   aStream << '\n';
}

void LocalTrackInitiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "LOCAL_TRACK_INITIATED";
   aStream << ',' << mPlatformPtr->GetName();
   aStream << ',' << mPlatformPtr->GetSide();
   aStream << ',' << mTrackPtr->GetTrackId();
   utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mPlatformPtr->GetSimulation());
   if (mSourcePtr != nullptr)
   {
      aStream << ',' << mSourcePtr->GetTrackId();
      if (mSourcePtr->GetSensorNameId() != 0)
      {
         aStream << ',' << mSourcePtr->GetSensorNameId();
         aStream << ',' << mSourcePtr->GetSensorTypeId();
         aStream << ',' << mSourcePtr->GetSensorModeId();
      }
      else
      {
         aStream << ",,,";
      }
   }
   else
   {
      aStream << ",,,,";
   }
   aStream << '\n';
}

void LocalTrackUpdated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "LOCAL_TRACK_UPDATED " << mPlatformPtr->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mPlatformPtr->GetSimulation());
   aStream << " TrackId: " << mTrackPtr->GetTrackId();
   utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mPlatformPtr->GetSimulation(), mSettings);
   if (mSourcePtr != nullptr)
   {
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << " Source_TrackId: " << mSourcePtr->GetTrackId();
      if (mSourcePtr->GetSensorNameId() != 0)
      {
         aStream << " Sensor: " << mSourcePtr->GetSensorNameId() << " Type: " << mSourcePtr->GetSensorTypeId()
                 << " Mode: " << mSourcePtr->GetSensorModeId();
      }
      aStream << " Update_Time: " << UtTime(mSourcePtr->GetUpdateTime(), mSettings.GetTimeFormat());
      aStream << " Update_Count: " << mSourcePtr->GetUpdateCount();
      aStream << " Quality: " << mSourcePtr->GetTrackQuality();
   }
   aStream << '\n';
}

void LocalTrackUpdated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "LOCAL_TRACK_UPDATED";
   aStream << ',' << mPlatformPtr->GetName();
   aStream << ',' << mPlatformPtr->GetSide();
   aStream << ',' << mTrackPtr->GetTrackId();
   utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mPlatformPtr->GetSimulation());
   if (mSourcePtr != nullptr)
   {
      aStream << ',' << mSourcePtr->GetTrackId();
      if (mSourcePtr->GetSensorNameId() != 0)
      {
         aStream << ',' << mSourcePtr->GetSensorNameId();
         aStream << ',' << mSourcePtr->GetSensorTypeId();
         aStream << ',' << mSourcePtr->GetSensorModeId();
      }
      else
      {
         aStream << ",,,";
      }
   }
   else
   {
      aStream << ",,,,";
   }
   aStream << '\n';
}

void MessageDeliveryAttempt::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_DELIVERY_ATTEMPT";
   if (mXmtrPtr != nullptr)
   {
      aStream << ' ' << mXmtrPtr->GetPlatform()->GetName();
   }
   else
   {
      aStream << " UNKNOWN";
   }
   if (mRcvrPtr != nullptr)
   {
      aStream << ' ' << mRcvrPtr->GetPlatform()->GetName() << " System: " << mRcvrPtr->GetName();
   }
   else
   {
      aStream << " UNKNOWN System: UNKNOWN";
   }
   aStream << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits"
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());

   // Only print these if they have been given valid values
   if (mResult.mDataRate != -1)
   {
      aStream << "  DataRate: " << mResult.mDataRate << " bit/s";
      if (mResult.mEnergyBitToNoiseDensity != -1)
      {
         aStream << " Eb/No: " << UtMath::SafeLinearToDB(mResult.mEnergyBitToNoiseDensity) << " dB"
                 << " BER: " << mResult.mBitErrorRate;
      }
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   }

   utils::PrintEM_Interaction(aStream, mResult, mSettings);
   if (mResult.mCheckedStatus != 0)
   {
      if (mResult.mFailedStatus == 0)
      {
         aStream << "  Succeeded\n";
      }
      else
      {
         aStream << "  Failed\n";
      }
   }
}

void MessageDeliveryAttempt::PrintCSV(std::ostream& aStream) const
{
   // Suppress printing if the attempt was a failure and failed attempts are to be suppressed.
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_DELIVERY_ATTEMPT" << ',';
   if (mXmtrPtr != nullptr)
   {
      aStream << mXmtrPtr->GetPlatform()->GetName() << ',' << mXmtrPtr->GetPlatform()->GetSide();
   }
   else
   {
      aStream << "UNKNOWN,UNKNOWN";
   }
   aStream << ',';
   if (mRcvrPtr != nullptr)
   {
      aStream << mRcvrPtr->GetPlatform()->GetName() << ',' << mRcvrPtr->GetName();
   }
   else
   {
      aStream << "UNKNOWN,UNKNOWN";
   }
   aStream << ',' << mMessage.GetSerialNumber() << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType() << ',' << mMessage.GetSizeBits();

   // Only print these if they have been given valid values
   if (mResult.mDataRate != -1)
   {
      aStream << ',' << mResult.mDataRate;
      if (mResult.mEnergyBitToNoiseDensity != -1)
      {
         aStream << ',' << UtMath::SafeLinearToDB(mResult.mEnergyBitToNoiseDensity) << ',' << mResult.mBitErrorRate;
      }
      else
      {
         aStream << ",,";
      }
   }
   else
   {
      aStream << ",,,";
   }

   utilsCSV::PrintEM_Interaction(aStream, mResult);

   aStream << ',';
   if (mResult.mCheckedStatus != 0)
   {
      if (mResult.mFailedStatus == 0)
      {
         aStream << "Succeeded";
      }
      else
      {
         aStream << "Failed";
      }
   }
   aStream << '\n';
}

void MessageDiscarded::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_DISCARDED " << mCommPtr->GetPlatform()->GetName() << " System: " << mCommPtr->GetName()
           << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits"
           << " Result: " << mComment;
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mCommPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageDiscarded::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_DISCARDED";
   aStream << ',' << mCommPtr->GetPlatform()->GetName();
   aStream << ',' << mCommPtr->GetPlatform()->GetSide();
   aStream << ',' << mCommPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mCommPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageFailedRouting::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_FAILED_ROUTING " << mCommPtr->GetPlatform()->GetName() << " System: " << mCommPtr->GetName()
           << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits "
           << " Intended for Platform: " << mRcvrPlatform->GetName();
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mCommPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageFailedRouting::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_FAILED_ROUTING";
   aStream << ',' << mCommPtr->GetPlatform()->GetName();
   aStream << ',' << mCommPtr->GetPlatform()->GetSide();
   aStream << ',' << mCommPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ',' << mRcvrPlatform->GetName();
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mCommPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageHop::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_HOP " << mRcvrPtr->GetPlatform()->GetName();
   aStream << " System: " << mRcvrPtr->GetName();
   aStream << " Number: " << mMessage.GetSerialNumber();
   aStream << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType();
   aStream << " Size: " << mMessage.GetSizeBits() << " bits";
   if (mDstPtr)
   {
      aStream << " Destination: " << mDstPtr->GetFullName();
   }
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mRcvrPtr->GetSimulation(), mSettings);
   aStream << '\n';
}

void MessageHop::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_HOP";
   aStream << ',' << mRcvrPtr->GetPlatform()->GetName();
   aStream << ',' << mRcvrPtr->GetPlatform()->GetSide();
   aStream << ',' << mRcvrPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   if (mDstPtr)
   {
      aStream << ',' << mDstPtr->GetPlatform()->GetName();
      aStream << ',' << mDstPtr->GetName();
   }
   else
   {
      aStream << ',' << ',';
   }
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mRcvrPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageQueued::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_QUEUED " << mCommPtr->GetPlatform()->GetName() << " System: " << mCommPtr->GetName()
           << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits"
           << " Queue_Size: " << mQueueSize;
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mCommPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageQueued::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_QUEUED";
   aStream << ',' << mCommPtr->GetPlatform()->GetName();
   aStream << ',' << mCommPtr->GetPlatform()->GetSide();
   aStream << ',' << mCommPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ',' << mQueueSize;
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mCommPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageReceived::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_RECEIVED " << mRcvrPtr->GetPlatform()->GetName() << " System: " << mRcvrPtr->GetName()
           << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits";
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mRcvrPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageReceived::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_RECEIVED";
   aStream << ',' << mRcvrPtr->GetPlatform()->GetName();
   aStream << ',' << mRcvrPtr->GetPlatform()->GetSide();
   aStream << ',' << mRcvrPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mRcvrPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageTransmitEnded::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_TRANSMIT_ENDED " << mXmtrPtr->GetPlatform()->GetName() << " System: " << mXmtrPtr->GetName()
           << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits";
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mXmtrPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageTransmitEnded::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_TRANSMIT_ENDED";
   aStream << ',' << mXmtrPtr->GetPlatform()->GetName();
   aStream << ',' << mXmtrPtr->GetPlatform()->GetSide();
   aStream << ',' << mXmtrPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mXmtrPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageTransmitted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_TRANSMITTED " << mXmtrPtr->GetPlatform()->GetName() << " System: " << mXmtrPtr->GetName()
           << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits";
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mXmtrPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageTransmitted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_TRANSMITTED";
   aStream << ',' << mXmtrPtr->GetPlatform()->GetName();
   aStream << ',' << mXmtrPtr->GetPlatform()->GetSide();
   aStream << ',' << mXmtrPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mXmtrPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageTransmittedHeartbeat::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_TRANSMITTED_HEARTBEAT " << mXmtrPtr->GetPlatform()->GetName()
           << " System: " << mXmtrPtr->GetName() << " Number: " << mMessage.GetSerialNumber() << " DataTag: ";
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Type: " << mMessage.GetType() << " Size: " << mMessage.GetSizeBits() << " bits";
   utils::PrintMessageData(aStream, mSimTime, mMessage, *mXmtrPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageTransmittedHeartbeat::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_TRANSMITTED_HEARTBEAT";
   aStream << ',' << mXmtrPtr->GetPlatform()->GetName();
   aStream << ',' << mXmtrPtr->GetPlatform()->GetSide();
   aStream << ',' << mXmtrPtr->GetName();
   aStream << ',' << mMessage.GetSerialNumber();
   aStream << ',';
   mMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mMessage.GetType();
   aStream << ',' << mMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mMessage, *mXmtrPtr->GetSimulation(), mSettings.GetMessagePrinters(), false);
   aStream << '\n';
}

void MessageUpdated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MESSAGE_UPDATED " << mCommPtr->GetPlatform()->GetName() << " System: " << mCommPtr->GetName()
           << " Old_Number: " << mOldMessage.GetSerialNumber() << " Old_DataTag: ";
   mOldMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " Old_Type: " << mOldMessage.GetType() << " Old_Size: " << mOldMessage.GetSizeBits() << " bits";
   utils::PrintMessageData(aStream, mSimTime, mOldMessage, *mCommPtr->GetSimulation(), mSettings);
   aStream << " New_Number: " << mNewMessage.GetSerialNumber() << " New_DataTag: ";
   mNewMessage.PrintDataTagPrecise(aStream, 18);
   aStream << " New_Type: " << mNewMessage.GetType() << " New_Size: " << mNewMessage.GetSizeBits() << " bits";
   utils::PrintMessageData(aStream, mSimTime, mNewMessage, *mCommPtr->GetSimulation(), mSettings);
   aStream << "\n";
}

void MessageUpdated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MESSAGE_UPDATED";
   aStream << ',' << mCommPtr->GetPlatform()->GetName();
   aStream << ',' << mCommPtr->GetPlatform()->GetSide();
   aStream << ',' << mCommPtr->GetName();
   aStream << ',' << mOldMessage.GetSerialNumber();
   aStream << ',';
   mOldMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mOldMessage.GetType();
   aStream << ',' << mOldMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mOldMessage, *mCommPtr->GetSimulation(), mSettings.GetMessagePrinters(), true);
   aStream << ',' << mNewMessage.GetSerialNumber() << ',';
   mNewMessage.PrintDataTagPrecise(aStream, 18);
   aStream << ',' << mNewMessage.GetType();
   aStream << ',' << mNewMessage.GetSizeBits();
   aStream << ','; // queue size
   aStream << ','; // comment
   utilsCSV::PrintMessageData(aStream, mSimTime, mNewMessage, *mCommPtr->GetSimulation(), mSettings.GetMessagePrinters(), true);
   aStream << '\n';
}

void MoverBroken::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_BROKEN ", " Mover: ", mMoverPtr, mSettings);
}

void MoverBroken::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_BROKEN", "Mover", mMoverPtr);
}

void MoverBurnedOut::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_BURNED_OUT ", " Mover: ", mMoverPtr, mSettings);
}

void MoverBurnedOut::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_BURNED_OUT", "Mover", mMoverPtr);
}

void MoverNonOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_NON_OPERATIONAL ", " Mover: ", mMoverPtr, mSettings);
}

void MoverNonOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_NON_OPERATIONAL", "Mover", mMoverPtr);
}

void MoverOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_OPERATIONAL ", " Mover: ", mMoverPtr, mSettings);
}

void MoverOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_OPERATIONAL", "Mover", mMoverPtr);
}

void MoverStaged::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_STAGED ", " Mover: ", mMoverPtr, mSettings);
}

void MoverStaged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_STAGED", "Mover", mMoverPtr);
}

void MoverTurnedOff::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_TURNED_OFF ", " Mover: ", mMoverPtr, mSettings);
}

void MoverTurnedOff::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_TURNED_OFF", "Mover", mMoverPtr);
}

void MoverTurnedOn::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_TURNED_ON ", " Mover: ", mMoverPtr, mSettings);
}

void MoverTurnedOn::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "MOVER_TURNED_ON", "Mover", mMoverPtr);
}

void NavigationStatusChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "NAVIGATION_STATUS_CHANGED " << mPlatformPtr->GetName() << " Status: " << mStatus << '\n';
}

void NavigationStatusChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "NAVIGATION_STATUS_CHANGED";
   aStream << ',' << mPlatformPtr->GetName();
   aStream << ',' << mPlatformPtr->GetSide();
   aStream << ',' << mStatus;
   aStream << '\n';
}

void NetworkAdded::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "NETWORK_ADDED " << mNetworkPtr->GetType() << " Address: " << mNetworkPtr->GetAddress() << '\n';
}

void NetworkAdded::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "NETWORK_ADDED";
   aStream << ',' << mNetworkPtr->GetType();
   aStream << ',' << mNetworkPtr->GetAddress();
   aStream << '\n';
}

void NetworkRemoved::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "NETWORK_REMOVED " << mNetworkPtr->GetType() << " Address: " << mNetworkPtr->GetAddress() << '\n';
}

void NetworkRemoved::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "NETWORK_REMOVED";
   aStream << ',' << mNetworkPtr->GetType();
   aStream << ',' << mNetworkPtr->GetAddress();
   aStream << '\n';
}

void OperatingLevelChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "OPERATING_LEVEL_CHANGED " << mProcessorPtr->GetPlatform()->GetName() << ' ' << mProcessorPtr->GetName()
           << " Name: " << mName << " Level: " << mLevel << '\n';
}

void OperatingLevelChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "OPERATING_LEVEL_CHANGED"
           << ',' << mProcessorPtr->GetPlatform()->GetName()
           << ',' << mProcessorPtr->GetPlatform()->GetSide()
           << ',' << mProcessorPtr->GetName()
           << ',' << mName
           << ',' << mLevel
           << '\n';
   // clang-format on
}

void PlatformAdded::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_ADDED ";
   utils::PrintPlatformEvent(aStream, mPlatformPtr, false, mSettings);
   aStream << '\n';
}

void PlatformAdded::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "PLATFORM_ADDED" << ',';
   utilsCSV::PrintPlatformEvent(aStream, mPlatformPtr, false);
   aStream << '\n';
}

void PlatformAddedTeamName::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "TEAM_NAME_DEFINITION " << mPlatformPtr->GetName() << ' ' << mPlatformPtr->GetSide() << '\n';
}

void PlatformAddedTeamName::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "TEAM_NAME_DEFINITION";
   aStream << ',' << mPlatformPtr->GetName();
   aStream << ',' << mPlatformPtr->GetSide();
   aStream << '\n';
}

void PlatformAppearanceChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_APPEARANCE_CHANGED " << mPlatformPtr->GetName();
   if (mAppearanceType == WsfObserver::cAPPEARANCE_WORD)
   {
      aStream << " Item: appearance_word Value: 0x" << std::hex << std::setw(8) << std::setfill('0')
              << mPlatformPtr->GetAppearance() << std::dec << std::setfill(' ');
   }
   else if (mAppearanceType == WsfObserver::cCONCEALMENT_FACTOR)
   {
      aStream << " Item: concealment_factor Value: " << mPlatformPtr->GetConcealmentFactor();
   }
   else if ((mAppearanceType >= WsfObserver::cSIGNATURE_STATE_START) &&
            (mAppearanceType <= WsfObserver::cSIGNATURE_STATE_LIMIT))
   {
      WsfSignatureList& sigList(mPlatformPtr->GetSignatureList());
      auto              sigIndex(static_cast<size_t>(mAppearanceType - WsfObserver::cSIGNATURE_STATE_START));
#undef GetClassName // Avoid conflict with Windows macro
      aStream << " Item: " << sigList.GetClassName(sigIndex) << " Value: " << sigList.GetState(sigIndex);
   }
   else if ((mAppearanceType >= WsfObserver::cSIGNATURE_SCALE_FACTOR_START) &&
            (mAppearanceType <= WsfObserver::cSIGNATURE_SCALE_FACTOR_LIMIT))
   {
      WsfSignatureList& sigList(mPlatformPtr->GetSignatureList());
      auto              sigIndex(static_cast<size_t>(mAppearanceType - WsfObserver::cSIGNATURE_SCALE_FACTOR_START));
      aStream << " Item: " << sigList.GetClassName(sigIndex);
      aStream << " Value: " << UtMath::SafeLinearToDB(sigList.GetScaleFactor(sigIndex)) << " dB";
   }
   else
   {
      aStream << " item: <unknown> value: <unknown>";
   }
   aStream << '\n';
}

void PlatformAppearanceChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "PLATFORM_APPEARANCE_CHANGED" << ',' << mPlatformPtr->GetName() << ',' << mPlatformPtr->GetSide();
   if (mAppearanceType == WsfObserver::cAPPEARANCE_WORD)
   {
      aStream << ",0x" << std::hex << std::setw(8) << std::setfill('0') << mPlatformPtr->GetAppearance() << std::dec
              << std::setfill(' ');
   }
   else if (mAppearanceType == WsfObserver::cCONCEALMENT_FACTOR)
   {
      aStream << ",," << mPlatformPtr->GetConcealmentFactor();
   }
   else if ((mAppearanceType >= WsfObserver::cSIGNATURE_STATE_START) &&
            (mAppearanceType <= WsfObserver::cSIGNATURE_STATE_LIMIT))
   {
      WsfSignatureList& sigList(mPlatformPtr->GetSignatureList());
      auto              sigIndex(static_cast<size_t>(mAppearanceType - WsfObserver::cSIGNATURE_STATE_START));
      aStream << ",,," << sigList.GetClassName(sigIndex) << ',' << sigList.GetState(sigIndex);
   }
   else if ((mAppearanceType >= WsfObserver::cSIGNATURE_SCALE_FACTOR_START) &&
            (mAppearanceType <= WsfObserver::cSIGNATURE_SCALE_FACTOR_LIMIT))
   {
      WsfSignatureList& sigList(mPlatformPtr->GetSignatureList());
      auto              sigIndex(static_cast<size_t>(mAppearanceType - WsfObserver::cSIGNATURE_SCALE_FACTOR_START));
      aStream << ",,," << sigList.GetClassName(sigIndex) << ",,"
              << UtMath::SafeLinearToDB(sigList.GetScaleFactor(sigIndex));
   }
   else
   {
      aStream << ",,,,,";
   }
   aStream << '\n';
}

void PlatformBroken::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_BROKEN ";
   utils::PrintPlatformEvent(aStream, mPlatformPtr, true, mSettings);
   aStream << '\n';
}

void PlatformBroken::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "PLATFORM_BROKEN" << ',';
   utilsCSV::PrintPlatformEvent(aStream, mPlatformPtr, true);
   aStream << '\n';
}

void PlatformCapabilityChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   std::string change;
   if (mIsCapable)
   {
      change = "GAINED";
   }
   else
   {
      change = "LOST";
   }
   aStream << "PLATFORM_CAPABILITY_CHANGED " << mPlatformPtr->GetName() << ' ' << mCapabilityId << ' ' << change << '\n';
}

void PlatformCapabilityChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   std::string change = "LOST";
   if (mIsCapable)
   {
      change = "GAINED";
   }
   // clang-format off
   aStream << ',' << "PLATFORM_CAPABILITY_CHANGED"
           << ',' << mPlatformPtr->GetName()
           << ',' << mPlatformPtr->GetSide()
           << ',' << mCapabilityId
           << ',' << change
           << '\n';
   // clang-format on
}

void PlatformDeleted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_DELETED ";
   utils::PrintPlatformEvent(aStream, mPlatformPtr, true, mSettings);
   aStream << '\n';
}

void PlatformDeleted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "PLATFORM_DELETED" << ',';
   utilsCSV::PrintPlatformEvent(aStream, mPlatformPtr, true);
   aStream << '\n';
}

void PlatformInitialized::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_INITIALIZED ";
   utils::PrintPlatformEvent(aStream, mPlatformPtr, false, mSettings);
   aStream << '\n';
}

void PlatformInitialized::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "PLATFORM_INITIALIZED" << ',';
   utilsCSV::PrintPlatformEvent(aStream, mPlatformPtr, true);
   aStream << '\n';
}

void PlatformOmitted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_OMITTED ";
   utils::PrintPlatformEvent(aStream, mPlatformPtr, false, mSettings);
   aStream << '\n';
}

void PlatformOmitted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "PLATFORM_OMITTED" << ',';
   utilsCSV::PrintPlatformEvent(aStream, mPlatformPtr, false);
   aStream << '\n';
}

void ProcessorBroken::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_BROKEN ", " Processor: ", mProcessorPtr, mSettings);
}

void ProcessorBroken::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_BROKEN", "Processor", mProcessorPtr);
}

void ProcessorNonOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_NON_OPERATIONAL ", " Processor: ", mProcessorPtr, mSettings);
}

void ProcessorNonOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_NON_OPERATIONAL", "Processor", mProcessorPtr);
}

void ProcessorOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_OPERATIONAL ", " Processor: ", mProcessorPtr, mSettings);
}

void ProcessorOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_OPERATIONAL", "Processor", mProcessorPtr);
}

void ProcessorTurnedOff::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_TURNED_OFF ", " Processor: ", mProcessorPtr, mSettings);
}

void ProcessorTurnedOff::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_TURNED_OFF", "Processor", mProcessorPtr);
}

void ProcessorTurnedOn::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_TURNED_ON ", " Processor: ", mProcessorPtr, mSettings);
}

void ProcessorTurnedOn::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "PROCESSOR_TURNED_ON", "Processor", mProcessorPtr);
}

void RouterBroken::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_BROKEN ", " Router: ", mRouterPtr, mSettings);
}

void RouterBroken::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_BROKEN", " Router", mRouterPtr);
}

void RouterNonOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_NON_OPERATIONAL ", " Router: ", mRouterPtr, mSettings);
}

void RouterNonOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_NON_OPERATIONAL", " Router", mRouterPtr);
}

void RouterOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_OPERATIONAL ", " Router: ", mRouterPtr, mSettings);
}

void RouterOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_OPERATIONAL", " Router", mRouterPtr);
}

void RouterTurnedOff::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_TURNED_OFF ", " Router: ", mRouterPtr, mSettings);
}

void RouterTurnedOff::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_TURNED_OFF", " Router", mRouterPtr);
}

void RouterTurnedOn::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_TURNED_ON ", " Router: ", mRouterPtr, mSettings);
}

void RouterTurnedOn::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "ROUTER_TURNED_ON", " Router", mRouterPtr);
}

void SensorBroken::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_BROKEN ", " Sensor: ", mSensorPtr, mSettings);
}

void SensorBroken::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_BROKEN", "Sensor", mSensorPtr);
}

void SensorDetectionAttempt::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_DETECTION_ATTEMPT " << mSensorPtr->GetPlatform()->GetName() << ' ' << mTargetPtr->GetName();
   utils::PrintSensorResult(aStream, mSensorPtr, mResult, mSettings);
   aStream << '\n';
}

void SensorDetectionAttempt::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_DETECTION_ATTEMPT";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   aStream << ',' << mTargetPtr->GetName();
   utilsCSV::PrintSensorResult(aStream, mSensorPtr, mResult);
   aStream << '\n';
}

void SensorDetectionChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_DETECTION_CHANGED " << mSensorPtr->GetPlatform()->GetName();

   WsfStringId nameId = mSensorPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   if (nameId != 0)
   {
      aStream << ' ' << nameId;
   }
   else
   {
      aStream << " UNKNOWN-TARGET";
   }
   utils::PrintSensorResult(aStream, mSensorPtr, mResult, mSettings);
   aStream << '\n';
}

void SensorDetectionChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_DETECTION_CHANGED";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   WsfStringId nameId = mSensorPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   if (nameId != 0)
   {
      aStream << ',' << nameId;
   }
   else
   {
      aStream << ',';
   }
   utilsCSV::PrintSensorResult(aStream, mSensorPtr, mResult);
   aStream << '\n';
}

void SensorFrequencyChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_FREQUENCY_CHANGED " << mSensorPtr->GetPlatform()->GetName();
   aStream << " Sensor: " << mSensorPtr->GetName() << " Mode: " << mModePtr->GetName();
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " ";
   utils::PrintLocationData(aStream, mSensorPtr->GetPlatform(), mSettings);

   // Frequency details for each beam/xmtr configured for this mode
   size_t modeIndex = mModePtr->GetModeIndex();
   size_t xmtrCount = mSensorPtr->GetEM_XmtrCount(modeIndex);
   for (size_t index = 0; index < xmtrCount; ++index)
   {
      auto& xmtrRef = mSensorPtr->GetEM_Xmtr(modeIndex, index);
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << "  Beam: " << (xmtrRef.GetIndex() + 1);
      double frequency = xmtrRef.GetFrequency();
      aStream << " Frequency Id: " << xmtrRef.GetCurrentAlternateFrequencyId();
      if (frequency >= 1.0E+9)
      {
         aStream << " Frequency: " << frequency * 1.0E-9 << " GHz";
      }
      else if (frequency >= 1.0E+6)
      {
         aStream << " Frequency: " << frequency * 1.0E-6 << " MHz";
      }
      else if (frequency >= 1.0E+3)
      {
         aStream << " Frequency: " << frequency * 1.0E-3 << " kHz";
      }
      else
      {
         aStream << " Frequency: " << frequency << " Hz";
      }
   }
   aStream << '\n';
}

void SensorFrequencyChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   size_t modeIndex = mModePtr->GetModeIndex();
   size_t xmtrCount = mSensorPtr->GetEM_XmtrCount(modeIndex);
   aStream << ',' << "SENSOR_FREQUENCY_CHANGED" << ',' << mSensorPtr->GetPlatform()->GetName() << ','
           << mSensorPtr->GetPlatform()->GetSide() << ',' << mSensorPtr->GetName() << ',' << mModePtr->GetName();
   utilsCSV::PrintLocationData(aStream, mSensorPtr->GetPlatform());
   aStream << ',' << xmtrCount;

   // Frequency details for each beam/xmtr configured for this mode
   for (size_t index = 0; index < xmtrCount; ++index)
   {
      auto& xmtrRef = mSensorPtr->GetEM_Xmtr(modeIndex, index);
      aStream << ',' << (xmtrRef.GetIndex() + 1);
      aStream << ',' << xmtrRef.GetCurrentAlternateFrequencyId();
      aStream << ',' << xmtrRef.GetFrequency();
   }

   if (xmtrCount == 0)
   {
      aStream << ",,,";
   }
   aStream << '\n';
}

void SensorModeActivated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_MODE_ACTIVATED " << mSensorPtr->GetPlatform()->GetName();
   aStream << " Sensor: " << mSensorPtr->GetName() << " Mode: " << mModePtr->GetName();
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " ";
   utils::PrintLocationData(aStream, mSensorPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void SensorModeActivated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_MODE_ACTIVATED";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   aStream << ',' << mSensorPtr->GetName();
   aStream << ',' << mModePtr->GetName();
   utilsCSV::PrintLocationData(aStream, mSensorPtr->GetPlatform());
   aStream << '\n';
}

void SensorModeDeactivated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_MODE_DEACTIVATED " << mSensorPtr->GetPlatform()->GetName() << " Sensor: " << mSensorPtr->GetName()
           << " Mode: " << mModePtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " ";
   utils::PrintLocationData(aStream, mSensorPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void SensorModeDeactivated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_MODE_DEACTIVATED";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   aStream << ',' << mSensorPtr->GetName();
   aStream << ',' << mModePtr->GetName();
   utilsCSV::PrintLocationData(aStream, mSensorPtr->GetPlatform());
   aStream << '\n';
}

void SensorNonOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_NON_OPERATIONAL ", " Sensor: ", mSensorPtr, mSettings);
}

void SensorNonOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_NON_OPERATIONAL", "Sensor", mSensorPtr);
}

void SensorOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_OPERATIONAL ", " Sensor: ", mSensorPtr, mSettings);
}

void SensorOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_OPERATIONAL", "Sensor", mSensorPtr);
}

void SensorRequestCanceled::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_REQUEST_CANCELED " << mSensorPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
   aStream << " Sensor: " << mSensorPtr->GetName() << " TrackId: " << mTrackPtr->GetTrackId() << '\n';
}

void SensorRequestCanceled::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_REQUEST_CANCELED";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   aStream << ',' << mSensorPtr->GetName();
   aStream << ',' << mTrackPtr->GetTrackId();
   aStream << ",,,,,,,,,,";
   utilsCSV::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
   aStream << '\n';
}

void SensorRequestInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_REQUEST_INITIATED " << mSensorPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
   aStream << " Sensor: " << mSensorPtr->GetName() << " Mode: " << mModePtr->GetName()
           << " TrackId: " << mTrackPtr->GetTrackId();
   utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation(), mSettings);
   aStream << '\n';
}

void SensorRequestInitiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_REQUEST_INITIATED";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   aStream << ',' << mSensorPtr->GetName();
   aStream << ',' << mModePtr->GetName();
   aStream << ',' << mTrackPtr->GetTrackId();
   utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation());
   aStream << '\n';
}

void SensorRequestUpdated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SENSOR_REQUEST_UPDATED " << mSensorPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
   aStream << " Sensor: " << mSensorPtr->GetName() << " Mode: " << mModePtr->GetName()
           << " TrackId: " << mTrackPtr->GetTrackId();
   utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation(), mSettings);
   aStream << '\n';
}

void SensorRequestUpdated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SENSOR_REQUEST_UPDATED";
   aStream << ',' << mSensorPtr->GetPlatform()->GetName();
   aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
   aStream << ',' << mSensorPtr->GetName();
   aStream << ',' << mModePtr->GetName();
   aStream << ',' << mTrackPtr->GetTrackId();
   utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation());
   aStream << '\n';
}

void SensorTrackCoasted::Print(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
      aStream << "SENSOR_TRACK_COASTED " << mSensorPtr->GetPlatform()->GetName();
      utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << " Sensor: " << mSensorPtr->GetName() << " TrackId: " << mTrackPtr->GetTrackId();
      utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation(), mSettings);
      aStream << '\n';
   }
}

void SensorTrackCoasted::PrintCSV(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "SENSOR_TRACK_COASTED";
      aStream << ',' << mSensorPtr->GetPlatform()->GetName();
      aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
      aStream << ',' << mSensorPtr->GetName();
      aStream << ',' << mTrackPtr->GetTrackId();
      utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << '\n';
   }
}

void SensorTrackDropped::Print(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
      aStream << "SENSOR_TRACK_DROPPED " << mSensorPtr->GetPlatform()->GetName();
      utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << " Sensor: " << mSensorPtr->GetName() << " TrackId: " << mTrackPtr->GetTrackId() << '\n';
   }
}

void SensorTrackDropped::PrintCSV(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "SENSOR_TRACK_DROPPED";
      aStream << ',' << mSensorPtr->GetPlatform()->GetName();
      aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
      aStream << ',' << mSensorPtr->GetName();
      aStream << ',' << mTrackPtr->GetTrackId();
      utilsCSV::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << ',';
      if (mTrackPtr->GetTargetType() != 0)
      {
         aStream << mTrackPtr->GetTargetType();
      }
      aStream << '\n';
   }
}

void SensorTrackInitiated::Print(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
      aStream << "SENSOR_TRACK_INITIATED " << mSensorPtr->GetPlatform()->GetName();
      utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << " Sensor: " << mSensorPtr->GetName() << " TrackId: " << mTrackPtr->GetTrackId();
      utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation(), mSettings);
      aStream << '\n';
   }
}

void SensorTrackInitiated::PrintCSV(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "SENSOR_TRACK_INITIATED";
      aStream << ',' << mSensorPtr->GetPlatform()->GetName();
      aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
      aStream << ',' << mSensorPtr->GetName();
      aStream << ',' << mTrackPtr->GetTrackId();
      utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << '\n';
   }
}

void SensorTrackUpdated::Print(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
      aStream << "SENSOR_TRACK_UPDATED " << mSensorPtr->GetPlatform()->GetName();
      utils::PrintTrackTargetName(aStream, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << " Sensor: " << mSensorPtr->GetName() << " TrackId: " << mTrackPtr->GetTrackId();
      utils::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation(), mSettings);
      aStream << '\n';
   }
}

void SensorTrackUpdated::PrintCSV(std::ostream& aStream) const
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (mTrackPtr->GetTrackType() != WsfTrack::cPSEUDO_SENSOR)
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "SENSOR_TRACK_UPDATED";
      aStream << ',' << mSensorPtr->GetPlatform()->GetName();
      aStream << ',' << mSensorPtr->GetPlatform()->GetSide();
      aStream << ',' << mSensorPtr->GetName();
      aStream << ',' << mTrackPtr->GetTrackId();
      utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, *mSensorPtr->GetSimulation());
      aStream << '\n';
   }
}

void SensorTurnedOff::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_TURNED_OFF ", " Sensor: ", mSensorPtr, mSettings);
}

void SensorTurnedOff::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_TURNED_OFF", "Sensor", mSensorPtr);
}

void SensorTurnedOn::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_TURNED_ON ", " Sensor: ", mSensorPtr, mSettings);
}

void SensorTurnedOn::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "SENSOR_TURNED_ON", "Sensor", mSensorPtr);
}

void SimulationComplete::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SIMULATION_COMPLETE ";
   utils::PrintDateTime(aStream, mSimulation);
   aStream << '\n';
}

void SimulationComplete::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SIMULATION_COMPLETE" << ',';
   utilsCSV::PrintDateTime(aStream, mSimTime, mSimulation);
   aStream << '\n';
}

void SimulationStarting::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "SIMULATION_STARTING ";
   utils::PrintDateTime(aStream, mSimulation);
   aStream << '\n';
}

void SimulationStarting::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "SIMULATION_STARTING" << ',';
   utilsCSV::PrintDateTime(aStream, mSimTime, mSimulation);
   aStream << '\n';
}

void StateEntry::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "STATE_ENTRY " << mProcessorPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mProcessorPtr->GetSimulation());
   if (mStateNameId > nullptr)
   {
      aStream << " State: " << mStateNameId;
   }
   else
   {
      aStream << " State: <DROPPED>";
   }
   aStream << " Processor: " << mProcessorPtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintLocationData(aStream, mProcessorPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void StateEntry::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "STATE_ENTRY";
   aStream << ',' << mProcessorPtr->GetPlatform()->GetName();
   aStream << ',' << mProcessorPtr->GetPlatform()->GetSide();
   utilsCSV::PrintTrackTargetName(aStream, mTrackPtr, *mProcessorPtr->GetSimulation());
   if (mStateNameId > nullptr)
   {
      aStream << ',' << mStateNameId;
   }
   else
   {
      aStream << ',' << "DROPPED";
   }
   aStream << ',' << mProcessorPtr->GetName();
   utilsCSV::PrintLocationData(aStream, mProcessorPtr->GetPlatform());
   aStream << '\n';
}

void StateExit::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "STATE_EXIT " << mProcessorPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTrackPtr, *mProcessorPtr->GetSimulation());
   if (mStateNameId > nullptr)
   {
      aStream << " State: " << mStateNameId;
   }
   else
   {
      aStream << " State: <DROPPED>";
   }
   aStream << " Processor: " << mProcessorPtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintLocationData(aStream, mProcessorPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void StateExit::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "STATE_EXIT";
   aStream << ',' << mProcessorPtr->GetPlatform()->GetName();
   aStream << ',' << mProcessorPtr->GetPlatform()->GetSide();
   utilsCSV::PrintTrackTargetName(aStream, mTrackPtr, *mProcessorPtr->GetSimulation());
   if (mStateNameId > nullptr)
   {
      aStream << ',' << mStateNameId;
   }
   else
   {
      aStream << ',' << "DROPPED";
   }
   aStream << ',' << mProcessorPtr->GetName();
   utilsCSV::PrintLocationData(aStream, mProcessorPtr->GetPlatform());
   aStream << '\n';
}

void TankingEvent::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "TANKING_EVENT " << mFuelingOpPtr->SupplyTankPtr()->GetPlatform()->GetName() << " TO "
           << mFuelingOpPtr->ReceiveTankPtr()->GetPlatform()->GetName() << ' ' << mEventNameId << '\n';
}

void TankingEvent::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "TANKING_EVENT";
   aStream << ',' << mFuelingOpPtr->SupplyTankPtr()->GetPlatform()->GetName();
   aStream << ',' << mFuelingOpPtr->SupplyTankPtr()->GetPlatform()->GetSide();
   aStream << ',' << mFuelingOpPtr->ReceiveTankPtr()->GetPlatform()->GetName();
   aStream << ',' << mEventNameId;
   aStream << '\n';
}

void TaskAssigned::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "TASK_ASSIGNED" << ' ' << mTaskPtr->GetAssigneePlatformName() << ' ' << mTaskPtr->GetTargetName() << ' '
           << mTaskPtr->GetAssignerPlatformName() << " Task_Type: " << mTaskPtr->GetTaskType()
           << " Resource: " << mTaskPtr->GetResourceName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << " TrackId: " << mTaskPtr->GetTrackId();
   if ((mTrackPtr != nullptr) && (!mTrackPtr->GetTrackId().IsNull()))
   {
      const auto& sim = *mTrackPtr->GetSimulation();
      utils::PrintTrackData(aStream, mSimTime, mTrackPtr, sim, mSettings);
      WsfPlatform* assigneePtr = sim.GetPlatformByIndex(mTaskPtr->GetAssigneePlatformIndex());
      if (assigneePtr != nullptr)
      {
         double trackLocWCS[3];
         if (mTrackPtr->GetExtrapolatedLocationWCS(mSimTime, trackLocWCS))
         {
            double trackLocNED[3];
            assigneePtr->ConvertWCSToNED(trackLocWCS, trackLocNED);
            double bearing = atan2(trackLocNED[1], trackLocNED[0]);
            aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
                    << " Assignee->Target_Range: " << UtVec3d::Magnitude(trackLocNED) << " m"
                    << " True_Bearing: " << utils::PrintableAngle(UtMath::NormalizeAngle0_TwoPi(bearing)) << " deg";
         }
      }
   }
   aStream << '\n';
}

void TaskAssigned::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "TASK_ASSIGNED"
           << ',' << mTaskPtr->GetAssigneePlatformName()
           << ',' << mTaskPtr->GetAssignerPlatformName()
           << ',' << mTaskPtr->GetTaskType()
           << ',' /*STATUS*/
           << ',' << mTaskPtr->GetAssignTime()
           << ',' << mTaskPtr->GetUpdateTime()
           << ',' << mTaskPtr->GetResourceName()
           << ',' << mTaskPtr->GetTargetName()
           << ',' << mTaskPtr->GetTrackId();
   // clang-format on

   // Track Data
   if ((mTrackPtr != nullptr) && (!mTrackPtr->GetTrackId().IsNull()))
   {
      const auto& sim = *mTrackPtr->GetSimulation();
      utilsCSV::PrintTrackData(aStream, mSimTime, mTrackPtr, sim);
      WsfPlatform* assigneePtr = sim.GetPlatformByIndex(mTaskPtr->GetAssigneePlatformIndex());
      if (assigneePtr != nullptr)
      {
         double trackLocWCS[3];
         if (mTrackPtr->GetExtrapolatedLocationWCS(mSimTime, trackLocWCS))
         {
            double trackLocNED[3];
            assigneePtr->ConvertWCSToNED(trackLocWCS, trackLocNED);
            double bearing = atan2(trackLocNED[1], trackLocNED[0]);
            aStream << ',' << UtVec3d::Magnitude(trackLocNED) << ',' << UtMath::NormalizeAngle0_TwoPi(bearing);
         }
         else
         {
            aStream << ",,";
         }
      }
      else
      {
         aStream << ",,";
      }
   }
   aStream << '\n';
}

void TaskCanceled::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   // clang-format off
   aStream << "TASK_CANCELED" << ' ' << mTaskPtr->GetAssigneePlatformName() << ' ' << mTaskPtr->GetTargetName()
           << ' ' << mTaskPtr->GetAssignerPlatformName() << " Task_Type: " << mTaskPtr->GetTaskType()
           << " Resource: " << mTaskPtr->GetResourceName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
           << " TrackId: " << mTaskPtr->GetTrackId()
           << " Time_Assigned: " << UtTime(mTaskPtr->GetAssignTime(), mSettings.GetTimeFormat())
           << " Time_Updated: " << UtTime(mTaskPtr->GetUpdateTime(), mSettings.GetTimeFormat()) << '\n';
   // clang-format on
}

void TaskCanceled::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "TASK_CANCELED"
           << ',' << mTaskPtr->GetAssigneePlatformName()
           << ',' << mTaskPtr->GetAssignerPlatformName()
           << ',' << mTaskPtr->GetTaskType()
           << ',' /*STATUS*/
           << ',' << mTaskPtr->GetAssignTime()
           << ',' << mTaskPtr->GetUpdateTime()
           << ',' << mTaskPtr->GetResourceName()
           << ',' << mTaskPtr->GetTargetName()
           << ',' << mTaskPtr->GetTrackId()
           << '\n';
   // clang-format on
}

void TaskCompleted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   // clang-format off
   aStream << "TASK_COMPLETED" << ' ' << mTaskPtr->GetAssigneePlatformName() << ' ' << mTaskPtr->GetTargetName()
           << ' ' << mTaskPtr->GetAssignerPlatformName() << ' ' << mStatus
           << " Task_Type: " << mTaskPtr->GetTaskType() << " Resource: " << mTaskPtr->GetResourceName()
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " TrackId: " << mTaskPtr->GetTrackId()
           << " Time_Assigned: " << UtTime(mTaskPtr->GetAssignTime(), mSettings.GetTimeFormat())
           << " Time_Updated: " << UtTime(mTaskPtr->GetUpdateTime(), mSettings.GetTimeFormat()) << '\n';
   // clang-format on
}

void TaskCompleted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "TASK_COMPLETED"
           << ',' << mTaskPtr->GetAssigneePlatformName()
           << ',' << mTaskPtr->GetAssignerPlatformName()
           << ',' << mTaskPtr->GetTaskType()
           << ',' << mStatus
           << ',' << mTaskPtr->GetAssignTime()
           << ',' << mTaskPtr->GetUpdateTime()
           << ',' << mTaskPtr->GetResourceName()
           << ',' << mTaskPtr->GetTargetName()
           << ',' << mTaskPtr->GetTrackId()
           << '\n';
   // clang-format on
}

} // namespace event
} // namespace wsf
