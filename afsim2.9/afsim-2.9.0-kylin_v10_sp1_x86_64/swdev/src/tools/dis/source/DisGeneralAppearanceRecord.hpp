// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISGENERALAPPEARANCERECORD_HPP
#define DISGENERALAPPEARANCERECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * This class is a utility to set and decipher DIS Entity Appearance bits.
 * Appearance Record is an attribute of DisEntityState.  One must include
 * DisAppearanceEnums.hpp to interpret the unsigned int values set or returned here.
 * Use namespace DisEnum::Appearance::Xxxx.  Example:  unsigned int GetPaintScheme()
 * return value of 1 means:  DisEnum::Appearance::PaintScheme::Camouflage,
 * versus a return value of zero, which means xxx::Uniform.
 */
class DIS_EXPORT DisGeneralAppearanceRecord
{
public:
   DisGeneralAppearanceRecord()                                       = default;
   DisGeneralAppearanceRecord(const DisGeneralAppearanceRecord& aSrc) = default;
   DisGeneralAppearanceRecord(DisUint16 aValue);

   DisUint16 Value() const { return mGeneralAppearanceRecord; }
   void      Set(DisUint16 aValue) { mGeneralAppearanceRecord = aValue; }

   std::string ToString() const;
   void        Stream(std::ostream& aStream);

   DisGeneralAppearanceRecord* Clone() const;

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   // Operators
   bool operator==(const DisGeneralAppearanceRecord& aRhs) const;

   unsigned int GetPercentComplete() const;
   unsigned int GetDamage() const;
   unsigned int Predistributed() const;
   unsigned int GetState() const;
   unsigned int IsSmoking() const;
   unsigned int IsFlaming() const;
   unsigned int IEDPresent() const;

   void SetPercentComplete(unsigned int aValue);
   void SetDamage(unsigned int aValue);
   void SetPredistributed(unsigned int aValue);
   void SetState(unsigned int aValue);
   void SetSmoking(unsigned int aValue);
   void SetFlaming(unsigned int aValue);
   void SetIEDPresent(unsigned int aValue);

private:
   DisUint16 mGeneralAppearanceRecord = 0;
};

// Define GenIO insertion and extraction operators
inline GenO& operator<<(GenO& aGenO, const DisGeneralAppearanceRecord& aRecord)
{
   aRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisGeneralAppearanceRecord& aRecord)
{
   aRecord.Get(aGenI);
   return aGenI;
}
#endif
