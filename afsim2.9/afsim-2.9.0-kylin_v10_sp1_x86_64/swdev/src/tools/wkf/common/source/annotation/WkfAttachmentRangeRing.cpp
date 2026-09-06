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

#include "WkfAttachmentRangeRing.hpp"

#include <array>

#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/MatrixTransform>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtUnitTypes.hpp"
#include "UtoMemory.hpp"
#include "UtoRawShape.hpp"
#include "VaDecoratorNode.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"

wkf::AttachmentRangeRing::AttachmentRangeRing(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr, const std::string& aName)
   : vespa::VaAttachment(aParent, aViewerPtr, aName)
   , mProperties()
{
   SetType<AttachmentRangeRing>();
   UtoRawShape raw;
   raw.SetReferenceFrame(UtoShape::cFRAME_ENTITY);
   mProperties.mName = QString::fromStdString(aName);
   std::string name  = GetName();
   name += "_";
   name += std::to_string(GetUniqueId());
   mShapePtr = AddShapeToParent(name, raw);
   mShapePtr->SetName(GetName());
   mShapePtr->OverrideRenderBin(1, false);

   mProjection = aViewerPtr->GetMapProjection();
   Build();
   mCallbacks.Add(wkf::Observer::MapProjectionChanged.Connect(&AttachmentRangeRing::MapProjectionChangedCB, this));
   mCallbacks.Add(wkf::Observer::ChangedUnitFormat.Connect(&AttachmentRangeRing::UnitsChangedCB, this));
   mCallbacks.Add(wkf::Observer::EntityMoved.Connect(&AttachmentRangeRing::EntityMovedCB, this, GetParent().GetUniqueId()));
}

void wkf::AttachmentRangeRing::SetCenterRadius(float aValue)
{
   mProperties.mBaseRadius = aValue;
   Build();
}

void wkf::AttachmentRangeRing::SetAdditionalRings(int aNumber, float aValue)
{
   mProperties.mAdditionalRings      = aNumber;
   mProperties.mAdditionalRingRadius = aValue;
   Build();
}

void wkf::AttachmentRangeRing::SetCenterAngle(float aValue)
{
   mProperties.mCenterAngle = aValue;
   Build();
}

void wkf::AttachmentRangeRing::SetAngleWidth(float aValue)
{
   mProperties.mWidth = aValue;
   Build();
}

void wkf::AttachmentRangeRing::SetRadialRate(float aValue)
{
   mProperties.mRadialRate = std::round(UtMath::cDEG_PER_RAD * aValue) * UtMath::cRAD_PER_DEG;
   Build();
}

void wkf::AttachmentRangeRing::SetRingColor(const QColor& aColor)
{
   mProperties.mRingColor = aColor;
   Build();
}

void wkf::AttachmentRangeRing::SetRingLineWidth(int aWidth)
{
   mProperties.mRingLineWidth = aWidth;
   Build();
}

void wkf::AttachmentRangeRing::SetRadialColor(const QColor& aColor)
{
   mProperties.mRadialColor = aColor;
   Build();
}

void wkf::AttachmentRangeRing::SetRadialLineWidth(int aWidth)
{
   mProperties.mRadialLineWidth = aWidth;
   Build();
}

void wkf::AttachmentRangeRing::SetRangeLabelsEnabled(bool aState)
{
   mProperties.mShowRangeLabels = aState;
   Build();
}

void wkf::AttachmentRangeRing::SetAngleLabelsEnabled(bool aState)
{
   mProperties.mShowAngleLabels = aState;
   Build();
}

void wkf::AttachmentRangeRing::SetAlignWithPlatformHeading(bool aState)
{
   mProperties.mAlignWithPlatform = aState;
   Build();
}

void wkf::AttachmentRangeRing::SetRangeLabelColor(const QColor& aColor)
{
   mProperties.mRangeLabelColor = aColor;
   Build();
}

void wkf::AttachmentRangeRing::SetAngleLabelColor(const QColor& aColor)
{
   mProperties.mAngleLabelColor = aColor;
   Build();
}

void wkf::AttachmentRangeRing::SetProperties(const Properties& aProps)
{
   mProperties = aProps;
   vespa::VaAttachment::SetName(aProps.mName.toStdString());
   Build();
}

void wkf::AttachmentRangeRing::SetName(const std::string& aName)
{
   mProperties.mName = QString::fromStdString(aName);
   vespa::VaAttachment::SetName(aName);
}

void wkf::AttachmentRangeRing::Build()
{
   if (!mProperties.mAlignWithPlatform)
   {
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_NED_AT_ENTITY);
   }
   else if (mProjection.projection == UtoMapProjection::Geocentric)
   {
      mHeading = 0.0;
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
   }
   else
   {
      mHeading = GetParent().GetHeading();
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_NED_AT_ENTITY);
   }

   unsigned int c = mShapePtr->GetTree()->getNumChildren();
   mShapePtr->GetTree()->removeChildren(0, c);

   auto ringgeode = ut::osg::make_ref<osg::Geode>();
   ringgeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   ringgeode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   ringgeode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::BlendFunc>(), osg::StateAttribute::ON);
   ringgeode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(mProperties.mRingLineWidth),
                                                          osg::StateAttribute::ON);
   mShapePtr->GetTree()->addChild(ringgeode);
   auto radialgeode = ut::osg::make_ref<osg::Geode>();
   radialgeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   radialgeode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   radialgeode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::BlendFunc>(), osg::StateAttribute::ON);
   radialgeode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(mProperties.mRadialLineWidth),
                                                            osg::StateAttribute::ON);
   mShapePtr->GetTree()->addChild(radialgeode);

   for (int j = 0; j <= mProperties.mAdditionalRings; ++j)
   {
      const float r = mProperties.mBaseRadius + j * mProperties.mAdditionalRingRadius;
      DrawRing(ringgeode, r);
   }

   auto radials = ut::osg::make_ref<osg::Geometry>();
   radialgeode->addDrawable(radials);
   auto vr     = ut::osg::make_ref<osg::Vec3Array>();
   auto carray = ut::osg::make_ref<osg::Vec4Array>();
   carray->push_back(osg::Vec4(mProperties.mRadialColor.redF(),
                               mProperties.mRadialColor.greenF(),
                               mProperties.mRadialColor.blueF(),
                               mProperties.mRadialColor.alphaF()));
   radials->setVertexArray(vr);
   radials->setColorArray(carray);
   radials->setColorBinding(osg::Geometry::BIND_OVERALL);
   unsigned int iStart = 0;

   if (mProperties.mWidth < UtMath::cTWO_PI)
   {
      iStart = DrawRadial(-mProperties.mWidth * 0.5f, vr, radials, iStart);
      iStart = DrawRadial(mProperties.mWidth * 0.5f, vr, radials, iStart);
      // draw the edges
   }

   if (mProperties.mRadialRate > 0)
   {
      // draw the centerline
      iStart = DrawRadial(0.0f, vr, radials, iStart);

      // draw the radials
      for (float f = mProperties.mRadialRate; f <= mProperties.mWidth * 0.5; f += mProperties.mRadialRate)
      {
         iStart = DrawRadial(f, vr, radials, iStart);
         iStart = DrawRadial(-f, vr, radials, iStart);
      }
   }
}

double wkf::AttachmentRangeRing::GetHeading() const
{
   return GetCenterAngle() + (GetAlignWithPlatformHeading() ? mHeading : 0);
}

void wkf::AttachmentRangeRing::DrawRing(osg::Geode* aGeodePtr, float aRadius)
{
   unsigned int start   = 0;
   unsigned int indices = 0;
   double       lastLon = 0;
   if (mProperties.mShowRangeLabels)
   {
      DrawRangeLabel(aRadius);
   }
   auto ring = ut::osg::make_ref<osg::Geometry>();
   aGeodePtr->addDrawable(ring);

   auto ca = ut::osg::make_ref<osg::Vec4Array>();
   ca->push_back(osg::Vec4(mProperties.mRingColor.redF(),
                           mProperties.mRingColor.greenF(),
                           mProperties.mRingColor.blueF(),
                           mProperties.mRingColor.alphaF()));
   auto            v        = ut::osg::make_ref<osg::Vec3Array>();
   constexpr float earthRad = UtEarth::cA;
   const float     s        = sin(aRadius * 0.5 / earthRad);
   const float     c        = cos(aRadius * 0.5 / earthRad);
   const float     vert     = 2 * earthRad * s * s;
   const float     horz     = 2 * earthRad * s * c;

   const float angleConv = mProperties.mWidth / 360.0;
   for (int i = 0; i <= 360; ++i)
   {
      bool      split = false;
      float     ang   = (GetHeading() - 0.5 * mProperties.mWidth) + i * angleConv;
      osg::Vec3 xyz(horz * cos(ang), horz * sin(ang), vert);
      if (mProjection.projection != UtoMapProjection::Geocentric)
      {
         ConvertToLocalFlat(xyz);
         if (fabs(lastLon - xyz[1]) > 180 * 111318.163)
         {
            split = true;
         }
         lastLon = xyz[1];
      }
      if (split)
      {
         if (indices > 1)
         {
            ring->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, start, indices));
         }
         start += indices;
         indices = 0;
      }
      v->push_back(xyz);
      indices++;
   }
   ring->setVertexArray(v);
   ring->setColorArray(ca);
   ring->setColorBinding(osg::Geometry::BIND_OVERALL);
   if (indices > 1)
   {
      ring->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, start, indices));
   }
}

unsigned int wkf::AttachmentRangeRing::DrawRadial(float           aAngle,
                                                  osg::Vec3Array* aVectorArray,
                                                  osg::Geometry*  aGeometry,
                                                  unsigned int    aStart)
{
   unsigned int    start   = aStart;
   unsigned int    indices = 0;
   double          lastLon = 0;
   const float     ca      = cos(aAngle + GetHeading());
   const float     sa      = sin(aAngle + GetHeading());
   const float     range   = mProperties.mBaseRadius + mProperties.mAdditionalRings * mProperties.mAdditionalRingRadius;
   constexpr float sampleRate = 11112.0f; // 1 nmi
   if (mProperties.mShowAngleLabels)
   {
      DrawAngleLabel(range, aAngle);
   }

   constexpr float earthRad = UtEarth::cA;
   for (float fi = 0; fi < range; fi += sampleRate)
   {
      bool        split = false;
      const float s     = sin(fi * 0.5 / earthRad);
      const float c     = cos(fi * 0.5 / earthRad);
      const float vert  = 2 * earthRad * s * s;
      const float horz  = 2 * earthRad * s * c;

      osg::Vec3 xyz(horz * ca, horz * sa, vert);
      if (mProjection.projection != UtoMapProjection::Geocentric)
      {
         ConvertToLocalFlat(xyz);
         if (fabs(lastLon - xyz[1]) > 180 * 111318.163)
         {
            split = true;
         }
         lastLon = xyz[1];
      }
      if (split)
      {
         if (indices > 1)
         {
            aGeometry->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, start, indices));
         }
         start += indices;
         indices = 0;
      }
      // if flat, convert
      aVectorArray->push_back(xyz);
      indices++;
   }
   // add a point at range
   const float s    = sin(range * 0.5 / earthRad);
   const float c    = cos(range * 0.5 / earthRad);
   const float vert = 2 * earthRad * s * s;
   const float horz = 2 * earthRad * s * c;

   osg::Vec3 xyz(horz * ca, horz * sa, vert);
   if (mProjection.projection != UtoMapProjection::Geocentric)
   {
      ConvertToLocalFlat(xyz);
   }
   aVectorArray->push_back(xyz);
   indices++;

   if (indices > 1)
   {
      aGeometry->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINE_STRIP, start, indices));
   }
   return start + indices;
}

void wkf::AttachmentRangeRing::MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection)
{
   if (!mProperties.mAlignWithPlatform)
   {
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_NED_AT_ENTITY);
   }
   else if (aProjection->projection == UtoMapProjection::Geocentric)
   {
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      mHeading = 0.0;
   }
   else
   {
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_NED_AT_ENTITY);
      mHeading = GetParent().GetHeading();
   }
   if (aViewerId == GetViewer()->GetUniqueId())
   {
      mProjection = *aProjection;
      Build();
   }
}

void wkf::AttachmentRangeRing::DrawRangeLabel(float aRange)
{
   const QString str = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(ValueType::cLENGTH, aRange);

   // draw at r=aRange, a=mCenterAngle
   auto matXform = ut::osg::make_ref<osg::MatrixTransform>();

   const float     ca       = cos(GetHeading());
   const float     sa       = sin(GetHeading());
   constexpr float earthRad = UtEarth::cA;
   const float     s        = sin(aRange * 0.5 / earthRad);
   const float     c        = cos(aRange * 0.5 / earthRad);
   const float     vert     = 2 * earthRad * s * s;
   const float     horz     = 2 * earthRad * s * c;
   osg::Vec3       xyz(ca * horz, sa * horz, vert);
   if (mProjection.projection != UtoMapProjection::Geocentric)
   {
      ConvertToLocalFlat(xyz);
   }
   matXform->preMult(osg::Matrix::translate(xyz));
   mShapePtr->GetTree()->addChild(matXform);
   auto dnode = ut::osg::make_ref<vespa::VaDecoratorNode>(vespa::VaDecoratorNode::cHideBehindEarth, 0, 0);
   dnode->SetText(str, Qt::transparent, mProperties.mRangeLabelColor);
   matXform->addChild(dnode);
}

void wkf::AttachmentRangeRing::DrawAngleLabel(float aRange, float aAngle)
{
   if (aAngle <= -3.14159f) // hey, this is an approximation of pi... I would actually prefer that it cuts off before
                            // pi, so I think this is ok.
   {
      return;
   }
   const UtUnitAngle::BaseUnit au      = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
   const float                 dispVal = UtUnitAngle::ConvertFromStandard(aAngle, au);
   const QString               str     = QString::number(dispVal) + " " +
                       QString::fromStdString(UtUnitAngle::FindUnitName(au)); // convert aAngle to user's preferred units
   // draw at r=aRange, a=aAngle
   auto matXform = ut::osg::make_ref<osg::MatrixTransform>();

   const float     ca       = cos(aAngle + GetHeading());
   const float     sa       = sin(aAngle + GetHeading());
   constexpr float earthRad = UtEarth::cA;
   const float     s        = sin(aRange * 0.5 / earthRad);
   const float     c        = cos(aRange * 0.5 / earthRad);
   const float     vert     = 2 * earthRad * s * s;
   const float     horz     = 2 * earthRad * s * c;
   osg::Vec3       xyz(ca * horz, sa * horz, vert);
   if (mProjection.projection != UtoMapProjection::Geocentric)
   {
      ConvertToLocalFlat(xyz);
   }
   matXform->preMult(osg::Matrix::translate(xyz));
   mShapePtr->GetTree()->addChild(matXform);
   auto dnode = ut::osg::make_ref<vespa::VaDecoratorNode>(vespa::VaDecoratorNode::cHideBehindEarth, 0, -15);
   dnode->SetText(str, Qt::transparent, mProperties.mAngleLabelColor);
   matXform->addChild(dnode);
}

void wkf::AttachmentRangeRing::UnitsChangedCB(int aUnitType, int aUnit)
{
   Build();
}

void wkf::AttachmentRangeRing::ConvertToLocalFlat(osg::Vec3& aXYZ) const
{
   std::array<double, 3> ecef;
   GetParent().GetPosition().GetECEF(ecef.data());
   UtEntity ent;
   ent.SetLocationWCS(ecef.data());
   std::array<double, 3> ned = {aXYZ[0], aXYZ[1], aXYZ[2]};
   std::array<double, 3> vecWCS;
   ent.ConvertNEDToWCS(ned.data(), vecWCS.data());
   mProjection.ConvertFromECEF(vecWCS.data());
   std::array<double, 3> entp = {GetParent().GetPosition().GetECEF_X(),
                                 GetParent().GetPosition().GetECEF_Y(),
                                 GetParent().GetPosition().GetECEF_Z()};
   mProjection.ConvertFromECEF(entp.data());
   aXYZ = osg::Vec3(vecWCS[1] - entp[1],
                    vecWCS[0] - entp[0],
                    std::min(vecWCS[2] - entp[2], -200.0)); // the 200 should put it about the ground, and on flat maps,
                                                            // the actual height doesn't really matter
}

void wkf::AttachmentRangeRing::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   if (mProjection.projection != UtoMapProjection::Geocentric)
   {
      mHeading = aEntityPtr->GetHeading();
      Build();
   }
}
