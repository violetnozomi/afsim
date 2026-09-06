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
#include "MapHoverInfoPlugin.hpp"

#include <QToolTip>

#include "MapHoverInfoPrefObject.hpp"
#include "UtAirspeed.hpp"
#include "UtUnitTypes.hpp"
#include "VaAttachment.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfTrack.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "event_marker/WkfEventMarker.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(MapHoverInfo::Plugin,
                          "Map Hover Info",
                          "The Map Hover Info plugin is responsible for displaying a customizable pop-up when hovering "
                          "over an entity on the map.",
                          "all");

MapHoverInfo::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new PrefWidget)
   , mManagerPtr(nullptr)
{
   AddHoverToWidget(vaEnv.GetStandardWidget());
   mCallbacks.Add(wkf::Observer::StandardWidgetChanged.Connect(&Plugin::AddHoverToWidget, this));
}

QList<wkf::PrefWidget*> MapHoverInfo::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void MapHoverInfo::Plugin::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   if (mManagerPtr != nullptr && mManagerPtr->IsEnabled())
   {
      mManagerPtr->Enable(false);
      mManagerPtr->HandleHoverEvent();
      connect(aMenu, &QMenu::aboutToHide, [=]() { mManagerPtr->Enable(true); });
   }
}

void MapHoverInfo::Plugin::AddHoverToWidget(vespa::VaWidget* aWidget)
{
   if (aWidget)
   {
      if (aWidget->findChild<HoverManager*>() == nullptr)
      {
         mManagerPtr = new HoverManager(aWidget);
         aWidget->installEventFilter(mManagerPtr);
         aWidget->setStyle(new HoverStyle(aWidget));
         connect(mPrefWidgetPtr->GetPreferenceObject(),
                 &PrefObject::HoverPrefsChanged,
                 [this]
                 {
                    this->mManagerPtr->ClearUpdaters();
                    this->mManagerPtr->Enable(mPrefWidgetPtr->GetPreferenceObject()->GetShowTooltips());
                 });
      }
   }
}

bool MapHoverInfo::HoverManager::eventFilter(QObject* aWatched, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::ToolTip)
   {
      HandleHoverEvent();
   }
   return false;
}

void MapHoverInfo::HoverManager::HandleHoverEvent()
{
   vespa::VaHitList hoverList;
   int              x;
   int              y;
   mParent->GetViewer()->GetMousePosition(x, y);
   mParent->GetViewer()->PickFromScreen(hoverList, x, y, true);

   bool    valid = true;
   QString fullText;

   if (mEnabled)
   {
      if (!hoverList.empty())
      {
         const auto& hit          = *hoverList.begin(); // Remove this line if using the loop.
         mObject                  = hit;
         vespa::VaAttachment* att = hit.FindAttachment();
         if (nullptr != att)
         {
            fullText += QString::fromStdString(att->GetDescription(hoverList.begin()->SubId()));
         }
         else
         {
            vespa::VaEntity* ent = hit.FindEntity();
            if (nullptr != ent)
            {
               QString text = GetEntityString(ent, valid);
               if (valid)
               {
                  fullText += text;
               }
            }
         }
      }
   }

   if (!fullText.isEmpty())
   {
      mCursor = QCursor::pos();
      QToolTip::showText(mCursor, fullText, mParent, QRect(mCursor.x() - 4, mCursor.y() - 4, 9, 9));
      mTimer.start(500);
   }
   else // we could either be waiting for updaters, or there is no data, yet
   {
      QToolTip::hideText();
      if (valid)
      {
         // this looks weird, but let me explain.
         // warlock data may not be immediately available (threading), so we
         // don't want to delete updaters that we are waiting on...
         // meanwhile there are cases (alt-tab, or right click to context menu, for example)
         // which will result in zombie updaters.  By checking valid we are making sure that
         // it isn't waiting on data, and clearing the updaters if the tooltip
         // using them is gone.
         ClearUpdaters();
      }
      else
      {
         mCursor = QCursor::pos();
         mTimer.start(500);
      }
   }
}

MapHoverInfo::HoverManager::HoverManager(vespa::VaWidget* aParent)
   : QObject(aParent)
   , mParent(aParent)
{
   connect(&mTimer, &QTimer::timeout, this, &HoverManager::Revisit);
}

void MapHoverInfo::HoverManager::ClearUpdaters()
{
   mUpdaters.clear();
}

QString MapHoverInfo::HoverManager::GetEntityString(vespa::VaEntity* aEntity, bool& aValid)
{
   QString text;
   aValid = true;

   auto prefs = wkfEnv.GetPreferenceObject<PrefObject>();
   if (prefs)
   {
      text += "<p style='white-space:pre'>";

      if (prefs->GetShowNames())
      {
         text += "<b>" + QString::fromStdString(aEntity->GetName()) + "</b> \n";
      }

      // Handle platforms using wkf::Updater
      wkf::Platform* platform = dynamic_cast<wkf::Platform*>(aEntity);
      if (platform != nullptr)
      {
         text += GetPlatformString(platform, aValid);
      }
      else
      {
         wkf::Entity* entity = dynamic_cast<wkf::Entity*>(aEntity);
         if (entity != nullptr)
         {
            text += GetNonPlatformString(entity, aValid);
         }
      }
      text += "</p>";
   }
   return text;
}

void MapHoverInfo::HoverManager::GetUpdatersForEntity(const QStringList& aUpdaterNames, vespa::VaEntity* aEntity)
{
   if (aEntity->IsA_TypeOf<wkf::Platform>())
   {
      for (const auto& updaterName : aUpdaterNames)
      {
         auto updater = wkfEnv.GetUpdater(updaterName, aEntity->GetName());
         if (updater)
         {
            mUpdaters[aEntity].push_back(std::move(updater));
         }
      }
   }
}

QString MapHoverInfo::HoverManager::GetPlatformString(wkf::Platform* aPlatform, bool& aValid)
{
   QString text;
   auto    prefs = wkfEnv.GetPreferenceObject<PrefObject>();

   // If no Updaters have been received for the current Entity, request them
   if (mUpdaters.count(aPlatform) == 0)
   {
      GetUpdatersForEntity(prefs->GetPlatformUpdaterList(), aPlatform);
   }
   // Determine if there are any valid updaters for this Entity
   // Hover Info will display if there is at least 1 valid updater
   // If there are no valid updaters, only the platform name will display
   auto it = mUpdaters.find(aPlatform);
   if (it != mUpdaters.end()) // If there are updaters
   {
      aValid = false;
      for (auto& i : it->second)
      {
         aValid |= i->IsValid(); // Determine if any updaters are valid
      }
      if (aValid) // If there are valid updaters
      {
         bool first = true;

         for (auto& i : it->second)
         {
            if (i->IsValid()) // Add only the valid updaters to the print string
            {
               if (!first)
               {
                  text += "\n";
               }
               first = false;

               if (prefs->GetShowLabels())
               {
                  text += i->GetUpdaterName() + ": ";
               }
               text += i->GetValueString();
            }
         }
      }
   }

   return text;
}

QString MapHoverInfo::HoverManager::GetNonPlatformString(const wkf::Entity* aEntity, bool& aValid)
{
   QString text;
   auto    prefs = wkfEnv.GetPreferenceObject<PrefObject>();

   // Handle all other info using NamedInfo

   std::string label;
   std::string value;
   QStringList list;

   // Get hover info list.
   if (aEntity->IsA_TypeOf<wkf::Track>())
   {
      list = prefs->GetTrackUpdaterList();
   }
   else if (aEntity->IsA_TypeOf<wkf::PlatformEventMarker>())
   {
      list = prefs->GetPlatformEventList();
   }
   else if (aEntity->IsA_TypeOf<wkf::WeaponEventMarker>())
   {
      list = prefs->GetWeaponEventList();
   }

   bool first = true;
   // Eventually GetTrackUpdaterList() should return a list of Updater pointers.
   // Then this for loop would instead ask each Updater for the display string,
   // and we would not have this logic in this loop here.
   for (const auto& item : list)
   {
      if (aEntity->GetNamedInfo(item.toStdString(), label, value))
      {
         if (first)
         {
            first = false;
         }
         else
         {
            text += "\n";
         }

         if (prefs->GetShowLabels())
         {
            text += QString::fromStdString(label) + ": ";
         }
         text += QString::fromStdString(value);
      }
   }

   return text;
}

void MapHoverInfo::HoverManager::Revisit()
{
   QRect r(mCursor.x() - 4, mCursor.y() - 4, 9, 9);
   if (!r.contains(QCursor::pos()))
   {
      mTimer.stop();
      ClearUpdaters();
      QToolTip::hideText();
      return;
   }
   else
   {
      QString              fullText;
      bool                 valid = true;
      vespa::VaAttachment* att   = mObject.FindAttachment();
      if (nullptr != att)
      {
         fullText += QString::fromStdString(att->GetDescription(mObject.SubId()));
      }
      else
      {
         vespa::VaEntity* ent = mObject.FindEntity();
         if (nullptr != ent)
         {
            QString text = GetEntityString(ent, valid);
            if (valid)
            {
               fullText += text;
            }
         }
         else
         {
            mTimer.stop();
            ClearUpdaters();
            QToolTip::hideText();
            return;
         }
      }

      if (!fullText.isEmpty())
      {
         QToolTip::showText(mCursor, fullText, mParent, r);
      }
      else
      {
         mTimer.stop();
         ClearUpdaters();
         QToolTip::hideText();
      }
   }
}
