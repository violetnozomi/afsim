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

#include "WsfXIO_EventResults.hpp"

#include <iomanip>
#include <iostream>
#include <ostream>

#include "GenIP.hpp"
#include "GenInternetSocketAddress.hpp"
#include "GenSocket.hpp"
#include "PakUDP_IO.hpp"
#include "WsfEventOutput.hpp"
#include "WsfEventUtils.hpp"
#include "WsfXIO_EventUtils.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"


namespace wsf
{
namespace event
{

void HandleConnect::Print(std::ostream& aStream) const
{
   {
      utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
      aStream << "XIO_CONNECT ID: ";
      if (mConnectionPtr->IsReliable())
      {
         utils::Write(aStream, mConnectionPtr->GetApplicationId());
         aStream << " Name: " << mConnectionPtr->GetApplicationName();
         aStream << " Type: " << mConnectionPtr->GetApplicationType();
         WsfXIO_Connection* linkedConnectionPtr = mConnectionPtr->GetLinkedConnection();
         if (linkedConnectionPtr != nullptr && linkedConnectionPtr->GetUDP_IO())
         {
            GenSockets::GenInternetSocketAddress addr = linkedConnectionPtr->GetUDP_IO()->GetSendSocket()->GetBoundAddr();
            aStream << " Linked UDP: " << (std::string)(GenSockets::GenIP)addr.GetAddress() << ':' << addr.GetPort();
         }
      }
      else
      {
         aStream << " Unreliable";
      }
      aStream << '\n';
   }
}

void HandleConnect::PrintCSV(std::ostream& aStream) const
{
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "XIO_CONNECT";
      if (mConnectionPtr->IsReliable())
      {
         aStream << ',' << "reliable" << ',';
         utilsCSV::Write(aStream, mConnectionPtr->GetApplicationId());
         aStream << "," << mConnectionPtr->GetApplicationName();
         aStream << "," << mConnectionPtr->GetApplicationType();
         WsfXIO_Connection* linkedConnectionPtr = mConnectionPtr->GetLinkedConnection();
         if (linkedConnectionPtr != nullptr && linkedConnectionPtr->GetUDP_IO())
         {
            GenSockets::GenInternetSocketAddress addr = linkedConnectionPtr->GetUDP_IO()->GetSendSocket()->GetBoundAddr();
            aStream << "," << (std::string)(GenSockets::GenIP)addr.GetAddress() << ',' << addr.GetPort();
         }
      }
      else
      {
         aStream << ',' << "unreliable"
                 << ",,,,,";
      }
      aStream << '\n';
   }
}

void HandleDisconnect::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "XIO_DISCONNECT ID: ";
   if (mConnectionPtr->IsReliable())
   {
      utils::Write(aStream, mConnectionPtr->GetApplicationId());
      aStream << " Name: " << mConnectionPtr->GetApplicationName();
      aStream << " Type: " << mConnectionPtr->GetApplicationType();
   }
   else
   {
      aStream << " Unreliable";
   }
   aStream << '\n';
}

void HandleDisconnect::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "XIO_DISCONNECT";
   if (mConnectionPtr->IsReliable())
   {
      aStream << ',' << "reliable" << ',';
      utilsCSV::Write(aStream, mConnectionPtr->GetApplicationId());
      aStream << "," << mConnectionPtr->GetApplicationName();
      aStream << "," << mConnectionPtr->GetApplicationType();
   }
   else
   {
      aStream << ',' << "unreliable"
              << ",,,";
   }
   aStream << '\n';
}

void HandleBandwidthData::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "XIO_BANDWIDTH Total Bytes: " << mTotalSentBytes << " / " << mTotalRecvBytes;
   aStream << " u/d; Delta Bytes: " << mDeltaSentBytes << " / " << mDeltaRecvBytes;
   aStream << " u/d; Rate: " << int(mDeltaSentBytes / mDeltaTime) << " / " << int(mDeltaRecvBytes / mDeltaTime);
   aStream << " u/d" << '\n';
}

void HandleBandwidthData::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   // clang-format off
   aStream << ',' << "XIO_BANDWIDTH"
           << ',' << mTotalSentBytes
           << "," << mTotalRecvBytes
           << "," << mDeltaSentBytes
           << "," << mDeltaRecvBytes
           << "," << int(mDeltaSentBytes / mDeltaTime)
           << "," << int(mDeltaRecvBytes / mDeltaTime)
           << '\n';
   // clang-format on
}

} // namespace event
} // namespace wsf
