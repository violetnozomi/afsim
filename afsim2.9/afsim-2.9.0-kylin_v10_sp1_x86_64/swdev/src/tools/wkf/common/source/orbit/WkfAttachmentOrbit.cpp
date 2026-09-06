// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAttachmentOrbit.hpp"

#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include "UtMath.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaEphemeris.hpp"
#include "VaViewer.hpp"
#include "WkfObserver.hpp"

osg::Program* wkf::AttachmentOrbit::sOsgProgram = nullptr;

wkf::AttachmentOrbit::AttachmentOrbit(vespa::VaEntity& aEntity, vespa::VaViewer* aViewerPtr, bool aCelestial)
   : vespa::VaAttachment(aEntity, aViewerPtr, "orbit")
   , mActiveShape(OrbitData(), nullptr)
   , mActiveShapeName("")
   , mFadeout(600)
   , mLineWidth(1)
   , mColor(1.0f, 1.0f, 1.0f, 1.0f)
   , mCelestial(aCelestial)
{
   SetType<AttachmentOrbit>();

   mCallbacks.Add(wkf::Observer::MapProjectionChanged.Connect(&AttachmentOrbit::MapProjectionChanged, this));
   mCallbacks.Add(wkf::Observer::ECI_ModeChanged.Connect(&AttachmentOrbit::ECI_ModeChanged, this));
}

wkf::AttachmentOrbit::~AttachmentOrbit()
{
   HandleCelestialCamera(false);
}

void wkf::AttachmentOrbit::Clear()
{
   for (auto& it : mOldShapes)
   {
      RemoveShapeFromParent(it.first);
   }
   mOldShapes.clear();
   if (mActiveShape.second)
   {
      HandleCelestialCamera(false);
      RemoveShapeFromParent(mActiveShapeName);
      mActiveShapeName    = "";
      mActiveShape.second = nullptr;
   }
}

void wkf::AttachmentOrbit::Add(double aSimTime,
                               double aSemiMajor,
                               double aEccentricity,
                               double aRAAN,
                               double aInclination,
                               double aArgumentOfPeriapsis,
                               double aTrueAnomaly,
                               bool   aManeuver)
{
   std::string name = "orbit";
   name += std::to_string(aSimTime) + std::to_string(aSemiMajor) + std::to_string(aEccentricity) + std::to_string(aRAAN) +
           std::to_string(aInclination) + std::to_string(aArgumentOfPeriapsis) + GetUniqueIdString();

   if (mActiveShape.second && name == mActiveShapeName)
   {
      return;
   }
   if (mOldShapes.find(name) != mOldShapes.end())
   {
      return;
   }

   if (mActiveShape.second)
   {
      HandleCelestialCamera(false);
      mActiveShape.second->OverrideRenderBin(2, false);
      for (auto& it : mOldShapes)
      {
         it.second.second->OverrideRenderBin(3, false);
      }
      double fadeTime = aSimTime; // we use simtime, because we want the fade to start with the new orbit
      if (!aManeuver)             // but if this is just an orbit update, drop it immediately
      {
         fadeTime -= 2.0 * mFadeout;
      }
      mActiveShape.first.mFadeout  = fadeTime;
      mOldShapes[mActiveShapeName] = mActiveShape;
   }

   mActiveShape.first.mSimTime             = aSimTime;
   mActiveShape.first.mSemiMajor           = aSemiMajor;
   mActiveShape.first.mEccentricity        = aEccentricity;
   mActiveShape.first.mRAAN                = aRAAN;
   mActiveShape.first.mInclination         = aInclination;
   mActiveShape.first.mArgumentOfPeriapsis = aArgumentOfPeriapsis;
   mActiveShape.first.mTrueAnomaly         = aTrueAnomaly;
   mActiveShape.first.mManeuver            = aManeuver;
   mActiveShapeName                        = name;
   Rebuild(mActiveShapeName, mActiveShape);
}

void wkf::AttachmentOrbit::UpdateTimeAngle(double aTimeAng, double aSimTime)
{
   std::set<std::string> remset;
   for (auto& it : mOldShapes)
   {
      double age = aSimTime - it.second.first.mFadeout;
      it.second.second->GetTree()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("uTimeAng", osg::Uniform::FLOAT)
         ->set((float)(-aTimeAng));
      float fade = 1.0 - age / (float)mFadeout;
      if (fade < 0.0)
      {
         remset.insert(it.first);
      }
      it.second.second->GetTree()->getOrCreateStateSet()->getOrCreateUniform("uFade", osg::Uniform::FLOAT)->set((float)(fade));
   }
   for (auto& jt : remset)
   {
      RemoveShapeFromParent(jt);
      mOldShapes.erase(jt);
   }
   if (mActiveShape.second)
   {
      mActiveShape.second->GetTree()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("uTimeAng", osg::Uniform::FLOAT)
         ->set((float)(-aTimeAng));
      mActiveShape.second->GetTree()->getOrCreateStateSet()->getOrCreateUniform("uFade", osg::Uniform::FLOAT)->set((float)(1.0));
   }
}

void wkf::AttachmentOrbit::SetFadeout(int aFadeout)
{
   mFadeout = aFadeout;
}

void wkf::AttachmentOrbit::SetLineWidth(int aLinewidth)
{
   mLineWidth = aLinewidth;
   for (auto& it : mOldShapes)
   {
      it.second.second->GetTree()->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(aLinewidth));
   }
   if (mActiveShape.second)
   {
      mActiveShape.second->GetTree()->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(aLinewidth));
   }
}

void wkf::AttachmentOrbit::SetColor(const UtColor& aColor)
{
   mColor = aColor;
   for (auto& it : mOldShapes)
   {
      it.second.second->GetTree()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("uColor", osg::Uniform::FLOAT_VEC3)
         ->set(osg::Vec3(mColor[0], mColor[1], mColor[2]));
   }
   if (mActiveShape.second)
   {
      mActiveShape.second->GetTree()
         ->getOrCreateStateSet()
         ->getOrCreateUniform("uColor", osg::Uniform::FLOAT_VEC3)
         ->set(osg::Vec3(mColor[0], mColor[1], mColor[2]));
   }
}

void wkf::AttachmentOrbit::HandleCelestialCamera(bool aShow)
{
   auto celestNode = mViewerPtr->GetCelestialNode();
   if (mCelestial && celestNode && mActiveShape.second && mActiveShape.second->GetTree())
   {
      if (aShow)
      {
         // prevent multiple additions to the node
         if (celestNode->getChildIndex(mActiveShape.second->GetTree()) == celestNode->getNumChildren())
         {
            celestNode->addChild(mActiveShape.second->GetTree());
         }
      }
      else
      {
         celestNode->removeChild(mActiveShape.second->GetTree());
      }
   }
}

void wkf::AttachmentOrbit::ECI_ModeChanged(bool aState)
{
   for (auto& it : mOldShapes)
   {
      Rebuild(it.first, it.second);
   }
   if (mActiveShape.second)
   {
      Rebuild(mActiveShapeName, mActiveShape);
   }
}

void wkf::AttachmentOrbit::MapProjectionChanged(unsigned int aViewerIndex, const UtoMapProjection* aProjection)
{
   for (auto& it : mOldShapes)
   {
      Rebuild(it.first, it.second);
   }
   if (mActiveShape.second)
   {
      Rebuild(mActiveShapeName, mActiveShape);
   }
}

void wkf::AttachmentOrbit::SetPeriods(unsigned int aPeriods)
{
   mPeriods = aPeriods;
   for (auto& it : mOldShapes)
   {
      Rebuild(it.first, it.second);
   }
   if (mActiveShape.second)
   {
      Rebuild(mActiveShapeName, mActiveShape);
   }
}

void wkf::AttachmentOrbit::Rebuild(const std::string& aName, ShapeData& aData)
{
   osg::ref_ptr<osg::StateSet> ss;
   if (aData.second)
   {
      ss = aData.second->GetTree()->getOrCreateStateSet();
      RemoveShapeFromParent(aName);
      aData.second = nullptr;
   }

   UtoRawShape raw;
   raw.SetReferenceFrame(UtoShape::cFRAME_WORLD);
   aData.second = AddShapeToParent(aName, raw);
   aData.second->OverrideRenderBin(1, false);
   aData.second->SetName(aName);

   osg::Geode* geode = new osg::Geode;
   if (ss.valid())
   {
      aData.second->GetTree()->setStateSet(ss);
   }
   aData.second->GetTree()->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(mLineWidth));
   aData.second->GetTree()
      ->getOrCreateStateSet()
      ->getOrCreateUniform("uColor", osg::Uniform::FLOAT_VEC3)
      ->set(osg::Vec3(mColor[0], mColor[1], mColor[2]));
   aData.second->GetTree()->addChild(geode);
   osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
   geom->setCullingActive(false); // using shaders!
   geode->addDrawable(geom);

   osg::Vec3Array* v              = new osg::Vec3Array;
   const double    sineFactor     = std::sqrt(1 - aData.first.mEccentricity * aData.first.mEccentricity);
   const double    semiMajorAxis3 = aData.first.mSemiMajor * aData.first.mSemiMajor * aData.first.mSemiMajor;
   const double    avgAngularVel  = std::sqrt(semiMajorAxis3 / UtEarth::cGRAVITATIONAL_PARAMETER);

   const double sinTerm0 = sineFactor * std::sin(aData.first.mTrueAnomaly);
   const double cosTerm0 = aData.first.mEccentricity + std::cos(aData.first.mTrueAnomaly);
   double       E0       = std::atan2(sinTerm0, cosTerm0);
   if (aData.first.mTrueAnomaly > UtMath::cPI)
   {
      E0 += UtMath::cTWO_PI;
   }
   const double M0 = E0 - aData.first.mEccentricity * std::sin(E0);

   constexpr unsigned int samples = 2000;

   unsigned int periods = GetViewer()->UseECI() ? 1 : mPeriods;
   double       lastE   = 0.0;
   unsigned int loops   = 0;
   for (unsigned int i = 0; i < samples * periods; ++i)
   {
      const double theta = aData.first.mTrueAnomaly - UtMath::cPI + UtMath::cTWO_PI * (i / static_cast<double>(samples));
      const double sinTerm = sineFactor * std::sin(theta);
      const double cosTerm = aData.first.mEccentricity + std::cos(theta);
      double       E       = std::atan2(sinTerm, cosTerm);
      if ((lastE) > 0 && (E < 0)) // if we have looped over
      {
         loops++;
      }
      lastE = E;

      E += loops * UtMath::cTWO_PI;

      const double M = E - aData.first.mEccentricity * std::sin(E);

      const double t = aData.first.mSimTime + avgAngularVel * (M - M0);

      UtCalendar curTime = vespa::VaEphemeris::GetStartTime();
      curTime.AdvanceTimeBy(t);

      v->push_back(osg::Vec3(static_cast<float>(theta), static_cast<float>(-curTime.GetEarthAngle()), 0));
   }
   geom->setVertexArray(v);
   geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, samples * periods));
   geom->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
   geom->getOrCreateStateSet()->addUniform(
      new osg::Uniform("u2D_Param", osg::Vec2((float)aData.first.mSemiMajor, (float)aData.first.mEccentricity)));
   geom->getOrCreateStateSet()->addUniform(new osg::Uniform(
      "uRotations",
      osg::Vec3((float)aData.first.mRAAN, (float)aData.first.mInclination, (float)aData.first.mArgumentOfPeriapsis)));

   if (!sOsgProgram)
   {
      std::set<std::string> defset;
      sOsgProgram =
         UtoShaders::UseProgramWithGeometry("orbitEllipse.geom", "orbitEllipse.vert", "orbitEllipse.frag", defset);
   }
   if (sOsgProgram)
   {
      aData.second->SetProgram(sOsgProgram, true);
   }
   HandleCelestialCamera(true);
}
