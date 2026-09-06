// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFEVENTMARKERDISPLAYINTERFACE_HPP
#define WKFEVENTMARKERDISPLAYINTERFACE_HPP

#include "wkf_common_export.h"

#include <map>
#include <vector>

#include "VaCallbackHolder.hpp"
#include "VaEntityPtr.hpp"
#include "VaScenario.hpp"
#include "WkfEventMarker.hpp"
#include "WkfEventMarkerPrefWidget.hpp"

namespace vespa
{
class VaEntity;
class VaPosition;
} // namespace vespa

namespace wkf
{
class Scenario;

class WKF_COMMON_EXPORT EventMarkerDisplayInterface : public QObject
{
   Q_OBJECT

public:
   EventMarkerDisplayInterface(QObject* aParent, EventMarkerPrefWidget* aPrefWidgetPtr);
   ~EventMarkerDisplayInterface() override = default;

   EventMarkerDisplayInterface(const EventMarkerDisplayInterface&) = delete;

   //! Takes ownership of aMarker.
   void AddMarker(vespa::EntityPtr<BaseEventMarker> aMarker, const vespa::VaPosition& aPos);

   //! Destroys markers that have expired.
   void CleanUp(double aSimTime);

   //! Destroys all event markers.
   void DeleteEventMarkers();

private:
   struct Marker
   {
      Marker() = default;
      Marker(double aCreationTime, vespa::EntityPtr<BaseEventMarker> aEntity);

      double                            mCreationTime = 0;
      vespa::EntityPtr<BaseEventMarker> mEntity       = nullptr;
   };

   using MarkerMap = std::map<std::string, std::vector<Marker>>;

   QColor     GetTypeColor(const std::string& aType) const;
   MarkerType GetMarkerType(const std::string& aType) const;
   QImage     GetImage(const std::string& aType) const;
   void       ReloadCustomModels();
   void       MarkerScaleChanged(float aScale);

   void ShowEventChanged(bool aState, const std::string& aEvent);
   void EventColorChanged(const QColor& aColor, const std::string& aEvent);
   void EventMarkerTypeChanged(MarkerType aMarkerType, const std::string& aEventType);
   void EventImageChanged(const QImage& aImage, const std::string& aEventType);

   void DeleteType(const std::string& aType);

   void StandardScenarioRemovedCB(wkf::Scenario* aScenarioPtr);

   EventMarkerPrefWidget* mPrefWidgetPtr;
   MarkerMap              mMarkers;

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf

#endif
