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

#include "UtoResourceIteratorPolicies.hpp"

#include "UtoResourceDB.hpp"
#include "UtoResourceIterators.hpp"

UtoTerrainIteratorPolicies::UtoTerrainIteratorPolicies()
   : m_pImpl(new UtoTerrainIterator_i)
{
}

UtoTerrainIteratorPolicies::UtoTerrainIteratorPolicies(UtoTerrainIterator_i* imp)
   : m_pImpl(imp)
{
}

UtoTerrainIteratorPolicies::UtoTerrainIteratorPolicies(const UtoTerrainIteratorPolicies& rhs)
   : m_pImpl(new UtoTerrainIterator_i(*rhs.m_pImpl))
{
}

UtoTerrainIteratorPolicies::~UtoTerrainIteratorPolicies()
{
   delete m_pImpl;
}

UtoTerrainIteratorPolicies::impl_ptr UtoTerrainIteratorPolicies::GetImpl()
{
   return m_pImpl;
}


UtoTerrainIteratorPolicies::value_type* UtoTerrainIteratorPolicies::ptr()
{
   return (m_pImpl->m_Iter->second);
}

void UtoTerrainIteratorPolicies::forward()
{
   ++m_pImpl->m_Iter;
}

bool UtoTerrainIteratorPolicies::is_equal(const UtoTerrainIteratorPolicies& rhs) const
{
   return (m_pImpl->m_Iter == rhs.m_pImpl->m_Iter);
}

void UtoTerrainIteratorPolicies::swap(UtoTerrainIteratorPolicies& rhs)
{
   UtoTerrainIterator_i* temp = rhs.m_pImpl;
   rhs.m_pImpl                = m_pImpl;
   m_pImpl                    = temp;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////////////////////// END TERRAIN ITERATOR POLICY //////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////////////////////// BEGIN SHAPE ITERATOR POLICY //////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

UtoShapeResIteratorPolicies::UtoShapeResIteratorPolicies()
   : m_pImpl(new UtoShapeResIterator_i)
{
}

UtoShapeResIteratorPolicies::UtoShapeResIteratorPolicies(UtoShapeResIterator_i* imp)
   : m_pImpl(imp)
{
}

UtoShapeResIteratorPolicies::UtoShapeResIteratorPolicies(const UtoShapeResIteratorPolicies& rhs)
   : m_pImpl(new UtoShapeResIterator_i(*rhs.m_pImpl))
{
}

UtoShapeResIteratorPolicies::~UtoShapeResIteratorPolicies()
{
   delete m_pImpl;
}

UtoShapeResIteratorPolicies::impl_ptr UtoShapeResIteratorPolicies::GetImpl()
{
   return m_pImpl;
}

UtoShapeResIteratorPolicies::value_type* UtoShapeResIteratorPolicies::ptr()
{
   if (m_pImpl->m_pNameShape == nullptr)
   {
      // tricky, sneaky in-place memory allocation.  don't try this at home.
      m_pImpl->m_pNameShape = new (m_pImpl->m_Memory) Utok::NameShape(m_pImpl->m_Iter->first, *m_pImpl->m_Iter->second);
   }
   return m_pImpl->m_pNameShape;
}

void UtoShapeResIteratorPolicies::forward()
{
   ++m_pImpl->m_Iter;
   m_pImpl->m_pNameShape = nullptr;
}

bool UtoShapeResIteratorPolicies::is_equal(const UtoShapeResIteratorPolicies& rhs) const
{
   return (m_pImpl->m_Iter == rhs.m_pImpl->m_Iter);
}

void UtoShapeResIteratorPolicies::swap(UtoShapeResIteratorPolicies& rhs)
{
   UtoShapeResIterator_i* temp = rhs.m_pImpl;
   rhs.m_pImpl                 = m_pImpl;
   m_pImpl                     = temp;
}
