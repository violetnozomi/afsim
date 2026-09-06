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
// UtoViewer.cpp : implementation file

#include "UtoViewer.hpp"

#include <algorithm>
#include <assert.h>
#include <iostream>

#include "UtMoon.hpp"
#include "UtoEntity.hpp"
#include "UtoEntityDB.hpp"
#include "UtoMemory.hpp"
#include "UtoPriv.hpp"
#include "UtoShape.hpp"
#include "UtoShapeImp.hpp"
#include "UtoViewFilter.hpp"
#include "string.h"
//#include "VideoCaptureHandler.h"
#include "UtoViewerProjection.hpp"
#include "UtoViewerScreenCaptureHandler.hpp"

//#include "VideoEncodeThread.hpp"

//#include "PolyLineShapeImp.h"
//#include "TextShapeImp.h"
#include <osgUtil/SceneView>
#include <osgUtil/UpdateVisitor>

#include "UtoHittestStrategy.hpp"
#include "UtoRenderInfo.hpp"
#include "UtoResourceDB.hpp"
#ifdef _WIN32
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#else
#include <osgViewer/api/X11/GraphicsWindowX11>
#endif
#include <osg/BlendFunc>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Vec3>
#include <osgDB/DatabasePager>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

// This places a static red sphere at the OpenGL origin which will not
// be moved as the perspective local origin as moved.
//#define REAL_ORIGIN 1
#ifdef REAL_ORIGIN
#include <osg/Shape>
#include <osg/ShapeDrawable>
#endif

#include "UtoTreeDump.hpp"

#ifdef _WIN32
#pragma warning(disable : 4355)
#endif

#define NO_OSG_EARTH_PREINITIALIZE

#ifndef NO_OSG_EARTH_PREINITIALIZE
#include <osgEarth/Registry>
#endif

/* UTO Function Block **********************************************************

Function: UtoViewer

Description:
   Constructor

*******************************************************************************/

UtoViewer::UtoViewer(const std::string& name)
   : m_pConnectedDB(nullptr)
   , m_ActiveCamera(UtoViewer::ORTHOGONAL)
   , m_OrthoCamera(this)
   , m_PerspectiveCamera(this)
   , m_OSGViewer(new osgViewer::Viewer)
   , m_DatabasePager(new osgDB::DatabasePager)
   , m_RootNode(new osg::Group)
   , mScene(new osg::Group)
   , m_World(this, m_DatabasePager.get())
   , m_Window(UtoSize(600, 600), -1, this, m_DatabasePager.get())
   , mScreenOverlayManager(UtoSize(600, 600), this)
   , m_Filter(nullptr)
   , m_FrameNumber(0)
   , m_SimulationTime(0)
   , m_HittestStrategy(new UtoHittestStrategy(this, true))
   , m_DoPrepareForRender(false)
{
   SetComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
   SetCullingMode((osg::CullSettings::DEFAULT_CULLING | osg::CullSettings::FAR_PLANE_CULLING) &
                  ~osg::CullSettings::SMALL_FEATURE_CULLING);

   m_RootNode->setName(name);
   m_OSGViewer->setUpViewerAsEmbeddedInWindow(100, 100, 500, 500);
   m_Window.InitSort();
   SetSceneData(m_RootNode.get());
#if REAL_ORIGIN
   osg::ShapeDrawable* drawable = new osg::ShapeDrawable;
   drawable->setShape(new osg::Sphere);
   drawable->setColor(osg::Vec4(1, 0, 0, 1));
   osg::Geode* geode = new osg::Geode;
   geode->setName("Real Origin sphere");
   geode->addDrawable(drawable);
   m_RootNode->addChild(geode);
#endif

   // setup the default projection
   SetProjection(UtoViewerProjection());

   // add the world overlays to the scene
   m_RootNode->addChild(m_World.GetTerrainTree());
   m_RootNode->addChild(m_World.GetOverlayTree());

   // add the window overlays to the scene
   m_RootNode->addChild(mScreenOverlayManager.GetTree());

   m_RootNode->addChild(mScene.get());
   mScene->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
   mScene->setName("Scene Node");

   // Pass a pointer to this viewer in the update visitor
   osgUtil::UpdateVisitor* visitor = new osgUtil::UpdateVisitor;
   UtoRenderInfo*          info    = new UtoRenderInfo(this);
   visitor->setUserData(info);
   SetUpdateVisitor(visitor);

   // Select the orthographic camera to start with.
   SetActiveCamera(UtoViewer::ORTHOGONAL);
   AddHandlers(m_OSGViewer);
}

UtoViewer::UtoViewer(osgViewer::Viewer* aViewer, const std::string& name)
   : m_pConnectedDB(nullptr)
   , m_ActiveCamera(UtoViewer::ORTHOGONAL)
   , m_OrthoCamera(this)
   , m_PerspectiveCamera(this)
   , m_OSGViewer(aViewer)
   , m_DatabasePager(new osgDB::DatabasePager)
   , m_RootNode(new osg::Group)
   , mScene(new osg::Group)
   , m_World(this, m_DatabasePager.get())
   , m_Window(UtoSize(600, 600), -1, this, m_DatabasePager.get())
   , mScreenOverlayManager(UtoSize(600, 600), this)
   , m_Filter(nullptr)
   , m_FrameNumber(0)
   , m_SimulationTime(0)
   , m_HittestStrategy(new UtoHittestStrategy(this, true))
   , m_DoPrepareForRender(false)
{
   m_RootNode->setName(name);

   if (aViewer)
   {
      SetComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
      SetCullingMode((osg::CullSettings::DEFAULT_CULLING | osg::CullSettings::FAR_PLANE_CULLING) &
                     ~osg::CullSettings::SMALL_FEATURE_CULLING);
   }

   m_Window.InitSort();
   SetSceneData(m_RootNode.get());
#if REAL_ORIGIN
   osg::ShapeDrawable* drawable = new osg::ShapeDrawable;
   drawable->setShape(new osg::Sphere);
   drawable->setColor(osg::Vec4(1, 0, 0, 1));
   osg::Geode* geode = new osg::Geode;
   geode->setName("Real Origin sphere");
   geode->addDrawable(drawable);
   m_RootNode->addChild(geode);
#endif

   // setup the default projection
   SetProjection(UtoViewerProjection());

   // add the world overlays to the scene
   m_RootNode->addChild(m_World.GetTerrainTree());
   m_RootNode->addChild(m_World.GetOverlayTree());

   // add the window overlays to the scene
   m_RootNode->addChild(mScreenOverlayManager.GetTree());

   m_RootNode->addChild(mScene.get());

   // Pass a pointer to this viewer in the update visitor
   osgUtil::UpdateVisitor* visitor = new osgUtil::UpdateVisitor;
   UtoRenderInfo*          info    = new UtoRenderInfo(this);
   visitor->setUserData(info);
   SetUpdateVisitor(visitor);

   // Select the orthographic camera to start with.
   SetActiveCamera(UtoViewer::ORTHOGONAL);
   if (m_OSGViewer)
   {
      AddHandlers(m_OSGViewer);
   }
}

void UtoViewer::SetupCelestialCamera()
{
   /////////////////////
   // This camera draws the stars / sun / moon / atmosphere
   /////////////////////
   if (!mCelestialCamera.valid())
   {
      mCelestialCamera = ut::osg::make_ref<osg::Camera>();
      mCelestialCamera->setName("celestialCamera");
      mCelestialCamera->setViewMatrix(osg::Matrix::identity());
      mCelestialCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      mCelestialCamera->setClearMask(GL_COLOR_BUFFER_BIT);
      mCelestialCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
      mCelestialCamera->setViewport(0, 0, 1, 1);

      mCelestialCamera->setRenderOrder(osg::Camera::PRE_RENDER);

      osg::Camera* orthoCamera = GetOSGViewer()->getCamera();
      orthoCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
      orthoCamera->addChild(mCelestialCamera.get());
   }
}

// copy the world from the main scene to the celestial scene for distant viewing
void UtoViewer::CopyTerrainToCelestial()
{
   mCelestialCamera->addChild(m_World.GetTerrainTree());
}

/* UTO Function Block **********************************************************

Function: UtoViewer

Description:
   Destructor

*******************************************************************************/

UtoViewer::~UtoViewer()
{
   // send the disconnect signal to
   // all active listeners
   Disconnect(true);

   // clear all the callback handlers
   clear();

   // remove the terrain before the projection is reset
   m_World.ClearTerrain();
   SetProjection(UtoViewerProjection());
   delete m_HittestStrategy;
   mScreenOverlayManager.RemoveAllOverlays();
   delete m_Filter;
}

void UtoViewer::AddHandlers(osgViewer::Viewer* aViewerPtr)
{
   mStatsHandler = new osgViewer::StatsHandler;
   mStatsHandler->setKeyEventTogglesOnScreenStats('1');
   aViewerPtr->addEventHandler(mStatsHandler.get());

   mScreenCapture = new UtoViewerScreenCaptureHandler();
   mScreenCapture->setKeyEventTakeScreenShot('6');
   aViewerPtr->addEventHandler(mScreenCapture);
}

/* UTO Function Block **********************************************************

Function: Name

Description:
   Return the name of the viewer.

*******************************************************************************/

std::string UtoViewer::Name()
{
   return m_RootNode->getName();
}

/* UTO Function Block **********************************************************

Function: Draw

Description:
   Draw the scenes in the viewer

*******************************************************************************/

void UtoViewer::Draw()
{
   m_FrameNumber++;

   m_Window.Draw();
}

void UtoViewer::PreSceneDraw()
{
   m_FrameNumber++;
   m_Window.PreSceneDraw();
}

void UtoViewer::SceneDraw()
{
   m_Window.SceneDraw();
}

void UtoViewer::PostSceneDraw()
{
   m_Window.PostSceneDraw();
}

/* UTO Function Block *********************************************************

Function: SetActiveCamera

Description:
   Sets the active camera for the given viewer

*******************************************************************************/

void UtoViewer::SetActiveCamera(UtoViewer::CameraMode mode)
{
   m_ActiveCamera = mode;

   // destroy the previous hitteseting strategy.
   delete m_HittestStrategy;

   switch (mode)
   {
   case UtoViewer::ORTHOGONAL:
      if (m_OSGViewer)
      {
         m_OrthoCamera.AttachViewer(m_OSGViewer.get());
      }
      SetUniform("UTO_OrthographicMode", 1.0);
      m_HittestStrategy = new UtoHittestStrategy(this, true);
      m_PerspectiveCamera.DetachViewer();
      break;
   case UtoViewer::PERSPECTIVE:
      if (m_OSGViewer)
      {
         m_PerspectiveCamera.AttachViewer(m_OSGViewer.get());
      }
      SetUniform("UTO_OrthographicMode", 0.0);
      m_HittestStrategy = new UtoHittestStrategy(this, false);
      m_OrthoCamera.DetachViewer();
   }

   sendEvent(CameraChangedEvent());
}

/* UTO Function Block *********************************************************

Function: Connect

Description:
   Connect the viewer to the database and create the new tree.
   - destory current database group node, symbol list, and ViewerEntityList
   - add a database group node
   - add each entity in the database

*******************************************************************************/

bool UtoViewer::Connect(UtoEntityDB& edb)
{
   if (m_pConnectedDB)
      return false;

   if (m_ActiveCamera == UtoViewer::ORTHOGONAL)
   {
      SetUniform("UTO_OrthographicMode", 1.0);
      SetUniform("UTO_OrthographicZoom", m_OrthoCamera.Zoom());
   }
   else
   {
      SetUniform("UTO_OrthographicMode", 0.0);
      SetUniform("UTO_OrthographicZoom", m_OrthoCamera.Zoom());
   }

   // set up a dummy texture to be used by 3D models that have none:
   SetUpDummyTexture();

   // ensure window sub-graph is ordered correctly after the DB node
   m_RootNode->removeChild(mScreenOverlayManager.GetTree());
   m_RootNode->addChild(mScreenOverlayManager.GetTree());

   m_pConnectedDB = &edb;
   m_pConnectedDB->AddViewer(this);
   // iterate all entities and add symbols.
   UtoEntityDB::IteratorPair        entites = edb.Entities();
   UtoEntityDB::EntityMap::iterator ent;
   for (ent = entites.first; ent != entites.second; ++ent)
   {
      AddEntityCB(ent->second);
      ent->second->ViewerAdded(this);
   }

   // Set up the callback functions for the database
   edb.connect(UtoEntityDB::AddEntityEvent(), this, &UtoViewer::AddEntityCB);
   edb.connect(UtoEntityDB::RemoveEntityEvent(), this, &UtoViewer::RemoveEntityCB);
   edb.connect(UtoEntityDB::DestroyedEvent(), this, &UtoViewer::DBDestroyedCB);

   sendEvent(ConnectedEvent(), this);

   return true;
}

/* UTO Function Block **********************************************************

Function: Disconnect

Description:
   Disconnect the viewer from the entity DB
   - destory the current database group node, symbol list, and ViewerEntityList

*******************************************************************************/

bool UtoViewer::Disconnect(bool shutdown)
{
   if (m_pConnectedDB == nullptr)
      return false;

   sendEvent(DisconnectedEvent(), this);

   if (m_pConnectedDB)
   {
      for (UtoEntityDB::EntityMap::const_iterator i = m_pConnectedDB->GetEntities().begin();
           i != m_pConnectedDB->GetEntities().end();
           ++i)
      {
         i->second->ViewerRemoved(this);
      }

      m_pConnectedDB->disconnect(UtoEntityDB::DestroyedEvent(), this, &UtoViewer::DBDestroyedCB);
      m_pConnectedDB->disconnect(UtoEntityDB::AddEntityEvent(), this, &UtoViewer::AddEntityCB);
      m_pConnectedDB->disconnect(UtoEntityDB::RemoveEntityEvent(), this, &UtoViewer::RemoveEntityCB);


      //// Clear out the viewer entities. Note that while the list of entities
      //// from the database had better match the list in m_ViewerEntities,
      //// we use the list from the database here because RemoveEntity
      //// modifies the m_ViewerEntities as it goes and invalidates the current
      //// iterator.
      // UtoEntityDB::IteratorPair EntListIP = m_pConnectedDB->Entities();
      // UtoEntityDB::EntityMap::iterator EntIt = EntListIP.first;
      // for(; EntIt != EntListIP.second; ++EntIt)
      //{
      //    RemoveEntityCB(EntIt->second);
      // }

      // assert(m_Symbols.empty());

      // Cancel the camera locks and look ats
#if _MSC_VER > 1200
      m_PerspectiveCamera.UnlockEntity();
      m_PerspectiveCamera.LookAt(UtoPosition(0, 0, 0));
      m_OrthoCamera.UnlockEntity();
#endif
   }

   m_pConnectedDB->RemoveViewer(this);
   m_pConnectedDB = nullptr;

   return true;
}

/* UTO Function Block **********************************************************

Function: IsConnected

Description:
   Viewer connected to entityDB?

*******************************************************************************/

bool UtoViewer::IsConnected() const
{
   return (m_pConnectedDB != nullptr);
}

/* UTO Function Block **********************************************************

Function: ConnectedDB

Description:
   Get the name of the connected DB

*******************************************************************************/

std::string UtoViewer::ConnectedDB() const
{
   return m_pConnectedDB->Name();
}

/* UTO Function Block **********************************************************

Function: HitTestSymbol

Description:
   Return a symbol hittest list.

*******************************************************************************/

UtoViewer::SymbolHittestList*
UtoViewer::HitTestSymbol(UtoPoint pos, const std::string& symbols, unsigned int max, bool aScreenScaled, float aIconScale)
{
   m_Window.HittestUpdate();
   float orthoScale = 0.0f;
   if (m_ActiveCamera == UtoViewer::ORTHOGONAL)
   {
      orthoScale = m_OrthoCamera.Zoom();
   }
   return m_HittestStrategy->HitTestSymbol(pos, symbols, max, aScreenScaled, orthoScale, aIconScale);
}

UtoSize UtoViewer::DisplaySize() const
{
   const osg::Viewport* vp = GetViewport();
   return UtoSize(vp->width(), vp->height());
}

void UtoViewer::SetDisplaySize(const UtoSize& aSize)
{
   if (GetOSGViewer() != nullptr)
   {
      SetViewport(0, 0, (int)aSize.cx, (int)aSize.cy);
   }

   // update both the projection and scene matrices for the current camera
   UtoCamera* camera;
   if (ActiveCamera() == UtoViewer::ORTHOGONAL)
   {
      camera = GetOrtho();
   }
   else // UtoViewer::PERSPECTIVE
   {
      camera = GetPerspective();
   }

   if (GetOSGViewer() != nullptr)
   {
      camera->UpdateProjection();
      camera->UpdateSceneView();
   }
}

bool UtoViewer::DisplayToWorld(UtoPosition& aPos)
{
   if (!m_OSGViewer)
   {
      return false;
   }
   // determine the "from" location based on mouse position
   const osg::Viewport* viewport = GetViewport();

   double vp[4] = {viewport->x(), viewport->y(), viewport->x() + viewport->width(), viewport->y() + viewport->height()};

   osg::Vec4d eye((2.0 * (aPos.x() - vp[0]) / vp[2]) - 1.0,
                  (2.0 * (aPos.y() - vp[1]) / vp[3]) - 1.0,
                  2.0 * aPos.z() - 1.0,
                  1.0);

   osg::Matrixd pminv;
   // the above check for the validity of OSGViewer should confirm the validity of these matrices
   pminv = *GetProjectionMatrix();
   pminv = *GetViewMatrix() * pminv;

   pminv = osg::Matrixd::inverse(pminv);
   if (!pminv.isNaN())
   {
      osg::Vec4d from = eye * pminv;
      aPos.x()        = from.x() / from.w();
      aPos.y()        = from.y() / from.w();
      aPos.z()        = from.z() / from.w();
   }
   return true;
}

bool UtoViewer::DisplayToWorld(UtoRect& aRect)
{
   if ((!GetOSGViewer()) || (ActiveCamera() != UtoViewer::ORTHOGONAL))
   {
      return false;
   }

   // get the projection box
   double xmin, xmax, ymin, ymax, zmin, zmax;
   GetProjectionMatrixAsOrtho(xmin, xmax, ymin, ymax, zmin, zmax);

   // determine the "from" location based on mouse position
   const osg::Viewport* viewport = GetViewport();

   double    xScale = (xmax - xmin) / viewport->width();
   double    yScale = (ymax - ymin) / viewport->height();
   osg::Vec3 TopLeftEye(xmin + aRect.left * xScale, ymin + aRect.top * yScale, zmax);
   osg::Vec3 BottomRightEye(xmin + aRect.right * xScale, ymin + aRect.bottom * yScale, zmax);

   // transform to world coordinate
   // the above check for the validity of OSGViewer should confirm the validity of this matrix
   osg::Matrix toWorld = osg::Matrix::inverse(*GetViewMatrix());

   osg::Vec3 TLFrom = TopLeftEye * toWorld;
   osg::Vec3 BRFrom = BottomRightEye * toWorld;

   // store them back
   aRect.left   = TLFrom.x();
   aRect.top    = TLFrom.y();
   aRect.right  = BRFrom.x();
   aRect.bottom = BRFrom.y();

   return true;
}

bool UtoViewer::AttachToWindowId(UtoWindowId aId, bool aReleaseContext)
{
   return m_Window.AttachToWindowId(aId, aReleaseContext);
}

bool UtoViewer::AttachToGraphicsContext(osg::GraphicsContext* aGC)
{
   return m_Window.AttachToGraphicsContext(aGC);
}

/* UTO Function Block **********************************************************

Function: SetFilter

Description:
   Set the filter

*******************************************************************************/

void UtoViewer::SetFilter(UtoViewFilter* filter)
{
   m_Filter = filter;
}


/* UTO Function Block **********************************************************

Function: SetProjection

Description:
   Sets the projection object for handing positional projections.

*******************************************************************************/

void UtoViewer::SetProjection(const UtoViewerProjection& p)
{
   m_pProjection = p;
   if (m_pConnectedDB)
   {
      const UtoEntityDB::EntityMap& entities = m_pConnectedDB->GetEntities();
      for (UtoEntityDB::const_iterator i = entities.begin(); i != entities.end(); ++i)
      {
         i->second->ForceMoveUpdate();
      }
   }
   sendEvent(ProjectionChangedEvent(), this);
}

void UtoViewer::DBDestroyedCB(UtoEntityDB* EntityDBImp)
{
   Disconnect(false);
}

/* UTO Function Block **********************************************************

Function: ResourcesChangedCB

Description:
   Handles the case that a shape just replaced another shape by the same name.
   TODO: Would it revert to hidden?

*******************************************************************************/

void UtoViewer::ResourcesChangedCB(UtoEntity* entity, UtoShapeImp* shapeOld, UtoShapeImp* shapeNew)
{
   assert(0);
}

/* UTO Function Block **********************************************************

Function: AddEntityCB

Description:
   Handles the Add entity message from the entityDB
   - create UtoViewerEntity
   - add it to the map
   - create MatrixTransform node
   - add it to the database
   - set transform to the UtoViewerEntity
   - process each symbol in the entity

*******************************************************************************/

void UtoViewer::AddEntityCB(UtoEntity* entityImp)
{
   // Create a new viewer entity
   // UtoViewerEntity* VEntity = new
   // UtoViewerEntity(*entityImp, this);

   // give it the current projection
   // VEntity->SetProjection();

   // add it to the viewer entity map
   // m_ViewerEntities.insert(
   // ViewerEntityMap::value_type(entityImp, VEntity));

   if (m_Filter)
   {
      // notify the user filter of the entity add
      m_Filter->OnAdd(*entityImp);
   }

   // VEntity->Initialize();
}

/* UTO Function Block **********************************************************

Function: RemoveEntityCB

Description:
   Handle the remove entity from the entityDB

*******************************************************************************/

void UtoViewer::RemoveEntityCB(UtoEntity* entityImp)
{
   UtoEntity* entity = entityImp;
   if (m_Filter)
   {
      // notify the filter of a removal
      m_Filter->OnRemove(*entity);
   }

   entity->ViewerRemoved(this);

   // find and remove the symbols for this entity
   // ViewerEntityMap::iterator VEMapEntry = m_ViewerEntities.find(entityImp);
   // assert(VEMapEntry != m_ViewerEntities.end());

   // delete VEMapEntry->second;
}

/* UTO Function Block **********************************************************

Function: DumpScene

Description:
   Dump the scene graph

*******************************************************************************/

void UtoViewer::DumpScene(std::ostream& out)
{
   if (m_RootNode)
   {
      osgDB::writeNodeFile(*(GetOSGViewer()->getCamera()), "scenegraph.osg");
   }
}

void UtoViewer::DumpScene(std::ostream& out, int skiptypes)
{
   UtoTreeDump td;
   td.Dump(out, m_RootNode.get(), (UtoTreeDump::SkipList)skiptypes);
}

void UtoViewer::Dump(std::ostream& o)
{
   // dump out: cameras, connection, world, window, visible symbols, relations, overlays.
   o << "******************************" << std::endl;
   o << "Viewer dump" << std::endl << std::endl;

   o << "  DB connection: ";
   if (IsConnected())
   {
      std::string name(ConnectedDB());
      if (name.empty())
         name = "<default>";

      o << name << std::endl;
   }
   else
   {
      o << "none" << std::endl;
   }
   std::string cam((ActiveCamera() == UtoViewer::ORTHOGONAL) ? "Orthogonal" : "Perspective");
   o << "  Active camera: " << cam << std::endl;
   o << std::endl;

   o << "Ortho camera dump" << std::endl;
   o << "-----------------" << std::endl;
   GetOrtho()->Dump(o);
   o << std::endl;

   o << "Perspective camera dump" << std::endl;
   o << "-----------------------" << std::endl;
   GetPerspective()->Dump(o);
   o << std::endl;

   if (GetFilter())
   {
      o << "Filter dump" << std::endl;
      o << "-----------" << std::endl;
      GetFilter()->Dump(o);
      o << std::endl;
   }

   o << "Scene Dump" << std::endl;
   DumpScene(o);
}

void UtoViewer::UpdateOffsets()
{
   if (m_pConnectedDB)
   {
      const UtoEntityDB::EntityMap& entities = m_pConnectedDB->GetEntities();
      for (UtoEntityDB::const_iterator i = entities.begin(); i != entities.end(); ++i)
      {
         i->second->ForceMoveUpdate();
      }
   }
   // update the world overlays
   m_World.UpdateOffsets();
}

void UtoViewer::SetSimulationTime(double aSimTime)
{
   m_SimulationTime = aSimTime;
   m_Window.SetSimulationTime(aSimTime);
   osg::StateSet* ss = m_RootNode->getOrCreateStateSet();
   ss->getOrCreateUniform("UTO_simTime", osg::Uniform::FLOAT)->set((float)aSimTime);
}

void UtoViewer::SetSunVector(const osg::Vec3& aSunVector)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();
      ss->getOrCreateUniform("vtk_sunVec", osg::Uniform::FLOAT_VEC3)->set(aSunVector);
   }
   if (nullptr != mCelestialCamera)
   {
      osg::StateSet* ss = mCelestialCamera->getOrCreateStateSet();
      ss->getOrCreateUniform("vtk_sunVec", osg::Uniform::FLOAT_VEC3)->set(aSunVector);
   }
}

void UtoViewer::SetUniform(const char* aName, const float aValue)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set((float)aValue);
   }
}

void UtoViewer::SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC3)->set(osg::Vec3(aValue1, aValue2, aValue3));
   }
}

void UtoViewer::SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3, const float aValue4)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(osg::Vec4(aValue1, aValue2, aValue3, aValue4));
   }
}

void UtoViewer::SetUniform(const char* aName, const unsigned int aIndex, const char* aFilename)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();


      osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
      osg::ref_ptr<osg::Image>     image   = osgDB::readImageFile(aFilename);
      if (image.valid())
      {
         texture->setImage(image.get());
         texture->setInternalFormat(GL_RGBA);
         texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
         texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

         texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::CLAMP_TO_EDGE);
         texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
         texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);

         ss->setTextureAttribute(aIndex, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
         //         ss->setTextureAttributeAndModes(aIndex, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
         ss->getOrCreateUniform(aName, osg::Uniform::SAMPLER_2D)->set((int)aIndex);
      }
   }
}

void UtoViewer::SetUniform(const char*          aName,
                           const unsigned int   aIndex,
                           const unsigned char* aData,
                           const unsigned int   aWidth,
                           const unsigned int   aHeight)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();

      unsigned char* datamem = new unsigned char[aWidth * aHeight * 3];
      memcpy(datamem, aData, aWidth * aHeight * 3);
      osg::ref_ptr<osg::Image> hmimage = new osg::Image();
      hmimage->setImage(aWidth, aHeight, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, datamem, osg::Image::USE_NEW_DELETE);
      osg::ref_ptr<osg::Texture2D> hmtexture = new osg::Texture2D();
      hmtexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      hmtexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      hmtexture->setImage(hmimage.get());

      ss->setTextureAttribute(aIndex, hmtexture.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
      ss->getOrCreateUniform(aName, osg::Uniform::SAMPLER_2D)->set((int)aIndex);
   }
}

void UtoViewer::SetUniform(const char* aName, const osg::Matrix& aMatrix)
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet* ss = m_RootNode->getOrCreateStateSet();
      ss->getOrCreateUniform(aName, osg::Uniform::FLOAT_MAT4)->set(aMatrix);
   }
}

osg::Group* UtoViewer::AddRTTGroup(const std::string aName)
{
   osg::ref_ptr<osg::Group> grp = new osg::Group;
   grp->setName(aName);
   m_RootNode->insertChild(0, grp);
   return grp;
}

void UtoViewer::SetViewMatrix(const osg::Matrix& aMatrix)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setViewMatrix(aMatrix);
      m_OSGViewer->getCamera()->getOrCreateStateSet()->getOrCreateUniform("UTO_ViewMatrix", osg::Uniform::FLOAT_MAT4)->set(aMatrix);
   }
   if (mCelestialCamera)
   {
      mCelestialCamera->setViewMatrix(aMatrix);
      mCelestialCamera->getOrCreateStateSet()->getOrCreateUniform("UTO_ViewMatrix", osg::Uniform::FLOAT_MAT4)->set(aMatrix);
   }
}

void UtoViewer::SetViewMatrixAsLookAt(const osg::Vec3& aEye, const osg::Vec3& aCenter, const osg::Vec3& aUp)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setViewMatrixAsLookAt(aEye, aCenter, aUp);
   }
   if (mCelestialCamera)
   {
      mCelestialCamera->setViewMatrixAsLookAt(aEye, aCenter, aUp);
   }
}

void UtoViewer::SetProjectionMatrixAsPerspective(float aFOV, float aAspectRatio, float aHither, float aYon)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setProjectionMatrixAsPerspective(aFOV, aAspectRatio, aHither, aYon);
      if (mCelestialCamera)
      {
         m_OSGViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
         mCelestialCamera->setNodeMask(~0);
      }
      SetUniform("UTO_ProjectionMatrixInverse", osg::Matrix::inverse(m_OSGViewer->getCamera()->getProjectionMatrix()));
   }
   if (mCelestialCamera)
   {
      mCelestialCamera->setProjectionMatrixAsPerspective(aFOV, aAspectRatio, 10000, UtMoon::cA * 3.5);
      osg::StateSet* scss = mCelestialCamera->getOrCreateStateSet();
      scss->getOrCreateUniform("UTO_ProjectionMatrixInverse", osg::Uniform::FLOAT_MAT4)
         ->set(osg::Matrix::inverse(mCelestialCamera->getProjectionMatrix()));
   }
}

void UtoViewer::SetProjectionMatrixAsFrustum(double aLeft, double aRight, double aBottom, double aTop, double aNear, double aFar)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setProjectionMatrixAsFrustum(aLeft, aRight, aBottom, aTop, aNear, aFar);
      if (mCelestialCamera)
      {
         m_OSGViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
         mCelestialCamera->setNodeMask(~0);
      }
      SetUniform("UTO_ProjectionMatrixInverse", osg::Matrix::inverse(m_OSGViewer->getCamera()->getProjectionMatrix()));
   }
   if (mCelestialCamera)
   {
      double factor = 10000.0 / aNear;
      mCelestialCamera->setProjectionMatrixAsFrustum(aLeft * factor,
                                                     aRight * factor,
                                                     aBottom * factor,
                                                     aTop * factor,
                                                     10000.0,
                                                     UtMoon::cA * 3.5);
      osg::StateSet* ss = mCelestialCamera->getOrCreateStateSet();
      ss->getOrCreateUniform("UTO_ProjectionMatrixInverse", osg::Uniform::FLOAT_MAT4)
         ->set(osg::Matrix::inverse(mCelestialCamera->getProjectionMatrix()));
   }
}

const osg::Viewport* UtoViewer::GetViewport() const
{
   osg::Viewport* vp = nullptr;
   if (m_OSGViewer)
   {
      vp = m_OSGViewer->getCamera()->getViewport();
   }
   return vp;
}

void UtoViewer::SetViewport(int aX, int aY, int aW, int aH)
{
   if (m_OSGViewer)
   {
      if (m_OSGViewer->isRealized())
      {
         m_OSGViewer->getCamera()->getGraphicsContext()->resized(aX, aY, aW, aH);
      }
      m_OSGViewer->getCamera()->setViewport(aX, aY, aW, aH);
      m_OSGViewer->getCamera()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("UTO_Viewport", osg::Uniform::FLOAT_VEC4)
         ->set(osg::Vec4(aX, aY, aW, aH));
   }
   if (mCelestialCamera)
   {
      mCelestialCamera->setViewport(aX, aY, aW, aH);
   }
}

void UtoViewer::SetProjectionMatrixAsOrtho(float aMinX, float aMaxX, float aMinY, float aMaxY, float aMinZ, float aMaxZ)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
      if (mCelestialCamera)
      {
         m_OSGViewer->getCamera()->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         mCelestialCamera->setNodeMask(0);
      }
      SetUniform("UTO_ProjectionMatrixInverse", osg::Matrix::inverse(m_OSGViewer->getCamera()->getProjectionMatrix()));
   }
   if (mCelestialCamera)
   {
      mCelestialCamera->setProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
      osg::StateSet* ss = mCelestialCamera->getOrCreateStateSet();
      ss->getOrCreateUniform("UTO_ProjectionMatrixInverse", osg::Uniform::FLOAT_MAT4)
         ->set(osg::Matrix::inverse(mCelestialCamera->getProjectionMatrix()));
   }
}


void UtoViewer::GetProjectionMatrixAsFrustum(double& aLeft,
                                             double& aRight,
                                             double& aBottom,
                                             double& aTop,
                                             double& aNear,
                                             double& aFar)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->getProjectionMatrixAsFrustum(aLeft, aRight, aBottom, aTop, aNear, aFar);
   }
}

void UtoViewer::GetProjectionMatrixAsOrtho(double& aMinX, double& aMaxX, double& aMinY, double& aMaxY, double& aMinZ, double& aMaxZ)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->getProjectionMatrixAsOrtho(aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ);
   }
}

const osg::Matrix* UtoViewer::GetViewMatrix() const
{
   if (m_OSGViewer)
   {
      return &m_OSGViewer->getCamera()->getViewMatrix();
   }
   else
   {
      return nullptr;
   }
}

const osg::Matrix* UtoViewer::GetProjectionMatrix() const
{
   if (m_OSGViewer)
   {
      return &m_OSGViewer->getCamera()->getProjectionMatrix();
   }
   else
   {
      return nullptr;
   }
}

void UtoViewer::SetFrameStamp(osg::FrameStamp* aFrameStamp)
{
   //   I am skipping this since it breaks the performance monitor
   //   m_OSGViewer->setFrameStamp(aFrameStamp);
}

void UtoViewer::SetComputeNearFarMode(osg::CullSettings::ComputeNearFarMode aSettings)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setComputeNearFarMode(aSettings);
   }
}

void UtoViewer::SetCullingMode(int aMode)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setCullingMode(aMode);
   }
}

void UtoViewer::FlushAllDeletedGLObjects() {}

void UtoViewer::CompileAndFlushGLObjects(double aTime)
{
   // TODO JON, DO I NEED TO DO ANYTHING HERE
}

void UtoViewer::SetDatabasePager(osgDB::DatabasePager* aPager)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->setDatabasePager(aPager);
   }
}

void UtoViewer::BuildGraphicsContext()
{
   if (m_OSGViewer)
   {
      osg::DisplaySettings*                      ds     = osg::DisplaySettings::instance().get();
      osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
      traits->windowName                                = "osgQt";
      traits->x                                         = 100;
      traits->y                                         = 100;
      traits->width                                     = 500;
      traits->height                                    = 500;
      traits->doubleBuffer                              = true;
      traits->alpha                                     = ds->getMinimumNumAlphaBits();
      traits->stencil                                   = ds->getMinimumNumStencilBits();
      traits->sampleBuffers                             = ds->getMultiSamples();
      traits->samples                                   = ds->getNumMultiSamples();
#ifdef _WIN32
      osgViewer::GraphicsWindowEmbedded* gw = new osgViewer::GraphicsWindowEmbedded(traits);
      m_OSGViewer->getCamera()->setGraphicsContext(gw);
#else
      osgViewer::GraphicsWindowEmbedded* gw = new osgViewer::GraphicsWindowEmbedded(traits);
      m_OSGViewer->getCamera()->setGraphicsContext(gw);
#endif
      // TODO JON, FIGURE OUT WHAT WE ARE ACTUALLY DOING HERE
      //   m_OSGViewer->getCamera()->getGraphicsContext()->getState()->setUseVertexAttributeAliasing(true);
      m_OSGViewer->getCamera()->getGraphicsContext()->getState()->setUseModelViewAndProjectionUniforms(true);
   }
}

void UtoViewer::UseGraphicsContext(osg::GraphicsContext* aGC)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setGraphicsContext(aGC);
      //      m_OSGViewer->getCamera()->getGraphicsContext()->getState()->setUseVertexAttributeAliasing(true);
      m_OSGViewer->getCamera()->getGraphicsContext()->getState()->setUseModelViewAndProjectionUniforms(true);
   }
}

void UtoViewer::DrawScene()
{
   if (m_OSGViewer)
   {
      if (!m_OSGViewer->isRealized())
      {
         m_OSGViewer->realize();
      }
      m_OSGViewer->frame(m_SimulationTime);
   }
}

void UtoViewer::SetLightingMode(osg::View::LightingMode aMode)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->setLightingMode(aMode);
   }
}

const osg::Light* UtoViewer::GetLight()
{
   if (m_OSGViewer)
   {
      return m_OSGViewer->getLight();
   }
   else
   {
      return nullptr;
   }
}

void UtoViewer::SetGlobalAttributeAndModes(osg::StateAttribute* aAttribute, osg::StateAttribute::Values aMode)
{
   // TODO JON
}

const osg::StateSet* UtoViewer::GetGlobalStateSet()
{
   return nullptr; // TODO JON
}

void UtoViewer::SetClearColor(const osg::Vec4& aInput)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getCamera()->setClearColor(aInput);
   }
}

const osg::Vec4& UtoViewer::GetClearColor()
{
   return m_OSGViewer->getCamera()->getClearColor();
}

void UtoViewer::SetSceneData(osg::Node* aRootNode)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->setSceneData(aRootNode);
   }
}

void UtoViewer::SetUpdateVisitor(osgUtil::UpdateVisitor* aVisitor)
{
   if (m_OSGViewer)
   {
      m_OSGViewer->setUpdateVisitor(aVisitor);
   }
}

void UtoViewer::ToggleStats()
{
   if (m_OSGViewer)
   {
      m_OSGViewer->getEventQueue()->keyPress('1');
      m_OSGViewer->getEventQueue()->keyRelease('1');
   }
}

void UtoViewer::ScreenShot(const std::string& aOutputDir, int aInstanceId)
{
   if (m_OSGViewer)
   {
      mScreenCapture->SetFilename("");
      mScreenCapture->SetCaptureDirectory(aOutputDir);
      mScreenCapture->SetContextId(aInstanceId);
      m_OSGViewer->getEventQueue()->keyPress('6');
      m_OSGViewer->getEventQueue()->keyRelease('6');
   }
}

void UtoViewer::ScreenShotToFilename(const std::string& aFilename)
{
   if (m_OSGViewer)
   {
      mScreenCapture->SetFilename(aFilename);
      m_OSGViewer->getEventQueue()->keyPress('6');
      m_OSGViewer->getEventQueue()->keyRelease('6');
      DrawScene();
   }
}

void UtoViewer::MaskSymbolShapes(const std::string& aPrefix, bool aOnForPrefix, bool aOnForOthers)
{
   if (!m_pConnectedDB)
      return;
   const UtoEntityDB::EntityMap& entities = m_pConnectedDB->GetEntities();
   for (UtoEntityDB::const_iterator i = entities.begin(); i != entities.end(); ++i)
   {
      UtoEntity*             entityPtr = i->second;
      std::vector<UtoShape*> shapes    = entityPtr->GetShapes();
      for (size_t j = 0; j < shapes.size(); ++j)
      {
         UtoShape* shapePtr = shapes[j];
         if (shapePtr->GetName().compare(0, aPrefix.size(), aPrefix))
         {
            shapePtr->Show(aOnForPrefix, *this);
         }
         else
         {
            shapePtr->Show(aOnForOthers, *this);
         }
      }
   }
}

void UtoViewer::SetUpDummyTexture()
{
   if (nullptr != m_RootNode)
   {
      osg::StateSet*           ss       = m_RootNode->getOrCreateStateSet();
      osg::ref_ptr<osg::Image> dummyImg = new osg::Image();
      unsigned char*           data     = new unsigned char[4];
      data[0]                           = 255;
      data[1]                           = 255;
      data[2]                           = 255;
      data[3]                           = 255;
      dummyImg->setImage(1, 1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)data, osg::Image::USE_NEW_DELETE);

      osg::ref_ptr<osg::Texture2D> dummy = new osg::Texture2D(dummyImg.get());

      dummy->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
      dummy->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

      dummy->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
      dummy->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
      dummy->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
      ss->setTextureAttributeAndModes(0, dummy, osg::StateAttribute::OFF);
   }
}

void UtoViewer::ToggleMode() const
{
   osg::StateSet*    ss          = m_OSGViewer->getCamera()->getOrCreateStateSet();
   osg::PolygonMode* polyModeObj = dynamic_cast<osg::PolygonMode*>(ss->getAttribute(osg::StateAttribute::POLYGONMODE));
   if (!polyModeObj)
   {
      polyModeObj = new osg::PolygonMode;
      ss->setAttribute(polyModeObj);
   }
   osg::PolygonMode::Mode currentMode = polyModeObj->getMode(osg::PolygonMode::FRONT);
   switch (currentMode)
   {
   case osg::PolygonMode::FILL:
      polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      break;
   case osg::PolygonMode::LINE:
      polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::POINT);
      break;
   case osg::PolygonMode::POINT:
      polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      break;
   }
}

osg::Group* UtoViewer::GetCelestialNode()
{
   return mCelestialCamera.get();
}

void UtoViewer::Preinitialize()
{
#ifndef NO_OSG_EARTH_PREINITIALIZE
   osgEarth::Registry::instance()->getCapabilities();
#endif
}

void UtoViewer::ShapeAdded(UtoShape* aShapePtr)
{
   if (m_Filter)
   {
      m_Filter->OnAddSymbol(*aShapePtr, *this);
   }
   NewSymbolAdded(aShapePtr);
}

void UtoViewer::ShapeRemoved(UtoShape* aShapePtr)
{
   if (m_Filter)
   {
      m_Filter->OnRemoveSymbol(*aShapePtr, *this);
   }
}

void UtoViewer::BeginUserStat(const std::string& aName)
{
   if (m_OSGViewer)
   {
      std::string beginName   = "begin_" + aName;
      std::string endName     = "end_" + aName;
      std::string elapsedName = "elapsed_" + aName;
      int         frameNumber = m_OSGViewer->getViewerStats()->getLatestFrameNumber();
      double      currentTime = m_OSGViewer->elapsedTime();
      RegisterStat(aName, beginName, endName, elapsedName, currentTime);
      m_OSGViewer->getViewerStats()->setAttribute(frameNumber, beginName, currentTime);
   }
}

void UtoViewer::EndUserStat(const std::string& aName)
{
   if (m_OSGViewer)
   {
      std::string endName     = "end_" + aName;
      std::string elapsedName = "elapsed_" + aName;
      int         frameNumber = m_OSGViewer->getViewerStats()->getLatestFrameNumber();
      double      currentTime = m_OSGViewer->elapsedTime();
      double      elapsedTime = currentTime - GetStatStart(aName);
      m_OSGViewer->getViewerStats()->setAttribute(frameNumber, elapsedName, elapsedTime);
      m_OSGViewer->getViewerStats()->setAttribute(frameNumber, endName, currentTime);
   }
}

void UtoViewer::RegisterStat(const std::string& aName,
                             const std::string& aBeginName,
                             const std::string& aEndName,
                             const std::string& aElapsedName,
                             double             aTime)
{
   if (mRegisteredStats.find(aName) == mRegisteredStats.end())
   {
      osg::Vec4 blue(0.6, 0.6, 1.0, 1.0);
      mStatsHandler->addUserStatsLine(aName, blue, blue, aElapsedName, 1000.0f, true, false, aBeginName, aEndName, 100.0);
   }
   mRegisteredStats[aName] = aTime;
}

double UtoViewer::GetStatStart(const std::string& aName)
{
   if (mRegisteredStats.find(aName) != mRegisteredStats.end())
   {
      return mRegisteredStats[aName];
   }
   return 0.0;
}

void UtoViewer::EnableSmallFeatureCulling(bool aState)
{
   if (aState)
   {
      SetCullingMode(osg::CullSettings::DEFAULT_CULLING | osg::CullSettings::FAR_PLANE_CULLING |
                     osg::CullSettings::SMALL_FEATURE_CULLING);
   }
   else
   {
      SetCullingMode(osg::CullSettings::DEFAULT_CULLING | osg::CullSettings::FAR_PLANE_CULLING);
   }
}

void UtoViewer::AddSubordinate(UtoViewer* aChildPtr)
{
   m_RootNode->addChild(aChildPtr->GetRootNode());
}

void UtoViewer::RemoveSubordinate(UtoViewer* aChildPtr)
{
   m_RootNode->removeChild(aChildPtr->GetRootNode());
}
