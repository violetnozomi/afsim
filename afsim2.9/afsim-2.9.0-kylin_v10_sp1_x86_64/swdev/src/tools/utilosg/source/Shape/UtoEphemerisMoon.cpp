// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoEphemerisMoon.hpp"

#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Geode>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtoMemory.hpp"
#include "UtoShaders.hpp"

Uto::EphemerisMoon::EphemerisMoon(bool aForCelestialSphere, const std::string& aResourcePath)
   : osg::PositionAttitudeTransform()
{
   if (aForCelestialSphere)
   {
      setName("moon_celestial");

      // star/sun block -4
      auto           moonBlock = ut::osg::make_ref<osg::Geode>();
      osg::StateSet* mbss      = moonBlock->getOrCreateStateSet();
      moonBlock->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), UtMoon::cMEAN_RADIUS)));
      mbss->setRenderBinDetails(-3, "DepthSortedBin");
      osg::ref_ptr<osg::Material> material = ut::osg::make_ref<osg::Material>();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.02F, 0.02F, 0.02F, 1.0F));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));
      mbss->setAttributeAndModes(material);

      addChild(moonBlock);

      // moon light -2
      auto           moonDraw = ut::osg::make_ref<osg::Geode>();
      osg::StateSet* mss      = moonDraw->getOrCreateStateSet();
      mss->setAttributeAndModes(UtoShaders::UseProgram("ephemerisMoon.vert", "ephemerisMoon.frag"));
      mss->setMode(GL_BLEND, osg::StateAttribute::ON);
      mss->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
      osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(GL_ONE, GL_ONE);
      mss->setAttributeAndModes(bf.get(), osg::StateAttribute::ON);
      setCullingActive(false);
      moonDraw->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), UtMoon::cMEAN_RADIUS)));
      mss->setRenderBinDetails(-2, "DepthSortedBin");
      osg::Image*   image   = osgDB::readImageFile(aResourcePath + "/moon.png");
      osg::Texture* texture = new osg::Texture2D();
      texture->setImage(0, image);
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      mss->setTextureAttributeAndModes(0, texture);
      addChild(moonDraw);
   }
   else
   {
      setName("moon");
      auto           moonDraw = ut::osg::make_ref<osg::Geode>();
      osg::StateSet* mss      = moonDraw->getOrCreateStateSet();
      mss->setAttributeAndModes(UtoShaders::UseProgram("ephemerisMoon.vert", "ephemerisMoon.frag"));
      mss->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
      setCullingActive(false);
      moonDraw->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), UtMoon::cMEAN_RADIUS)));
      osg::Image*   image   = osgDB::readImageFile(aResourcePath + "/moon.png");
      osg::Texture* texture = new osg::Texture2D();
      texture->setImage(0, image);
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      mss->setTextureAttributeAndModes(0, texture);
      addChild(moonDraw);
   }
}

void Uto::EphemerisMoon::SetTime(const UtCalendar& aTime)
{
   double eci[3];
   UtMoon::GetLocationECI(aTime, eci);
   UtEntity m;
   m.SetECI_ReferenceEpoch(aTime);
   m.SetLocationECI(eci);
   double wcs[3];
   m.GetLocationWCS(wcs);
   setPosition(osg::Vec3(wcs[0], wcs[1], wcs[2]));
   setAttitude(osg::Quat(UtMath::cPI_OVER_2 - atan2(wcs[0], wcs[1]), osg::Vec3(0.0F, 0.0F, 1.0F)));
}
