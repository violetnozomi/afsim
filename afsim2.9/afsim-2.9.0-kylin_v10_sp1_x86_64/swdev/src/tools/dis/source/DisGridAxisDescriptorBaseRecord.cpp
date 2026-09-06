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

#include "DisGridAxisDescriptorBaseRecord.hpp"

#include "DisValidationUtils.hpp"

DisGridAxisDescriptorBaseRecord::DisGridAxisDescriptorBaseRecord(const DisGridAxisDescriptorBaseRecord& aSrc)
   : mDomainInitialX(aSrc.mDomainInitialX)
   , mDomainFinalX(aSrc.mDomainFinalX)
   , mDomainPointsX(aSrc.mDomainPointsX)
   , mInterleafFactor(aSrc.mInterleafFactor)
   , mAxisType(aSrc.mAxisType)
{
}

DisGridAxisDescriptorBaseRecord::DisGridAxisDescriptorBaseRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisGridAxisDescriptorBaseRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisGridAxisDescriptorBaseRecord::GetLength()
{
   return cRECORD_BASE_LENGTH_IN_OCTETS;
}

void DisGridAxisDescriptorBaseRecord::Put(GenO& aGenO) const
{
   aGenO << mDomainInitialX;
   aGenO << mDomainFinalX;
   aGenO << mDomainPointsX;
   aGenO << mInterleafFactor;
   aGenO << mAxisType;
}

DisFloat64 DisGridAxisDescriptorBaseRecord::GetDomainInitialX() const
{
   return mDomainInitialX;
}

DisFloat64 DisGridAxisDescriptorBaseRecord::GetDomainFinalX() const
{
   return mDomainFinalX;
}

DisUint16 DisGridAxisDescriptorBaseRecord::GetDomainPoints() const
{
   return mDomainPointsX;
}

DisUint8 DisGridAxisDescriptorBaseRecord::GetInterleafFactor() const
{
   return mInterleafFactor;
}

DisEnum8 DisGridAxisDescriptorBaseRecord::GetAxisType() const
{
   return mAxisType;
}

void DisGridAxisDescriptorBaseRecord::SetDomainInitialX(DisFloat64 aDomainInitialX)
{
   mDomainInitialX = aDomainInitialX;
}

void DisGridAxisDescriptorBaseRecord::SetDomainFinalX(DisFloat64 aDomainFinalX)
{
   mDomainFinalX = aDomainFinalX;
}

void DisGridAxisDescriptorBaseRecord::SetDomainPoints(DisUint16 aDomainPointsX)
{
   mDomainPointsX = aDomainPointsX;
}

void DisGridAxisDescriptorBaseRecord::SetInterleafFactor(DisUint8 aInterleafFactor)
{
   mInterleafFactor = aInterleafFactor;
}

void DisGridAxisDescriptorBaseRecord::SetAxisType(DisEnum8 aAxisType)
{
   mAxisType = aAxisType;
}

bool DisGridAxisDescriptorBaseRecord::IsValid() const
{
   return ValidateScalar(mDomainInitialX) && ValidateScalar(mDomainFinalX);
}

DisGridAxisDescriptorBaseRecord* DisGridAxisDescriptorBaseRecord::Clone() const
{
   return new DisGridAxisDescriptorBaseRecord(*this);
}

void DisGridAxisDescriptorBaseRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}

std::string DisGridAxisDescriptorBaseRecord::ToString() const
{
   std::string returnString = "-------Grid Access Descriptor Record-------\n";
   returnString += "Initial Domain X: " + std::to_string(mDomainInitialX) + '\n';
   returnString += "Final Domain X:   " + std::to_string(mDomainFinalX) + '\n';
   returnString += "Domain Points X:  " + std::to_string(mDomainPointsX) + '\n';
   returnString += "Interleaf Factor: " + std::to_string(mInterleafFactor) + '\n';
   returnString += "Final Domain X:   " + std::to_string(mDomainFinalX) + '\n';
   returnString += "Axis Type:        " + std::to_string(mAxisType) + '\n';
   returnString += "-----End Grid Access Descriptor Record-----\n";
   return returnString;
}

void DisGridAxisDescriptorBaseRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mDomainInitialX;
   aGenI >> mDomainFinalX;
   aGenI >> mDomainPointsX;
   aGenI >> mInterleafFactor;
   aGenI >> mAxisType;
}

// Operators
bool DisGridAxisDescriptorBaseRecord::operator==(const DisGridAxisDescriptorBaseRecord& aRhs) const
{
   return ((mDomainInitialX == aRhs.mDomainInitialX) && (mDomainFinalX == aRhs.mDomainFinalX) &&
           (mDomainPointsX == aRhs.mDomainPointsX) && (mInterleafFactor == aRhs.mInterleafFactor) &&
           (mAxisType == aRhs.mAxisType));
}

bool DisGridAxisDescriptorBaseRecord::operator!=(const DisGridAxisDescriptorBaseRecord& aRhs) const
{
   return ((mDomainInitialX != aRhs.mDomainInitialX) || (mDomainFinalX != aRhs.mDomainFinalX) ||
           (mDomainPointsX != aRhs.mDomainPointsX) || (mInterleafFactor != aRhs.mInterleafFactor) ||
           (mAxisType != aRhs.mAxisType));
}

bool DisGridAxisDescriptorBaseRecord::operator<(const DisGridAxisDescriptorBaseRecord& aRhs) const
{
   bool isLess = true;

   if (mDomainInitialX > aRhs.mDomainInitialX)
   {
      isLess = false;
   }
   else if (mDomainInitialX == aRhs.mDomainInitialX)
   {
      if (mDomainFinalX > aRhs.mDomainFinalX)
      {
         isLess = false;
      }
      else if (mDomainFinalX == aRhs.mDomainFinalX)
      {
         if (mDomainPointsX > aRhs.mDomainPointsX)
         {
            isLess = false;
         }
         else if (mDomainPointsX == aRhs.mDomainPointsX)
         {
            if (mInterleafFactor > aRhs.mInterleafFactor)
            {
               isLess = false;
            }
            else if (mInterleafFactor == aRhs.mInterleafFactor)
            {
               if (mAxisType >= aRhs.mAxisType)
               {
                  isLess = false;
               }
            }
         }
      }
   }
   return isLess;
}
