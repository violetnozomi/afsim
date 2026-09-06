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
// UtoCameraImp.hpp : header file

#if !defined(_UTOCAMERAIMP_HPP_)
#define _UTOCAMERAIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/Matrix>
#include <osgUtil/SceneView>
#include <osgViewer/Viewer>

#include "UtoCallback.hpp"
#include "UtoTypes.hpp"

class UtoCamera;
class UtoEntity;
class UtoViewer;
class UtoHittestStrategy;

class UtoCamera : public UtoCallback<UtoCamera>
{
public:
   explicit UtoCamera(UtoViewer*);
   ~UtoCamera() override;

   UtoPosition Position() const { return m_Position; }
   void        SetPosition(UtoPosition pos);

   UtoOrientation Orientation() const { return m_Orientation; }
   void           SetOrientation(UtoOrientation Orientation);

   const osg::Matrixd& GetViewMatrix() const { return m_ExplicitViewMatrix; }
   void                ViewMatrix(double ViewMatrix[4][4]);
   void                SetViewMatrix(const osg::Matrix& ViewMatrix);
   bool                ViewMatrixActive() const { return m_ExplicitViewMatrixActive; }

   virtual bool AttachSceneView(osgUtil::SceneView* SceneView);
   virtual bool DetachSceneView();

   virtual bool AttachViewer(osgViewer::Viewer* aViewer);
   virtual bool DetachViewer();

   bool             LockEntity(const UtoEntity& ent);
   const UtoEntity* LockedEntity() const;
   bool             UnlockEntity();
   bool             LockEntityOrientation(const UtoEntity& ent);
   bool             UnlockEntityOrientation();

   virtual bool UpdateProjection() = 0;
   virtual bool UpdateSceneView()  = 0;

   typedef Event1<0, UtoCamera*> PositionChangedEvent;

protected:
   virtual void UpdateRelativePosition();
   void         ClearExplicitViewMatrix();

   void Dump(std::ostream&);

   osg::Matrixd m_ExplicitViewMatrix;

   UtoViewer*  m_Owner;
   UtoEntity*  m_ObservedEntity;
   UtoPosition m_RelativePosition;
   //   osg::ref_ptr<osgUtil::SceneView> m_SceneView;
   //   osg::ref_ptr<osgViewer::Viewer> m_OSGViewer;

private:
   UtoCamera();
   UtoCamera(const UtoCamera&);
   UtoCamera& operator=(const UtoCamera&);

   // osg::Matrix m_ModelToWorldMat;

   UtoPosition    m_Position;
   UtoOrientation m_Orientation;

   bool m_PosObserve;
   bool m_OrientObserve;
   bool m_ExplicitViewMatrixActive;

   void UpdateObservation(UtoEntity* NewObservedEntity, bool NewPosObserve, bool NewOrientObserve);

   void EntityPositionCB(UtoEntity* entity);
   void EntityOrientationCB(UtoEntity* entity);
   void EntityRemovedCB(UtoEntity* entity);
};

#endif //_UTOCAMERAIMP_HPP_
