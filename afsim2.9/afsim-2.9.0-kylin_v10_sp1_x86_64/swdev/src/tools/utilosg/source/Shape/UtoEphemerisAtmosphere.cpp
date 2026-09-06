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

#include "UtoEphemerisAtmosphere.hpp"

#include <iostream>

#include <UtEarth.hpp>
#include <UtMath.hpp>
#include <osg/PolygonMode>
#include <osg/ShadeModel>
#include <osg/Texture2D>
#include <osgDB/WriteFile>

#include "UtoShaders.hpp"

// static members
float UtoEphemerisAtmosphere::mInSpaceFactor = 1.0f;
float UtoEphemerisAtmosphere::mLightColor[3] = {1.0f, 1.0f, 1.0f};

UtoEphemerisAtmosphere::UtoEphemerisAtmosphere()
   : mDraw(true)
{
   setCullingActive(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-2, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   osg::ShadeModel* shadeModel = new osg::ShadeModel(); // defaults to SMOOTH
   stateSet->setAttributeAndModes(shadeModel);
   stateSet->setAttributeAndModes(UtoShaders::UseProgram("ephemAtmos.vert", "ephemAtmos.frag"));
   //   osg::PolygonMode* polymode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
   //   stateSet->setAttributeAndModes(polymode);
   BuildShaderGeometry();
}

UtoEphemerisAtmosphere::UtoEphemerisAtmosphere(const UtoEphemerisAtmosphere& drawer, const osg::CopyOp& copyop)
   : osg::Geometry(drawer, copyop)
   , mDraw(true)
{
   setCullingActive(false);
   osg::StateSet* stateSet = getOrCreateStateSet();
   stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateSet->setRenderBinDetails(-2, "RenderBin");
   stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
   stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   stateSet->setAttributeAndModes(UtoShaders::UseProgram("ephemAtmos.vert", "ephemAtmos.frag"));

   BuildShaderGeometry();
}

void UtoEphemerisAtmosphere::BuildShaderGeometry()
{
   osg::Vec3Array* vertArray = new osg::Vec3Array;
   setVertexArray(vertArray);

   int i;
   for (i = 0; i < 90; ++i)
   {
      float s1 = sin(i * 4 * UtMath::cRAD_PER_DEG + 0.035);
      float c1 = cos(i * 4 * UtMath::cRAD_PER_DEG + 0.035);
      vertArray->push_back(osg::Vec3(s1, c1, 0.0));
      vertArray->push_back(osg::Vec3(s1, c1, 1.0));
   }
   float s1 = sin(0.035);
   float c1 = cos(0.035);
   vertArray->push_back(osg::Vec3(s1, c1, 0.0));
   vertArray->push_back(osg::Vec3(s1, c1, 1.0));
   addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, 182));
}

void UtoEphemerisAtmosphere::GetLightColor(float& aRed, float& aGreen, float& aBlue) const
{
   aRed   = mLightColor[0] * (1 - mInSpaceFactor) + mInSpaceFactor;
   aGreen = mLightColor[1] * (1 - mInSpaceFactor) + mInSpaceFactor;
   aBlue  = mLightColor[2] * (1 - mInSpaceFactor) + mInSpaceFactor;
}

void UtoEphemerisAtmosphere::EnableVisualization(const bool aState)
{
   if (mDraw && !aState)
   {
      removePrimitiveSet(0, getNumPrimitiveSets());
   }
   else if (!mDraw && aState)
   {
      BuildShaderGeometry();
   }
   mDraw = aState;
}
