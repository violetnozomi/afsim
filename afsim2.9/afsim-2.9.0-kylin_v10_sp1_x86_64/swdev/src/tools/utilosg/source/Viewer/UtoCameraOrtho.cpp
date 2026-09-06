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
// UtoCameraOrthoImp.cpp : implementation file

#include "UtoCameraOrtho.hpp"

#include <cassert>

#include "UtoEntity.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerProjection.hpp"

/* UTO Function Block **********************************************************

Function: UtoCameraOrthoImp

Description:
Constructor

*******************************************************************************/

UtoCameraOrtho::UtoCameraOrtho(UtoViewer* pOwner)
   : UtoCamera(pOwner)
   , m_fZoom(50.0)
   , m_bUpdateZoom(false)
   , m_bNoYaw(true)
   , m_Hither(0.0)
   , m_Yon(0.0)
{
   // Store the initial Z value.
   m_Yon = UtoCamera::Position().z();
}

/* UTO Function Block **********************************************************

Function: UtoCameraOrthoImp

Description:
Destructor

*******************************************************************************/

UtoCameraOrtho::~UtoCameraOrtho() {}

UtoPosition UtoCameraOrtho::Position() const
{
   // The camera follows the entities x & y when it is locked on an entity,
   // but it doesn't follow the z value.
   UtoPosition pos = UtoCamera::Position();
   pos.z()         = m_Yon;
   return pos;
}

void UtoCameraOrtho::SetPosition(UtoPosition pos)
{
   // update the projection if the camer's clipping plane needs updating
   // m_Yon is used when the ViewMatrix is updated, make sure it is set
   // before the base SetPosition is called.
   if (m_Yon != pos.z())
   {
      m_Yon = pos.z();
      UpdateProjection();
   }

   UtoCamera::SetPosition(pos);
}

UtoOrientation UtoCameraOrtho::Orientation() const
{
   return UtoCamera::Orientation();
}

void UtoCameraOrtho::SetOrientation(const UtoOrientation& orientation)
{
   UtoCamera::SetOrientation(orientation);
}

void UtoCameraOrtho::SetViewMatrix(const osg::Matrix& ViewMatrix)
{
   UtoCamera::SetViewMatrix(ViewMatrix);
}

bool UtoCameraOrtho::LockEntity(const UtoEntity& ent)
{
   bool ret = UtoCamera::LockEntity(ent);
   if (ret)
   {
      UpdateSceneView();
   }
   return ret;
}

const UtoEntity* UtoCameraOrtho::LockedEntity() const
{
   return UtoCamera::LockedEntity();
}

bool UtoCameraOrtho::UnlockEntity()
{
   bool ret = UtoCamera::UnlockEntity();
   if (ret)
   {
      UpdateSceneView();
   }
   return ret;
}

/* UTO Function Block **********************************************************

Function: Scroll

Description:
Updates the camera position base on the scroll distance and angle

*******************************************************************************/

bool UtoCameraOrtho::Scroll(double offset, double angle)
{
   // Update the camera position
   UtoPosition CurrentPos = Position();

   // Calculate the new position
   CurrentPos.x() += (offset * sin(DtoR(angle)));
   CurrentPos.y() += (offset * cos(DtoR(angle)));

   // This class's SetPosition updates the camera's z value, we don't want
   // that, just update the base class position.
   UtoCamera::SetPosition(CurrentPos);

   UpdateRelativePosition();

   return true;
}

/* UTO Function Block **********************************************************

Function: Zoom

Description:
Zooms the camera in by a given offset

*******************************************************************************/

bool UtoCameraOrtho::SetZoom(double fZoomLevel)
{
   if (fZoomLevel <= 0)
      return false;

   m_fZoom = fZoomLevel;
   m_Owner->SetUniform("UTO_OrthographicZoom", m_fZoom);


   m_bUpdateZoom = true;

   UpdateProjection();
   UpdateSceneView();

   // force a change in position in order to fire off a PositionChanged() event
   UtoCamera::SetPosition(UtoCamera::Position());

   return true;
}

/* UTO Function Block **********************************************************

Function: Zoom

Description:
Returns the Zoom level

*******************************************************************************/

double UtoCameraOrtho::Zoom() const
{
   return m_fZoom;
}

/* UTO Function Block **********************************************************

Function: CenterEntity

Description:
Set the camera position to center on the given entity position

*******************************************************************************/

bool UtoCameraOrtho::CenterEntity(UtoEntity& ent)
{
   UtoPosition NewPos = m_Owner->Projection()->CalcPoint(ent.BodyCF());

   // This class's SetPosition updates the camera's z value, we don't want
   // that, just update the base class position.
   UtoCamera::SetPosition(NewPos);
   return true;
}

bool UtoCameraOrtho::SetNearClip(double hither)
{
   m_Hither = hither;
   UpdateProjection();
   return true;
}

bool UtoCameraOrtho::SetFarClip(double yon)
{
   m_Yon = yon;
   UpdateProjection();
   return true;
}

bool UtoCameraOrtho::Set(double hither, double yon)
{
   m_Hither = hither;
   m_Yon    = yon;
   UpdateProjection();
   return true;
}

// Updates the Scene to the current camera position
bool UtoCameraOrtho::UpdateSceneView()
{
   /*   if((!m_SceneView.get()) && (!m_OSGViewer.get()))
   {
   return false;
   }*/

   // Set the view matrix directly.
   if (ViewMatrixActive())
   {
      m_Owner->SetViewMatrix(m_ExplicitViewMatrix);
      /*      if (m_SceneView.valid())
      {
      m_SceneView->setViewMatrix(m_ExplicitViewMatrix);
      }
      else
      {
      m_OSGViewer->getCamera()->setViewMatrix(m_ExplicitViewMatrix);
      }*/
      return true;
   }

   // Set the position and orientation

   UtoPosition CameraPos = Position();

   if (m_bNoYaw)
   {
      m_Owner->SetViewMatrixAsLookAt(osg::Vec3(CameraPos.x(), CameraPos.y(), m_Yon),
                                     osg::Vec3(CameraPos.x(), CameraPos.y(), 0.0f),
                                     osg::Vec3(0, 1, 0));
      /*      if (m_SceneView.valid())
      {
      m_SceneView->setViewMatrixAsLookAt(
      osg::Vec3(CameraPos.x, CameraPos.y, m_Yon),
      osg::Vec3(CameraPos.x, CameraPos.y, 0.0f),
      osg::Vec3(0, 1, 0));
      }
      else
      {
      m_OSGViewer->getCamera()->setViewMatrixAsLookAt(
      osg::Vec3(CameraPos.x, CameraPos.y, m_Yon),
      osg::Vec3(CameraPos.x, CameraPos.y, 0.0f),
      osg::Vec3(0, 1, 0));
      }*/
   }
   else
   {
      osg::Matrix matrix;
      if (m_ObservedEntity)
      {
         // calculate the angle to rotate
         double hpr[3];
         m_ObservedEntity->GetOrientationNED(hpr[0], hpr[1], hpr[2]);
         float Angle = DtoR(hpr[0] - Orientation().yaw);

         // entity data
         double xyz[3];
         m_ObservedEntity->GetLocationWCS(xyz);
         UtoPosition EntPos(xyz[0], xyz[1], xyz[2]);
         UtoPosition offset = CameraPos - EntPos;

         // Rotate the camera keeping the entity
         // at the same position on the screen

         matrix *= osg::Matrix::translate(-EntPos.x(), -EntPos.y(), 0);
         matrix *= osg::Matrix::rotate(Angle, osg::Vec3(0, 0, 1));
         matrix *= osg::Matrix::translate(-offset.x(), -offset.y(), -m_Yon);
      }
      else
      {
         float Angle = DtoR(Orientation().yaw);

         matrix *= osg::Matrix::translate(-CameraPos.x(), -CameraPos.y(), -m_Yon);
         matrix *= osg::Matrix::rotate(Angle, osg::Vec3(0, 0, 1));
      }

      m_Owner->SetViewMatrix(matrix);
      /*      if (m_SceneView.valid())
      {
      m_SceneView->setViewMatrix(matrix);
      }
      else
      {
      m_OSGViewer->getCamera()->setViewMatrix(matrix);
      }*/
   }

   return true;
}

bool UtoCameraOrtho::AttachSceneView(osgUtil::SceneView* SceneView)
{
   // Disable lighting & fog in 2D views.
   if (SceneView)
   {
      osg::StateSet* stateset = SceneView->getGlobalStateSet();
      assert(stateset);
      stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
      stateset->setMode(GL_FOG, osg::StateAttribute::OFF);
   }

   return UtoCamera::AttachSceneView(SceneView);
}

// Updates the projection matrix to the current settings
bool UtoCameraOrtho::UpdateProjection()
{
   /*   if(!(m_SceneView.get()) && (!m_OSGViewer.get()))
   return false;*/

   const osg::Viewport* viewport = m_Owner->GetViewport();
   /*   if (m_SceneView.valid())
   {
   viewport = m_SceneView->getViewport();
   }
   else
   {
   viewport = m_OSGViewer->getCamera()->getViewport();
   }*/
   if (viewport)
   {
      double HalfZoomWidth  = m_fZoom * viewport->width() / 2.0;
      double HalfZoomHeight = m_fZoom * viewport->height() / 2.0;
      m_Owner->SetProjectionMatrixAsOrtho(-HalfZoomWidth, HalfZoomWidth, -HalfZoomHeight, HalfZoomHeight, m_Hither, m_Yon + 1.0);
   }
   /*   if (m_SceneView.valid())
   {
   m_SceneView->setProjectionMatrixAsOrtho(-HalfZoomWidth, HalfZoomWidth,
   -HalfZoomHeight, HalfZoomHeight,
   m_Hither, m_Yon+1.0);
   }
   else
   {
   m_OSGViewer->getCamera()->setProjectionMatrixAsOrtho(-HalfZoomWidth, HalfZoomWidth,
   -HalfZoomHeight, HalfZoomHeight,
   m_Hither, m_Yon+1.0);
   }*/
   return true;
}

void UtoCameraOrtho::Dump(std::ostream& o)
{
   UtoCamera::Dump(o);
}

void UtoCameraOrtho::SetCameraFlags(long mask, long state)
{
   if (mask & NO_YAW)
   {
      bool nstate = state & NO_YAW ? true : false;
      if (nstate != m_bNoYaw)
      {
         m_bNoYaw = nstate;
         UpdateSceneView();
      }
   }
}

bool UtoCameraOrtho::UpdateZoom()
{
   return m_bUpdateZoom;
}

void UtoCameraOrtho::ClearUpdateZoom()
{
   m_bUpdateZoom = false;
}


///////////////////////////////////////////////////////////////////////////////
//// UtoCameraOrtho.cpp : implementation file
//
//#include "StdAfx.h"
//#include "UtoCameraOrtho.hpp"
//#include "UtoCameraOrthoImp.hpp"
//#include "UtoEntity.hpp"
//
///**
//\ingroup view
//\class UtoCameraOrtho
//\brief 2D orthongonal camera.
//
// An orthogonal camera object is accessed from UtoViewer, and is never
// created directly by the user.  An orthogonal camera is used to develop
// top-down visual displays where typically the symbols are uniform size.
//
// The orthogonal camera has a position and an orientation, which can be
// set and retrieved.  The camera position dictates how the scene will be
// viewed while the yaw and yaw lock control which direction is the top
// of the screen.
//
// The orthogonal camera also allows scrolling and zooming with the
// Scroll and Zoom methods respectively.  The camera will automatically
// maintain its aspect ratio determined by the last Zoom call when the
// viewer is resized.
//
// The orthogonal camera supports locking onto an entity.  This feature
// eases the task of keeping an entity in the same relative location on
// the viewer by moving the camera with entity position changes.  The
// camera can also be unlocked to stop this behavior.  Lastly, the camera
// can be centered on an entity.  Combining the camera lock and centering
// features provides an easy process to keep any entity locked in the
// center of the viewer.
//
//\note
//
// The z or Up coordinate is used to determine the layering of the
// entites, allowing one entity to be displayed above another entity.
// The camera will display entites that are between -1 and Camera Height.
// It appears to depend on the computer if this is an exclusive or
// inclusive range, be assured that if you have an entity or object above
// or below this range it won't be displayed.
//
//*/
// class UtoCameraOrtho;
//
///** @name Type Definitions */
////@{
///**
//\enum UtoCameraOrtho::Flags
//\brief Camera Orientation Flags
//*/
//
///**
//\var UtoCameraOrtho::Flags UtoCameraOrtho::NO_YAW
//\brief Removes the ability of the camera to do yaw rotations.
//*/
////@}
//
// UtoCameraOrtho::UtoCameraOrtho()
//{
//   TRACE ("UtoCameraOrtho");
//}
//
///** @name Destructors */
////@{
///**
//\brief Destroys the object.
//
// This destroys all the subordinate objects for this object.
//*/
// UtoCameraOrtho::~UtoCameraOrtho()
//{
//   TRACE ("~UtoCameraOrtho");
//}
////@}
//
///** @name Accessors */
////@{
//
///**
//\fn UtoPosition UtoCameraOrtho::Position () const
//\brief Returns the camera position.
//
// Returns the camera position.
//\return UtoPosition - current camera position.
//*/
//
///**
//\fn UtoOrientation UtoCameraOrtho::Orientation()
//\brief Returns the camera orientation.
//
// Returns the camera orientation.
//\return UtoOrientation - the current camera orientation.
//*/
//
///**
//\fn double UtoCameraOrtho::Zoom()
//\brief Returns the current zoom level of the camera.
//
// Returns the current zoom level of the camera.  The zoom level is
// in feet per pixel.
//*/
//
///**
//\fn const UtoEntity* UtoCameraOrtho::LockedEntity () const
//\brief Returns the entity the camera is currently locked on to.
//
// Returns the entity the camera is currently locked on to.  This will return NULL
// if the camera is not currently locked to an entity.
//*/
////@}
//
///** @name Operations */
////@{
//
///**
//\fn bool UtoCameraOrtho::LockEntity (const UtoEntity& ent)
//\brief Locks the camera on to an entity.
//
// Locks the camera on to an entity, so that when the entity moves, the camera
// automatically moves with it.  The camera will maintain a relative position to the
// locked entity, so the entity does not get centered with this call.  In order to
// keep the entity centered and locked call CenterEntity as well as this method.
// If the camera was previously locked then this call will change the lock to the new entity.
//\param ent the entity to lock on to.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera locked on.</td></tr>
//                  <tr><td>false</td><td>Camera not locked on.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraOrtho::UnlockEntity ()
//\brief Unlocks the camera from the locked state.
//
// Unlocks the camera from the locked state.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera unlocked.</td></tr>
//                  <tr><td>false</td><td>Camera not unlocked or no entity previously locked on.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraOrtho::Scroll (double offset, double angle)
//\brief Scrolls the camera by the specified amount.
//
// Moves the camera position in the direction specified by angle, and
// by the distance specified by offset.
//\param offset the distance to move the camera in feet.
//\param angle the clockwise angle from the +North in degrees.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera moved.</td></tr>
//                  <tr><td>false</td><td>Camera not moved.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraOrtho::SetZoom (double factor)
//\brief Zooms the camera to the specified factor.
//
// Changes the camera altitude so that the feet per pixel factor is achieved.
//\param factor the feet per pixel factor.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera zoom set.</td></tr>
//                  <tr><td>false</td><td>Camera zoom not set.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraOrtho::CenterEntity (const UtoEntity& ent)
//\brief Centers the camera on an entity.
//
// Moves the camera position so that the specified entity is centered.
//\param ent the entity to center on.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera centered.</td></tr>
//                  <tr><td>false</td><td>Camera not centered.</td></tr>
//               </table>
//*/
//
///**
//\fn void UtoCameraOrtho::SetCameraFlags (long mask, long state)
//\brief Sets and clears the shape flags.
//
// These flags are only used when the camera has been locked to an
// entity using the LockEntity function call.  Use the SetCameraFlags
// function to enable or disable entity orientation locks with the
// camera.  To set North to be the top of the screen set NO_YAW, to
// set the top of the screen relative to the entity's Yaw, disable that
// flag.
//
//<table>
//   <tr><td><b>Flag</td><td>Description</b></td></tr>
//   <tr><td>NO_YAW</td><td>Turns off entity yaw effects on this shape.
//                          Otherwise, the camera will yaw when the locked
//                          entity yaws.</td></tr>
//</table>
//
// If the NO_YAW flag is set, North will always be to the top of the
// screen.  Otherwise North will be rotated clockwise by the camera yaw.
// If the camera is locked to an entity, the entity's yaw will be added
// to the camera's yaw.  By setting the camera yaw to zero, the locked
// entity will point to the top of the screen.
//
// The camera rotation and locking to an entity needs some additional
// explanation and diagrams.  The goal of the orthographic camera
// rotations is to keep the locked entity in the same viewport location
// before and after the lock.
//
//*Image 1: Viewport centered on the world origin*
//
//\image html ortho_initial.gif
//\image html ortho_key.gif
//
// For this example the world has two entites and the camera.  The camera
// is locked onto the red entity.  Each graph has a green square
// representing the viewport which is what would be displayed on the
// screen if no further transformations were to take place.  The graph's
// coordinate system says centered of the viewport as the transforms take
// place.  Image 1 is the untranslated and unrotated scene.  Note the
// initial image would be empty.
//
//*Image 2: Final image without rotation*
//
//\image html ortho_norotate.gif
//
// If the NO_YAW flag is set, there isn't any rotation and the rendered
// image will have the camera at the same point as the center of the
// viewport.  The viewport is translated to the camera location which is
// the distance of the green arrows.
//
//*Image 3: Centering the locked entity*
//
//\image html ortho_step1.gif
//
// The more complicated case is when NO_YAW is not set and the entity is
// already pointing a direction other than North.  These steps are
// internal to Uto, but they are important to understand some details on
// how to use the orthographic camera.  The first step is to translate
// the viewport so the locked entity is in the center as shown in image
// three.
//
//*Image 4: Rotating the viewport*
//
//\image html ortho_step2.gif
//
// The next step is to rotate the entity so its heading is to the top of
// the screen show in image four.  In this example the world is being
// rotated while the viewport is staying fixed.
//
//*Image 5: Final rotated view*
//
//\image html ortho_step3.gif
//
// The final step is to move the locked entity to its final position.
// Image two could have been composed by first translating the viewport
// over the red entity, and then translating from the red entity to the
// camera location.  Image four is created in the same manner only a
// rotate step is added before the viewport is displaced by the amount
// from the entity to the camera.  It is displaced by the original amount
// and direction between the entity and camera but only after the world
// has been rotated.  What can be confusing about this method is that the
// camera most likely won't be in the center of the viewport anymore.
//
// When the camera is locked into an entity without NO_YAW set the entity
// will stay in the same position on the screen, but the rest of the
// world will be rotated around the entity.  One operation is to lock
// onto a different entity.  This can be challenging.  The most logical
// result is for the new entity to stay on the same pixel on the screen
// with the world rotated around it.  This can be accomplished by storing
// the current world position of the new entity, using WorldToWindow to
// find the pixel that it is under, unlock the current entity, use
// WindowToWorld to find out where the original pixel currently is in
// world coordinates, and then move the camera so the new entity is moved
// to that screen position.
//
// When the map is to be dragged with the mouse and North is to the top
// of the viewport the camera is moved in the opposite direction as the
// mouse.  When the camera is rotated, however, if the application
// doesn't compensate for the rotation the mouse's up/right/down/left
// will move the map north/east/south/west which won't follow the mouse.
// If the camera is rotated without being locked onto an entity, the
// application needs to convert the distance the mouse moved into polar
// coordinates, adjust the angle to the camera rotation, and then convert
// back for the x,y camera displacements.  As it turns out when the
// camera is locked onto an entity and the entity points to the top of
// the screen no rotation compensation is needed because, as the example
// shows, the camera movement happens after the rotation putting the x,y
// axis inline with the x,y of the viewport.
//
//\param mask a bit mask that will be used to set the shape flags.
//\param state a state mask that will be used to set the shape flags.
//*/
//
///**
//\fn void UtoCameraOrtho::SetPosition (UtoPosition pos)
//\brief Sets the camera position.
//
// Sets the camera position.
//\param pos the new camera position.
//*/
//
///**
//\fn void UtoCameraOrtho::SetOrientation (const UtoOrientation &orientation )
//\brief Sets the camera orientation.
//
// Sets the camera orientation.  For orthographic cameras, only the yaw component is used.
//\param orientation orientation the new orientation of the camera.
//*/
////@}
//
