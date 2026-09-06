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


// UtoTerrainFactory.cpp: implementation of the UtoTerrainFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "UtoTerrainFactory.hpp"

#include "UtoObjectManager.hpp"

UtoTerrainFactory* UtoTerrainFactory::m_pMe = nullptr;

//////////////////////////////////////////////////////////////////////

UtoTerrainFactory::UtoTerrainFactory()
{
   UtoObjectManager::Instance()->Register("TerrainFactory", this);
}

UtoTerrainFactory::~UtoTerrainFactory() {}

UtoTerrainFactory* UtoTerrainFactory::Instance()
{
   if (m_pMe == nullptr)
   {
      m_pMe = new UtoTerrainFactory;
   }
   return m_pMe;
}

bool UtoTerrainFactory::RegisterTerrain(const std::string& type, CreateTerrainCB func)
{
   return m_Callbacks.insert(std::make_pair(type, func)).second;
}

bool UtoTerrainFactory::UnregisterTerrain(const std::string& type)
{
   return m_Callbacks.erase(type) == 1;
}

UtoTerrain* UtoTerrainFactory::CreateTerrain(const std::string& type)
{
   CallbackMap::iterator it = m_Callbacks.find(type.c_str());

   if (it == m_Callbacks.end())
   {
      // throw std::runtime_error ("Unknown shape type");
      // DEBUG (("Unknown terrain type: %s", type.c_str ()));
      return nullptr;
   }

   return it->second();
}

bool UtoTerrainFactory::Supports(const std::string& type) const
{
   CallbackMap::const_iterator it = m_Callbacks.find(type);
   return (it != m_Callbacks.end());
}
