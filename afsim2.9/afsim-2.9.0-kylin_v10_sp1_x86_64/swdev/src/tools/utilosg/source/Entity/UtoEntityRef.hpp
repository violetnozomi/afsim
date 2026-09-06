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
// UtoEntityRef.hpp : header file
#if !defined(_UTO_ENTITYREF_HPP_)
#define _UTO_ENTITYREF_HPP_

class UtoEntity;
class UtoEntityDB;
class UtoEntity;

#include "utilosg_export.h"


class UTILOSG_EXPORT UtoEntityRef
{
public:
   UtoEntityRef();
   UtoEntityRef(const UtoEntityRef& rEntityRef);
   UtoEntityRef(UtoEntity* entity);
   virtual ~UtoEntityRef();

   void SetEntity(UtoEntity* entity);
   void Reset();

   bool IsValid() const { return m_pEntity != nullptr; }

   UtoEntity*       GetEntity() { return m_pEntity; }
   const UtoEntity* GetEntity() const { return m_pEntity; }

   UtoEntity&       operator*();
   const UtoEntity& operator*() const;

   UtoEntityRef& operator=(const UtoEntityRef&);

   UtoEntity*       operator->() { return m_pEntity; }
   const UtoEntity* operator->() const { return m_pEntity; }

   bool operator==(const UtoEntityRef& rhs) const;
   bool operator<(const UtoEntityRef& rhs) const;

   // Not to be called be application.
   void OnRemoveEntityCB(UtoEntity* pEntityImp);

private:
   bool Connect();
   void Disconnect();

   UtoEntity* m_pEntity;
};

#endif
