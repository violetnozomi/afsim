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

#include "DisGaussianPlumeRecord.hpp"

#include "DisValidationUtils.hpp"

DisGaussianPlumeRecord::DisGaussianPlumeRecord()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i]         = 0;
      mLeadingEdgeVelocity[i] = 0;
   }
}

DisGaussianPlumeRecord::DisGaussianPlumeRecord(const DisGaussianPlumeRecord& aSrc)
   : DisPointRecord(aSrc)
   , mPlumeLength(aSrc.mPlumeLength)
   , mPlumeWidth(aSrc.mPlumeWidth)
   , mPlumeHeight(aSrc.mPlumeHeight)
   , mDeltaPlumeLength(aSrc.mDeltaPlumeLength)
   , mDeltaPlumeWidth(aSrc.mDeltaPlumeWidth)
   , mDeltaPlumeHeight(aSrc.mDeltaPlumeHeight)
   , mLeadingEdgeCentroidHeight(aSrc.mLeadingEdgeCentroidHeight)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i]         = aSrc.mOrientation[i];
      mLeadingEdgeVelocity[i] = aSrc.mLeadingEdgeVelocity[i];
   }
}

DisGaussianPlumeRecord::DisGaussianPlumeRecord(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisGaussianPlumeRecord::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisGaussianPlumeRecord::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisGaussianPlumeRecord::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mPlumeLength;
   aGenO << mPlumeWidth;
   aGenO << mPlumeHeight;
   aGenO << mDeltaPlumeLength;
   aGenO << mDeltaPlumeWidth;
   aGenO << mDeltaPlumeHeight;
   aGenO << mLeadingEdgeCentroidHeight;
   aGenO << mLeadingEdgeVelocity[0] << mLeadingEdgeVelocity[1] << mLeadingEdgeVelocity[2];
   aGenO << mPadding;
}

bool DisGaussianPlumeRecord::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalarArray(mOrientation, 3) && ValidateScalar(mPlumeLength) &&
          ValidateScalar(mPlumeWidth) && ValidateScalar(mPlumeHeight) && ValidateScalar(mDeltaPlumeLength) &&
          ValidateScalar(mDeltaPlumeWidth) && ValidateScalar(mDeltaPlumeHeight) &&
          ValidateScalar(mLeadingEdgeCentroidHeight) && ValidateScalarArray(mLeadingEdgeVelocity, 3);
}

DisGaussianPlumeRecord* DisGaussianPlumeRecord::Clone() const
{
   return new DisGaussianPlumeRecord(*this);
}

void DisGaussianPlumeRecord::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

DisFloat32 DisGaussianPlumeRecord::GetPlumeLength() const
{
   return mPlumeLength;
}

DisFloat32 DisGaussianPlumeRecord::GetPlumeWidth() const
{
   return mPlumeWidth;
}

DisFloat32 DisGaussianPlumeRecord::GetPlumeHeight() const
{
   return mPlumeHeight;
}

DisFloat32 DisGaussianPlumeRecord::GetDeltaPlumeLength() const
{
   return mDeltaPlumeLength;
}

DisFloat32 DisGaussianPlumeRecord::GetDeltaPlumeWidth() const
{
   return mDeltaPlumeWidth;
}

DisFloat32 DisGaussianPlumeRecord::GetDeltaPlumeHeight() const
{
   return mDeltaPlumeHeight;
}

DisFloat32 DisGaussianPlumeRecord::GetLeadingEdgeCentroidHeight() const
{
   return mLeadingEdgeCentroidHeight;
}

void DisGaussianPlumeRecord::GetLeadingEdgeVelocity(DisFloat32& aXComponent,
                                                    DisFloat32& aYComponent,
                                                    DisFloat32& aZComponent) const
{
   aXComponent = mLeadingEdgeVelocity[0];
   aYComponent = mLeadingEdgeVelocity[1];
   aZComponent = mLeadingEdgeVelocity[2];
}

void DisGaussianPlumeRecord::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisGaussianPlumeRecord::SetPlumeLength(DisFloat32 aPlumeLength)
{
   mPlumeLength = aPlumeLength;
}

void DisGaussianPlumeRecord::SetPlumeWidth(DisFloat32 aPlumeWidth)
{
   mPlumeWidth = aPlumeWidth;
}

void DisGaussianPlumeRecord::SetPlumeHeight(DisFloat32 aPlumeHeight)
{
   mPlumeHeight = aPlumeHeight;
}

void DisGaussianPlumeRecord::SetDeltaPlumeLength(DisFloat32 aDeltaPlumeLength)
{
   mDeltaPlumeLength = aDeltaPlumeLength;
}

void DisGaussianPlumeRecord::SetDeltaPlumeWidth(DisFloat32 aDeltaPlumeWidth)
{
   mDeltaPlumeWidth = aDeltaPlumeWidth;
}

void DisGaussianPlumeRecord::SetDeltaPlumeHeight(DisFloat32 aDeltaPlumeHeight)
{
   mDeltaPlumeHeight = aDeltaPlumeHeight;
}

void DisGaussianPlumeRecord::SetLeadingEdgeCentroidHeight(DisFloat32 aLeadingEdgeControlType)
{
   mLeadingEdgeCentroidHeight = aLeadingEdgeControlType;
}

void DisGaussianPlumeRecord::SetLeadingEdgeVelocity(DisFloat32 aXComponent, DisFloat32 aYComponent, DisFloat32 aZComponent)
{
   mLeadingEdgeVelocity[0] = aXComponent;
   mLeadingEdgeVelocity[1] = aYComponent;
   mLeadingEdgeVelocity[2] = aZComponent;
}

std::string DisGaussianPlumeRecord::ToString() const
{
   std::string returnString = "-------Gaussian Plume Record-------\n";
   returnString += "-------Orientation-------\n";
   returnString += "Orientation X: " + std::to_string(mOrientation[0]) + '\n';
   returnString += "Orientation Y: " + std::to_string(mOrientation[1]) + '\n';
   returnString += "Orientation Z: " + std::to_string(mOrientation[2]) + '\n';
   returnString += "-----End Orientation-----\n";
   returnString += "Plume Length: " + std::to_string(mPlumeLength) + '\n';
   returnString += "Plume Width: " + std::to_string(mPlumeWidth) + '\n';
   returnString += "Plume Height: " + std::to_string(mPlumeHeight) + '\n';
   returnString += "Delta Plume Length: " + std::to_string(mDeltaPlumeLength) + '\n';
   returnString += "Delta Plume Width: " + std::to_string(mDeltaPlumeWidth) + '\n';
   returnString += "Delta Plume Height: " + std::to_string(mDeltaPlumeHeight) + '\n';
   returnString += "Leading Edge Control Height: " + std::to_string(mLeadingEdgeCentroidHeight) + '\n';
   returnString += "-------Leading Edge Velocity-------\n";
   returnString += "Leading Edge Velocity X: " + std::to_string(mLeadingEdgeVelocity[0]) + '\n';
   returnString += "Leading Edge Velocity Y: " + std::to_string(mLeadingEdgeVelocity[1]) + '\n';
   returnString += "Leading Edge Velocity Z: " + std::to_string(mLeadingEdgeVelocity[2]) + '\n';
   returnString += "-----End Leading Edge Velocity-----\n";
   returnString += "-----End Gaussian Plume Record-----\n";
   return returnString;
}

void DisGaussianPlumeRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisGaussianPlumeRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mPlumeLength;
   aGenI >> mPlumeWidth;
   aGenI >> mPlumeHeight;
   aGenI >> mDeltaPlumeLength;
   aGenI >> mDeltaPlumeWidth;
   aGenI >> mDeltaPlumeHeight;
   aGenI >> mLeadingEdgeCentroidHeight;
   aGenI >> mLeadingEdgeVelocity[0] >> mLeadingEdgeVelocity[1] >> mLeadingEdgeVelocity[2];
   aGenI >> mPadding;
}
