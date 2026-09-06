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
// UtoWorldImp.cpp : implementation file

#include <assert.h>

#include <osg/Switch>
#include <osgDB/DatabasePager>

//////////////////////////////////////////////////////////////////////////
#include <osg/TexEnv>
#include <osg/TexGenNode>
#include <osg/Texture2D>
//////////////////////////////////////////////////////////////////////////

#include "UtMath.hpp"
#include "UtoOverlay.hpp"
#include "UtoPriv.hpp"
#include "UtoResourceDB.hpp"
#include "UtoShape.hpp"
#include "UtoShapeImp.hpp"
#include "UtoTerrain.hpp"
#include "UtoTerrainImp.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerProjection.hpp"
#include "UtoWorld.hpp"

extern const char* g_CoordSystemNames[];

/* UTO Function Block **********************************************************

Function: UtoWorldImp

Description:
Constructor

*******************************************************************************/

UtoWorld::UtoWorld(UtoViewer* viewer, osgDB::DatabasePager* pgr)
   : m_Viewer(viewer)
   , mOverlayTransform(new osg::MatrixTransform)
   , mTerrainTransform(new osg::MatrixTransform)
   , m_Switch(new osg::Switch)
   , m_DatabasePager(pgr)
{
   // the transform node is use to offset the camera position changes
   m_Switch->setName("Overlay Switch");
   mOverlayTransform->addChild(m_Switch.get());
   mOverlayTransform->setName("World Overlays");
   mTerrainTransform->setName("Terrain Transform");

   // need to get updates to the projection
   viewer->connect(UtoViewer::ProjectionChangedEvent(), this, &UtoWorld::SetProjection);
}

/* UTO Function Block **********************************************************

Function: UtoWorldImp

Description:
Destructor

*******************************************************************************/

UtoWorld::~UtoWorld()
{
   ClearTerrain();
   RemoveAllOverlays();

   m_Viewer->disconnect(UtoViewer::ProjectionChangedEvent(), this, &UtoWorld::SetProjection);
}

/* UTO Function Block **********************************************************

Function: SetTerrain

Description:

*******************************************************************************/

bool UtoWorld::SetTerrain(UtoTerrain& Terrain)
{
   // determine the current state of the terrain
   bool bTerrainVisible = TerrainIsVisible() || !m_TerrainNode.get();

   if (m_TerrainNode.get())
   {
      ClearTerrain();
   }

   // clear terrain should clear this up
   assert(!m_Terrain);

   // clone the terrain
   m_Terrain = Terrain.Clone();

   // Add the terrain to the world
   m_TerrainNode = m_Terrain->Impl()->AddToWorld(*m_Viewer);
   if (!m_TerrainNode.get())
   {
      return false;
   }
   mTerrainTransform->addChild(m_TerrainNode.get());
   m_DatabasePager->registerPagedLODs(m_TerrainNode.get());

   // clear the terrain projection
   ClearProjection();

   // reset the current visible state of the terrain
   ShowTerrain(bTerrainVisible);

   return true;
}

bool UtoWorld::SetTerrain(const std::string& name)
{
   UtoResourceDB&                  rdb       = UtoResourceDB::Instance();
   UtoResourceDB::terrain_iterator itTerrain = rdb.FindTerrain(name);
   if (itTerrain != rdb.Terrains().second)
   {
      return SetTerrain(*itTerrain->second);
   }
   return false;
}

void UtoWorld::ClearTerrain()
{
   if (m_TerrainNode.get())
   {
      m_Terrain->Impl()->RemoveFromWorld(*m_Viewer);

      // Remove the terrain to the world
      mTerrainTransform->removeChild(m_TerrainNode.get());

      // clear the terrain
      if (m_Terrain)
         delete m_Terrain;

      m_Terrain     = nullptr;
      m_TerrainNode = nullptr;

      m_pCoordSys.projection = UtoMapProjection::Geocentric;
      m_Viewer->SetProjection(m_pCoordSys);
   }
   else
   {
      if (m_Terrain)
      {
         m_Terrain->Impl()->RemoveFromWorld(*m_Viewer);
         delete m_Terrain;
         m_Terrain = nullptr;
      }
   }
}

void UtoWorld::ShowTerrain(bool bShow)
{
   // make sure there is a valid terrain
   if (m_TerrainNode.get())
   {
      // determine if the node is attached to the main transform
      bool bAttached = mTerrainTransform->containsNode(m_TerrainNode.get());

      if (bAttached && !bShow)
      {
         // hiding the terrain
         // remove the node from the main transform
         mTerrainTransform->removeChild(m_TerrainNode.get());
      }
      else if (!bAttached && bShow)
      {
         // showing the terrain
         // attach the node to the main transform
         mTerrainTransform->addChild(m_TerrainNode.get());
      }
   }
}

/* UTO Function Block **********************************************************

Function: ClearProjection

Description:
Clears the current map projection

*******************************************************************************/

bool UtoWorld::ClearProjection()
{
   // resets to terrain projection, or none.
   m_pCoordSys = m_Terrain->Impl()->Projection();
   m_Viewer->SetProjection(m_pCoordSys);

   return true;
}

/* UTO Function Block **********************************************************

Function: SetCME_WCS_Projection

Description:
Sets the CME World coordinate system projection

*******************************************************************************/

bool UtoWorld::SetCME_WCS_Projection()
{
   // replaces terrain projection with user specified one.
   m_pCoordSys.projection = UtoMapProjection::Geocentric;
   m_Viewer->SetProjection(m_pCoordSys);
   return true;
}

/* UTO Function Block **********************************************************

Function: AddOverlay

Description:
Adds an overlay to the world
- an overlay is created using the OverlayManager
- the root of that overlay is returned using the GetTree function on
the overlay
- the root node is added to the overlay switch

*******************************************************************************/

UtoWorld::overlay_iterator UtoWorld::AddOverlay(const std::string& name)
{
   UtoOverlayManager::OverlayList::iterator OverlayIt;
   UtoOverlayManager::IteratorPair          OverlayItPair = m_Overlays.Overlays();

   // Call the base class and store the iterator
   OverlayIt = m_Overlays.AddOverlay(name, this);

   if (OverlayIt != OverlayItPair.second)
   {
      UtoOverlay* NewOverlay = dynamic_cast<UtoOverlay*>(OverlayIt->overlay);

      // Add the overlay to the scene
      m_Switch->addChild(NewOverlay->GetTree());
   }

   return OverlayIt;
}

/* UTO Function Block **********************************************************

Function: InsertOverlay

Description:
Inserts an overlay at the given iterators position

*******************************************************************************/

UtoWorld::overlay_iterator UtoWorld::InsertOverlay(const std::string& name, overlay_iterator it)
{
   return m_Overlays.InsertOverlay(name, it, this);
}

/* UTO Function Block **********************************************************

Function: RemoveOverlay

Description:
Removes an overlay

*******************************************************************************/

void UtoWorld::RemoveOverlay(overlay_iterator remove)
{
   UtoOverlay* Overlay = dynamic_cast<UtoOverlay*>(remove->overlay);
   m_Switch->removeChild(Overlay->GetTree());

   // Call the base class
   m_Overlays.RemoveOverlay(remove);
}

/* UTO Function Block **********************************************************

Function: RemoveAllOverlays

Description:
Removes all of the overlays

*******************************************************************************/

void UtoWorld::RemoveAllOverlays()
{
   // Get the list of overlays
   // UtoOverlayManager::OverlayList::iterator it;
   // UtoOverlayManager::IteratorPair OverlayItPair = m_Overlays.Overlays();

   // remove all the children instead of by nodes
   // removing children by nodes for the switch causes twice
   // as many nodes to be removed due to the nature of how the
   // switch inherits from the group node.
   m_Switch->removeChild(0, m_Switch->getNumChildren());

   // Remove the overlay from the overlay manager
   m_Overlays.RemoveAllOverlays();
}

/* UTO Function Block **********************************************************

Function: FindOverlay

Description:
Returns an overlay iterator based on the given name

*******************************************************************************/

UtoWorld::overlay_iterator UtoWorld::FindOverlay(const std::string& name)
{
   return m_Overlays.FindOverlay(name);
}

/* UTO Function Block **********************************************************

Function: SetProjection

Description:
Sets the current projection type

*******************************************************************************/

void UtoWorld::SetProjection(UtoViewer* viewer)
{
   m_Projection = viewer->Projection();
}

/* UTO Function Block **********************************************************

Function: UpdateViewport

Description:
Updates the current viewport

*******************************************************************************/

bool UtoWorld::UpdateViewport()
{
   return false;
}

void UtoWorld::UpdateOffsets()
{
   if (!m_Projection)
      return;

   // There is only one matrix to update because it is above the rest of
   // the overlays.
   UtoPosition Offset = m_Projection->Offset();
   osg::Matrix matrix = osg::Matrix::translate(Offset);
   mTerrainTransform->setMatrix(matrix);
   mOverlayTransform->setMatrix(matrix);
}


bool UtoWorld::WorldToWindow(UtoPosition& WorldPosition)
{
   if (!m_Viewer->GetOSGViewer())
   {
      return false;
   }
   // compute the view matrix
   // the above check for the validity of OSGViewer should confirm the validity of these matrices
   osg::Matrix matView     = mOverlayTransform->getMatrix() * *m_Viewer->GetViewMatrix();
   osg::Matrix rProjection = *m_Viewer->GetProjectionMatrix();

   // obtain a reference to the projection matrix pointer
   const osg::Matrix::value_type* pProjPtr = rProjection.ptr();
   // obtain a constant reference to the window size
   const UtoSize oWndSize = m_Viewer->DisplaySize();

   // convert the world coordinates to viewer coordinates
   osg::Vec3 vViewer = osg::Vec3(WorldPosition) * matView;
   // convert the viewer coordinates to the clip coordinates
   osg::Vec3 vClip = vViewer * rProjection;
   // convert the clip coordinates to screen coordinates
   WorldPosition.x() = (vClip.x() * oWndSize.cx * 0.5) + oWndSize.cx * 0.5;
   WorldPosition.y() = (vClip.y() * oWndSize.cy * 0.5) + oWndSize.cy * 0.5;
   WorldPosition.z() = vClip.z();

   // calculate the w component of the clip vector
   osg::Vec3::value_type vW =
      pProjPtr[3] * vViewer.x() + pProjPtr[7] * vViewer.y() + pProjPtr[11] * vViewer.z() + pProjPtr[15];

   // The w component represents the perpendicular distance to the bisecting plane
   // at the camera.  This plane is parallel to both the near and far planes.  When
   // the w component is positive, it represents that the point is in front of the
   // plane ( or in front of the camera ).  When the value is negative, the point is
   // behind the plane ( or behind the camera ).
   return vW > 0 && 0 <= WorldPosition.x() && WorldPosition.x() < oWndSize.cx && 0 <= WorldPosition.y() &&
          WorldPosition.y() < oWndSize.cy;
}


bool UtoWorld::WorldToWindowUnlimited(UtoPosition& WorldPosition)
{
   if (!m_Viewer->GetOSGViewer())
   {
      return false;
   }

   // compute the view matrix
   // the above check for the validity of OSGViewer should confirm the validity of these matrices
   osg::Matrix matView     = mOverlayTransform->getMatrix() * *m_Viewer->GetViewMatrix();
   osg::Matrix rProjection = *m_Viewer->GetProjectionMatrix();

   // obtain a reference to the projection matrix pointer
   const osg::Matrix::value_type* pProjPtr = rProjection.ptr();
   // obtain a constant reference to the window size
   const UtoSize oWndSize = m_Viewer->DisplaySize();

   // convert the world coordinates to viewer coordinates
   osg::Vec3 vViewer = WorldPosition * matView;
   // convert the viewer coordinates to the clip coordinates
   osg::Vec3 vClip = vViewer * rProjection;
   // convert the clip coordinates to screen coordinates
   WorldPosition.x() = (vClip.x() * oWndSize.cx * 0.5) + oWndSize.cx * 0.5;
   WorldPosition.y() = (vClip.y() * oWndSize.cy * 0.5) + oWndSize.cy * 0.5;
   WorldPosition.z() = vClip.z();

   // calculate the w component of the clip vector
   osg::Vec3::value_type vW =
      pProjPtr[3] * vViewer.x() + pProjPtr[7] * vViewer.y() + pProjPtr[11] * vViewer.z() + pProjPtr[15];

   // The w component represents the perpendicular distance to the bisecting plane
   // at the camera.  This plane is parallel to both the near and far planes.  When
   // the w component is positive, it represents that the point is in front of the
   // plane ( or in front of the camera ).  When the value is negative, the point is
   // behind the plane ( or behind the camera ).
   return vW > 0;
}

bool UtoWorld::IsTerrainValid()
{
   if (m_Terrain)
      return true;

   return false;
}
