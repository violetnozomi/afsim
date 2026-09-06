// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISGROUPDESTINATIONRECORD_HPP
#define DISGROUPDESTINATIONRECORD_HPP

#include "dis_export.h"

#include "DisDestinationLineStateCommandEnum.hpp"
#include "DisICTypeRecord.hpp"

//! This record shall be used to send an Intercom Control PDU to an arbitrary group.
class DIS_EXPORT DisGroupDestinationRecord : public DisICTypeRecord
{
public:
   DisGroupDestinationRecord();
   DisGroupDestinationRecord(GenI& aGenI);
   DisGroupDestinationRecord(const DisICTypeRecord& aRecord, GenI& aGenI);
   DisGroupDestinationRecord(const DisGroupDestinationRecord& aSrc);
   ~DisGroupDestinationRecord() = default;

   DisGroupDestinationRecord* Clone() const override;

   // Input / Output
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;
   void      Get(GenI& aGenI) override;

   // Data Validation
   bool IsValid() const override;

   // Getters
   DisUint32                          GetDestinationGroupBitField() const;
   DisUint8                           GetDestinationPriority() const;
   DisDestinationLineStateCommandEnum GetDestinationLineStateCommand() const;

   // Setters
   void SetDestinationGroupBitField(DisUint32 aDestinationGroupBitField);
   void SetDestinationPriority(DisUint8 aDestinationPriority);
   void SetDestinationLineStateCommand(DisDestinationLineStateCommandEnum aDestinationLineStateCommand);

   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI);

   //! Destination Group specifier
   DisUint32 mDestinationGroupBitField = 0;

   //! The priority this record over other messages being transmitted on the same intercom channel
   DisUint8 mDestinationPriority = 0;

   //! The line state command of the destination intercom
   DisEnum8               mDestinationLineStateCommand = DisDestinationLineStateCommandEnum::NONE;
   DisUint16              mPadding                     = 0;
   static const DisUint16 cMIN_LENGTH_OCTETS           = 8;
};
#endif
