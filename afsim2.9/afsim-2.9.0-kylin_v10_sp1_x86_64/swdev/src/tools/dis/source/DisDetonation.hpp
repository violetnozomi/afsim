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

#ifndef DISDETONATE_HPP
#define DISDETONATE_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityId.hpp"
#include "DisEntityPart.hpp"
#include "DisEntityType.hpp"
#include "DisEventId.hpp"
#include "DisPdu.hpp"
#include "DisPtrVec.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"

class GenI;
class GenO;

/*! The detonation or impact of munitions, as well as non-munition explosions, the burst or initial bloom of
 *   chaff, and the ignition of a flare
 */
class DIS_EXPORT DisDetonation : public DisPdu
{
public:
   DisDetonation();
   DisDetonation(DisPdu& aPdu, GenI& aGenI);
   DisDetonation(GenI& aGenI);
   ~DisDetonation() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId&   GetFiringEntity() const;
   const DisEntityId&   GetTargetEntity() const;
   const DisEntityId&   GetWeaponEntity() const;
   const DisEventId&    GetEvent() const;
   void                 GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void                 GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const;
   const DisEntityType& GetWeaponType() const;
   DisEnum16            GetWarhead() const;
   DisEnum16            GetFuse() const;
   DisUint16            GetQuantity() const;
   DisUint16            GetRate() const;
   DisEnum8             GetDetonationResult() const;
   void GetImpactLocation(DisFloat32& aImpactLocationX, DisFloat32& aImpactLocationY, DisFloat32& aImpactLocationZ) const;

   // Mutators

   void SetFiringEntity(const DisEntityId& aFiringEntity);
   void SetTargetEntity(const DisEntityId& aTargetEntity);
   void SetWeaponEntity(const DisEntityId& aWeaponEntity);
   void SetEvent(const DisEventId& aEvent);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);
   void SetWeaponType(const DisEntityType& aWeaponType);
   void SetWarhead(DisEnum16 aWarhead);
   void SetFuse(DisEnum16 aFuse);
   void SetQuantity(DisUint16 aQuantity);
   void SetRate(DisUint16 aRate);
   void SetDetonationResult(DisEnum8 aDetonationResult);
   void SetImpactLocation(DisFloat32 aImpactLocationX, DisFloat32 aImpactLocationY, DisFloat32 aImpactLocationZ);

   // Input/Output

   DisUint16   GetLength() override;
   void        Get(GenI& aGenI) override;
   void        Put(GenO& aGenO) override;
   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

   // Articulated part list maintenance

   void                 AddEntityPart(const DisEntityPart& aPart);
   const DisEntityPart* GetEntityPart(int aIndex) const;
   void                 RemoveEntityPart(int aIndex);
   DisUint8             GetEntityPartCount() const { return mEntityPartCount; }

private:
   void GetMemberData(GenI& aGenI);
   void RemoveAllEntityParts();

   /*!
    * Specifies the entity that fired the munition
    */
   DisEntityId mFiringEntity;
   /*!
    * Specifies the target entity of the munition
    */
   DisEntityId mTargetEntity;
   /*!
    * Identifies the type of munition or expendable entity
    */
   DisEntityId mWeaponEntity;
   /*!
    * Contains the same data as the event ID field of the Fire PDU
    * that communicated the launch of the munition or expendable
    */
   DisEventId mEvent;
   /*!
    * Specifies the velocity of the munition immediately before detonation/impact,
    * the velocity of a non-munition entity immediately before exploding, or the
    * the velocity of an expendable immediately before a chaff burst or ignition of a flare.
    * The velocity is represented in world coordinates.
    */
   DisFloat32 mVelocity[3];
   /*!
    * Specifies the location of the detonation in world coordinates.
    */
   DisFloat64 mLocation[3];
   // The following fields are necessary for describing the types of munitions avaliable for firing
   /*!
    * Specifies the type of weapon
    */
   DisEntityType mWeaponType;
   /*!
    * Specifies the type of warhead
    */
   DisEnum16 mWarhead;
   /*!
    * Specifies the fusing
    */
   DisEnum16 mFuse;
   /*!
    * Represents the number of rounds fired in the burst or the number of munitions simultaneously launched
    */
   DisUint16 mQuantity;
   /*!
    * Indicates the rate of fire such as rounds per minute for a munition.
    * If the quantity field is equal to one then the rate should be equal to zero.
    */
   DisUint16 mRate;
   /*!
    * Specifies the location of the munition's detonation
    */
   DisFloat32 mImpactLocation[3];
   /*!
    * Specifies the result of the detonation
    */
   DisEnum8 mDetonationResult;
   /*!
    * Indicates the number of entity parts involved in this detonation
    */
   DisUint8 mEntityPartCount;
   /*!
    * The list of entity parts involved with this detonation
    */
   DisPtrVec<DisEntityPart> mEntityPart;
};

// Accessors

inline const DisEntityId& DisDetonation::GetFiringEntity() const
{
   return mFiringEntity;
}
inline const DisEntityId& DisDetonation::GetTargetEntity() const
{
   return mTargetEntity;
}
inline const DisEntityId& DisDetonation::GetWeaponEntity() const
{
   return mWeaponEntity;
}
inline const DisEventId& DisDetonation::GetEvent() const
{
   return mEvent;
}
inline void DisDetonation::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}
inline void DisDetonation::GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}
inline const DisEntityType& DisDetonation::GetWeaponType() const
{
   return mWeaponType;
}
inline DisEnum16 DisDetonation::GetWarhead() const
{
   return mWarhead;
}
inline DisEnum16 DisDetonation::GetFuse() const
{
   return mFuse;
}
inline DisUint16 DisDetonation::GetQuantity() const
{
   return mQuantity;
}
inline DisUint16 DisDetonation::GetRate() const
{
   return mRate;
}
inline void DisDetonation::GetImpactLocation(DisFloat32& aImpactLocationX,
                                             DisFloat32& aImpactLocationY,
                                             DisFloat32& aImpactLocationZ) const
{
   aImpactLocationX = mImpactLocation[0];
   aImpactLocationY = mImpactLocation[1];
   aImpactLocationZ = mImpactLocation[2];
}
inline DisEnum8 DisDetonation::GetDetonationResult() const
{
   return mDetonationResult;
}

// Mutators

inline void DisDetonation::SetFiringEntity(const DisEntityId& aFiringEntity)
{
   mFiringEntity = aFiringEntity;
}
inline void DisDetonation::SetTargetEntity(const DisEntityId& aTargetEntity)
{
   mTargetEntity = aTargetEntity;
}
inline void DisDetonation::SetWeaponEntity(const DisEntityId& aWeaponEntity)
{
   mWeaponEntity = aWeaponEntity;
}
inline void DisDetonation::SetEvent(const DisEventId& aEvent)
{
   mEvent = aEvent;
}
inline void DisDetonation::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}
inline void DisDetonation::SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}
inline void DisDetonation::SetWeaponType(const DisEntityType& aWeaponType)
{
   mWeaponType = aWeaponType;
}
inline void DisDetonation::SetWarhead(DisEnum16 aWarhead)
{
   mWarhead = aWarhead;
}
inline void DisDetonation::SetFuse(DisEnum16 aFuse)
{
   mFuse = aFuse;
}
inline void DisDetonation::SetQuantity(DisUint16 aQuantity)
{
   mQuantity = aQuantity;
}
inline void DisDetonation::SetRate(DisUint16 aRate)
{
   mRate = aRate;
}
inline void DisDetonation::SetImpactLocation(DisFloat32 aImpactLocationX,
                                             DisFloat32 aImpactLocationY,
                                             DisFloat32 aImpactLocationZ)
{
   mImpactLocation[0] = aImpactLocationX;
   mImpactLocation[1] = aImpactLocationY;
   mImpactLocation[2] = aImpactLocationZ;
}
inline void DisDetonation::SetDetonationResult(DisEnum8 aDetonationResult)
{
   mDetonationResult = aDetonationResult;
}

#endif
