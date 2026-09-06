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

#ifndef DISBASICGROUNDSOLDIERRECORD_HPP
#define DISBASICGROUNDSOLDIERRECORD_HPP

#include "dis_export.h"

#include "DisGroupEntityDescriptionRecord.hpp"

/**!
 * Specification of entity state information about an individual entity within a
 * group of ground combat soldiers.
 */
class DIS_EXPORT DisBasicGroundSoldierRecord : public DisGroupedEntityDescriptionRecord
{
public:
   DisBasicGroundSoldierRecord() = default;
   DisBasicGroundSoldierRecord(GenI& aGenI);
   DisBasicGroundSoldierRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisBasicGroundSoldierRecord(const DisBasicGroundSoldierRecord& aSrc);

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                         IsValid() const override;
   virtual DisBasicGroundSoldierRecord* Clone() const override;

   // Getters
   DisInt8 GetSpeed() const;
   DisInt8 GetHeadAzimuth() const;
   DisInt8 GetHeadElevation() const;
   DisInt8 GetHeadScanRate() const;
   DisInt8 GetHeadElevationRate() const;

   // Setters
   void SetSpeed(const DisInt8 aSpeed);
   void SetHeadAzimuth(const DisInt8 aHeadAzimuth);
   void SetHeadElevation(const DisInt8 aHeadElevation);
   void SetHeadScanRate(const DisInt8 aHeadScanRate);
   void SetHeadElevationRate(const DisInt8 aHeadElevationRate);

   virtual std::string ToString() const override;

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisBasicGroundSoldierRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   /**!
    * The signed magnitude value of the entity's absolute velocity vector. Negative values indicate that
    * the entity is moving backwards. Measured in (m/sec)
    */
   DisInt8 mEntitySpeed = 0;

   //! Azimuth of the soldier's head, represented in 25 milliradian/sec increments
   DisInt8 mHeadAzimuth = 0;

   //! Head elevation, represented in 25 milliradian/sec increments
   DisInt8 mHeadElevation = 0;

   //! Head scan rate, represented in 25 milliradian/sec increments
   DisInt8 mHeadScanRate = 0;

   //! Head elevation rate, represented in 25 milliradian/sec increments
   DisInt8 mHeadElevationRate = 0;

   //! Local record size in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 5;
};
#endif
