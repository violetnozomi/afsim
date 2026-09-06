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
// Options.cpp : implementation file

#include "UtoOptions.hpp"

#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <limits.h>

#include "UtoObjectManager.hpp"
#include "UtoPriv.hpp"

// osg includes
#include <osgUtil/RenderBin>

static const std::string UTOMODELSPATH("UTOMODELSPATH");
static const std::string UTORESOURCEFILE("utores.dat");
static const std::string ENDTOKENS(" ~`!@#$%^&*()_-+=|]}[{';:/?>,<"
                                   "");

/////////////////////////////////////////////////////////////////////////////

UtoOptions* UtoOptions::m_pMe = nullptr;

UtoOptions::UtoOptions()
   : m_sModelsPath("")
   , m_sResourceConfigFile("")
   , m_sDebugString("")
   , m_sPluginSearchOrder("")
   , m_nRenderBinSortMethod(osgUtil::RenderBin::SORT_BY_STATE)
   , m_bMipMapping(true)
   , m_bAntiAlias(true)
   , m_bTextureMapping(true)
   , m_bTransparency(true)
   , m_bShowCullSpheres(false)
   , m_bSmoothLines(false)
   , m_bSmoothGrid(true)
   , m_bAlphaHitTest(true)
   , m_bGLCallbacksDisabled(false)
   , m_bDisableShaderSupport(false)
   , m_bWarnGenericDriver(true)
   , m_bRescaleNormal(true)
{
   UtoObjectManager::Instance()->Register("UtoOptions", this);
}

UtoOptions::~UtoOptions() {}

UtoOptions* UtoOptions::Instance()
{
   if (m_pMe == nullptr)
   {
      m_pMe = new UtoOptions;
   }
   return m_pMe;
}

bool UtoOptions::AddModelsPath(const std::string& path)
{
   if (path.empty())
      return false;

   // expand path with environment strings.
   std::string sExpanded = UtoExpandEnvString(path);

   // validate a non empty string
   if (sExpanded.empty())
      return false;

   // add path to models path, use the first delimiter.
   if (!m_sModelsPath.empty())
      m_sModelsPath.append(1, *UtoSearchDelimiter);

   m_sModelsPath.append(sExpanded);

   return true;
}

void UtoOptions::SetModelsPath(const std::string& path)
{
   // set the models path.
   m_sModelsPath = path;
}

#if defined(_DEBUG)
const std::string& UtoOptions::ApplicationPath() const
{
   return m_sApplicationPath;
}

const std::string& UtoOptions::ModelsPath() const
{
   return m_sModelsPath;
}

const std::string& UtoOptions::ResourceConfigFile() const
{
   return m_sResourceConfigFile;
}

const std::string& UtoOptions::DebugString() const
{
   return m_sDebugString;
}

const std::string& UtoOptions::PluginSearchOrder() const
{
   return m_sPluginSearchOrder;
}
#endif

bool UtoOptions::UseMipMappingDefault() const
{
   return m_bMipMapping;
}

bool UtoOptions::UseAntiAliasDefault() const
{
   return m_bAntiAlias;
}

bool UtoOptions::UseTextureMappingDefault() const
{
   return m_bTextureMapping;
}

bool UtoOptions::UseTransparencyDefault() const
{
   return m_bTransparency;
}

bool UtoOptions::ShowCullSpheres() const
{
   return m_bShowCullSpheres;
}

bool UtoOptions::UseSmoothLines() const
{
   return m_bSmoothLines;
}

bool UtoOptions::UseSmoothGrid() const
{
   return m_bSmoothGrid;
}

bool UtoOptions::UseAlphaHitTest() const
{
   return m_bAlphaHitTest;
}

bool UtoOptions::GLCallbacksDisabled() const
{
   return m_bGLCallbacksDisabled;
}

bool UtoOptions::UseThreadsafeRefCnt() const
{
   return m_bThreadsafeRefCnt;
}

bool UtoOptions::DisableShaderSupport() const
{
   return m_bDisableShaderSupport;
}

bool UtoOptions::WarnGenericDriver() const
{
   return m_bWarnGenericDriver;
}
