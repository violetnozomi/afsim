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

#include "DisEntityPart.hpp"

#include "DisVariableParameterRecord.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEntityPart::DisEntityPart()
   : DisVariableParameterRecord()
   , mNumericValue()
   , mEntityType()
{
   SetTypeDesignator(TypeArticulated);
   mNumericValue.mFloat32[0] = 0.0F;
   mNumericValue.mFloat32[1] = 0.0F;
}

DisEntityPart::DisEntityPart(const DisEntityPart& aSrc)
   : DisVariableParameterRecord(aSrc)
   , mChangeIndicator(aSrc.mChangeIndicator)
   , mPartAttachedTo(aSrc.mPartAttachedTo)
   , mParameterType(aSrc.mParameterType)
   , mNumericValue(aSrc.mNumericValue)
   , mEntityType(aSrc.mEntityType)
{
   SetTypeDesignator(aSrc.GetTypeDesignator());
}

DisEntityPart::DisEntityPart(const DisVariableParameterRecord& aVariableParameterRecord, GenI& aGenI)
   : DisVariableParameterRecord(aVariableParameterRecord)
{
   GetMemberData(aGenI);
}

DisEntityPart* DisEntityPart::Clone() const
{
   return new DisEntityPart(*this);
}

void DisEntityPart::Get(GenI& aGenI)
{
   DisVariableParameterRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisEntityPart::Put(GenO& aGenO) const
{
   DisVariableParameterRecord::Put(aGenO);
   aGenO << mChangeIndicator;
   aGenO << mPartAttachedTo;
   aGenO << mParameterType;

   if (GetTypeDesignator() == TypeArticulated)
   {
      aGenO << mNumericValue.mFloat32[0] << mNumericValue.mFloat32[1];
   }
   else if (GetTypeDesignator() == TypeAttached)
   {
      mEntityType.Put(aGenO);
   }
   else
   {
      aGenO << mNumericValue.mUnknown[0] << mNumericValue.mUnknown[1];
   }
}

void DisEntityPart::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisEntityPart::ToString() const
{
   std::string returnString = "Type designator: " + std::to_string(GetTypeDesignator()) + '\n';
   returnString += "Change indicator: " + std::to_string(mChangeIndicator) + '\n';
   returnString += "Part attached to: " + std::to_string(mPartAttachedTo) + '\n';
   returnString += "Parameter type: " + std::to_string(mParameterType) + '\n';

   if (GetTypeDesignator() == TypeArticulated)
   {
      returnString += std::to_string(mNumericValue.mFloat32[0]) + ", " + std::to_string(mNumericValue.mFloat32[1]);
   }
   else if (GetTypeDesignator() == TypeAttached)
   {
      returnString += mEntityType.ToString();
   }
   else
   {
      returnString += std::to_string(mNumericValue.mUnknown[0]) + ", " + std::to_string(mNumericValue.mUnknown[1]);
   }

   return returnString;
}

DisEntityPart& DisEntityPart::operator=(const DisEntityPart& aRhs)
{
   if (this != &aRhs)
   {
      SetTypeDesignator(aRhs.GetTypeDesignator());
      mChangeIndicator = aRhs.mChangeIndicator;
      mPartAttachedTo  = aRhs.mPartAttachedTo;
      mParameterType   = aRhs.mParameterType;
      mNumericValue    = aRhs.mNumericValue;
      mEntityType      = aRhs.mEntityType;
   }
   return *this;
}

bool DisEntityPart::operator==(const DisVariableParameterRecord& aRhs)
{
   bool                 equal = true;
   const DisEntityPart* p     = dynamic_cast<const DisEntityPart*>(&aRhs);

   if (p) // is type EntityPart
   {
      equal = equal & (DisVariableParameterRecord::operator==(aRhs));
      equal = equal & (mChangeIndicator == p->mChangeIndicator);
      equal = equal & (mPartAttachedTo == p->mPartAttachedTo);
      equal = equal & (mParameterType == p->mParameterType);

      if (GetTypeDesignator() == 0) // articulated part
      {
         equal = equal & (mParameterType == p->mParameterType);
      }
      else // attached part
      {
         equal = equal & (mEntityType == p->mEntityType);
      }
   }
   else
   {
      equal = false;
   }

   return equal;
}

bool DisEntityPart::IsValid() const
{
   return mEntityType.IsValid();
}

void DisEntityPart::GetMemberData(GenI& aGenI)
{
   aGenI >> mChangeIndicator;
   aGenI >> mPartAttachedTo;
   aGenI >> mParameterType;

   if (GetTypeDesignator() == TypeArticulated)
   {
      aGenI >> mNumericValue.mFloat32[0] >> mNumericValue.mFloat32[1];
   }
   else if (GetTypeDesignator() == TypeAttached)
   {
      mEntityType.Get(aGenI);
   }
   else
   {
      aGenI >> mNumericValue.mUnknown[0] >> mNumericValue.mUnknown[1];
   }
}
