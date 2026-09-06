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

#ifndef DISLEFIRE_HPP
#define DISLEFIRE_HPP

#include "dis_export.h"

#include <string>

#include "DisEntityId.hpp"
#include "DisEventId.hpp"
#include "DisLiveEntityId.hpp"
#include "DisMunitionDescriptorRecord.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * Defines a DisLEFire which communicates information associated with the firing
 * of a weapon by a Live Entity
 */
class DIS_EXPORT DisLEFire : public DisPdu
{
public:
   DisLEFire();
   DisLEFire(const DisLEFire& aSrc);
   DisLEFire(const DisPdu& aPdu, GenI& aGenI);
   explicit DisLEFire(GenI& aGenI);
   ~DisLEFire() override;

   DisLEFire* Clone() const override;

   // Get PDU Type and Family enums
   int GetClass() const override;
   int GetFamily() const override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Accessors
   const DisLiveEntityId&             GetFiringEntityId() const;
   DisUint8                           GetFlags() const;
   const DisLiveEntityId&             GetTargetEntityId() const;
   const DisLiveEntityId&             GetMunitionEntityId() const;
   const DisEventId&                  GetEventId() const;
   DisUint16                          GetReferencePoint() const;
   void                               GetEntityLocation(DisInt16& aDeltaX, DisInt16& aDeltaY, DisInt16& aDeltaZ) const;
   const DisMunitionDescriptorRecord& GetMunitionDescriptorRecord() const;
   void                               GetVelocity(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const;
   DisUint16                          GetRange() const;


   // Mutators
   void SetFiringEntityId(const DisLiveEntityId& aId);
   void SetFlags(DisUint8 aFlag);
   void SetTargetEntityId(const DisLiveEntityId& aId);
   void SetMunitionEntityId(const DisLiveEntityId& aId);
   void SetEventId(const DisEventId& aId);
   void SetReferencePoint(DisUint16 aPoint);
   void SetEntityLocation(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ);
   void SetMunitionDescriptorRecord(const DisMunitionDescriptorRecord& aRecord);
   void SetVelocity(DisInt16 aX, DisInt16 aY, DisInt16 aZ);
   void SetRange(DisUint16 aRange);

   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! Identifies the firing entity
   DisLiveEntityId mFiringEntityID;

   //! Flags
   /*!
    *  Identifies those optional data fields that are being transmitted with
    *  the current PDU.
    *  -------------------------------------------------------------------------------------------------
    *  |      |          |                                                                             |
    *  | Bit  |  State   |Flag Octet 1 Meaning                                                         |
    *  |      |          |                                                                             |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   7  | 0        | Reserved for flag continuation field                                        |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   6  | 0        | Location field from which fire event occurs is not included                 |
    *  |      | 1        | Location field from which fire event occurs is included                     |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   5  | 0        | Quantity and Rate fields of the Munition Descriptor record are not included |
    *  |      | 1        | Quantity and Rate fields of the Munition Descriptor record are included     |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   4  | 0        | Warhead and Fuse fields of the Munition Descriptor record are not included  |
    *  |      |          | (use munition default)                                                      |
    *  |      | 1        | Warhead and Fuse fields of the Munition Descriptor record are present       |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   3  | 0        | Site Number and Application Number data are the same as the firing          |
    *  |      |          | entity’s and are not included in the Event ID                               |
    *  |      | 1        | Site Number and Application Number fields are included in the Event ID      |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   2  | 0        | Munition ID field is not included                                           |
    *  |      | 1        | Munition ID field is included                                               |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   1  | 0        | Site Number and Application Number data are the same as the firing          |
    *  |      |          | entity’s and are not included in the Munition ID                            |
    *  |      | 1        | Site Number and Application Number fields are included in the               |
    *  |      |          | Munition ID                                                                 |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   0  | 0        | Target Entity ID field is not included                                      |
    *  |      | 1        | Target Entity ID field is included                                          |
    *  |-----------------------------------------------------------------------------------------------|
    */
   DisUint8 mFlags = 0;

   //! Identifies the intended target.
   DisLiveEntityId mTargetEntityID = DisLiveEntityId::cLIVE_ENTITY_ID_UNKNOWN;

   //! identify the fired munition if tracking data are required.
   DisLiveEntityId mMunitionEntityID = DisLiveEntityId::cLIVE_ENTITY_ID_UNKNOWN;

   /**!
    * Contains an identification generated by the firing entity to associate related
    * firing and detonation events.
    */
   DisEventId mEventID;

   /**!
    * The number of the reference point against which the Delta-
    * X, Delta-Y, and Delta-Z location distances are calculated.
    */
   DisUint16 mReferencePoint = 0;

   /**!
    * Specify the location, in relative coordinates, from which the
    * munition was launched
    */
   UtVec3<DisInt16> mEntityLocation;

   //! Describes the type of munition fired, warhead, fuse, quantity, and rate.
   DisMunitionDescriptorRecord mMunitionDescriptorRecord;

   /**!
    * Specify the velocity of the fired munition at the point when the issuing
    * simulation application intends the externally visible effects of the launch
    * (e.g., exhaust plume ormuzzle blast) to first become apparent.
    */
   UtVec3<DisInt16> mVelocity;

   /**!
    * Specify the range in meters that an entity’s fire control system has assumed in
    * computing the fire control solution. For systems where range is unknown or unavailable
    * , this field shall contain the value zero.
    */
   DisUint16 mRange = 0;

   /**!
    * Binary Point in fixed binary point data specified by subprotocol
    * Otherwise set to three.
    */
   DisUint8 mBinaryPoint = 3;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 23;
};

// Accessors
inline int DisLEFire::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::LEFire);
}

inline int DisLEFire::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::LiveEntity);
}

inline const DisLiveEntityId& DisLEFire::GetFiringEntityId() const
{
   return mFiringEntityID;
}

inline DisUint8 DisLEFire::GetFlags() const
{
   return mFlags;
}

inline const DisLiveEntityId& DisLEFire::GetTargetEntityId() const
{
   return mTargetEntityID;
}

inline const DisLiveEntityId& DisLEFire::GetMunitionEntityId() const
{
   return mMunitionEntityID;
}

inline const DisEventId& DisLEFire::GetEventId() const
{
   return mEventID;
}

inline DisUint16 DisLEFire::GetReferencePoint() const
{
   return mReferencePoint;
}

inline void DisLEFire::GetEntityLocation(DisInt16& aDeltaX, DisInt16& aDeltaY, DisInt16& aDeltaZ) const
{
   aDeltaX = mEntityLocation[0];
   aDeltaY = mEntityLocation[1];
   aDeltaZ = mEntityLocation[2];
}

inline const DisMunitionDescriptorRecord& DisLEFire::GetMunitionDescriptorRecord() const
{
   return mMunitionDescriptorRecord;
}

inline void DisLEFire::GetVelocity(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const
{
   aX = mVelocity[0];
   aY = mVelocity[1];
   aZ = mVelocity[2];
}

inline DisUint16 DisLEFire::GetRange() const
{
   return mRange;
}

inline std::string DisLEFire::GetStringId() const
{
   return "LE FIRE";
}

// Mutators
inline void DisLEFire::SetFiringEntityId(const DisLiveEntityId& aId)
{
   mFiringEntityID = aId;
}

inline void DisLEFire::SetFlags(DisUint8 aFlag)
{
   mFlags = aFlag;
}

inline void DisLEFire::SetTargetEntityId(const DisLiveEntityId& aId)
{
   mTargetEntityID = aId;
}

inline void DisLEFire::SetMunitionEntityId(const DisLiveEntityId& aId)
{
   mMunitionEntityID = aId;
}

inline void DisLEFire::SetEventId(const DisEventId& aId)
{
   // site and application are 8-bit, event number is 16-bit
   DisUint8 site = (DisUint8)aId.GetSite(), application = (DisUint8)aId.GetApplication();
   mEventID.SetSite(site);
   mEventID.SetApplication(application);
   mEventID.SetNumber(aId.GetNumber());
}

inline void DisLEFire::SetReferencePoint(DisUint16 aPoint)
{
   mReferencePoint = aPoint;
}

inline void DisLEFire::SetEntityLocation(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ)
{
   mEntityLocation.Set(aDeltaX, aDeltaY, aDeltaZ);
}

inline void DisLEFire::SetMunitionDescriptorRecord(const DisMunitionDescriptorRecord& aRecord)
{
   mMunitionDescriptorRecord = aRecord;
}

inline void DisLEFire::SetVelocity(DisInt16 aX, DisInt16 aY, DisInt16 aZ)
{
   mVelocity.Set(aX, aY, aZ);
}

inline void DisLEFire::SetRange(DisUint16 aRange)
{
   mRange = aRange;
}

#endif
