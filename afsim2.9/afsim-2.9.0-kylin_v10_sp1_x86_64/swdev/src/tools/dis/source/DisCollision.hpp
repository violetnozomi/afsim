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

#ifndef DISCOLLISION_HPP
#define DISCOLLISION_HPP

#include "dis_export.h"

#include <string>

#include "DisEntityId.hpp"
#include "DisEventId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

/**!
 * Use to communicate information about a collision between two simulated entities or between
 * a simulated entity and another object in the simulated world (e.g., a cultural feature such
 * as a bridge or building).
 */
class DIS_EXPORT DisCollision : public DisPdu
{
public:
   enum
   {
      // Collision Type [UID 189] - TODO implement at higher level?
      cINELASTIC                   = 0,
      cELASTIC                     = 1,
      cREFUELING_BOOM_NOZZLE_CLEAR = 55
   };

   DisCollision();
   DisCollision(const DisCollision& aSrc);
   DisCollision(const DisPdu& aPdu, GenI& aGenI);
   explicit DisCollision(GenI& aGenI);
   ~DisCollision() = default;

   DisCollision* Clone() const override;
   int           GetClass() const override;
   int           GetFamily() const override;

   // Accessors
   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetIssuingEntityId() const;
   const DisEntityId& GetCollidingEntityId() const;
   const DisEventId&  GetEventId() const;
   DisUint8           GetCollisionType() const;
   void               GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   DisFloat32         GetMass() const;
   void               GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const;

   // Mutators
   void SetIssuingEntityId(const DisEntityId& aEntityId);
   void SetCollidingEntityId(const DisEntityId& aEntityId);
   void SetEventId(const DisEventId& aEventId);
   void SetCollisionType(const DisUint8 aCollisionType);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetMass(DisFloat32 aMass);
   void SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ);

   // Logging
   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   //! The entity issuing the collision PDU
   DisEntityId mIssuingEntityId;

   //! The entity that was collided with by issuing entity
   DisEntityId mCollidingEntityId;

   /**!
    * The event ID of the collision. This is used to associate this collision
    * event with other related collision events
    */
   DisEventId mEventId;

   //! The type of collision
   DisUint8 mCollisionType = cINELASTIC;

   DisUint8 mPadding = 0;

   //! The velocity of the (issuing) entity at the time of the collision
   DisFloat32 mVelocity[3] = {0, 0, 0};

   //! The mass of the (issuing) entity at the time of the collision
   DisFloat32 mMass = 0;

   //! The location of the (issuing) entity at the time of the collision
   DisFloat32 mLocation[3] = {0, 0, 0};

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const int cPDU_MIN_LENGTH_OCTETS = 48;
};

//! Accessor for entity ID that issued this PDU
inline const DisEntityId& DisCollision::GetIssuingEntityId() const
{
   return mIssuingEntityId;
}

//! Accessor for entity ID that collided with issuing entity
inline const DisEntityId& DisCollision::GetCollidingEntityId() const
{
   return mCollidingEntityId;
}

//! Accessor for the event ID for this collision
inline const DisEventId& DisCollision::GetEventId() const
{
   return mEventId;
}

//! Accessor for type of collision
inline DisUint8 DisCollision::GetCollisionType() const
{
   return mCollisionType;
}

//! Accessor for issuing entity's velocity
inline void DisCollision::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

//! Accessor for issuing entity's mass (kilograms)
inline DisFloat32 DisCollision::GetMass() const
{
   return mMass;
}

//! Accessor for issuing entity's location
inline void DisCollision::GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}

// Mutators

//! Mutator for issuing entity's ID
inline void DisCollision::SetIssuingEntityId(const DisEntityId& aEntityId)
{
   mIssuingEntityId = aEntityId;
}

//! Mutator for colliding entity's ID
inline void DisCollision::SetCollidingEntityId(const DisEntityId& aEntityId)
{
   mCollidingEntityId = aEntityId;
}

//! Mutator for this collision's event ID
inline void DisCollision::SetEventId(const DisEventId& aEventId)
{
   mEventId = aEventId;
}

//! Mutator for collision type
inline void DisCollision::SetCollisionType(DisUint8 aCollisionType)
{
   mCollisionType = aCollisionType;
}

//! Mutator for issuing entity's velocity
inline void DisCollision::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

//! Mutator for issuing entity's mass (kilograms)
inline void DisCollision::SetMass(DisFloat32 aMass)
{
   mMass = aMass;
}

//! Mutator for issuing entity's location
inline void DisCollision::SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

//! Returns string ID for this PDU type
inline std::string DisCollision::GetStringId() const
{
   return "COLLISION";
}

#endif
