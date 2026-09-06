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

#ifndef DISIOCOMMNODERECORD_HPP
#define DISIOCOMMNODERECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisEntityId.hpp"
#include "DisIOBaseRecord.hpp"
#include "DisIOCommunicationNodeIdRecord.hpp"
#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

//! Class for identification of a communications node
class DIS_EXPORT DisIOCommNodeRecord : public DisIOBaseRecord
{
public:
   DisIOCommNodeRecord();
   DisIOCommNodeRecord(const DisIOCommNodeRecord& aSrc);
   DisIOCommNodeRecord(const DisIOBaseRecord& aBaseRecord, GenI& aGenI);
   explicit DisIOCommNodeRecord(GenI& aGenI);
   ~DisIOCommNodeRecord() override = default;

   DisIOCommNodeRecord* Clone() const override;
   DisIORecordTypeEnum  GetIORecordType() const override;

   //----------------------- Script accessibility methods -----------------------
   const char* GetScriptClassName() const override;
   //--------------------- End script accessibility methods ---------------------

   // Input / Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   // virtual void Stream(std::ostream& aStream, std::string spacing) override;
   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const override;

   // Data Validation
   bool IsValid() const override;
   bool operator==(const DisRecordHeader& aOther) const;

   void SetCommNodeType(DisEnum8 aCommNodeType);
   void SetCommsNodeId(const DisIOCommunicationsNodeIdRecord aCommsNodeId);

   DisUint32                       GetCommNodeType() const;
   DisIOCommunicationsNodeIdRecord GetCommsNodeId() const;

private:
   void GetMemberData(GenI& aGenI) override;
   /*!
    * Record type can be found in the SISO-REF-010-2019.pdf section 19: Variable Record Types.
    * NOTE: Not all values are present and the document assumes additional family
    * values are incremented in accordance to the notation described in
    * previous family values.
    */
   static const DisUint32 cRECORD_TYPE = 5501;
   // fixed record size in octets
   static const DisUint32 cRECORD_SIZE_IN_OCTETS = 16;

   DisEnum8                        mCommNodeType;
   DisIOCommunicationsNodeIdRecord mCommsNodeId;
};

#endif
