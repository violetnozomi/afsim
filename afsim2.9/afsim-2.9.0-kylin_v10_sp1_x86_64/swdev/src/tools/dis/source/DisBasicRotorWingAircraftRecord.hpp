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

#ifndef DISBASICROTORWINGAIRCRAFTRECORD_HPP
#define DISBASICROTORWINGAIRCRAFTRECORD_HPP

#include "dis_export.h"

#include "DisGroupEntityDescriptionRecord.hpp"
#include "DisTypes.hpp"

/**!
 * The specification of entity state information about an individual entity
 * within a group of fixed rotor wing aircraft
 */
class DIS_EXPORT DisBasicRotorWingAircraftRecord : public DisGroupedEntityDescriptionRecord
{
public:
   DisBasicRotorWingAircraftRecord() = default;
   DisBasicRotorWingAircraftRecord(GenI& aGenI);
   DisBasicRotorWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisBasicRotorWingAircraftRecord(const DisBasicRotorWingAircraftRecord& aSrc);

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                             IsValid() const override;
   virtual DisBasicRotorWingAircraftRecord* Clone() const override;
   virtual std::string                      ToString() const override;

   // Getters
   DisUint8 GetFuelStatus() const;
   DisInt8  GetHorizontalMovementDeviation() const;
   DisInt8  GetVerticalMovementDeviation() const;
   DisInt16 GetMovementSpeed() const;
   DisInt8  GetTurretAzimuth() const;
   DisInt8  GetGunElevation() const;
   DisInt8  GetTurretSlewRate() const;
   DisInt8  GetGunElevationRate() const;

   // Setters
   void SetFuelStatus(const DisUint8 aFuelStatus);
   void SetHorizontalDeviation(const DisInt8 aHorizontalDeviation);
   void SetVerticalDeviation(const DisInt8 aVerticalDeviation);
   void SetMovementSpeed(const DisInt16 aMovementSpeed);
   void SetTurretAzimuth(const DisInt8 aTurretAzimuth);
   void SetGunElevation(const DisInt8 aGunElevation);
   void SetTurretSlewRate(const DisInt8 aTurretSlewRate);
   void SetGunElevationRate(const DisInt8 aGunElevationRate);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisBasicRotorWingAircraftRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! The amount of primary fuel remaining
   DisUint8 mFuelStatus = 0;

   //! The horizontal deviation from the Entity Orientation
   DisInt8 mHorizontalDeviation = 0;
   //! The vertial deviation from the Entity Orientation
   DisInt8 mVerticalDeviation = 0;

   /**!
    * The signed magnitude value of the entity's absolute velocity vector. Negative values indicate that
    * the entity is moving backwards. Entity Speed shall be specified in 0.1 meter per second increments
    * measured in (m/sec)
    */
   DisInt16 mMovementSpeed = 0;

   //! Azimuth of the turret, represented in 25 milliradian/sec increments
   DisInt8 mTurretAzimuth = 0;

   //! Elevation of the gun, represented in 25 milliradian/sec increments
   DisInt8 mGunElevation = 0;

   //! Turret slew rate, represented in 25 milliradian/sec increments
   DisInt8 mTurretSlewRate = 0;

   //! Gun elevation rate, represented in 25 milliradian/sec increments
   DisInt8 mGunElevationRate = 0;

   //! Size of local record in octets
   static const DisUint16 cLOCAL_RECORD_SIZE_IN_OCTETS = 9;
};
#endif
