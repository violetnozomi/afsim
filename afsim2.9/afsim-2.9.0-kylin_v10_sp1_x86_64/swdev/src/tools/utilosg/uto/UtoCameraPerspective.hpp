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
// UtoCameraOrthoImp.hpp : header file

#if !defined(_UTOCAMERAPERSPECTIVEIMP_HPP_)
#define _UTOCAMERAPERSPECTIVEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osgUtil/SceneView>

#include "UtoCamera.hpp"
#include "UtoPriv.hpp"
#include "UtoTypes.hpp"

class UtoViewer;

class UtoCameraPerspective : public UtoCamera
{
public:
   enum Flags
   {
      NO_ROLL  = 0x0001,
      NO_PITCH = 0x0002,
      NO_YAW   = 0x0004,
   };
   UtoCameraPerspective(UtoViewer*);
   ~UtoCameraPerspective() override;

   virtual UtoPosition Position() const;

   virtual void SetPosition(UtoPosition pos);
   virtual void SetOrientation(UtoOrientation orientation);
   virtual void SetViewMatrix(const osg::Matrix& ViewMatrix, const double aX = 0, const double aY = 0, const double aZ = 0);
   virtual UtoOrientation   Orientation() const;
   virtual bool             LookAt(const UtoPosition& LookAtPoint);
   virtual bool             LookAt(const UtoEntity& ent);
   virtual bool             Rotate(unsigned int axis, double angle);
   virtual bool             LockEntity(const UtoEntity& ent);
   virtual const UtoEntity* LockedEntity() const;
   virtual bool             UnlockEntity();
   bool                     UpdateProjection() override;
   bool                     UpdateSceneView() override;
   virtual bool             LockEntityOrientation(const UtoEntity& ent);
   virtual bool             UnlockEntityOrientation();

   virtual const UtoEntity*  LookAtEntity();
   virtual const UtoPosition LookAtPosition();
   virtual void              SetCameraFlags(long mask, long state);

   virtual bool   SetFOV(double fov);
   virtual double FOV() const { return m_FOV; }
   virtual bool   SetNearClip(double near);
   virtual double NearClip() const { return m_Hither; }
   virtual bool   SetFarClip(double far);
   virtual double FarClip() const { return m_Yon; }
   virtual bool   Set(double fov, double near, double far);

   bool AttachSceneView(osgUtil::SceneView* SceneView) override;
   bool DetachSceneView() override;

   void Dump(std::ostream&);

   static const float m_GridExtentX;
   static const float m_GridExtentY;
   static const float m_GridExtentZ;

protected:
private:
   UtoCameraPerspective();
   UtoCameraPerspective(const UtoCameraPerspective&);
   UtoCameraPerspective& operator=(const UtoCameraPerspective&);

   void CancelLookAt();
   void CalculateOffset(UtoPosition& CameraPos);
   void EntityLookAtCB(UtoEntity* entity);
   void LookAtEntityRemovedCB(UtoEntity* entity);

   UtoPosition m_LookAtPoint;
   UtoEntity*  m_LookAtEntity;
   bool        m_LookAtActive;
   long        m_Flags;
   float       m_FOV; // radians
   float       m_Hither;
   float       m_Yon;
};

#endif // !defined(_UTOCAMERAPERSPECTIVEIMP_H_)
