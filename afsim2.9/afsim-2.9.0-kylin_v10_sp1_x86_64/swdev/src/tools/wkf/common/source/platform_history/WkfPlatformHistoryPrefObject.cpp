// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfPlatformHistoryPrefObject.hpp"

#include <QSettings>

wkf::PlatformHistoryPrefObject::PlatformHistoryPrefObject(QObject* parent)
   : PrefObjectT<TraceLinesPrefData>(parent, "PlatformHistory")
   , mCurrentRibbonStyle(RibbonStyle())
{
}

const double wkf::PlatformHistoryPrefObject::GetWingRibbonWidthScale() const
{
   return mCurrentPrefs.mWingRibbonWidthScale;
}
const int wkf::PlatformHistoryPrefObject::GetWingRibbonDeathTransparency() const
{
   return mCurrentPrefs.mWingRibbonDeathTransparencyPercent;
}

const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonMainMaxColor() const
{
   return mCurrentRibbonStyle.mWingRibbonMainMaxColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonMainMinColor() const
{
   return mCurrentRibbonStyle.mWingRibbonMainMinColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonEdgeMaxColor() const
{
   return mCurrentRibbonStyle.mWingRibbonEdgeMaxColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonEdgeMinColor() const
{
   return mCurrentRibbonStyle.mWingRibbonEdgeMinColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonMainMaxFlippedColor() const
{
   return mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonMainMinFlippedColor() const
{
   return mCurrentRibbonStyle.mWingRibbonMainMinFlippedColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonEdgeMaxFlippedColor() const
{
   return mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetWingRibbonEdgeMinFlippedColor() const
{
   return mCurrentRibbonStyle.mWingRibbonEdgeMinFlippedColor;
}

const double wkf::PlatformHistoryPrefObject::GetWingRibbonHardMaxEdgeLimit() const
{
   return mCurrentRibbonStyle.mWingRibbonHardMaxEdgeLimit;
}
const double wkf::PlatformHistoryPrefObject::GetWingRibbonHardMinEdgeLimit() const
{
   return mCurrentRibbonStyle.mWingRibbonHardMinEdgeLimit;
}
const double wkf::PlatformHistoryPrefObject::GetWingRibbonHardMaxMainLimit() const
{
   return mCurrentRibbonStyle.mWingRibbonHardMaxMainLimit;
}
const double wkf::PlatformHistoryPrefObject::GetWingRibbonHardMinMainLimit() const
{
   return mCurrentRibbonStyle.mWingRibbonHardMinMainLimit;
}
const double wkf::PlatformHistoryPrefObject::GetWingRibbonHardEdgeWidth() const
{
   return mCurrentRibbonStyle.mWingRibbonHardEdgeWidth;
}
const double wkf::PlatformHistoryPrefObject::GetWingRibbonSoftEdgeWidth() const
{
   return mCurrentRibbonStyle.mWingRibbonSoftEdgeWidth;
}
const std::map<std::string, QColor>& wkf::PlatformHistoryPrefObject::GetBehaviorColorMap() const
{
   return mCurrentPrefs.mBehaviorTraceLineColor;
}
const QColor& wkf::PlatformHistoryPrefObject::GetBehaviorDefaultColor() const
{
   return mCurrentPrefs.mBehaviorDefaultColor;
}

const wkf::InterpolationLimitsMethod wkf::PlatformHistoryPrefObject::GetWingRibbonEdgeLimitType() const
{
   return mCurrentRibbonStyle.mWingRibbonEdgeLimitType;
}
const wkf::InterpolationLimitsMethod wkf::PlatformHistoryPrefObject::GetWingRibbonMainLimitType() const
{
   return mCurrentRibbonStyle.mWingRibbonMainLimitType;
}
const wkf::TrackableStateValue wkf::PlatformHistoryPrefObject::GetWingRibbonEdgeStateBeingTracked() const
{
   return mCurrentRibbonStyle.mWingRibbonEdgeStateBeingTracked;
}
const wkf::TrackableStateValue wkf::PlatformHistoryPrefObject::GetWingRibbonMainStateBeingTracked() const
{
   return mCurrentRibbonStyle.mWingRibbonMainStateBeingTracked;
}

void wkf::PlatformHistoryPrefObject::Apply()
{
   if (mTraceLineColorChanged)
   {
      if (mCurrentPrefs.mTraceLineColor == eTRACE_BEHAVIOR)
      {
         emit TraceLineBehaviorColorChanged(mCurrentPrefs.mTraceLineColor, mCurrentPrefs.mBehaviorTraceLineColor);
      }
      else
      {
         emit TraceLineColorationChanged(mCurrentPrefs.mTraceLineColor, mCurrentPrefs.mTraceLineColorationList);
      }
      mTraceLineColorChanged = false;
   }
   if (mTraceLineLengthChanged)
   {
      emit TraceLineLengthChanged(mCurrentPrefs.mTraceLineLength);
      mTraceLineLengthChanged = false;
   }
   if (mTraceLineWidthChanged)
   {
      emit TraceLineWidthChanged(mCurrentPrefs.mTraceLineWidth);
      mTraceLineWidthChanged = false;
   }
   if (mWingRibbonsLengthChanged)
   {
      emit WingRibbonsLengthChanged(mCurrentPrefs.mWingRibbonsLength);
      mWingRibbonsLengthChanged = false;
   }
   if (mWingRibbonWidthScaleChanged)
   {
      emit WingRibbonWidthScaleFieldChanged(mCurrentPrefs.mWingRibbonWidthScale);
      mWingRibbonWidthScaleChanged = false;
   }
   if (mWingRibbonDeathTransparencyPercentChanged)
   {
      emit DeathTransparencySliderChanged(mCurrentPrefs.mWingRibbonDeathTransparencyPercent);
      mWingRibbonDeathTransparencyPercentChanged = false;
   }
   if (mColorSchemeChanged)
   {
      emit ColorSchemeChanged();
      mColorSchemeChanged = false;
   }
}

void wkf::PlatformHistoryPrefObject::SetPreferenceDataP(const TraceLinesPrefData& aPrefData)
{
   mTraceLineColorChanged       = ((mCurrentPrefs.mTraceLineColor != aPrefData.mTraceLineColor) ||
                             (aPrefData.mTraceLineColorationList != mCurrentPrefs.mTraceLineColorationList) ||
                             (aPrefData.mBehaviorTraceLineColor != mCurrentPrefs.mBehaviorTraceLineColor) ||
                             (aPrefData.mBehaviorDefaultColor != mCurrentPrefs.mBehaviorDefaultColor));
   mTraceLineLengthChanged      = mCurrentPrefs.mTraceLineLength != aPrefData.mTraceLineLength;
   mTraceLineWidthChanged       = mCurrentPrefs.mTraceLineWidth != aPrefData.mTraceLineWidth;
   mWingRibbonsLengthChanged    = mCurrentPrefs.mWingRibbonsLength != aPrefData.mWingRibbonsLength;
   mWingRibbonWidthScaleChanged = mCurrentPrefs.mWingRibbonWidthScale != aPrefData.mWingRibbonWidthScale;
   mWingRibbonDeathTransparencyPercentChanged =
      mCurrentPrefs.mWingRibbonDeathTransparencyPercent != aPrefData.mWingRibbonDeathTransparencyPercent;
   mColorSchemeChanged = mCurrentPrefs.mColorScheme != aPrefData.mColorScheme;

   SetCurrentWingRibbonColorScheme(aPrefData.mColorScheme);
   BasePrefObject::SetPreferenceDataP(aPrefData);
}

wkf::TraceLinesPrefData wkf::PlatformHistoryPrefObject::ReadSettings(QSettings& aSettings) const
{
   TraceLinesPrefData pData;
   pData.mTraceLineColor =
      static_cast<TraceStateValue>(aSettings.value("tracelineColor", mDefaultPrefs.mTraceLineColor).toInt());
   int asize = aSettings.beginReadArray("tracelineColorList");
   if (asize <= 0)
   {
      pData.mTraceLineColorationList = mDefaultPrefs.mTraceLineColorationList;
   }
   else
   {
      pData.mTraceLineColorationList.clear();
      for (int i = 0; i < asize; ++i)
      {
         aSettings.setArrayIndex(i);
         pData.mTraceLineColorationList.push_back(aSettings.value("value").value<QColor>());
      }
   }

   aSettings.endArray();

   aSettings.beginGroup("BehaviorColor");
   QStringList keys = aSettings.childKeys();
   for (QString key : keys)
   {
      pData.mBehaviorTraceLineColor[key.toStdString()] = aSettings.value(key).value<QColor>();
   }
   aSettings.endGroup();
   pData.mBehaviorDefaultColor =
      aSettings.value("behaviorDefaultColor", mDefaultPrefs.mBehaviorDefaultColor).value<QColor>();

   pData.mTraceLineLength      = aSettings.value("tracelineLength", mDefaultPrefs.mTraceLineLength).toDouble();
   pData.mTraceLineWidth       = aSettings.value("tracelineWidth", mDefaultPrefs.mTraceLineWidth).toUInt();
   pData.mWingRibbonsLength    = aSettings.value("wingRibbonsLength", mDefaultPrefs.mWingRibbonsLength).toDouble();
   pData.mWingRibbonWidthScale = aSettings.value("WidthScale", mDefaultPrefs.mWingRibbonWidthScale).toDouble();
   pData.mWingRibbonDeathTransparencyPercent =
      aSettings.value("DeathTransparencyPercent", mDefaultPrefs.mWingRibbonDeathTransparencyPercent).toInt();
   pData.mColorScheme =
      static_cast<WingRibbonStyle>(aSettings.value("WingRibbonStyle", mDefaultPrefs.mColorScheme).toInt());
   return pData;
}

void wkf::PlatformHistoryPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("tracelineColor", static_cast<int>(mCurrentPrefs.mTraceLineColor));
   aSettings.beginWriteArray("tracelineColorList", static_cast<int>(mCurrentPrefs.mTraceLineColorationList.size()));
   int i = 0;
   for (auto color : mCurrentPrefs.mTraceLineColorationList)
   {
      aSettings.setArrayIndex(i);
      aSettings.setValue("value", color);
      ++i;
   }
   aSettings.endArray();

   aSettings.remove("BehaviorColor");
   aSettings.beginGroup("BehaviorColor");
   std::map<std::string, QColor>::const_iterator b = mCurrentPrefs.mBehaviorTraceLineColor.begin();
   while (b != mCurrentPrefs.mBehaviorTraceLineColor.end())
   {
      aSettings.setValue(QString::fromStdString(b->first), b->second);
      ++b;
   }
   aSettings.endGroup();
   aSettings.setValue("behaviorDefaultColor", mCurrentPrefs.mBehaviorDefaultColor);

   aSettings.setValue("tracelineLength", mCurrentPrefs.mTraceLineLength);
   aSettings.setValue("tracelineWidth", mCurrentPrefs.mTraceLineWidth);
   aSettings.setValue("wingRibbonsLength", mCurrentPrefs.mWingRibbonsLength);
   aSettings.setValue("WidthScale", mCurrentPrefs.mWingRibbonWidthScale);
   aSettings.setValue("DeathTransparencyPercent", mCurrentPrefs.mWingRibbonDeathTransparencyPercent);
   aSettings.setValue("WingRibbonStyle", static_cast<int>(mCurrentPrefs.mColorScheme));
}

void wkf::PlatformHistoryPrefObject::SetCurrentWingRibbonColorScheme(WingRibbonStyle aWingRibbonStyle)
{
   switch (aWingRibbonStyle)
   {
   case eTEAM:
      mCurrentRibbonStyle.mWingRibbonMainMaxColor = QColor(240, 240, 240, 128); // Should not matter when in team mode.
      mCurrentRibbonStyle.mWingRibbonMainMinColor = mCurrentRibbonStyle.mWingRibbonMainMaxColor;
      mCurrentRibbonStyle.mWingRibbonEdgeMaxColor = QColor(128, 128, 128, 255); // Gray
      mCurrentRibbonStyle.mWingRibbonEdgeMinColor = mCurrentRibbonStyle.mWingRibbonEdgeMaxColor;
      mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor =
         mCurrentRibbonStyle.mWingRibbonMainMaxColor.darker(); // Should not matter when in team mode.
      mCurrentRibbonStyle.mWingRibbonMainMinFlippedColor = mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor;
      mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor =
         mCurrentRibbonStyle.mWingRibbonEdgeMaxColor.darker(); // Darker version of other side
      mCurrentRibbonStyle.mWingRibbonEdgeMinFlippedColor   = mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor;
      mCurrentRibbonStyle.mWingRibbonHardMaxEdgeLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMinEdgeLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMaxMainLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMinMainLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardEdgeWidth         = 1;
      mCurrentRibbonStyle.mWingRibbonSoftEdgeWidth         = 1;
      mCurrentRibbonStyle.mWingRibbonEdgeLimitType         = eHARD_LIMITS;
      mCurrentRibbonStyle.mWingRibbonMainLimitType         = eHARD_LIMITS;
      mCurrentRibbonStyle.mWingRibbonEdgeStateBeingTracked = eTRACK_NONE;
      mCurrentRibbonStyle.mWingRibbonMainStateBeingTracked = eTEAM_COLOR;
      break;
   case eGREEN:
      mCurrentRibbonStyle.mWingRibbonMainMaxColor = QColor(50, 220, 50, 128); // bright green
      mCurrentRibbonStyle.mWingRibbonMainMinColor = mCurrentRibbonStyle.mWingRibbonMainMaxColor;
      mCurrentRibbonStyle.mWingRibbonEdgeMaxColor = QColor(32, 32, 32, 255); // Nearly black
      mCurrentRibbonStyle.mWingRibbonEdgeMinColor = mCurrentRibbonStyle.mWingRibbonEdgeMaxColor;
      mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor =
         mCurrentRibbonStyle.mWingRibbonMainMaxColor.darker(); // Darker version of other side
      mCurrentRibbonStyle.mWingRibbonMainMinFlippedColor = mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor;
      mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor =
         mCurrentRibbonStyle.mWingRibbonEdgeMaxColor.darker(); // Darker version of other side
      mCurrentRibbonStyle.mWingRibbonEdgeMinFlippedColor   = mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor;
      mCurrentRibbonStyle.mWingRibbonHardMaxEdgeLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMinEdgeLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMaxMainLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMinMainLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardEdgeWidth         = 1;
      mCurrentRibbonStyle.mWingRibbonSoftEdgeWidth         = 1;
      mCurrentRibbonStyle.mWingRibbonEdgeLimitType         = eHARD_LIMITS;
      mCurrentRibbonStyle.mWingRibbonMainLimitType         = eHARD_LIMITS;
      mCurrentRibbonStyle.mWingRibbonEdgeStateBeingTracked = eTRACK_NONE;
      mCurrentRibbonStyle.mWingRibbonMainStateBeingTracked = eTRACK_NONE;
      break;
   case eGRAY:
   default:
      mCurrentRibbonStyle.mWingRibbonMainMaxColor = QColor(192, 192, 192, 128); // Light gray
      mCurrentRibbonStyle.mWingRibbonMainMinColor = mCurrentRibbonStyle.mWingRibbonMainMaxColor;
      mCurrentRibbonStyle.mWingRibbonEdgeMaxColor = QColor(32, 32, 32, 255); // Nearly black
      mCurrentRibbonStyle.mWingRibbonEdgeMinColor = mCurrentRibbonStyle.mWingRibbonEdgeMaxColor;
      mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor =
         mCurrentRibbonStyle.mWingRibbonMainMaxColor.darker(); // Darker version of other side
      mCurrentRibbonStyle.mWingRibbonMainMinFlippedColor = mCurrentRibbonStyle.mWingRibbonMainMaxFlippedColor;
      mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor =
         mCurrentRibbonStyle.mWingRibbonEdgeMaxColor.darker(); // Darker version of other side
      mCurrentRibbonStyle.mWingRibbonEdgeMinFlippedColor   = mCurrentRibbonStyle.mWingRibbonEdgeMaxFlippedColor;
      mCurrentRibbonStyle.mWingRibbonHardMaxEdgeLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMinEdgeLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMaxMainLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardMinMainLimit      = 1;
      mCurrentRibbonStyle.mWingRibbonHardEdgeWidth         = 1;
      mCurrentRibbonStyle.mWingRibbonSoftEdgeWidth         = 1;
      mCurrentRibbonStyle.mWingRibbonEdgeLimitType         = eHARD_LIMITS;
      mCurrentRibbonStyle.mWingRibbonMainLimitType         = eHARD_LIMITS;
      mCurrentRibbonStyle.mWingRibbonEdgeStateBeingTracked = eTRACK_NONE;
      mCurrentRibbonStyle.mWingRibbonMainStateBeingTracked = eTRACK_NONE;
      break;
   }
}
