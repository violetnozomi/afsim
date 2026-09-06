//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef DIS_MUNITIONDESCRIPTORECORD_HPP
#define DIS_MUNITIONDESCRIPTORECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisDescriptorBaseRecord.hpp"
#include "DisEntityType.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

/**!
 * Defines a record containing information about the firing or detonation
 * of a munition. A munition for the purpose of this record, is defined as
 * any munition type that the "kind" field in of the DisEntityType record is
 * set to "Munition (2)". This includes both detonations as a result of its
 * intended use (e.g. firing  a missile at a target) as well as unintended
 * use detonations such as the following:
 *    1. An explosion due to a demolition event
 *    2. An explosive ordanance disposal (EOD) activity
 *    3. A secondary explosion caused by some other munition or non-munition
 *       explosion.
 */
class DIS_EXPORT DisMunitionDescriptorRecord : public DisDescriptorBaseRecord
{
public:
   DisMunitionDescriptorRecord();
   DisMunitionDescriptorRecord(const DisMunitionDescriptorRecord& aSrc);
   explicit DisMunitionDescriptorRecord(GenI& aGenI);
   ~DisMunitionDescriptorRecord() = default;

   // Input/output
   DisUint16 GetLength() const override;
   void      Get(GenI& aGenI) override;
   void      Put(GenO& aGenO) const override;

   // Validate Data
   bool IsValid() const override;

   // Operators
   DisMunitionDescriptorRecord& operator=(const DisMunitionDescriptorRecord& aRhs);
   bool                         operator==(const DisMunitionDescriptorRecord& aRhs) const;

   // Accessors
   DisEnum16 GetWarhead() const;
   DisEnum16 GetFuse() const;
   DisUint16 GetQuantity() const;
   DisUint16 GetRate() const;

   // Mutators
   void SetWarhead(DisEnum16 aWarhead);
   void SetFuse(DisEnum16 aFuse);
   void SetQuantity(DisUint16 aQuantity);
   void SetRate(DisUint16 aRate);

   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The warhead type used
   DisEnum16 mWarhead = 0;

   //! The fuse type used
   DisEnum16 mFuse = 0;

   //! The quantity of munitions
   DisUint16 mQuantity = 0;

   //! The rate of fire
   DisUint16       mRate              = 0;
   const DisUint16 cMIN_RECORD_LENGTH = 8;
};

// Accessors

inline DisEnum16 DisMunitionDescriptorRecord::GetWarhead() const
{
   return mWarhead;
}

inline DisEnum16 DisMunitionDescriptorRecord::GetFuse() const
{
   return mFuse;
}

inline DisUint16 DisMunitionDescriptorRecord::GetQuantity() const
{
   return mQuantity;
}

inline DisUint16 DisMunitionDescriptorRecord::GetRate() const
{
   return mRate;
}

// Mutators

inline void DisMunitionDescriptorRecord::SetWarhead(DisEnum16 aWarhead)
{
   mWarhead = aWarhead;
}

inline void DisMunitionDescriptorRecord::SetFuse(DisEnum16 aFuse)
{
   mFuse = aFuse;
}

inline void DisMunitionDescriptorRecord::SetQuantity(DisUint16 aQuantity)
{
   mQuantity = aQuantity;
}

inline void DisMunitionDescriptorRecord::SetRate(DisUint16 aRate)
{
   mRate = aRate;
}

// Define GenIO insertion and extraction operators
inline GenO& operator<<(GenO& aGenO, const DisMunitionDescriptorRecord& aMunitionDescriptorRecord)
{
   aMunitionDescriptorRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisMunitionDescriptorRecord& aMunitionDescriptorRecord)
{
   aMunitionDescriptorRecord.Get(aGenI);
   return aGenI;
}

inline DisMunitionDescriptorRecord& DisMunitionDescriptorRecord::operator=(const DisMunitionDescriptorRecord& aRhs)
{
   if (this != &aRhs)
   {
      DisDescriptorBaseRecord::operator=(aRhs);
      mWarhead                         = aRhs.mWarhead;
      mFuse                            = aRhs.mFuse;
      mQuantity                        = aRhs.mQuantity;
      mRate                            = aRhs.mRate;
   }
   return *this;
}

inline bool DisMunitionDescriptorRecord::operator==(const DisMunitionDescriptorRecord& aRhs) const
{
   return (mWarhead == aRhs.mWarhead) && (mFuse == aRhs.mFuse) && (mQuantity == aRhs.mQuantity) && (mRate == aRhs.mRate);
}

#endif
