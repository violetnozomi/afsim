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
// UtoViewerHighlightImp.h : header file

#if !defined(_UTOVIEWERHIGHLIGHTIMP_HPP_)
#define _UTOVIEWERHIGHLIGHTIMP_HPP_

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

namespace osgViewer
{
class Viewer;
}

class UtoEntity;
class UtoEntityDB;
class UtoViewerProjection;
class UtoHittestStrategy;
class UtoShapeImp;
class UtoNodeReference;

class UTILOSG_EXPORT UtoViewerHighlight : public UtoViewer
{
public:
   UtoViewerHighlight(const std::string& name                   = "Viewer",
                      int                aEntitySelectionMask   = 0x02,
                      bool               aHideGlowBehindTerrain = false);
   UtoViewerHighlight(osgViewer::Viewer* aViewerPtr,
                      const std::string& name                   = "Viewer",
                      int                aEntitySelectionMask   = 0x02,
                      bool               aHideGlowBehindTerrain = false);

   ~UtoViewerHighlight() override;


   bool Connect(UtoEntityDB& edb) override;

   void SetViewport(int aX, int aY, int aW, int aH) override;

   void SetViewMatrix(const osg::Matrix& aMatrix) override;
   void SetViewMatrixAsLookAt(const osg::Vec3& aEye, const osg::Vec3& aCenter, const osg::Vec3& aUp) override;
   void SetProjectionMatrixAsPerspective(float aFOV, float aAspectRatio, float aHither, float aYon) override;
   void SetProjectionMatrixAsOrtho(float aMinX, float aMaxX, float aMinY, float aMaxY, float aMinZ, float aMaxZ) override;

   virtual void ApplyMyShaderTo(osg::StateSet* ss);
   virtual void ApplyObjectShader(osg::StateSet* ss);

   void HighlightSymbolsWithColor(UtoEntity*                      aEntityPtr,
                                  const std::vector<std::string>& aShapeNameList,
                                  bool                            aOn,
                                  const float                     aColor[4],
                                  bool                            aAutoScale) override;

   void HighlightWithColor(osg::Node* aNodePtr, bool aState, const float aColor[4]) override;

   void HighlightNode(osg::Node* aNodePtr, bool aState, const float aColor[4]) override;

   void SuppressSelectionIndication(bool aState) override;


   void SetUniform(const char* aName, const float aValue) override;

   void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3, const float aValue4) override;

protected:
   void ShapeRemoved(UtoShape* aShapePtr) override;
   //   virtual void  RemoveEntityShapeCB(UtoEntity * entityImp, UtoShape * shape);

private:
   void Setup();
   void NewSymbolAdded(UtoShape* aShapePtr) override;

   osg::ref_ptr<osg::Camera>    mTextureCamera;  // this is the camera that looks at the scene to form the texture
   osg::ref_ptr<osg::Camera>    mLastPassCamera; // this is the camera that redraws the selected shapes
   osg::ref_ptr<osg::Texture2D> mRawTexture;
   osg::ref_ptr<osg::Geode>     mQuadGeode;
   osg::ref_ptr<osg::Group>     mSelectedEntityGroup;

   int  mEntitySelectionMask;
   bool mShowHighlightSelection;
   bool mHideGlowBehindTerrain;

   std::map<osg::Node*, UtoNodeReference*> mNodeToRef;
};

#endif // !defined(_UTOVIEWERHIGHLIGHTIMP_HPP_)
