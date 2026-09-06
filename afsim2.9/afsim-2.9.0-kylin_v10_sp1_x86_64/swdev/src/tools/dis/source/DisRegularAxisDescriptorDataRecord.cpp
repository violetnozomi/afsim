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

#include "DisRegularAxisDescriptorDataRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisRegularAxisDescriptorDataRecord::DisRegularAxisDescriptorDataRecord()
   : DisGridAxisDescriptorBaseRecord()
{
}

DisRegularAxisDescriptorDataRecord::DisRegularAxisDescriptorDataRecord(const DisRegularAxisDescriptorDataRecord& aSrc)
   : DisGridAxisDescriptorBaseRecord(aSrc)
   , mNumberOfPointsOnAxis(aSrc.mNumberOfPointsOnAxis)
   , mInitialIndexX(aSrc.mInitialIndexX)
{
}

DisRegularAxisDescriptorDataRecord::DisRegularAxisDescriptorDataRecord(GenI& aGenI)
   : DisGridAxisDescriptorBaseRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisRegularAxisDescriptorDataRecord::Get(GenI& aGenI)
{
   DisGridAxisDescriptorBaseRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisRegularAxisDescriptorDataRecord::GetLength()
{
   return DisGridAxisDescriptorBaseRecord::GetLength() + cMIN_LENGTH_IN_OCTETS;
}

void DisRegularAxisDescriptorDataRecord::Put(GenO& aGenO) const
{
   DisGridAxisDescriptorBaseRecord::Put(aGenO);
   aGenO << mNumberOfPointsOnAxis;
   aGenO << mInitialIndexX;
}

DisUint16 DisRegularAxisDescriptorDataRecord::GetNumberOfPointsOnAxis() const
{
   return mNumberOfPointsOnAxis;
}

DisUint16 DisRegularAxisDescriptorDataRecord::GetInitialIndexX() const
{
   return mInitialIndexX;
}

void DisRegularAxisDescriptorDataRecord::SetNumberOfPointsOnAxis(DisUint16 aNumberOfPointsOnAxis)
{
   mNumberOfPointsOnAxis = aNumberOfPointsOnAxis;
}

void DisRegularAxisDescriptorDataRecord::SetInitialIndexX(DisUint16 aInitialIndexX)
{
   mInitialIndexX = aInitialIndexX;
}

bool DisRegularAxisDescriptorDataRecord::IsValid() const
{
   return DisGridAxisDescriptorBaseRecord::IsValid();
}

DisRegularAxisDescriptorDataRecord* DisRegularAxisDescriptorDataRecord::Clone() const
{
   return new DisRegularAxisDescriptorDataRecord(*this);
}

std::string DisRegularAxisDescriptorDataRecord::ToString() const
{
   std::string returnString = DisGridAxisDescriptorBaseRecord::ToString();
   returnString += "------Regular Axis Descriptor Data Object-------\n";
   returnString += "Number of points on axis: " + std::to_string(mNumberOfPointsOnAxis) + "\n";
   returnString += "Initial Index X:          " + std::to_string(mInitialIndexX) + "\n";
   returnString += "----End Regular Axis Descriptor Data Object-----\n";
   return returnString;
}

void DisRegularAxisDescriptorDataRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mNumberOfPointsOnAxis;
   aGenI >> mInitialIndexX;
}
