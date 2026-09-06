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

#ifndef DISGRIDAXISDESCRIPTORBASERECORD_HPP
#define DISGRIDAXISDESCRIPTORBASERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityType.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a record which provides detailed information about each grid axes: constant
 * or variable spacing, origin coordinate, endpoint coordinate, number of grid points
 * along the axis, interleaf factor, axis type, number of axis grid points, coordinate
 * scale factor coordinate offset value, and grid locaiton coordinates.
 */
class DIS_EXPORT DisGridAxisDescriptorBaseRecord
{
public:
   DisGridAxisDescriptorBaseRecord() = default;
   DisGridAxisDescriptorBaseRecord(const DisGridAxisDescriptorBaseRecord& aSrc);
   DisGridAxisDescriptorBaseRecord(GenI& aGenI);
   ~DisGridAxisDescriptorBaseRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Getters
   DisFloat64 GetDomainInitialX() const;
   DisFloat64 GetDomainFinalX() const;
   DisUint16  GetDomainPoints() const;
   DisUint8   GetInterleafFactor() const;
   DisEnum8   GetAxisType() const;

   // Setters
   void SetDomainInitialX(DisFloat64 aDomainInitialX);
   void SetDomainFinalX(DisFloat64 aDomainFinalX);
   void SetDomainPoints(DisUint16 aDomainPointsX);
   void SetInterleafFactor(DisUint8 aInterleafFactor);
   void SetAxisType(DisEnum8 aAxisType);

   // Data Validation
   bool IsValid() const;

   DisGridAxisDescriptorBaseRecord* Clone() const;

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisGridAxisDescriptorBaseRecord& aRhs) const;
   bool operator!=(const DisGridAxisDescriptorBaseRecord& aRhs) const;
   bool operator<(const DisGridAxisDescriptorBaseRecord& aRhs) const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

private:
   void GetMemberData(GenI& aGenI);

   /**!
    * This field shall specify the coordinate of the origin (or initial value) for the xi axis
    * for the environmental state variable data contained within the bounding domain
    */
   DisFloat64 mDomainInitialX = 0;

   /**!
    * This field shall specify the coordinate of the endpoint (or final value) for the xi axis
    * for the environmental state variable data contained within the bounding domain
    */
   DisFloat64 mDomainFinalX = 0;

   /**!
    * This field shall specify the number of grid points along the xi domain axis for the
    * environmental state variable data.This number may exceed the number of points along an axis for a
    * single Gridded Data PDU if there are multiple PDUs being issued to contain all the data.
    */
   DisUint16 mDomainPointsX = 0;

   /**!
    * This field shall specify the integer valued interleaf factor along a domain (grid)
    * axis.A value of one shall indicate no subsampling(interleaving), while integer values greater than
    * one shall indicate the sampling frequency along an axis
    */
   DisUint8 mInterleafFactor = 0;

   /**!
    * This field shall specify the type of grid axis represented with the Grid Axis Descriptor
    * record.Setting this field to Regular Axis(0) shall indicate a grid axis with constant grid spacing,
    * while setting it to Irregular Axis(1) shall indicate a grid axis with variable grid spacing.
    * Two values: 0 and 1.
    */
   DisEnum8 mAxisType = 0;

   //! This is the length of the record in octets
   static const DisUint16 cRECORD_BASE_LENGTH_IN_OCTETS = 20;
};
#endif
