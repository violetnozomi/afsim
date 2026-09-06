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

#ifndef DISREGULARAXISDECRIPTORDATARECORD_HPP
#define DISREGULARAXISDECRIPTORDATARECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGridAxisDescriptorBaseRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a record which represents a regular axis descriptor data object
 */
class DIS_EXPORT DisRegularAxisDescriptorDataRecord : public DisGridAxisDescriptorBaseRecord
{
public:
   DisRegularAxisDescriptorDataRecord();
   DisRegularAxisDescriptorDataRecord(const DisRegularAxisDescriptorDataRecord& aSrc);
   DisRegularAxisDescriptorDataRecord(GenI& aGenI);
   virtual ~DisRegularAxisDescriptorDataRecord() = default;

   // Input/output
   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength();
   virtual void      Put(GenO& aGenO) const;

   // Getters
   DisUint16 GetNumberOfPointsOnAxis() const;
   DisUint16 GetInitialIndexX() const;

   // Setters
   void SetNumberOfPointsOnAxis(DisUint16 aNumberOfPointsOnAxis);
   void SetInitialIndexX(DisUint16 aInitialIndexX);

   // Data Validation
   virtual bool IsValid() const;

   virtual DisRegularAxisDescriptorDataRecord* Clone() const;

   virtual std::string ToString() const;

private:
   virtual void GetMemberData(GenI& aGenI);

   /*!
    * Number of Points on xi Axis. This field shall specify the number of grid locations along the xi
    * axis for the environmental state variable data contained within the current PDU.
    */
   DisUint16 mNumberOfPointsOnAxis = 0;

   /*!
    * This field shall specify the index of the initial grid point for the current PDU
    * along the xi domain axis.A value of zero shall indicate that the PDU grid and the domain grid
    * have the same initial point.
    */
   DisUint16 mInitialIndexX = 0;

   static const DisUint16 cMIN_LENGTH_IN_OCTETS = 4;
};
#endif
