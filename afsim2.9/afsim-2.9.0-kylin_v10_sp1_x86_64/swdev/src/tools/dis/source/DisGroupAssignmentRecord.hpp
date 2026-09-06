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

#ifndef DISGROUPASSIGNMENTRECORD_HPP
#define DISGROUPASSIGNMENTRECORD_HPP

#include "dis_export.h"

#include "DisDestinationLineStateCommandEnum.hpp"
#include "DisEntityId.hpp"
#include "DisICTypeRecord.hpp"

//! This record shall be used to assign a group bit field to a specific intercom.
class DIS_EXPORT DisGroupAssignmentRecord : public DisICTypeRecord
{
public:
   DisGroupAssignmentRecord();
   DisGroupAssignmentRecord(GenI& aGenI);
   DisGroupAssignmentRecord(const DisICTypeRecord& aRecord, GenI& aGenI);
   DisGroupAssignmentRecord(const DisGroupAssignmentRecord& aSrc);
   ~DisGroupAssignmentRecord() = default;

   DisGroupAssignmentRecord* Clone() const override;

   // Input / Output
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;
   void      Get(GenI& aGenI) override;

   // Data Validation
   bool IsValid() const override;

   // Getters
   DisUint32   GetDestinationGroupBitField() const;
   DisEntityId GetDestinationIntercomReferenceIdentifier() const;
   DisUint16   GetDestinationIntercomNumber() const;
   DisUint8    GetDestinationLineId() const;

   // Setters
   void SetDestinationGroupBitField(DisUint32 aDestinationGroupBitField);
   void SetDestinationIntercomReferenceIdentifier(const DisEntityId& aDestinationIntercomReferenceIdentifier);
   void SetDestinationIntercomNumber(DisUint16 aDestinationIntercomNumber);
   void SetDestinationLineId(DisUint8 aDestinationLineId);

   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI);
   //! Destination Group specifier
   DisUint32 mDestinationGroupBitField = 0;
   //! The reference identifer of the destination intercom
   DisEntityId mDestinationIntercomReferenceIdentifier = DisEntityId::ENTITY_ID_UNKNOWN;
   //! The Number id of the destination intercom
   DisUint16 mDestinationIntercomNumber = 0;
   //! The line id of the destination intercom
   DisUint8               mDestinationLineId = 0;
   DisUint8               mPadding1          = 0;
   DisUint16              mPadding2          = 0;
   static const DisUint16 cMIN_LENGTH_OCTETS = 16;
};
#endif
