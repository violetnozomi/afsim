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

#ifndef DISGRIDDATATYPEREPRESENTATIONTWO_HPP
#define DISGRIDDATATYPEREPRESENTATIONTWO_HPP

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
 * Child class of the DisGridDataRepresentationType record class. It contains a stream of 4 byte
 * values which are used to store a variety of user defined information. The format and kind of information
 * carried by this record is defined and agreed upon outside of and prior to a simulation start.
 */
class DIS_EXPORT DisGridDataRepresentationTypeTwo : public DisGridDataRepresentationType
{
public:
   DisGridDataRepresentationTypeTwo();
   DisGridDataRepresentationTypeTwo(const DisGridDataRepresentationTypeTwo& aSrc);
   explicit DisGridDataRepresentationTypeTwo(GenI& aGenI);
   ~DisGridDataRepresentationTypeTwo() = default;

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() const override;
   void      Put(GenO& aGenO) const override;

   // Getters
   std::vector<DisFloat32> GetDataValues() const;
   // Setters
   void SetDataValues(std::vector<DisFloat32> aDataValues);
   void AddDataValue(DisFloat32 aDataValue);

   // Data Validation
   bool                              IsValid() const override;
   DisGridDataRepresentationTypeTwo* Clone() const override;

   std::string ToString() const override;

private:
   //! Copies the contents of a given DisGridDataRecordTypeZero Record from a source to this PDU
   void Copy(const DisGridDataRepresentationTypeTwo& aSrc);
   void GetMemberData(GenI& aGenI);

   DisUint16 mPadding = 0;

   std::vector<DisFloat32> mDataValues;

   static const DisUint16 cMIN_LENGTH_IN_OCTETS = 2;
};
#endif
