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

// UtoCmeTerrainImp.cpp: implementation of the UtoCmeTerrainImp class.
//
//////////////////////////////////////////////////////////////////////

#include "UtoCmeTerrainImp.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Material>
#include <osg/NodeCallback>
#include <osg/PolygonOffset>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Texture1D>
#include <osgDB/ReadFile>
#include <osgEarth/GLUtils>
#include <osgEarth/ImageLayer>
#include <osgEarth/MapNode>
#include <osgEarth/VirtualProgram>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/osg/OSGOptions>
#include <osgEarthFeatures/FeatureModelLayer>
#include <osgEarthFeatures/Filter>
#include <osgEarthUtil/GLSLColorFilter>
#include <osgUtil/CullVisitor>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/Optimizer>

#include "UtoGeometryUtil.hpp"
#include "UtoMemory.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"
#include "UtoShaderUtilities.hpp"
#include "UtoShaders.hpp"
#include "UtoTerrainColorVisitor.hpp"
#include "UtoUtility.hpp"
#include "UtoViewer.hpp"

OSGEARTH_REGISTER_SIMPLE_FEATUREFILTER(simpleattributefilter, SimpleAttributeFilter);

Utok::Properties_T<UtoCmeTerrainImp> UtoCmeTerrainImp::m_Properties;

///////////////////////////////////////////////////////////////////////////////

UtoCmeTerrainImp::UtoCmeTerrainImp()
   : m_pSetColorTransparency(nullptr)
   , m_pUpdateVisitor(nullptr)
   , mOsgEarthMapNode(nullptr)
{
   InitMap();
   mLit = false;
}

UtoCmeTerrainImp::UtoCmeTerrainImp(const UtoCmeTerrainImp& rhs)
   : UtoTerrainImp(rhs)
   , m_Path(rhs.m_Path)
   , m_pSetColorTransparency(rhs.m_pSetColorTransparency)
   , m_RootNode(rhs.m_RootNode)
   , m_pUpdateVisitor(rhs.m_pUpdateVisitor)
   , mOsgEarthMapNode(rhs.mOsgEarthMapNode)
{
   SetProperties(&m_Properties);
   mLit = rhs.mLit;
}

UtoCmeTerrainImp::~UtoCmeTerrainImp() {}

void UtoCmeTerrainImp::LoadVirtualPrograms()
{
   std::string colorCode;
   std::string vertexCode;
   if (Projection().projection == UtoMapProjection::Equirectangular) // flat
   {
      vertexCode += "uniform mat4 osg_ViewMatrix;\n";
      vertexCode += "uniform mat4 osg_ViewMatrixInverse;\n";
      vertexCode += "uniform vec3 vtk_sunVec;\n";
      vertexCode += "varying float lightIntensity;\n";
      vertexCode += "const float fMeterPerRad = 6366707.019;\n";
      vertexCode += "void vtkVertexMod(inout vec4 vertex)\n";
      vertexCode += "{\n";
      if (mLit)
      {
         vertexCode += "   vec4 light = osg_ViewMatrix * vec4(vtk_sunVec, 0);\n";
         vertexCode += "   vec4 v = osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex;\n";
         vertexCode += "   float lat = v.y / fMeterPerRad;\n";
         vertexCode += "   float lon = v.x / fMeterPerRad;\n";
         vertexCode += "   vec3 tnorm = vec3(cos(lon) * cos(lat), sin(lon) * cos(lat), sin(lat));\n";
         vertexCode += "   normalize(tnorm);\n";
         vertexCode += "   lightIntensity = min(max(5.0 * dot(light.xyz, tnorm), 0.0) + 0.2, 1.0);\n";
      }
      else
      {
         vertexCode += "   lightIntensity = 1.0;\n";
      }
      vertexCode += "}\n";
   }
   else
   {
      vertexCode += "uniform mat4 osg_ViewMatrix;\n";
      vertexCode += "uniform vec3 vtk_sunVec;\n";
      vertexCode += "varying float lightIntensity;\n";
      vertexCode += "void vtkVertexMod(inout vec4 vertex)\n";
      vertexCode += "{\n";
      if (mLit)
      {
         vertexCode += "   vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal);\n";
         vertexCode += "   vec4 light = osg_ViewMatrix * vec4(vtk_sunVec, 0);\n";
         vertexCode += "   lightIntensity = min(max(5.0 * dot(light.xyz, tnorm), 0.0) + 0.2, 1.0);\n";
      }
      else
      {
         vertexCode += "   lightIntensity = 1.0;\n";
      }
      vertexCode += "}\n";
   }
   std::string logarithmicDepthCode;
   logarithmicDepthCode += "uniform float VA_LogDepth;\n";
   logarithmicDepthCode += "uniform mat4 UTO_ProjectionMatrixInverse;\n";
   logarithmicDepthCode += "void vtkLogDepth(inout vec4 vertex)\n";
   logarithmicDepthCode += "{\n";
   logarithmicDepthCode += "   mat4 clip2view = UTO_ProjectionMatrixInverse;\n";
   logarithmicDepthCode += "   vec4 farPoint = clip2view * vec4(0.0, 0.0, 1.0, 1.0);\n";
   logarithmicDepthCode += "   float FAR = -farPoint.z / farPoint.w;\n";
   logarithmicDepthCode += "   float C = 10.0;\n";
   logarithmicDepthCode +=
      "   if (VA_LogDepth > 0.5) vertex.z = log(C * vertex.z + 1.0) / log(C * FAR + 1.0) * vertex.w;";
   logarithmicDepthCode += "}\n";

   colorCode += "uniform float fSaturation;\n";
   colorCode += "uniform float fBrightener;\n";
   colorCode += "uniform float fDarkener;\n";
   colorCode += "uniform float fContrast;\n";
   colorCode += "varying float lightIntensity;\n";
   colorCode += "void vtkColorMod(inout vec4 color)\n";
   colorCode += "{\n";
   colorCode += "   color = vec4(color.rgb + (1.0 - color.rgb) * fBrightener + -color.rgb * fDarkener, color.a);\n";
   colorCode += "   color = vec4((color.rgb - 0.5) * (fContrast + 1.0) + 0.5, color.a);\n";
   colorCode += "   float mean = 0.3333 * (color.r + color.g + color.b);\n";
   colorCode += "   color = vec4(lightIntensity * (fSaturation * (color.rgb - mean) + color.rgb), color.a);\n";
   colorCode += "}\n";
   osgEarth::VirtualProgram* vp = osgEarth::VirtualProgram::getOrCreate(mOsgEarthMapNode->getOrCreateStateSet());
   vp->setFunction("vtkVertexMod", vertexCode, osgEarth::ShaderComp::LOCATION_VERTEX_MODEL);
   vp->setFunction("vtkColorMod", colorCode, osgEarth::ShaderComp::LOCATION_FRAGMENT_LIGHTING);
   vp->setFunction("vtkLogDepth", logarithmicDepthCode, osgEarth::ShaderComp::LOCATION_VERTEX_CLIP);
}

void UtoCmeTerrainImp::InitMap()
{
   if (m_Properties.IsEmpty())
   {
      m_Properties.Set("file", &UtoCmeTerrainImp::ReadFile);
   }

   SetProperties(&m_Properties);
}

bool UtoCmeTerrainImp::PostLoad()
{
   if (!UtoTerrainImp::PostLoad())
      return false;

   // The terrain will be loaded when added to the world, not before.

   return !m_Path.empty();
}

typedef std::vector<std::string> Paths;
Paths                            CmeUnpackPaths(const std::string& aPath, const std::string& separator)
{
   Paths       paths;
   std::size_t start = 0;
   std::size_t stop  = aPath.find(separator, start);
   while (stop != std::string::npos)
   {
      std::string addPath = aPath.substr(start, stop - start);
      paths.push_back(addPath);
      start = stop + 1;
      stop  = aPath.find(separator, start);
   }
   std::string lastPath = aPath.substr(start, aPath.length() - start);
   paths.push_back(lastPath);
   return paths;
}
const std::string sPATH_SEPARATOR(";");

void UtoCmeTerrainImp::SetFiles(const std::string& aFiles)
{
   m_Path = aFiles;
}

bool UtoCmeTerrainImp::ReadFile(const std::string& attr, const std::string& buf)
{
   m_Path      = "";
   Paths paths = CmeUnpackPaths(buf, sPATH_SEPARATOR);
   int   count = 0;
   for (auto&& iter : paths)
   {
      if (count > 0)
      {
         m_Path += ";";
      }
      count++;
      // need to resolve filename here since ModelsPath() will contain paths
      // specified in the resource definition file.
      std::string path = (UtoFindFirstPath(iter.c_str(), UtoOptions::Instance()->ModelsPath())).c_str();
      m_Path += path;
   }
   return !m_Path.empty();
}

void UtoCmeTerrainImp::Dump(std::ostream& o) {}

osg::Node* UtoCmeTerrainImp::AddToWorld(UtoViewer& viewer)
{
   osg::Group*     node  = new osg::Group;
   osg::BlendFunc* blend = new osg::BlendFunc;
   node->getOrCreateStateSet()->setAttributeAndModes(blend);

   Paths              paths = CmeUnpackPaths(m_Path, sPATH_SEPARATOR);
   osgUtil::Optimizer optimizer;
   for (auto&& iter : paths)
   {
      // load the subgraph for the Cme terrain.
      osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(iter.c_str());
      if (loadedModel)
      {
         mOsgEarthMapNode = dynamic_cast<osgEarth::MapNode*>(loadedModel.get());
         if (mOsgEarthMapNode)
         {
            osgEarth::GLUtils::setGlobalDefaults(mOsgEarthMapNode->getOrCreateStateSet());
            LoadVirtualPrograms();
         }
         // optimize the scene graph, remove redundant nodes and state etc.
         optimizer.optimize(loadedModel.get());
         node->addChild(loadedModel.get());
      }
   }

   if (!IsColorBlendingDisabled())
   {
      InitializeTerrainBlending(node);
   }

   // store the root node
   m_RootNode = node;

   if (node->getOrCreateStateSet()->getNestRenderBins())
   {
      node->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
   }

   return node;
}

void UtoCmeTerrainImp::RemoveFromWorld(UtoViewer& Viewer) {}

void UtoCmeTerrainImp::SetColor(const UtoColor& rColor, unsigned int nClrMask)
{
   // validate the change
   if (((nClrMask & UtoColor::ALPHA) && m_oBlendColor.AlphaF() != rColor.AlphaF()) ||
       ((nClrMask & UtoColor::RED) && m_oBlendColor.RedF() != rColor.RedF()) ||
       ((nClrMask & UtoColor::GREEN) && m_oBlendColor.GreenF() != rColor.GreenF()) ||
       ((nClrMask & UtoColor::BLUE) && m_oBlendColor.BlueF() != rColor.BlueF()))
   {
      // set the color
      UtoTerrainImp::SetColor(rColor, nClrMask);

      // validate and call the member function
      if (m_pSetColorTransparency)
         (this->*m_pSetColorTransparency)();
   }
}

void UtoCmeTerrainImp::SetShaderColorTransparency()
{
   // change the blending color inside the fragment shader
   UtoGeometryUtil::SetColorTransparencyPixelShader(m_RootNode.get(), m_oBlendColor);
}

void UtoCmeTerrainImp::SetVisitorColorTransparency()
{
   // obtain a local blend vector
   osg::Vec4& vBlendColor = m_pUpdateVisitor->m_vBlendColor;

   // set the new color and transparency
   vBlendColor[0] = (float)m_oBlendColor.RedF();
   vBlendColor[1] = (float)m_oBlendColor.GreenF();
   vBlendColor[2] = (float)m_oBlendColor.BlueF();
   vBlendColor[3] = (float)m_oBlendColor.AlphaF();

   // reset the node colors
   m_pUpdateVisitor->ClearChildNodes();
}

osg::ref_ptr<osg::Texture2D> UtoCmeTerrainImp::CreateProjectorRenderTarget()
{
   osg::ref_ptr<osg::Texture2D> renderTarget = ut::osg::make_ref<osg::Texture2D>();

   renderTarget->setTextureSize(1024, 1024);

   renderTarget->setInternalFormat(GL_R32F);
   renderTarget->setSourceFormat(GL_RED);
   renderTarget->setSourceType(GL_FLOAT);
   renderTarget->setBorderColor(osg::Vec4(std::numeric_limits<float>::lowest(), 0.0F, 0.0F, 0.0F));
   renderTarget->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
   renderTarget->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);

   return renderTarget;
}

osg::ref_ptr<osg::Camera> UtoCmeTerrainImp::CreateProjectorRenderTargetCamera(osg::Texture2D& aTexture)
{
   auto camera = new osg::Camera();
   // note: we used to use a FRAME_BUFFER_OBJECT here with PRE_RENDER, but the switch to QOpenGlWidget seemed to break
   // that we stopped PRE_RENDER, which results in a one frame latency
   camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
   //      camera->setRenderOrder(Camera::PRE_RENDER);
   camera->setViewport(0, 0, aTexture.getTextureWidth(), aTexture.getTextureHeight());

   camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


   camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   camera->attach(osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER0), &aTexture);
   camera->setCullingMode(camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
   camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

   static const char* vertSource = "#version " GLSL_VERSION_STR "\n"
                                   "out vec4 projcoord;\n"
                                   "void oe_mrt_vertex(inout vec4 vertexClip)\n"
                                   "{\n"
                                   "    projcoord = vertexClip;"
                                   "}\n";

   static const char* fragSource =
      "#version " GLSL_VERSION_STR "\n"
      "in vec4 projcoord;\n"
      "layout(location=0) out vec4 gdepth; \n"
      "void oe_mrt_fragment(inout vec4 color)\n"
      "{\n"
      "    gdepth = vec4(projcoord.z / projcoord.w, projcoord.z / projcoord.w, projcoord.z / projcoord.w, 1.0); \n"
      "}\n";
   osgEarth::VirtualProgram* vp = osgEarth::VirtualProgram::getOrCreate(camera->getOrCreateStateSet());
   vp->setFunction("oe_mrt_vertex", vertSource, osgEarth::ShaderComp::LOCATION_VERTEX_CLIP);
   vp->setFunction("oe_mrt_fragment", fragSource, osgEarth::ShaderComp::LOCATION_FRAGMENT_OUTPUT);

   return camera;
}

void UtoCmeTerrainImp::Height(double* rPosn, UtoOrientation* pOrient)
{
   // call the base height of terrain function
   HeightOfTerrain(m_RootNode.get(), rPosn, pOrient);
}

bool UtoCmeTerrainImp::Intersect(const double aPos[3],
                                 const double aVec[3],
                                 double       aIPoint[3],
                                 double       aINorm[3],
                                 double       aMagnitude) const
{
   return IntersectTerrain(m_RootNode.get(), aPos, aVec, aIPoint, aINorm, aMagnitude);
}

bool UtoCmeTerrainImp::Intersect(osgUtil::IntersectionVisitor* aIsectVisitor) const
{
   m_RootNode->accept(*aIsectVisitor);
   return true;
}

void UtoCmeTerrainImp::DisableColorBlending(bool bDisable)
{
   // validate a change
   if (bDisable != IsColorBlendingDisabled())
   {
      // call the base class
      UtoTerrainImp::DisableColorBlending(bDisable);

      // states to be concerned about
      // T -> F ===> Disable shader or visitor
      // F -> T ===> Enable shader or visitor
      if (bDisable)
      {
         // disable shader or visitor
         if (m_pSetColorTransparency == &UtoCmeTerrainImp::SetShaderColorTransparency)
         {
            // disable the shader
            UtoGeometryUtil::RemoveColorTransparencyPixelShader(m_RootNode.get());
         }
         else
         {
            // remove the terrain update vistor
            m_RootNode->setUpdateCallback(nullptr);
            // create a local visitor to reset the colors to white
            UtoClearTerrainColorVisitor oClearTerrain;
            // visit all the nodes and reset the values
            m_RootNode->accept(oClearTerrain);
         }

         // remove the current color
         m_oBlendColor = UtoColor(0xFF, 0xFF, 0xFF, 0xFF);
         // remove the function pointer
         m_pSetColorTransparency = nullptr;
      }
      else
      {
         // enable shader or visitor
         InitializeTerrainBlending(m_RootNode.get());
      }
   }
}

void UtoCmeTerrainImp::InitializeTerrainBlending(osg::Node* pNode)
{
   UtoMapProjection projectionPtr = Projection();
   // try to attach a shader to the root node
   bool bShaderEnabled = UtoShaderUtilities::ShadersEnabled();
   if (bShaderEnabled)
   {
      std::set<std::string> nullDefs;
      if (!mLit)
      {
         if (projectionPtr.projection == UtoMapProjection::Geocentric)
         {
            pNode->getOrCreateStateSet()->setAttributeAndModes(
               UtoShaders::UseProgram("terrainRoundUnlit.vert", "terrainRoundUnlit.frag", nullDefs));
         }
         else if (projectionPtr.projection == UtoMapProjection::Equirectangular)
         {
            pNode->getOrCreateStateSet()->setAttributeAndModes(
               UtoShaders::UseProgram("terrainFlatUnlit.vert", "terrainFlatUnlit.frag", nullDefs));
         }
      }
      else
      {
         if (projectionPtr.projection == UtoMapProjection::Geocentric)
         {
            pNode->getOrCreateStateSet()->setAttributeAndModes(
               UtoShaders::UseProgram("terrainRoundLit.vert", "terrainRoundLit.frag", nullDefs));
         }
         else if (projectionPtr.projection == UtoMapProjection::Equirectangular)
         {
            pNode->getOrCreateStateSet()->setAttributeAndModes(
               UtoShaders::UseProgram("terrainFlatLit.vert", "terrainFlatLit.frag", nullDefs));
         }
      }
   }

   pNode->getOrCreateStateSet()->getOrCreateUniform("Texture0", osg::Uniform::INT)->set(0);
   pNode->getOrCreateStateSet()->getOrCreateUniform("Texture1", osg::Uniform::INT)->set(1);
   pNode->getOrCreateStateSet()->getOrCreateUniform("Texture2", osg::Uniform::INT)->set(2);
   pNode->getOrCreateStateSet()->getOrCreateUniform("Texture3", osg::Uniform::INT)->set(3);
   pNode->getOrCreateStateSet()->getOrCreateUniform("Texture7", osg::Uniform::INT)->set(7);

   // if a shader is not being used, use the mark node visitor
   if (!bShaderEnabled)
   {
      // create a new update visitor
      m_pUpdateVisitor = new UtoTerrainUpdateMarkNodeVisitor((float)m_oBlendColor.RedF(),
                                                             (float)m_oBlendColor.GreenF(),
                                                             (float)m_oBlendColor.BlueF(),
                                                             (float)m_oBlendColor.AlphaF());

      // set the update callback for the root node
      pNode->setUpdateCallback(m_pUpdateVisitor.get());

      // set the correct attributes for the member function pointers
      m_pSetColorTransparency = &UtoCmeTerrainImp::SetVisitorColorTransparency;
   }
   else
   {
      // set the correct attributes for the member function pointers
      m_pSetColorTransparency = &UtoCmeTerrainImp::SetShaderColorTransparency;
      // attach the cull face gl attribute to the terrain
      pNode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
   }

   // Depth writes are off in equirectangular mode
   osg::Depth* depthAttribute = new osg::Depth();
   depthAttribute->setWriteMask(projectionPtr.projection == UtoMapProjection::Geocentric);
   pNode->getOrCreateStateSet()->setAttribute(depthAttribute);
}

void UtoCmeTerrainImp::EnableLighting(const bool aLit)
{
   mLit = aLit;
   LoadVirtualPrograms();
}

bool UtoCmeTerrainImp::Lighting()
{
   return mLit;
}

bool UtoCmeTerrainImp::SupportsLayers() const
{
   return (mOsgEarthMapNode != nullptr);
}

bool UtoCmeTerrainImp::AddShapeLayer(const std::string& aFilename)
{
   bool retval = true;
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* ml = mOsgEarthMapNode->getMap()->getLayerByName(aFilename);
      if (nullptr != ml)
      {
         mOsgEarthMapNode->getMap()->removeLayer(ml);
         retval = false;
      }
      osgEarth::Drivers::OGRFeatureOptions featureOptions;
      featureOptions.url()               = aFilename;
      featureOptions.buildSpatialIndex() = true;
      featureOptions.filters().push_back(SimpleAttributeFilterOptions(osgEarth::Config("simpleattributefilter")));

      osgEarth::Drivers::FeatureGeomModelOptions geomOptions;

      geomOptions.enableLighting() = false;
      geomOptions.featureOptions() = featureOptions;
      geomOptions.styles()         = new osgEarth::StyleSheet();

      osgEarth::Features::FeatureModelLayerOptions layerOptions(geomOptions);
      layerOptions.name() = aFilename;
      mOsgEarthMapNode->getMap()->addLayer(new osgEarth::Features::FeatureModelLayer(layerOptions));
   }
   return retval;
}

bool UtoCmeTerrainImp::AddShapeLayer(const std::string& aFilename, float aRed, float aGreen, float aBlue)
{
   bool retval = true;
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* ml = mOsgEarthMapNode->getMap()->getLayerByName(aFilename);
      if (nullptr != ml)
      {
         mOsgEarthMapNode->getMap()->removeLayer(ml);
         retval = false;
      }
      osgEarth::Drivers::OGRFeatureOptions featureOptions;
      featureOptions.url()               = aFilename;
      featureOptions.buildSpatialIndex() = true;

      osgEarth::Drivers::FeatureGeomModelOptions geomOptions;

      osgEarth::Style sstyle;
      sstyle.setName("outlines");
      osgEarth::LineSymbol* ls     = sstyle.getOrCreateSymbol<osgEarth::LineSymbol>();
      ls->stroke()->color()        = osgEarth::Color(aRed, aGreen, aBlue);
      ls->stroke()->width()        = 2.0f;
      geomOptions.enableLighting() = false;
      geomOptions.featureOptions() = featureOptions;
      geomOptions.styles()         = new osgEarth::StyleSheet();

#ifdef EXTRUSION
      osgEarth::Style bstyle;
      bstyle.setName("buildings");
      osgEarth::ExtrusionSymbol* es = bstyle.getOrCreateSymbol<osgEarth::ExtrusionSymbol>();
      es->heightExpression()        = osgEarth::NumericExpression("3.5 * max( [story_ht_], 1)");
      es->flatten()                 = true;

      osgEarth::PolygonSymbol* poly = bstyle.getOrCreateSymbol<osgEarth::PolygonSymbol>();
      poly->fill()->color()         = osgEarth::Color(aRed, aGreen, aBlue);

      osgEarth::AltitudeSymbol* alt = bstyle.getOrCreateSymbol<osgEarth::AltitudeSymbol>();
      alt->clamping()               = alt->CLAMP_TO_TERRAIN;
      alt->binding()                = alt->BINDING_VERTEX;

      osgEarth::Features::FeatureDisplayLayout layout;
      layout.tileSizeFactor() = 52.0;
      layout.addLevel(osgEarth::Features::FeatureLevel(0.0f, 20000.0f, "buildings"));
      layout.addLevel(osgEarth::Features::FeatureLevel(0.0f, FLT_MAX, "outlines"));
      geomOptions.layout()         = layout;
      geomOptions.enableLighting() = true;
      geomOptions.styles()->addStyle(bstyle);
#endif
      geomOptions.styles()->addStyle(sstyle);

      osgEarth::Features::FeatureModelLayerOptions layerOptions(geomOptions);
      layerOptions.name() = aFilename;
      mOsgEarthMapNode->getMap()->addLayer(new osgEarth::Features::FeatureModelLayer(layerOptions));
   }
   return retval;
}

void UtoCmeTerrainImp::RemoveLayer(const std::string& aFilename)
{
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* layer = mOsgEarthMapNode->getMap()->getLayerByName(aFilename);
      if (nullptr != layer)
      {
         mOsgEarthMapNode->getMap()->removeLayer(layer);
      }
   }
}

void UtoCmeTerrainImp::ShowLayer(const std::string& aFilename, bool aState)
{
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* layer = mOsgEarthMapNode->getMap()->getLayerByName(aFilename);
      if (nullptr != layer)
      {
         layer->setEnabled(aState);
      }
   }
}

void UtoCmeTerrainImp::SetShapeLayerColor(const std::string& aFilename, float aRed, float aGreen, float aBlue)
{
   AddShapeLayer(aFilename, aRed, aGreen, aBlue);
}

bool UtoCmeTerrainImp::AddImageLayer(const std::string& aFilename)
{
   bool retval = true;
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* il = mOsgEarthMapNode->getMap()->getLayerByName(aFilename);
      if (nullptr != il)
      {
         mOsgEarthMapNode->getMap()->removeLayer(il);
         retval = false;
      }
      osgEarth::Drivers::GDALOptions driverOpts;
      driverOpts.url() = aFilename;
      mOsgEarthMapNode->getMap()->addLayer(new osgEarth::ImageLayer(osgEarth::ImageLayerOptions(aFilename, driverOpts)));

      osgEarth::ImageLayer* img =
         dynamic_cast<osgEarth::ImageLayer*>(mOsgEarthMapNode->getMap()->getLayerByName(aFilename));
      if (img->getProfile() == nullptr)
      {
         mOsgEarthMapNode->getMap()->removeLayer(img);
         return false;
      }
   }
   return retval;
}

unsigned int UtoCmeTerrainImp::AddProjector(const std::string& aName, const UtColor& aColor)
{
   if (nullptr != mOsgEarthMapNode)
   {
      // getOrCreate ensures we don't overwrite an existing VP.
      osg::StateSet&            ss                     = *mOsgEarthMapNode->getOrCreateStateSet();
      unsigned int              samplerIndex           = ut::osg::GetAvailableTextureUnit(ss);
      std::string               idxStr                 = std::to_string(samplerIndex);
      std::string               projectorImageUniform  = "projectorImage_" + idxStr;
      std::string               projectorMatrixUniform = "projectorMatrix_" + idxStr;
      std::string               projectorColorUniform  = "projectorColor_" + idxStr;
      std::string               vertFunctionName       = "ProjectorCoord_" + idxStr;
      std::string               fragFunctionName       = "ProjectorColor_" + idxStr;
      std::string               coordVariant           = "projCoord_" + idxStr;
      osg::ref_ptr<osg::Camera> rttCam;

      auto texture = CreateProjectorRenderTarget();
      rttCam       = CreateProjectorRenderTargetCamera(*texture.get());
      rttCam->addChild(mOsgEarthMapNode);
      auto group = m_RootNode->asGroup();
      if (group)
      {
         group->addChild(rttCam);
      }

      std::string calcProjCoordCode = "uniform mat4 " + projectorMatrixUniform +
                                      ";\n"
                                      "uniform mat4 osg_ViewMatrixInverse;\n"
                                      "varying vec4 " +
                                      coordVariant +
                                      ";\n"
                                      "void " +
                                      vertFunctionName +
                                      "(inout vec4 vertex)\n"
                                      "{\n"
                                      "   " +
                                      coordVariant + " = " + projectorMatrixUniform +
                                      " * osg_ViewMatrixInverse * gl_ModelViewMatrix * vertex;\n"
                                      "}\n";
      std::string fs = "uniform sampler2D " + projectorImageUniform +
                       ";\n"
                       "uniform vec4 " +
                       projectorColorUniform +
                       ";\n"
                       "varying vec4 " +
                       coordVariant +
                       ";\n"
                       "void " +
                       fragFunctionName +
                       "(inout vec4 color)\n"
                       "{\n"
                       "   vec3 depthCoord = " +
                       coordVariant + ".xyz / " + coordVariant +
                       ".w;\n"
                       "   float closestDepth = texture2D(" +
                       projectorImageUniform +
                       ", 0.5 + depthCoord.xy * 0.5).x;\n"
                       "   if (depthCoord.z - closestDepth < 0.0002)\n"
                       "      color = mix(color, " +
                       projectorColorUniform +
                       ", 0.6);\n"
                       "}\n";

      osgEarth::VirtualProgram* vp = osgEarth::VirtualProgram::getOrCreate(&ss);
      vp->setFunction(vertFunctionName, calcProjCoordCode, osgEarth::ShaderComp::LOCATION_VERTEX_MODEL);
      vp->setFunction(fragFunctionName, fs, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING);
      ss.setTextureAttributeAndModes(samplerIndex, texture.get(), osg::StateAttribute::ON);
      ss.getOrCreateUniform(projectorImageUniform, osg::Uniform::SAMPLER_2D)->set((int)samplerIndex);
      float rgba[4];
      aColor.Get(rgba);
      ss.getOrCreateUniform(projectorColorUniform, osg::Uniform::FLOAT_VEC4)
         ->set(osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
      mProjectorMap[samplerIndex] = rttCam;

      return samplerIndex;
   }
   return 0;
}

void UtoCmeTerrainImp::RemoveProjector(unsigned int aId)
{
   if (nullptr != mOsgEarthMapNode)
   {
      // getOrCreate ensures we don't overwrite an existing VP.
      osg::StateSet* ss = mOsgEarthMapNode->getStateSet();
      if (ss != nullptr)
      {
         std::string               strid            = std::to_string(aId);
         std::string               vertFunctionName = "ProjectorCoord_" + strid;
         std::string               fragFunctionName = "ProjectorColor_" + strid;
         osgEarth::VirtualProgram* vp               = osgEarth::VirtualProgram::getOrCreate(ss);
         vp->removeShader(vertFunctionName);
         vp->removeShader(fragFunctionName);                            // remove the virtual program shaders
         ss->removeTextureAttribute(aId, osg::StateAttribute::TEXTURE); // remove the texture
         auto&& proj = mProjectorMap.find(aId);
         if (proj != mProjectorMap.end())
         {
            proj->second->getParent(0)->removeChild(proj->second); // remove the camera
         }
      }
   }
   mProjectorMap.erase(aId);
}

void UtoCmeTerrainImp::UpdateProjectorMatrix(unsigned int       aId,
                                             const osg::Matrix& aViewMatrix,
                                             const osg::Matrix& aProjectionMatrix)
{
   auto&& jt = mProjectorMap.find(aId);
   if (jt != mProjectorMap.end())
   {
      std::string projectorMatrixUniform = "projectorMatrix_" + std::to_string(aId);
      mOsgEarthMapNode->getOrCreateStateSet()
         ->getOrCreateUniform(projectorMatrixUniform, osg::Uniform::FLOAT_MAT4)
         ->set(aViewMatrix * aProjectionMatrix);
      jt->second->setViewMatrix(aViewMatrix);
      jt->second->setProjectionMatrix(aProjectionMatrix);
   }
}

bool UtoCmeTerrainImp::AddSensorPlotImageLayer(const std::string&              aFilename,
                                               const std::string&              aIdentifier,
                                               double                          aMinValue,
                                               double                          aMaxValue,
                                               unsigned int                    aDataSet,
                                               const std::map<float, UtColor>& aGradient)
{
   bool retval = true;
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* il = mOsgEarthMapNode->getMap()->getLayerByName(aIdentifier);
      if (nullptr != il)
      {
         mOsgEarthMapNode->getMap()->removeLayer(il);
         retval = false;
      }
      osgEarth::Drivers::GDALOptions driverOpts;
      driverOpts.url()        = aFilename;
      driverOpts.subDataSet() = aDataSet + 1;
      driverOpts.coverage()   = true;

      osgEarth::ImageLayerOptions layerOpt(aIdentifier, driverOpts);

      mOsgEarthMapNode->getMap()->addLayer(new osgEarth::ImageLayer(layerOpt));
      SetSensorPlotLayerGradient(aIdentifier, aMinValue, aMaxValue, aGradient);
      osgEarth::ImageLayer* img =
         dynamic_cast<osgEarth::ImageLayer*>(mOsgEarthMapNode->getMap()->getLayerByName(aIdentifier));

      if (img->getProfile() == nullptr)
      {
         mOsgEarthMapNode->getMap()->removeLayer(img);
         return false;
      }
   }
   return retval;
}

void UtoCmeTerrainImp::SetSensorPlotLayerGradient(const std::string               aIdentifier,
                                                  double                          aMinValue,
                                                  double                          aMaxValue,
                                                  const std::map<float, UtColor>& aGradient)
{
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::ImageLayer* img =
         dynamic_cast<osgEarth::ImageLayer*>(mOsgEarthMapNode->getMap()->getLayerByName(aIdentifier));

      if (nullptr != img)
      {
         osgEarth::ColorFilterChain chain = img->getColorFilters();
         for (auto& f : chain)
         {
            img->removeColorFilter(f);
         }
         img->addColorFilter(new SensorPlotColorFilter(aMinValue, aMaxValue, aGradient));
      }
   }
}

bool UtoCmeTerrainImp::AddImageLayer(const std::string& aFilename, double aMinLat, double aMinLon, double aMaxLat, double aMaxLon)
{
   bool retval = true;
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::Layer* il = mOsgEarthMapNode->getMap()->getLayerByName(aFilename);
      if (nullptr != il)
      {
         mOsgEarthMapNode->getMap()->removeLayer(il);
         retval = false;
      }
      osgEarth::Drivers::OSGOptions driverOpts;
      driverOpts.url()                  = aFilename;
      driverOpts.profile()->srsString() = "+proj=latlong +ellps=WGS84 +datum=WGS84";
      driverOpts.profile()->bounds()    = osgEarth::Bounds(aMinLon, aMinLat, aMaxLon, aMaxLat);

      osgEarth::ImageLayerOptions layerOpt(aFilename, driverOpts);

      mOsgEarthMapNode->getMap()->addLayer(new osgEarth::ImageLayer(layerOpt));
      osgEarth::ImageLayer* img =
         dynamic_cast<osgEarth::ImageLayer*>(mOsgEarthMapNode->getMap()->getLayerByName(aFilename));
      if (img->getProfile() == nullptr)
      {
         mOsgEarthMapNode->getMap()->removeLayer(img);
         return false;
      }
   }
   return retval;
}

void UtoCmeTerrainImp::SetImageLayerOpacity(const std::string& aIdentifier, float aValue)
{
   if (nullptr != mOsgEarthMapNode)
   {
      osgEarth::ImageLayer* layer =
         dynamic_cast<osgEarth::ImageLayer*>(mOsgEarthMapNode->getMap()->getLayerByName(aIdentifier));
      if (nullptr != layer)
      {
         layer->setOpacity(aValue);
      }
   }
}

UtoCmeTerrainImp::SensorPlotColorFilter::SensorPlotColorFilter(float aMin, float aMax, const std::map<float, UtColor>& aGradient)
   : osgEarth::ColorFilter()
   , mMin(aMin)
   , mMax(aMax)
   , mGradient(aGradient)
{
}

void UtoCmeTerrainImp::SensorPlotColorFilter::install(osg::StateSet* aStateSet) const
{
   std::string fs = "uniform sampler1D gradient; \n"
                    "uniform vec2 minmax; \n"
                    "void SensorPlotColorFilter(inout vec4 color) \n"
                    "{ \n"
                    "   float v = (color.r - minmax.x) / (minmax.y - minmax.x); \n"
                    "   color = texture1D(gradient, v); \n"
                    "} \n";

   // getOrCreate ensures we don't overwrite an existing VP.
   osgEarth::VirtualProgram* vp = osgEarth::VirtualProgram::getOrCreate(aStateSet);
   vp->setShader("SensorPlotColorFilter", new osg::Shader(osg::Shader::FRAGMENT, fs));
   aStateSet->getOrCreateUniform("minmax", osg::Uniform::FLOAT_VEC2)->set(osg::Vec2(mMin, mMax));

   unsigned int samplerIndex = ut::osg::GetAvailableTextureUnit(*aStateSet);

   unsigned char* datamem = new unsigned char[cGRADIENT_TEXTURE_SIZE * 4]; // 4 components per texel
   auto           low     = mGradient.begin();
   auto           hi      = low;
   ++hi;
   for (unsigned int i = 0; i < cGRADIENT_TEXTURE_SIZE; ++i)
   {
      float factor = static_cast<float>(i) / static_cast<float>(cGRADIENT_TEXTURE_SIZE);
      if ((factor < low->first) || (hi == mGradient.end()))
      {
         datamem[i * 4]     = low->second[0];
         datamem[i * 4 + 1] = low->second[1];
         datamem[i * 4 + 2] = low->second[2];
         datamem[i * 4 + 3] = low->second[3];
      }
      else
      {
         if (factor > hi->first)
         {
            low++;
            hi++;
         }
         if (hi == mGradient.end())
         {
            datamem[i * 4]     = low->second[0];
            datamem[i * 4 + 1] = low->second[1];
            datamem[i * 4 + 2] = low->second[2];
            datamem[i * 4 + 3] = low->second[3];
         }
         else
         {
            float lowFactor    = (hi->first - factor) / (hi->first - low->first);
            datamem[i * 4]     = (low->second[0] * lowFactor + hi->second[0] * (1.0f - lowFactor));
            datamem[i * 4 + 1] = (low->second[1] * lowFactor + hi->second[1] * (1.0f - lowFactor));
            datamem[i * 4 + 2] = (low->second[2] * lowFactor + hi->second[2] * (1.0f - lowFactor));
            datamem[i * 4 + 3] = (low->second[3] * lowFactor + hi->second[3] * (1.0f - lowFactor));
         }
      }
   } // build texture

   osg::ref_ptr<osg::Image> gimage = new osg::Image();
   gimage->setImage(cGRADIENT_TEXTURE_SIZE, 1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, datamem, osg::Image::USE_NEW_DELETE);
   osg::ref_ptr<osg::Texture1D> gtexture = new osg::Texture1D();
   gtexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
   gtexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
   gtexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
   gtexture->setImage(gimage.get());

   aStateSet->setTextureAttributeAndModes(samplerIndex, gtexture.get(), osg::StateAttribute::ON);

   aStateSet->getOrCreateUniform("gradient", osg::Uniform::SAMPLER_1D)->set((int)samplerIndex);
}

osgEarth::Features::FilterContext SimpleAttributeFilter::push(osgEarth::Features::FeatureList&   aInput,
                                                              osgEarth::Features::FilterContext& aContext)
{
   for (auto i : aInput)
   {
      osgEarth::Features::Feature* feature = i.get();
      float                        width   = 1.0F;
      osgEarth::Color              color(1.0F, 1.0F, 1.0F);
      const auto&                  attrs = feature->getAttrs();
      auto                         wattr = attrs.find("WSFWIDTH");
      if (wattr != attrs.end())
      {
         if (wattr->second.first == osgEarth::Features::AttributeType::ATTRTYPE_STRING)
         {
            width = stof(wattr->second.getString());
         }
      }
      auto cattr = attrs.find("WSFCOLOR");
      if (cattr != attrs.end())
      {
         if (cattr->second.first == osgEarth::Features::AttributeType::ATTRTYPE_STRING)
         {
            UtColor c(cattr->second.getString());
            c.Get(color.r(), color.g(), color.b(), color.a());
         }
      }

      osgEarth::LineSymbol* ls = feature->style()->getOrCreateSymbol<osgEarth::LineSymbol>();
      ls->stroke()->color()    = color;
      ls->stroke()->width()    = width;
   }
   return aContext;
}
