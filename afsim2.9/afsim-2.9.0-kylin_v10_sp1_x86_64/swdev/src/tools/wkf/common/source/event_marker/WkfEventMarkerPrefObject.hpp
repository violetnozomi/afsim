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
#ifndef WKFEVENTMARKERPREFOBJECT_HPP
#define WKFEVENTMARKERPREFOBJECT_HPP

#include <array>
class QSettings;
#include "wkf_common_export.h"

#include "WkfEventMarker.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
enum MarkerType
{
   DOT,
   X,
   CUSTOM
};

struct EventMarkerDetails
{
   bool       mShow{false};
   QColor     mColor;
   MarkerType mMarkerType{DOT};
   QString    mIconPath;
   QImage     mIcon;
};

struct EventMarkerPrefData
{
   EventMarkerPrefData();

   int                                       mTimeout{30};
   float                                     mMarkerScale{1.0f};
   std::map<std::string, EventMarkerDetails> mEventData;

   std::vector<std::string> mHoverInfo = {"Summary"};
};

class WKF_COMMON_EXPORT EventMarkerPrefObject : public wkf::PrefObjectT<EventMarkerPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "EventMarkerPrefObject";

   EventMarkerPrefObject(QObject* parent = nullptr);

   bool IsSharedPreference() const final { return true; }

   int   GetTimeout() const { return mCurrentPrefs.mTimeout; }
   float GetMarkerScale() const { return mCurrentPrefs.mMarkerScale; }

   bool       GetShowEvent(const std::string& aEvent) const { return mCurrentPrefs.mEventData.at(aEvent).mShow; }
   QColor     GetEventColor(const std::string& aEvent) const { return mCurrentPrefs.mEventData.at(aEvent).mColor; }
   MarkerType GetMarkerType(const std::string& aEvent) const { return mCurrentPrefs.mEventData.at(aEvent).mMarkerType; }
   const QImage& GetEventImage(const std::string& aEvent) const { return mCurrentPrefs.mEventData.at(aEvent).mIcon; }
   const std::vector<std::string>& GetHoverInfo() const { return mCurrentPrefs.mHoverInfo; }

   void RegisterEvent(const std::string& aEvent, const QColor& aColor);

   std::set<std::string> GetEventTypes() const;
signals:
   void TimeoutChanged(int aTime);
   void MarkerScaleChanged(const float& aScale);

   void ShowEventChanged(bool aShow, const std::string& aEvent);
   void EventColorChanged(const QColor& aColor, const std::string& aEvent);
   void EventMarkerTypeChanged(MarkerType aMarkerType, const std::string& aEvent);
   void EventIconChanged(const QImage& aImage, const std::string& aEvent);
   void NewEventTypeRegistered(const QString& aNAme, const QColor& aDefaultColor);

private:
   void Apply() override;
   void SetPreferenceDataP(const EventMarkerPrefData& aPrefData) override;

   EventMarkerPrefData ReadSettings(QSettings& aSettings) const override;
   void                SaveSettingsP(QSettings& aSettings) const override;

   bool mTimeoutChanged;
   bool mMarkerScaleChanged;

   using EventChangedFlags = std::map<std::string, bool>;
   EventChangedFlags mShowEventChanged;
   EventChangedFlags mEventColorChanged;
   EventChangedFlags mEventMarkerChanged;
   EventChangedFlags mEventIconPathChanged;

   std::map<std::string, EventMarkerDetails> mRegisteredPrefs;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::MarkerType);
Q_DECLARE_METATYPE(wkf::EventMarkerPrefData)
#endif // WKFEVENTMARKERPREFOBJECT_HPP
