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

//
///////////////////////////////////////////////////////////////////////////////
//// UtoCameraPerspective.cpp : implementation file
//
//#include "StdAfx.h"
//#include "UtoCameraPerspective.hpp"
//#include "UtoCameraPerspectiveImp.hpp"
//#include "UtoEntity.hpp"
//
///**
//\ingroup view
//\class UtoCameraPerspective
//\brief 3D perspective camera.
//
// A perspective camera object is accessed from UtoViewer, and is never
// created directly by the user.  A perspective camera is typically used to
// develop stealth-eye or 3D displays.  These displays can be used to
// represent a realistic representation of the scene.
//
// A perspective camera has position and orientation which can be set and
// retrieved.  These spatial data dictate how the scene will be viewed.
//
// The perspective camera allows modification of the field-of-view, and near
// and far clipping planes.  This information also affects how the scene is
// viewed.  By default the camera uses a 45 degree field-of-view, and 1 foot
// near plane, and 40,000 foot far plane.
//
// The perspective camera supports locking onto an entity.  This feature eases
// the task of moving the camera along with an entity.  For example, a
// pilots-eye display would have the camera tied to a point in the cockpit
// corresponding to the pilots head.  In addition, the camera also can be
// pointed by setting the orientation directly, or calling one of the LookAt
// methods.  The LookAt methods provide a convenient way to point the camera.
// A position or another entity can be looked at.  The camera can also be
// unlocked.
//
// Lastly, the camera supports flags which modify some of the other methods.
// Currently, the camera has flags for turning off the effects of orientation.
// For instance, NO_YAW , NO_PITCH, and NO_ROLL.
//*/
// class UtoCameraPerspective;
//
///** @name Type Definitions */
////@{
///**
//\enum UtoCameraPerspective::Flags
//\brief Camera Orientation Flags
//*/
//
///**
//\var UtoCameraPerspective::Flags UtoCameraPerspective::NO_ROLL
//\brief Removes the ability of the camera to do roll rotations.
//*/
//
///**
//\var UtoCameraPerspective::Flags UtoCameraPerspective::NO_PITCH
//\brief Removes the ability of the camera to do pitch rotations.
//*/
//
///**
//\var UtoCameraPerspective::Flags UtoCameraPerspective::NO_YAW
//\brief Removes the ability of the camera to do yaw rotations.
//*/
////@}
//
// UtoCameraPerspective::UtoCameraPerspective ()
//{
//   TRACE ("UtoCameraPerspective");
//}
//
///** @name Destructors */
////@{
///**
//\brief Destroys the object.
//
// This destroys all the subordinate objects for this object.
//*/
// UtoCameraPerspective::~UtoCameraPerspective ()
//{
//   TRACE ("~UtoCameraPerspective");
//}
////@}
//
///** @name Accessors */
////@{
//
///**
//\fn UtoPosition UtoCameraPerspective::Position () const
//\brief Returns the camera position.
//
// Returns the camera position.
//\return UtoPosition - the current camera position.
//*/
//
///**
//\fn UtoOrientation UtoCameraPerspective::Orientation()
//\brief Returns the camera orientation.
//
// Returns the camera orientation.
//\return UtoOrientation - the current camera orientation.
//*/
//
///**
//\fn const UtoEntity* UtoCameraPerspective::LookAtEntity()
//\brief Returns the look at entity.
//
// Returns the look at entity.  NULL will be returned if the camera is not currently
// looking at an entity.
//\return const UtoEntity * - the entity currently being looked at.
//*/
//
///**
//\fn const UtoPosition UtoCameraPerspective::LookAtPosition()
//\brief Returns the position of the LookAtEntity.
//
// Returns the position of the LookAtEntity if one exists.
//\return const UtoPosition - the position of the LookAtEntity.
//*/
//
///**
//\fn double UtoCameraPerspective::FOV () const
//\brief Returns the current field of view.
//
// Returns the current field of view.
//\return double - the current field of view.
//*/
//
///**
//\fn double UtoCameraPerspective::NearClip () const
//\brief Returns the current near clipping plane value in feet.
//
// Returns the current near clipping plane value in feet.
//\return double - the current near clipping plane value.
//*/
//
///**
//\fn double UtoCameraPerspective::FarClip () const
//\brief Returns the current far clipping plane value in feet.
//
// Returns the current far clipping plane value in feet.
//\return double - the current far clipping plane value.
//*/
//
///**
//\fn const UtoEntity* UtoCameraPerspective::LockedEntity () const
//\brief Returns the entity locked on by the camera.
//
// Returns the entity locked on by the camera.  If no entity is
// currently locked on, this function will return NULL.
//\return const UtoEntity * - the currently locked on entity.
//*/
////@}
//
///** @name Operations */
////@{
//
///**
//\fn void UtoCameraPerspective::SetPosition (UtoPosition pos)
//\brief Sets the camera position.
//
// Sets the camera position.
//\param pos the new camera position.
//*/
//
///**
//\fn void UtoCameraPerspective::SetOrientation( UtoOrientation orientation )
//\brief Sets the camera orientation.
//
// Sets the camera orientation.
//\param orientation - the new orientation.
//*/
//
///**
//\fn bool UtoCameraPerspective::LookAt (const UtoPosition& LookAtPoint)
//\brief Looks at the specified point.
//
// Looks at the specified point.  Moves the camera orientation so that it is looking
// at the specified point in space.  The camera remains 'locked' looking at this point
// as the camera is moved.
//\param LookAtPoint - the new point the camera is too look at.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera set successfully.</td></tr>
//                  <tr><td>false</td><td>Camera not set successfully.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::LookAt (const UtoEntity& ent)
//\brief Looks at the specified entity.
//
// Looks at the specified entity.  Moves the camera orientation so that it is looking
// at the specified entity in space.  The camera remains 'locked' looking at this entity
// as the camera is moved.  To cancel this mode, simply call SetOrientation, or any of
// the LookAt overloads.
//\param ent - the new entity the camera is too look at.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera set successfully.</td></tr>
//                  <tr><td>false</td><td>Camera not set successfully.</td></tr>
//               </table>
//*/
//
///**
//\fn void UtoCameraPerspective::SetCameraFlags (long mask, long state)
//\brief Sets and clears the camera flags.
//
// Sets and clears the camera flags.
//
// These flags are only used when the camera has been locked to an entity using
// the LockEntity function call.  Use the SetCameraFlags function to enable or disable
// entity orientation locks with the camera.  For instance, in a pilot's eye view it makes
// sense to enable yaw, pitch, and roll; however, in a chase view it may only be desirable to
// follow the yaw, or heading of the entity.
//
//<table>
//   <tr><td><b>Flag</td><td>Description</b></td></tr>
//   <tr><td>NO_ROLL</td><td>Turns off entity roll effects on this camera.  Otherwise, the camera will
//                           roll when the locked entity rolls.</td></tr>
//   <tr><td>NO_PITCH</td><td>Turns off entity pitch effects on this camera.  Otherwise, the camera will
//                            pitch when the locked entity pitches.</td></tr>
//   <tr><td>NO_YAW</td><td>Turns off entity yaw effects on this camera.  Otherwise, the camera will yaw
//                          when the locked entity yaws.</td></tr>
//</table>
//
//\param mask a bit mask that will be used to set the shape flags.
//\param state a state mask that will be used to set the shape flags.
//*/
//
///**
//\fn bool UtoCameraPerspective::Rotate (unsigned int axis, double angle)
//\brief Rotates the camera about an axis.
//
// Moves the camera orientation around the specified axis, by the amount specified
// by angle.
//
//\note Not Implemented
//
//\param axis the X, Y, or Z axis.
//\param angle the clockwise angle about the specified axis in degrees.
//\return bool - <table>
//                  <tr><td>true</td><td>Rotation successful.</td></tr>
//                  <tr><td>false</td><td>Rotation not successful.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::LockEntity (const UtoEntity& ent)
//\brief Locks the camera on to an entity.
//
// Locks the camera on to an entity, so that when the entity moves, the camera
// automatically moves with it.  The camera will maintain a relative position to the
// locked entity.
//\param ent the entity to lock on to.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera locked.</td></tr>
//                  <tr><td>false</td><td>Camera not locked.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::UnlockEntity()
//\brief Unlocks the camera from the locked state.
//
// Unlocks the camera from the locked state.  This method will return flase if the
// camera was not previously locked.
//\return bool - <table>
//                  <tr><td>true</td><td>Camera unlocked.</td></tr>
//                  <tr><td>false</td><td>Camera not unlocked or previous locked entity not set.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::SetFOV (double fov)
//\brief Sets the current field of view.
//
// Sets the current field of view.
//\param fov the new field of view.
//\return bool - <table>
//                  <tr><td>true</td><td>FOV set successfully.</td></tr>
//                  <tr><td>false</td><td>FOV not set successfully.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::SetNearClip (double near)
//\brief Sets the current near clipping plane value.
//
// Sets the current near clipping plane value.
//\param near the new near clipping plane value.
//\return bool - <table>
//                  <tr><td>true</td><td>Near plane set successfully.</td></tr>
//                  <tr><td>false</td><td>Near plane not set successfully.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::SetFarClip (double far)
//\brief Sets the current far clipping plane value.
//
// Sets the current far clipping plane value.
//\param far the new far clipping plane value.
//\return bool - <table>
//                  <tr><td>true</td><td>Far plane set successfully.</td></tr>
//                  <tr><td>false</td><td>Far plane not set successfully.</td></tr>
//               </table>
//*/
//
///**
//\fn bool UtoCameraPerspective::Set (double fov, double near, double far)
//\brief Sets the field of view, near and far clipping plane values.
//
// Sets the field of view, near and far clipping plane values.
//\param fov the new field of view.
//\param near the new near clipping plane value.
//\param far the new far clipping plane value.
//\return bool - <table>
//                  <tr><td>true</td><td>Values set successfully.</td></tr>
//                  <tr><td>false</td><td>Values not set successfully.</td></tr>
//               </table>
//*/
////@}
//


/////////////////////////////////////////////////////////////////////////////
// UtoCameraPerspectiveImp.cpp : implementation file

#include "UtoCameraPerspective.hpp"

#include <cassert>

#include "UtoEntity.hpp"
#include "UtoPriv.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerProjection.hpp"

const float UtoCameraPerspective::m_GridExtentX = 10000.0f;
const float UtoCameraPerspective::m_GridExtentY = 10000.0f;
const float UtoCameraPerspective::m_GridExtentZ = 10000.0f;

UtoCameraPerspective::UtoCameraPerspective(UtoViewer* pOwner)
   : UtoCamera(pOwner)
   , m_LookAtEntity(nullptr)
   , m_LookAtActive(false)
   , m_Flags(0)
   , m_FOV(45.0f)
   , m_Hither(1.0f)
   , m_Yon(40000.0f)
{
}

UtoCameraPerspective::~UtoCameraPerspective()
{
   CancelLookAt();
}

UtoPosition UtoCameraPerspective::Position() const
{
   return UtoCamera::Position();
}

void UtoCameraPerspective::SetPosition(UtoPosition pos)
{
   CalculateOffset(pos);
   UtoCamera::SetPosition(pos);
}

void UtoCameraPerspective::SetOrientation(UtoOrientation orientation)
{
   CancelLookAt();
   UtoCamera::SetOrientation(orientation);
}

void UtoCameraPerspective::SetViewMatrix(const osg::Matrix& ViewMatrix, const double aX, const double aY, const double aZ)
{
   CancelLookAt();
   UtoCamera::SetPosition(UtoPosition(aX, aY, aZ));
   UtoCamera::SetViewMatrix(ViewMatrix);
}

UtoOrientation UtoCameraPerspective::Orientation() const
{
   return UtoCamera::Orientation();
}

bool UtoCameraPerspective::LockEntity(const UtoEntity& ent)
{
   return UtoCamera::LockEntity(ent);
}

bool UtoCameraPerspective::UnlockEntity()
{
   return UtoCamera::UnlockEntity();
}

const UtoEntity* UtoCameraPerspective::LockedEntity() const
{
   return UtoCamera::LockedEntity();
}

bool UtoCameraPerspective::LockEntityOrientation(const UtoEntity& ent)
{
   // CancelLookAt();
   return UtoCamera::LockEntityOrientation(ent);
}

bool UtoCameraPerspective::UnlockEntityOrientation()
{
   return UtoCamera::UnlockEntityOrientation();
}

/* UTO Function Block **********************************************************

Function: LookAt

Description:
   Looks at a point

*******************************************************************************/

bool UtoCameraPerspective::LookAt(const UtoPosition& LookAtPoint)
{
   // Clean up existing look at
   CancelLookAt();

   ClearExplicitViewMatrix();

   // <LookAtPoint> must be in the viewer coordinate system.
   m_LookAtPoint = LookAtPoint;

   m_LookAtActive = true;

   return UpdateSceneView();
}

/* UTO Function Block **********************************************************

Function: LookAt

Description:
   Looks at an entity

*******************************************************************************/

bool UtoCameraPerspective::LookAt(const UtoEntity& ent)
{
   // Clean up existing look at
   CancelLookAt();

   //@@ Remove const cast
   UtoEntity* ent2 = const_cast<UtoEntity*>(&ent);

   // Set the callback to the entity for position, orientation, and removal
   ent2->connect(UtoEntity::PositionChangedEvent(), this, &UtoCameraPerspective::EntityLookAtCB);
   ent2->connect(UtoEntity::PosnOrientChangedEvent(), this, &UtoCameraPerspective::EntityLookAtCB);
   ent2->connect(UtoEntity::DestroyedEvent(), this, &UtoCameraPerspective::LookAtEntityRemovedCB);

   // Store this entity
   m_LookAtEntity = ent2;
   m_LookAtPoint  = m_Owner->Projection()->CalcPointNoOffset(m_LookAtEntity->BodyCF());

   m_LookAtActive = true;

   UpdateSceneView();

   return true;
}

bool UtoCameraPerspective::Rotate(unsigned int axis, double angle)
{
   return false;
}

bool UtoCameraPerspective::SetFOV(double fov)
{
   m_FOV = fov;
   UpdateProjection();
   return true;
}

bool UtoCameraPerspective::SetNearClip(double hither)
{
   m_Hither = hither;
   UpdateProjection();
   return true;
}

bool UtoCameraPerspective::SetFarClip(double yon)
{
   m_Yon = yon;
   UpdateProjection();
   return true;
}

bool UtoCameraPerspective::Set(double fov, double hither, double yon)
{
   m_FOV    = fov;
   m_Hither = hither;
   m_Yon    = yon;
   UpdateProjection();
   return false;
}

/* UTO Function Block **********************************************************

  Function: UpdateSceneView

    Description:
    Updates the view matrix

*******************************************************************************/

bool UtoCameraPerspective::UpdateSceneView()
{
   /*   if((m_SceneView.get() == NULL) && (m_OSGViewer.get() == NULL))
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
   }
   else if (m_ObservedEntity)
   {
      // Check for the limits
      UtoPosition EntityPos = m_Owner->Projection()->CalcPointNoOffset(m_ObservedEntity->BodyCF());
      UtoPosition FinalCamPos(EntityPos + m_RelativePosition);

      CalculateOffset(FinalCamPos);
      if (m_LookAtActive)
      {
         UtoPosition offset         = m_Owner->Projection()->Offset();
         UtoPosition adjustCamPos   = FinalCamPos + offset;
         UtoPosition adjustedLookAt = m_LookAtPoint + offset;
         // Note: when rotate is implemented the up vector for the camera
         // will need to be calculated.
         m_Owner->SetViewMatrixAsLookAt(adjustCamPos,              // eye
                                        adjustedLookAt,            // center
                                        osg::Vec3(0.0, 0.0, 1.0)); // up
      }
      else
      {
         // Compute the view matrix: These matrix operations are read
         // backwards with the last multiplications being applied to the
         // identity matrix.  For example the operation that needs to be
         // done first is yaw about the center of the camera and so it is
         // the last multiplication in the code.  The order of rotations
         // is yaw, pitch, roll, so the C++ code first does roll, then
         // pitch, and finally yaw.
         //
         // Reading the operations backwards this code can be thought about
         // by placing the entity at the GL origin as well as the camera.
         // The camera's yaw, pitch, and roll are applied for camera
         // pointing, it is then translated away from the origin to
         // position the camera around the entity, the next set of
         // rotations are applied to both the camera and entity causing the
         // camera to rotate around the entity as the entity is rotated,
         // the last step is to translate the entity to its position in the
         // world, then translating the camera to maintain the relative
         // distance from the entity.

         UtoOrientation EntOrient;
         UtoPosition    EntPos = m_Owner->Projection()->CalcPointOrient(m_ObservedEntity->BodyCF(), EntOrient);

         osg::Matrix viewMatrix;

         // Entity Translation and Rotation
         viewMatrix *= viewMatrix.translate(-EntPos);


         // Add the desired rotations from the entity
         if (!(m_Flags & UtoCameraPerspective::NO_YAW) && EntOrient.yaw)
         {
            viewMatrix *= osg::Matrix::rotate(DtoR(EntOrient.yaw), YawAxis);
         }
         if (!(m_Flags & UtoCameraPerspective::NO_PITCH) && EntOrient.pitch)
         {
            viewMatrix *= osg::Matrix::rotate(DtoR(EntOrient.pitch), PitchAxis);
         }
         if (!(m_Flags & UtoCameraPerspective::NO_ROLL) && EntOrient.roll)
         {
            viewMatrix *= osg::Matrix::rotate(DtoR(EntOrient.roll), RollAxis);
         }

         UtoOrientation CameraOrient = Orientation();
         double         roll = DtoR(CameraOrient.roll), pitch = DtoR(CameraOrient.pitch), yaw = DtoR(CameraOrient.yaw);

         // Camera Translation and Rotation
         viewMatrix *= osg::Matrix::translate(-m_RelativePosition);

         viewMatrix *= osg::Matrix::rotate(yaw, YawAxis);
         viewMatrix *= osg::Matrix::rotate(pitch, PitchAxis);
         viewMatrix *= osg::Matrix::rotate(roll, RollAxis);
         viewMatrix *= osg::Matrix::rotate(-M_PI / 2, osg::Vec3(1, 0, 0));

         m_Owner->SetViewMatrix(viewMatrix);
      }
   }
   else
   {
      // Adjusted Camera Position and look at point
      UtoPosition offset    = m_Owner->Projection()->Offset();
      UtoPosition CameraPos = Position() + offset;

      if (m_LookAtActive)
      {
         UtoPosition adjustedLookAt = m_LookAtPoint + offset;
         m_Owner->SetViewMatrixAsLookAt(CameraPos,      // eye
                                        adjustedLookAt, // center
                                        osg::Vec3(0, 0, 1));
         /*         if (m_SceneView.valid())
                  {
                     m_SceneView->setViewMatrixAsLookAt(
                        CameraPos, // eye
                        adjustedLookAt, // center
                        osg::Vec3(0, 0, 1)); // up
                  }
                  else
                  {
                     m_OSGViewer->getCamera()->setViewMatrixAsLookAt(
                        CameraPos, // eye
                        adjustedLookAt, // center
                        osg::Vec3(0, 0, 1)); // up
                  }*/
      }
      else
      {
         UtoOrientation CameraOrient = Orientation();
         double         roll = DtoR(CameraOrient.roll), pitch = DtoR(CameraOrient.pitch), yaw = DtoR(CameraOrient.yaw);
         osg::Matrix    viewMatrix;

         // create a matrix translated to position
         viewMatrix *= osg::Matrix::translate(-CameraPos);

         viewMatrix *= osg::Matrix::rotate(yaw, YawAxis);
         viewMatrix *= osg::Matrix::rotate(pitch, PitchAxis);
         viewMatrix *= osg::Matrix::rotate(roll, RollAxis);
         viewMatrix *= osg::Matrix::rotate(-M_PI / 2, osg::Vec3(1, 0, 0));

         m_Owner->SetViewMatrix(viewMatrix);
         /*         if (m_SceneView.valid())
                  {
                     m_SceneView->setViewMatrix(viewMatrix);
                  }
                  else
                  {
                     m_OSGViewer->getCamera()->setViewMatrix(viewMatrix);
                  }*/
      }
   }
   return true;
}

/* UTO Function Block **********************************************************

  Function: UpdateProjection

    Description:
    Updates the projection matrix

*******************************************************************************/

bool UtoCameraPerspective::UpdateProjection()
{
   /*   if((!m_SceneView.get()) && (!m_OSGViewer.get()))
         return false;*/

   // reset the projection matrix, needs to be called when ...
   // when the perspective camera is selected
   // the window's size changes(aspect)
   // the near or far clip values change
   // the field of view has changed
   // NOTE!: unlike most other OpenSceneGraph arguments the field of view
   // expects to be in Degrees.
   m_Owner->SetProjectionMatrixAsPerspective(m_FOV, m_Owner->GetViewport()->aspectRatio(), m_Hither, m_Yon);
   /*   if (m_SceneView.valid())
      {
         m_SceneView->setProjectionMatrixAsPerspective(m_FOV,
            m_SceneView->getViewport()->aspectRatio(), m_Hither, m_Yon);
      }
      else
      {
         m_OSGViewer->getCamera()->setProjectionMatrixAsPerspective(m_FOV,
            m_OSGViewer->getCamera()->getViewport()->aspectRatio(), m_Hither, m_Yon);
      }*/
   return true;
}

/* UTO Function Block **********************************************************

Function: EntityLookAtCB

Description:
   Call back for looking at entities

*******************************************************************************/

void UtoCameraPerspective::EntityLookAtCB(UtoEntity* entity)
{
   m_LookAtPoint = m_Owner->Projection()->CalcPointNoOffset(entity->BodyCF());

   UpdateSceneView();
}


void UtoCameraPerspective::Dump(std::ostream& o)
{
   UtoCamera::Dump(o);
}

/* UTO Function Block **********************************************************

Function: CancelLookAt

Description:
   Removes observation of an entity

*******************************************************************************/

void UtoCameraPerspective::CancelLookAt()
{
   if (m_LookAtEntity)
   {
      m_LookAtEntity->disconnect(UtoEntity::PositionChangedEvent(), this, &UtoCameraPerspective::EntityLookAtCB);
      m_LookAtEntity->disconnect(UtoEntity::PosnOrientChangedEvent(), this, &UtoCameraPerspective::EntityLookAtCB);
      m_LookAtEntity->disconnect(UtoEntity::DestroyedEvent(), this, &UtoCameraPerspective::LookAtEntityRemovedCB);
      m_LookAtEntity = nullptr;
   }

   m_LookAtActive = false;
}

/* UTO Function Block **********************************************************

Function: LookAtEntityRemovedCB

Description:
   Cancels the look at on the removed entity

*******************************************************************************/

void UtoCameraPerspective::LookAtEntityRemovedCB(UtoEntity* entity)
{
   CancelLookAt();
}

const UtoEntity* UtoCameraPerspective::LookAtEntity()
{
   if (m_LookAtEntity)
      return m_LookAtEntity;
   else
      return nullptr;
}

const UtoPosition UtoCameraPerspective::LookAtPosition()
{
   return m_LookAtPoint;
}

void UtoCameraPerspective::SetCameraFlags(long mask, long state)
{
   m_Flags &= ~mask | state;
   m_Flags |= mask & state;
}

void UtoCameraPerspective::CalculateOffset(UtoPosition& CameraPos)
{
   // only update if the perspective camera is active, otherwise the
   // orthographic scene is drawn offset

   UtoPosition Offset;
   UtoPosition TempCameraPos = CameraPos + m_Owner->Projection()->Offset();

   int diffX = int(TempCameraPos.x() / m_GridExtentX);
   int diffY = int(TempCameraPos.y() / m_GridExtentY);
   int diffZ = int(TempCameraPos.z() / m_GridExtentZ);

   if ((diffX != 0) || (diffY != 0) || (diffZ != 0))
   {
      // Find the absolute position
      diffX = int(CameraPos.x() / m_GridExtentX);
      diffY = int(CameraPos.y() / m_GridExtentY);
      diffZ = int(CameraPos.z() / m_GridExtentZ);

      Offset.x() = -diffX * m_GridExtentX;
      Offset.y() = -diffY * m_GridExtentY;
      Offset.z() = -diffZ * m_GridExtentZ;

      m_Owner->Projection()->SetOffset(Offset);
      m_Owner->UpdateOffsets();
   }
}

bool UtoCameraPerspective::AttachSceneView(osgUtil::SceneView* SceneView)
{
   // Set the offset
   UtoPosition tempPos;
   CalculateOffset(tempPos);

   // The orthographic camera disables lighting, make sure it is set
   // back to the default mode.
   if (SceneView)
   {
      osg::StateSet* stateset = SceneView->getGlobalStateSet();
      assert(stateset);
      stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
      stateset->setMode(GL_FOG, osg::StateAttribute::INHERIT);
   }

   return UtoCamera::AttachSceneView(SceneView);
}

bool UtoCameraPerspective::DetachSceneView()
{
   // Clear the offset
   m_Owner->Projection()->SetOffset(UtoPosition(0, 0, 0));
   // notify all entities
   m_Owner->UpdateOffsets();

   return UtoCamera::DetachSceneView();
}
