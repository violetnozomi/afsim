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

#ifndef DISBASICGROUNDCOMBATVEHICLERECORD_HPP
#define DISBASICGROUNDCOMBATVEHICLERECORD_HPP

#include "dis_export.h"

#include "DisGroupEntityDescriptionRecord.hpp"

/**!
 * The specification of entity state information about an individual entity
 * within a group of ground combat vehicles
 */
class DIS_EXPORT DisBasicGroundCombatVehicleRecord : public DisGroupedEntityDescriptionRecord
{
public:
   DisBasicGroundCombatVehicleRecord() = default;
   DisBasicGroundCombatVehicleRecord(GenI& aGenI);
   DisBasicGroundCombatVehicleRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisBasicGroundCombatVehicleRecord(const DisBasicGroundCombatVehicleRecord& aSrc);

   // Input/output
   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const;

   // Data Validation
   virtual bool                               IsValid() const override;
   virtual DisBasicGroundCombatVehicleRecord* Clone() const override;
   virtual std::string                        ToString() const override;

   // Getters
   DisInt8 GetSpeed() const;
   DisInt8 GetTurretAzimuth() const;
   DisInt8 GetGunElevation() const;
   DisInt8 GetTurretSlewRate() const;
   DisInt8 GetGunElevationRate() const;

   // Setters
   void SetSpeed(const DisInt8 aSpeed);
   void SetTurretAzimuth(const DisInt8 aTurretAzimuth);
   void SetGunElevation(const DisInt8 aGunElevation);
   void SetTurretSlewRate(const DisInt8 aTurretSlewRate);
   void SetGunElevationRate(const DisInt8 aGunElevationRate);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisBasicGroundCombatVehicleRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   /**!
    * The signed magnitude value of the entity's absolute velocity vector. Negative values indicate that
    * the entity is moving backwards. Measured in (m/sec)
    */
   DisInt8 mEntitySpeed = 0;

   //! Azimuth of the vehicle's turret, represented in 25 milliradian/sec increments
   DisInt8 mTurretAzimuth = 0;

   //! Gun elevation, represented in 25 milliradian/sec increments
   DisInt8 mGunElevation = 0;

   //! Turrent slew rate, represented in 25 milliradian/sec increments
   DisInt8 mTurretSlewRate = 0;

   //! Gun elevation rate, represented in 25 milliradian/sec increments
   DisInt8 mGunElevationRate = 0;

   //! Local record size in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 5;
};
#endif
