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

#include "MysticPluginZoneBrowser.hpp"

#include "RvResultData.hpp"
#include "VaAttachment.hpp"
#include "WkfMainWindow.hpp"
#include "zone/WkfAttachmentZone.hpp"
#include "zone/WkfZonePoint.hpp"

namespace
{
wkf::CommonZoneVariables CommonZoneVariableConversion(const rv::CommonZoneData& aData)
{
   wkf::CommonZoneVariables vars;
   vars.mZoneName              = aData.zoneName();
   vars.mMaxAltitude           = aData.maxAltitude();
   vars.mMaxAltitudeSet        = aData.maxAltitudeSet();
   vars.mMinAltitude           = aData.minAltitude();
   vars.mMinAltitudeSet        = aData.minAltitudeSet();
   vars.mReferencePlatformName = aData.referencePlatform();
   vars.mReferenceLat          = aData.referenceLat();
   vars.mReferenceLon          = aData.referenceLon();
   vars.mHeading               = aData.heading();
   vars.mLocationDefined       = aData.locationDefined();
   return vars;
}

void CircularZoneHandler(wkf::ZoneSetData& aData, const rv::CircularZoneInfo& aInfo)
{
   wkf::CircularZoneVariables vars;
   vars.mRadius     = aInfo.radius();
   vars.mMinRadius  = aInfo.minRadius();
   vars.mStartAngle = aInfo.startAngle();
   vars.mStopAngle  = aInfo.stopAngle();
   auto commonVars  = CommonZoneVariableConversion(aInfo.commonInfo());
   aData.AddCircularData(commonVars, vars);
}

void EllipticalZoneHandler(wkf::ZoneSetData& aData, const rv::EllipticalZoneInfo& aInfo)
{
   wkf::EllipticalZoneVariables vars;
   vars.mLatAxis    = aInfo.latAxis();
   vars.mLonAxis    = aInfo.lonAxis();
   vars.mMinRadius  = aInfo.minRadius();
   vars.mStartAngle = aInfo.startAngle();
   vars.mStopAngle  = aInfo.stopAngle();
   auto commonVars  = CommonZoneVariableConversion(aInfo.commonInfo());
   aData.AddEllipticalData(commonVars, vars);
}

void SphericalZoneHandler(wkf::ZoneSetData& aData, const rv::SphericalZoneInfo& aInfo)
{
   wkf::SphericalZoneVariables vars;
   vars.mRadius     = aInfo.radius();
   vars.mMinRadius  = aInfo.minRadius();
   vars.mStartAngle = aInfo.startAngle();
   vars.mStopAngle  = aInfo.stopAngle();
   auto commonVars  = CommonZoneVariableConversion(aInfo.commonInfo());
   aData.AddSphericalData(commonVars, vars);
}

void PolygonalZoneHandler(wkf::ZoneSetData& aData, const rv::PolygonalZoneInfo& aInfo)
{
   wkf::PolygonalZoneVariables vars;
   vars.mUseLatLon = aInfo.useLatLon();
   auto pointList  = aInfo.Points();
   for (const auto& it : pointList)
   {
      vars.mPoints.push_back(wkf::ZonePointStruct(it.x(), it.y()));
   }
   auto commonVars = CommonZoneVariableConversion(aInfo.commonInfo());
   aData.AddPolygonalData(commonVars, vars);
}
} // namespace

WKF_PLUGIN_DEFINE_SYMBOLS(MysticZoneBrowser::Plugin,
                          "Zones",
                          "Provides a selection browser to visualize zones on the Map Display",
                          "mystic")

MysticZoneBrowser::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mDataContainer()
   , mPrefWidgetPtr(new wkf::ZoneBrowserPrefWidget)
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mDockWidgetPtr                 = new DockWidget(mDataContainer, mainWindowPtr);
   mainWindowPtr->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->hide();

   QObject::connect(&rvEnv, &rv::Environment::ZoneRead, this, &MysticZoneBrowser::Plugin::ZoneRead, Qt::DirectConnection);
   mCallbacks.Add(wkf::Observer::PlatformActiveChanged.Connect(&MysticZoneBrowser::Plugin::PlatformActiveChangedCB, this));
   mRandom.SetSeed(15);

   // Overwrite existing colors when preferences change
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &wkf::ZoneBrowserPrefObject::ColorChoiceChanged,
           this,
           &MysticZoneBrowser::Plugin::ApplyColorPrefs);
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &wkf::ZoneBrowserPrefObject::DefaultLineColorChanged,
           [this](const QColor& aColor) { ApplyColorPrefs(mPrefWidgetPtr->GetPreferenceObject()->GetColorChoice()); });
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &wkf::ZoneBrowserPrefObject::DefaultFillColorChanged,
           [this](const QColor& aColor) { ApplyColorPrefs(mPrefWidgetPtr->GetPreferenceObject()->GetColorChoice()); });
}

QList<wkf::PrefWidget*> MysticZoneBrowser::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void MysticZoneBrowser::Plugin::ZoneRead(rv::MsgBase& aMessage)
{
   // Note: Zones should always be associated with the parent platform, even if they will be drawn as an attachment to
   // some other entity (reference).
   auto handleData = [this](bool aGlobal, const std::string& aPlatformName, const wkf::ZoneSetData& aData)
   {
      if (aGlobal)
      {
         mDataContainer.AddZoneSet(aData);
      }
      mPlatformZoneData[aPlatformName].push_back(aData);
   };

   if (aMessage.GetMessageId() == rv::MsgCircularZone::cMESSAGE_ID)
   {
      rv::MsgCircularZone         msg  = static_cast<rv::MsgCircularZone&>(aMessage);
      const rv::CircularZoneInfo& info = msg.zoneInfo();
      wkf::ZoneSetData            data(info.commonInfo().zoneName(), info.commonInfo().parentPlatform());
      CircularZoneHandler(data, info);
      AddColors(data, info.commonInfo().lineColor(), info.commonInfo().fillColor());
      handleData(info.commonInfo().isGlobal(), info.commonInfo().parentPlatform(), data);
   }
   else if (aMessage.GetMessageId() == rv::MsgEllipticalZone::cMESSAGE_ID)
   {
      rv::MsgEllipticalZone         msg  = static_cast<rv::MsgEllipticalZone&>(aMessage);
      const rv::EllipticalZoneInfo& info = msg.zoneInfo();
      wkf::ZoneSetData              data(info.commonInfo().zoneName(), info.commonInfo().parentPlatform());
      EllipticalZoneHandler(data, info);
      AddColors(data, info.commonInfo().lineColor(), info.commonInfo().fillColor());
      handleData(info.commonInfo().isGlobal(), info.commonInfo().parentPlatform(), data);
   }
   else if (aMessage.GetMessageId() == rv::MsgSphericalZone::cMESSAGE_ID)
   {
      rv::MsgSphericalZone         msg  = static_cast<rv::MsgSphericalZone&>(aMessage);
      const rv::SphericalZoneInfo& info = msg.zoneInfo();
      wkf::ZoneSetData             data(info.commonInfo().zoneName(), info.commonInfo().parentPlatform());
      SphericalZoneHandler(data, info);
      AddColors(data, info.commonInfo().lineColor(), info.commonInfo().fillColor());
      handleData(info.commonInfo().isGlobal(), info.commonInfo().parentPlatform(), data);
   }
   else if (aMessage.GetMessageId() == rv::MsgPolygonalZone::cMESSAGE_ID)
   {
      rv::MsgPolygonalZone         msg  = static_cast<rv::MsgPolygonalZone&>(aMessage);
      const rv::PolygonalZoneInfo& info = msg.zoneInfo();
      wkf::ZoneSetData             data(info.commonInfo().zoneName(), info.commonInfo().parentPlatform());
      PolygonalZoneHandler(data, info);
      AddColors(data, info.commonInfo().lineColor(), info.commonInfo().fillColor());
      handleData(info.commonInfo().isGlobal(), info.commonInfo().parentPlatform(), data);
   }
   else if (aMessage.GetMessageId() == rv::MsgZoneSet::cMESSAGE_ID)
   {
      rv::MsgZoneSet   msg = static_cast<rv::MsgZoneSet&>(aMessage);
      wkf::ZoneSetData data(msg.zoneSetName(), msg.parentPlatform());
      for (const auto& it : msg.circularZones())
      {
         CircularZoneHandler(data, it);
      }
      for (const auto& it : msg.ellipticalZones())
      {
         EllipticalZoneHandler(data, it);
      }
      for (const auto& it : msg.sphericalZones())
      {
         SphericalZoneHandler(data, it);
      }
      for (const auto& it : msg.polygonalZones())
      {
         PolygonalZoneHandler(data, it);
      }
      AddColors(data, msg.lineColor(), msg.fillColor());
      handleData(msg.isGlobal(), msg.parentPlatform(), data);
   }
}

void MysticZoneBrowser::Plugin::ClearScenario(bool aFullReset)
{
   mDataContainer.ClearData();
}

void MysticZoneBrowser::Plugin::AddColors(wkf::ZoneSetData& aData, const rv::ColorF& aLineColor, const rv::ColorF& aFillColor)
{
   auto colorFToUtColor = [this](const rv::ColorF& aColor, bool aLine)
   {
      if (aColor.red() >= 0 && aColor.green() >= 0 && aColor.blue() >= 0 && aColor.alpha() >= 0)
      {
         return UtColor(aColor.red(), aColor.green(), aColor.blue(), aColor.alpha());
      }
      else // use the preferences instead
      {
         auto prefs = mPrefWidgetPtr->GetPreferenceObject();
         if (prefs->GetColorChoice() == wkf::ColorOption::cSINGLESELECT)
         {
            QColor defaultColor = (aLine) ? prefs->GetDefaultLineColor() : prefs->GetDefaultFillColor();
            return UtColor(defaultColor.redF(), defaultColor.greenF(), defaultColor.blueF(), defaultColor.alphaF());
         }
         else // if == wkf::ColorOption::cRANDOM
         {
            unsigned char alpha = (aLine) ? 255u : 63u; // lines and fills default to these values
            UtColor       retVal;
            retVal.Set(mRandom.Uniform(0, 255), mRandom.Uniform(0, 255), mRandom.Uniform(0, 255), alpha);
            return retVal;
         }
      }
   };

   aData.SetLineColor(colorFToUtColor(aLineColor, true));
   aData.SetFillColor(colorFToUtColor(aFillColor, false));
}

// Handles both reference platforms as well as zones drawn directly on the parent platform
void MysticZoneBrowser::Plugin::PlatformActiveChangedCB(wkf::Platform* aPlatformPtr, bool aActive)
{
   if (aActive)
   {
      if (mPlatformZoneData.count(aPlatformPtr->GetName()) > 0)
      {
         for (const auto& it : mPlatformZoneData.at(aPlatformPtr->GetName()))
         {
            mDataContainer.AddZoneSet(it);
         }
      }
      mDockWidgetPtr->PlatformAdded(aPlatformPtr->GetName());
   }
   else
   {
      mDataContainer.RemovePlatform(aPlatformPtr->GetName());
   }
}

void MysticZoneBrowser::Plugin::ApplyColorPrefs(wkf::ColorOption aColorChoice)
{
   if (aColorChoice == wkf::ColorOption::cRANDOM)
   {
      mDockWidgetPtr->ApplyRandomColorPreferences();
   }
   else if (aColorChoice == wkf::ColorOption::cSINGLESELECT)
   {
      mDockWidgetPtr->ApplySetColorPreferences(mPrefWidgetPtr->GetPreferenceObject()->GetDefaultLineColor(),
                                               mPrefWidgetPtr->GetPreferenceObject()->GetDefaultFillColor());
   }
}
