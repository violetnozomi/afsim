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

#ifndef DISENVIRONMENTALRECORD_HPP
#define DISENVIRONMENTALRECORD_HPP

#include "dis_export.h"

#include <memory>

#include "DisEnvironmentalAndGeometricRecordTypeEnum.hpp"
#include "DisEnvironmentalSpaceDataRecord.hpp"
#include "DisEnvironmentalStateRecord.hpp"
#include "DisGeometryRecord.hpp"
#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines the base record type for all Environmental Records
 */
class DIS_EXPORT DisEnvironmentalRecord : public DisRecordHeader
{
public:
   DisEnvironmentalRecord();
   DisEnvironmentalRecord(DisEnum32 aRecordType);
   DisEnvironmentalRecord(const DisEnvironmentalRecord& aSrc);
   DisEnvironmentalRecord(const DisEnvironmentalRecord& aDisRecord, GenI& aGenI);
   DisEnvironmentalRecord(GenI& aGenI);
   ~DisEnvironmentalRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength();
   virtual void      Put(GenO& aGenO) const;

   // Data Validation
   virtual bool IsValid() const;

   virtual DisEnvironmentalRecord* Clone() const;

   // Getters
   DisUint16                        GetLengthBits() const;
   DisUint8                         GetIndex() const;
   DisEnvironmentalSpaceDataRecord& GetGeometryOrStateRecord();

   // Setters
   void SetIndex(DisUint8 aIndex);
   void SetGeometryOrStateRecord(std::unique_ptr<DisEnvironmentalSpaceDataRecord> aGeometryOrStateRecord);

   // Logging
   virtual void        Stream(std::ostream& aStream) const;
   virtual std::string ToString() const;

protected:
   /**!
    * Helper method which compartimentalizes the process of getting the correct record from the wire
    * @param GenI& The input content
    * @return boolean - true if class was found, false otherwise
    */
   bool        GetSubRecordType(GenI& aGenI);
   DisUint16   GetCorrectSubRecordLength() const;
   void        PutSubRecordType(GenO& aGenO) const;
   bool        GetSubRecordIsValid() const;
   std::string GetSubRecordToString() const;
   /**
    * Since there a a lot of types for the Geomtric and Environmental state (sub) records,
    * I wrapped all the types (Geometric and Environmental) under one
    * class: DisEnvironmentalSpaceDataRecord.
    */
   static const DisInt16 GEOMETRY_RECORD_MIN_BOUND      = 251;
   static const DisInt16 GEOMETRY_RECORD_MAX_BOUND      = 267;
   static const DisInt16 ENVIRONMENTAL_RECORD_MIN_BOUND = 268;
   static const DisInt16 ENVIRONMENTAL_RECORD_MAX_BOUND = 269;

private:
   void GetMemberData(GenI& aGenI);
   //! The length of this record
   DisUint16 mLengthBits = 0;
   //! Identifies the sequentially numbered record index
   DisUint8                                         mIndex   = 0;
   DisUint8                                         mPadding = 0;
   std::unique_ptr<DisEnvironmentalSpaceDataRecord> mGeometryOrStateRecord;
   DisUint64                                        mPaddingToBoundry      = 0;
   static const DisUint16                           cBASE_LENGTH_IN_OCTETS = 8;
};
#endif
