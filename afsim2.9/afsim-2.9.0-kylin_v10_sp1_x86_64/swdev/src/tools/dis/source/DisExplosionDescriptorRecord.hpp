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

#ifndef DIS_EXPLOSIONDESCRIPTORRECORD_HPP
#define DIS_EXPLOSIONDESCRIPTORRECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisDescriptorBaseRecord.hpp"
#include "DisEntityType.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

/**!
 * Defines a record containing information the explosion of a non-munition. The exploding
 * object may be an entity for which an Entity State PDU has been issued or may be an articulated
 * or attached part of an entity. Details for the associated entity can be developed and added
 * to the existing VP records.
 *
 * NOTE: If a munition explodes, whether intentional or otherwise, it should be represented by a
 *       Munition Descriptor record and not a Explosion Description record.
 *
 */
class DIS_EXPORT DisExplosionDescriptorRecord : public DisDescriptorBaseRecord
{
public:
   DisExplosionDescriptorRecord();
   DisExplosionDescriptorRecord(const DisExplosionDescriptorRecord& aSrc);
   explicit DisExplosionDescriptorRecord(GenI& aGenI);
   ~DisExplosionDescriptorRecord() = default;

   // Input/output
   DisUint16 GetLength() const override;
   void      Get(GenI& aGenI) override;
   void      Put(GenO& aGenO) const override;

   // Validate Data
   bool IsValid() const override;

   // Operators
   DisExplosionDescriptorRecord& operator=(const DisExplosionDescriptorRecord& aRhs);
   bool                          operator==(const DisExplosionDescriptorRecord& aRhs) const;

   // Accessors
   DisEnum16  GetExplosiveMaterial() const;
   DisFloat32 GetExplosiveForce() const;

   // Mutators
   void SetExplosiveMaterial(DisEnum16 aExplosiveMaterial);
   void SetExplosiveForce(DisFloat32 aExplosiveForce);

   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! Indicates the material exploded
   DisEnum16 mExplosiveMaterial = 0;

   //! The fuse type used
   DisUint16 mPadding = 0;

   //! The quantity of munitions
   DisFloat32 mExplosiveForce = 0;

   const DisUint16 cMIN_RECORD_LENGTH_OCTETS = 8;
};

inline DisExplosionDescriptorRecord& DisExplosionDescriptorRecord::operator=(const DisExplosionDescriptorRecord& aRhs)
{
   if (this != &aRhs)
   {
      DisDescriptorBaseRecord::operator=(aRhs);
      mExplosiveMaterial               = aRhs.mExplosiveMaterial;
      mExplosiveForce                  = aRhs.mExplosiveForce;
   }
   return *this;
}

inline bool DisExplosionDescriptorRecord::operator==(const DisExplosionDescriptorRecord& aRhs) const
{
   return mExplosiveMaterial == aRhs.mExplosiveMaterial && mExplosiveForce == aRhs.mExplosiveForce;
}
inline DisEnum16 DisExplosionDescriptorRecord::GetExplosiveMaterial() const
{
   return mExplosiveMaterial;
}
inline DisFloat32 DisExplosionDescriptorRecord::GetExplosiveForce() const
{
   return mExplosiveForce;
}
inline void DisExplosionDescriptorRecord::SetExplosiveMaterial(DisEnum16 aExplosiveMaterial)
{
   mExplosiveMaterial = aExplosiveMaterial;
}
inline void DisExplosionDescriptorRecord::SetExplosiveForce(DisFloat32 aExplosiveForce)
{
   mExplosiveForce = aExplosiveForce;
}
#endif
