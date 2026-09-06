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

#ifndef VAOVERLAYELEVATIONLINES_HPP
#define VAOVERLAYELEVATIONLINES_HPP

#include "vespatk_export.h"

#include <osg/ref_ptr>

#include "UtUnitTypes.hpp"
#include "VaOverlay.hpp"

namespace osg
{
class Camera;
class Geode;
class Group;
class Node;
class Program;
class Texture2D;
class Uniform;
} // namespace osg

namespace osgEarth
{
class ImageLayer;
class Map;
class MapNode;
} // namespace osgEarth

class UtoRawShape;

namespace vespa
{
class VESPATK_EXPORT VaOverlayElevationLines : public VaOverlay
{
public:
   VaOverlayElevationLines();
   VaOverlayElevationLines(const std::string& name);
   ~VaOverlayElevationLines() override;

   VaOverlayElevationLines* Clone() const override;

   bool Initialize() override;

   void SetDatasetFileName(const std::string& fileName);
   void SetElevationStep(UtLengthValue value);
   void SetElevationColorRange(UtLengthValue aMin, UtLengthValue aMax);

private:
   VaOverlayElevationLines(const VaOverlayElevationLines& other);

   auto CreateDtedMapNode() -> osg::ref_ptr<osgEarth::MapNode>;
   auto CreateRenderTarget() -> osg::ref_ptr<osg::Texture2D>;
   auto CreateRenderTargetCamera(osg::Texture2D* renderTarget) -> osg::ref_ptr<osg::Camera>;
   auto CreateLinesOverlayQuad(osg::Texture2D* renderTarget) -> osg::ref_ptr<osg::Geode>;
   auto CreateLinesProgram() -> osg::Program*;

   void UpdateDatasetFileName();
   void UpdateElevationStepMeters();

   UtoRawShape*  mShape{};
   UtLengthValue mStep{};
   std::string   mDatasetFileName;
   float         mMin;
   float         mMax;

   osgEarth::Map*             mDtedMap{};
   osg::ref_ptr<osg::Uniform> mStepMetersUniform;
   osg::ref_ptr<osg::Uniform> mMinStepMetersUniform;
   osg::ref_ptr<osg::Uniform> mMaxStepMetersUniform;
   osg::ref_ptr<osg::Uniform> mColorScalarUniform;
   osg::ref_ptr<osg::Uniform> mColorSpaceUniform;
   osg::ref_ptr<osg::Uniform> mTargetLayerUniform;
   osg::ref_ptr<osg::Uniform> mTexCoordUniform;
   osg::ref_ptr<osg::Camera>  mRTT_Cam;
};
} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayElevationLines)
#endif
