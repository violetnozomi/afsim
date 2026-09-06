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

#include "UtoEphemerisNode.hpp"

#include <osg/Geode>

#include "UtEntity.hpp"
#include "UtPath.hpp"
#include "UtoEphemerisAtmosphere.hpp"
#include "UtoMemory.hpp"
#include "UtoShaders.hpp"

Uto::EphemerisNode::EphemerisNode(const std::string& aResourcePath)
{
   getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
   BuildSun();
   BuildMoon(aResourcePath);
   BuildStars(aResourcePath);
   BuildAtmosphere();
}

void Uto::EphemerisNode::SetTime(const UtCalendar& aTime)
{
   mMoon->SetTime(aTime);
   mSun->SetTime(aTime);
   mStars->SetTime(aTime);
}

osg::Vec3 Uto::EphemerisNode::GetSunVector()
{
   return mSun->GetVector();
}

void Uto::EphemerisNode::BuildSun()
{
   osg::ref_ptr<osg::Geode> geode = ut::osg::make_ref<osg::Geode>();
   geode->setName("sun");
   mSun = ut::osg::make_ref<UtoEphemerisSun>();
   geode->addDrawable(mSun.get());
   geode->setCullingActive(false);
   geode->getOrCreateStateSet()->setRenderBinDetails(-5, "DepthSortedBin");
   addChild(geode.get());
}

void Uto::EphemerisNode::BuildMoon(const std::string& aResourceDir)
{
   mMoon = ut::osg::make_ref<Uto::EphemerisMoon>(true, aResourceDir);
   addChild(mMoon);
}

void Uto::EphemerisNode::BuildStars(const std::string& aResourceDir)
{
   osg::ref_ptr<osg::Geode> geode = ut::osg::make_ref<osg::Geode>();
   geode->setName("stars");
   std::string catalogPath = aResourceDir + "/starcatalog";
   mStars                  = ut::osg::make_ref<UtoEphemerisStars>(catalogPath.c_str());
   geode->addDrawable(mStars.get());
   geode->setCullingActive(false);
   geode->getOrCreateStateSet()->setRenderBinDetails(-6, "DepthSortedBin");
   addChild(geode.get());
}

void Uto::EphemerisNode::BuildAtmosphere()
{
   osg::ref_ptr<osg::Geode> geode = ut::osg::make_ref<osg::Geode>();
   geode->setName("atmosphere");
   geode->addDrawable(new UtoEphemerisAtmosphere());
   geode->setCullingActive(false);
   geode->getOrCreateStateSet()->setRenderBinDetails(-2, "DepthSortedBin");
   addChild(geode.get());
}
