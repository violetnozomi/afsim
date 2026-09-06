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

// NOTE: By default, all UDP broadcast/multicast I/O is performed using the
//       GenUDP_IO class (which doesn't use the UMP library).  If
//       WSF_USE_UMP is defined then UDP I/O will be performed using UMP.  If
//       using UMP then the Developer Studio project/project_defines.mak files
//       must be modified to reference the UMP include and library directories.

#include "dis/WsfDisUDP_Device.hpp"

#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisPduFactory.hpp"
#include "DisTime.hpp"
#ifdef WSF_USE_UMP
#include "GenUmpIO.hpp"
#else
#include "GenIP.hpp"
#include "GenSocket.hpp"
#include "GenUDP_IO.hpp"
#endif

//! Create a device for performing network I/O using UDP broadcast.
WsfDisUDP_Device::WsfDisUDP_Device(WsfDisInterface*   aInterfacePtr,
                                   const std::string& aSendAddress,
                                   unsigned int       aSendPort,
                                   unsigned int       aRcvPort,
                                   bool               aRememberSenderHostname)
   : WsfDisDevice(aInterfacePtr)
   , mGenIOPtr(nullptr)
{
#ifdef WSF_USE_UMP
   mGenIOPtr = new GenUmpIO();
   if (mGenIOPtr->Init(0, aBroadcastAddress.c_str(), aPort, aPort, 0) < 0)
   {
      delete mGenIOPtr;
      mGenIOPtr = 0;
      throw OpenError();
   }
#else

   GenSockets::GenIP sendIP(aSendAddress);
   if (sendIP.IsValidForm() && sendIP.IsBroadcast())
   {
      mDeviceType = cDEVICE_BROADCAST;
   }
   else
   {
      mDeviceType = cDEVICE_UNICAST;
   }

   mGenIOPtr = new GenUDP_IO();
   mGenIOPtr->RememberSenderAddress(aRememberSenderHostname);
   if (!mGenIOPtr->Init(aSendAddress, aSendPort, aRcvPort))
   {
      delete mGenIOPtr;
      mGenIOPtr = nullptr;
      throw OpenError();
   }

#endif
}

//! Create a device for performing network I/O using UDP multicast.
WsfDisUDP_Device::WsfDisUDP_Device(WsfDisInterface*   aInterfacePtr,
                                   const std::string& aMulticastAddress,
                                   const std::string& aInterfaceAddress,
                                   unsigned int       aSendPort,
                                   unsigned int       aRcvPort,
                                   int                aTimeToLive,
                                   bool               aRememberSenderHostname)
   : WsfDisDevice(aInterfacePtr)
   , mGenIOPtr(nullptr)
{
#ifdef WSF_USE_UMP
   std::string address(aMulticastAddress);
   address += ' ';
   address += aInterfaceAddress;
   mGenIOPtr = new GenUmpIO();
   if (mGenIOPtr->Init(0, address.c_str(), aPort, aPort, 0) < 0)
   {
      delete mGenIOPtr;
      mGenIOPtr = 0;
      throw OpenError();
   }
#else
   mGenIOPtr = new GenUDP_IO();
   mGenIOPtr->RememberSenderAddress(aRememberSenderHostname);
   if (!mGenIOPtr->Init(aMulticastAddress, aInterfaceAddress, aSendPort, aRcvPort))
   {
      delete mGenIOPtr;
      mGenIOPtr = nullptr;
      throw OpenError();
   }
   if (!aInterfaceAddress.empty())
   {
      mGenIOPtr->AddMulticastMembership(aInterfaceAddress, aMulticastAddress);
   }
   if (aTimeToLive != -1)
   {
      mGenIOPtr->GetSocket()->SetMulticastTTL(aTimeToLive);
   }
   mDeviceType = cDEVICE_MULTICAST;
#endif
}

// virtual
WsfDisUDP_Device::~WsfDisUDP_Device()
{
   delete mGenIOPtr;
}

// virtual
DisPdu* WsfDisUDP_Device::GetPdu(DisPduFactory* aPduFactoryPtr)
{
   DisPdu* pduPtr = nullptr;

   // Determine if there is any data left in the buffer
   long int bytesLeft = mGenIOPtr->GetGetOffset(GenBuf::FromDataEnd);
   if (bytesLeft == 0)
   {
      // No data is left in the buffer so attempt to refill it.
      if (mGenIOPtr->Receive(0) > 0)
      {
         bytesLeft = mGenIOPtr->GetGetOffset(GenBuf::FromDataEnd);
      }
   }

   // If there is data and there is enough to possible be a PDU,
   // create the PDU and return it to the user.
   //
   // If there is data but not enough to possible be a PDU, discard
   // the remainder of the buffer.

   if (bytesLeft > 12)
   {
      pduPtr = DisPdu::Create(*mGenIOPtr, aPduFactoryPtr);
   }
   else if (bytesLeft > 0)
   {
      // This can't be a valid PDU.  Discard any remnants
      mGenIOPtr->SetGetOffset(0, GenBuf::FromDataEnd);
   }

   return pduPtr;
}

// virtual
void WsfDisUDP_Device::PutPduP(double aSimTime, DisPdu& aPdu)
{
   if (DisTime::UseAbsoluteTimestamp() || aPdu.GetProtocolFamily() == DisEnum::Pdu::Family::SimulationManagement ||
       aPdu.GetProtocolFamily() == DisEnum::Pdu::Family::SimulationManagementWithReliability)
   {
      aPdu.SetTimestamp(DisTime::GetAbsoluteTimestamp());
   }
   else
   {
      aPdu.SetTime(aSimTime);
   }
   aPdu.Put(*mGenIOPtr);
   mGenIOPtr->Send();
}
