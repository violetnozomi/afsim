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
// UtoCameraImp.cpp : implementation file

#include "UtoCamera.hpp"

#include <osg/io_utils>

#include "UtoEntity.hpp"
#include "UtoPriv.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerProjection.hpp"

/* UTO Function Block **********************************************************

Function: UtoCameraImp

Description:
   Constructor

*******************************************************************************/

UtoCamera::UtoCamera(UtoViewer* Owner)
   : m_Owner(Owner)
   , m_ObservedEntity(nullptr)
   , m_Position(0, 0, 50.0)
   , m_PosObserve(false)
   , m_OrientObserve(false)
   , m_ExplicitViewMatrixActive(false)
{
}

/* UTO Function Block **********************************************************

Function: UtoCameraImp

Description:
   Destructor

*******************************************************************************/

UtoCamera::~UtoCamera()
{
   UnlockEntity();
   UnlockEntityOrientation();
}

/* UTO Function Block **********************************************************

Function: SetPosition

Description:
   Sets the position of the camera and updates the relative position of the
   camera if it observing any entities.

*******************************************************************************/

void UtoCamera::SetPosition(UtoPosition pos)
{
   m_Position = pos;

   UpdateRelativePosition();

   UpdateSceneView();

   sendEvent(PositionChangedEvent(), this);
}

/* UTO Function Block **********************************************************

  Function: AttachSceneView

    Description:
    Attach the SceneView to the camera

*******************************************************************************/

bool UtoCamera::AttachSceneView(osgUtil::SceneView* SceneView)
{
   //   m_SceneView = SceneView;

   // update the two matrices
   UpdateProjection();
   UpdateSceneView();

   return true;
}

/* UTO Function Block **********************************************************

  Function: DetachSceneView

    Description:
    Detaches the active SceneView from the camera

*******************************************************************************/

bool UtoCamera::DetachSceneView()
{
   //   m_SceneView = NULL;
   return true;
}

/* UTO Function Block **********************************************************

  Function: AttachViewer

    Description:
    Attach the viewer to the camera

*******************************************************************************/

bool UtoCamera::AttachViewer(osgViewer::Viewer* aViewer)
{
   //   m_OSGViewer = aViewer;

   // update the two matrices
   UpdateProjection();
   UpdateSceneView();

   return true;
}

/* UTO Function Block **********************************************************

  Function: DetachViewer

    Description:
    Detaches the active Viewer from the camera

*******************************************************************************/

bool UtoCamera::DetachViewer()
{
   //   m_OSGViewer = NULL;
   return true;
}

/* UTO Function Block **********************************************************

Function: EntityPositionCB

Description:
   Callback which will update the camera position on a locked entity

*******************************************************************************/

void UtoCamera::EntityPositionCB(UtoEntity* entity)
{
   UtoPosition NewPos = m_Owner->Projection()->CalcPointNoOffset(entity->BodyCF());
   m_Position         = NewPos + m_RelativePosition;

   UpdateSceneView();

   sendEvent(PositionChangedEvent(), this);
}

/* UTO Function Block **********************************************************

Function: EntityOrientationCB

Description:
   Callback which will update the camera's orientation on a locked entity

*******************************************************************************/

void UtoCamera::EntityOrientationCB(UtoEntity* entityImp)
{
   UpdateSceneView();
}

/* UTO Function Block **********************************************************

Function: EntityRemovedCB

Description:
   Callback for a removed entity which is being observed

*******************************************************************************/

void UtoCamera::EntityRemovedCB(UtoEntity* entityImp)
{
   UnlockEntity();
}


/* UTO Function Block **********************************************************

Function: UpdateRelativePosiiton

Description:
   Updates the relative postion between the observed entity and the camera

*******************************************************************************/

void UtoCamera::UpdateRelativePosition()
{
   if (m_ObservedEntity != nullptr)
   {
      UtoPosition NewPos = m_Owner->Projection()->CalcPointNoOffset(m_ObservedEntity->BodyCF());
      m_RelativePosition = m_Position - NewPos;
   }
}

/* UTO Function Block **********************************************************

Function: LockEntity

Description:
   Locks the cameras relative position to a given entity.

*******************************************************************************/

bool UtoCamera::LockEntity(const UtoEntity& ent)
{
   UtoEntity* ent2 = const_cast<UtoEntity*>(&ent);

   // Set the position and orientation observation flag
   UpdateObservation(ent2, true, true);

   // Set up the relative position
   UpdateRelativePosition();

   // Disable the explicit view matrix.
   ClearExplicitViewMatrix();

   return true;
}

/* UTO Function Block **********************************************************

Function: LockedEntity

Description:
   Returns the locked entity

*******************************************************************************/

const UtoEntity* UtoCamera::LockedEntity() const
{
   return m_ObservedEntity ? m_ObservedEntity : nullptr;
}

/* UTO Function Block **********************************************************

Function: UnlockEntity

Description:
   Unlocks the camera from an entity

*******************************************************************************/

bool UtoCamera::UnlockEntity()
{
   bool wasLocked = m_PosObserve || m_OrientObserve;

   // Set the position observation flag
   UpdateObservation(nullptr, false, false);

   return wasLocked;
}


bool UtoCamera::LockEntityOrientation(const UtoEntity& ent)
{
   UtoEntity* ent2 = const_cast<UtoEntity*>(&ent);

   // Keep the position flag while setting the orientation flag.
   UpdateObservation(ent2, m_PosObserve, true);

   // Set up the relative position
   UpdateRelativePosition();

   return true;
}

bool UtoCamera::UnlockEntityOrientation()
{
   // Keep the position flag while setting the orientation flag.
   UpdateObservation(nullptr, m_PosObserve, false);

   return true;
}

void UtoCamera::Dump(std::ostream& o)
{
   o << "  Position: " << Position() << std::endl;

   if (m_ObservedEntity)
   {
      o << "  Locked Entity: " << m_ObservedEntity->ObjectName() << "'";
      o << m_ObservedEntity->Name() << "'" << std::endl;
   }
}


/* UTO Function Block **********************************************************

Function: SetOrientation

Description:
   Sets the cameras orientation

*******************************************************************************/

void UtoCamera::SetOrientation(UtoOrientation Orientation)
{
   m_Orientation = Orientation;

   UpdateSceneView();
}

void UtoCamera::ViewMatrix(double ViewMatrix[4][4])
{
   for (int i = 0; i < 4; ++i)
   {
      for (int j = 0; j < 4; ++j)
      {
         ViewMatrix[i][j] = m_ExplicitViewMatrix(i, j);
      }
   }
}

void UtoCamera::SetViewMatrix(const osg::Matrix& ViewMatrix)
{
   m_ExplicitViewMatrix = ViewMatrix;

   m_ExplicitViewMatrixActive = true;

   UpdateSceneView();
}

void UtoCamera::ClearExplicitViewMatrix()
{
   m_ExplicitViewMatrix       = osg::Matrixd();
   m_ExplicitViewMatrixActive = false;
}

void UtoCamera::UpdateObservation(UtoEntity* NewObservedEntity, bool NewPosObserve, bool NewOrientObserve)
{
   bool NewEntity = false;
   if (m_ObservedEntity != NewObservedEntity)
   {
      NewEntity = true;
   }

   if (m_ObservedEntity)
   {
      if (m_PosObserve && (NewEntity || !NewPosObserve))
      {
         // Detach Position update
         m_ObservedEntity->disconnect(UtoEntity::PositionChangedEvent(), this, &UtoCamera::EntityPositionCB);
      }
      if (m_OrientObserve && (NewEntity || !NewOrientObserve))
      {
         // Detach Position update
         m_ObservedEntity->disconnect(UtoEntity::OrientationChangedEvent(), this, &UtoCamera::EntityOrientationCB);
      }
      if ((m_PosObserve || m_OrientObserve) && (NewEntity || !(NewPosObserve || NewOrientObserve)))
      {
         // Detach Position and Orientation update
         m_ObservedEntity->disconnect(UtoEntity::PosnOrientChangedEvent(), this, &UtoCamera::EntityPositionCB);
      }

      if (NewEntity)
      {
         // Detach entity remove
         m_ObservedEntity->disconnect(UtoEntity::DestroyedEvent(), this, &UtoCamera::EntityRemovedCB);
      }
   }

   if (NewObservedEntity)
   {
      if (NewPosObserve && (NewEntity || !m_PosObserve))
      {
         // Setup Position update
         NewObservedEntity->connect(UtoEntity::PositionChangedEvent(), this, &UtoCamera::EntityPositionCB);
      }
      if (NewOrientObserve && (NewEntity || !m_OrientObserve))
      {
         // Setup Position update
         NewObservedEntity->connect(UtoEntity::OrientationChangedEvent(), this, &UtoCamera::EntityOrientationCB);
      }
      if ((NewPosObserve || NewOrientObserve) && (NewEntity || !(m_PosObserve || m_OrientObserve)))
      {
         // Setup Position and Orientation update
         // Call the position update callback because it does everything
         // the orientation callback does and more.
         NewObservedEntity->connect(UtoEntity::PosnOrientChangedEvent(), this, &UtoCamera::EntityPositionCB);
      }

      if (NewEntity)
      {
         // Setup entity remove
         NewObservedEntity->connect(UtoEntity::DestroyedEvent(), this, &UtoCamera::EntityRemovedCB);
      }
   }

   // Update the variables
   m_PosObserve     = NewPosObserve;
   m_OrientObserve  = NewOrientObserve;
   m_ObservedEntity = NewObservedEntity;
}
