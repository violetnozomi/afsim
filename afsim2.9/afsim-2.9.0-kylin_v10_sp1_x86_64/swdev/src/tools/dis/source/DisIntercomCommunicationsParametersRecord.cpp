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

#include "DisIntercomCommunicationsParametersRecord.hpp"

#include <cassert>

#include "DisGroupAssignmentRecord.hpp"
#include "DisGroupDestinationRecord.hpp"
#include "DisSpecificDestinationRecord.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisIntercomCommunicationsParametersRecord::DisIntercomCommunicationsParametersRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisIntercomCommunicationsParametersRecord::DisIntercomCommunicationsParametersRecord(
   const DisIntercomCommunicationsParametersRecord& aSrc)
   : mRecordSpecificField(aSrc.mRecordSpecificField)
{
}

void DisIntercomCommunicationsParametersRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisIntercomCommunicationsParametersRecord::GetMemberData(GenI& aGenI)
{
   mRecordSpecificField.Get(aGenI);
}

void DisIntercomCommunicationsParametersRecord::Put(GenO& aGenO) const
{
   mRecordSpecificField.Put(aGenO);
}

DisUint16 DisIntercomCommunicationsParametersRecord::GetLength()
{
   return mRecordSpecificField.GetLength();
}

const DisICTypeRecord& DisIntercomCommunicationsParametersRecord::GetRecordSpecificField() const
{
   return mRecordSpecificField;
}

void DisIntercomCommunicationsParametersRecord::SetRecordSpecificField(const DisICTypeRecord& aRecordSpecificField)
{
   mRecordSpecificField = aRecordSpecificField;
}

bool DisIntercomCommunicationsParametersRecord::IsValid() const
{
   return mRecordSpecificField.IsValid();
}

std::string DisIntercomCommunicationsParametersRecord::ToString() const
{
   std::string returnString = "===== DisIntercomCommunicationsParametersRecord =====\n";
   returnString += mRecordSpecificField.ToString();
   returnString += "=== End DisIntercomCommunicationsParametersRecord ===\n";
   return returnString;
}

void DisIntercomCommunicationsParametersRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}
