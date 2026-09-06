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
// UtoPolyLineShapeImp.cpp : implementation file

#include "UtoPolyShapeImp.hpp"

#include <osg/Geometry>
#include <osg/PrimitiveSet>

// This class contains the code common between UtoPolyLineShapeImp and
// UtoPolygonShapeImp.

UtoPolyShapeImp::UtoPolyShapeImp() {}

/* UTO Function Block **********************************************************

Function: Copy Constructor

Description:
   Since the class hierarchy keeps pointers to each object in the tree it is
   easier to do a shallow copy, copy the next level in the hierarchy,
   destroy the old references, and add the new copies than doing a deep copy
   and looking through the tree for the correct pointers.

   Copy the primitive set,
   remove the old primitive set,
   add the new primitive set

*******************************************************************************/

UtoPolyShapeImp::UtoPolyShapeImp(const UtoPolyShapeImp& rhs)
   : UtoGeometryShapeImp(rhs)
{
}

int UtoPolyShapeImp::Insert(int pos, const UtoPosition pts[], int num)
{
   // update the number of elements to draw
   // Note, enabling sides will create additional vertices,
   // m_Vertex->size() is no longer the number to look at.
   m_PrimitiveSet->setCount(m_PrimitiveSet->getCount() + num);

   // insert the new points
   m_Vertex->insert(m_Vertex->begin() + pos, num, pts[0]);
   // the first element is already in place
   osg::Vec3Array::iterator vertex = m_Vertex->begin() + pos + 1;
   for (int i = 1; i < num; ++i, ++vertex)
   {
      *vertex = pts[i];
   }

   // if not using a display list, do not call to dirty it
   if (m_Geometry->getUseDisplayList())
   {
      m_Geometry->dirtyDisplayList();
   }

   // update the bounding volume
   m_Geometry->dirtyBound();

   NeedCallback();

   return 0;
}

int UtoPolyShapeImp::Remove(int pos)
{
   int size = m_PrimitiveSet->getCount();

   if (pos > size || pos < 0)
      return -1;

   // update the number of elements to draw
   m_PrimitiveSet->setCount(size - 1);

   m_Vertex->erase(m_Vertex->begin() + pos);

   // if not using a display list, do not call to dirty it
   if (m_Geometry->getUseDisplayList())
   {
      m_Geometry->dirtyDisplayList();
   }

   // update the bounding volume
   m_Geometry->dirtyBound();

   NeedCallback();

   return 0;
}

void UtoPolyShapeImp::Clear()
{
   // If there are sides, the vertex list is doubled.
   int index = HasSides() ? m_Vertex->size() / 2 : m_Vertex->size();
   // Call remove until all elements have been removed.
   while (Remove(--index) != -1)
   {
   }
}

int UtoPolyShapeImp::Size() const
{
   // Note, enabling sides will create additional vertices,
   // m_Vertex->size() is no longer the number to look at.
   return m_PrimitiveSet->getCount();
}

UtoPosition UtoPolyShapeImp::At(int n)
{
   osg::Vec3Array::iterator vertex = m_Vertex->begin() + n;
   return UtoPosition(vertex->x(), vertex->y(), vertex->z());
}

int UtoPolyShapeImp::SetAt(int n, const UtoPosition& pos)
{
   osg::Vec3Array::iterator vertex = m_Vertex->begin() + n;
   *vertex                         = pos;

   // if not using a display list, do not call to dirty it
   if (m_Geometry->getUseDisplayList())
   {
      m_Geometry->dirtyDisplayList();
   }

   // update the bounding volume
   m_Geometry->dirtyBound();

   NeedCallback();

   return 0;
}
