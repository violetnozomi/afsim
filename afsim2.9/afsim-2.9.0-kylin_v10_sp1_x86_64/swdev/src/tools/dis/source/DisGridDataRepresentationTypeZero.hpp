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

#ifndef DISGRIDDATAREPRESENTATIONTYPEZERO_HPP
#define DISGRIDDATAREPRESENTATIONTYPEZERO_HPP

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
 * Child class of the DisGridDataRepresentationType record class. It contains a stream of 1 byte
 * values which are used to store a variety of user defined information. The format and kind of information
 * carried by this record is defined and agreed upon outside of and prior to a simulation start.
 *
 * NOTE: This representation type is specifically designed to be flexbile to accomodate anything
 *       from character data to data expressed as floating point numbers
 */
class DIS_EXPORT DisGridDataRepresentationTypeZero : public DisGridDataRepresentationType
{
public:
   DisGridDataRepresentationTypeZero();
   DisGridDataRepresentationTypeZero(const DisGridDataRepresentationTypeZero& aSrc);
   explicit DisGridDataRepresentationTypeZero(GenI& aGenI);
   ~DisGridDataRepresentationTypeZero() = default;

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() const override;
   void      Put(GenO& aGenO) const override;

   // Getters
   std::vector<DisUint8> GetDataValues() const;

   void SetDataValues(std::vector<DisUint8> aDataValues);
   void AddDataValue(DisUint8 aDataValue);

   // Data Validation
   bool                               IsValid() const override;
   DisGridDataRepresentationTypeZero* Clone() const override;

   std::string ToString() const override;

private:
   //! Copies the contents of a given DisGridDataRecordTypeZero Record from a source to this PDU
   void      Copy(const DisGridDataRepresentationTypeZero& aSrc);
   void      GetMemberData(GenI& aGenI);
   DisUint16 CalculateNumberOfPaddingOctets() const override;

   DisUint8 mPadding = 0;

   std::vector<DisUint8> mDataValues;

   static const DisUint16 cMIN_LENGTH_IN_OCTETS = 0;
};
#endif
