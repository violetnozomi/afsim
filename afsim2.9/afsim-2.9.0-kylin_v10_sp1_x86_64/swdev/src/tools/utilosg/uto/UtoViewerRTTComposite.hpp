// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoViewerRTTComposite.h : header file

#if !defined(_UTOVIEWERRTTCOMPOSITE_HPP_)
#define _UTOVIEWERRTTCOMPOSITE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>
#include <map>
#include <vector>

#include "UtoTypes.hpp"
#include "UtoViewer.hpp"
//#include "UtoSymbol.hpp"
#include <osg/ref_ptr>

#include "UtoCallback.hpp"
#include "UtoViewer.hpp"

class UtoEntity;
class UtoEntityDB;
class UtoViewerProjection;
class UtoHittestStrategy;
class UtoShapeImp;
class UtoNodeReference;

class UTILOSG_EXPORT UtoViewerRTTComposite : public UtoViewer
{
public:
   UtoViewerRTTComposite(const std::string& name = "Viewer", unsigned int aResolutionX = 256, unsigned int aResolutionY = 256);

   ~UtoViewerRTTComposite() override;


   bool Connect(UtoEntityDB& edb) override;

   const osg::Viewport* GetViewport() const override;
   void                 SetViewport(int aX, int aY, int aW, int aH) override;

   void SetViewMatrix(const osg::Matrix& aMatrix) override;
   void SetViewMatrixAsLookAt(const osg::Vec3& aEye, const osg::Vec3& aCenter, const osg::Vec3& aUp) override;
   void SetProjectionMatrixAsPerspective(float aFOV, float aAspectRatio, float aHither, float aYon) override;
   void SetProjectionMatrixAsOrtho(float aMinX, float aMaxX, float aMinY, float aMaxY, float aMinZ, float aMaxZ) override;

   void               SetProjectionMatrixAsFrustum(double aLeft, double aRight, double aBottom, double aTop, double aNear, double aFar) override;
   const osg::Matrix* GetProjectionMatrix() const override;
   osg::Texture2D*    GetTexture() override { return mRawTexture.valid() ? mRawTexture : nullptr; }

   osg::Group* GetRootNode() override { return mTextureCamera; }

   void SetUniform(const char* aName, const float aValue) override;

   void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3) override;

   void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3, const float aValue4) override;

   void SetUniform(const char* aName, const unsigned int aIndex, const char* aFilename) override;


   void         SetUniform(const char*          aName,
                           const unsigned int   aIndex,
                           const unsigned char* aData,
                           const unsigned int   aWidth,
                           const unsigned int   aHeight) override;
   unsigned int GetResolutionX() const { return mResolutionX; }
   unsigned int GetResolutionY() const { return mResolutionY; }

   osg::Light*      GetLight() override { return nullptr; }
   void             SetLightingMode(osg::View::LightingMode aMode) override {}
   void             SetCullingMode(int mode) override;
   void             SetClearColor(const osg::Vec4& aInput) override;
   const osg::Vec4& GetClearColor() override;
   void             BeginUserStat(const std::string& aName) override {}
   void             EndUserStat(const std::string& aName) override {}

protected:
private:
   void Setup();

   osg::ref_ptr<osg::Camera>    mTextureCamera; // this is the camera that looks at the scene to form the texture
   osg::ref_ptr<osg::Texture2D> mRawTexture;

   unsigned int mResolutionX;
   unsigned int mResolutionY;
};

#endif // !defined(_UTOVIEWERRTTCOMPOSITE_HPP_)
