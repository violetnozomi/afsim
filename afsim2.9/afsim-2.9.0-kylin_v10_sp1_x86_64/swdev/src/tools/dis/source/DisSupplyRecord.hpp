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

#ifndef DISSUPPLYRECORD_HPP
#define DISSUPPLYRECORD_HPP

#include "dis_export.h"

#include <ostream>

#include "DisEntityType.hpp"
#include "DisTypes.hpp"


class GenI;
class GenO;

class DIS_EXPORT DisSupplyRecord
{
public:
   DisSupplyRecord();
   DisSupplyRecord(const DisSupplyRecord& aSrc);
   ~DisSupplyRecord();

   // Accessors

   static DisUint16 GetLength();

   const DisEntityType& GetSupplyTypeValue() const;
   DisFloat32           GetQuantity() const;

   // Mutators

   void SetSupplyTypeValue(const DisEntityType& aSupplyType);
   void SetQuantity(DisFloat32 aQuantity);

   // Input/output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;
   void Stream(std::ostream& aStream) const;

   // Operators

   DisSupplyRecord& operator=(const DisSupplyRecord& aRhs);

   //! Data Validation
   bool IsValid() const;

private:
   DisEntityType mSupplyType;
   DisFloat32    mQuantity;
};

// Accessors

inline const DisEntityType& DisSupplyRecord::GetSupplyTypeValue() const
{
   return mSupplyType;
}

inline DisFloat32 DisSupplyRecord::GetQuantity() const
{
   return mQuantity;
}

// Mutators

inline void DisSupplyRecord::SetSupplyTypeValue(const DisEntityType& aSupplyType)
{
   mSupplyType = aSupplyType;
}
inline void DisSupplyRecord::SetQuantity(DisFloat32 aQuantity)
{
   mQuantity = aQuantity;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisSupplyRecord& aPart)
{
   aPart.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisSupplyRecord& aPart)
{
   aPart.Get(aGenI);
   return aGenI;
}

#endif
