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

#include "DisEntityId.hpp"

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisUint16         DisEntityId::mNextEntity = 0;
const DisEntityId DisEntityId::D_SPOT_NO_ENTITY(Dis::cD_SPOT_NO_ENTITY);
const DisEntityId DisEntityId::ENTITY_ID_UNKNOWN(Dis::cENTITY_ID_UNKNOWN);
const DisEntityId DisEntityId::NO_SPECIFIC_ENTITY(Dis::cNO_SPECIFIC_ENTITY);
const DisEntityId DisEntityId::TARGET_ID_UNKNOWN(Dis::cTARGET_ID_UNKNOWN);
const DisEntityId DisEntityId::MUNITION_NOT_TRACKED(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);
const DisEntityId DisEntityId::NO_ENTITY_IMPACTED(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);
const DisEntityId DisEntityId::NO_LOCATION(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);

DisEntityId::DisEntityId()
   : mSite(Dis::cNO_SITE)
   , mApplication(Dis::cNO_APPLIC)
   , mEntity(Dis::cNO_ENTITY)
{
}

DisEntityId::DisEntityId(const DisEntityId& aSrc)
   : mSite(aSrc.mSite)
   , mApplication(aSrc.mApplication)
   , mEntity(aSrc.mEntity)
{
}

DisEntityId::DisEntityId(DisUint16 aSite, DisUint16 aApplication, DisUint16 aEntity)
   : mSite(aSite)
   , mApplication(aApplication)
   , mEntity(aEntity)
{
}

const char* DisEntityId::GetScriptClassName() const
{
   return "DisEntityId";
}

// Set the next entity number to be assigned by 'AssignNewEntity()'.
// static
void DisEntityId::SetNextEntity(DisUint16 aNextEntity)
{
   mNextEntity = aNextEntity;
}

// Assigns the next sequential entity number.  Site and Application
// are not modified.
void DisEntityId::AssignNewEntity()
{
   while ((mNextEntity == Dis::cNO_ENTITY) || (mNextEntity == Dis::cALL_ENTITIES) || (mNextEntity == Dis::cRQST_ASSIGN_ID))
   {
      ++mNextEntity;
   }
   mEntity = mNextEntity;
   ++mNextEntity;
}

bool DisEntityId::IsDefined() const
{
   // A entity value of ASSIGN_ENTITY is reserved for creation PDUs to alert the
   // receiving simulation to assign an Entity ID to the entity it creates.
   return (mSite != Dis::cNO_SITE && mApplication != Dis::cNO_APPLIC && mEntity != Dis::cNO_ENTITY &&
           mEntity != Dis::cRQST_ASSIGN_ID);
}

bool DisEntityId::IsSingle() const
{
   return (IsDefined() && mSite != Dis::cALL_SITES && mApplication != Dis::cALL_APPLIC && mEntity != Dis::cALL_ENTITIES);
}

DisEntityId* DisEntityId::Clone() const
{
   return new DisEntityId(*this);
}

void DisEntityId::Get(GenI& aGenI)
{
   aGenI >> mSite;
   aGenI >> mApplication;
   aGenI >> mEntity;
}

void DisEntityId::Put(GenO& aGenO) const
{
   aGenO << mSite;
   aGenO << mApplication;
   aGenO << mEntity;
}

DisEntityId& DisEntityId::operator=(const DisEntityId& aRhs)
{
   if (this != &aRhs)
   {
      mSite        = aRhs.mSite;
      mApplication = aRhs.mApplication;
      mEntity      = aRhs.mEntity;
   }
   return *this;
}

bool DisEntityId::operator<(const DisEntityId& aRhs) const
{
   bool isLess = true;

   if (mSite > aRhs.mSite)
   {
      isLess = false;
   }
   else if (mSite == aRhs.mSite)
   {
      if (mApplication > aRhs.mApplication)
      {
         isLess = false;
      }
      else if (mApplication == aRhs.mApplication)
      {
         if (mEntity >= aRhs.mEntity)
         {
            isLess = false;
         }
      }
   }
   return isLess;
}

std::string DisEntityId::ToString() const
{
   return std::to_string(mSite) + ':' + std::to_string(mApplication) + ':' + std::to_string(mEntity);
}

void DisEntityId::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

void DisEntityId::Get(DisUint16& aSite, DisUint16& aApplication, DisUint16& aEntity) const
{
   aSite        = mSite;
   aApplication = mApplication;
   aEntity      = mEntity;
}

bool DisEntityId::IsValid() const
{
   return true;
}
