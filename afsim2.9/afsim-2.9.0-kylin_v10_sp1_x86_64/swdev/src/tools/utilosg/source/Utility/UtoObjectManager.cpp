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
// UtoObjectManager.cpp : implementation file

#include "UtoObjectManager.hpp"

#include <algorithm>
#include <functional>

UtoObjectManager* UtoObjectManager::m_pMe = nullptr;

UtoObjectManager::UtoObjectManager() {}

UtoObjectManager::~UtoObjectManager() {}

UtoObjectManager* UtoObjectManager::Instance()
{
   if (m_pMe == nullptr)
   {
      m_pMe = new UtoObjectManager;
   }
   return m_pMe;
}

bool UtoObjectManager::Register_i(const std::string& name, Registration* reg)
{
   m_DestroyList.push_back(reg);
   return m_Objects.insert(ObjectMap::value_type(name, reg)).second;
}

bool UtoObjectManager::Unregister(const std::string& name)
{
   ObjectMap::iterator it = m_Objects.find(name);
   if (it != m_Objects.end())
   {
      m_DestroyList.erase(
         std::find_if(m_DestroyList.begin(), m_DestroyList.end(), [it](Registration* aPtr) { return it->second == aPtr; }));
      delete (it->second);
      m_Objects.erase(it);
   }
   return true;
}

void UtoObjectManager::Destroy(const std::string& name)
{
   ObjectMap::iterator it = m_Objects.find(name);
   if (it != m_Objects.end())
   {
      m_DestroyList.erase(
         std::find_if(m_DestroyList.begin(), m_DestroyList.end(), [it](Registration* aPtr) { return it->second == aPtr; }));
      it->second->Destroy();
      m_Objects.erase(it);
   }
}

void UtoObjectManager::DestroyAllObjects(bool delete_this)
{
   std::for_each(m_DestroyList.begin(), m_DestroyList.end(), std::mem_fn(&Registration::Destroy));

   if (delete_this)
   {
      delete this;
      m_pMe = nullptr;
   }
   else
   {
      m_Objects.clear();
      m_DestroyList.clear();
   }
}
