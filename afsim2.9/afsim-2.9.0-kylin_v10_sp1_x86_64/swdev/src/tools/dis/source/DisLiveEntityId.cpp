// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisLiveEntityId.hpp"

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisUint16 DisLiveEntityId::mNextEntity = 0;

const DisLiveEntityId DisLiveEntityId::cLIVE_ENTITY_ID_UNKNOWN(cLIVE_ENTITY_ID_UNKNOWN);
const DisLiveEntityId DisLiveEntityId::cLIVE_MUNITION_NOT_TRACKED(cLIVE_NO_SITE, cLIVE_NO_APPLIC, Dis::cNO_ENTITY);
const DisLiveEntityId DisLiveEntityId::cLIVE_NO_ENTITY_IMPACTED(cLIVE_NO_SITE, cLIVE_NO_APPLIC, Dis::cNO_ENTITY);
const DisLiveEntityId DisLiveEntityId::cLIVE_NO_LOCATION(cLIVE_NO_SITE, cLIVE_NO_APPLIC, Dis::cNO_ENTITY);
const DisLiveEntityId DisLiveEntityId::cLIVE_NO_SPECIFIC_ENTITY(cLIVE_NO_SPECIFIC_ENTITY);
const DisLiveEntityId DisLiveEntityId::cLIVE_TARGET_ID_UNKNOWN(cLIVE_TARGET_ID_UNKNOWN);

const DisLiveEntityId DisLiveEntityId::cLIVE_D_SPOT_NO_ENTITY(cLIVE_D_SPOT_NO_ENTITY);

DisLiveEntityId::DisLiveEntityId()
   : mSite(cLIVE_NO_SITE)
   , mApplication(cLIVE_NO_APPLIC)
   , mEntity(Dis::cNO_ENTITY)
{
}

DisLiveEntityId::DisLiveEntityId(const DisLiveEntityId& aSrc)
   : mSite(aSrc.mSite)
   , mApplication(aSrc.mApplication)
   , mEntity(aSrc.mEntity)
{
}

DisLiveEntityId::DisLiveEntityId(DisUint8 aSite, DisUint8 aApplication, DisUint16 aEntity)
   : mSite(aSite)
   , mApplication(aApplication)
   , mEntity(aEntity)
{
}

const char* DisLiveEntityId::GetScriptClassName() const
{
   return "DisLiveEntityId";
}

// Set the next entity number to be assigned by 'AssignNewEntity()'.
// static
void DisLiveEntityId::SetNextEntity(DisUint16 aNextEntity)
{
   mNextEntity = aNextEntity;
}

// Assigns the next sequential entity number.  Site and Application
// are not modified.
void DisLiveEntityId::AssignNewEntity()
{
   while ((mNextEntity == Dis::cNO_ENTITY) || (mNextEntity == Dis::cALL_ENTITIES) || (mNextEntity == Dis::cRQST_ASSIGN_ID))
   {
      ++mNextEntity;
   }
   mEntity = mNextEntity;
   ++mNextEntity;
}

bool DisLiveEntityId::IsDefined() const
{
   // A entity value of ASSIGN_ENTITY is reserved for creation PDUs to alert the
   // receiving simulation to assign an Entity ID to the entity it creates.
   return (mSite != cLIVE_NO_SITE && mApplication != cLIVE_NO_APPLIC && mEntity != Dis::cNO_ENTITY &&
           mEntity != Dis::cRQST_ASSIGN_ID);
}

bool DisLiveEntityId::IsSingle() const
{
   return (IsDefined() && mSite != Dis::cALL_SITES && mApplication != Dis::cALL_APPLIC && mEntity != Dis::cALL_ENTITIES);
}

DisLiveEntityId* DisLiveEntityId::Clone() const
{
   return new DisLiveEntityId(*this);
}

void DisLiveEntityId::Get(GenI& aGenI)
{
   aGenI >> mSite;
   aGenI >> mApplication;
   aGenI >> mEntity;
}

void DisLiveEntityId::Put(GenO& aGenO) const
{
   aGenO << mSite;
   aGenO << mApplication;
   aGenO << mEntity;
}

DisLiveEntityId& DisLiveEntityId::operator=(const DisLiveEntityId& aRhs)
{
   if (this != &aRhs)
   {
      mSite        = aRhs.mSite;
      mApplication = aRhs.mApplication;
      mEntity      = aRhs.mEntity;
   }
   return *this;
}

bool DisLiveEntityId::operator<(const DisLiveEntityId& aRhs) const
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

std::string DisLiveEntityId::ToString() const
{
   return std::to_string(mSite) + ':' + std::to_string(mApplication) + ':' + std::to_string(mEntity);
}

void DisLiveEntityId::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisLiveEntityId::Get(DisUint8& aSite, DisUint8& aApplication, DisUint16& aEntity) const
{
   aSite        = mSite;
   aApplication = mApplication;
   aEntity      = mEntity;
}

bool DisLiveEntityId::IsValid() const
{
   return true;
}
