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

#include "DisSpecificDestinationRecord.hpp"

#include "DisEntityId.hpp"
#include "DisIntercomCommunicationsParametersEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisSpecificDestinationRecord::DisSpecificDestinationRecord()
   : DisICTypeRecord(1, 12) // Values are found in SISO-REF-010-2019 Enumerations v26.pdf
{
}

DisSpecificDestinationRecord::DisSpecificDestinationRecord(GenI& aGenI)
   : DisICTypeRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisSpecificDestinationRecord::DisSpecificDestinationRecord(const DisICTypeRecord& aRecord, GenI& aGenI)
   : DisICTypeRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisSpecificDestinationRecord::DisSpecificDestinationRecord(const DisSpecificDestinationRecord& aSrc)
   : DisICTypeRecord(aSrc)
   , mDestinationIntercomReferenceIdentifier(aSrc.mDestinationIntercomReferenceIdentifier)
   , mDestinationIntercomNumber(aSrc.mDestinationIntercomNumber)
   , mDestinationLineId(aSrc.mDestinationLineId)
   , mDestinationPriority(aSrc.mDestinationPriority)
   , mDestinationLineStateCommand(aSrc.mDestinationLineStateCommand)
{
}

DisSpecificDestinationRecord* DisSpecificDestinationRecord::Clone() const
{
   return new DisSpecificDestinationRecord(*this);
}

void DisSpecificDestinationRecord::Put(GenO& aGenO) const
{
   DisICTypeRecord::Put(aGenO);
   mDestinationIntercomReferenceIdentifier.Put(aGenO);
   aGenO << mDestinationIntercomNumber;
   aGenO << mDestinationLineId;
   aGenO << mDestinationPriority;
   aGenO << mDestinationLineStateCommand;
   aGenO << mPadding;
}

DisUint16 DisSpecificDestinationRecord::GetLength()
{
   return DisICTypeRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisSpecificDestinationRecord::Get(GenI& aGenI)
{
   DisICTypeRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisSpecificDestinationRecord::GetMemberData(GenI& aGenI)
{
   mDestinationIntercomReferenceIdentifier.Get(aGenI);
   aGenI >> mDestinationIntercomNumber;
   aGenI >> mDestinationLineId;
   aGenI >> mDestinationPriority;
   aGenI >> mDestinationLineStateCommand;
   aGenI >> mPadding;
}

std::string DisSpecificDestinationRecord::ToString() const
{
   std::string returnString = DisICTypeRecord::ToString();
   returnString += "===== DisEntityDestinationRecord =====\n";
   returnString += "Destination IntercomReference Identifier: " + mDestinationIntercomReferenceIdentifier.ToString() + '\n';
   returnString += "Destination IntercomNumber:               " + std::to_string(mDestinationIntercomNumber) + '\n';
   returnString += "Destination Line Id:                      " + std::to_string(mDestinationLineId) + '\n';
   returnString += "Destination Priority:                     " + std::to_string(mDestinationPriority) + '\n';
   returnString += "Destination LineState Command:            " + std::to_string(mDestinationLineStateCommand) + '\n';
   returnString += "=== End DisEntityDestinationRecord ===\n";
   return returnString;
}

bool DisSpecificDestinationRecord::IsValid() const
{
   return DisICTypeRecord::IsValid() && mDestinationIntercomReferenceIdentifier.IsValid();
}

DisEntityId DisSpecificDestinationRecord::GetDestinationIntercomReferenceIdentifier() const
{
   return mDestinationIntercomReferenceIdentifier;
}

DisUint16 DisSpecificDestinationRecord::GetDestinationIntercomNumber() const
{
   return mDestinationIntercomNumber;
}

DisUint8 DisSpecificDestinationRecord::GetDestinationLineId() const
{
   return mDestinationLineId;
}

DisUint8 DisSpecificDestinationRecord::GetDestinationPriority() const
{
   return mDestinationPriority;
}

DisDestinationLineStateCommandEnum DisSpecificDestinationRecord::GetDestinationLineStateCommand() const
{
   int enumVal = static_cast<int>(mDestinationLineStateCommand);
   return static_cast<DisDestinationLineStateCommandEnum>(enumVal);
}

void DisSpecificDestinationRecord::SetDestinationIntercomReferenceIdentifier(const DisEntityId& aDestinationIntercomReferenceIdentifier)
{
   mDestinationIntercomReferenceIdentifier = aDestinationIntercomReferenceIdentifier;
}

void DisSpecificDestinationRecord::SetDestinationIntercomNumber(DisUint16 aDestinationIntercomNumber)
{
   mDestinationIntercomNumber = aDestinationIntercomNumber;
}

void DisSpecificDestinationRecord::SetDestinationLineId(DisUint8 aDestinationLineId)
{
   mDestinationLineId = aDestinationLineId;
}

void DisSpecificDestinationRecord::SetDestinationPriority(DisUint8 aDestinationPriority)
{
   mDestinationPriority = aDestinationPriority;
}

void DisSpecificDestinationRecord::SetDestinationLineStateCommand(DisDestinationLineStateCommandEnum aDestinationLineStateCommand)
{
   mDestinationLineStateCommand = static_cast<DisEnum8>(aDestinationLineStateCommand);
}
