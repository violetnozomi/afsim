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
// UtoOptions.hpp : header file

#if !defined(_UTOOPTIONS_HPP_)
#define _UTOOPTIONS_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <vector>

#include "UtoTypes.hpp"

/////////////////////////////////////////////////////////////////////////////
// UtoOptions

class UtoOptions
{
public:
   typedef std::vector<char> BufType;

   virtual ~UtoOptions();

   static UtoOptions* Instance();

   const std::string& ApplicationPath() const;
   const std::string& ModelsPath() const;
   const std::string& ResourceConfigFile() const;
   const std::string& DebugString() const;
   const std::string& PluginSearchOrder() const;
   bool               UseMipMappingDefault() const;
   bool               UseAntiAliasDefault() const;
   bool               UseTextureMappingDefault() const;
   bool               UseTransparencyDefault() const;
   bool               ShowCullSpheres() const;
   bool               UseSmoothLines() const;
   bool               UseSmoothGrid() const;
   bool               UseAlphaHitTest() const;
   bool               GLCallbacksDisabled() const;
   bool               UseThreadsafeRefCnt() const;
   bool               DisableShaderSupport() const;
   bool               WarnGenericDriver() const;
   int                RenderBinSortMethod() const;

   bool AddModelsPath(const std::string& path);
   void SetModelsPath(const std::string& path);

   // Microsoft's OpenGL driver doesn't support GL_RESCALE_NORMAL,
   // detect if it is available or not and store the value here.
   void SetRescaleNormal(bool enable) { m_bRescaleNormal = enable; }
   bool RescaleNormal() { return m_bRescaleNormal; }

protected:
private:
   UtoOptions();
   UtoOptions(const UtoOptions&);
   UtoOptions& operator=(const UtoOptions&);

   static UtoOptions* m_pMe;

   std::string m_sApplicationPath;
   std::string m_sModelsPath;
   std::string m_sResourceConfigFile;
   std::string m_sDebugString;
   std::string m_sPluginSearchOrder;
   int         m_nRenderBinSortMethod;
   bool        m_bMipMapping;
   bool        m_bAntiAlias;
   bool        m_bTextureMapping;
   bool        m_bTransparency;
   bool        m_bShowCullSpheres;
   bool        m_bSmoothLines;
   bool        m_bSmoothGrid;
   bool        m_bAlphaHitTest;
   bool        m_bGLCallbacksDisabled;
   bool        m_bThreadsafeRefCnt;
   bool        m_bDisableShaderSupport;
   bool        m_bWarnGenericDriver;
   bool        m_bRescaleNormal;
};

#if !defined(_DEBUG)
inline const std::string& UtoOptions::ApplicationPath() const
{
   return m_sApplicationPath;
}

inline const std::string& UtoOptions::ModelsPath() const
{
   return m_sModelsPath;
}

inline const std::string& UtoOptions::ResourceConfigFile() const
{
   return m_sResourceConfigFile;
}

inline const std::string& UtoOptions::DebugString() const
{
   return m_sDebugString;
}

inline const std::string& UtoOptions::PluginSearchOrder() const
{
   return m_sPluginSearchOrder;
}
#endif

inline int UtoOptions::RenderBinSortMethod() const
{
   return m_nRenderBinSortMethod;
}

#endif // !defined(_UTOOPTIONS_HPP_)
