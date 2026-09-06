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

#if !defined(_UTOCAMERAORTHOIMP_HPP_)
#define _UTOCAMERAORTHOIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osgUtil/SceneView>

#include "UtoCamera.hpp"
#include "UtoCameraOrtho.hpp"
#include "UtoTypes.hpp"

class UtoViewer;

class UtoCameraOrtho : public UtoCamera
{
public:
   enum Flags
   {
      NO_YAW = 0x0004
   };


   UtoCameraOrtho(UtoViewer*);
   ~UtoCameraOrtho() override;

   virtual UtoPosition      Position() const;
   virtual void             SetPosition(UtoPosition pos);
   virtual UtoOrientation   Orientation() const;
   virtual void             SetOrientation(const UtoOrientation& orientation);
   virtual void             SetViewMatrix(const osg::Matrix& ViewMatrix);
   virtual bool             LockEntity(const UtoEntity& ent);
   virtual bool             UnlockEntity();
   virtual const UtoEntity* LockedEntity() const;
   virtual bool             Scroll(double offset, double angle);
   virtual bool             SetZoom(double offset);
   virtual double           Zoom() const;
   virtual bool             CenterEntity(UtoEntity& ent);

   virtual bool   SetNearClip(double near);
   virtual double NearClip() const { return m_Hither; }
   virtual bool   SetFarClip(double far);
   virtual double FarClip() const { return m_Yon; }
   virtual bool   Set(double near, double far);

   virtual void Dump(std::ostream&);
   virtual void SetCameraFlags(long mask, long state);
   bool         UpdateProjection() override;
   bool         UpdateSceneView() override;

   bool AttachSceneView(osgUtil::SceneView* SceneView) override;

protected:
private:
   UtoCameraOrtho();
   UtoCameraOrtho(const UtoCameraOrtho&);
   UtoCameraOrtho& operator=(const UtoCameraOrtho&);

   bool UpdateZoom();
   void ClearUpdateZoom();

   double m_fZoom;
   bool   m_bUpdateZoom;
   bool   m_bNoYaw;
   double m_Hither;
   double m_Yon;
};

#endif // !defined(_UTOCAMERAORTHOIMP_HPP_)
