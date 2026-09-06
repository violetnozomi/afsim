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

// UtoTerrainFactory.hpp: interface for the UtoTerrainFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UTOTERRAINFACTORY_HPP_)
#define _UTOTERRAINFACTORY_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <istream>
#include <map>

#include "UtoTypes.hpp"

class UtoTerrain;

class UtoTerrainFactory
{
public:
   virtual ~UtoTerrainFactory();

   typedef UtoTerrain* (*CreateTerrainCB)();

   static UtoTerrainFactory* Instance();

   bool RegisterTerrain(const std::string& type, CreateTerrainCB func);
   bool UnregisterTerrain(const std::string& type);
   // friend UtoTerrain* CreateTerrain (const std::string& type);
   UtoTerrain* CreateTerrain(const std::string& type);
   bool        Supports(const std::string& type) const;

private:
   typedef std::map<std::string, CreateTerrainCB> CallbackMap;

   UtoTerrainFactory();
   UtoTerrainFactory(const UtoTerrainFactory&);
   UtoTerrainFactory& operator=(const UtoTerrainFactory&);

   static UtoTerrainFactory* m_pMe;

   CallbackMap m_Callbacks;
};

#endif // !defined(_UTOTERRAINFACTORY_HPP_)
