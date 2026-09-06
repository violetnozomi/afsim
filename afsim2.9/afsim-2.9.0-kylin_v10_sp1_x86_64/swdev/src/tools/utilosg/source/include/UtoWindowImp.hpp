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
// UtoWindowImp.hpp : header file

#if !defined(_UTOWINDOWIMP_HPP_)
#define _UTOWINDOWIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

//#include <GL/gl.h>

#if defined(_WIN32)
typedef /*HGLRC*/ void* UtoGC; // Graphics Context or GL Context
typedef /*HDC*/ void*   UtoDC; // Device Context
#else
typedef /*GLXContext*/ void*  UtoGC;
typedef /*GLXDrawable*/ void* UtoDC;
typedef /*Display*/ void*     UtoDisplay;
#endif

#include <set>

#include <osg/Timer>
#include <osg/ref_ptr>

#include "UtoTypes.hpp"
#include "UtoWindow.hpp"

namespace osg
{
class FrameStamp;
class GraphicsContext;
} // namespace osg
namespace osgDB
{
class DatabasePager;
}

class UtoViewer;

//#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
//#include <windows.h>

class UtoWindow;

class UtoWindowImp : public UtoWindow
{
public:
   UtoWindowImp(const UtoSize& size, int nPxlFmt, UtoViewer* pViewer, osgDB::DatabasePager* pgr);
   ~UtoWindowImp() override;

   UtoWindowId Window() const override { return m_Wid; }

   // virtual bool Popup();
   bool         AttachToWindowId(UtoWindowId, bool releaseContext) override;
   virtual bool AttachToWindowId(UtoWindowId);
   bool         AttachToGraphicsContext(osg::GraphicsContext* aGC) override;

   void Draw();

   void PreSceneDraw();
   void SceneDraw();
   void PostSceneDraw();

   void MakeCurrent();
   void ReleaseCurrent();

   UtoWindowImp& operator=(const UtoWindowImp&);

   void InitSort();

   void HittestUpdate();
   void SetSimulationTime(double aSimTime);

protected:
private:
   static UtoGC CreateGC(UtoDC dc, int nPixelFormat);
   void         DestroyGC();

   UtoWindowImp();
   UtoWindowImp(const UtoWindowImp&, UtoViewer* pViewer);

   int         m_nPixelFormat;
   UtoWindowId m_Wid;

   UtoGC m_OpenGLContext; // OpenGL rendering context
   bool  m_ReleaseContext;
#ifdef _WIN32
#else
   UtoDisplay m_Display;
   // X window ID or GLX pixmap ID
#endif
   UtoDC m_DeviceContext;

   UtoViewer* m_Viewer;

   osg::ref_ptr<osgDB::DatabasePager> m_DatabasePager;
   osg::ref_ptr<osg::FrameStamp>      m_FrameStamp;
   osg::Timer                         m_Timer;
   osg::Timer_t                       m_StartTime;

   bool m_DoPrepareForRender;

   unsigned int          m_FrameSerialNum;
   unsigned int          m_LastUpdateSN;
   bool                  m_CullVisitorNearFarCalcDisabled;
   osg::GraphicsContext* mOSG_GraphicsContextPtr;
};

#endif // !defined(_UTOWINDOWIMP_HPP_)
