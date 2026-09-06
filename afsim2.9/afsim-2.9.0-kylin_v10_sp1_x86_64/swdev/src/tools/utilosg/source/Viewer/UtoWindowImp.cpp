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
// UtoWindowImp.cpp : implementation file

#include "UtoWindowImp.hpp"

#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <tchar.h>
#endif

#include <osg/CullSettings>
#include <osg/FrameStamp>
#include <osg/Timer>
#include <osgDB/DatabasePager>
#include <osgUtil/CullVisitor>

#include "UtoOptions.hpp"
#include "UtoPriv.hpp"
#include "UtoViewer.hpp"

#ifndef _WIN32
#include <GL/glx.h>
#endif

/////////////////////////////////////////////////////////////////////////////

/* This callback is added to the render bins that need their depth buffer
 * cleared before being drawn to again.
 */
struct OpaqueRenderBinCallback : public osgUtil::RenderBin::DrawCallback
{
   // constructor
   OpaqueRenderBinCallback()
      : m_Nested(0)
   {
   }

   void drawImplementation(osgUtil::RenderBin* bin, osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous) override
   {
      if (m_Nested++)
      {
         bin->drawImplementation(renderInfo, previous);
      }
      else
      {
         if (bin->getBinNum() > 0)
         {
            // enable the depth write mask
            // the new open flight plugin has a new feature that disables depth buffer
            // writes for subsurface features.  this mean that the state set might or
            // might not have the mask enabled, meaning the glClear command will do nothing.
            // this is causing the layering of certain symbols to be shown behind others
            // since the depth buffer is the same across multiple layers.
            //            EnableDepthWriteMask();
            // clear the depth and stencil buffers
            //            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
         }

         bin->drawImplementation(renderInfo, previous);

         if (bin->getBinNum() < 0)
         {
            // enable the depth write mask
            //            EnableDepthWriteMask();
            // clear the depth and stencil buffers
            //            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
         }
      }
      --m_Nested;
   }

   void EnableDepthWriteMask()
   {
      // local(s)
      GLboolean bDepthWrite = GL_FALSE;

      // obtain the depth write mask value
      glGetBooleanv(GL_DEPTH_WRITEMASK, &bDepthWrite);

      // enable the write mask if not enabled
      if (bDepthWrite == GL_FALSE)
         glDepthMask(GL_TRUE);
   }


   int m_Nested;
};

/////////////////////////////////////////////////////////////////////////////

/* This callback is added to the render bins that need their depth function
 * reset to GL_LEQUAL before rendering.
 */
struct TransparentRenderBinCallback : public osgUtil::RenderBin::DrawCallback
{
   void drawImplementation(osgUtil::RenderBin* bin, osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous) override
   {
      int  function;
      bool changed = false;
      glGetIntegerv(GL_DEPTH_FUNC, &function);
      if (function == GL_LESS)
      {
         glDepthFunc(GL_LEQUAL);
         changed = true;
      }

      bin->drawImplementation(renderInfo, previous);

      if (changed)
      {
         glDepthFunc(GL_LESS);
      }
   }
};

/////////////////////////////////////////////////////////////////////////////

static osg::ref_ptr<OpaqueRenderBinCallback>      m_OpaqueRenderBinCallback      = new OpaqueRenderBinCallback;
static osg::ref_ptr<TransparentRenderBinCallback> m_TransparentRenderBinCallback = new TransparentRenderBinCallback;

void ReleaseRenderBinCallbacks(void)
{
   m_OpaqueRenderBinCallback      = nullptr;
   m_TransparentRenderBinCallback = nullptr;

   osgUtil::RenderBin::getRenderBinPrototype("RenderBin")->setDrawCallback(nullptr);
   osgUtil::RenderBin::getRenderBinPrototype("DepthSortedBin")->setDrawCallback(nullptr);
}

#ifdef _WIN32
#include <windows.h>
#endif

/* UTO Function Block **********************************************************

Function: UtoWindowImp

Description:
Constructor

*******************************************************************************/

UtoWindowImp::UtoWindowImp(const UtoSize& size, int nPxlFmt, UtoViewer* pViewer, osgDB::DatabasePager* pgr)
   : m_nPixelFormat(nPxlFmt)
   , m_Wid(nullptr)
   , m_ReleaseContext(true)
   ,
#ifndef _WIN32
   m_Display(nullptr)
   ,
#endif
   m_DeviceContext(nullptr)
   , m_Viewer(pViewer)
   , m_DatabasePager(pgr)
   , m_DoPrepareForRender(true)
   , m_FrameSerialNum(0)
   , m_LastUpdateSN(0)
   , m_CullVisitorNearFarCalcDisabled(false)
   , mOSG_GraphicsContextPtr(nullptr)
{
   // setup the time and frame counts for the update traverser
   m_FrameStamp = new osg::FrameStamp;
   m_Viewer->SetFrameStamp(m_FrameStamp.get());
   m_StartTime = m_Timer.tick();

   // disable the auto clipping plane range calculations
   // Start culling on the far clipping plane now that it will be fixed.
   // The near clipping plane won't help very much for the perspective
   // camera, and presumably there won't be much above the camera in
   // the orthographic case.
}

/* UTO Function Block **********************************************************

Function: UtoWindowImp

Description:
Destructor

*******************************************************************************/

UtoWindowImp::~UtoWindowImp()
{
   // cleanup as many objects as possible, but must have the correct GL
   // context active
   MakeCurrent();
   // double available_time = 60.0;
   m_Viewer->FlushAllDeletedGLObjects();

   DestroyGC();

#ifdef _WIN32
   if (m_ReleaseContext)
      ::ReleaseDC((HWND)m_Wid, (HDC)m_DeviceContext);
#endif
}

/* UTO Function Block **********************************************************

Function: UtoWindowImp

Description:
Constructor

*******************************************************************************/

void UtoWindowImp::InitSort()
{
   // attach the default renderbin callbacks
   osgUtil::RenderBin::getRenderBinPrototype("RenderBin")->setDrawCallback(m_OpaqueRenderBinCallback.get());
   osgUtil::RenderBin::getRenderBinPrototype("DepthSortedBin")->setDrawCallback(m_TransparentRenderBinCallback.get());
   // for the RenderBin method, set the sort method to be sort by state and then from front to back
   // sorting from front to back can improve rendering performance on the graphics card because closer objects
   // will be drawn first and all other objects behind the closer object will fail the z buffer test.  this
   // will help in saving against the fill rate of a graphics card since the card will not have to write to
   // the buffer all the time.
   osgUtil::RenderBin::getRenderBinPrototype("RenderBin")
      ->setSortMode((osgUtil::RenderBin::SortMode)UtoOptions::Instance()->RenderBinSortMethod());

   // setup the database pager (must happen after UtoViewer calls
   m_Viewer->SetDatabasePager(m_DatabasePager.get());
}

/* UTO Function Block **********************************************************

Function: Draw

Description:
Draws the scene in a platform specific way.

*******************************************************************************/

void UtoWindowImp::Draw()
{
   PreSceneDraw();
   SceneDraw();
   PostSceneDraw();
}

void UtoWindowImp::PreSceneDraw()
{
   // Set this window's OpenGL context to be the current one.
   MakeCurrent();

   m_LastUpdateSN = ++m_FrameSerialNum;
   m_FrameStamp->setFrameNumber(m_LastUpdateSN);
   osg::Timer_t now = m_Timer.tick();
   m_FrameStamp->setReferenceTime(m_Timer.delta_s(m_StartTime, now));

   // required for Terrex terrain paging
   if (m_DatabasePager.get())
   {
      // Don't expire LOD nodes based on time. Only page LOD nodes out based on the view frustum.
      // m_DatabasePager->setExpiryDelay(DBL_MAX);

      // tell the DatabasePager the frame number of that the scene
      // graph is being actively used to render a frame
      m_DatabasePager->signalBeginFrame(m_FrameStamp.get());
      // synchronize changes required by the DatabasePager thread
      // to the scene graph
      m_DatabasePager->updateSceneGraph(*m_FrameStamp.get());
   }

   if (!m_CullVisitorNearFarCalcDisabled)
   {
      m_CullVisitorNearFarCalcDisabled = true;
      m_Viewer->SetComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
   }
}


void UtoWindowImp::SceneDraw()
{
   m_Viewer->DrawScene();
}

void UtoWindowImp::PostSceneDraw()
{
   if (m_DatabasePager.get())
   {
      // tell the DatabasePager that the frame is complete and that scene
      // graph is no longer be activity traversed.
      m_DatabasePager->signalEndFrame();
      // clean up  and compile gl objects with a specified limit
      double availableTime = 0.00125; // 1.25 ms
                                      // compile any GL objects that are required.
      m_Viewer->CompileAndFlushGLObjects(availableTime);
   }

   // release the current OpenGL context for other threads to be able to use
   ReleaseCurrent();
}

#ifdef _WIN32
bool UtoWindowImp::AttachToWindowId(UtoWindowId wid, bool releaseContext)
{
   if (m_Wid)
   {
      // DEBUG(("Calling Attach after the window is already attached"))
      return false;
   }

   m_Wid = wid;
   m_Viewer->BuildGraphicsContext();

   m_ReleaseContext = releaseContext;

   if (m_ReleaseContext)
   {
      m_DeviceContext = ::GetDC((HWND)m_Wid);
      if (!m_DeviceContext)
      {
         // DEBUG(("Unable to get a device context"))
         return false;
      }

      m_OpenGLContext = wglGetCurrentContext();
      if (!m_OpenGLContext)
      {
         // Unable to get a OpenGL Graphics Context
         m_DeviceContext = 0;
         return false;
      }
   }

   return true;
}
#else
bool UtoWindowImp::AttachToWindowId(UtoWindowId wid, bool releaseContext)
{
   if (m_Wid)
   {
      // DEBUG(("Calling Attach after the window is already attached"))
      return false;
   }

   m_Wid            = wid;
   m_ReleaseContext = releaseContext;

   if (m_ReleaseContext)
   {
      m_DeviceContext = wid;
      m_OpenGLContext = glXGetCurrentContext();
      if (!m_OpenGLContext)
      {
         // DEBUG(("Unable to get a OpenGL Graphics Context"))
         return false;
      }
      m_Display = (UtoDisplay)glXGetCurrentDisplay();
      if (!m_Display)
      {
         // DEBUG(("Couldn't get current xdisplay"))
         return false;
      }
   }

   return true;
}
#endif

bool UtoWindowImp::AttachToGraphicsContext(osg::GraphicsContext* aContextPtr)
{
   if (m_Wid)
   {
      // DEBUG(("Calling Attach after the window is already attached"))
      return false;
   }
   mOSG_GraphicsContextPtr = aContextPtr;
   m_Wid                   = UtoWindowId(1001);
   m_Viewer->UseGraphicsContext(aContextPtr);

   return true;
}


/* UTO Function Block **********************************************************

Function: AttachToWindowId

Description:
Attaches the window

*******************************************************************************/

#ifdef _WIN32
bool UtoWindowImp::AttachToWindowId(UtoWindowId wid)
{
   if (m_Wid)
   {
      // DEBUG(("Calling Attach after the window is already attached"))
      return false;
   }

   m_Wid = wid;

   m_DeviceContext = ::GetDC((HWND)m_Wid);
   if (!m_DeviceContext)
   {
      // DEBUG(("Unable to get a device context"))
      return false;
   }

   m_OpenGLContext = CreateGC(m_DeviceContext, 0);
   if (!m_DeviceContext)
   {
      m_DeviceContext = 0;
      return false;
   }

   /*   if (!wglShareLists((HGLRC)s_HiddenGLContext, (HGLRC)m_OpenGLContext))
      {
         unsigned int error = GetLastError();
         std::string message = UtoDebug::GetErrorMessage(error);
         LOG("Error sharing OpenGL display lists. %d:%s\n",
             error, message.c_str());
         return false;
      }*/
   // The context is created here, but only in the draw routine do we make
   // the context current.  That way only one thread will ever try
   // to make the same context current at one time.
   return true;
}
#else
bool UtoWindowImp::AttachToWindowId(UtoWindowId wid)
{
   // TODO - Generate display and graphics context.
   return false;
}
#endif

/* UTO Function Block **********************************************************

Function: MakeCurrent

Description:
Makes the context in this class the current OpenGL Context.

*******************************************************************************/

void UtoWindowImp::MakeCurrent()
{
   if (m_ReleaseContext)
   {
      if (nullptr != mOSG_GraphicsContextPtr)
      {
         mOSG_GraphicsContextPtr->makeCurrent();
         return;
      }
#ifdef _WIN32
      if (!wglMakeCurrent((HDC)m_DeviceContext, (HGLRC)m_OpenGLContext))
      {
         // unsigned int error = GetLastError();
         // std::string message = UtoDebug::GetErrorMessage(error);
         // LOG("wglMakeCurrent failed to set the OpenGL context, %d:%s\n", error, message.c_str());
      }
#else
      if (!glXMakeCurrent((Display*)m_Display, (GLXDrawable)m_DeviceContext, (GLXContext)m_OpenGLContext))
      {
         // LOG("glXMakeCurrent failed to set the OpenGL context");
      }
#endif
   }
}


/* UTO Function Block **********************************************************

Function: ReleaseCurrent

Description:
Releases the current context so that there isn't an OpenGL context current.

*******************************************************************************/

void UtoWindowImp::ReleaseCurrent()
{
   if (m_ReleaseContext)
   {
#ifdef _WIN32
      if (!wglMakeCurrent((HDC)m_DeviceContext, NULL))
      {
         // unsigned int error = GetLastError();
         // std::string message = UtoDebug::GetErrorMessage(error);
         // LOG("MakeCurrent failed to release the OpenGL context, %d:%s\n", error, message.c_str());
      }
#else
      if (!glXMakeCurrent((Display*)m_Display, None, nullptr))
      {
         // LOG("glXMakeCurrent failed to release the OpenGL context");
      }
#endif
   }
}

#ifdef _DEBUG

// indicates the pixel format for the graphics card
std::ofstream& operator<<(std::ofstream& rFile, const PIXELFORMATDESCRIPTOR& rPFD)
{
   rFile << "                      nSize:" << rPFD.nSize << std::endl;
   rFile << "                   nVersion:" << rPFD.nVersion << std::endl;

   rFile << "                    dwFlags:";

   if (rPFD.dwFlags & PFD_DRAW_TO_WINDOW)
      rFile << "PFD_DRAW_TO_WINDOW ";
   if (rPFD.dwFlags & PFD_DRAW_TO_BITMAP)
      rFile << "PFD_DRAW_TO_BITMAP ";
   if (rPFD.dwFlags & PFD_SUPPORT_GDI)
      rFile << "PFD_SUPPORT_GDI ";
   if (rPFD.dwFlags & PFD_SUPPORT_OPENGL)
      rFile << "PFD_SUPPORT_OPENGL ";
   if (rPFD.dwFlags & PFD_GENERIC_ACCELERATED)
      rFile << "PFD_GENERIC_ACCELERATED ";
   if (rPFD.dwFlags & PFD_NEED_PALETTE)
      rFile << "PFD_NEED_PALETTE ";
   if (rPFD.dwFlags & PFD_NEED_SYSTEM_PALETTE)
      rFile << "PFD_NEED_SYSTEM_PALETTE ";
   if (rPFD.dwFlags & PFD_DOUBLEBUFFER)
      rFile << "PFD_DOUBLEBUFFER ";
   if (rPFD.dwFlags & PFD_STEREO)
      rFile << "PFD_STEREO ";
   if (rPFD.dwFlags & PFD_DEPTH_DONTCARE)
      rFile << "PFD_DEPTH_DONTCARE ";
   if (rPFD.dwFlags & PFD_DOUBLEBUFFER_DONTCARE)
      rFile << "PFD_DOUBLEBUFFER_DONTCARE ";
   if (rPFD.dwFlags & PFD_STEREO_DONTCARE)
      rFile << "PFD_STEREO_DONTCARE ";
   if (rPFD.dwFlags & PFD_SWAP_COPY)
      rFile << "PFD_SWAP_COPY ";
   if (rPFD.dwFlags & PFD_SWAP_EXCHANGE)
      rFile << "PFD_SWAP_EXCHANGE ";

   rFile << std::endl << "                 iPixelType:";

   if (rPFD.iPixelType & PFD_TYPE_RGBA)
      rFile << "PFD_TYPE_RGBA ";
   if (rPFD.iPixelType & PFD_TYPE_COLORINDEX)
      rFile << "PFD_TYPE_COLORINDEX ";

   rFile << std::endl << "                 cColorBits:" << (int)rPFD.cColorBits << std::endl;
   rFile << "     cRedGreenBlueAlpahBits:" << (int)rPFD.cRedBits << " " << (int)rPFD.cGreenBits << " "
         << (int)rPFD.cBlueBits << " " << (int)rPFD.cAlphaBits << std::endl;
   rFile << "    cRedGreenBlueAlphaShift:" << (int)rPFD.cRedShift << " " << (int)rPFD.cGreenShift << " "
         << (int)rPFD.cBlueShift << " " << (int)rPFD.cAlphaShift << std::endl;
   rFile << "                 cAccumBits:" << (int)rPFD.cAccumBits << std::endl;
   rFile << "cAccumRedGreenBlueAlphaBits:" << (int)rPFD.cAccumRedBits << " " << (int)rPFD.cAccumGreenBits << " "
         << (int)rPFD.cAccumBlueBits << " " << (int)rPFD.cAccumAlphaBits << std::endl;
   rFile << "               cDepthBitsts:" << (int)rPFD.cDepthBits << std::endl;
   rFile << "               cStencilBits:" << (int)rPFD.cStencilBits << std::endl;
   rFile << "                cAuxBuffers:" << (int)rPFD.cAuxBuffers << std::endl;

   rFile << "                 iLayerType:";

   switch (rPFD.iLayerType)
   {
   case PFD_MAIN_PLANE:
      rFile << "PFD_MAIN_PLANE" << std::endl;
      break;
   case PFD_OVERLAY_PLANE:
      rFile << "PFD_OVERLAY_PLANE" << std::endl;
      break;
   case PFD_UNDERLAY_PLANE:
      rFile << "PFD_UNDERLAY_PLANE" << std::endl;
      break;
   }

   rFile << "                  bReserved:" << (int)rPFD.bReserved << std::endl;
   rFile << "                dwLayerMask:" << rPFD.dwLayerMask << std::endl;
   rFile << "              dwVisibleMask:" << rPFD.dwVisibleMask << std::endl;
   rFile << "               dwDamageMask:" << rPFD.dwDamageMask << std::endl;

   return rFile;
}

#endif

#ifdef _WIN32
UtoGC UtoWindowImp::CreateGC(UtoDC dc, int nPixelFormat)
{
   HGLRC OpenGLContext = 0;

   // pick the pixel format
   PIXELFORMATDESCRIPTOR info;
   memset(&info, 0, sizeof(PIXELFORMATDESCRIPTOR));
   info.nSize      = (sizeof(PIXELFORMATDESCRIPTOR));
   info.nVersion   = 1;
   info.iPixelType = PFD_TYPE_RGBA;
   info.cColorBits = 24; // 24 bits of color
   info.cAlphaBits = 8;  // 8 bits of alpha
   info.dwFlags    = PFD_DOUBLEBUFFER | PFD_SWAP_EXCHANGE | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
   info.cDepthBits = 32;

#ifdef _DEBUG

   std::ofstream ofDebug;

   ofDebug.open("PixelFormat.txt", std::ios_base::out);

   ofDebug << "****** Requested Pixel Format ******" << std::endl;
   ofDebug << info << std::endl;

#endif
   int visualId = nPixelFormat;

   if (!visualId)
   {
      visualId = ::ChoosePixelFormat((HDC)dc, &info);
   }

   if (!visualId)
   {
      // DEBUG(("Unable to find a suitble pixel format"))
      return 0;
   }

#ifdef _DEBUG

   PIXELFORMATDESCRIPTOR oChoosenPFD;

   DescribePixelFormat((HDC)dc, visualId, sizeof(PIXELFORMATDESCRIPTOR), &oChoosenPFD);

   if (UtoOptions::Instance()->WarnGenericDriver())
   {
      if (oChoosenPFD.dwFlags & PFD_GENERIC_FORMAT)
         MessageBox(NULL, "Generic Implementation Only!", "Pixel Format", MB_OK);
      if (oChoosenPFD.dwFlags & PFD_GENERIC_ACCELERATED)
         MessageBox(NULL, "Generic Acceleration Only!", "Pixel Format", MB_OK);
   }

   ofDebug << "****** Choosen Pixel Format ******" << std::endl;
   ofDebug << oChoosenPFD << std::endl;

#endif

   if (!::SetPixelFormat((HDC)dc, visualId, &info))
   {
      // DEBUG(("Failed to set selected pixel format"))
      return 0;
   }

   OpenGLContext = wglCreateContext((HDC)dc);
   if (!OpenGLContext)
   {
      // DEBUG(("Failed to get a valid OpenGL context"))
      return 0;
   }
   return OpenGLContext;
}

#else

UtoGC UtoWindowImp::CreateGC(UtoDC dc, int nPixelFormat)
{
   // TODO - LMM - This is just a stub and needs to filled in to work on Linux
   UtoGC OpenGLContext = nullptr;
   return OpenGLContext;
}

#endif

void UtoWindowImp::HittestUpdate()
{
   if (m_LastUpdateSN != m_FrameSerialNum)
   {
      m_LastUpdateSN = ++m_FrameSerialNum;
      m_FrameStamp->setFrameNumber(m_LastUpdateSN);
      osg::Timer_t now = m_Timer.tick();
      m_FrameStamp->setReferenceTime(m_Timer.delta_s(m_StartTime, now));
   }
}

void UtoWindowImp::SetSimulationTime(double aSimTime)
{
   m_FrameStamp->setSimulationTime(aSimTime);
}

void UtoWindowImp::DestroyGC()
{
   if (m_ReleaseContext)
   {
      if (nullptr != mOSG_GraphicsContextPtr)
      {
         mOSG_GraphicsContextPtr->closeImplementation();
         return;
      }
#ifdef _WIN32
      wglDeleteContext((HGLRC)m_OpenGLContext);
#else
      glXDestroyContext((Display*)m_Display, (GLXContext)m_OpenGLContext);
#endif
   }
}
