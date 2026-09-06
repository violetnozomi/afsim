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

#ifndef WSFDISUDP_DEVICE_HPP
#define WSFDISUDP_DEVICE_HPP

#include "wsf_export.h"

#include <string>

#ifdef WSF_USE_UMP
class GenUmpIO;
#else
class GenUDP_IO;
#endif
#include "dis/WsfDisDevice.hpp"

/**
   A specialization of WsfDisDevice to support IP-based UDP I/O.

   The current implementation only supports UDP broadcast and multicast.
*/

class WSF_EXPORT WsfDisUDP_Device : public WsfDisDevice
{
public:
   WsfDisUDP_Device(WsfDisInterface*   aInterfacePtr,
                    const std::string& aSendAddress,
                    unsigned int       aPort,
                    unsigned int       aRcvPort,
                    bool               aRememberSenderHostname = false);

   WsfDisUDP_Device(WsfDisInterface*   aInterfacePtr,
                    const std::string& aMulticastAddress,
                    const std::string& aInterfaceAddress,
                    unsigned int       aPort,
                    unsigned int       aRcvPort,
                    int                aTimeToLive             = -1,
                    bool               aRememberSenderHostname = false);

   ~WsfDisUDP_Device() override;

   DisPdu* GetPdu(DisPduFactory* aPduFactoryPtr) override;

   void PutPduP(double aSimTime, DisPdu& aPdu) override;

#ifdef WSF_USE_UMP
   GenUmpIO* UmpIOPtr() { return mGenIOPtr; }
#else
   GenUDP_IO* GenUDP_IOPtr() override { return mGenIOPtr; }
#endif

private:
#ifdef WSF_USE_UMP
   GenUmpIO* mGenIOPtr;
#else
   GenUDP_IO* mGenIOPtr;
#endif
};

#endif
