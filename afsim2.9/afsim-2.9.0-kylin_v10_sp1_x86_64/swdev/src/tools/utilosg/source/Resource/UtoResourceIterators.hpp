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

#ifndef _UTO_RESOURCE_ITERATORS_HPP_
#define _UTO_RESOURCE_ITERATORS_HPP_

// uto includes
#include "UtoResourceDB.hpp"
#include "UtoSmallObj.hpp"
#include "UtoTypes.hpp"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////// BEGIN TERRAIN ITERATOR //////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

struct UtoTerrainIterator_i : public Loki::SmallObject<>
{
   // public typedefs
   typedef UtoResourceDB::terrain_iterator iterator;
   // constructors
   UtoTerrainIterator_i() /*: m_Iter( 0 )PMG CANNOT INIT AN ITERATOR TO NULL*/ {}
   UtoTerrainIterator_i(iterator i)
      : m_Iter(i)
   {
   }
   // public member variables
   iterator m_Iter;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////////////////////////// BEGIN SHAPE ITERATOR /////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

struct UtoShapeResIterator_i : public Loki::SmallObject<>
{
   // public typedefs
   typedef UtoResourceDB::shape_iterator iterator;
   // constructors
   UtoShapeResIterator_i()
      : /* m_Iter (0),PMG CANNOT INIT AN ITERATOR TO NULL*/ m_pNameShape(nullptr)
   {
   }
   UtoShapeResIterator_i(iterator i)
      : m_pNameShape(nullptr)
      , m_Iter(i)
   {
   }
   UtoShapeResIterator_i(const UtoShapeResIterator_i& rhs)
      : m_pNameShape(nullptr)
      , m_Iter(rhs.m_Iter)
   {
      if (rhs.m_pNameShape)
      {
         m_pNameShape = new (m_Memory) Utok::NameShape(m_Iter->first, *m_Iter->second);
      }
   }
   // public member variables
   Utok::NameShape* m_pNameShape;
   iterator         m_Iter;
   char             m_Memory[sizeof(Utok::NameShape)];
};

#endif // _UTO_RESOURCE_ITERATORS_H_
