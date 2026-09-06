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

// UtoCmeTerrainImp.hpp: interface for the UtoCmeTerrainImp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UTOCMETERRAINIMP_HPP_)
#define _UTOCMETERRAINIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include <osg/Texture2D>
#include <osg/ref_ptr>
#include <osgEarth/ColorFilter>
#include <osgEarth/VirtualProgram>
#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/Filter>

#include "UtColor.hpp"
#include "UtoTerrainImp.hpp"
#include "UtoTypes.hpp"

// public forward references
namespace osg
{
class Node;
}
namespace osgEarth
{
class MapNode;
}
namespace osgUtil
{
class IntersectionVisitor;
}

class UtoTerrainUpdateMarkNodeVisitor;

class SimpleAttributeFilterOptions : public osgEarth::Features::ConfigOptions
{
public:
   SimpleAttributeFilterOptions(const osgEarth::Config& aConf)
      : ConfigOptions(aConf)
   {
   }
};
class SimpleAttributeFilter : public osgEarth::Features::FeatureFilter
{
public:
   SimpleAttributeFilter(const osgEarth::Features::Config& aConf) {}
   virtual ~SimpleAttributeFilter() {}

   osgEarth::Features::FilterContext push(osgEarth::Features::FeatureList&   aInput,
                                          osgEarth::Features::FilterContext& aContext) override;
};

class UtoCmeTerrainImp : public UtoTerrainImp
{
public:
   // constructor / deconstructor
   UtoCmeTerrainImp();
   UtoCmeTerrainImp(const UtoCmeTerrainImp&);
   ~UtoCmeTerrainImp() override;

   void       Dump(std::ostream& o) override;
   osg::Node* AddToWorld(UtoViewer& viewer) override;
   void       RemoveFromWorld(UtoViewer& Viewer) override;

   // sets the color blending value
   void SetColor(const UtoColor& rColor, unsigned int nClrMask) override;

   // returns the height of the terrain
   void Height(double* rPosn, UtoOrientation* pOrient = nullptr) override;

   // returns the terrain intersection point

   virtual bool Intersect(const double aPos[3],
                          const double aVec[3],
                          double       aIPoint[3],
                          double       aINorm[3],
                          double       aMagnitude = 100000.0) const;

   virtual bool Intersect(osgUtil::IntersectionVisitor* aIsectVisitor) const;

   // enables / disable color blending
   void DisableColorBlending(bool bDisable) override;

   void SetUniform(const std::string& name, const float value) override
   {
      UtoTerrainImp::SetUniform(m_RootNode.get(), name, value);
   }

   void SetUniform(const std::string& name, const float value[4]) override
   {
      UtoTerrainImp::SetUniform(m_RootNode.get(), name, value);
   }

   void SetUniformMatrix(const std::string& name, const double value[][4]) override
   {
      UtoTerrainImp::SetUniformMatrix(m_RootNode.get(), name, value);
   }

   void SetUniformInverseMatrix(const std::string& name, const double value[][4]) override
   {
      UtoTerrainImp::SetUniformInverseMatrix(m_RootNode.get(), name, value);
   }

   void EnableLighting(const bool aLit);
   bool Lighting();
   void SetFiles(const std::string& aFiles);

   bool SupportsLayers() const;
   bool AddShapeLayer(const std::string& aFilename);
   bool AddShapeLayer(const std::string& aFilename, float aRed, float aGreen, float aBlue);
   void RemoveLayer(const std::string& aFilename);
   void ShowLayer(const std::string& aFilename, bool aState);
   void SetShapeLayerColor(const std::string& aFilename, float aRed, float aGreen, float aBlue);
   bool AddImageLayer(const std::string& aFilename);
   bool AddImageLayer(const std::string& aFilename, double aMinLat, double aMinLon, double aMaxLat, double aMaxLon);

   unsigned int AddProjector(const std::string& aName, const UtColor& aColor);
   void         RemoveProjector(unsigned int aId);
   void UpdateProjectorMatrix(unsigned int aId, const osg::Matrix& aViewMatrix, const osg::Matrix& aProjectionMatrix);

   bool AddSensorPlotImageLayer(const std::string&              aFilename,
                                const std::string&              aIdentifier,
                                double                          aMinValue,
                                double                          aMaxValue,
                                unsigned int                    aDataSet,
                                const std::map<float, UtColor>& aGradient);
   void SetSensorPlotLayerGradient(const std::string               aIdentifier,
                                   double                          aMinValue,
                                   double                          aMaxValue,
                                   const std::map<float, UtColor>& aGradient);
   void SetImageLayerOpacity(const std::string& aIdentifier, float aValue);

private:
   void LoadVirtualPrograms();

   // private typedef
   typedef void (UtoCmeTerrainImp::*ColorTransFuncPtr)(void);

   // do not allow terrain imps to be copied
   UtoCmeTerrainImp& operator=(const UtoCmeTerrainImp&);

   void InitMap();
   bool PostLoad() override;
   bool ReadFile(const std::string& attr, const std::string& buf);

   // enables terrain coloring
   void InitializeTerrainBlending(osg::Node* pNode);

   // sets the color and transparency for either the shader or the node visitor
   void SetShaderColorTransparency();
   void SetVisitorColorTransparency();

   osg::ref_ptr<osg::Texture2D> CreateProjectorRenderTarget();
   osg::ref_ptr<osg::Camera>    CreateProjectorRenderTargetCamera(osg::Texture2D& aTexture);

   // private member variables
   std::string                                   m_Path;
   ColorTransFuncPtr                             m_pSetColorTransparency;
   osg::ref_ptr<osg::Node>                       m_RootNode;
   osg::ref_ptr<UtoTerrainUpdateMarkNodeVisitor> m_pUpdateVisitor;
   static Utok::Properties_T<UtoCmeTerrainImp>   m_Properties;

   bool mLit;

   osgEarth::MapNode* mOsgEarthMapNode;

   const static unsigned int cGRADIENT_TEXTURE_SIZE = 256;

   class SensorPlotColorFilter : public osgEarth::ColorFilter
   {
   public:
      SensorPlotColorFilter(float aMin, float aMax, const std::map<float, UtColor>& aGradient);
      ~SensorPlotColorFilter() override = default;
      void        install(osg::StateSet* stateSet) const override;
      std::string getEntryPointFunctionName() const override { return "SensorPlotColorFilter"; }

   private:
      float                    mMin;
      float                    mMax;
      std::map<float, UtColor> mGradient;
   };

   std::map<unsigned int, osg::ref_ptr<osg::Camera>> mProjectorMap;
};

#endif // !defined(_UTOCMETERRAINIMP_HPP_)
