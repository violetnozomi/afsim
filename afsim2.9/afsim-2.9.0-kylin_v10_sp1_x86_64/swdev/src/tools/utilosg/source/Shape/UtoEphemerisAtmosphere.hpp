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

#ifndef _UTOEPHEMERISATMOSPHERE_HPP_
#define _UTOEPHEMERISATMOSPHERE_HPP_

#include "utilosg_export.h"

#include <vector>

#include <osg/Geometry>

class UTILOSG_EXPORT UtoEphemerisAtmosphere : public osg::Geometry
{
public:
   UtoEphemerisAtmosphere();

   /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
   UtoEphemerisAtmosphere(const UtoEphemerisAtmosphere& drawer, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

   UtoEphemerisAtmosphere& operator=(const UtoEphemerisAtmosphere& aRHS) = delete;


   META_Object(StealthView, UtoEphemerisAtmosphere)

      virtual void BuildShaderGeometry();

   // the draw immediate mode method is where the OSG wraps up the drawing of
   // of OpenGL primitives.

   void  EnableVisualization(const bool aState);
   float GetSpaceFactor() const { return mInSpaceFactor; }
   void  GetLightColor(float& aRed, float& aGreen, float& aBlue) const;

protected:
   ~UtoEphemerisAtmosphere() override = default;

private:
   bool mDraw;

   static float mInSpaceFactor; // normalized value [0,1] 0 - "sea level", 1 - in space
   static float mLightColor[3];

public:
};

#endif
