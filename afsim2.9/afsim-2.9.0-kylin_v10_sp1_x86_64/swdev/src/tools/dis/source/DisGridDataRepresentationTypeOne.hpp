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

#ifndef DISGRIDDATAREPRESENTATIONTYPEONE_HPP
#define DISGRIDDATAREPRESENTATIONTYPEONE_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGridDataRecord.hpp"
#include "DisGridDataRepresentationType.hpp"
#include "DisSampleTypeEnum.hpp"
#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

using std::cerr;

/*!
 * Child class of the DisGridDataRepresentationType record class. It contains a stream of 2 byte
 * values which are used to store a variety of user defined information. The format and kind of information
 * carried by this record is defined and agreed upon outside of and prior to a simulation start.
 */
class DIS_EXPORT DisGridDataRepresentationTypeOne : public DisGridDataRepresentationType
{
public:
   DisGridDataRepresentationTypeOne() = default;
   DisGridDataRepresentationTypeOne(const DisGridDataRepresentationTypeOne& aSrc);
   explicit DisGridDataRepresentationTypeOne(GenI& aGenI);
   ~DisGridDataRepresentationTypeOne() = default;

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() const override;
   void      Put(GenO& aGenO) const override;

   // Getters
   std::vector<DisUint16> GetDataValues() const;
   DisFloat32             GetFieldScale() const;
   DisFloat32             GetFieldOffset() const;

   // Setters
   void SetDataValues(std::vector<DisUint16> aDataValues);
   void AddDataValue(DisUint16 aDataValue);
   void SetFieldScale(DisFloat32 aFieldScale);
   void SetFieldOffset(DisFloat32 aFieldOffset);

   // Data Validation
   bool                              IsValid() const override;
   DisGridDataRepresentationTypeOne* Clone() const override;

   std::string ToString() const override;

private:
   //! Copies the contents of a given DisGridDataRecordTypeZero Record from a source to this PDU
   void      Copy(const DisGridDataRepresentationTypeOne& aSrc);
   void      GetMemberData(GenI& aGenI);
   DisUint16 CalculateNumberOfPaddingOctets() const override;
   /**!
    * Specifies the constant scale factor used to scale the environmental state
    * variable data values contained in this record
    */
   DisFloat32 mFieldScale = 0;

   /**!
    * Specifies the contant offset used to scale the environmental state
    * variable data values contained in this record
    */
   DisFloat32 mFieldOffset = 0;

   DisUint16 mPadding = 0;

   std::vector<DisUint16> mDataValues;

   /**!
    * This is the length of the record without taking the length of the data associated
    * by the pointer.
    */
   static const DisUint16 cMIN_LENGTH_IN_OCTETS = 10;
};
#endif
