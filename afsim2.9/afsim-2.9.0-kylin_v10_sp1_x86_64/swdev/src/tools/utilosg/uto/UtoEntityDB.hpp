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
// UtoEntityDB.hpp : header file

#if !defined(_UTOENTITYDB_HPP_)
#define _UTOENTITYDB_HPP_

#if _MSC_VER > 1000
#pragma once
#endif


#include <list>
#include <map>
#include <set>

#include "UtoCallback.hpp"
#include "UtoEntityDB.hpp"
#include "UtoTypes.hpp"

class UtoViewer;
class UtoEntity;

class UTILOSG_EXPORT UtoEntityDB : public UtoCallback<UtoEntityDB>
{
public:
   typedef std::map<unsigned long, UtoEntity*>                 EntityMap;
   typedef std::pair<EntityMap::iterator, EntityMap::iterator> IteratorPair;
   typedef EntityMap::iterator                                 iterator;
   typedef EntityMap::const_iterator                           const_iterator;
   static UtoEntityDB&                                         Instance(const std::string& name);
   UtoEntityDB(const std::string& name);
   ~UtoEntityDB() override;

   std::string Name() const { return m_sDBName; }

   UtoEntity*          AddEntity();
   EntityMap::iterator Add();
   EntityMap::iterator Add(UtoEntity* ent);
   // EntityMap::iterator Add (const UtoEntityImp& master);
   bool                Remove(EntityMap::iterator entity);
   bool                Remove(UtoEntity& aEntity);
   bool                RemoveAll();
   EntityMap::iterator Find(unsigned long name);
   IteratorPair        Entities();

   void Dump(std::ostream&);
   int  Destroy();

   const EntityMap& GetEntities() const { return m_Entities; }

   void                            AddViewer(UtoViewer* aViewer);
   void                            RemoveViewer(UtoViewer* aViewer);
   const std::vector<UtoViewer*>&  Viewers() const { return mViewers; }
   typedef Event1<1, UtoEntity*>   AddEntityEvent;
   typedef Event1<2, UtoEntity*>   RemoveEntityEvent;
   typedef Event1<3, UtoEntityDB*> DestroyedEvent;

protected:
private:
   UtoEntityDB(const UtoEntityDB&);
   UtoEntityDB& operator=(const UtoEntityDB&);


   static unsigned short m_nNextID;

   EntityMap               m_Entities;
   std::vector<UtoViewer*> mViewers;
   std::string             m_sDBName;
   std::string             m_sInstanceName;

   std::set<UtoEntity*> m_EntityCleanup;
};

#endif // !defined(_ENTITYDB_H_)
