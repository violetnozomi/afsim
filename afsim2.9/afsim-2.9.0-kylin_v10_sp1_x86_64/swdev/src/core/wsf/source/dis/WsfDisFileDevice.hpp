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

#ifndef WSFDISFILEDEVICE_HPP
#define WSFDISFILEDEVICE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

#include "GenFileO.hpp"
#include "dis/WsfDisDevice.hpp"

/**
   A specialization of WsfDisDevice to support recording to a CME replay file.
*/

class WSF_EXPORT WsfDisFileDevice : public WsfDisDevice
{
public:
   WsfDisFileDevice(const std::string& aRecordFile, WsfDisInterface* aDisInterfacePtr);
   ~WsfDisFileDevice() override = default;

   void PutPduP(double aSimTime, DisPdu& aPdu) override;

private:
   std::unique_ptr<GenFileO> mGenIOPtr;
};

#endif
