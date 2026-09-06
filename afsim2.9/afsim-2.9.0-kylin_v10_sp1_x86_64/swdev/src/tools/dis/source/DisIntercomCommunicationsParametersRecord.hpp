// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#ifndef DISINTERCOMCOMMUNICATIONSPARAMETERSRECORD_HPP
#define DISINTERCOMCOMMUNICATIONSPARAMETERSRECORD_HPP

#include "dis_export.h"

#include <vector>

#include "DisICTypeRecord.hpp"
#include "DisPtrContainer.hpp"
#include "DisRecordHeader.hpp"

/**!
 * Defines parameters to the Intercom Control PDU.
 */
class DIS_EXPORT DisIntercomCommunicationsParametersRecord
{
public:
   DisIntercomCommunicationsParametersRecord() = default;
   DisIntercomCommunicationsParametersRecord(GenI& aGenI);
   DisIntercomCommunicationsParametersRecord(const DisIntercomCommunicationsParametersRecord& aSrc);

   // Input / Output
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO) const;
   DisUint16 GetLength();

   const DisICTypeRecord& GetRecordSpecificField() const;

   void SetRecordSpecificField(const DisICTypeRecord& aRecordSpecificField);


   // Data Validation
   bool IsValid() const;

   std::string ToString() const;
   void        Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI);
   /**!
    * The variable record which contains the record-specific fields. They consist of variable-length fields
    * specifying additional intercom communications parameters. The length of each record is a function of the
    * record type field (found in the base class).
    *
    * NOTE: The padding for this record is handled by the specific type implementations
    */
   DisICTypeRecord mRecordSpecificField;
};
#endif
