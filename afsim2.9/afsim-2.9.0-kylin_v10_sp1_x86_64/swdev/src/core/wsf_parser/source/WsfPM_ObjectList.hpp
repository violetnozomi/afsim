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

#ifndef WSFPM_OBJECTLIST_HPP
#define WSFPM_OBJECTLIST_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyList.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_List : public WsfPProxyNode
{
public:
   WsfPM_List() {}

   WsfPM_List(const WsfPProxyNode& aNode)
      : WsfPProxyNode(aNode)
   {
   }
   WsfPProxyList* list() const { return GetValue().GetList(); }

   size_t size() const
   {
      WsfPProxyList* listPtr = list();
      return (listPtr ? listPtr->Size() : 0);
   }

   WsfPProxyNode back() const;

   bool append(WsfPProxyValue aValue);
   bool insert(WsfPProxyValue aValue, size_t aIndex);
   bool remove(size_t aIndex);

   //! Returns the entry with the given name
   //! If no object exists with that name, IsNull() returns true
   WsfPProxyNode operator[](size_t aIndex);
};

template<typename TYPE>
class WSF_PARSER_EXPORT WsfPM_ObjectList : public WsfPM_List
{
public:
   WsfPM_ObjectList() {}

   WsfPM_ObjectList(const WsfPProxyNode& aNode)
      : WsfPM_List(aNode)
   {
   }

   //! Returns the entry with the given name
   //! If no object exists with that name, IsNull() returns true
   TYPE operator[](size_t aIndex) { return TYPE(WsfPM_List::operator[](aIndex)); }
};

#endif
