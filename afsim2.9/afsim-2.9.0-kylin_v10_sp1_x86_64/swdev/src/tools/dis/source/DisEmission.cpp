// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisEmission.hpp"

#include "Dis.hpp"
#include "DisBaseSizes.hpp"
#include "DisEmissionEnums.hpp"
#include "DisPduEnums.hpp"
#include "DisSystem.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEmission::DisEmission()
   : DisPdu()
   , mEmittingEntityId()
   , mEventId()
   , mStateUpdateIndicator(0)
   , mReportedNumberOfSystems(0)
{
   SetPduType(DisEnum::Pdu::Type::Emission);
   SetProtocolFamily(DisEnum::Pdu::Family::DistributedEmissionRegeneration);
   SetLength(cBASE_EMISSION_SIZE);
}

DisEmission::DisEmission(const DisEmission& aSrc)
   : DisPdu(aSrc)
   , mEmittingEntityId(aSrc.mEmittingEntityId)
   , mEventId(aSrc.mEventId)
   , mStateUpdateIndicator(aSrc.mStateUpdateIndicator)
   , mReportedNumberOfSystems(aSrc.mReportedNumberOfSystems)
   , mSystemList(aSrc.mSystemList)
{
}

DisEmission::DisEmission(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisEmission::DisEmission(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisPdu* DisEmission::Clone() const
{
   return new DisEmission(*this);
}

// Virtual
int DisEmission::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Emission;
}

// Virtual
void DisEmission::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisEmission::GetMemberData(aGenI);
}

DisUint16 DisEmission::GetLengthOctets() const
{
   DisUint16 octetCount = cBASE_EMISSION_SIZE;

   std::vector<DisSystem*>::const_iterator iter(mSystemList.GetBegin());
   while (iter != mSystemList.GetEnd())
   {
      octetCount += (*iter++)->GetLengthOctets();
   }
   return octetCount;
}

void DisEmission::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   RemoveAllSystems();

   aGenI >> mEmittingEntityId;
   aGenI >> mEventId;
   aGenI >> mStateUpdateIndicator;
   aGenI >> mReportedNumberOfSystems;
   // Skip 16 bits of padding (2 octets)
   DisUint16 pad16;
   aGenI >> pad16;

   lengthToRead -= cBASE_EMISSION_SIZE;

   mSystemList.Reserve(mReportedNumberOfSystems);
   for (int i = 0; i < mReportedNumberOfSystems; ++i)
   {
      DisSystem* system = new DisSystem();
      aGenI >> *system;
      mSystemList.Add(system); // List takes ownership
      system->SetParentEmission(this);
      lengthToRead -= system->GetLengthRead();
   }

   // Skip any 'extra' data
   ReadExtraData(aGenI, lengthToRead);
}

void DisEmission::Put(GenO& aGenO)
{
   SetLength(GetLengthOctets()); // Ensure the length is current

   DisPdu::Put(aGenO);
   aGenO << mEmittingEntityId;
   aGenO << mEventId;
   aGenO << mStateUpdateIndicator;
   aGenO << GetNumberOfSystems();
   aGenO << DisUint16(0); // 16 bits of padding

   for (auto iter = mSystemList.GetBegin(); iter != mSystemList.GetEnd(); iter++)
   {
      (**iter).Put(aGenO);
   }
}

void DisEmission::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisEmission::ToString() const
{
   using namespace std;
   using namespace DisEnum;

   std::ostringstream ss("");

   ss << "***** Emission PDU *****\n";
   ss << DisPdu::ToString() << '\n'
      << "***** Emission Data *****\n"
      << "Emitting Entity ID:     " << GetEmittingEntityId().ToString() << '\n'
      << "Event ID:               " << GetEventId().ToString() << '\n'
      << "State Update Indicator: " << static_cast<DisUint16>(GetStateUpdateIndicator()) << " ("
      << DisEnum::emission::state_update::ToString(GetStateUpdateIndicator()) << ")\n"
      << "Number Of Systems:      " << static_cast<DisUint16>(GetNumberOfSystems()) << '\n';

   for (auto it = mSystemList.GetBegin(); it != mSystemList.GetEnd(); it++)
   {
      ss << (**it).ToString() << '\n';
   }
   ss << "***** Emission PDU End *****";
   return ss.str();
}

bool DisEmission::CanAddSystem(DisSystem* aSystem) const
{
   // Calculate Size limit
   DisUint32 max_size = Dis::GetMAX_PDU_SIZE_OCTETS();
   max_size -= GetLengthOctets();
   if (!aSystem)
   {
      max_size -= (cBASE_SYSTEM_SIZE + cBASE_BEAM_SIZE);
   }
   else
   {
      max_size -= aSystem->GetLengthOctets();
   }
   return (max_size >= 0);
}
bool DisEmission::AddSystem(DisSystem* aSystem)
{
   if (!CanAddSystem(aSystem))
   {
      return false;
   }
   mSystemList.Add(aSystem);
   SetLength(GetLengthOctets());
   return true;
}

unsigned int DisEmission::RemoveSystemWithNoDelete(DisSystem* aSystem)
{
   bool removed = mSystemList.RemoveWithNoDelete(aSystem);
   if (removed)
   {
      aSystem->SetParentEmission(nullptr);
   }
   return removed;
}

void DisEmission::RemoveAllSystemsWithNoDelete()
{
   std::vector<DisSystem*>::const_iterator iter(mSystemList.GetBegin());
   while (iter != mSystemList.GetEnd())
   {
      (*iter++)->SetParentEmission(nullptr);
   }
   mSystemList.RemoveAllWithNoDelete();
}

bool DisEmission::IsValid() const
{
   // Perform data validation.
   if (!DisPdu::IsValid())
   {
      return false;
   }
   if (!mEmittingEntityId.IsSingle())
   {
      return false;
   }
   if (!mEventId.IsValid())
   {
      return false;
   }
   if (!DisEnum::emission::state_update::IsValid(mStateUpdateIndicator))
   {
      return false;
   }

   std::vector<DisSystem*>::const_iterator iter(mSystemList.GetBegin());
   while (iter != mSystemList.GetEnd())
   {
      if (!(*iter++)->IsValid())
      {
         return false;
      }
   }
   return true;
}
