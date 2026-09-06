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
// UtoWorldImp.hpp : header file

#if !defined(_UTOWORLDIMP_HPP_)
#define _UTOWORLDIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include "UtoMapProjection.hpp"
#include "UtoOverlayManager.hpp"
#include "UtoTypes.hpp"

namespace osg
{
class Switch;
}
namespace osgDB
{
class DatabasePager;
}

class UtoViewer;
class UtoTerrain;
class UtoViewerProjection;
class UtoCamera;
class UtoShape;

class UtoWorld
{
public:
   // public typedefs
   typedef UtoOverlayManager::iterator overlay_iterator;


   UtoWorld(UtoViewer*, osgDB::DatabasePager* pgr);
   virtual ~UtoWorld();

   virtual bool        IsTerrainValid();
   virtual UtoTerrain& Terrain() const { return *m_Terrain; }
   virtual bool        SetTerrain(UtoTerrain& Terrain);
   virtual bool        SetTerrain(const std::string& name);
   virtual void        ClearTerrain();
   virtual void        ShowTerrain(bool bShow = true);
   virtual bool        TerrainIsVisible() const
   {
      return m_TerrainNode.get() && mTerrainTransform->containsNode(m_TerrainNode.get());
   };

   virtual bool                    ClearProjection();
   virtual bool                    SetCME_WCS_Projection();
   virtual const UtoMapProjection& Projection() const;

   virtual overlay_iterator           AddOverlay(const std::string& name);
   virtual overlay_iterator           InsertOverlay(const std::string& name, overlay_iterator it);
   virtual void                       RemoveOverlay(overlay_iterator remove);
   virtual void                       RemoveAllOverlays();
   virtual UtoWorld::overlay_iterator FindOverlay(const std::string& name);
   const UtoOverlayManager&           Overlays() { return m_Overlays; }

   virtual bool WorldToWindow(UtoPosition& WorldPosition);
   virtual bool WorldToWindowUnlimited(UtoPosition& WorldPosition);

   void SetProjection(UtoViewer* viewer);
   void UpdateOffsets();

   osg::Node* GetOverlayTree() { return mOverlayTransform.get(); }
   osg::Node* GetTerrainTree() { return mTerrainTransform.get(); }

   UtoViewer& GetViewer() { return *m_Viewer; }

protected:
private:
   UtoWorld();
   UtoWorld(const UtoWorld&);
   UtoWorld& operator=(const UtoWorld&);

   bool UpdateViewport();

   UtoOverlayManager m_Overlays;

   UtoTerrain*          m_Terrain{nullptr};
   UtoMapProjection     m_pCoordSys;
   UtoViewerProjection* m_Projection{nullptr};
   UtoViewer*           m_Viewer;

   osg::ref_ptr<osg::MatrixTransform> mOverlayTransform;
   osg::ref_ptr<osg::MatrixTransform> mTerrainTransform;
   osg::ref_ptr<osg::Switch>          m_Switch;
   osg::ref_ptr<osgDB::DatabasePager> m_DatabasePager;
   osg::ref_ptr<osg::Node>            m_TerrainNode;
};

inline const UtoMapProjection& UtoWorld::Projection() const
{
   return m_pCoordSys;
}

#endif // !defined(_UTOWORLDIMP_HPP_)
