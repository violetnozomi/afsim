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

#include "RvPluginAnnotation.hpp"

#include "RvResultData.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "annotation/WkfAnnotationDisplayInterface.hpp"

namespace
{
QColor Vec3fToQColor(const rv::Vec3f& aColor)
{
   QColor newColor;
   newColor.setRgbF(aColor.x(), aColor.y(), aColor.z());
   return newColor;
}

wkf::AttachmentRangeRing::Properties GetRangeRingProperties(const rv::MsgAnnotationRangeRing& aRangeRing)
{
   wkf::AttachmentRangeRing::Properties properties;
   properties.mName                 = QString::fromStdString(aRangeRing.ringName());
   properties.mBaseRadius           = aRangeRing.centerRadius();
   properties.mAdditionalRings      = aRangeRing.additionalRings();
   properties.mAdditionalRingRadius = aRangeRing.additionalRadii();
   properties.mCenterAngle          = aRangeRing.centerAngle();
   properties.mWidth                = aRangeRing.angleWidth();
   properties.mRadialRate           = aRangeRing.radialRate();
   properties.mRingColor            = Vec3fToQColor(aRangeRing.ringColor());
   properties.mRadialColor          = Vec3fToQColor(aRangeRing.radialColor());
   properties.mRingLineWidth        = aRangeRing.ringWidth();
   properties.mRadialLineWidth      = aRangeRing.radialWidth();
   properties.mShowRangeLabels      = aRangeRing.showRangeLabels();
   properties.mShowAngleLabels      = aRangeRing.showAngleLabels();
   properties.mRangeLabelColor      = Vec3fToQColor(aRangeRing.rangeLabelColor());
   properties.mAngleLabelColor      = Vec3fToQColor(aRangeRing.angleLabelColor());
   properties.mAlignWithPlatform    = aRangeRing.alignPlatform();
   properties.mExportable           = false;
   return properties;
}
} // namespace

WKF_PLUGIN_DEFINE_SYMBOLS(RvAnnotation::Plugin,
                          "Annotation",
                          "The Annotation plugin is responsible for showing visual annotations on the map display.",
                          "mystic")

RvAnnotation::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::PluginT<Annotation::Plugin>(aPluginName, aUniqueId)
{
   QObject::connect(&rvEnv, &rv::Environment::AnnotationRead, this, &RvAnnotation::Plugin::AnnotationReadCB, Qt::DirectConnection);
   mCallbacks.Add(wkf::Observer::PlatformActiveChanged.Connect(&Plugin::PlatformActiveChangedCB, this));

   connect(&wkfEnv, &wkf::Environment::DeleteShortcutTriggered, this, &Plugin::DeleteCurrentlySelectedAnnotations);
}

// Annotations are read prior to platforms being made visible, so platform-dependent draws will be deferred until later,
// but other draws can still be performed
void RvAnnotation::Plugin::AnnotationReadCB(rv::MsgBase& aMessage)
{
   if (aMessage.GetMessageId() == rv::MsgAnnotationDecoration::cMESSAGE_ID)
   {
      rv::MsgAnnotationDecoration decoration = static_cast<rv::MsgAnnotationDecoration&>(aMessage);
      wkf::Entity*                ent        = dynamic_cast<wkf::Entity*>(vaEnv.FindEntity(decoration.entityName()));
      if (ent)
      {
         QString text      = "";
         QString imageDesc = "";
         if ((rv::AnnotationDecorationType::Value)decoration.decorationType() == rv::AnnotationDecorationType::text)
         {
            text = QString::fromStdString(decoration.decorationInfo());
         }
         else
         {
            imageDesc = QString::fromStdString(decoration.decorationInfo());
         }
         mDisplayInterface->AddDecoration(*ent, text, imageDesc);
         auto* dec = ent->FindFirstAttachmentOfType<Annotation::AnnotationDecorator>();
         if (dec)
         {
            dec->SetExportable(false);
         }
      }
      else
      {
         mDeferredDecorations[decoration.entityName()] = decoration;
      }
   }
   else if (aMessage.GetMessageId() == rv::MsgAnnotationPoi::cMESSAGE_ID)
   {
      rv::MsgAnnotationPoi poi = static_cast<rv::MsgAnnotationPoi&>(aMessage);
      if (!poi.bullseye())
      {
         mDisplayInterface->AddDetailedPOI<wkf::PointOfInterest>("pointOfInterest",
                                                                 poi.lat(),
                                                                 poi.lon(),
                                                                 poi.alt(),
                                                                 poi.poiName(),
                                                                 false);
      }
      else
      {
         mDisplayInterface->AddDetailedPOI<wkf::Bullseye>("bullseye", poi.lat(), poi.lon(), poi.alt(), poi.poiName(), false);
      }
      // Add the deferred range ring when the associated poi is added
      auto rrIt = mDeferredPoi_RangeRings.find(poi.poiName());
      if (rrIt != mDeferredPoi_RangeRings.end())
      {
         wkf::PointOfInterest* poiPtr = dynamic_cast<wkf::PointOfInterest*>(vaEnv.FindEntity(rrIt->first));
         mDisplayInterface->AddRangeRing(poiPtr, GetRangeRingProperties(rrIt->second));
         mDeferredPoi_RangeRings.erase(rrIt);
         mDeferredPlatform_RangeRings.erase(poi.poiName());
      }
   }
   else if (aMessage.GetMessageId() == rv::MsgAnnotationRangeRing::cMESSAGE_ID)
   {
      rv::MsgAnnotationRangeRing rangeRing = static_cast<rv::MsgAnnotationRangeRing&>(aMessage);
      wkf::PointOfInterest*      poi = dynamic_cast<wkf::PointOfInterest*>(vaEnv.FindEntity(rangeRing.referenceName()));
      if (poi)
      {
         mDisplayInterface->AddRangeRing(poi, GetRangeRingProperties(rangeRing));
      }
      else
      {
         mDeferredPlatform_RangeRings.emplace(rangeRing.referenceName(), rangeRing);
         mDeferredPoi_RangeRings[rangeRing.referenceName()] = rangeRing;
      }
   }
}

void RvAnnotation::Plugin::PlatformActiveChangedCB(wkf::Platform* aPlatformPtr, bool aActive)
{
   if (aActive)
   {
      std::string platName = aPlatformPtr->GetName();
      auto        decIt    = mDeferredDecorations.find(platName);
      if (decIt != mDeferredDecorations.end())
      {
         wkf::Entity* entity = dynamic_cast<wkf::Entity*>(vaEnv.FindEntity(decIt->first));
         if (entity)
         {
            QString text      = "";
            QString imageDesc = "";
            char    decType   = decIt->second.decorationType();
            if (decType == rv::AnnotationDecorationType::Value::text)
            {
               text = QString::fromStdString(decIt->second.decorationInfo());
            }
            else // (decType == rv::AnnotationDecorationType::Value::preset) || (decType ==
                 // rv::AnnotationDecorationType::Value::image)
            {
               imageDesc = QString::fromStdString(decIt->second.decorationInfo());
               ;
            }
            mDisplayInterface->AddDecoration(*entity, text, imageDesc);
            auto* dec = entity->FindFirstAttachmentOfType<Annotation::AnnotationDecorator>();
            if (dec)
            {
               dec->SetExportable(false);
            }
            mDeferredDecorations.erase(decIt);
         }
      }
      auto rrIt = mDeferredPlatform_RangeRings.find(platName);
      while (rrIt != mDeferredPlatform_RangeRings.end())
      {
         wkf::Platform* entity = dynamic_cast<wkf::Platform*>(vaEnv.FindEntity(rrIt->first));
         mDisplayInterface->AddRangeRing(*entity, GetRangeRingProperties(rrIt->second));
         mDeferredPlatform_RangeRings.erase(rrIt);
         mDeferredPoi_RangeRings.erase(platName);
         rrIt = mDeferredPlatform_RangeRings.find(platName);
      }
   }
}
