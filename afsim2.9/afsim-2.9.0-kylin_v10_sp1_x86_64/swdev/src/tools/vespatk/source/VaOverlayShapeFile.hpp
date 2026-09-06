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

#ifndef VAOVERLAYSHAPEFILE_HPP
#define VAOVERLAYSHAPEFILE_HPP

#include "vespatk_export.h"

#include <osg/Geometry>

#include "GeoShapeFile.hpp"
#include "UtColor.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

class UtoMapProjection;
class UtoRawShape;

namespace vespa
{

class VESPATK_EXPORT VaOverlayShapeFile : public VaOverlay
{
public:
   VaOverlayShapeFile();

   VaOverlayShapeFile(const std::string& aName);

   VaOverlayShapeFile* Clone() const override;

   ~VaOverlayShapeFile() override;

   bool Initialize() override;

   void LoadFile(const std::string& aFilename);

   virtual UtColor GetColor();
   virtual void    SetColor(const float* aColor);
   void            SetColor(const UtColor& aColor);

   virtual void SetWidth(const float& aWidth);


   void MapProjectionChangedCB(unsigned int aViewer, const UtoMapProjection* aProjection);

private:
   void VTransform(UtoPosition& aOutput, const GeoShapeFile::Position& aInput, const double aTransform[3], bool aGeocentric);

   VaCallbackHolder mCallbacks;

   UtoRawShape*             mShapePtr;
   std::string              mShapeName;
   osg::ref_ptr<osg::Geode> mGeode;

   std::string mFilename;
   float       mColor[3];
   float       mWidth;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayShapeFile)
#endif
