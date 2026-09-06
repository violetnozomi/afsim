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

#ifndef DISIRREGULARAXISDESCRIPTORDATARECORD_HPP
#define DISIRREGULARAXISDESCRIPTORDATARECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGridAxisDescriptorBaseRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a record which represents an irregular axis descriptor data object
 */
class DIS_EXPORT DisIrregularAxisDescriptorDataRecord : public DisGridAxisDescriptorBaseRecord
{
public:
   DisIrregularAxisDescriptorDataRecord();
   DisIrregularAxisDescriptorDataRecord(const DisIrregularAxisDescriptorDataRecord& aSrc);
   DisIrregularAxisDescriptorDataRecord(GenI& aGenI);
   ~DisIrregularAxisDescriptorDataRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Getters
   DisUint16              GetNumberOfPointsOnAxis() const;
   DisUint16              GetInitialIndexX() const;
   DisFloat64             GetCoordinateScale() const;
   DisFloat64             GetCoordinateOffset() const;
   std::vector<DisUint16> GetXiNiValues() const;
   DisUint16              GetValueAtIndex(int aIndex) const;

   // Setters
   void SetNumberOfPointsOnAxis(DisUint16 aNumberOfPointsOnAxis);
   void SetInitialIndexX(DisUint16 aInitialIndexX);
   void SetCoordinateScale(DisFloat64 aCoordinateScale);
   void SetCoordinateOffset(DisFloat64 aCoordinateOffset);
   void AddXiNiValue(DisUint16 aXiNiValue);

   // Data Validation
   bool IsValid() const;

   DisIrregularAxisDescriptorDataRecord* Clone() const;

   std::string ToString() const;

private:
   /**!
    * Calculates and returns the of number of padding octets for the XiNi values. The
    * equation for which can be found on pg 278 of the IEEE 1278.1-2012
    */
   DisUint16 CalculateNumberOfPaddingOctets() const;
   void      GetMemberData(GenI& aGenI);

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

   /*!
    * This field shall specify the value that linearly scales the coordinates of the
    * grid locations for the xi axis.
    */
   DisFloat64 mCoordinateScale = 0;

   /*!
    * This field shall specify the constant offset value that shall be applied to the
    * grid locations for the xi axis(for example, the Earth’s radius is the z - axis offset).
    * The default value shall be zero
    */
   DisFloat64 mCoordinateOffset = 0;

   /*!
    * This field specifies the coordinate values for the Ni grid locations along the
    * irregular(variable spacing) xi axis for environmental data values contained within the PDU.
    */
   std::vector<DisUint16> mXiNiValues;
   // This value only takes into consideration the number of octets in this record
   static const DisUint16 cMIN_LENGTH_IN_OCTETS = 20;
};
#endif
