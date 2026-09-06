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

#ifndef DISLEDETONATION_HPP
#define DISLEDETONATION_HPP

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
 * Defines a DisLEDetonation PDU which communicates information associated with the
 * impact or detonation of a munition.
 */
class DIS_EXPORT DisLEDetonation : public DisPdu
{
public:
   DisLEDetonation();
   DisLEDetonation(const DisLEDetonation& aSrc);
   DisLEDetonation(const DisPdu& aPdu, GenI& aGenI);
   explicit DisLEDetonation(GenI& aGenI);
   ~DisLEDetonation() override = default;

   DisLEDetonation* Clone() const override;

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
   DisUint8                           GetDetonationFlag1() const;
   DisUint8                           GetDetonationFlag2() const;
   const DisLiveEntityId&             GetTargetEntityId() const;
   const DisLiveEntityId&             GetMunitionEntityId() const;
   const DisEventId&                  GetEventId() const;
   DisUint16                          GetReferencePoint() const;
   void                               GetLocationWC(DisInt16& aDeltaX, DisInt16& aDeltaY, DisInt16& aDeltaZ) const;
   void                               GetVelocity(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const;
   void                               GetMunitionOrientation(DisInt16& aPsi, DisInt16& aTheta, DisInt16& aPhi) const;
   const DisMunitionDescriptorRecord& GetMunitionDescriptorRecord() const;
   void                               GetLocationEC(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const;
   DisEnum8                           GetDetonationResult() const;

   // Mutators
   void SetFiringEntityId(const DisLiveEntityId& aId);
   void SetDetonationFlag1(DisUint8 aFlag);
   void SetDetonationFlag2(DisUint8 aFlag);
   void SetTargetEntityId(const DisLiveEntityId& aId);
   void SetMunitionEntityId(const DisLiveEntityId& aId);
   void SetEventId(const DisEventId& aId);
   void SetReferencePoint(DisUint16 aPoint);
   void SetLocationWC(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ);
   void SetVelocity(DisInt16 aX, DisInt16 aY, DisInt16 aZ);
   void SetMunitionOrientation(DisInt16 aPsi, DisInt16 aTheta, DisInt16 aPhi);
   void SetMunitionDescriptorRecord(const DisMunitionDescriptorRecord& aRecord);
   void SetLocationEC(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ);
   void SetDetonationResult(DisEnum8 aResult);

   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! Identifies the firing entity
   DisLiveEntityId mFiringEntityID = DisLiveEntityId::cLIVE_ENTITY_ID_UNKNOWN;

   //! Detonation Flag 1
   /*!
    *  Identifies those optional data fields that are being transmitted with
    *  the current PDU.
    *  -------------------------------------------------------------------------------------------------
    *  |      |          |                                                                             |
    *  | Bit  |  State   |Flag Octet 1 Meaning                                                         |
    *  |      |          |                                                                             |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   7  | 0        | Flag Octet 2 is not included                                                |
    *  |      | 1        | Flag Octet 2 is included                                                    |
    *  |-----------------------------------------------------------------------------------------------|
    *  |      | 0        | Location in Entity Coordinates field is not included; Location in           |
    *  |   6  |          | Relative World Coordinates field is included                                |
    *  |      | 1        | Location in Entity Coordinates field is included; Location in Relative      |
    *  |      |          | World Coordinates field is not included                                     |
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
   DisUint8 mDetonationFlag1 = 0;

   //! Detonation Flag 2
   /*!
    *  Identifies those optional data fields that are being transmitted with
    *  the current PDU.
    *  -------------------------------------------------------------------------------------------------
    *  |      |          |                                                                             |
    *  | Bit  |  State   |Flag Octet 2 Meaning                                                         |
    *  |      |          |                                                                             |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   7  | 0        | Reserved for flag continuation flag                                         |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   6  | 0        | Unused                                                                      |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   5  | 0        | Unused                                                                      |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   4  | 0        | Unused                                                                      |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   3  | 0        | Unused                                                                      |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   2  | 0        | Unused                                                                      |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   1  | 0        | Event Number field is not included in Event ID                              |
    *  |      | 1        | Event Number field is included in Event ID                                  |
    *  |-----------------------------------------------------------------------------------------------|
    *  |   0  | 0        | Munition Orientation field is not included                                  |
    *  |      | 1        | Munition Orientation field is included                                      |
    *  |-----------------------------------------------------------------------------------------------|
    */
   DisUint8 mDetonationFlag2 = 0;

   //! Identifies the intended target.
   DisLiveEntityId mTargetEntityID = DisLiveEntityId::cLIVE_ENTITY_ID_UNKNOWN;

   //! Identifies the fired munition if tracking data are required.
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

   //! Specify the location, in world coordinates, from which the munition was launched
   UtVec3<DisInt16> mLocationWC;

   /**!
    * Specify the velocity of the fired munition at the point when the issuing
    * simulation application intends the externally visible effects of the launch
    * (e.g., exhaust plume ormuzzle blast) to first become apparent.
    */
   UtVec3<DisInt16> mVelocity;

   /**!
    * Specify the orientation of the munition in entitybased coordinates at
    * the time of detonation.
    */
   UtVec3<DisInt16> mMunitionOrientation;

   //! Describes the type of munition fired, warhead, fuse, quantity, and rate.
   DisMunitionDescriptorRecord mMunitionDescriptorRecord;

   /**!
    * Specify the location of the detonation or impact in the target
    * entity’s coordinates.
    */
   UtVec3<DisInt16> mLocationEC;

   //! Specify the result of the detonation
   DisEnum8 mDetonationResult = 0;

   /**!
    * Binary Point in fixed binary point data specified by subprotocol
    * Otherwise set to three.
    */
   DisUint8 mBinaryPoint = 3;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 20;
};

// Accessors
inline int DisLEDetonation::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::LEDetonation);
}

inline int DisLEDetonation::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::LiveEntity);
}

inline const DisLiveEntityId& DisLEDetonation::GetFiringEntityId() const
{
   return mFiringEntityID;
}

inline DisUint8 DisLEDetonation::GetDetonationFlag1() const
{
   return mDetonationFlag1;
}

inline DisUint8 DisLEDetonation::GetDetonationFlag2() const
{
   return mDetonationFlag2;
}

inline const DisLiveEntityId& DisLEDetonation::GetTargetEntityId() const
{
   return mTargetEntityID;
}

inline const DisLiveEntityId& DisLEDetonation::GetMunitionEntityId() const
{
   return mMunitionEntityID;
}

inline const DisEventId& DisLEDetonation::GetEventId() const
{
   return mEventID;
}

inline DisUint16 DisLEDetonation::GetReferencePoint() const
{
   return mReferencePoint;
}

inline void DisLEDetonation::GetLocationWC(DisInt16& aDeltaX, DisInt16& aDeltaY, DisInt16& aDeltaZ) const
{
   aDeltaX = mLocationWC[0];
   aDeltaY = mLocationWC[1];
   aDeltaZ = mLocationWC[2];
}

inline const DisMunitionDescriptorRecord& DisLEDetonation::GetMunitionDescriptorRecord() const
{
   return mMunitionDescriptorRecord;
}

inline void DisLEDetonation::GetLocationEC(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const
{
   aX = mLocationEC[0];
   aY = mLocationEC[1];
   aZ = mLocationEC[2];
}

inline DisEnum8 DisLEDetonation::GetDetonationResult() const
{
   return mDetonationResult;
}

inline void DisLEDetonation::GetVelocity(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const
{
   aX = mVelocity[0];
   aY = mVelocity[1];
   aZ = mVelocity[2];
}

inline void DisLEDetonation::GetMunitionOrientation(DisInt16& aPsi, DisInt16& aTheta, DisInt16& aPhi) const
{
   aPsi   = mMunitionOrientation[0];
   aTheta = mMunitionOrientation[1];
   aPhi   = mMunitionOrientation[2];
}


inline std::string DisLEDetonation::GetStringId() const
{
   return "LE DETONATION";
}

// Mutators
inline void DisLEDetonation::SetFiringEntityId(const DisLiveEntityId& aId)
{
   mFiringEntityID = aId;
}

inline void DisLEDetonation::SetDetonationFlag1(DisUint8 aFlag)
{
   mDetonationFlag1 = aFlag;
}

inline void DisLEDetonation::SetDetonationFlag2(DisUint8 aFlag)
{
   mDetonationFlag2 = aFlag;
}

inline void DisLEDetonation::SetTargetEntityId(const DisLiveEntityId& aId)
{
   mTargetEntityID = aId;
}

inline void DisLEDetonation::SetMunitionEntityId(const DisLiveEntityId& aId)
{
   mMunitionEntityID = aId;
}

inline void DisLEDetonation::SetEventId(const DisEventId& aId)
{
   // site and application are 8-bit, event number is 16-bit
   DisUint8 site = (DisUint8)aId.GetSite(), application = (DisUint8)aId.GetApplication();
   mEventID.SetSite(site);
   mEventID.SetApplication(application);
   mEventID.SetNumber(aId.GetNumber());
}

inline void DisLEDetonation::SetReferencePoint(DisUint16 aPoint)
{
   mReferencePoint = aPoint;
}

inline void DisLEDetonation::SetLocationWC(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ)
{
   mLocationWC.Set(aDeltaX, aDeltaY, aDeltaZ);
}

inline void DisLEDetonation::SetMunitionDescriptorRecord(const DisMunitionDescriptorRecord& aRecord)
{
   mMunitionDescriptorRecord = aRecord;
}

inline void DisLEDetonation::SetLocationEC(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ)
{
   mLocationEC.Set(aDeltaX, aDeltaY, aDeltaZ);
}

inline void DisLEDetonation::SetDetonationResult(DisEnum8 aResult)
{
   mDetonationResult = aResult;
}

inline void DisLEDetonation::SetVelocity(DisInt16 aX, DisInt16 aY, DisInt16 aZ)
{
   mVelocity.Set(aX, aY, aZ);
}

inline void DisLEDetonation::SetMunitionOrientation(DisInt16 aPsi, DisInt16 aTheta, DisInt16 aPhi)
{
   mMunitionOrientation.Set(aPsi, aTheta, aPhi);
}
#endif
