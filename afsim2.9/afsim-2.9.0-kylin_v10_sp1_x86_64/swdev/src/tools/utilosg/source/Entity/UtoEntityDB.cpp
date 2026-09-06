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
// UtoEntityDB.cpp : implementation file

#include "UtoEntityDB.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <sstream>

#include "UtMemory.hpp"
#include "UtoEntity.hpp"
#include "UtoObjectManager.hpp"
#include "UtoPriv.hpp"

unsigned short UtoEntityDB::m_nNextID = 0;

/* UTO Function Block **********************************************************

Function: UtoEntityDB

Description:
Constructor

*******************************************************************************/

UtoEntityDB::UtoEntityDB(const std::string& name)
   : m_sDBName(name)
{
   std::ostringstream sstr;
   sstr << "EDB" << m_nNextID++;
   m_sInstanceName = sstr.str();
}

/* UTO Function Block **********************************************************

Function: UtoEntityDB

Description:
Destructor

*******************************************************************************/

UtoEntityDB::~UtoEntityDB()
{
   // Invoke the callbacks for the interested observers
   sendEvent(DestroyedEvent(), this);

   RemoveAll();
}

/* UTO Function Block **********************************************************

Function: Add

Description:
Add a new entity to the database and notify any observers

*******************************************************************************/

UtoEntityDB::EntityMap::iterator UtoEntityDB::Add(UtoEntity* ent)
{
   // Put the entity in the map
   std::pair<EntityMap::iterator, bool> ib = m_Entities.insert(std::make_pair(ent->ObjectName(), ent));
   if (!ib.second)
      throw 0;

   ent->SetDB(this);

   // store these for cleanup.
   m_EntityCleanup.insert(ent);

   // Notify the observers
   sendEvent(AddEntityEvent(), ent);

   return ib.first;
}

UtoEntityDB::iterator UtoEntityDB::Add()
{
   UtoEntity* new_entity = new UtoEntity;
   return Add(new_entity);
}

UtoEntity* UtoEntityDB::AddEntity()
{
   UtoEntity* new_entity = new UtoEntity;
   Add(new_entity);
   return new_entity;
}

/* UTO Function Block **********************************************************

Function: Remove

Description:
Remove entity and notify the observers

*******************************************************************************/

bool UtoEntityDB::Remove(EntityMap::iterator entity)
{
   // Order of callbacks occurring on this event:
   // 1. UTO_EDB_REMOVE
   // 2. UTO_ENT_REMOVE

   UtoEntity* ent_i = entity->second;

   // Notify the observers of data base
   sendEvent(RemoveEntityEvent(), ent_i);

   if (m_EntityCleanup.find(ent_i) != m_EntityCleanup.end())
   {
      delete ent_i;
      m_EntityCleanup.erase(ent_i);
   }
   m_Entities.erase(entity);

   return true;
}

bool UtoEntityDB::Remove(UtoEntity& aEntity)
{
   iterator i = m_Entities.find(aEntity.ObjectName());
   if (i != m_Entities.end())
   {
      return Remove(i);
   }
   return false;
}


/* UTO Function Block **********************************************************

Function: RemoveAll

Description:
Remove all entities and notify the observers

*******************************************************************************/

bool UtoEntityDB::RemoveAll()
{
   // Order of callbacks occurring on this event:
   // 1. UTO_EDB_REMOVE
   // 2. UTO_ENT_REMOVE

   EntityMap::iterator ent(m_Entities.begin()), ent_end(m_Entities.end());
   while (ent != ent_end)
   {
      UtoEntity* ent_i = ent->second;

      // Notify the observers of database
      sendEvent(RemoveEntityEvent(), ent_i);

      delete ent_i;
      ++ent;
   }
   m_Entities.clear();
   m_EntityCleanup.clear();

   return true;
}

/* UTO Function Block **********************************************************

Function: Find

Description:
Returns the entity if it is found

*******************************************************************************/

UtoEntityDB::EntityMap::iterator UtoEntityDB::Find(unsigned long name)
{
   return m_Entities.find(name);
}

/* UTO Function Block **********************************************************

Function: Entities

Description:
Returns the begin and end of the entity map

*******************************************************************************/

UtoEntityDB::IteratorPair UtoEntityDB::Entities()
{
   IteratorPair pr = std::make_pair(m_Entities.begin(), m_Entities.end());
   return pr;
}

void UtoEntityDB::Dump(std::ostream& o)
{
   o << "******************************\n";
   o << "EntityDB Dump\n";
   o << "  Name: " << m_sDBName << "\n";
   o << "  Instance: " << m_sInstanceName << "\n";

   if (m_Entities.empty())
   {
      o << "  No entities!\n";
   }
   else
   {
      o << "  NumEntities: " << (unsigned int)m_Entities.size() << "\n\n";

      UtoEntityDB::EntityMap::iterator it, end(m_Entities.end());
      for (it = m_Entities.begin(); it != end; ++it)
      {
         o << "Entity Dump\n";
         o << "-----------\n";
         it->second->Dump(o);
         o << "\n";
      }
   }
}

int UtoEntityDB::Destroy()
{
   RemoveAll();
   return 0;
}

/*
Retrieves the named entity database.

On the first call to this method with a new name, an entity database will
be created and associated with that name.  The default database is specified
with an empty string ("") and is actually created during UtoInit.
Entity databases do not have to be created through this method.  However, using
this method provides a convienent technique for making the entity database
available throughout your application.
\param name the entity database to be returned.
\return UtoEntityDB & - a reference to the named entity database.
*/
UtoEntityDB& UtoEntityDB::Instance(const std::string& name)
{
   UtoEntityDB* db = nullptr;
   std::string  db_name("EDB:");
   db_name.append(name);

   UtoObjectManager* omgr = UtoObjectManager::Instance();
   if (!omgr->IsRegistered(db_name.c_str()))
   {
      // database gets the user name.
      auto obj = ut::make_unique<UtoEntityDB>(name);

      // insert DB into ObjectManager using modified user name.
      // if (!omgr->Register (db_name, obj->m_pImp, EntityDBDestroyer))
      // if (!omgr->Register (db_name, obj->m_pImp, EntityDBDestroyer2 ()))
      if (!omgr->Register(db_name.c_str(), obj.get() /*, std::mem_fun(&UtoEntityDB::Destroy)*/))
         throw 0;

      db = obj.release();
   }
   else
   {
      db = UtoObjectSingleton<UtoEntityDB>::Instance(db_name.c_str());
   }

   assert(db != nullptr);

   return *db;
}

void UtoEntityDB::AddViewer(UtoViewer* aViewer)
{
   mViewers.push_back(aViewer);
}

void UtoEntityDB::RemoveViewer(UtoViewer* aViewer)
{
   mViewers.erase(std::find(mViewers.begin(), mViewers.end(), aViewer));
}
