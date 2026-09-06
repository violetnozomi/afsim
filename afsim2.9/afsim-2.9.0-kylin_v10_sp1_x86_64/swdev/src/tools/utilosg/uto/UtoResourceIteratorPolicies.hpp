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

#ifndef _UTO_RESOURCE_ITERATOR_POLICIES_HPP_
#define _UTO_RESOURCE_ITERATOR_POLICIES_HPP_

// uto includes
#include "utilosg_export.h"

#include "UtoTypes.hpp"

// forward declarations
class UtoTerrain;

struct UtoTerrainIterator_i;
struct UtoShapeResIterator_i;

struct UTILOSG_EXPORT UtoTerrainIteratorPolicies
{
   typedef UtoTerrain            value_type;
   typedef UtoTerrainIterator_i* impl_ptr;

public:
   impl_ptr GetImpl();

protected:
   UtoTerrainIteratorPolicies();
   UtoTerrainIteratorPolicies(impl_ptr imp);
   UtoTerrainIteratorPolicies(const UtoTerrainIteratorPolicies& rhs);
   ~UtoTerrainIteratorPolicies();

   value_type* ptr();
   void        forward();
   bool        is_equal(const UtoTerrainIteratorPolicies&) const;
   void        swap(UtoTerrainIteratorPolicies&);

private:
   UtoTerrainIteratorPolicies& operator=(const UtoTerrainIteratorPolicies& rhs);

   impl_ptr m_pImpl;
};

struct UTILOSG_EXPORT UtoShapeResIteratorPolicies
{
   typedef Utok::NameShape        value_type;
   typedef UtoShapeResIterator_i* impl_ptr;

public:
   impl_ptr GetImpl();

protected:
   UtoShapeResIteratorPolicies();
   UtoShapeResIteratorPolicies(impl_ptr imp);
   UtoShapeResIteratorPolicies(const UtoShapeResIteratorPolicies& rhs);
   ~UtoShapeResIteratorPolicies();

   value_type* ptr();
   void        forward();
   bool        is_equal(const UtoShapeResIteratorPolicies&) const;
   void        swap(UtoShapeResIteratorPolicies&);

private:
   UtoShapeResIteratorPolicies& operator=(const UtoShapeResIteratorPolicies& rhs);

   impl_ptr m_pImpl;
};

#endif // _UTO_RESOURCE_ITERATOR_POLICIES_HPP_
