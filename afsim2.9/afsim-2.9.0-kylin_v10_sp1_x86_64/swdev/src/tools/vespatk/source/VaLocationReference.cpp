// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaLocationReference.hpp"

#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"

namespace vespa
{

VaLocationReference::VaLocationReference()
   : VaLocationReference(nullptr)
{
}

VaLocationReference::VaLocationReference(const VaLocationReference& aSrc)
   : mEntityPtr(aSrc.mEntityPtr)
   , mPosition(aSrc.mPosition)
   , mCallbacks()
{
   UtVec3d::Set(mVelocity, aSrc.mVelocity);
   UtVec3d::Set(mOrientation, aSrc.mOrientation);

   SetupCallbacks();
}

VaLocationReference::VaLocationReference(VaEntity* aEntityPtr)
   : VaLocationReference(aEntityPtr, VaPosition())
{
}

VaLocationReference::VaLocationReference(const VaPosition& aPosition)
   : VaLocationReference(nullptr, aPosition)
{
}

VaLocationReference::VaLocationReference(VaEntity* aEntityPtr, const VaPosition& aPosition)
   : mEntityPtr(aEntityPtr)
   , mPosition(aPosition)
   , mCallbacks()
{
   UtVec3d::Set(mVelocity, 0.0);
   UtVec3d::Set(mOrientation, 0.0);

   SetupCallbacks();
}

VaLocationReference::~VaLocationReference()
{
   mEntityPtr = nullptr;
   mCallbacks.Clear();
}

VaLocationReference& VaLocationReference::operator=(const VaLocationReference& aRhs)
{
   mEntityPtr = aRhs.mEntityPtr;
   mPosition  = aRhs.mPosition;
   UtVec3d::Set(mVelocity, aRhs.mVelocity);
   UtVec3d::Set(mOrientation, aRhs.mOrientation);
   SetupCallbacks();
   return *this;
}

void VaLocationReference::SetPosition(const VaPosition& aPosition)
{
   // Clear the entity.
   if (GetEntity() != nullptr)
   {
      mEntityPtr = nullptr;
      SetupCallbacks();
   }
   mPosition = aPosition;
}

const VaPosition& VaLocationReference::GetPosition() const
{
   if (GetEntity() != nullptr)
   {
      mPosition = GetEntity()->GetPosition();
   }
   return mPosition;
}

void VaLocationReference::SetVelocity(const double aVelocity[3])
{
   // Clear the entity.
   if (GetEntity() != nullptr)
   {
      mEntityPtr = nullptr;
      SetupCallbacks();
   }
   UtVec3d::Set(mVelocity, aVelocity);
}

void VaLocationReference::GetVelocity(double aVelocity[3]) const
{
   if (GetEntity() != nullptr)
   {
      UtVec3d::Set(mVelocity, GetEntity()->GetVelocityWCS().GetData());
   }
   UtVec3d::Set(aVelocity, mVelocity);
}

void VaLocationReference::SetOrientation(const double aOrientation[3])
{
   // Clear the entity.
   if (GetEntity() != nullptr)
   {
      mEntityPtr = nullptr;
      SetupCallbacks();
   }
   UtVec3d::Set(mOrientation, aOrientation);
}

void VaLocationReference::GetOrientation(double aOrientation[3]) const
{
   if (GetEntity() != nullptr)
   {
      GetEntity()->GetOrientation(mOrientation[0], mOrientation[1], mOrientation[2]);
   }
   UtVec3d::Set(aOrientation, mOrientation);
}

void VaLocationReference::SetEntity(const VaEntity* aEntityPtr)
{
   mEntityPtr = aEntityPtr;
   SetupCallbacks();
}

void VaLocationReference::SetupCallbacks()
{
   mCallbacks.Clear();
   if (mEntityPtr != nullptr)
   {
      mCallbacks.Add(VaObserver::EntityRemoved.Connect(&VaLocationReference::EntityRemovedOrDeletedCB,
                                                       this,
                                                       mEntityPtr->GetUniqueId()));
      mCallbacks.Add(VaObserver::EntityDeleted.Connect(&VaLocationReference::EntityRemovedOrDeletedCB,
                                                       this,
                                                       mEntityPtr->GetUniqueId()));
   }
}

void VaLocationReference::EntityRemovedOrDeletedCB(VaEntity* aEntityPtr)
{
   if ((mEntityPtr != nullptr) && (mEntityPtr == aEntityPtr))
   {
      mEntityPtr = nullptr;
      SetupCallbacks();
   }
}
} // end namespace vespa
