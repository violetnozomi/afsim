//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "DisArticulatedParts.hpp"

#include <iostream>

#include "DisEntityPart.hpp"
#include "DisPduEnums.hpp"
#include "DisVariableParameterRecord.hpp"
#include "DisVariableParameterRecordEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisArticulatedParts::DisArticulatedParts()
   : DisPdu()
{
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
   SetPduType(DisEnum::Pdu::Type::ArticulatedParts);
   SetProtocolFamily(DisEnum::Pdu::Family::LiveEntity);
}

DisArticulatedParts::DisArticulatedParts(const DisArticulatedParts& aSrc)
   : DisPdu(aSrc)
   , mLiveEntityId(aSrc.mLiveEntityId)
   , mVariableParameterRecordCount(aSrc.mVariableParameterRecordCount)
{
   // clone all variable parameter records
   mVariableParameterRecords.reserve(aSrc.mVariableParameterRecordCount);

   for (const auto& e : aSrc.mVariableParameterRecords)
   {
      mVariableParameterRecords.push_back(std::move(std::unique_ptr<DisVariableParameterRecord>(e->Clone())));
   }

   GetLength(); // Ensure consistent length
}

DisArticulatedParts::DisArticulatedParts(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisArticulatedParts::DisArticulatedParts(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

void DisArticulatedParts::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisArticulatedParts::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   aGenI >> mLiveEntityId;
   aGenI >> mVariableParameterRecordCount;

   if (mVariableParameterRecordCount > 0)
   {
      mVariableParameterRecords.reserve(mVariableParameterRecordCount);
      // add all variable parameter records
      GetRecords(aGenI);
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisArticulatedParts::GetRecords(GenI& aGenI)
{
   for (int i = 0; i < mVariableParameterRecordCount; i++)
   {
      DisVariableParameterRecord record(aGenI);

      if (record.GetTypeDesignator() == static_cast<int>(DisVariableParameterRecordTypes::cATTACHED) ||
          record.GetTypeDesignator() == static_cast<int>(DisVariableParameterRecordTypes::cARTICULATED))
      {
         DisEntityPart temp(record, aGenI);
         mVariableParameterRecords.emplace_back(&temp);
      }
   }
}

void DisArticulatedParts::PutRecords(GenO& aGenO)
{
   for (int i = 0; i < static_cast<int>(mVariableParameterRecords.size()); i++)
   {
      if (mVariableParameterRecords[i].get()->GetTypeDesignator() ==
             static_cast<int>(DisVariableParameterRecordTypes::cATTACHED) ||
          mVariableParameterRecords[i].get()->GetTypeDesignator() ==
             static_cast<int>(DisVariableParameterRecordTypes::cARTICULATED))
      {
         mVariableParameterRecords[i].get()->Put(aGenO);
      }
   }
}

DisUint16 DisArticulatedParts::GetLength()
{
   DisUint16 octets = (DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS + 16 * mVariableParameterRecordCount);
   SetLength(octets);
   return octets;
}

void DisArticulatedParts::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   aGenO << mLiveEntityId;
   aGenO << mVariableParameterRecordCount;

   PutRecords(aGenO);
}

std::string DisArticulatedParts::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "PDU Type = ARTICULATED_PARTS\n";
   returnString += "---------Articulated Parts PDU Information---------\n";
   returnString += "LiveEntityID    = \n" + mLiveEntityId.ToString() + '\n';
   returnString += "   ForceID    = " + std::to_string(static_cast<unsigned int>(mVariableParameterRecordCount)) + '\n';

   for (const auto& it : mVariableParameterRecords)
   {
      returnString += it->ToString();
   }

   returnString += "-------End Articulated Parts PDU Information-------\n";
   return returnString;
}

void DisArticulatedParts::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

bool DisArticulatedParts::IsValid() const
{
   for (auto it = mVariableParameterRecords.begin(); it != mVariableParameterRecords.end(); it++)
   {
      if (!(*it).get()->IsValid())
      {
         return false;
      }
   }
   return DisPdu::IsValid();
}

std::string DisArticulatedParts::GetStringId() const
{
   return "DisArticulatedParts";
}

DisArticulatedParts* DisArticulatedParts::Clone() const
{
   return new DisArticulatedParts(*this);
}

void DisArticulatedParts::AddRecord(DisVariableParameterRecord& aPartPtr)
{
   mVariableParameterRecords.push_back(std::unique_ptr<DisVariableParameterRecord>(aPartPtr.Clone()));
   mVariableParameterRecordCount++;
}

inline void DisArticulatedParts::RemoveAllRecords()
{
   mVariableParameterRecords.clear();
   mVariableParameterRecordCount = 0;
}

void DisArticulatedParts::RemoveRecord(DisVariableParameterRecord* aPartPtr)
{
   for (auto it = mVariableParameterRecords.begin(); it != mVariableParameterRecords.end(); ++it)
   {
      if ((*it).get() == aPartPtr)
      {
         mVariableParameterRecords.erase(it);
         break;
      }
   }
   mVariableParameterRecordCount--;
}
