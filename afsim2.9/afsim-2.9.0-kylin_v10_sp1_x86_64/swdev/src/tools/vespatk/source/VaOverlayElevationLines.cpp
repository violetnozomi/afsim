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

#include "VaOverlayElevationLines.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PolygonMode>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osgEarth/ElevationLayer>
#include <osgEarth/ImageLayer>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/VirtualProgram>
#include <osgEarthDrivers/tms/TMSOptions>

#include "DtedTmsOptions.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "UtoViewer.hpp"
#include "VaCamera.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"

namespace
{
const float EARTH_MIN_ELEVATION_M = -413.0f;
const float EARTH_MAX_ELEVATION_M = 8850.0f;
const int   RENDER_TEXTURE_WIDTH  = 2048;
const int   RENDER_TEXTURE_HEIGHT = 2048;
} // namespace

namespace vespa
{
using namespace osg;
using namespace osgEarth;
using namespace osgEarth::Drivers;

VaOverlayElevationLines::VaOverlayElevationLines()
   : VaOverlay{"elevationlines", cOVERLAY_WORLD}
   , mMin(0.0f)
   , mMax(1.0f)
   , mRTT_Cam(nullptr)
{
   SetType<VaOverlayElevationLines>();
}

VaOverlayElevationLines::VaOverlayElevationLines(const std::string& name)
   : VaOverlay{name, cOVERLAY_WORLD}
   , mMin(0.0f)
   , mMax(1.0f)
   , mRTT_Cam(nullptr)
{
   SetType<VaOverlayElevationLines>();
}

VaOverlayElevationLines::VaOverlayElevationLines(const VaOverlayElevationLines& other)
   : VaOverlay{other}
   , mMin(0.0f)
   , mMax(1.0f)
   , mRTT_Cam(nullptr)
{
   SetType<VaOverlayElevationLines>();
}

VaOverlayElevationLines::~VaOverlayElevationLines() {}

VaOverlayElevationLines* VaOverlayElevationLines::Clone() const
{
   return new VaOverlayElevationLines{*this};
}

bool VaOverlayElevationLines::Initialize()
{
   auto shape = UtoRawShape{};
   mShape     = dynamic_cast<UtoRawShape*>(BindShape("elevationlines", shape));
   auto root  = mShape->GetRoot();

   auto dtedMapNode  = CreateDtedMapNode();
   auto renderTarget = CreateRenderTarget();
   mRTT_Cam          = CreateRenderTargetCamera(renderTarget);
   mRTT_Cam->addChild(dtedMapNode);

   auto elevationLinesGroup = new Group{};
   elevationLinesGroup->addChild(mRTT_Cam);

   auto quad = CreateLinesOverlayQuad(renderTarget);

   root->addChild(elevationLinesGroup);
   root->addChild(quad);

   UpdateElevationStepMeters();
   UpdateDatasetFileName();

   return true;
}

void VaOverlayElevationLines::SetDatasetFileName(const std::string& fileName)
{
   if (mDatasetFileName != fileName)
   {
      mDatasetFileName = fileName;
      UpdateDatasetFileName();
   }
}

void VaOverlayElevationLines::SetElevationStep(UtLengthValue value)
{
   mStep = value;
   UpdateElevationStepMeters();
}

auto VaOverlayElevationLines::CreateDtedMapNode() -> ref_ptr<osgEarth::MapNode>
{
   mDtedMap         = new Map{};
   auto dtedMapNode = new MapNode{mDtedMap};

   mStepMetersUniform    = ref_ptr<Uniform>{new Uniform{"stepMeters", 0.0f}};
   mMinStepMetersUniform = ref_ptr<Uniform>{new Uniform{"minStepMeters", 0.0f}};
   mMaxStepMetersUniform = ref_ptr<Uniform>{new Uniform{"maxStepMeters", 0.0f}};
   mColorScalarUniform   = ref_ptr<Uniform>{new Uniform{"colorScalar", 2.0f}};
   mTargetLayerUniform   = ref_ptr<Uniform>{new Uniform{"targetLayer", 0}};

   auto stateSet = dtedMapNode->getOrCreateStateSet();
   stateSet->addUniform(mStepMetersUniform);
   stateSet->addUniform(mMinStepMetersUniform);
   stateSet->addUniform(mMaxStepMetersUniform);
   stateSet->addUniform(mColorScalarUniform);
   stateSet->addUniform(mTargetLayerUniform);

   auto vp = VirtualProgram::getOrCreate(stateSet);
   vp->setFunction("ColorizeTerrain",
                   UtoShaders::LoadSource("elevationLinesPass1.frag"),
                   ShaderComp::LOCATION_FRAGMENT_OUTPUT);

   return dtedMapNode;
}

auto VaOverlayElevationLines::CreateRenderTarget() -> ref_ptr<Texture2D>
{
   auto renderTarget = ref_ptr<Texture2D>{new Texture2D{}};

   renderTarget->setTextureSize(RENDER_TEXTURE_WIDTH, RENDER_TEXTURE_HEIGHT);
   renderTarget->setInternalFormat(GL_R32F);
   renderTarget->setSourceFormat(GL_RED);
   renderTarget->setFilter(Texture2D::MIN_FILTER, Texture2D::LINEAR);
   renderTarget->setFilter(Texture2D::MAG_FILTER, Texture2D::LINEAR);

   return renderTarget;
}

auto VaOverlayElevationLines::CreateRenderTargetCamera(Texture2D* renderTarget) -> ref_ptr<Camera>
{
   auto camera = new Camera{};

   camera->setClearColor(Vec4{});
   camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // note: we used to use a FRAME_BUFFER_OBJECT here with PRE_RENDER, but the switch to QOpenGlWidget seemed to break
   // that we stopped PRE_RENDER, which results in a one frame latency
   camera->setRenderTargetImplementation(Camera::FRAME_BUFFER_OBJECT);
   //      camera->setRenderOrder(Camera::PRE_RENDER);

   camera->setViewport(0, 0, renderTarget->getTextureWidth(), renderTarget->getTextureHeight());
   camera->attach(Camera::COLOR_BUFFER, renderTarget);

   return camera;
}

auto VaOverlayElevationLines::CreateLinesOverlayQuad(Texture2D* renderTarget) -> ref_ptr<Geode>
{
   auto geom =
      createTexturedQuadGeometry(Vec3{-1.0f, -1.0f, 0.0f}, Vec3{2.0f, 0.0f, 0.0f}, Vec3{0.0f, 2.0f, 0.0f}, 0.0f, 0.0f, 1.0f, 1.0f);
   geom->setCullingActive(false);
   auto quad = new Geode{};
   quad->addDrawable(geom);

   auto off = StateAttribute::OFF | StateAttribute::PROTECTED | StateAttribute::OVERRIDE;

   auto stateSet = geom->getOrCreateStateSet();
   stateSet->setAttribute(new PolygonMode{PolygonMode::FRONT_AND_BACK, PolygonMode::FILL}, off);
   stateSet->setMode(GL_LIGHTING, off);
   stateSet->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 0.0, 1.0, false));

   auto program = CreateLinesProgram();

   ref_ptr<Image> spectrumImage{new Image{}};
   int            colorMapSize  = 256;
   unsigned char* spectrumBytes = new unsigned char[colorMapSize * 4];
   for (int i = 0; i < colorMapSize; ++i)
   {
      float h = 0.0f;
      float v = 1.0f;
      if (i < 10)
      {
         h = 0.0f;
         v = i / 20.0f + 0.5f;
      }
      else if (i > colorMapSize - 11)
      {
         h = 1.0f;
         v = (colorMapSize - 1 - i) / 20.0f + 0.5f;
      }
      else
      {
         h = (i - 10) / (float)(colorMapSize - 21);
         v = 1.0f;
      }
      float hsv[] = {240.0F * (1.0F - h), 0.8F, v};
      float rgb[3];
      VaUtils::HSV_ToRGB(hsv, rgb);
      spectrumBytes[i * 4 + 0] = (unsigned char)(rgb[0] * 255.F);
      spectrumBytes[i * 4 + 1] = (unsigned char)(rgb[1] * 255.F);
      spectrumBytes[i * 4 + 2] = (unsigned char)(rgb[2] * 255.F);
      spectrumBytes[i * 4 + 3] = 255;
   }
   spectrumImage->setImage(256, 1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, spectrumBytes, osg::Image::USE_NEW_DELETE);
   ref_ptr<Texture1D> spectrum{new Texture1D{}};
   spectrum->setImage(spectrumImage);
   spectrum->setTextureWidth(256);
   spectrum->setInternalFormat(GL_RGBA);
   spectrum->setSourceFormat(GL_RGBA);
   spectrum->setFilter(Texture2D::MIN_FILTER, Texture2D::LINEAR);
   spectrum->setFilter(Texture2D::MAG_FILTER, Texture2D::LINEAR);
   spectrum->setBorderColor(osg::Vec4d(0.0, 0.0, 0.0, 0.0));
   spectrum->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::CLAMP_TO_BORDER);
   spectrum->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);

   stateSet->setTextureAttributeAndModes(1, spectrum, StateAttribute::ON);
   stateSet->addUniform(new Uniform{"spectrum", 1});

   stateSet->setTextureAttributeAndModes(0, renderTarget, StateAttribute::ON | StateAttribute::OVERRIDE);
   stateSet->setAttributeAndModes(program);
   stateSet->addUniform(new Uniform{"texture", 0});
   stateSet->addUniform(mColorScalarUniform);
   mColorSpaceUniform = ref_ptr<Uniform>{new Uniform{"colorSpace", osg::Vec2(0.0f, 1.0f)}};
   SetElevationColorRange(mMin, mMax);
   stateSet->addUniform(mColorSpaceUniform);
   stateSet->setNestRenderBins(false);
   stateSet->setRenderBinDetails(1, "RenderBin");
   mTexCoordUniform = ref_ptr<Uniform>{new Uniform{"textureScalar", osg::Vec2(1.0f, 1.0f)}};
   stateSet->addUniform(mTexCoordUniform);

   auto blendFunc = new BlendFunc{};
   blendFunc->setFunction(BlendFunc::SRC_ALPHA, BlendFunc::ONE_MINUS_SRC_ALPHA);
   stateSet->setMode(GL_BLEND, StateAttribute::ON);
   stateSet->setAttributeAndModes(blendFunc, StateAttribute::ON);

   return quad;
}

auto VaOverlayElevationLines::CreateLinesProgram() -> Program*
{
   return UtoShaders::UseProgram("elevationLinesPass2.vert", "elevationLinesPass2.frag");
}

void VaOverlayElevationLines::UpdateDatasetFileName()
{
   if (!mDtedMap || mDatasetFileName.empty())
      return;

   mDtedMap->clear();

   auto tms        = DtedTmsOptions{};
   tms.url()       = mDatasetFileName;
   tms.coverage()  = true;
   auto dtedLayer  = new ImageLayer{"dted0", tms};
   auto edtedLayer = new ElevationLayer{"elev0", tms};

   mDtedMap->addLayer(dtedLayer);
   mDtedMap->addLayer(edtedLayer);
   mTargetLayerUniform->set(dtedLayer->getUID());
}

void VaOverlayElevationLines::UpdateElevationStepMeters()
{
   if (!mStepMetersUniform)
      return;

   auto stepMeters    = static_cast<float>(mStep.GetAsUnit(UtUnitLength::cMETERS));
   auto minStepMeters = ceil(EARTH_MIN_ELEVATION_M / stepMeters) * stepMeters - stepMeters;
   auto maxStepMeters = ceil(EARTH_MAX_ELEVATION_M / stepMeters) * stepMeters;

   // Line color is adjusted based on the step size. This is because the edge
   // detection algorithm returns varying color based on the step size. This
   // formula is a four parameter logistic curve fitting.
   //
   // a = the minimum value that can be obtained (when x=0)
   // b = Hill's slope of the curve (steepness of the curve at point c)
   // c = point of inflection (point on S that is half way between a and d)
   // d = the maximum value that can be obtained (when x=infinity)
   //
   // y = d + (a-d)/(a+(x/c)^b)
   auto a           = 80.0f * 1e6f;
   auto b           = 1.06f;
   auto c           = 99.0f * 1e-7f;
   auto d           = 0.1f;
   auto colorScalar = d + (a - d) / (1.0f + powf(stepMeters / c, b));

   mStepMetersUniform->set(stepMeters);
   mMinStepMetersUniform->set(minStepMeters);
   mMaxStepMetersUniform->set(maxStepMeters);
   mColorScalarUniform->set(colorScalar);
}

void VaOverlayElevationLines::SetElevationColorRange(UtLengthValue aMin, UtLengthValue aMax)
{
   mMin = aMin;
   mMax = aMax;
   if (mShape)
   {
      mColorSpaceUniform->set(osg::Vec2((aMin - EARTH_MIN_ELEVATION_M) / (EARTH_MAX_ELEVATION_M - EARTH_MIN_ELEVATION_M),
                                        (aMax - EARTH_MIN_ELEVATION_M) / (EARTH_MAX_ELEVATION_M - EARTH_MIN_ELEVATION_M)));
   }
}
} // namespace vespa
