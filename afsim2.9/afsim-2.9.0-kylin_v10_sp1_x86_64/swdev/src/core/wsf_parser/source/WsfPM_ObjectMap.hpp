// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPM_OBJECTMAP_HPP
#define WSFPM_OBJECTMAP_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyNode.hpp"
#include "WsfPProxyObjectMap.hpp"

// This file contains classes which wrap the proxy values providing a more simplified interface
// to a reduced set of information.
//
// Users should be wary that these classes are just references to the proxy tree.
// If the proxy tree is changed, these references may become invalid.  Invalidate() may be used
// to attempt to recreate the reference.  In general, no instances of these classes should be
// retained for longer than necessary.

class WSF_PARSER_EXPORT WsfPM_ObjectMap : public WsfPProxyNode
{
public:
   WsfPM_ObjectMap();
   WsfPM_ObjectMap(const WsfPProxyNode& aNode);

   WsfPProxyObjectMap*      GetObjectMap() const;
   std::vector<std::string> GetObjectNames() const;
   std::vector<std::string> GetAllObjectNames();

   using iterator       = std::map<std::string, WsfPProxyValue>::iterator;
   using const_iterator = std::map<std::string, WsfPProxyValue>::const_iterator;
   iterator begin() const;
   iterator end() const;

   WsfPProxyNode Find(const std::string& aName) const;
   WsfPProxyNode At(const iterator& i) const;
   bool          Exists(const std::string& aName) const;
   bool          Remove(const std::string& aName, bool removedByUser = false);

   WsfPProxyNode Add(const std::string& aName, WsfPProxyValue aEntry);
   WsfPProxyNode AssignEntry(const std::string& aName, WsfPProxyValue aEntry);

   bool Move(const std::string& aOldName, const std::string& aNewName);
   bool CanMove(const std::string& aOldName, const std::string& aNewName);
   void GetValues(std::vector<WsfPProxyNode>& aAll) const;
   void GetAllValues(std::vector<WsfPProxyNode>& aAll);

protected:
   template<typename TYPE>
   void GetValuesT(std::vector<TYPE>& aAll) const
   {
      WsfPProxyValue val = GetValue();
      if (val.GetObjectMap())
      {
         WsfPProxyObjectMap* mapPtr = val.GetObjectMap();
         for (auto i = mapPtr->GetValues().begin(); i != mapPtr->GetValues().end(); ++i)
         {
            aAll.push_back(TYPE(*this + i->first));
         }
      }
   }

   template<typename TYPE>
   void GetAllValuesT(std::vector<TYPE>& aAll)
   {
      GetValuesT<TYPE>(aAll);
      if (SwitchToBasicTypes())
      {
         GetValuesT<TYPE>(aAll);
         Invalidate();
      }
   }
};

template<typename TYPE>
class WsfPM_ObjectMapT : public WsfPM_ObjectMap
{
public:
   WsfPM_ObjectMapT() {}
   WsfPM_ObjectMapT(const WsfPProxyNode& aNode)
      : WsfPM_ObjectMap(aNode)
   {
   }

   //! Returns the entry with the given name
   //! If no object exists with that name, IsNull() returns true
   TYPE FindT(const std::string& aName) const { return TYPE(WsfPM_ObjectMap::Find(aName)); }

   TYPE AtT(const iterator& i) const { return TYPE(WsfPM_ObjectMap::At(i)); }

   void GetValuesT(std::vector<TYPE>& aAll) const { WsfPM_ObjectMap::GetValuesT<TYPE>(aAll); }

   void GetAllValuesT(std::vector<TYPE>& aAll) { WsfPM_ObjectMap::GetAllValuesT<TYPE>(aAll); }
};

#endif
