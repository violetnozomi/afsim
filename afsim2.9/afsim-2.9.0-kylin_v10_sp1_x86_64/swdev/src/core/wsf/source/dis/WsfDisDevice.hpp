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

#ifndef WSFDISDEVICE_HPP
#define WSFDISDEVICE_HPP

#include "wsf_export.h"

#include <vector>

class DisPdu;
class DisPduFactory;
class GenUDP_IO;
class WsfDisFilterAllowance;
class WsfDisInterface;

/**
   A base class for a DIS I/O device (file, network, etc).
*/

class WSF_EXPORT WsfDisDevice
{
public:
   enum DeviceType
   {
      cDEVICE_NONE,
      cDEVICE_RECORD,
      cDEVICE_PLAYBACK,
      cDEVICE_MULTICAST,
      cDEVICE_BROADCAST,
      cDEVICE_UNICAST,
      cDEVICE_CEE
   };

   //! Exception class

   class OpenError
   {
   };

   WsfDisDevice(WsfDisInterface* aInterfacePtr);
   virtual ~WsfDisDevice();

   virtual DisPdu* GetPdu(DisPduFactory* aPduFactoryPtr);

   void PutPdu(double aSimTime, DisPdu& aPdu);

   virtual void PutPduP(double, DisPdu&) = 0;

   DeviceType         GetDeviceType() const { return mDeviceType; }
   virtual GenUDP_IO* GenUDP_IOPtr() { return nullptr; }

   void AddFilterAllowance(WsfDisFilterAllowance* anAllowance);

protected:
   bool AllowedThroughFilter(DisPdu& aPdu);

   DeviceType                          mDeviceType;
   std::vector<WsfDisFilterAllowance*> mFilterAllowances;
   WsfDisInterface*                    mInterfacePtr;
};

#endif
