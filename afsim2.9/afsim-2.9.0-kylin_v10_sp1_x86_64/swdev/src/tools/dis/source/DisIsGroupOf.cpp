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

#include "DisIsGroupOf.hpp"

#include "DisBasicFixedWingAircraftRecord.hpp"
#include "DisBasicGroundCombatVehicleRecord.hpp"
#include "DisBasicGroundSoldierRecord.hpp"
#include "DisBasicRotorWingAircraftRecord.hpp"
#include "DisEnhancedFixedWingAircraftRecord.hpp"
#include "DisEnhancedGroundCombatVehicleRecord.hpp"
#include "DisEnhancedGroundSoldierRecord.hpp"
#include "DisEnhancedRotorWingAircraftRecord.hpp"
#include "DisGroundLogisticsVehicleRecord.hpp"
#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisIsGroupOf::DisIsGroupOf()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::IsGroupOf);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);
   for (int i = 0; i < 2; i++)
   {
      mGroupLocation[i] = 0;
   }
}

DisIsGroupOf::DisIsGroupOf(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   SetPduType(DisEnum::Pdu::Type::IsGroupOf);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);
   GetMemberData(aGenI);
}

DisIsGroupOf::DisIsGroupOf(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisIsGroupOf::DisIsGroupOf(const DisIsGroupOf& aSrc)
   : DisPdu(aSrc)
   , mGroupId(aSrc.mGroupId)
   , mGroupedEntityCategory(aSrc.mGroupedEntityCategory)
   , mNumberOfGroupedEntities(aSrc.mNumberOfGroupedEntities)
   , mGroupEntityDescription(aSrc.mGroupEntityDescription)
{
   SetPduType(DisEnum::Pdu::Type::IsGroupOf);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);
   for (int i = 0; i < 2; i++)
   {
      mGroupLocation[i] = aSrc.mGroupLocation[i];
   }
   GetLength();
}

DisIsGroupOf* DisIsGroupOf::Clone() const
{
   return new DisIsGroupOf(*this);
}

int DisIsGroupOf::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::IsGroupOf);
}

int DisIsGroupOf::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::EntityManagement);
}

void DisIsGroupOf::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIsGroupOf::GetLength()
{
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS + GetSpecificRecordLength();
   SetLength(length);
   return length;
}

void DisIsGroupOf::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mGroupId.Put(aGenO);
   aGenO << mGroupedEntityCategory;
   aGenO << mNumberOfGroupedEntities;
   aGenO << mPadding;
   aGenO << mGroupLocation[0] << mGroupLocation[1];
   PutSpecificRecord(aGenO);
}

const DisEntityId& DisIsGroupOf::GetGroupId() const
{
   return mGroupId;
}

DisEnum8 DisIsGroupOf::GetGroupedEntityCategory() const
{
   return mGroupedEntityCategory;
}

DisUint8 DisIsGroupOf::GetNumberOfGroupedEntities() const
{
   return mNumberOfGroupedEntities;
}

void DisIsGroupOf::GetGroupLocation(DisFloat64& aLatitude, DisFloat64& aLongitude) const
{
   aLatitude  = mGroupLocation[0];
   aLongitude = mGroupLocation[1];
}

const DisGroupedEntityDescriptionRecord* DisIsGroupOf::GetGroupEntityDescriptionAtIndex(DisUint16 aIndex) const
{
   const DisGroupedEntityDescriptionRecord* recordPtr{nullptr};
   if (aIndex >= 0 && aIndex < static_cast<int>(mGroupEntityDescription.GetSize()))
   {
      return mGroupEntityDescription[aIndex];
   }

   return recordPtr;
}

void DisIsGroupOf::SetGroupId(const DisEntityId& aGroupId)
{
   mGroupId = aGroupId;
}

void DisIsGroupOf::SetGroupedEntityCategory(DisGroupEntityCategoryEnum aCategory)
{
   mGroupedEntityCategory = static_cast<DisEnum8>(aCategory);
}

void DisIsGroupOf::SetNumberOfGroupedEntities(DisUint8 aNumberofGroupedEntities)
{
   mNumberOfGroupedEntities = aNumberofGroupedEntities;
}

void DisIsGroupOf::SetGroupLocation(DisFloat64 aLatitude, DisFloat64 aLongitude)
{
   mGroupLocation[0] = aLatitude;
   mGroupLocation[1] = aLongitude;
}

bool DisIsGroupOf::IsValid() const
{
   return DisPdu::IsValid() && ValidateScalarArray(mGroupLocation, 3) && ValidateSpecificRecord();
}

std::string DisIsGroupOf::GetStringId() const
{
   return "DisIsGroupOf";
}

std::string DisIsGroupOf::ToString() const
{
   std::string returnString = mGroupId.ToString() + '\n';
   returnString += "Grouped Entity Category: " + std::to_string(mGroupedEntityCategory) + '\n';
   returnString += "Number of grouped entities: " + std::to_string(mNumberOfGroupedEntities) + '\n';
   returnString += "   Latitude: " + std::to_string(mGroupLocation[0]) + '\n';
   returnString += "   Longitude: " + std::to_string(mGroupLocation[1]) + '\n';
   returnString += "Associated Group Entity Description Records\n";
   returnString += SpecificRecordToString();
   returnString += "End Associated Group Entity Description Records\n";
   return returnString;
}

void DisIsGroupOf::AddGroupEntityDescription(DisGroupedEntityDescriptionRecord* aParameter)
{
   mGroupEntityDescription.Add(aParameter);
}

void DisIsGroupOf::RemoveGroupEntityDescription(DisGroupedEntityDescriptionRecord* aParameter)
{
   mGroupEntityDescription.Remove(aParameter);
}

void DisIsGroupOf::RemoveAllGroupEntityDescriptions()
{
   mGroupEntityDescription.RemoveAll();
}

void DisIsGroupOf::RemoveGroupEntityDescriptionNoDelete(DisGroupedEntityDescriptionRecord* aParameter)
{
   mGroupEntityDescription.RemoveWithNoDelete(aParameter);
}

void DisIsGroupOf::RemoveAllGroupEntityDescriptionsNoDelete()
{
   mGroupEntityDescription.RemoveAllWithNoDelete();
}

std::vector<DisGroupedEntityDescriptionRecord*>::iterator DisIsGroupOf::GetGroupEntityDescriptionRecordsBegin()
{
   return mGroupEntityDescription.GetBegin();
}

std::vector<DisGroupedEntityDescriptionRecord*>::const_iterator DisIsGroupOf::GetGroupEntityDescriptionRecordsBegin() const
{
   return mGroupEntityDescription.GetBegin();
}

std::vector<DisGroupedEntityDescriptionRecord*>::iterator DisIsGroupOf::GetGroupEntityDescriptionRecordsEnd()
{
   return mGroupEntityDescription.GetEnd();
}

std::vector<DisGroupedEntityDescriptionRecord*>::const_iterator DisIsGroupOf::GetGroupEntityDescriptionRecordsEnd() const
{
   return mGroupEntityDescription.GetEnd();
}

void DisIsGroupOf::GetMemberData(GenI& aGenI)
{
   aGenI >> mGroupId;
   aGenI >> mGroupedEntityCategory;
   aGenI >> mNumberOfGroupedEntities;
   aGenI >> mPadding;
   aGenI >> mGroupLocation[0] >> mGroupLocation[1];
   GetSpecificRecord(aGenI);
   GetLength();
}

void DisIsGroupOf::PutSpecificRecord(GenO& aGenO)
{
   for (auto it = mGroupEntityDescription.GetBegin(); it != mGroupEntityDescription.GetEnd(); it++)
   {
      (**it).Put(aGenO);
   }
}

void DisIsGroupOf::GetSpecificRecord(GenI& aGenI)
{
   for (int i = 0; i < mNumberOfGroupedEntities; i++)
   {
      switch (mGroupedEntityCategory)
      {
      case DisGroupEntityCategoryEnum::cBASIC_GROUND_COMBAT_VEHICLE:
      {
         DisBasicGroundCombatVehicleRecord* temp = new DisBasicGroundCombatVehicleRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cENHANCED_GROUND_COMBAT_VEHICLE:
      {
         DisEnhancedGroundCombatVehicleRecord* temp = new DisEnhancedGroundCombatVehicleRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cBASIC_GROUND_COMBAT_SOLDIER:
      {
         DisBasicGroundSoldierRecord* temp = new DisBasicGroundSoldierRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cENHANCED_GROUND_COMBAT_SOLDIER:
      {
         DisEnhancedGroundSoldierRecord* temp = new DisEnhancedGroundSoldierRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cBASIC_ROTOR_WING_AIRCRAFT:
      {
         DisBasicRotorWingAircraftRecord* temp = new DisBasicRotorWingAircraftRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cENHANCED_ROTOR_WING_AIRCRAFT:
      {
         DisEnhancedRotorWingAircraftRecord* temp = new DisEnhancedRotorWingAircraftRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cBASIC_FIXED_WING_AIRCRAFT:
      {
         DisBasicFixedWingAircraftRecord* temp = new DisBasicFixedWingAircraftRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cENHANCED_FIXED_WING_AIRCRAFT:
      {
         DisEnhancedFixedWingAircraftRecord* temp = new DisEnhancedFixedWingAircraftRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      case DisGroupEntityCategoryEnum::cGROUND_LOGISTICS_VEHICLE:
      {
         DisGroundLogisticsVehicleRecord* temp = new DisGroundLogisticsVehicleRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      default:
      {
         DisGroupedEntityDescriptionRecord* temp = new DisGroupedEntityDescriptionRecord(aGenI);
         mGroupEntityDescription.Add(temp);
         break;
      }
      }
   }
}

DisUint16 DisIsGroupOf::GetSpecificRecordLength()
{
   DisUint16 length = 0;

   for (auto it = mGroupEntityDescription.GetBegin(); it != mGroupEntityDescription.GetEnd(); it++)
   {
      length += (*it)->GetLength();
   }

   return length;
}

std::string DisIsGroupOf::SpecificRecordToString() const
{
   std::string returnString;

   for (auto it = mGroupEntityDescription.GetBegin(); it != mGroupEntityDescription.GetEnd(); it++)
   {
      returnString += (*it)->ToString() + '\n';
   }

   return returnString;
}

bool DisIsGroupOf::ValidateSpecificRecord() const
{
   bool isValid = true;

   for (auto it = mGroupEntityDescription.GetBegin(); it != mGroupEntityDescription.GetEnd(); it++)
   {
      isValid &= (*it)->IsValid();
   }

   return isValid;
}
