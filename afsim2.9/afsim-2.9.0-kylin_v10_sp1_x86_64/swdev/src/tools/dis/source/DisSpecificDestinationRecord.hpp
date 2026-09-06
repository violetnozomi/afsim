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

#ifndef DISSPECIFICDESTINATIONRECORD_HPP
#define DISSPECIFICDESTINATIONRECORD_HPP

#include "dis_export.h"

#include "DisDestinationLineStateCommandEnum.hpp"
#include "DisEntityId.hpp"
#include "DisICTypeRecord.hpp"

//! This record is used to send an Intercom Control PDU directly to a specific intercom device
class DIS_EXPORT DisSpecificDestinationRecord : public DisICTypeRecord
{
public:
   DisSpecificDestinationRecord();
   DisSpecificDestinationRecord(GenI& aGenI);
   DisSpecificDestinationRecord(const DisICTypeRecord& aRecord, GenI& aGenI);
   DisSpecificDestinationRecord(const DisSpecificDestinationRecord& aSrc);
   ~DisSpecificDestinationRecord() = default;

   DisSpecificDestinationRecord* Clone() const override;

   // Input / Output
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;
   void      Get(GenI& aGenI) override;

   // Data Validation
   bool IsValid() const override;

   // Getters
   DisEntityId                        GetDestinationIntercomReferenceIdentifier() const;
   DisUint16                          GetDestinationIntercomNumber() const;
   DisUint8                           GetDestinationLineId() const;
   DisUint8                           GetDestinationPriority() const;
   DisDestinationLineStateCommandEnum GetDestinationLineStateCommand() const;

   // Setters
   void SetDestinationIntercomReferenceIdentifier(const DisEntityId& aDestinationIntercomReferenceIdentifier);
   void SetDestinationIntercomNumber(DisUint16 aDestinationIntercomNumber);
   void SetDestinationLineId(DisUint8 aDestinationLineId);
   void SetDestinationPriority(DisUint8 aDestinationPriority);
   void SetDestinationLineStateCommand(DisDestinationLineStateCommandEnum aDestinationLineStateCommand);

   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI);

   //! The reference identifer of the destination intercom
   DisEntityId mDestinationIntercomReferenceIdentifier = DisEntityId::ENTITY_ID_UNKNOWN;

   //! The Number id of the destination intercom
   DisUint16 mDestinationIntercomNumber = 0;

   //! The line id of the destination intercom
   DisUint8 mDestinationLineId = 0;

   //! The priority this record over other messages being transmitted on the same intercom channel
   DisUint8 mDestinationPriority = 0;

   //! The line state command of the destination intercom
   DisEnum8 mDestinationLineStateCommand = DisDestinationLineStateCommandEnum::NONE;
   DisUint8 mPadding                     = 0;
   //! The length (in octets) to read this record
   static const DisUint16 cMIN_LENGTH_OCTETS = 12;
};
#endif
