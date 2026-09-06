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
// Uto.cpp : implementation file.

#include "Uto.hpp"

#include <algorithm>
#include <assert.h>
#include <sstream>
#include <string.h>
#include <vector>

#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgUtil/SceneView>

#include "../Shape/UtoModelPartImp.hpp"
#include "../Shape/UtoModelSwitchImp.hpp"
#include "Doxygen.h"
#include "UtoEntityDB.hpp"
#include "UtoObjectManager.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"
#include "UtoResourceDB.hpp"
#include "UtoShaders.hpp"
#include "UtoTexturedPolygonImp.hpp"

#if _WIN32
#include <windows.h>
#endif

// {secret}
static bool bUtoInitialized = false;

// {secret}
// void UtoVTreeErrorMsgHandler (const vtInt8 severity, const char* buf);

// {secret}
void UtoTermCleanup();

/**
\ingroup rt_term
\return int - <table>
                  <tr><td> &gt;= 0 </td><td>UTO terminated successful.</td></tr>
                  <tr><td> &lt; 0 </td><td>UTO terminated unsuccessful. </td></tr>
              </table>
*/
int UtoTerm()
{
   // The terrex code creates another thread.  The DatabasePager's
   // destructor causes it to cleanly exit, the best way to go.  Hopefully
   // by this point there aren't any viewers olding onto references to
   // the DatabasePager.
   // LMM - This method is no longer available in OSG 2.8.1
   // osgDB::Registry::instance()->setDatabasePager(NULL);


   UtoObjectManager* omgr = UtoObjectManager::Instance();
   omgr->DestroyAllObjects(false);

   // Make sure a context is active and it has the same display list
   // space as the ones being drawn on.  Create a new SceneView object
   // because it will then cleanup display lists and other objects.
   //   UtoWindowImp::MakeHiddenContextCurrent();
   osg::ref_ptr<osgUtil::SceneView> sceneview = new osgUtil::SceneView;
   sceneview->setDefaults();
   // sceneview->flushDeletedGLObjects(available_time);
   sceneview->flushAllDeletedGLObjects();

   // Destory the hidden window.
   //   UtoWindowImp::DestroyHiddenWindow();

   // put a dummy app back into the object manager so it remains for a bit
   // longer.  however, this app will not be initialized, so may crash if the
   // user application performs significant work after calling UtoTerm().
   // omgr->Register ("App", new UtoApp);

   //   osgEarth::Registry::instance(true);

   // clear out all local cache and archive objects first
   // the dlls are being unloaded before the cache and
   // archives have a chance to unload the data they contain.
   //   osgDB::Registry::instance()->clearObjectCache();
   //   osgDB::Registry::instance()->clearArchiveCache();

   // need to close all the dynamic link libraries
   // the problem is when the registry is deleted through the use
   // of osgDB::Registry::instance(true), the registry would be
   // nullified.  the dynamic plugin used by the registry uses
   // an instance of itself to unload all the plugins and the
   // associated ReaderWriter.  because the registry is null,
   // the ReaderWriters will never be unloaded, which then would
   // cause an access violation.
   //   osgDB::Registry::instance()->closeAllLibraries();

   // delete the registry
   //   osgDB::Registry::instance(true);

   // special cleaup function
   UtoTermCleanup();
   UtoShaders::Shutdown();

   // kill the object manager
   omgr->DestroyAllObjects(true);

   bUtoInitialized = false;
   return 0;
}

void UtoTermCleanup()
{
   // external global dependencies
   extern osg::ref_ptr<osg::StateSet> g_pPolyLineShapeStateSet;

   extern UtoModelPartImp*   m_DummyPart;
   extern UtoModelSwitchImp* m_DummySwitch;

   // external to release the renderbin callbacks
   extern void ReleaseRenderBinCallbacks(void);

   // release the external dependencies
   delete m_DummyPart;
   delete m_DummySwitch;

   g_pPolyLineShapeStateSet = nullptr;

   ReleaseRenderBinCallbacks();

   UtoTexturedPolygonImp::s_Color     = nullptr;
   UtoTexturedPolygonImp::s_Normal    = nullptr;
   UtoTexturedPolygonImp::s_TexCoord  = nullptr;
   UtoTexturedPolygonImp::s_Primitive = nullptr;
}

#if defined(_WIN32) && defined(UTO_BUILD_DLL)

// {secret}
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
   switch (ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
      break;
   }

   return TRUE;
}

#endif

/**
\addtogroup res_db_lookup

This helper function is used to lookup resources in the database.  The
non-template function will return the default shape if the resource
isn't found.  For Visual Studio 7.1 the default shape will be returned
if the template type is UtoShape.  All other calls for the templated
function will cause an exception of type Utok::no_resource to be
thrown instead of returning the default shape.  The default shape is a
shape the user must add with the name "Default".  Utok::no_resource
will also be thrown if the default shape isn't found.  Using this
helper is an easy way to get undefined resources showing up in your
displays when the resource definition file does not have them.  For
instance, a red circle shape could be used as the default shape, which
will be easily seen on your viewers.

This example shows how the function is used to perform the default shape
lookup (With Visual Studio 7.1):

\code
      try {
         airfield->Bind( "2D_body", UtoResDBLookup<UtoShape>("AIRFIELD") );
\endcode

This example shows how the function is used to perform a model shape
lookup without the default shape lookup:

\code
         airframe->Bind( "2D_body", UtoResDBLookup<UtoModelShape>("F15_3D") );
      }catch(Utok::no_resource no_r)
      {
         std::cerr << "Resource " << no_r.what() << " not found\n";
      }
\endcode
*/
template<class T>
T& UtoResDBLookup(const std::string& res_name);
