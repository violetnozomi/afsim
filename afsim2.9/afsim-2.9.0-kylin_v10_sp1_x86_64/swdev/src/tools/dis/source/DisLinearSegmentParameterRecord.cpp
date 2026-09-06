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

#include "DisLinearSegmentParameterRecord.hpp"

#include "DisEntityAppearance.hpp"
#include "DisEntityType.hpp"
#include "DisValidationUtils.hpp"

DisLinearSegmentParameterRecord::DisLinearSegmentParameterRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mSegmentLocation[i]    = 0;
      mSegmentOrientation[i] = 0;
   }
}

DisLinearSegmentParameterRecord::DisLinearSegmentParameterRecord(const DisLinearSegmentParameterRecord& aSrc)
   : mSegmentNumber(aSrc.mSegmentNumber)
   , mSegmentModification(aSrc.mSegmentModification)
   , mGeneralSegmentAppearance(aSrc.mGeneralSegmentAppearance)
   , mSpecificSegmentAppearance(aSrc.mSpecificSegmentAppearance)
   , mSegmentLength(aSrc.mSegmentLength)
   , mSegmentWidth(aSrc.mSegmentWidth)
   , mSegmentHeight(aSrc.mSegmentHeight)
   , mSegmentDepth(aSrc.mSegmentDepth)
{
   for (int i = 0; i < 3; i++)
   {
      mSegmentLocation[i]    = aSrc.mSegmentLocation[i];
      mSegmentOrientation[i] = aSrc.mSegmentOrientation[i];
   }
}

DisLinearSegmentParameterRecord::DisLinearSegmentParameterRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisLinearSegmentParameterRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisLinearSegmentParameterRecord::GetLength()
{
   return cBASE_LENGTH_IN_OCTETS;
}

void DisLinearSegmentParameterRecord::Put(GenO& aGenO) const
{
   aGenO << mSegmentNumber;
   aGenO << mSegmentModification;
   mGeneralSegmentAppearance.Put(aGenO);
   mSpecificSegmentAppearance.Put(aGenO);
   aGenO << mSegmentLocation[0] << mSegmentLocation[1] << mSegmentLocation[2];
   aGenO << mSegmentOrientation[0] << mSegmentOrientation[1] << mSegmentOrientation[2];
   aGenO << mSegmentLength;
   aGenO << mSegmentWidth;
   aGenO << mSegmentHeight;
   aGenO << mSegmentDepth;
   aGenO << mPadding;
}

bool DisLinearSegmentParameterRecord::IsValid() const
{
   return ValidateScalarArray(mSegmentLocation, 3) && ValidateScalarArray(mSegmentOrientation, 3) &&
          ValidateScalar(mSegmentLength) && ValidateScalar(mSegmentWidth) && ValidateScalar(mSegmentHeight) &&
          ValidateScalar(mSegmentDepth);
}

DisLinearSegmentParameterRecord* DisLinearSegmentParameterRecord::Clone() const
{
   return new DisLinearSegmentParameterRecord(*this);
}

DisUint8 DisLinearSegmentParameterRecord::GetSegmentNumber() const
{
   return mSegmentNumber;
}

DisEnum8 DisLinearSegmentParameterRecord::GetSegmentModification() const
{
   return mSegmentModification;
}

const DisGeneralAppearanceRecord DisLinearSegmentParameterRecord::GetGeneralSegmentAppearance() const
{
   return mGeneralSegmentAppearance;
}

const DisEntityAppearance DisLinearSegmentParameterRecord::GetSpecificSegmentAppearance() const
{
   return mSpecificSegmentAppearance;
}

void DisLinearSegmentParameterRecord::GetSegmentLocation(DisFloat64& aSegmentLocationX,
                                                         DisFloat64& aSegmentLocationY,
                                                         DisFloat64& aSegmentLocationZ) const
{
   aSegmentLocationX = mSegmentLocation[0];
   aSegmentLocationY = mSegmentLocation[1];
   aSegmentLocationZ = mSegmentLocation[2];
}

void DisLinearSegmentParameterRecord::GetSegmentOrientation(DisFloat32& aSegmentOrientationX,
                                                            DisFloat32& aSegmentOrientationY,
                                                            DisFloat32& aSegmentOrientationZ) const
{
   aSegmentOrientationX = mSegmentOrientation[0];
   aSegmentOrientationY = mSegmentOrientation[1];
   aSegmentOrientationZ = mSegmentOrientation[2];
}

DisFloat32 DisLinearSegmentParameterRecord::GetSegmentLength() const
{
   return mSegmentLength;
}

DisFloat32 DisLinearSegmentParameterRecord::GetSegmentWidth() const
{
   return mSegmentWidth;
}

DisFloat32 DisLinearSegmentParameterRecord::GetSegmentHeight() const
{
   return mSegmentHeight;
}

DisFloat32 DisLinearSegmentParameterRecord::GetSegmentDepth() const
{
   return mSegmentDepth;
}

void DisLinearSegmentParameterRecord::SetSegmentNumber(DisUint8 aSegmentNumber)
{
   mSegmentNumber = aSegmentNumber;
}

void DisLinearSegmentParameterRecord::SetSegmentModification(DisEnum8 aSegmentModifications)
{
   mSegmentModification = aSegmentModifications;
}

void DisLinearSegmentParameterRecord::SetGeneralSegmentAppearance(const DisGeneralAppearanceRecord& aGeneralSegmentAppearance)
{
   mGeneralSegmentAppearance = aGeneralSegmentAppearance;
}

void DisLinearSegmentParameterRecord::SetSpecificSegmentAppearance(const DisEntityAppearance& aSpecificSegmentAppearance)
{
   mSpecificSegmentAppearance = aSpecificSegmentAppearance;
}

void DisLinearSegmentParameterRecord::SetSegmentLocation(DisFloat64 aSegmentLocationX,
                                                         DisFloat64 aSegmentLocationY,
                                                         DisFloat64 aSegmentLocationZ)
{
   mSegmentLocation[0] = aSegmentLocationX;
   mSegmentLocation[1] = aSegmentLocationY;
   mSegmentLocation[2] = aSegmentLocationZ;
}

void DisLinearSegmentParameterRecord::SetSegmentOrientation(DisFloat32 aSegmentOrientationX,
                                                            DisFloat32 aSegmentOrientationY,
                                                            DisFloat32 aSegmentOrientationZ)
{
   mSegmentOrientation[0] = aSegmentOrientationX;
   mSegmentOrientation[1] = aSegmentOrientationY;
   mSegmentOrientation[2] = aSegmentOrientationZ;
}

void DisLinearSegmentParameterRecord::SetSegmentLength(DisFloat32 aSegmentLength)
{
   mSegmentLength = aSegmentLength;
}

void DisLinearSegmentParameterRecord::SetSegmentWidth(DisFloat32 aSegmentWidth)
{
   mSegmentWidth = aSegmentWidth;
}

void DisLinearSegmentParameterRecord::SetSegmentHeight(DisFloat32 aSegmentHeight)
{
   mSegmentHeight = aSegmentHeight;
}

void DisLinearSegmentParameterRecord::SetSegmentDepth(DisFloat32 aSegmentDepth)
{
   mSegmentDepth = aSegmentDepth;
}

void DisLinearSegmentParameterRecord::Stream(std::ostream& aStream, DisObjectType aObjectType) const
{
   aStream << ToString(aObjectType) << std::endl;
}

std::string DisLinearSegmentParameterRecord::ToString(const DisObjectType& aObjectType) const
{
   std::string returnString = "-------Linear Segment Parameter-------\n";
   returnString += "Segment Number: " + std::to_string(mSegmentNumber) + '\n';
   returnString += "Segment Modifications: " + std::to_string(mSegmentModification) + '\n';
   returnString += "General Segment Appearance: " + mGeneralSegmentAppearance.ToString() + '\n';
   returnString += "Specific Segment Appearance: " +
                   mSpecificSegmentAppearance.ToString(aObjectType.GetObjectKind(), aObjectType.GetDomain()) + '\n';
   returnString += "-------Segment Location-------\n";
   returnString += "   X: " + std::to_string(mSegmentLocation[0]) + '\n';
   returnString += "   Y: " + std::to_string(mSegmentLocation[1]) + '\n';
   returnString += "   Z: " + std::to_string(mSegmentLocation[2]) + '\n';
   returnString += "-----End Segment Location-----\n";
   returnString += "-------Segment Orientation-------\n";
   returnString += "   X: " + std::to_string(mSegmentOrientation[0]) + '\n';
   returnString += "   Y: " + std::to_string(mSegmentOrientation[1]) + '\n';
   returnString += "   Z: " + std::to_string(mSegmentOrientation[2]) + '\n';
   returnString += "-----End Segment Orientation-----\n";
   returnString += "Segment Length: " + std::to_string(mSegmentLength) + '\n';
   returnString += "Segment Width: " + std::to_string(mSegmentWidth) + '\n';
   returnString += "Segment Height: " + std::to_string(mSegmentHeight) + '\n';
   returnString += "Segment Depth: " + std::to_string(mSegmentDepth) + '\n';
   returnString += "-----End Linear Segment Parameter-----\n";
   return returnString;
}

void DisLinearSegmentParameterRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mSegmentNumber;
   aGenI >> mSegmentModification;
   mGeneralSegmentAppearance.Get(aGenI);
   mSpecificSegmentAppearance.Get(aGenI);
   aGenI >> mSegmentLocation[0] >> mSegmentLocation[1] >> mSegmentLocation[2];
   aGenI >> mSegmentOrientation[0] >> mSegmentOrientation[1] >> mSegmentOrientation[2];
   aGenI >> mSegmentLength;
   aGenI >> mSegmentWidth;
   aGenI >> mSegmentHeight;
   aGenI >> mSegmentDepth;
   aGenI >> mPadding;
}
