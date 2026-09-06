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

#ifndef DISICTYPERECORD_HPP
#define DISICTYPERECORD_HPP

#include "dis_export.h"

#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

//! Base class for the records stored by the DisIntercomCommunicationParametersRecord
class DIS_EXPORT DisICTypeRecord
{
public:
   DisICTypeRecord() = default;
   DisICTypeRecord(DisEnum16 aRecordType, DisUint16 aRecordLength);
   DisICTypeRecord(GenI& aGenI);
   DisICTypeRecord(const DisICTypeRecord& aSrc);
   ~DisICTypeRecord() = default;

   virtual DisICTypeRecord* Clone() const;

   // Input / Output
   virtual DisUint16 GetLength();
   virtual void      Get(GenI& aGenI);
   virtual void      Put(GenO& aGenO) const;

   DisEnum16 GetRecordType() const;
   DisUint16 GetRecordLength() const;

   void SetRecordType(DisEnum16 aRecordType);
   void SetRecordLength(DisUint16 aRecordLength);

   virtual void        Stream(std::ostream& aStream) const;
   virtual std::string ToString() const;

   // Data Validation
   virtual bool IsValid() const;

private:
   virtual void GetMemberData(GenI& aGenI);

   //! The record type, for further information, see SISO-REF-010-2019 Enumerations v26.pdf
   DisEnum16 mRecordType = 0;

   //! Length of this record, for further information, see SISO-REF-010-2019 Enumerations v26.pdf
   DisUint16 mRecordLength = 0;

   static const DisUint16 cBASE_LENGTH_OCTETS = 4;
};
#endif
