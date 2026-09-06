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

#include "DisGroupAssignmentRecord.hpp"

#include "DisIntercomCommunicationsParametersEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGroupAssignmentRecord::DisGroupAssignmentRecord()
   : DisICTypeRecord(3, 16) // Values are found in SISO-REF-010-2019 Enumerations v26.pdf
{
}

DisGroupAssignmentRecord::DisGroupAssignmentRecord(GenI& aGenI)
   : DisICTypeRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisGroupAssignmentRecord::DisGroupAssignmentRecord(const DisICTypeRecord& aRecord, GenI& aGenI)
   : DisICTypeRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisGroupAssignmentRecord::DisGroupAssignmentRecord(const DisGroupAssignmentRecord& aSrc)
   : DisICTypeRecord(aSrc)
   , mDestinationGroupBitField(aSrc.mDestinationGroupBitField)
   , mDestinationIntercomReferenceIdentifier(aSrc.mDestinationIntercomReferenceIdentifier)
   , mDestinationIntercomNumber(aSrc.mDestinationIntercomNumber)
   , mDestinationLineId(aSrc.mDestinationLineId)
{
}

DisGroupAssignmentRecord* DisGroupAssignmentRecord::Clone() const
{
   return new DisGroupAssignmentRecord(*this);
}

DisUint16 DisGroupAssignmentRecord::GetLength()
{
   return DisICTypeRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisGroupAssignmentRecord::Put(GenO& aGenO) const
{
   DisICTypeRecord::Put(aGenO);
   aGenO << mDestinationGroupBitField;
   aGenO << mDestinationIntercomReferenceIdentifier;
   aGenO << mDestinationIntercomNumber;
   aGenO << mDestinationLineId;
   aGenO << mPadding1;
   aGenO << mPadding2;
}

void DisGroupAssignmentRecord::Get(GenI& aGenI)
{
   DisICTypeRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisGroupAssignmentRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mDestinationGroupBitField;
   aGenI >> mDestinationIntercomReferenceIdentifier;
   aGenI >> mDestinationIntercomNumber;
   aGenI >> mDestinationLineId;
   aGenI >> mPadding1;
   aGenI >> mPadding2;
}

std::string DisGroupAssignmentRecord::ToString() const
{
   std::string returnString = DisICTypeRecord::ToString();
   returnString += "===== DisEntityDestinationRecord =====\n";
   returnString += "Destination IntercomReference Identifier: " + mDestinationIntercomReferenceIdentifier.ToString() + '\n';
   returnString += "Destination IntercomNumber:               " + std::to_string(mDestinationIntercomNumber) + '\n';
   returnString += "Destination Line Id:                      " + std::to_string(mDestinationLineId) + '\n';
   returnString += "Destination Group Bit Field:              " + std::to_string(mDestinationGroupBitField) + '\n';
   returnString += "=== End DisEntityDestinationRecord ===\n";
   return returnString;
}

bool DisGroupAssignmentRecord::IsValid() const
{
   return mDestinationIntercomReferenceIdentifier.IsValid();
}

DisUint32 DisGroupAssignmentRecord::GetDestinationGroupBitField() const
{
   return mDestinationGroupBitField;
}

DisEntityId DisGroupAssignmentRecord::GetDestinationIntercomReferenceIdentifier() const
{
   return mDestinationIntercomReferenceIdentifier;
}

DisUint16 DisGroupAssignmentRecord::GetDestinationIntercomNumber() const
{
   return mDestinationIntercomNumber;
}

DisUint8 DisGroupAssignmentRecord::GetDestinationLineId() const
{
   return mDestinationLineId;
}

void DisGroupAssignmentRecord::SetDestinationGroupBitField(DisUint32 aDestinationGroupBitField)
{
   mDestinationGroupBitField = aDestinationGroupBitField;
}

void DisGroupAssignmentRecord::SetDestinationIntercomReferenceIdentifier(const DisEntityId& aDestinationIntercomReferenceIdentifier)
{
   mDestinationIntercomReferenceIdentifier = aDestinationIntercomReferenceIdentifier;
}

void DisGroupAssignmentRecord::SetDestinationIntercomNumber(DisUint16 aDestinationIntercomNumber)
{
   mDestinationIntercomNumber = aDestinationIntercomNumber;
}

void DisGroupAssignmentRecord::SetDestinationLineId(DisUint8 aDestinationLineId)
{
   mDestinationLineId = aDestinationLineId;
}
