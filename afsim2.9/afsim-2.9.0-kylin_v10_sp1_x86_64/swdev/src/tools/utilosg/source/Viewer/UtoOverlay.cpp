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
// OverlayImp.cpp : implementation file

#include "UtoOverlay.hpp"

#include <cassert>

#include <osg/LineSegment>
#include <osg/Switch>
#include <osgUtil/IntersectVisitor>

#include "UtoEntity.hpp"
#include "UtoGroupShape.hpp"
#include "UtoHittestUtils.hpp"
#include "UtoShape.hpp"
#include "UtoShapeImp.hpp"
#include "UtoSmallObj.hpp"
#include "UtoWorld.hpp"

/////////////////////////////////////////////////////////////////////////////

// {secret}
struct UtoOverlayIterator_i : public Loki::SmallObject<>
{
   UtoOverlay::ShapeMap::iterator m_Iter;
   char                           m_Memory[sizeof(Utok::NameShape)];
   Utok::NameShape*               m_pNameShape;

   UtoOverlayIterator_i()
      : /* m_Iter(0), PMG CANNOT INIT AN ITERATOR TO NULL*/ m_pNameShape(nullptr)
   {
   }
   UtoOverlayIterator_i(UtoOverlay::ShapeMap::iterator i)
      : m_Iter(i)
      , m_pNameShape(nullptr)
   {
   }
   UtoOverlayIterator_i(const UtoOverlayIterator_i& rhs)
      : m_Iter(rhs.m_Iter)
      , m_pNameShape(nullptr)
   {
      if (rhs.m_pNameShape)
      {
         m_pNameShape = new (m_Memory) Utok::NameShape(m_Iter->first, *m_Iter->second);
      }
   }
};

/////////////////////////////////////////////////////////////////////////////

UtoOverlay::UtoOverlay(UtoWorld* pWorld, const std::string& aName)
   : m_RootNode(new osg::Group)
   , m_pTransform(new osg::MatrixTransform)
   , m_BoundDirty(false)
   , m_World(pWorld)
   , m_AttachedEntity(nullptr)
{
   m_RootNode->setName(aName);
   m_RootNode->addChild(m_pTransform);
}

UtoOverlay::~UtoOverlay()
{
   RemoveAll();
}

#include <iostream>
void UtoOverlay::Show(bool show)
{
   // the parent of our root is a switch we can use to show or hide this branch
   osg::Switch* Switch = dynamic_cast<osg::Switch*>(m_RootNode->getParent(0));
   if (Switch)
      Switch->setChildValue(m_RootNode.get(), show);
   else
      assert(0);
}

bool UtoOverlay::IsVisible() const
{
   bool visible = false;

   // the parent of our root is a switch we can use to show or hide this branch
   const osg::Switch* Switch = dynamic_cast<const osg::Switch*>(m_RootNode->getParent(0));
   if (Switch)
   {
      visible = Switch->getChildValue(m_RootNode.get());
   }
   return visible;
}

UtoShape* UtoOverlay::Bind(const std::string& name, const UtoShape& shape)
{
   UtoShape*& rShapePtr = m_Shapes[name];
   if (!rShapePtr)
   {
      rShapePtr           = shape.Clone();
      UtoShapeImp* pShape = rShapePtr->Impl();
      // Insert the shape in to the scene graph
      // osg::Node* subgraph = pShape->GetTree();
      // subgraph->setName(name);
      pShape->BindToViewer(m_World->GetViewer(), m_pTransform);
      // m_pTransform->addChild(subgraph);
   }

   return rShapePtr;
}

bool UtoOverlay::Unbind(const std::string& name)
{
   ShapeMap::iterator i = m_Shapes.find(name);
   if (i != m_Shapes.end())
   {
      i->second->UnbindViewers();
      delete i->second;
      m_Shapes.erase(i);
      return true;
   }
   return false;
}

UtoShape* UtoOverlay::Lookup(const std::string& name)
{
   ShapeMap::iterator i = m_Shapes.find(name);
   if (i != m_Shapes.end())
   {
      return i->second;
   }
   return nullptr;
}

bool UtoOverlay::Rebind(const std::string& name, const UtoShape& shape)
{
   Unbind(name);
   return Bind(name, shape) != nullptr;
}

void UtoOverlay::Dump(std::ostream& o)
{
   o << "Shape Dump" << std::endl;
   UtoOverlay::ShapeMap::iterator it(m_Shapes.begin()), end(m_Shapes.end());
   if (it == end)
   {
      o << "  None" << std::endl;
   }
   else
      for (; it != end; ++it)
      {
         o << "  Name: " << it->first << std::endl;
         it->second->Dump(o);
         o << std::endl;
      }
}

void UtoOverlay::RemoveAll()
{
   ShapeMap           copy = m_Shapes;
   ShapeMap::iterator End(copy.end());
   ShapeMap::iterator ShapeIt(copy.begin());

   for (; ShapeIt != End; ++ShapeIt)
   {
      Unbind(ShapeIt->first);
   }
}

int UtoOverlay::HitTest(const UtoPoint& point, HitTestCallback& callback)
{
   int nHitCount = 0;

   ShapeMap::iterator shapeIt = m_Shapes.begin();

   bool continueTesting = true;

   // if there are no shapes in this overlay return error
   if (shapeIt == m_Shapes.end())
   {
      nHitCount = -1;
   }

   // do intersection test to see if anything was hit
   osg::Vec3 userStart(point.x, point.y, 1000.0f);
   osg::Vec3 userEnd(point.x, point.y, -1000.0f);

   osg::ref_ptr<osg::LineSegment> userLS = new osg::LineSegment(userStart, userEnd);

   osgUtil::IntersectVisitor iv;

   iv.addLineSegment(userLS.get());

   // put intersect visitor in
   m_pTransform->accept(iv);

   if (iv.hits())
   {
      // found something, get the list
      osgUtil::IntersectVisitor::HitList hl = iv.getHitList(userLS.get());
      if (!hl.empty())
      {
         // for all of the hitlist hits
         osgUtil::IntersectVisitor::HitList::iterator hitItr = hl.begin();

         osg::Vec3 lastHitPoint(0.0f, 0.0f, 0.0f);

         while (hitItr != hl.end() && continueTesting)
         {
            osg::ref_ptr<osg::Drawable> refDrawable = hitItr->_drawable;
            osg::ref_ptr<osg::Geode>    refGeode    = hitItr->_geode;
            osg::Vec3                   hitPoint    = hitItr->_intersectPoint;

            if (hitPoint.x() != lastHitPoint.x() || hitPoint.y() != lastHitPoint.y())
            {
               shapeIt = m_Shapes.begin();

               bool bShapeFound = false;

               while (shapeIt != m_Shapes.end() && !bShapeFound)
               {
                  // check to see if it's a group shape
                  UtoGroupShape* pGroupShape = dynamic_cast<UtoGroupShape*>(shapeIt->second);

                  if (!pGroupShape)
                  {
                     // it's not a group shape, just a regular shape
                     UtoShape* pShape = shapeIt->second;

                     UtoPosition attachedEntityOffset(0.0, 0.0, 0.0);

                     osg::Vec3 start, end;

                     if (m_AttachedEntity)
                     {
                        double xyz[3];
                        m_World->Projection().Convert(m_AttachedEntity->BodyCF(), xyz);

                        attachedEntityOffset = osg::Vec3d(xyz[0], xyz[1], xyz[2]);

                        m_World->WorldToWindow(attachedEntityOffset);

                        start.set(pShape->Position().x() + attachedEntityOffset.x(),
                                  pShape->Position().y() + attachedEntityOffset.y(),
                                  1000.0f);
                        end.set(pShape->Position().x() + attachedEntityOffset.x(),
                                pShape->Position().y() + attachedEntityOffset.y(),
                                -1000.0f);
                     }
                     else
                     {
                        start.set(pShape->Position().x(), pShape->Position().y(), 1000.0f);
                        end.set(pShape->Position().x(), pShape->Position().y(), -1000.0f);
                     }

                     // do another intersection test from the shape's position and see if the hit is the same shape
                     osg::ref_ptr<osg::LineSegment> ls = new osg::LineSegment(start, end);
                     osgUtil::IntersectVisitor      newIV;
                     newIV.addLineSegment(ls.get());
                     m_pTransform->accept(newIV);
                     osgUtil::IntersectVisitor::HitList newHL = newIV.getHitList(ls.get());

                     // compare the drawables to see if this is the object that was hit
                     if (!newHL.empty())
                     {
                        if (refDrawable.get() == newHL.front()._drawable.get() &&
                            refGeode.get() == newHL.front()._geode.get())
                        {
                           // test to see if the geometry hit is a texture that has see-through parts
                           TexelIntersectPolicy           tip;
                           osg::ref_ptr<osg::LineSegment> linseg =
                              new osg::LineSegment(osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() + 1.0f),
                                                   osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() - 1.0f));
                           bool result = tip.process_hit(*linseg.get(), *refDrawable.get());

                           if (result)
                           {
                              // if it was, call the user callback on it
                              continueTesting = callback.OnHit(*pShape);
                              nHitCount++;
                           }

                           bShapeFound = true;
                        }
                     }
                  }
                  else
                  {
                     // recursively handle group shapes
                     std::vector<std::pair<std::string, UtoShape*>> shapeVec;
                     GetShapesFromGroup(pGroupShape, shapeVec);

                     std::vector<std::pair<std::string, UtoShape*>>::iterator itr = shapeVec.begin();
                     while (itr != shapeVec.end() && !bShapeFound)
                     {
                        UtoShape* pShape = itr->second;

                        UtoPosition attachedEntityOffset(0.0, 0.0, 0.0);

                        osg::Vec3 start, end;

                        if (m_AttachedEntity)
                        {
                           double xyz[3];
                           m_World->Projection().Convert(m_AttachedEntity->BodyCF(), xyz);

                           attachedEntityOffset = osg::Vec3d(xyz[0], xyz[1], xyz[2]);

                           m_World->WorldToWindow(attachedEntityOffset);

                           start.set(pShape->Position().x() + attachedEntityOffset.x(),
                                     pShape->Position().y() + attachedEntityOffset.y(),
                                     1000.0f);
                           end.set(pShape->Position().x() + attachedEntityOffset.x(),
                                   pShape->Position().y() + attachedEntityOffset.y(),
                                   -1000.0f);
                        }
                        else
                        {
                           start.set(pShape->Position().x(), pShape->Position().y(), 1000.0f);
                           end.set(pShape->Position().x(), pShape->Position().y(), -1000.0f);
                        }

                        // do another intersection test from the shape's position and see if the hit is the same shape
                        osg::ref_ptr<osg::LineSegment> ls = new osg::LineSegment(start, end);
                        osgUtil::IntersectVisitor      newIV;
                        newIV.addLineSegment(ls.get());
                        m_pTransform->accept(newIV);
                        osgUtil::IntersectVisitor::HitList newHL = newIV.getHitList(ls.get());

                        // compare the drawables to see if this is the object that was hit
                        if (!newHL.empty())
                        {
                           if (refDrawable.get() == newHL.front()._drawable.get() &&
                               refGeode.get() == newHL.front()._geode.get())
                           {
                              // test to see if the geometry hit is a texture that has see-through parts
                              TexelIntersectPolicy           tip;
                              osg::ref_ptr<osg::LineSegment> linseg =
                                 new osg::LineSegment(osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() + 1.0f),
                                                      osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() - 1.0f));
                              bool result = tip.process_hit(*linseg.get(), *refDrawable.get());

                              if (result)
                              {
                                 // if it was, call the user callback on it
                                 continueTesting = callback.OnHit(*pShape);
                                 nHitCount++;
                              }

                              bShapeFound = true;
                           }
                        }

                        ++itr;
                     }
                  }

                  ++shapeIt;
               }
            }

            // save to get rid of duplicate hits
            lastHitPoint = hitPoint;

            ++hitItr;
         }
      }
   }

   return nHitCount;
}

int UtoOverlay::HitTest(const UtoPoint& point, HitTestCallback& callback, const std::string& sNameFilter)
{
   int nHitCount = 0;

   ShapeMap::iterator shapeIt = m_Shapes.begin();

   bool continueTesting = true;

   // if there are no shapes in this overlay return error
   if (shapeIt == m_Shapes.end())
   {
      nHitCount = -1;
   }

   // do intersection test to see if anything was hit
   osg::Vec3 userStart(point.x, point.y, 1000.0f);
   osg::Vec3 userEnd(point.x, point.y, -1000.0f);

   osg::ref_ptr<osg::LineSegment> userLS = new osg::LineSegment(userStart, userEnd);

   osgUtil::IntersectVisitor iv;

   iv.addLineSegment(userLS.get());

   // put intersect visitor in
   m_pTransform->accept(iv);

   if (iv.hits())
   {
      // found something, get the list
      osgUtil::IntersectVisitor::HitList hl = iv.getHitList(userLS.get());
      if (!hl.empty())
      {
         // for all of the hitlist hits
         osgUtil::IntersectVisitor::HitList::iterator hitItr = hl.begin();

         osg::Vec3 lastHitPoint(0.0f, 0.0f, 0.0f);

         while (hitItr != hl.end() && continueTesting)
         {
            osg::ref_ptr<osg::Drawable> refDrawable = hitItr->_drawable;
            osg::ref_ptr<osg::Geode>    refGeode    = hitItr->_geode;
            osg::Vec3                   hitPoint    = hitItr->_intersectPoint;

            if (hitPoint.x() != lastHitPoint.x() || hitPoint.y() != lastHitPoint.y())
            {
               shapeIt = m_Shapes.begin();

               bool bShapeFound = false;

               while (shapeIt != m_Shapes.end() && !bShapeFound)
               {
                  // check to see if it's a group shape
                  UtoGroupShape* pGroupShape = dynamic_cast<UtoGroupShape*>(shapeIt->second);

                  if (!pGroupShape)
                  {
                     UtoShape* pShape = shapeIt->second;

                     UtoPosition attachedEntityOffset(0.0, 0.0, 0.0);

                     osg::Vec3 start, end;

                     if (m_AttachedEntity)
                     {
                        double xyz[3];
                        m_World->Projection().Convert(m_AttachedEntity->BodyCF(), xyz);

                        attachedEntityOffset = osg::Vec3d(xyz[0], xyz[1], xyz[2]);

                        m_World->WorldToWindow(attachedEntityOffset);

                        start.set(pShape->Position().x() + attachedEntityOffset.x(),
                                  pShape->Position().y() + attachedEntityOffset.y(),
                                  1000.0f);
                        end.set(pShape->Position().x() + attachedEntityOffset.x(),
                                pShape->Position().y() + attachedEntityOffset.y(),
                                -1000.0f);
                     }
                     else
                     {
                        start.set(pShape->Position().x(), pShape->Position().y(), 1000.0f);
                        end.set(pShape->Position().x(), pShape->Position().y(), -1000.0f);
                     }

                     // do another intersection test from the shape's position and see if the hit is the same shape
                     osg::ref_ptr<osg::LineSegment> ls = new osg::LineSegment(start, end);
                     osgUtil::IntersectVisitor      newIV;
                     newIV.addLineSegment(ls.get());
                     m_pTransform->accept(newIV);
                     osgUtil::IntersectVisitor::HitList newHL = newIV.getHitList(ls.get());

                     // compare the drawables to see if this is the object that was hit
                     if (!newHL.empty())
                     {
                        if (refDrawable.get() == newHL.front()._drawable.get() &&
                            refGeode.get() == newHL.front()._geode.get())
                        {
                           // test to see if the geometry hit is a texture that has see-through parts
                           TexelIntersectPolicy           tip;
                           osg::ref_ptr<osg::LineSegment> linseg =
                              new osg::LineSegment(osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() + 1.0f),
                                                   osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() - 1.0f));
                           bool result = tip.process_hit(*linseg.get(), *refDrawable.get());

                           if (result)
                           {
                              if (sNameFilter.find(shapeIt->first) != std::string::npos)
                              {
                                 // if it's in the name filter, call the callback on it
                                 continueTesting = callback.OnHit(*pShape);
                                 nHitCount++;
                              }
                           }

                           bShapeFound = true;
                        }
                     }
                  }
                  else
                  {
                     // recursively handle group shapes
                     std::vector<std::pair<std::string, UtoShape*>> shapeVec;
                     GetShapesFromGroup(pGroupShape, shapeVec);

                     std::vector<std::pair<std::string, UtoShape*>>::iterator itr = shapeVec.begin();
                     while (itr != shapeVec.end() && !bShapeFound)
                     {
                        UtoShape* pShape = itr->second;

                        UtoPosition attachedEntityOffset(0.0, 0.0, 0.0);

                        osg::Vec3 start, end;

                        if (m_AttachedEntity)
                        {
                           double xyz[3];
                           m_World->Projection().Convert(m_AttachedEntity->BodyCF(), xyz);

                           attachedEntityOffset = osg::Vec3d(xyz[0], xyz[1], xyz[2]);

                           m_World->WorldToWindow(attachedEntityOffset);

                           start.set(pShape->Position().x() + attachedEntityOffset.x(),
                                     pShape->Position().y() + attachedEntityOffset.y(),
                                     1000.0f);
                           end.set(pShape->Position().x() + attachedEntityOffset.x(),
                                   pShape->Position().y() + attachedEntityOffset.y(),
                                   -1000.0f);
                        }
                        else
                        {
                           start.set(pShape->Position().x(), pShape->Position().y(), 1000.0f);
                           end.set(pShape->Position().x(), pShape->Position().y(), -1000.0f);
                        }

                        // do another intersection test from the shape's position and see if the hit is the same shape
                        osg::ref_ptr<osg::LineSegment> ls = new osg::LineSegment(start, end);
                        osgUtil::IntersectVisitor      newIV;
                        newIV.addLineSegment(ls.get());
                        m_pTransform->accept(newIV);
                        osgUtil::IntersectVisitor::HitList newHL = newIV.getHitList(ls.get());

                        // compare the drawables to see if this is the object that was hit
                        if (!newHL.empty())
                        {
                           if (refDrawable.get() == newHL.front()._drawable.get() &&
                               refGeode.get() == newHL.front()._geode.get())
                           {
                              // test to see if the geometry hit is a texture that has see-through parts
                              TexelIntersectPolicy           tip;
                              osg::ref_ptr<osg::LineSegment> linseg =
                                 new osg::LineSegment(osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() + 1.0f),
                                                      osg::Vec3(hitPoint.x(), hitPoint.y(), hitPoint.z() - 1.0f));
                              bool result = tip.process_hit(*linseg.get(), *refDrawable.get());

                              if (result)
                              {
                                 if (sNameFilter.find(itr->first) != std::string::npos)
                                 {
                                    // if it's in the name filter, call the callback on it
                                    continueTesting = callback.OnHit(*pShape);
                                    nHitCount++;
                                 }
                              }

                              bShapeFound = true;
                           }
                        }

                        ++itr;
                     }
                  }

                  ++shapeIt;
               }
            }

            // save to get rid of duplicate hits
            lastHitPoint = hitPoint;

            ++hitItr;
         }
      }
   }

   return nHitCount;
}

void UtoOverlay::GetShapesFromGroup(UtoGroupShape* pGroupShape, std::vector<std::pair<std::string, UtoShape*>>& vec)
{
   const UtoShapeMap& shapes = pGroupShape->Shapes();
   for (UtoShapeMap::const_iterator itr = shapes.begin(); itr != shapes.end(); ++itr)
   {
      // if the shape is another group shape, recurse
      UtoGroupShape* pGroupTest = dynamic_cast<UtoGroupShape*>(itr->second);

      if (pGroupTest)
      {
         // recurse
         GetShapesFromGroup(pGroupTest, vec);
      }
      else
      {
         // put the shape in the vector
         std::pair<std::string, UtoShape*> tmp;
         tmp.first  = itr->first;
         tmp.second = itr->second;
         vec.push_back(tmp);
      }
   }
}

bool UtoOverlay::Attach(UtoEntity& ent)
{
   Detach();

   if (m_World)
   {
      ent.connect(UtoEntity::PositionChangedEvent(), this, &UtoOverlay::EntityMoved);
      ent.connect(UtoEntity::PosnOrientChangedEvent(), this, &UtoOverlay::EntityMoved);
      ent.connect(UtoEntity::DestroyedEvent(), this, &UtoOverlay::EntityDestroyed);

      EntityMoved(&ent);

      m_AttachedEntity = &ent;
      return true;
   }
   return false;
}

bool UtoOverlay::Detach()
{
   if (m_AttachedEntity)
   {
      m_AttachedEntity->disconnect(UtoEntity::PositionChangedEvent(), this, &UtoOverlay::EntityMoved);
      m_AttachedEntity->disconnect(UtoEntity::PosnOrientChangedEvent(), this, &UtoOverlay::EntityMoved);
      m_AttachedEntity->disconnect(UtoEntity::DestroyedEvent(), this, &UtoOverlay::EntityDestroyed);

      m_AttachedEntity = nullptr;
   }
   return true;
}

void UtoOverlay::EntityMoved(UtoEntity* entity)
{
   assert(m_World);

   double xyz[3];
   m_World->Projection().Convert(entity->BodyCF(), xyz);

   UtoPosition spos(xyz[0], xyz[1], xyz[2]);

   m_World->WorldToWindow(spos);

   osg::Matrix m;
   m.setTrans(spos.x(), spos.y(), 0);
   m_pTransform->setMatrix(m);
}

void UtoOverlay::EntityDestroyed(UtoEntity* entity)
{
   assert(entity == m_AttachedEntity);

   Detach();
}


///////////////////////////////////////////////////////////////////////////////
//// UtoOverlay.cpp : implementation file
//
//#include "StdAfx.h"
//#include "UtoOverlay.hpp"
//#include "UtoOverlay.hpp"
//#include "UtoShape.hpp"
//
///**
//\ingroup view
//\class UtoOverlay
//\brief Provides access to overlays on UtoWindow and UtoWorld.
//
// An overlay object is accessed from UtoWindow or UtoWorld, and is never
// created directly by the user.  The overlay provides management of shapes
// for the window or world object.  Overlays are used to show visual reference
// information and enhancements.  Examples include a world grid, entity
// locator arrow, and a Boeing logo.
//
// The overlay provides Bind, Rebind, Unbind, and Lookup methods for shape
// operations.  The shapes can be accessed through iterators, and the Shapes
// method returns the begin and end iterators.  A RemoveAll method exists to
// clear out all shapes on the overlay.  Lastly the overlay can be hidden or
// visible using the Show method.
//*/
// class UtoOverlay;
//
///////////////////////////////////////////////////////////////////////////////
//
///** @name Destructors */
////@{
///**
//\fn UtoOverlay::~UtoOverlay( )
//\brief Destroys the object.
//
// This destroys all the subordinate objects for this object.
//*/
////@}
//
///** @name Type Definitions */
////@{
///**
//\typedef UtoOverlay::Utok::Iterator<UtoOverlayIteratorPolicies> UtoOverlay::iterator
//\brief Provides access to a Utok::NameShape.
//*/
//
///**
//\typedef UtoOverlay::Utok::IteratorPair<iterator> UtoOverlay::iterator_pair
//\brief Provides access to an iterator.
//*/
////@}
//
///** @name Accessors */
////@{
//
///**
//\fn bool UtoOverlay::IsVisible() const
//\brief Indicates the visibility of the overlay.
//
// Indicates the visibility of the overlay.
//\return bool - <table>
//                  <tr><td>true</td><td>Overlay is visible.</td></tr>
//                  <tr><td>false</td><td>Overlay is not visible.</td></tr>
//               </table>
//*/
//
///**
//\fn UtoOverlay::iterator UtoOverlay::Lookup (const std::string& name)
//\brief References a bound shape within the overlay.
//
// References a bound shape within the overlay.
//\param name the name of the bound shape.
//\return UtoOverlay::iterator - an iterator referencing the bound shape.
//*/
//
///**
//\fn UtoOverlay::iterator_pair UtoOverlay::Shapes ()
//\brief Returns begin & end iterators for the bound shapes.
//
// Returns begin & end iterators for the bound shapes.
//\return UtoOverlay::iterator_pair - references to begin & end iterators.
//*/
////@}
//
///** @name Operations */
////@{
//
///**
//\fn void UtoOverlay::Show (bool show)
//\brief Sets the overlay visibility.
//
// Sets the overlay visibility.  Overlays may be visible or not.  If an overlay is
// visible, then each shape is also visible; otherwise, all overlay shapes are hidden.
//\param show &nbsp; <table>
//                     <tr><td>true</td><td>Show overlay.</td></tr>
//                     <tr><td>false</td><td>Hide overlay.</td></tr>
//                   </table>
//*/
//
///**
//\fn UtoOverlay::iterator UtoOverlay::Bind (const std::string& name, const UtoShape& shape)
//\brief Binds a shape to the specified name.
//
// Binds a shape to the specified name.  This function will create a copy of the shape
// object bound by the specified name.
//\param name the name bound to the given shpae.
//\param shape the shape bound to the given name.
//\return UtoOverlay::iterator - an iterator that references the bound shape.
//*/
//
///**
//\fn bool UtoOverlay::Unbind (const std::string& name)
//\brief Removes a shape bound to the specified name.
//
// Removes a shape bound to the specified name.  The shape bound will be destroyed.
//\param name the name of the shape bound to the overlay.
//\return bool - <table>
//                  <tr><td>true</td><td>Shape unbound.</td></tr>
//                  <tr><td>false</td><td>Shape not unbound.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoOverlay::Rebind (const std::string& name, const UtoShape& shape)
//\brief Rebinds a shape to the specified name.
//
// Rebinds a shape to the specified name.
//
// Destroys the current shape bound to the name if any binding currenty
// exists.  Any iterators referencing this binding become invalid, all
// other iterators remain valid.  Use this method when the shape needs to
// change type.  Otherwise use the existing shape to make modifications to
// the properties or geometry of the shape.
//\param name the name bound to the given shpae.
//\param shape the shape bound to the given name.
//\return bool - <table>
//                  <tr><td>true</td><td>Shape rebound.</td></tr>
//                  <tr><td>false</td><td>Shape not rebound.</td></tr>
//               </table>
//*/
//
///**
//\fn void UtoOverlay::RemoveAll()
//\brief Removes all of the shape from the overlay.
//
// Removes all of the shape from the overlay.
//*/
//
///**
//\fn int UtoOverlay::HitTest( const UtoPoint& point, HitTestCallback& callback )
//\brief Performs a hittest on the overlay shapes
//
// Goes through all shapes in the overlay to see if any are hit by POINT
// and if any are hit, calls the user callback object's 'OnHit' function on those shapes. The callback
// can then return whether or not to continue searching. Shapes are tested
// by finding the bounding sphere of a shape and then comparing those bounds
// to the POINT that was passed in.
//
//\param UtoPoint& - the point to see if there are shapes under
//\param HitTestCallback& - object with a defined OnHit function in it
//\return int - the number of shapes hit(if the user cancels the search it will be smaller)
//*/
//
///**
//\fn int UtoOverlay::HitTest( const UtoPoint& point, HitTestCallback& callback, const std::string& sNameFilter )
//\brief Performs a hittest on the overlay shapes with a certain name
//
// Goes through all shapes with names in the comma-delimited name filter string to see if any are hit by POINT
// and if any are hit, calls the user callback object's 'OnHit' function on those shapes. The callback
// can then return whether or not to continue searching. Shapes are tested
// by finding the bounding sphere of a shape and then comparing those bounds
// to the POINT that was passed in.
//
//\param UtoPoint& - the point to see if there are shapes under
//\param HitTestCallback& - object with a defined OnHit function in it
//\param string& - a comma delimited string containing names of shapes to look for
//\return int - the number of shapes hit(if the user cancels the search it will be smaller)
//*/
//
///**
//\fn bool Attach( UtoEntity& ent )
//\brief This function attaches the overlay to an entity after which they both move together
//
//\param UtoEntity& - refernce to entity
//\return bool - success or not
//*/
//
///**
//\fn bool Detach()
//\brief This function detaches the overlay from the entity it was attached to previously
//
//\return bool - success or not
//*/
