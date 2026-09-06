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

#include "DisGroupEntityDescriptionRecord.hpp"

#include <iostream>
#include <ostream>

#include "Dis.hpp"
#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGroupedEntityDescriptionRecord::DisGroupedEntityDescriptionRecord()
   : mEntityId()
   , mEntityAppearance()
{
   for (int i = 0; i < 3; i++)
   {
      mEntityLocation[i]    = 0;
      mEntityOrientation[i] = 0;
   }
}

DisGroupedEntityDescriptionRecord::DisGroupedEntityDescriptionRecord(const DisGroupedEntityDescriptionRecord& aDisRecord)
   : mEntityId(aDisRecord.mEntityId)
   , mEntityAppearance(aDisRecord.mEntityAppearance)
{
   for (int i = 0; i < 3; i++)
   {
      mEntityLocation[i]    = aDisRecord.mEntityLocation[i];
      mEntityOrientation[i] = aDisRecord.mEntityOrientation[i];
   }
}

DisGroupedEntityDescriptionRecord::DisGroupedEntityDescriptionRecord(GenI& aGenI)
   : mEntityId()
   , mEntityAppearance()
{
   GetMemberData(aGenI);
}

void DisGroupedEntityDescriptionRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisGroupedEntityDescriptionRecord::GetLength()
{
   return cRECORD_MIN_LENGTH_OCTETS;
}

void DisGroupedEntityDescriptionRecord::Put(GenO& aGenO) const
{
   aGenO << mEntityId;
   aGenO << mEntityLocation[0] << mEntityLocation[1] << mEntityLocation[2];
   mEntityAppearance.Put(aGenO);
   aGenO << mEntityOrientation[0] << mEntityOrientation[1] << mEntityOrientation[2];
}

bool DisGroupedEntityDescriptionRecord::IsValid() const
{
   return true;
}

void DisGroupedEntityDescriptionRecord::HandleReadError()
{
   if (Dis::ShowError(Dis::cIO_ERRORS))
   {
      std::cerr << " Length=" << cRECORD_MIN_LENGTH_OCTETS;
      std::cerr << " Type=" << static_cast<unsigned int>(DisGroupEntityCategoryEnum::cUNDEFINED);
      std::cerr << std::endl;
   }
}

DisGroupedEntityDescriptionRecord* DisGroupedEntityDescriptionRecord::Clone() const
{
   return new DisGroupedEntityDescriptionRecord(*this);
}

void DisGroupedEntityDescriptionRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << std::endl;
}

std::string DisGroupedEntityDescriptionRecord::ToString() const
{
   std::string returnString = "Entity Identifier: " + std::to_string(mEntityId) + '\n';
   returnString += "Entity Location x: " + std::to_string(mEntityLocation[0]) + '\n';
   returnString += "Entity Location y: " + std::to_string(mEntityLocation[1]) + '\n';
   returnString += "Entity Location z: " + std::to_string(mEntityLocation[2]) + '\n';
   returnString += "Entity Appearance: " + std::to_string(mEntityAppearance.Value()) + '\n';
   returnString += "Entity Orientation x: " + std::to_string(mEntityOrientation[0]) + '\n';
   returnString += "Entity Orientation y: " + std::to_string(mEntityOrientation[1]) + '\n';
   returnString += "Entity Orientation z: " + std::to_string(mEntityOrientation[2]) + '\n';
   return returnString;
}

DisUint16 DisGroupedEntityDescriptionRecord::GetEntityIdentifier() const
{
   return mEntityId;
}

void DisGroupedEntityDescriptionRecord::GetEntityLocation(DisInt16& aEntityLocationX,
                                                          DisInt16& aEntityLocationY,
                                                          DisInt16& aEntityLocationZ) const
{
   aEntityLocationX = mEntityLocation[0];
   aEntityLocationY = mEntityLocation[1];
   aEntityLocationZ = mEntityLocation[2];
}

DisUint32 DisGroupedEntityDescriptionRecord::GetEntityAppearance() const
{
   return mEntityAppearance.Value();
}

void DisGroupedEntityDescriptionRecord::GetEntityOrientation(DisInt8& aDisEntityOrienationX,
                                                             DisInt8& aDisEntityOrienationY,
                                                             DisInt8& aDisEntityOrienationZ) const
{
   aDisEntityOrienationX = mEntityOrientation[0];
   aDisEntityOrienationY = mEntityOrientation[1];
   aDisEntityOrienationZ = mEntityOrientation[2];
}

void DisGroupedEntityDescriptionRecord::SetEntityIdentifier(const DisUint16 aEntityId)
{
   mEntityId = aEntityId;
}

void DisGroupedEntityDescriptionRecord::SetEntityLocation(DisInt16 aEntityLocationX,
                                                          DisInt16 aEntityLocationY,
                                                          DisInt16 aEntityLocationZ)
{
   mEntityLocation[0] = aEntityLocationX;
   mEntityLocation[1] = aEntityLocationY;
   mEntityLocation[2] = aEntityLocationZ;
}

void DisGroupedEntityDescriptionRecord::SetEntityAppearance(const DisUint32 aEntityAppearance)
{
   mEntityAppearance = aEntityAppearance;
}

void DisGroupedEntityDescriptionRecord::SetEntityOrientation(DisInt8 aDisEntityOrienationX,
                                                             DisInt8 aDisEntityOrienationY,
                                                             DisInt8 aDisEntityOrienationZ) const
{
   mEntityOrientation[0] = aDisEntityOrienationX;
   mEntityOrientation[1] = aDisEntityOrienationY;
   mEntityOrientation[2] = aDisEntityOrienationZ;
}

void DisGroupedEntityDescriptionRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mEntityId;
   aGenI >> mEntityLocation[0] >> mEntityLocation[1] >> mEntityLocation[2];
   mEntityAppearance.Get(aGenI);
   aGenI >> mEntityOrientation[0] >> mEntityOrientation[1] >> mEntityOrientation[2];
}

// Operators
bool DisGroupedEntityDescriptionRecord::operator==(const DisGroupedEntityDescriptionRecord& aRhs) const
{
   return ((mEntityId == aRhs.mEntityId) && (mEntityLocation[0] == aRhs.mEntityLocation[0]) &&
           (mEntityLocation[1] == aRhs.mEntityLocation[1]) && (mEntityLocation[2] == aRhs.mEntityLocation[2]) &&
           (mEntityAppearance == aRhs.mEntityAppearance) && (mEntityOrientation[0] == aRhs.mEntityOrientation[0]) &&
           (mEntityOrientation[1] == aRhs.mEntityOrientation[1]) && (mEntityOrientation[2] == aRhs.mEntityOrientation[2]));
}
