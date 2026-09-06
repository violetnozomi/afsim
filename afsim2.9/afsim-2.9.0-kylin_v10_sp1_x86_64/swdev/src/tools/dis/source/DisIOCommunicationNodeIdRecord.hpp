//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DISIOCOMMUNICATIONSNODEIDRECORD_HPP
#define DISIOCOMMUNICATIONSNODEIDRECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisEntityId.hpp"
#include "DisIOBaseRecord.hpp"
#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

//! Class for identification of a communications node
class DIS_EXPORT DisIOCommunicationsNodeIdRecord : public DisIOBaseRecord
{
public:
   DisIOCommunicationsNodeIdRecord();
   DisIOCommunicationsNodeIdRecord(const DisIOCommunicationsNodeIdRecord& aSrc);
   DisIOCommunicationsNodeIdRecord(const DisIOBaseRecord& aBaseRecord, GenI& aGenI);
   explicit DisIOCommunicationsNodeIdRecord(GenI& aGenI);
   ~DisIOCommunicationsNodeIdRecord() override = default;

   DisIOCommunicationsNodeIdRecord* Clone() const override;
   DisIORecordTypeEnum              GetIORecordType() const override;

   //----------------------- Script accessibility methods -----------------------
   const char* GetScriptClassName() const override;
   //--------------------- End script accessibility methods ---------------------

   // Input / Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;
   bool        operator==(const DisIOCommunicationsNodeIdRecord& aOther) const;
   // Data Validation
   bool IsValid() const override;

   void SetEntityId(const DisEntityId aEntityId);
   void SetElementId(const DisUint16 aElementId);

   DisEntityId GetEntityId() const;
   DisUint16   GetElementId() const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! the entity containing the communications node
   DisEntityId mEntityId;
   //! a particular communications element (e.g., radio, network
   //! component, or modem) within a given entity.
   DisUint16 mElementId;
   /*!
    * Record type can be found in the SISO-REF-010-2019.pdf section 19: Variable Record Types.
    * NOTE: Not all values are present and the document assumes additional family
    * values are incremented in accordance to the notation described in
    * previous family values.
    */
   static const DisUint32 cRECORD_TYPE = 5502;
   // fixed record size in octets
   static const DisUint32 cRECORD_SIZE_IN_OCTETS = 8;
};

#endif
