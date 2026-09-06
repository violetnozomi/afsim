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

#include "DisAppearance.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisAppearance::DisAppearance()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::Appearance);
   SetProtocolFamily(DisEnum::Pdu::Family::LiveEntity);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisAppearance::DisAppearance(const DisAppearance& aSrc)
   : DisPdu(aSrc)
   , mEntityID(aSrc.mEntityID)
   , mAppearanceFlag1(aSrc.mAppearanceFlag1)
   , mAppearanceFlag2(aSrc.mAppearanceFlag2)
   , mForceID(aSrc.mForceID)
   , mEntityType(aSrc.mEntityType)
   , mAlternateEntityType(aSrc.mAlternateEntityType)
   , mMarkingCharSet(aSrc.mMarkingCharSet)
   , mCapabilities(aSrc.mCapabilities)
{
   for (int i = 0; i < 11; i++)
   {
      mMarkingChars[i] = aSrc.mMarkingChars[i];
   }
   GetLength();
}

DisAppearance::DisAppearance(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisAppearance::DisAppearance(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisAppearance* DisAppearance::Clone() const
{
   return new DisAppearance(*this);
}

std::string DisAppearance::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += DisPdu::ToString();
   returnString += "-Attribute PDU-\n";
   returnString += mEntityID.ToString() + '\n';
   returnString += "Appearance Flag 1: " + std::to_string(static_cast<unsigned int>(mAppearanceFlag1)) + '\n';
   returnString += "Appearance Flag 2: " + std::to_string(static_cast<unsigned int>(mAppearanceFlag2)) + '\n';
   returnString += "Force ID:          " + std::to_string(static_cast<unsigned int>(mForceID)) + '\n';
   returnString += mEntityType.ToString();
   returnString += mEntityType.ToString();
   returnString += "Marking Char Set:  " + std::to_string(static_cast<unsigned int>(mMarkingCharSet)) + '\n';
   returnString += "Marking Characters:"; // no new line ON PURPOSE
   for (int i = 0; i < 11; i++)
   {
      returnString += std::to_string(mMarkingChars[i]);
   }
   returnString += '\n';
   returnString += "Capabilities:      \n";
   returnString += std::to_string(static_cast<unsigned int>(mCapabilities.Value())) + '\n';
   returnString += "Appearance:        \n";
   returnString += "    Visual:        \n";
   returnString += mAppearance.Visual.ToString(mEntityType.GetEntityKind(), mEntityType.GetDomain());
   returnString += "    IR:            \n";
   returnString += mAppearance.IR.ToString(mEntityType.GetEntityKind(), mEntityType.GetDomain());
   returnString += "    EM:            \n";
   returnString += mAppearance.EM.ToString(mEntityType.GetEntityKind(), mEntityType.GetDomain());
   returnString += "    Audio:         \n";
   returnString += mAppearance.Audio.ToString(mEntityType.GetEntityKind(), mEntityType.GetDomain());
   return returnString;
}

bool DisAppearance::IsValid() const
{
   return DisPdu::IsValid() && GetSubprotocolNumber() > 1 && mEntityID.IsValid() && mEntityType.IsValid() &&
          mAlternateEntityType.IsValid();
}

void DisAppearance::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisAppearance::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   DisUint8  mask   = 1;

   if (mask & mAppearanceFlag1) // Force ID
   {
      octets += 1;
   }
   if ((mask << 1) & mAppearanceFlag1) // Entity Type
   {
      octets += 8;
   }
   if ((mask << 2) & mAppearanceFlag1) // Alternate Entity Type
   {
      octets += 8;
   }
   if ((mask << 3) & mAppearanceFlag1) // Entity Marking
   {
      octets += 12;
   }
   if ((mask << 4) & mAppearanceFlag1) // Capabiities
   {
      octets += 4;
   }
   if ((mask << 5) & mAppearanceFlag1) // Appearance-Visual
   {
      octets += 4;
   }
   if ((mask << 6) & mAppearanceFlag1) // Appearance=IR
   {
      octets += 4;
   }

   if ((mask << 7) & mAppearanceFlag1) // Appearance Flag 2 included
   {
      octets += 1;
      if (mask & mAppearanceFlag2) // Appearance-EM
      {
         octets += 4;
      }
      if ((mask << 1) & mAppearanceFlag2) // Appearance-Audio
      {
         octets += 4;
      }
   }

   SetLength(octets);

   return octets;
}

void DisAppearance::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   // Live Entity ID conversion (site and application are 8 bit vs 16)
   mEntityID.Put(aGenO);

   aGenO << mAppearanceFlag1;

   DisUint8 mask = 1;
   // If appearance flag 2 is included
   if ((mask << 7) & mAppearanceFlag1)
   {
      aGenO << mAppearanceFlag2;
   }

   if (mask & mAppearanceFlag1) // Force ID
   {
      aGenO << mForceID;
   }
   if ((mask << 1) & mAppearanceFlag1) // Entity Type
   {
      aGenO << mEntityType;
   }
   if ((mask << 2) & mAppearanceFlag1) // Alternate Entity Type
   {
      aGenO << mAlternateEntityType;
   }
   if ((mask << 3) & mAppearanceFlag1) // Entity Marking
   {
      aGenO << mMarkingCharSet;
      for (const auto& it : mMarkingChars)
      {
         aGenO << it;
      }
   }
   if ((mask << 4) & mAppearanceFlag1) // Capabiities
   {
      // TODO: this value needs to be tested
      aGenO << mCapabilities.Value();
   }
   if ((mask << 5) & mAppearanceFlag1) // Appearance-Visual
   {
      aGenO << mAppearance.Visual;
   }
   if ((mask << 6) & mAppearanceFlag1) // Appearance-IR
   {
      aGenO << mAppearance.IR;
   }

   if ((mask << 7) & mAppearanceFlag1) // Appearance Flag 2 included
   {
      if (mask & mAppearanceFlag2) // Appearance-EM
      {
         aGenO << mAppearance.EM;
      }
      if ((mask << 1) & mAppearanceFlag2) // Appearance-Audio
      {
         aGenO << mAppearance.Audio;
      }
   }
}


void DisAppearance::GetMemberData(GenI& aGenI)
{
   mEntityID.Get(aGenI);

   aGenI >> mAppearanceFlag1;

   DisUint8 mask = 1;
   // If appearance flag 2 is included
   if ((mask << 7) & mAppearanceFlag1)
   {
      aGenI >> mAppearanceFlag2;
   }

   if (mask & mAppearanceFlag1) // Force ID
   {
      aGenI >> mForceID;
   }
   if ((mask << 1) & mAppearanceFlag1) // Entity Type
   {
      aGenI >> mEntityType;
   }
   if ((mask << 2) & mAppearanceFlag1) // Alternate Entity Type
   {
      aGenI >> mAlternateEntityType;
   }
   if ((mask << 3) & mAppearanceFlag1) // Entity Marking
   {
      aGenI >> mMarkingCharSet;
      for (auto& it : mMarkingChars)
      {
         aGenI >> it;
      }
   }
   if ((mask << 4) & mAppearanceFlag1) // Capabiities
   {
      DisUint32 temp = 0;
      aGenI >> temp;
      mCapabilities.Set(temp);
   }
   if ((mask << 5) & mAppearanceFlag1) // Appearance-Visual
   {
      aGenI >> mAppearance.Visual;
   }
   if ((mask << 6) & mAppearanceFlag1) // Appearance-IR
   {
      aGenI >> mAppearance.IR;
   }
   if ((mask << 7) & mAppearanceFlag1) // Appearance Flag 2 included
   {
      if (mask & mAppearanceFlag2) // Appearance-EM
      {
         aGenI >> mAppearance.EM;
      }
      if ((mask << 1) & mAppearanceFlag2) // Appearance-Audio
      {
         aGenI >> mAppearance.Audio;
      }
   }
}
