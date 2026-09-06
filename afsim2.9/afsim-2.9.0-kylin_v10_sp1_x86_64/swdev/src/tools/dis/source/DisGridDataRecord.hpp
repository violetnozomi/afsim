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

#ifndef DISGRIDDATARECORD_HPP
#define DISGRIDDATARECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGridDataRepresentationType.hpp"
#include "DisSampleTypeEnum.hpp"
#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/*!
 * The Grid Data record contains the actual environmental state variable data for each grid location. The record
 * specifies the data sample type, the format by which the data are represented, field scale factor and offset
 * values, and the actual data values in 8-bit, 16-bit, or 32-bit values as a function of the representation field.
 */
class DIS_EXPORT DisGridDataRecord
{
public:
   DisGridDataRecord();
   DisGridDataRecord(const DisGridDataRecord& aSrc);
   DisGridDataRecord(const DisGridDataRecord& aSrc, GenI& aGenI);
   explicit DisGridDataRecord(GenI& aGenI);
   virtual ~DisGridDataRecord() = default;

   // Input/output
   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength();
   virtual void      Put(GenO& aGenO) const;

   // Getters
   DisEnum16                            GetSampleType() const;
   DisEnum16                            GetDataRepresentationType() const;
   const DisGridDataRepresentationType* GetGridDataRepresentationType() const;

   // Setters
   void SetGridDataRepresentationType(DisGridDataRepresentationType* aGridDataObject);
   void SetSampleType(DisEnum16 aSampleType);
   void SetDataRepresentationType(DisEnum16 aDataRepresentationType);
   // Data Validation
   virtual bool               IsValid() const;
   virtual DisGridDataRecord* Clone() const;

   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const;

private:
   void GetMemberData(GenI& aGenI);
   void GetRecord(GenI& aGenI);
   /**!
    * Sample type if SISO-REF-010 document is unspecified in version 26.
    */
   DisEnum16 mSampleType = 0;
   /**!
    * The type of data that this class contains. This field will match the specific child
    * class implementation that it represents.
    * NOTE: This field is intentionally NOT set to prevent child casting to default to
    *       Type 0 (DisGridDataRepresentationTypeZero)
    */
   DisEnum16 mDataRepresentationType = -1;
   //! Contains the specific grid data specified by the type indicate in the class
   DisGridDataRepresentationType* mGridDataRepresentationType;
   //! This is the length of the record without taking the length of the data object
   static const DisUint16 cBASE_LENGTH_IN_OCTETS = 4;
};
#endif
