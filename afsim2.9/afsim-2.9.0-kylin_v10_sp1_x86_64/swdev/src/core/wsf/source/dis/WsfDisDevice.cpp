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

#include "dis/WsfDisDevice.hpp"

#include "DisPdu.hpp"
#include "dis/WsfDisFilterAllowance.hpp"

WsfDisDevice::WsfDisDevice(WsfDisInterface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisDevice::~WsfDisDevice() {}

// virtual
DisPdu* WsfDisDevice::GetPdu(DisPduFactory* /* aPduPtr */)
{
   return nullptr;
}

// virtual
// this method serves as implementation for putting pdus
// after they have been filtered
// void PutPduP(double  aSimTime,
//              DisPdu& aPdu)
//{
// }

// this is the entry point of any pdu going out of the device
// it might be filtered out here, or passed along
// to the output implementation PutPduP()
void WsfDisDevice::PutPdu(double aSimTime, DisPdu& aPdu)
{
   if (AllowedThroughFilter(aPdu))
   {
      PutPduP(aSimTime, aPdu);
   }
}

void WsfDisDevice::AddFilterAllowance(WsfDisFilterAllowance* anAllowance)
{
   mFilterAllowances.push_back(anAllowance);
}

bool WsfDisDevice::AllowedThroughFilter(DisPdu& aPdu)
{
   // either there are no filter allowances and all PDUs are allowed
   // or the pdu has to match one of the allowances
   if (mFilterAllowances.empty())
   {
      return true;
   }

   for (size_t i = 0; i < mFilterAllowances.size(); ++i)
   {
      if (mFilterAllowances[i]->PduAllowed(aPdu, mInterfacePtr))
      {
         return true;
      }
   }
   return false;
}
