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

#include "DisSupplyRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

// static
DisUint16 DisSupplyRecord::GetLength()
{
   return (8 + 8 + 16 + 8 + 8 + 8 + 8 + 32) / 8;
}

DisSupplyRecord::DisSupplyRecord()
   : mSupplyType()
   , mQuantity(0.0F)
{
}

DisSupplyRecord::DisSupplyRecord(const DisSupplyRecord& aSrc)
   : mSupplyType(aSrc.mSupplyType)
   , mQuantity(aSrc.mQuantity)
{
}

DisSupplyRecord::~DisSupplyRecord() {}

void DisSupplyRecord::Get(GenI& aGenI)
{
   aGenI >> mSupplyType;
   aGenI >> mQuantity;
}

void DisSupplyRecord::Put(GenO& aGenO) const
{
   aGenO << mSupplyType;
   aGenO << mQuantity;
}

DisSupplyRecord& DisSupplyRecord::operator=(const DisSupplyRecord& aRhs)
{
   if (this != &aRhs)
   {
      mSupplyType = aRhs.mSupplyType;
      mQuantity   = aRhs.mQuantity;
   }
   return *this;
}

void DisSupplyRecord::Stream(std::ostream& aStream) const
{
   aStream << " SupplyType = ";
   aStream << static_cast<int>(mSupplyType.GetEntityKind()) << ':';
   aStream << static_cast<int>(mSupplyType.GetDomain()) << ':';
   aStream << static_cast<int>(mSupplyType.GetCountry()) << ':';
   aStream << static_cast<int>(mSupplyType.GetCategory()) << ':';
   aStream << static_cast<int>(mSupplyType.GetSubcategory()) << ':';
   aStream << static_cast<int>(mSupplyType.GetSpecific()) << ':';
   aStream << static_cast<int>(mSupplyType.GetExtra());
   aStream << ", Quantity = " << mQuantity;
}

bool DisSupplyRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!mSupplyType.IsValid())
      isValid = false;
   if (!ValidateScalar(mQuantity))
      isValid = false;
   return isValid;
}
