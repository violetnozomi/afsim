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

// UtoCmeTerrain.hpp: interface for the UtoCmeTerrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UTOCMETERRAIN_HPP_)
#define _UTOCMETERRAIN_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "utilosg_export.h"

#include <osg/Matrix>

#include "UtColor.hpp"
#include "UtoTerrain.hpp"
#include "UtoTypes.hpp"


class UtoTerrainImp;
class UtoCmeTerrainImp;

class UTILOSG_EXPORT UtoCmeTerrain : public UtoTerrain
{
public:
   // constructor / deconstructor
   UtoCmeTerrain();
   UtoCmeTerrain(const UtoCmeTerrain&);
   ~UtoCmeTerrain() override;

   std::string Name() override;
   void        SetName(const std::string& name) override;

   void               Dump(std::ostream&) override;
   friend UtoTerrain* CreateCmeTerrain();

   // sets the color blending value
   void SetColor(const UtoColor& rColor, unsigned int nClrMask = UtoColor::RGBA) override;
   // returns the current color blending value
   const UtoColor& Color() const override;

   // returns the height of the terrain
   void Height(double* rPosn) const override;
   void Height(double* rPosn, UtoOrientation& rOrient) const override;

   bool Intersect(const double aPos[3],
                  const double aVec[3],
                  double       aIPoint[3],
                  double       aINorm[3],
                  double       aMagnitude = 100000.0) const override;

   // returns the terrain intersection geometry
   bool Intersect(osgUtil::IntersectionVisitor* aIsectVisitor) const override;

   // enables / disables the color blending
   void SetDisableColorBlending(bool bDisable) override;
   // obtains the state of color blending
   bool IsColorBlendingDisabled() const override;

   void SetUniform(const std::string& aName, const float aValue) override;
   void SetUniform(const std::string& aName, const float aValue[4]) override;
   void SetUniformMatrix(const std::string& aName, const double aValue[][4]) override;

   bool Lighting() override;
   void EnableLighting(const bool aLit) override;

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

   void UpdateProjectorMatrix(unsigned int aProjectorId, const osg::Matrix& aViewMatrix, const osg::Matrix& aProjectionMatrix);

   bool AddSensorPlotImageLayer(const std::string&              aFilename,
                                const std::string&              aIdentifier,
                                double                          aMinValue,
                                double                          aMaxValue,
                                unsigned int                    aDataSet,
                                const std::map<float, UtColor>& aGradient);

   void SetSensorPlotLayerGradient(const std::string&              aIdentifier,
                                   double                          aMinValue,
                                   double                          aMaxValue,
                                   const std::map<float, UtColor>& aGradient);
   void SetImageLayerOpacity(const std::string& aIdentifier, float aValue);

private:
   UtoCmeTerrain& operator=(const UtoCmeTerrain&);

   UtoTerrainImp*       Impl() override;
   const UtoTerrainImp* Impl() const override;
   UtoTerrain*          Clone() const override;

   UtoCmeTerrainImp* m_pImpRoot;
};

#endif // !defined(_UTOCMETERRAIN_HPP_)
