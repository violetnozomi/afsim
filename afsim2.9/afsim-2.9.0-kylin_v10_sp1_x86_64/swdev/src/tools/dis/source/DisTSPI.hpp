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

#ifndef DISTSPI_HPP
#define DISTSPI_HPP

#include "dis_export.h"

#include <string>
#include <vector>

#include "DisEntityId.hpp"
#include "DisLiveEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * Defines information about a live entity's state vector.
 */
class DIS_EXPORT DisTSPI : public DisPdu
{
public:
   DisTSPI();
   DisTSPI(const DisTSPI& aSrc);
   DisTSPI(const DisPdu& aPdu, GenI& aGenI);
   explicit DisTSPI(GenI& aGenI);
   ~DisTSPI() override = default;

   DisTSPI* Clone() const override;

   // Get PDU Type and Family enums
   int GetClass() const override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Accessors
   const DisLiveEntityId& GetLiveEntityId() const;
   DisUint8               GetTSPIFlag() const;
   DisUint16              GetReferencePoint() const;
   const void             GetEntityLocation(DisInt16& aDeltaX, DisInt16& aDeltaY, DisInt16& aDeltaZ) const;
   const void             GetEntityLinearVelocity(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const;
   const void             GetEntityOrientation(DisInt8& aPsi, DisInt8& aTheta, DisInt8& aPhi) const;
   const void             GetPositionError(DisInt16& aHorizontalError, DisInt16& aVerticalError) const;
   const void GetOrientationError(DisInt16& aAzimuthError, DisInt16& aElevationError, DisInt16& aRotationError) const;
   DisEnum8   GetDeadReckoningAlgorithm() const;
   const void GetEntityLinearAcceleration(DisInt8& aX, DisInt8& aY, DisInt8& aZ) const;
   const void GetEntityAngularVelocity(DisInt8& aX, DisInt8& aY, DisInt8& aZ) const;
   DisInt16   GetMeasuredSpeed() const;
   DisUint8   GetSystemSpecificDataLength() const;

   // Mutators
   void SetLiveEntityId(const DisLiveEntityId& aId);
   void SetTSPIFlag(DisUint8 aFlag);
   void SetReferencePoint(DisUint16 aPoint);
   void SetEntityLocation(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ);
   void SetEntityLinearVelocity(DisInt16 aX, DisInt16 aY, DisInt16 aZ);
   void SetEntityOrientation(DisInt8 aPsi, DisInt8 aTheta, DisInt8 aPhi);
   void SetPositionError(DisInt16 aHorizontalError, DisInt16 aVerticalError);
   void SetOrientationError(DisInt16 aAzimuthError, DisInt16 aElevationError, DisInt16 aRotationError);
   void SetDeadReckoningAlgorithm(DisEnum8 aAlgorithm);
   void SetEntityLinearAcceleration(DisInt8 aX, DisInt8 aY, DisInt8 aZ);
   void SetEntityAngularVelocity(DisInt8 aX, DisInt8 aY, DisInt8 aZ);
   void SetMeasuredSpeed(DisInt16 aSpeed);
   void SetSystemSpecificDataLength(DisUint8 aLength);
   void SetSystemSpecificData(const std::vector<unsigned char>& aData);

   std::vector<unsigned char>::iterator       GetSystemSpecificDataBegin();
   std::vector<unsigned char>::const_iterator GetSystemSpecificDataBegin() const;
   std::vector<unsigned char>::iterator       GetSystemSpecificDataEnd();
   std::vector<unsigned char>::const_iterator GetSystemSpecificDataEnd() const;

   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! Identifies the entity issuing the PDU
   DisLiveEntityId mEntityID = DisLiveEntityId::cLIVE_ENTITY_ID_UNKNOWN;

   //! TSPI Flag
   /*!
    *  Identifies those optional data fields that are being transmitted with
    *  the current PDU.
    *  ----------------------------------------------------------------------------------
    *  |      |          |                                                              |
    *  | Bit  |  State   |Flag Octet 1 Meaning                                          |
    *  |      |          |                                                              |
    *  |--------------------------------------------------------------------------------|
    *  |   7  | 0        | Reserved for flag continuation flag                          |
    *  |--------------------------------------------------------------------------------
    *  |   6  | 0        | System-Specific Data field is not included                   |
    *  |      | 1        | System-Specific Data field is included with the length       |
    *  |      |          |  defined by the System-Specific Data Length field            |
    *  |--------------------------------------------------------------------------------|
    *  |   5  | 0        | Measured Speed field is not included                         |
    *  |      | 1        | Measured Speed field is included                             |
    *  |--------------------------------------------------------------------------------|
    *  |   4  | 0        | Dead Reckoning Parameter field is not included               |
    *  |      |          | (use subprotocol default)                                    |
    *  |      | 1        | Dead Reckoning Parameter field is included; algorithm        |
    *  |      |          | requirements establish the need and existence of the         |
    *  |      |          | Entity LinearAcceleration and Entity Angular Velocity fields |
    *  |--------------------------------------------------------------------------------|
    *  |   3  | 0        | Orientation Error field is not included                      |
    *  |      | 1        | Orientation Error field is included                          |
    *  |--------------------------------------------------------------------------------|
    *  |   2  | 0        | Position Error field is not included                         |
    *  |      | 1        | Position Error field is included                             |
    *  |--------------------------------------------------------------------------------|
    *  |   1  | 0        | Entity Orientation field is not included                     |
    *  |      | 1        | Entity Orientation field is included                         |
    *  |--------------------------------------------------------------------------------|
    *  |   0  | 0        | Entity Linear Velocity field is not included                 |
    *  |      | 1        | Entity Linear Velocity field is included                     |
    *  |--------------------------------------------------------------------------------|
    */
   DisUint8 mTSPIFlag = 0;

   /**!
    * The number of the reference point against which the Delta-
    * X, Delta-Y, and Delta-Z distances are calculated.
    */
   DisUint16 mReferencePoint = 0;

   /**!
    * Location of a grouped entity is specified by three offsets. The offsets are determined by the difference
    * (meters) of the grouped entity from the group reference point
    */
   UtVec3<DisInt16> mEntityLocation;

   /**!
    * This optional field specifies an entity’s linear velocity. The coordinate system
    * for an entity’s linear velocity depends on the dead reckoning algorithm used.
    */
   UtVec3<DisInt16> mEntityLinearVelocity;

   //! This optional field shall specify an entity’s orientation.
   UtVec3<DisInt8> mEntityOrientation;

   /**!
    * This optional field shall specify the potential horizontal and vertical
    * position error of the reporting entity.
    */
   UtVec2<DisInt16> mPositionError;

   /**!
    * This optional field shall specify the potential orientation error of the
    * reporting entity.
    */
   UtVec3<DisInt16> mOrientationError;

   /**!
    * Dead Reckoning Parameters fields (optional)
    * Dead reckoning algorithm in use,
    */
   DisEnum8 mDeadReckoningAlgorithm = 0;

   //! Entity linear acceleration (optional based on dead reckoning algorithm in use)
   UtVec3<DisInt8> mEntityLinearAcceleration;

   //! Entity angular velocity (optional based on dead reckoning algorithm in use)
   UtVec3<DisInt8> mEntityAngularVelocity;

   //! Specify the entity’s own measurement of speed
   DisInt16 mMeasuredSpeed = 0;

   /**!
    * optional field shall specify the number of octets of system-specific
    * data that immediately follow this field.
    */
   DisUint8 mSystemSpecificDataLength = 0;

   /**!
    * optional field shall contain user-defined data and be specified in length (of octets)
    * by mSystemSpecificDataLength
    */
   std::vector<unsigned char> mSystemSpecificData;

   /**!
    * Binary Point in fixed binary point data specified by subprotocol
    * Otherwise set to three.
    */
   DisUint8 mBinaryPoint = 3;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 42;
};

// Accessors
inline int DisTSPI::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::TSPI);
}

inline const DisLiveEntityId& DisTSPI::GetLiveEntityId() const
{
   return mEntityID;
}

inline DisUint8 DisTSPI::GetTSPIFlag() const
{
   return mTSPIFlag;
}

inline DisUint16 DisTSPI::GetReferencePoint() const
{
   return mReferencePoint;
}

inline const void DisTSPI::GetEntityLocation(DisInt16& aDeltaX, DisInt16& aDeltaY, DisInt16& aDeltaZ) const
{
   aDeltaX = mEntityLocation[0];
   aDeltaY = mEntityLocation[1];
   aDeltaZ = mEntityLocation[2];
}

inline const void DisTSPI::GetEntityLinearVelocity(DisInt16& aX, DisInt16& aY, DisInt16& aZ) const
{
   aX = mEntityLinearVelocity[0];
   aY = mEntityLinearVelocity[1];
   aZ = mEntityLinearVelocity[2];
}

inline const void DisTSPI::GetEntityOrientation(DisInt8& aPsi, DisInt8& aTheta, DisInt8& aPhi) const
{
   aPsi   = mEntityOrientation[0];
   aTheta = mEntityOrientation[1];
   aPhi   = mEntityOrientation[2];
}

inline const void DisTSPI::GetPositionError(DisInt16& aHorizontalError, DisInt16& aVerticalError) const
{
   aHorizontalError = mPositionError[0];
   aVerticalError   = mPositionError[1];
}

inline const void DisTSPI::GetOrientationError(DisInt16& aAzimuthError, DisInt16& aElevationError, DisInt16& aRotationError) const
{
   aAzimuthError   = mOrientationError[0];
   aElevationError = mOrientationError[1];
   aRotationError  = mOrientationError[2];
}

inline DisEnum8 DisTSPI::GetDeadReckoningAlgorithm() const
{
   return mDeadReckoningAlgorithm;
}

inline const void DisTSPI::GetEntityLinearAcceleration(DisInt8& aX, DisInt8& aY, DisInt8& aZ) const
{
   aX = mEntityLinearAcceleration[0];
   aY = mEntityLinearAcceleration[1];
   aZ = mEntityLinearAcceleration[2];
}

inline const void DisTSPI::GetEntityAngularVelocity(DisInt8& aX, DisInt8& aY, DisInt8& aZ) const
{
   aX = mEntityAngularVelocity[0];
   aY = mEntityAngularVelocity[1];
   aZ = mEntityAngularVelocity[2];
}

inline DisInt16 DisTSPI::GetMeasuredSpeed() const
{
   return mMeasuredSpeed;
}

inline DisUint8 DisTSPI::GetSystemSpecificDataLength() const
{
   return mSystemSpecificDataLength;
}

inline std::string DisTSPI::GetStringId() const
{
   return "DisTSPI";
}

// Mutators
inline void DisTSPI::SetLiveEntityId(const DisLiveEntityId& aId)
{
   mEntityID = aId;
}

inline void DisTSPI::SetTSPIFlag(DisUint8 aFlag)
{
   mTSPIFlag = aFlag;
}

inline void DisTSPI::SetReferencePoint(DisUint16 aPoint)
{
   mReferencePoint = aPoint;
}

inline void DisTSPI::SetEntityLocation(DisInt16 aDeltaX, DisInt16 aDeltaY, DisInt16 aDeltaZ)
{
   mEntityLocation.Set(aDeltaX, aDeltaY, aDeltaZ);
}

inline void DisTSPI::SetEntityLinearVelocity(DisInt16 aX, DisInt16 aY, DisInt16 aZ)
{
   mEntityLinearVelocity.Set(aX, aY, aZ);
}

inline void DisTSPI::SetEntityOrientation(DisInt8 aPsi, DisInt8 aTheta, DisInt8 aPhi)
{
   mEntityOrientation.Set(aPsi, aTheta, aPhi);
}

inline void DisTSPI::SetPositionError(DisInt16 aHorizontalError, DisInt16 aVerticalError)
{
   mPositionError.Set(aHorizontalError, aVerticalError);
}

inline void DisTSPI::SetOrientationError(DisInt16 aAzimuthError, DisInt16 aElevationError, DisInt16 aRotationError)
{
   mOrientationError.Set(aAzimuthError, aElevationError, aRotationError);
}

inline void DisTSPI::SetDeadReckoningAlgorithm(DisEnum8 aAlgorithm)
{
   mDeadReckoningAlgorithm = aAlgorithm;
}

inline void DisTSPI::SetEntityLinearAcceleration(DisInt8 aX, DisInt8 aY, DisInt8 aZ)
{
   mEntityLinearAcceleration.Set(aX, aY, aZ);
}

inline void DisTSPI::SetEntityAngularVelocity(DisInt8 aX, DisInt8 aY, DisInt8 aZ)
{
   mEntityAngularVelocity.Set(aX, aY, aZ);
}

inline void DisTSPI::SetMeasuredSpeed(DisInt16 aSpeed)
{
   mMeasuredSpeed = aSpeed;
}

inline void DisTSPI::SetSystemSpecificDataLength(DisUint8 aLength)
{
   mSystemSpecificDataLength = aLength;
}

inline void DisTSPI::SetSystemSpecificData(const std::vector<unsigned char>& aData)
{
   mSystemSpecificData = aData;
}

inline std::vector<unsigned char>::iterator DisTSPI::GetSystemSpecificDataBegin()
{
   return mSystemSpecificData.begin();
}

inline std::vector<unsigned char>::const_iterator DisTSPI::GetSystemSpecificDataBegin() const
{
   return mSystemSpecificData.begin();
}

inline std::vector<unsigned char>::iterator DisTSPI::GetSystemSpecificDataEnd()
{
   return mSystemSpecificData.end();
}

inline std::vector<unsigned char>::const_iterator DisTSPI::GetSystemSpecificDataEnd() const
{
   return mSystemSpecificData.end();
}
#endif
