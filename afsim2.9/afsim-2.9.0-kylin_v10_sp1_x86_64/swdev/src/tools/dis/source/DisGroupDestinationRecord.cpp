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

#include "DisGroupDestinationRecord.hpp"

#include <cassert>

#include "DisIntercomCommunicationsParametersEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGroupDestinationRecord::DisGroupDestinationRecord()
   : DisICTypeRecord(2, 8) // Values are found in SISO-REF-010-2019 Enumerations v26.pdf
{
}

DisGroupDestinationRecord::DisGroupDestinationRecord(GenI& aGenI)
   : DisICTypeRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisGroupDestinationRecord::DisGroupDestinationRecord(const DisICTypeRecord& aRecord, GenI& aGenI)
   : DisICTypeRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisGroupDestinationRecord::DisGroupDestinationRecord(const DisGroupDestinationRecord& aSrc)
   : DisICTypeRecord(aSrc)
   , mDestinationGroupBitField(aSrc.mDestinationGroupBitField)
   , mDestinationPriority(aSrc.mDestinationPriority)
   , mDestinationLineStateCommand(aSrc.mDestinationLineStateCommand)
{
}

DisGroupDestinationRecord* DisGroupDestinationRecord::Clone() const
{
   return new DisGroupDestinationRecord(*this);
}

DisUint16 DisGroupDestinationRecord::GetLength()
{
   return DisICTypeRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisGroupDestinationRecord::Put(GenO& aGenO) const
{
   DisICTypeRecord::Put(aGenO);
   aGenO << mDestinationGroupBitField;
   aGenO << mDestinationPriority;
   aGenO << mDestinationLineStateCommand;
   aGenO << mPadding;
}

void DisGroupDestinationRecord::Get(GenI& aGenI)
{
   DisICTypeRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisGroupDestinationRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mDestinationGroupBitField;
   aGenI >> mDestinationPriority;
   aGenI >> mDestinationLineStateCommand;
   aGenI >> mPadding;
}

std::string DisGroupDestinationRecord::ToString() const
{
   std::string returnString = DisICTypeRecord::ToString();
   returnString += "===== DisEntityDestinationRecord =====\n";
   returnString += "Destination Priority:          " + std::to_string(mDestinationPriority) + '\n';
   returnString += "Destination Group Bit Field:   " + std::to_string(mDestinationGroupBitField) + '\n';
   returnString += "Destination LineState Command: " + std::to_string(mDestinationLineStateCommand) + '\n';
   returnString += "=== End DisEntityDestinationRecord ===\n";
   return returnString;
}

bool DisGroupDestinationRecord::IsValid() const
{
   return true;
}

DisUint32 DisGroupDestinationRecord::GetDestinationGroupBitField() const
{
   return mDestinationGroupBitField;
}

DisUint8 DisGroupDestinationRecord::GetDestinationPriority() const
{
   return mDestinationPriority;
}

DisDestinationLineStateCommandEnum DisGroupDestinationRecord::GetDestinationLineStateCommand() const
{
   int enumVal = static_cast<int>(mDestinationLineStateCommand);
   return static_cast<DisDestinationLineStateCommandEnum>(enumVal);
}

void DisGroupDestinationRecord::SetDestinationGroupBitField(DisUint32 aDestinationGroupBitField)
{
   mDestinationGroupBitField = aDestinationGroupBitField;
}

void DisGroupDestinationRecord::SetDestinationPriority(DisUint8 aDestinationPriority)
{
   mDestinationPriority = aDestinationPriority;
}

void DisGroupDestinationRecord::SetDestinationLineStateCommand(DisDestinationLineStateCommandEnum aDestinationLineStateCommand)
{
   mDestinationLineStateCommand = static_cast<DisEnum8>(aDestinationLineStateCommand);
}
