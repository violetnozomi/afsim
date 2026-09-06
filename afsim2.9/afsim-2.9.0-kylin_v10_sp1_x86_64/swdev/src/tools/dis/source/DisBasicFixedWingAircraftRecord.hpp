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

#ifndef DISBASICFIXEDWINGAIRCRAFTRECORD_HPP
#define DISBASICFIXEDWINGAIRCRAFTRECORD_HPP

#include "dis_export.h"

#include "DisGroupEntityDescriptionRecord.hpp"

//! Contains additional information regarding an entity found within a group of fixed wing aircraft
class DIS_EXPORT DisBasicFixedWingAircraftRecord : public DisGroupedEntityDescriptionRecord
{
public:
   DisBasicFixedWingAircraftRecord() = default;
   DisBasicFixedWingAircraftRecord(GenI& aGenI);
   DisBasicFixedWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisBasicFixedWingAircraftRecord(const DisBasicFixedWingAircraftRecord& aSrc);

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                             IsValid() const override;
   virtual DisBasicFixedWingAircraftRecord* Clone() const override;
   virtual std::string                      ToString() const override;

   // Getters
   DisUint8 GetFuelStatus() const;
   DisInt8  GetHorizontalMovementDeviation() const;
   DisInt8  GetVerticalMovementDeviation() const;
   DisInt16 GetMovementSpeed() const;

   // Setters
   void SetFuelStatus(const DisUint8 aFuelStatus);
   void SetHorizontalDeviation(const DisInt8 aHorizontalDeviation);
   void SetVerticalDeviation(const DisInt8 aVerticalDeviation);
   void SetMovementSpeed(const DisInt16 aMovementSpeed);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisBasicFixedWingAircraftRecord& aRhs) const;

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

   //! Local record size in octets
   static const DisUint16 cLOCAL_RECORD_SIZE_IN_OCTETS = 5;
};
#endif
