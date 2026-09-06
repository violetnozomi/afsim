// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfEventMarkerPrefObject.hpp"

#include <QDataStream>
#include <QSettings>

wkf::EventMarkerPrefData::EventMarkerPrefData()
{
   mEventData["DAMAGED"].mColor       = QColor(64, 64, 64);
   mEventData["REMOVED"].mColor       = QColor(196, 196, 196);
   mEventData["WEAPON_HIT"].mColor    = Qt::red;
   mEventData["WEAPON_MISSED"].mColor = Qt::yellow;
}

wkf::EventMarkerPrefObject::EventMarkerPrefObject(QObject* parent)
   : wkf::PrefObjectT<wkf::EventMarkerPrefData>(parent, cNAME)
   , mTimeoutChanged(false)
   , mMarkerScaleChanged(false)
   , mShowEventChanged()
   , mEventColorChanged()
   , mEventMarkerChanged()
   , mEventIconPathChanged()
{
}

void wkf::EventMarkerPrefObject::Apply()
{
   if (mTimeoutChanged)
   {
      emit TimeoutChanged(mCurrentPrefs.mTimeout);
      mTimeoutChanged = false;
   }
   if (mMarkerScaleChanged)
   {
      emit MarkerScaleChanged(mCurrentPrefs.mMarkerScale);
      mMarkerScaleChanged = false;
   }

   for (auto& etype : mCurrentPrefs.mEventData)
   {
      EventMarkerDetails& details = etype.second;
      // marker visibility
      bool& showChanged = mShowEventChanged[etype.first];
      if (showChanged)
      {
         emit ShowEventChanged(details.mShow, etype.first);
         showChanged = false;
      }

      // color
      bool& colorChanged = mEventColorChanged[etype.first];
      if (colorChanged)
      {
         emit EventColorChanged(details.mColor, etype.first);
         colorChanged = false;
      }

      // marker type
      bool& markerChanged = mEventMarkerChanged[etype.first];
      if (markerChanged)
      {
         emit EventMarkerTypeChanged(details.mMarkerType, etype.first);
         markerChanged = false;
      }

      // Icon
      bool& iconPathChanged = mEventIconPathChanged[etype.first];
      if (iconPathChanged)
      {
         details.mIcon = QImage(details.mIconPath);
         emit EventIconChanged(details.mIcon, etype.first);
         iconPathChanged = false;
      }
   }
}

void wkf::EventMarkerPrefObject::SetPreferenceDataP(const EventMarkerPrefData& aPrefData)
{
   mTimeoutChanged     = aPrefData.mTimeout != mCurrentPrefs.mTimeout;
   mMarkerScaleChanged = aPrefData.mMarkerScale != mCurrentPrefs.mMarkerScale;

   for (const auto& etype : mCurrentPrefs.mEventData)
   {
      const auto newDetails = aPrefData.mEventData.find(etype.first);
      if (newDetails == aPrefData.mEventData.end())
      {
         mShowEventChanged[etype.first]     = true;
         mEventColorChanged[etype.first]    = true;
         mEventMarkerChanged[etype.first]   = true;
         mEventIconPathChanged[etype.first] = true;
      }
      else
      {
         const auto& currentDetails = etype.second;

         mShowEventChanged[etype.first] = newDetails->second.mShow != currentDetails.mShow;
         // the 2nd condition is to ensure the color of custom icons doesn't change after color selector widget is modified
         mEventColorChanged[etype.first] =
            (newDetails->second.mColor != currentDetails.mColor) && (newDetails->second.mMarkerType != CUSTOM);
         mEventMarkerChanged[etype.first] = newDetails->second.mMarkerType != currentDetails.mMarkerType;
         mEventIconPathChanged[etype.first] =
            (newDetails->second.mIconPath != currentDetails.mIconPath) && (newDetails->second.mMarkerType == CUSTOM);
      }
   }
   EventMarkerPrefData toset = aPrefData;
   for (auto& pref : mRegisteredPrefs)
   {
      if (toset.mEventData.find(pref.first) == toset.mEventData.end())
      {
         toset.mEventData[pref.first] = pref.second;
      }
   }

   BasePrefObject::SetPreferenceDataP(toset);
}

wkf::EventMarkerPrefData wkf::EventMarkerPrefObject::ReadSettings(QSettings& aSettings) const
{
   EventMarkerPrefData pData;
   pData.mTimeout     = aSettings.value("timeout", mDefaultPrefs.mTimeout).toInt();
   pData.mMarkerScale = aSettings.value("markerScale", mDefaultPrefs.mMarkerScale).toFloat();

   for (const auto& defaultevent : mDefaultPrefs.mEventData)
   {
      QString tag       = QString::fromStdString(defaultevent.first);
      auto&   event     = pData.mEventData[tag.toStdString()];
      event.mShow       = aSettings.value("show" + tag, defaultevent.second.mShow).toBool();
      event.mColor      = aSettings.value(tag + "_color", defaultevent.second.mColor).value<QColor>();
      event.mMarkerType = aSettings.value(tag + "_markerType", defaultevent.second.mMarkerType).value<wkf::MarkerType>();
      event.mIconPath   = aSettings.value(tag + "_iconPath", defaultevent.second.mIconPath).toString();
      if (!event.mIconPath.isEmpty())
      {
         event.mIcon = QImage(event.mIconPath);
      }
   }
   for (const auto& pref : mRegisteredPrefs)
   {
      QString tag       = QString::fromStdString(pref.first);
      auto&   event     = pData.mEventData[tag.toStdString()];
      event.mShow       = aSettings.value("show" + tag, pref.second.mShow).toBool();
      event.mColor      = aSettings.value(tag + "_color", pref.second.mColor).value<QColor>();
      event.mMarkerType = aSettings.value(tag + "_markerType", pref.second.mMarkerType).value<wkf::MarkerType>();
      event.mIconPath   = aSettings.value(tag + "_iconPath", pref.second.mIconPath).toString();
      if (!event.mIconPath.isEmpty())
      {
         event.mIcon = QImage(event.mIconPath);
      }
   }

   // load registered markers
   return pData;
}

void wkf::EventMarkerPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("timeout", mCurrentPrefs.mTimeout);
   aSettings.setValue("markerScale", mCurrentPrefs.mMarkerScale);

   for (const auto& event : mCurrentPrefs.mEventData)
   {
      QString tag = QString::fromStdString(event.first);
      aSettings.setValue("show" + tag, event.second.mShow);
      aSettings.setValue(tag + "_color", event.second.mColor);
      aSettings.setValue(tag + "_markerType", event.second.mMarkerType);
      aSettings.setValue(tag + "_iconPath", event.second.mIconPath);
   }
}

void wkf::EventMarkerPrefObject::RegisterEvent(const std::string& aEvent, const QColor& aColor)
{
   mRegisteredPrefs[aEvent].mColor         = aColor;
   mCurrentPrefs.mEventData[aEvent].mColor = aColor;
   NewEventTypeRegistered(QString::fromStdString(aEvent), aColor);
}

std::set<std::string> wkf::EventMarkerPrefObject::GetEventTypes() const
{
   std::set<std::string> eventlist;

   for (auto& e : mCurrentPrefs.mEventData)
   {
      eventlist.insert(e.first);
   }

   return eventlist;
}
