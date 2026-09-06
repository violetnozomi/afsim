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
// UtoOverlayImp.hpp : header file

#if !defined(_UTOOVERLAYIMP_HPP_)
#define _UTOOVERLAYIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <map>
#include <vector>

#include <osg/ref_ptr>

#include "UtoOverlay.hpp"
#include "UtoTypes.hpp"

class UtoShape;
class UtoWorld;
class UtoEntity;
class UtoGroupShape;

namespace osg
{
class Group;
class MatrixTransform;
} // namespace osg

class UtoOverlay
{
public:
   typedef std::map<std::string, UtoShape*> ShapeMap;
   typedef ShapeMap::iterator               iterator;

   class HitTestCallback
   {
   public:
      virtual ~HitTestCallback() {}
      virtual bool OnHit(UtoShape& shape) = 0;
   };

   UtoOverlay(UtoWorld* pWorld, const std::string& aName = std::string());
   virtual ~UtoOverlay();

   virtual void      Show(bool show);
   virtual UtoShape* Bind(const std::string& name, const UtoShape& shape);
   virtual bool      Unbind(const std::string& name);
   virtual UtoShape* Lookup(const std::string& name);
   virtual bool      Rebind(const std::string& name, const UtoShape& shape);
   virtual void      RemoveAll();
   const ShapeMap&   Shapes() { return m_Shapes; }
   virtual bool      IsVisible() const;
   osg::Group*       GetTree() { return m_RootNode.get(); }

   virtual void Dump(std::ostream&);

   int HitTest(const UtoPoint& point, HitTestCallback& callback);
   int HitTest(const UtoPoint& point, HitTestCallback& callback, const std::string& sNameFilter);

   virtual bool Attach(UtoEntity& ent);
   virtual bool Detach();
   void         EntityMoved(UtoEntity* entity);
   void         EntityDestroyed(UtoEntity* entity);

protected:
private:
   void CreateTree();
   void GetShapesFromGroup(UtoGroupShape* pGroupShape, std::vector<std::pair<std::string, UtoShape*>>& vec);

   UtoOverlay(const UtoOverlay&);
   UtoOverlay& operator=(const UtoOverlay&);

   ShapeMap                 m_Shapes;
   osg::ref_ptr<osg::Group> m_RootNode;
   osg::MatrixTransform*    m_pTransform;
   bool                     m_BoundDirty;
   UtoWorld*                m_World;
   UtoEntity*               m_AttachedEntity;
};

#endif // !defined(_UTOOVERLAYIMP_HPP_)
