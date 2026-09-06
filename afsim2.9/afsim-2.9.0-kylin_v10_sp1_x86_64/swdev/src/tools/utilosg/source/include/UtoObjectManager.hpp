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
// UtoObjectManager.hpp : header file

#if !defined(_UTOOBJECTMANAGER_HPP_)
#define _UTOOBJECTMANAGER_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <list>
#include <map>
#include <string>

class UTILOSG_EXPORT UtoObjectManager
{
public:
   // class Registration
   class Registration
   {
   public:
      virtual ~Registration() {}
      virtual int Destroy() = 0;
   };

   typedef std::map<std::string, Registration*> ObjectMap;
   typedef std::list<Registration*>             DestroyerList;

   // class ObjRegistration
   template<class T>
   class ObjRegistration : public Registration
   {
   public:
      ObjRegistration(T* obj)
         : m_pObject(obj)
      {
      }
      ~ObjRegistration() override {}

      T*       Get() { return m_pObject; }
      const T* Get() const { return m_pObject; }

   protected:
      T* m_pObject;
   };

   // class Destroyer
   template<class T>
   struct Destroyer
   {
      void operator()(T* obj) { delete obj; }
   };

   // class ObjDestroyer
   template<class T, class Fun>
   class ObjDestroyer : public ObjRegistration<T>
   {
   public:
      ObjDestroyer(T* obj)
         : ObjRegistration<T>(obj)
      {
      }
      ObjDestroyer(T* obj, Fun f)
         : ObjRegistration<T>(obj)
         , m_DestroyFunction(f)
      {
      }
      ~ObjDestroyer() override {}

      using ObjRegistration<T>::m_pObject;
      int Destroy() override
      {
         m_DestroyFunction(m_pObject);
         delete this;
         return 0;
      }

   private:
      Fun m_DestroyFunction;
   };

   // Methods
   virtual ~UtoObjectManager();

   static UtoObjectManager* Instance();

   template<class T>
   bool Register(const std::string& name, T* obj)
   {
      return Register_i(name, new ObjDestroyer<T, Destroyer<T>>(obj));
   }

   template<class T, class Fun>
   bool Register(const std::string& name, T* obj, Fun f)
   {
      return Register_i(name, new ObjDestroyer<T, Fun>(obj, f));
   }

   bool Unregister(const std::string& name);
   void Destroy(const std::string& name);
   void DestroyAllObjects(bool delete_this = true);

   Registration* Find(const std::string& name)
   {
      ObjectMap::iterator it = m_Objects.find(name);
      if (it != m_Objects.end())
         return it->second;
      return nullptr;
   }

   bool IsRegistered(const std::string& name) { return (m_Objects.find(name) != m_Objects.end()); }

protected:
private:
   UtoObjectManager();
   UtoObjectManager(const UtoObjectManager&);
   UtoObjectManager& operator=(const UtoObjectManager&);

   bool Register_i(const std::string& name, Registration* reg);

   static UtoObjectManager* m_pMe;

   ObjectMap     m_Objects;
   DestroyerList m_DestroyList;
};

template<class T>
class UtoObjectSingleton
{
public:
   static T* Instance(const std::string& name)
   {
      UtoObjectManager::Registration* reg = UtoObjectManager::Instance()->Find(name);
      if (reg)
      {
         return dynamic_cast<UtoObjectManager::ObjRegistration<T>*>(reg)->Get();
      }
      return nullptr;
   }
};

#endif // !defined(_UTOOBJECTMANAGER_HPP_)
