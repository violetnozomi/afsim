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

#ifndef DISENCODINGSCHEMERECORD_HPP
#define DISENCODINGSCHEMERECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisEncodingSchemeRecordEnums.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

/**!
 * This record stores either the encoding type or number of TDL messages for a specific encoding scheme.
 * If the record holds an encoding type, then the getter for TDL messages will return 0 and vice versa for the encoding
 * type getter. For more documation about this record type, please reference IEEE 1278.1-2012 section 7.7.3.d
 */
class DIS_EXPORT DisEncodingSchemeRecord
{
public:
   DisEncodingSchemeRecord() = default;
   DisEncodingSchemeRecord(DisUint32 aValue);

   DisUint16 Value() const { return mEncodingSchemeRecord; }
   void      Set(DisUint16 aValue) { mEncodingSchemeRecord = aValue; }

   std::string ToString() const;
   void        Stream(std::ostream& aStream);

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   // Operators
   bool operator==(const DisEncodingSchemeRecord& aRhs) const;

   unsigned int GetEncodingTypeOrNumberOfTDLMessages() const;
   unsigned int GetEncodingClass() const;

   void SetEncodingTypeOrNumberOfTDLMessages(unsigned int aValue);
   void SetEncodingClass(unsigned int aValue);

private:
   void      GetMemberData(GenI& aGenI);
   DisUint16 mEncodingSchemeRecord = 0;
};

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisEncodingSchemeRecord& aRecord)
{
   aRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisEncodingSchemeRecord& aRecord)
{
   aRecord.Get(aGenI);
   return aGenI;
}
#endif
