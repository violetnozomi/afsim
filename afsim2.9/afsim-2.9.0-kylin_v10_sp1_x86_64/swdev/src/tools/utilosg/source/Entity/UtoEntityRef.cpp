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

/////////////////////////////////////////////////////////////////////////////
// UtoEntityRef.cpp : implementation file
#include "UtoEntityRef.hpp"

#include "UtoEntity.hpp"
#include "UtoEntityDB.hpp"

/* UTO Function Block **********************************************************

Function: UtoEntityRef

Description:
   Default constructor

*******************************************************************************/
UtoEntityRef::UtoEntityRef()
   : m_pEntity(nullptr)
{
}

/* UTO Function Block **********************************************************

Function: UtoEntityRef

Description:
   Copy constructor

*******************************************************************************/
UtoEntityRef::UtoEntityRef(const UtoEntityRef& rhs)
   : m_pEntity(rhs.m_pEntity)
{
   Connect();
}

/* UTO Function Block **********************************************************

Function: UtoEntityRef

Description:
   Constructor

*******************************************************************************/
UtoEntityRef::UtoEntityRef(UtoEntity* entity)
{
   // Set the new entity pointer.
   m_pEntity = entity;

   // Connect callbacks to the new entity.
   Connect();
}

/* UTO Function Block **********************************************************

Function: ~UtoEntityRef

Description:
   Destructor

*******************************************************************************/
UtoEntityRef::~UtoEntityRef()
{
   Disconnect();

   m_pEntity = nullptr;
}

/* UTO Function Block **********************************************************

Function: SetEntity

Description:
   Sets an entity to watch

*******************************************************************************/
void UtoEntityRef::SetEntity(UtoEntity* entity)
{
   // Disconnect any existing callbacks.
   Disconnect();

   // Set the new entity pointer.
   m_pEntity = entity;

   // Connect callbacks to the new entity.
   Connect();
}

/* UTO Function Block **********************************************************

Function: Connect

Description:
   Connect callbacks to the entity to track destruction.

*******************************************************************************/
bool UtoEntityRef::Connect()
{
   bool success = false;
   if (m_pEntity)
   {
      m_pEntity->connect(UtoEntity::DestroyedEvent(), this, &UtoEntityRef::OnRemoveEntityCB);
      success = true;
   }

   return success;
}

/* UTO Function Block **********************************************************

Function: Disconnect

Description:
   Disconnects callbacks to the entity to track destruction.

*******************************************************************************/
void UtoEntityRef::Disconnect()
{
   if (m_pEntity)
   {
      m_pEntity->disconnect(UtoEntity::DestroyedEvent(), this, &UtoEntityRef::OnRemoveEntityCB);
   }
}

/* UTO Function Block **********************************************************

Function: Reset

Description:
   Disconnects callbacks, and sets the pointer to null.

*******************************************************************************/
void UtoEntityRef::Reset()
{
   Disconnect();

   m_pEntity = nullptr;
}

/* UTO Function Block **********************************************************

Function: OnRemoveEntityCB

Description:
   A callback that gets called when the underlying entity is destroyed.

*******************************************************************************/
void UtoEntityRef::OnRemoveEntityCB(UtoEntity* pEntityImp)
{
   Reset();
}

/* UTO Function Block **********************************************************

Function: operator*

Description:
   A dereference operator to get at the underlying entity.

*******************************************************************************/
UtoEntity& UtoEntityRef::operator*()
{
   return *m_pEntity;
}

const UtoEntity& UtoEntityRef::operator*() const
{
   return *m_pEntity;
}

/* UTO Function Block **********************************************************

Function: operator=

Description:
   Assignment operator.

*******************************************************************************/
UtoEntityRef& UtoEntityRef::operator=(const UtoEntityRef& rhs)
{
   if (this == &rhs)
   {
      return *this;
   }

   Disconnect();

   m_pEntity = rhs.m_pEntity;

   Connect();

   return *this;
}

/* UTO Function Block **********************************************************

Function: operator==

Description:
   Equality operator.  Compares the underlying entity pointer.

*******************************************************************************/
bool UtoEntityRef::operator==(const UtoEntityRef& rhs) const
{
   return (m_pEntity == rhs.m_pEntity);
}

/* UTO Function Block **********************************************************

Function: operator<

Description:
   Primarily supplied so this class can be used in STL containers.

*******************************************************************************/
bool UtoEntityRef::operator<(const UtoEntityRef& rhs) const
{
   return m_pEntity < rhs.m_pEntity;
}
