// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfOverlaySpace.hpp"

#include <QColor>
#include <QIcon>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/PolygonOffset>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"
#include "UtoMemory.hpp"
#include "VaEphemeris.hpp"
#include "VaObserver.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
const QString cDEFAULT_SUN_PATH  = ":/icons/sun.png";
const QString cDEFAULT_MOON_PATH = ":/icons/moon.png";
} // namespace

wkf::OverlaySpace::OverlaySpace()
   : vespa::VaOverlay("space_overlay", cOVERLAY_WORLD)
{
}

wkf::OverlaySpace::OverlaySpace(const std::string& aName)
   : VaOverlay(aName, cOVERLAY_WORLD)
{
}

wkf::OverlaySpace::~OverlaySpace()
{
   RemoveTerminator();
   RemoveSolarPath();
}

vespa::VaOverlay* wkf::OverlaySpace::Clone() const
{
   return new OverlaySpace();
}

bool wkf::OverlaySpace::Initialize()
{
   mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&OverlaySpace::MapProjectionChangedCB, this));

   return true;
}

void wkf::OverlaySpace::ShowTerminator(bool aState)
{
   mShowTerminator = aState;
   if (aState)
   {
      mTerminatorUpdateEpoch = vespa::VaEphemeris::GetSimTime();
      RebuildTerminator();
   }
   else
   {
      RemoveTerminator();
   }
}

void wkf::OverlaySpace::ShowSolarPath(bool aState)
{
   mShowSolarPath = aState;
   if (aState)
   {
      mSolarPathUpdateEpoch = vespa::VaEphemeris::GetSimTime();
      RebuildSolarPath();
   }
   else
   {
      RemoveSolarPath();
   }
}

void wkf::OverlaySpace::ShowSubSolarPoint(bool aState)
{
   ShowSubPoint(mSubSolarPoint, aState);
}

void wkf::OverlaySpace::ShowSubLunarPoint(bool aState)
{
   ShowSubPoint(mSubLunarPoint, aState);
}

void wkf::OverlaySpace::RebuildTerminator()
{
   if (!mTerminatorShapePtr)
   {
      UtoRawShape shapePtr;
      mTerminatorShapePtr = dynamic_cast<UtoRawShape*>(BindShape("t_" + Name(), shapePtr));
      if (mTerminatorShapePtr == nullptr)
      {
         ut::log::warning() << "Could not bind shape: terminator";
      }
      auto geode = ut::osg::make_ref<osg::Geode>();
      mTerminatorShapePtr->GetTree()->addChild(geode.get());
      mTerminatorGeometry = ut::osg::make_ref<osg::Geometry>();
      geode->addDrawable(mTerminatorGeometry.get());
      mTerminatorVArray = ut::osg::make_ref<osg::Vec3Array>();
      mTerminatorCArray = ut::osg::make_ref<osg::Vec4Array>();
      mTerminatorGeometry->setVertexArray(mTerminatorVArray.get());
      mTerminatorGeometry->setColorArray(mTerminatorCArray.get());
      mTerminatorGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      mTerminatorGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      mTerminatorGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(mTerminatorWidth));
   }
   mTerminatorCArray->clear();
   mTerminatorVArray->clear();
   mTerminatorGeometry->removePrimitiveSet(0, mTerminatorGeometry->getNumPrimitiveSets());
   UtVec3<double> svec;
   UtSun::GetSunVecWCS(mTerminatorUpdateEpoch, svec);
   osg::Vec3 sunvec(svec[0], svec[1], svec[2]);
   osg::Vec3 north(0.0f, 0.0f, 1.0f);
   osg::Vec3 termPoint = sunvec ^ north;
   termPoint.normalize();
   osg::Quat q;
   q.makeRotate(UtMath::cTWO_PI / 800.0, sunvec);

   double lastlla[3] = {UtMath::cDEG_PER_RAD *
                           atan2(termPoint[2], sqrt(termPoint[0] * termPoint[0] + termPoint[1] * termPoint[1])),
                        UtMath::cDEG_PER_RAD * atan2(termPoint[1], termPoint[0]),
                        500};
   int    start      = 0;
   for (unsigned int i = 0; i < 800; ++i)
   {
      termPoint     = q * termPoint;
      double lla[3] = {UtMath::cDEG_PER_RAD *
                          atan2(termPoint[2], sqrt(termPoint[0] * termPoint[0] + termPoint[1] * termPoint[1])),
                       UtMath::cDEG_PER_RAD * atan2(termPoint[1], termPoint[0]),
                       500};

      if (fabs(lastlla[1] - lla[1]) > 270) // if we cross the date-line
      {
         start = i;
      }

      double xyz[3];
      //      UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, 500, ecef);
      GetViewer()->GetMapProjection().ConvertFromLLA(lla, xyz);
      // scale it out to the earth radius
      // add the point to the geometry
      mTerminatorVArray->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
      mTerminatorCArray->push_back(mTerminatorColor);

      lastlla[0] = lla[0];
      lastlla[1] = lla[1];
      lastlla[2] = lla[2];
   }
   mTerminatorGeometry->addPrimitiveSet(
      ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, start, 800 - start));
   mTerminatorGeometry->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, 0, start));
}

void wkf::OverlaySpace::RebuildSolarPath()
{
   if (!mSolarPathShapePtr)
   {
      UtoRawShape shapePtr;
      mSolarPathShapePtr = dynamic_cast<UtoRawShape*>(BindShape("a_" + Name(), shapePtr));
      if (mSolarPathShapePtr == nullptr)
      {
         ut::log::warning() << "Could not bind shape: SolarPath";
      }
      auto geode = ut::osg::make_ref<osg::Geode>();
      mSolarPathShapePtr->GetTree()->addChild(geode.get());
      mSolarPathGeometry = ut::osg::make_ref<osg::Geometry>();
      geode->addDrawable(mSolarPathGeometry.get());
      mSolarPathVArray = ut::osg::make_ref<osg::Vec3Array>();
      mSolarPathCArray = ut::osg::make_ref<osg::Vec4Array>();
      mSolarPathGeometry->setVertexArray(mSolarPathVArray.get());
      mSolarPathGeometry->setColorArray(mSolarPathCArray.get());
      mSolarPathGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      mSolarPathGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      mSolarPathGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(mSolarPathWidth));
   }
   mSolarPathBreak.clear();
   mSolarPathCArray->clear();
   mSolarPathVArray->clear();
   mSolarPathGeometry->removePrimitiveSet(0, mSolarPathGeometry->getNumPrimitiveSets());

   double     sTime = vaEnv.GetTime()->GetCurrentTime();
   UtCalendar epoch = vespa::VaEphemeris::GetStartTime();
   CreateZeroSolarPathPoint();
   int lastStart = 0;

   for (double t = 1; t < sTime; t += mUpdatePeriod)
   {
      UtVec3<double> svec;
      UtSun::GetSunVecWCS(epoch, svec);
      osg::Vec3 sunvec(svec[0], svec[1], svec[2]);
      double    lla[3] = {UtMath::cDEG_PER_RAD * atan2(sunvec[2], sqrt(sunvec[0] * sunvec[0] + sunvec[1] * sunvec[1])),
                       UtMath::cDEG_PER_RAD * atan2(sunvec[1], sunvec[0]),
                       500};
      double    xyz[3];
      GetViewer()->GetMapProjection().ConvertFromLLA(lla, xyz);
      if (fabs(lla[1] - mLastSolarPathLon) > 270)
      {
         size_t vsize = mSolarPathVArray->size();
         mSolarPathGeometry->addPrimitiveSet(
            ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, lastStart, vsize - lastStart));
         mSolarPathBreak.emplace_back(vsize);
         lastStart = vsize;
         // add a break
      }
      mSolarPathVArray->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
      mSolarPathCArray->push_back(mSolarPathColor);
      // add points
      mLastSolarPathLon = lla[1];
      epoch.AdvanceTimeBy(mUpdatePeriod);
   }

   unsigned int vsize = mSolarPathVArray->size();
   mSolarPathGeometry->addPrimitiveSet(
      ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, lastStart, vsize - lastStart));
}

void wkf::OverlaySpace::RemoveTerminator()
{
   UnbindShape("t_" + Name());
   mTerminatorShapePtr = nullptr;
}

void wkf::OverlaySpace::RemoveSolarPath()
{
   UnbindShape("a_" + Name());
   mSolarPathShapePtr = nullptr;
}


void wkf::OverlaySpace::Update()
{
   double epoch = vespa::VaEphemeris::GetSimTime().GetEpoch();
   if (mShowTerminator)
   {
      if (fabs(epoch - mTerminatorUpdateEpoch.GetEpoch()) > mUpdatePeriod / UtCalendar::cSEC_IN_DAY)
      {
         mTerminatorUpdateEpoch = vespa::VaEphemeris::GetSimTime();
         RebuildTerminator(); // update terminator
      }
   }
   if (mShowSolarPath)
   {
      if (mStartEpoch != vespa::VaEphemeris::GetStartTime().GetEpoch())
      {
         RebuildSolarPath();
      }
      else if (fabs(epoch - mSolarPathUpdateEpoch.GetEpoch()) > mUpdatePeriod / UtCalendar::cSEC_IN_DAY)
      {
         mSolarPathUpdateEpoch = vespa::VaEphemeris::GetSimTime();

         double sTime = vaEnv.GetTime()->GetCurrentTime();
         if (sTime > mSolarPathVArray->size() * mUpdatePeriod)
         {
            if (mSolarPathVArray->empty())
            {
               CreateZeroSolarPathPoint();
               // add a zero time point
            }
            AddSolarPathPoints(sTime);
         }
         if (!mSolarPathVArray->empty())
         {
            RebuildSolarPathGeometry(sTime);
         }
      }
   }
   if (mSubSolarPoint.showPoint)
   {
      if (fabs(epoch - mSubSolarPoint.pointUpdateEpoch.GetEpoch()) > mUpdatePeriod / UtCalendar::cSEC_IN_DAY)
      {
         mSubSolarPoint.pointUpdateEpoch = vespa::VaEphemeris::GetSimTime();
         RebuildPoint(mSubSolarPoint); // update sub-solar point
      }
   }
   if (mSubLunarPoint.showPoint)
   {
      if (fabs(epoch - mSubLunarPoint.pointUpdateEpoch.GetEpoch()) > mUpdatePeriod / UtCalendar::cSEC_IN_DAY)
      {
         mSubLunarPoint.pointUpdateEpoch = vespa::VaEphemeris::GetSimTime();
         RebuildPoint(mSubLunarPoint); // update sub-lunar point
      }
   }
}

void wkf::OverlaySpace::SetTerminatorColor(const float aColor[4])
{
   mTerminatorColor.set(aColor[0], aColor[1], aColor[2], aColor[3]);
   if (mShowTerminator)
   {
      RebuildTerminator();
   }
}

void wkf::OverlaySpace::SetTerminatorWidth(unsigned int aWidth)
{
   mTerminatorWidth = aWidth;
   if (mTerminatorGeometry.valid())
   {
      mTerminatorGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(aWidth));
   }
}

void wkf::OverlaySpace::SetSolarPathColor(const float aColor[4])
{
   mSolarPathColor.set(aColor[0], aColor[1], aColor[2], aColor[3]);
   if (mShowSolarPath)
   {
      RebuildSolarPath();
   }
}

void wkf::OverlaySpace::SetSolarPathWidth(unsigned int aWidth)
{
   mSolarPathWidth = aWidth;
   if (mSolarPathGeometry.valid())
   {
      mSolarPathGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(aWidth));
   }
}

void wkf::OverlaySpace::SetSubSolarPointColor(const float aColor[4])
{
   SetSubPointColor(mSubSolarPoint, aColor);
}

void wkf::OverlaySpace::SetSubSolarPointScale(const float aScale)
{
   SetSubPointScale(mSubSolarPoint, aScale);
}

void wkf::OverlaySpace::SetSubSolarPointIcon(const QString& aIconPath)
{
   SetSubPointIcon(mSubSolarPoint, aIconPath);
}

void wkf::OverlaySpace::SetSubLunarPointColor(const float aColor[4])
{
   SetSubPointColor(mSubLunarPoint, aColor);
}

void wkf::OverlaySpace::SetSubLunarPointScale(const float aScale)
{
   SetSubPointScale(mSubLunarPoint, aScale);
}

void wkf::OverlaySpace::SetSubLunarPointIcon(const QString& aIconPath)
{
   SetSubPointIcon(mSubLunarPoint, aIconPath);
}

void wkf::OverlaySpace::SetUpdatePeriod(const double aPeriod)
{
   mUpdatePeriod = aPeriod;
   if (mShowSolarPath)
   {
      RebuildSolarPath();
   }
}

std::string wkf::OverlaySpace::Name(bool aSolar) const
{
   return std::string(((aSolar) ? "sun" : "moon") + std::to_string(GetUniqueId()));
}

void wkf::OverlaySpace::MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjectionPtr)
{
   if (aViewerId == GetViewer()->GetUniqueId())
   {
      if (mShowTerminator)
      {
         RebuildTerminator();
      }
      if (mShowSolarPath)
      {
         RebuildSolarPath();
      }
      if (mSubSolarPoint.showPoint)
      {
         RebuildPoint(mSubSolarPoint);
      }
      if (mSubLunarPoint.showPoint)
      {
         RebuildPoint(mSubLunarPoint);
      }
   }
}

void wkf::OverlaySpace::RebuildSolarPathGeometry(double aSimTime)
{
   mSolarPathGeometry->removePrimitiveSet(0, mSolarPathGeometry->getNumPrimitiveSets());
   size_t lastb = 0;
   for (size_t b : mSolarPathBreak)
   {
      if (b * mUpdatePeriod < aSimTime)
      {
         mSolarPathGeometry->addPrimitiveSet(
            ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, lastb, b - lastb));
         lastb = b;
      }
   }
   int lastindex = std::min((size_t)(aSimTime / mUpdatePeriod), mSolarPathVArray->size());
   mSolarPathGeometry->addPrimitiveSet(
      ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, lastb, lastindex - lastb));
}

void wkf::OverlaySpace::CreateZeroSolarPathPoint()
{
   UtCalendar     epoch = vespa::VaEphemeris::GetStartTime();
   UtVec3<double> svec;
   UtSun::GetSunVecWCS(epoch, svec);
   osg::Vec3 sunvec(svec[0], svec[1], svec[2]);
   double    lla[3]  = {UtMath::cDEG_PER_RAD * atan2(sunvec[2], sqrt(sunvec[0] * sunvec[0] + sunvec[1] * sunvec[1])),
                    UtMath::cDEG_PER_RAD * atan2(sunvec[1], sunvec[0]),
                    500};
   mLastSolarPathLon = lla[1];
   double xyz[3];
   GetViewer()->GetMapProjection().ConvertFromLLA(lla, xyz);
   mSolarPathVArray->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
   mSolarPathCArray->push_back(mSolarPathColor);
}

void wkf::OverlaySpace::AddSolarPathPoints(double aSimTime)
{
   int        idx   = mSolarPathVArray->size();
   UtCalendar epoch = vespa::VaEphemeris::GetStartTime();
   epoch.AdvanceTimeBy(mUpdatePeriod * idx);
   for (double t = idx * mUpdatePeriod; t < aSimTime; t += mUpdatePeriod)
   {
      UtVec3<double> svec;
      UtSun::GetSunVecWCS(epoch, svec);
      osg::Vec3 sunvec(svec[0], svec[1], svec[2]);
      double    lla[3] = {UtMath::cDEG_PER_RAD * atan2(sunvec[2], sqrt(sunvec[0] * sunvec[0] + sunvec[1] * sunvec[1])),
                       UtMath::cDEG_PER_RAD * atan2(sunvec[1], sunvec[0]),
                       500};
      double    xyz[3];
      GetViewer()->GetMapProjection().ConvertFromLLA(lla, xyz);
      if (fabs(lla[1] - mLastSolarPathLon) > 270)
      {
         mSolarPathBreak.emplace_back(mSolarPathVArray->size());
         // add a break
      }
      mSolarPathVArray->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
      mSolarPathCArray->push_back(mSolarPathColor);
      // add points
      mLastSolarPathLon = lla[1];
      epoch.AdvanceTimeBy(mUpdatePeriod);
   }
}

void wkf::OverlaySpace::SetSubPointColor(SubPointInfo& aInfo, const float aColor[4])
{
   aInfo.pointColor.set(aColor[0], aColor[1], aColor[2], aColor[3]);
   if (aInfo.showPoint)
   {
      aInfo.pointDecoratorNode->SetMultColor(aInfo.pointColor.r(),
                                             aInfo.pointColor.g(),
                                             aInfo.pointColor.b(),
                                             aInfo.pointColor.a());
   }
}

void wkf::OverlaySpace::SetSubPointScale(SubPointInfo& aInfo, const float aScale)
{
   aInfo.pointScale = aScale;
   if (aInfo.showPoint)
   {
      aInfo.pointDecoratorNode->SetScale(aScale);
   }
}

void wkf::OverlaySpace::SetSubPointIcon(SubPointInfo& aInfo, const QString& aIconPath)
{
   QString imagePath   = (aInfo.pointType) ? cDEFAULT_SUN_PATH : cDEFAULT_MOON_PATH;
   aInfo.pointIconPath = aIconPath;
   if (aInfo.showPoint)
   {
      if (aIconPath.isEmpty())
      {
         aInfo.pointDecoratorNode->SetImage(QIcon(imagePath).pixmap(32, 32).toImage(), "hi");
         // load the default icon
      }
      else
      {
         QIcon icon(aInfo.pointIconPath);
         if (icon.isNull())
         {
            aInfo.pointDecoratorNode->SetImage(QIcon(imagePath).pixmap(32, 32).toImage(), "hi");
         }
         else
         {
            aInfo.pointDecoratorNode->SetImage(icon.pixmap(32, 32).toImage(), "hi");
         }
      }
   }
}

void wkf::OverlaySpace::RebuildPoint(SubPointInfo& aInfo)
{
   QString imagePath = (aInfo.pointType) ? cDEFAULT_SUN_PATH : cDEFAULT_MOON_PATH;
   if (!aInfo.pointShapePtr)
   {
      UtoRawShape shapePtr;
      aInfo.pointShapePtr = dynamic_cast<UtoRawShape*>(BindShape("p_" + Name(aInfo.pointType), shapePtr));
      if (aInfo.pointShapePtr == nullptr)
      {
         std::string type = (aInfo.pointType) ? "solar" : "lunar";
         ut::log::warning() << ("Could not bind shape: sub-" + type + " point");
      }
      aInfo.pointTransform = ut::osg::make_ref<osg::PositionAttitudeTransform>();
      aInfo.pointShapePtr->GetTree()->addChild(aInfo.pointTransform.get());
      aInfo.pointDecoratorNode = ut::osg::make_ref<vespa::VaDecoratorNode>(vespa::VaDecoratorNode::cCentered);
      aInfo.pointTransform->addChild(aInfo.pointDecoratorNode.get());
      if (aInfo.pointIconPath.isEmpty())
      {
         aInfo.pointDecoratorNode->SetImage(QIcon(imagePath).pixmap(32, 32).toImage(), "hi");
      }
      else
      {
         QIcon icon(aInfo.pointIconPath);
         if (icon.isNull())
         {
            aInfo.pointDecoratorNode->SetImage(QIcon(imagePath).pixmap(32, 32).toImage(), "hi");
         }
         else
         {
            aInfo.pointDecoratorNode->SetImage(icon.pixmap(32, 32).toImage(), "hi");
         }
      }
      aInfo.pointDecoratorNode->SetMultColor(aInfo.pointColor.r(),
                                             aInfo.pointColor.g(),
                                             aInfo.pointColor.b(),
                                             aInfo.pointColor.a());
      aInfo.pointDecoratorNode->SetScale(aInfo.pointScale);
   }
   UtVec3<double> svec;
   (aInfo.pointType) ? UtSun::GetSunVecWCS(aInfo.pointUpdateEpoch, svec) : UtMoon::GetVecWCS(aInfo.pointUpdateEpoch, svec);
   osg::Vec3 sunvec(svec[0], svec[1], svec[2]);
   double    lla[3] = {UtMath::cDEG_PER_RAD * atan2(sunvec[2], sqrt(sunvec[0] * sunvec[0] + sunvec[1] * sunvec[1])),
                    UtMath::cDEG_PER_RAD * atan2(sunvec[1], sunvec[0]),
                    500};
   double    xyz[3];
   GetViewer()->GetMapProjection().ConvertFromLLA(lla, xyz);
   aInfo.pointTransform->setPosition(osg::Vec3(xyz[0], xyz[1], xyz[2]));
}

void wkf::OverlaySpace::RemovePoint(SubPointInfo& aInfo)
{
   UnbindShape("p_" + Name(aInfo.pointType));
   aInfo.pointShapePtr = nullptr;
}

void wkf::OverlaySpace::ShowSubPoint(SubPointInfo& aInfo, bool aState)
{
   aInfo.showPoint = aState;
   if (aState)
   {
      aInfo.pointUpdateEpoch = vespa::VaEphemeris::GetSimTime();
      RebuildPoint(aInfo);
   }
   else
   {
      RemovePoint(aInfo);
   }
}
