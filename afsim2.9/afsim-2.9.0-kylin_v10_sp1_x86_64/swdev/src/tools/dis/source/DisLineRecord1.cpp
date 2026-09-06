// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisLineRecord1.hpp"

DisLineRecord1::DisLineRecord1(const DisLineRecord1& aSrc)
   : mStartPointLocation(aSrc.mStartPointLocation)
   , mEndPointLocation(aSrc.mEndPointLocation)
{
}

DisLineRecord1::DisLineRecord1(GenI& aGenI)
   : mStartPointLocation()
   , mEndPointLocation()
{
   GetMemberData(aGenI);
}

void DisLineRecord1::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisLineRecord1::GetLength()
{
   return cLENGTH_IN_OCTETS;
}

void DisLineRecord1::Put(GenO& aGenO) const
{
   mStartPointLocation.Put(aGenO);
   mEndPointLocation.Put(aGenO);
}

bool DisLineRecord1::IsValid() const
{
   return mStartPointLocation.IsValid() && mEndPointLocation.IsValid();
}

DisLineRecord1* DisLineRecord1::Clone() const
{
   return new DisLineRecord1(*this);
}

DisPointRecord& DisLineRecord1::GetStartPointLocation()
{
   return mStartPointLocation;
}

DisPointRecord& DisLineRecord1::GetEndPointLocation()
{
   return mEndPointLocation;
}

void DisLineRecord1::SetStartPointLocation(const DisPointRecord& aStartPointLocation)
{
   mStartPointLocation = aStartPointLocation;
}

void DisLineRecord1::SetEndPointLocation(const DisPointRecord& aEndPointLocation)
{
   mEndPointLocation = aEndPointLocation;
}

std::string DisLineRecord1::ToString() const
{
   DisFloat64  startLocation[3];
   DisFloat64  endLocation[3];
   std::string returnString = "-------Line Record-------\n";
   returnString += "---------Start Point Location-----------\n";
   mStartPointLocation.GetLocation(startLocation[0], startLocation[1], startLocation[2]);
   returnString += "Starting X: " + std::to_string(startLocation[0]) + '\n';
   returnString += "Starting Y: " + std::to_string(startLocation[1]) + '\n';
   returnString += "Starting Z: " + std::to_string(startLocation[2]) + '\n';
   returnString += "-------End Start Point Location---------\n";
   returnString += "---------End Point Location-----------\n";
   mEndPointLocation.GetLocation(endLocation[0], endLocation[1], endLocation[2]);
   returnString += "Ending X: " + std::to_string(endLocation[0]) + '\n';
   returnString += "Ending Y: " + std::to_string(endLocation[1]) + '\n';
   returnString += "Ending Z: " + std::to_string(endLocation[2]) + '\n';
   returnString += "------End End Point Location---------\n";
   returnString += "-----End Line Record-----\n";
   return returnString;
}

void DisLineRecord1::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisLineRecord1::GetMemberData(GenI& aGenI)
{
   mStartPointLocation.Get(aGenI);
   mEndPointLocation.Get(aGenI);
}
