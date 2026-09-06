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
#include "RvPluginPlatformData.hpp"

#include <memory>

#include <QMenu>

#include "RvEventPipeMessages.hpp"
#include "RvPlatformDataInterface.hpp"
#include "RvPlatformDataPlotUpdater.hpp"
#include "RvPlatformDataUpdater.hpp"
#include "RvPlottingWidget.hpp"
#include "RvResultPlatform.hpp"
#include "UtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitTypes.hpp"
#include "WkfVtkEnvironment.hpp"
#include "data_ring/WkfAttachmentDataRing.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   RvPlatformData::Plugin,
   "Platform Data",
   "Gets platform data from the simulation and prepares it for display within the platform data display",
   "mystic")

RvPlatformData::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mInterfacePtr(new Interface(this))
{
   mDataToStringMap[eSIDE]           = "Side";
   mDataToStringMap[eTYPE]           = "Type";
   mDataToStringMap[eICON]           = "Icon";
   mDataToStringMap[eSPATIAL_DOMAIN] = "Spatial Domain";
   mDataToStringMap[eSTATUS]         = "Status";
   mDataToStringMap[eSTART_TIME]     = "Start Time";
   mDataToStringMap[eEND_TIME]       = "End Time";
   mDataToStringMap[eDAMAGE_FACTOR]  = "Damage Factor";
   mDataToStringMap[eFUEL_CAPACITY]  = "Fuel Capacity";
   mDataToStringMap[eFUEL_CURRENT]   = "Fuel";
   mDataToStringMap[eLATITUDE]       = "Latitude";
   mDataToStringMap[eLONGITUDE]      = "Longitude";
   mDataToStringMap[eALTITUDE_MSL]   = "Altitude MSL";
   mDataToStringMap[eX]              = "Location X";
   mDataToStringMap[eY]              = "Location Y";
   mDataToStringMap[eZ]              = "Location Z";
   mDataToStringMap[eYAW]            = "Heading";
   mDataToStringMap[ePITCH]          = "Pitch";
   mDataToStringMap[eROLL]           = "Roll";
   mDataToStringMap[ePSI]            = "Psi";
   mDataToStringMap[eTHETA]          = "Theta";
   mDataToStringMap[ePHI]            = "Phi";
   mDataToStringMap[eSPEED]          = "Speed";
   mDataToStringMap[eMACH]           = "Mach";
   mDataToStringMap[eVEL_N]          = "Velocity N";
   mDataToStringMap[eVEL_E]          = "Velocity E";
   mDataToStringMap[eVEL_D]          = "Velocity D";
   mDataToStringMap[eVEL_X]          = "Velocity X";
   mDataToStringMap[eVEL_Y]          = "Velocity Y";
   mDataToStringMap[eVEL_Z]          = "Velocity Z";
   mDataToStringMap[eACCELERATION]   = "Acceleration";
   mDataToStringMap[eACCEL_N]        = "Acceleration N";
   mDataToStringMap[eACCEL_E]        = "Acceleration E";
   mDataToStringMap[eACCEL_D]        = "Acceleration D";
   mDataToStringMap[eACCEL_X]        = "Acceleration X";
   mDataToStringMap[eACCEL_Y]        = "Acceleration Y";
   mDataToStringMap[eACCEL_Z]        = "Acceleration Z";

   wkf::PlatformDataTreeItem* infoparent = new wkf::PlatformDataTreeItem();
   infoparent->setText(0, "Info");
   infoparent->setData(0, wkf::DISPLAY_PRECEDENCE, 0);
   mTopLevelWidgets.push_back(infoparent);
   mStateItemPtr = new wkf::PlatformDataTreeItem();
   mStateItemPtr->setText(0, "State");
   mStateItemPtr->setData(0, wkf::DISPLAY_PRECEDENCE, 1);
   mTopLevelWidgets.push_back(mStateItemPtr);
   mPartsItemPtr = new wkf::PlatformDataTreeItem();
   mPartsItemPtr->setText(0, "Parts");
   mPartsItemPtr->setData(0, wkf::DISPLAY_PRECEDENCE, 2);
   mTopLevelWidgets.push_back(mPartsItemPtr);

   CreateItem(infoparent, eSIDE);
   CreateItem(infoparent, eTYPE);
   CreateItem(infoparent, eICON);
   CreateItem(infoparent, eSPATIAL_DOMAIN);
   CreateItem(infoparent, eSTATUS);
   CreateItem(infoparent, eSTART_TIME);
   CreateItem(infoparent, eEND_TIME);
   CreateItem(infoparent, eFUEL_CAPACITY);
   CreateItem(infoparent, eFUEL_CURRENT);

   mCategoriesItemPtr = new wkf::PlatformDataTreeItem();
   mCategoriesItemPtr->setText(0, "Categories");
   infoparent->addChild(mCategoriesItemPtr);

   CreateItem(mStateItemPtr, eDAMAGE_FACTOR);
   CreateItem(mStateItemPtr, eLATITUDE);
   CreateItem(mStateItemPtr, eLONGITUDE);
   CreateItem(mStateItemPtr, eALTITUDE_MSL);
   CreateItem(mStateItemPtr, eX);
   CreateItem(mStateItemPtr, eY);
   CreateItem(mStateItemPtr, eZ);
   CreateItem(mStateItemPtr, eYAW);
   CreateItem(mStateItemPtr, ePITCH);
   CreateItem(mStateItemPtr, eROLL);
   CreateItem(mStateItemPtr, ePSI);
   CreateItem(mStateItemPtr, eTHETA);
   CreateItem(mStateItemPtr, ePHI);
   CreateItem(mStateItemPtr, eSPEED);
   CreateItem(mStateItemPtr, eMACH);
   CreateItem(mStateItemPtr, eVEL_N);
   CreateItem(mStateItemPtr, eVEL_E);
   CreateItem(mStateItemPtr, eVEL_D);
   CreateItem(mStateItemPtr, eVEL_X);
   CreateItem(mStateItemPtr, eVEL_Y);
   CreateItem(mStateItemPtr, eVEL_Z);
   CreateItem(mStateItemPtr, eACCELERATION);
   CreateItem(mStateItemPtr, eACCEL_N);
   CreateItem(mStateItemPtr, eACCEL_E);
   CreateItem(mStateItemPtr, eACCEL_D);
   CreateItem(mStateItemPtr, eACCEL_X);
   CreateItem(mStateItemPtr, eACCEL_Y);
   CreateItem(mStateItemPtr, eACCEL_Z);

   QMenu* toolsMenuPtr = wkfEnv.GetMainWindow()->GetMenuByName("Tools");
   if (toolsMenuPtr)
   {
      wkf::Action* plotAction = new wkf::Action(QIcon::fromTheme("plot"), "Plotting...", wkfEnv.GetMainWindow());
      connect(plotAction, &QAction::triggered, this, &Plugin::MenuPlot);
      toolsMenuPtr->addAction(plotAction);
   }

   // Register platform updaters with Environment
   wkfEnv.RegisterUpdater<DamageFactorUpdater>(*this);
   wkfEnv.RegisterUpdater<PositionUpdater>(*this);
   wkfEnv.RegisterUpdater<LatitudeUpdater>(*this);
   wkfEnv.RegisterUpdater<LongitudeUpdater>(*this);
   wkfEnv.RegisterUpdater<AltitudeUpdater>(*this);
   wkfEnv.RegisterUpdater<HeadingUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchUpdater>(*this);
   wkfEnv.RegisterUpdater<RollUpdater>(*this);
   wkfEnv.RegisterUpdater<SpeedUpdater>(*this);
   wkfEnv.RegisterUpdater<MachUpdater>(*this);
   wkfEnv.RegisterUpdater<PlatformTypeUpdater>(*this);
   wkfEnv.RegisterUpdater<PlatformSideUpdater>(*this);
   wkfEnv.RegisterUpdater<PlatformIconUpdater>(*this);
   wkfEnv.RegisterUpdater<PlatformDomainUpdater>(*this);
   wkfEnv.RegisterUpdater<PlatformStatusUpdater>(*this);
   wkfEnv.RegisterUpdater<AccelerationUpdater>(*this);

   mDataRingOption = RegisterOption(nullptr, "Data Rings")->type();
}

void RvPlatformData::Plugin::MenuPlot()
{
   QString xAxis = "Time";
   QString yAxis = "Altitude MSL";

   rv::PlottingWidget* plotWidget =
      vaEnv.GetFactory()->CreateDockWidget<rv::PlottingWidget>("PlottingWidget", wkfEnv.GetMainWindow()->centralWidget());

   // Find the updater
   std::unique_ptr<rv::PlotUpdater> updaterPtr = GetPlotUpdater(xAxis, yAxis, wkf::PlatformGrouping(0, ""), "");
   if (updaterPtr != nullptr)
   {
      plotWidget->SetUpdater(std::move(updaterPtr));
   }

   plotWidget->setFloating(true);
   plotWidget->show();
}

std::unique_ptr<wkf::Updater> RvPlatformData::Plugin::GetUpdater(const QString&     aPlatformName,
                                                                 const QString&     aDatum,
                                                                 const unsigned int aContext) const
{
   if (aDatum == "Start Time")
   {
      return wkf::make_updater<StartTimeUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == "End Time")
   {
      return wkf::make_updater<EndTimeUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Damage Factor")
   {
      return wkf::make_updater<DamageFactorUpdater>(aPlatformName.toStdString());
   }
   else if ((aDatum == "Latitude") || (aDatum == "Longitude"))
   {
      return wkfEnv.GetUpdater("Position", aPlatformName.toStdString());
   }
   else if (aDatum == "Altitude MSL")
   {
      return wkfEnv.GetUpdater("Altitude", aPlatformName.toStdString());
   }
   else if (aDatum == "Location X")
   {
      return wkf::make_updater<LocationX_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Location Y")
   {
      return wkf::make_updater<LocationY_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Location Z")
   {
      return wkf::make_updater<LocationZ_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Psi")
   {
      return wkf::make_updater<PsiUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Theta")
   {
      return wkf::make_updater<ThetaUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Phi")
   {
      return wkf::make_updater<PhiUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Velocity N")
   {
      return wkf::make_updater<VelocityN_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Velocity E")
   {
      return wkf::make_updater<VelocityE_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Velocity D")
   {
      return wkf::make_updater<VelocityD_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Velocity X")
   {
      return wkf::make_updater<VelocityX_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Velocity Y")
   {
      return wkf::make_updater<VelocityY_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Velocity Z")
   {
      return wkf::make_updater<VelocityZ_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Mach")
   {
      return wkf::make_updater<MachUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Acceleration N")
   {
      return wkf::make_updater<AccelerationN_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Acceleration E")
   {
      return wkf::make_updater<AccelerationE_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Acceleration D")
   {
      return wkf::make_updater<AccelerationD_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Acceleration X")
   {
      return wkf::make_updater<AccelerationX_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Acceleration Y")
   {
      return wkf::make_updater<AccelerationY_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == "Acceleration Z")
   {
      return wkf::make_updater<AccelerationZ_Updater>(aPlatformName.toStdString());
   }
   else
   {
      return wkfEnv.GetUpdater(aDatum, aPlatformName.toStdString());
   }
   // we currently don't have updaters for categories and parts (because they are dynamic in number)
   // note that the time updaters don't use the formatting preferences and the altitude updaters
   // don't switch between low and high units
}

void RvPlatformData::Plugin::ResetOptionStates()
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      auto platformMap = scenario->GetIndexPlatformMap();
      for (auto& platformPair : platformMap)
      {
         auto rings = platformPair.second->FindAttachmentsOfType<wkf::AttachmentDataRing>();
         for (auto& ring : rings)
         {
            ring->SetStateVisibility(true);
         }
      }
   }
}

void RvPlatformData::Plugin::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatform)
{
   auto rings = aPlatform->FindAttachmentsOfType<wkf::AttachmentDataRing>();
   for (auto ring : rings)
   {
      ring->SetStateVisibility(aState);
   }
}

Qt::CheckState RvPlatformData::Plugin::GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatform) const
{
   Qt::CheckState state = Qt::Checked;
   auto           rings = aPlatform->FindAttachmentsOfType<wkf::AttachmentDataRing>();
   for (auto ring : rings)
   {
      if (ring->GetStateVisibility() == false)
      {
         state = Qt::Unchecked;
         break;
      }
   }
   return state;
}

void RvPlatformData::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   std::vector<wkf::AttachmentDataRing*> rings = aEntityPtr->FindAttachmentsOfType<wkf::AttachmentDataRing>();
   if (!rings.empty())
   {
      QMenu* removeRingMenu = aMenu->addMenu("Remove Data Rings");
      for (const auto& ring : rings)
      {
         wkf::Updater* updater = ring->GetUpdater();
         if (updater != nullptr)
         {
            QAction* ringAction =
               new QAction(QIcon::fromTheme("data_ring"), "Remove " + updater->GetUpdaterName() + " Ring", removeRingMenu);

            unsigned int ringId    = ring->GetUniqueId();
            int          ringLevel = ring->GetLevel();
            connect(ringAction,
                    &QAction::triggered,
                    [aEntityPtr, ringId, ringLevel]()
                    {
                       aEntityPtr->RemoveAttachment(ringId);

                       for (const auto& ring : aEntityPtr->FindAttachmentsOfType<wkf::AttachmentDataRing>())
                       {
                          if (ring->GetLevel() > ringLevel)
                          {
                             ring->SetLevel(ring->GetLevel() - 1);
                          }
                       }
                    });
            removeRingMenu->addAction(ringAction);
         }
      }

      QAction* removeAllAction = new QAction(QIcon::fromTheme("data_ring"), "Remove all Rings", removeRingMenu);

      connect(removeAllAction,
              &QAction::triggered,
              [aEntityPtr, rings]()
              {
                 for (auto& ring : rings)
                 {
                    aEntityPtr->RemoveAttachment(ring->GetUniqueId());
                 }
              });
      removeRingMenu->addAction(removeAllAction);
   }
}

QList<wkf::PrefWidget*> RvPlatformData::Plugin::GetPreferencesWidgets() const
{
   return QList<wkf::PrefWidget*>();
}

QList<QTreeWidgetItem*> RvPlatformData::Plugin::GetPlatformData(const std::string& aPlatformName)
{
   mInterfacePtr->SetPlatformOfInterest(aPlatformName);
   mUpdatePosted = true;
   return mTopLevelWidgets;
}

void RvPlatformData::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
   mUpdatePosted = true;
}

void RvPlatformData::Plugin::RegularRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
   mUpdatePosted = true;
}

void RvPlatformData::Plugin::GuiUpdate()
{
   if (mUpdatePosted)
   {
      RvPlatformData::Interface::PlatformData data = mInterfacePtr->GetPlatformData();

      mStringDataWidgets[eSIDE]->setText(1, data.side);
      mStringDataWidgets[eTYPE]->setText(1, data.type);
      mStringDataWidgets[eICON]->setText(1, data.icon);
      mStringDataWidgets[eSPATIAL_DOMAIN]->setText(1, data.spatial_domain);
      mStringDataWidgets[eSTATUS]->setText(1, data.status);
      mUnitDataWidgets[eSTART_TIME]->SetValue(data.startTime);
      if (data.endTime >= 0)
      {
         mUnitDataWidgets[eEND_TIME]->SetValue(data.endTime);
         mUnitDataWidgets[eEND_TIME]->SetHidden(false);
      }
      else
      {
         mUnitDataWidgets[eEND_TIME]->SetHidden(true);
      }

      size_t childCount = mCategoriesItemPtr->childCount();
      for (size_t i = data.categories.size(); i < childCount; ++i)
      {
         QTreeWidgetItem* toDel = mCategoriesItemPtr->takeChild(static_cast<int>(i));
         delete toDel;
      }
      if (data.categories.empty())
      {
         mCategoriesItemPtr->SetHidden(true);
      }
      else
      {
         mCategoriesItemPtr->SetHidden(false);
         int i = 0;
         for (auto& it : data.categories)
         {
            wkf::PlatformDataTreeItem* item = dynamic_cast<wkf::PlatformDataTreeItem*>(mCategoriesItemPtr->child(i));
            if (nullptr == item)
            {
               item = new wkf::PlatformDataTreeItem();
               item->SetUnhideable();
               mCategoriesItemPtr->addChild(item);
            }
            item->setText(1, it);
            ++i;
         }
      }

      if (data.state_valid)
      {
         mStateItemPtr->SetHidden(false);
         mUnitDataWidgets[eDAMAGE_FACTOR]->SetValue(data.damageFactor);
         mUnitDataWidgets[eFUEL_CAPACITY]->SetValue(data.fuelCapacity);
         mUnitDataWidgets[eFUEL_CURRENT]->SetValue(data.fuelCurrent);
         mUnitDataWidgets[eLATITUDE]->SetValue(data.latitude);
         mUnitDataWidgets[eLONGITUDE]->SetValue(data.longitude);
         mUnitDataWidgets[eALTITUDE_MSL]->SetValue(data.altitude_m);
         mUnitDataWidgets[eX]->SetValue(data.x);
         mUnitDataWidgets[eY]->SetValue(data.y);
         mUnitDataWidgets[eZ]->SetValue(data.z);
         mUnitDataWidgets[eYAW]->SetValue(data.yaw);
         mUnitDataWidgets[ePITCH]->SetValue(data.pitch);
         mUnitDataWidgets[eROLL]->SetValue(data.roll);
         mUnitDataWidgets[ePSI]->SetValue(data.psi);
         mUnitDataWidgets[eTHETA]->SetValue(data.theta);
         mUnitDataWidgets[ePHI]->SetValue(data.phi);
         mUnitDataWidgets[eSPEED]->SetValue(data.speed_m);
         mUnitDataWidgets[eMACH]->SetPrecision(2, 'f');
         mUnitDataWidgets[eMACH]->SetValue(data.mach);
         mUnitDataWidgets[eVEL_N]->SetValue(data.vel_n);
         mUnitDataWidgets[eVEL_E]->SetValue(data.vel_e);
         mUnitDataWidgets[eVEL_D]->SetValue(data.vel_d);
         mUnitDataWidgets[eVEL_X]->SetValue(data.vel_x);
         mUnitDataWidgets[eVEL_Y]->SetValue(data.vel_y);
         mUnitDataWidgets[eVEL_Z]->SetValue(data.vel_z);
         mUnitDataWidgets[eACCELERATION]->SetValue(data.accel_m);
         mUnitDataWidgets[eACCEL_N]->SetValue(data.accel_n);
         mUnitDataWidgets[eACCEL_E]->SetValue(data.accel_e);
         mUnitDataWidgets[eACCEL_D]->SetValue(data.accel_d);
         mUnitDataWidgets[eACCEL_X]->SetValue(data.accel_x);
         mUnitDataWidgets[eACCEL_Y]->SetValue(data.accel_y);
         mUnitDataWidgets[eACCEL_Z]->SetValue(data.accel_z);

         // Configure each DataWidget to display only if appropriate for the platform of interest
         CheckDataCompatability(data);
      }
      else
      {
         mStateItemPtr->SetHidden(true);
      }

      std::map<QString, wkf::PlatformDataTreeItem*> groupmap;
      for (int i = 0; i < mPartsItemPtr->childCount(); ++i)
      {
         wkf::PlatformDataTreeItem* group =
            dynamic_cast<wkf::PlatformDataTreeItem*>(mPartsItemPtr->child(i)); // for every item in the parts gui

         if (group != nullptr)
         {
            if (data.parts.find(group->text(0)) == data.parts.end()) // if it isn't in the parts for the platform of interest
            {
               if (!group->isHidden()) // and it isn't hidden
               {
                  group->SetHidden(true); // hide it
               }
            }
            else // if it is in the parts for the platform of interest
            {
               groupmap[group->text(0)] = group; // record the pointer
               if (group->isHidden())
               {
                  group->SetHidden(false); // show it
               }
               size_t childCount = static_cast<size_t>(group->childCount());
               for (size_t j = data.parts[group->text(0)].size(); j < childCount; ++j)
               {
                  delete group->takeChild(0);
               } // trim the child items to the number needed
            }
         }
      }
      for (auto&& typeit : data.parts) // for every part needed
      {
         wkf::PlatformDataTreeItem* group;
         std::string                tname = typeit.first.toStdString();
         if (groupmap.find(typeit.first) == groupmap.end()) // if we haven't recorded a pointer for it
         {
            group = new wkf::PlatformDataTreeItem();
            group->setText(0, typeit.first);
            group->SetUnhideable();
            mPartsItemPtr->addChild(group);
            group->setExpanded(true); // add a new one
         }
         else
         {
            group = groupmap[typeit.first]; // otherwise, grab the recorded one
         }
         int i = 0;
         for (auto& jt : typeit.second) // for every part in the group
         {
            wkf::PlatformDataTreeItem* part = dynamic_cast<wkf::PlatformDataTreeItem*>(group->child(i));
            if (!part) // if there isn't an item we can use
            {
               part = new wkf::PlatformDataTreeItem();
               part->SetUnhideable();
               group->addChild(part); // make one
            }
            part->setText(0, jt.first);
            part->setText(1, jt.second); // set the data
            ++i;
         }
      }

      mUpdatePosted = false;
   }
}

void RvPlatformData::Plugin::CheckDataCompatability(Interface::PlatformData& aData)
{
   // Restrict these to display only on platforms that have valid data
   mUnitDataWidgets[eMACH]->SetHidden((!aData.machValid), wkf::PlatformDataTreeItem::PROGRAMMATIC);
   mUnitDataWidgets[eFUEL_CAPACITY]->SetHidden((!aData.fuelCapacityValid), wkf::PlatformDataTreeItem::PROGRAMMATIC);
}

void RvPlatformData::Plugin::CreateItem(QTreeWidgetItem* parent, ePlatformData aType)
{
   wkf::PlatformDataTreeItem* item = nullptr;
   switch (aType)
   {
   case eFUEL_CAPACITY:
   case eFUEL_CURRENT:
      mUnitDataWidgets[aType] = new wkf::MassTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eLATITUDE:
      mUnitDataWidgets[aType] = new wkf::LatitudeTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eLONGITUDE:
      mUnitDataWidgets[aType] = new wkf::LongitudeTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eALTITUDE_MSL:
      mUnitDataWidgets[aType] = new wkf::AltitudeTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eSPEED:
   case eVEL_N:
   case eVEL_E:
   case eVEL_D:
   case eVEL_X:
   case eVEL_Y:
   case eVEL_Z:
      mUnitDataWidgets[aType] = new wkf::SpeedTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eX:
   case eY:
   case eZ:
      mUnitDataWidgets[aType] = new wkf::LengthTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eYAW:
   case ePITCH:
   case eROLL:
   case ePSI:
   case eTHETA:
   case ePHI:
      mUnitDataWidgets[aType] = new wkf::AngleTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eACCELERATION:
   case eACCEL_N:
   case eACCEL_E:
   case eACCEL_D:
   case eACCEL_X:
   case eACCEL_Y:
   case eACCEL_Z:
      mUnitDataWidgets[aType] = new wkf::AccelerationTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eSTART_TIME:
   case eEND_TIME:
      mUnitDataWidgets[aType] = new wkf::TimeTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eTYPE:
   case eSIDE:
   case eICON:
   case eSPATIAL_DOMAIN:
   case eSTATUS:
      mStringDataWidgets[aType] = new wkf::PlatformDataTreeItem(parent);
      mStringDataWidgets[aType]->setText(0, mDataToStringMap[aType]);
      item = mStringDataWidgets[aType];
      break;
   case eDAMAGE_FACTOR:
   case eMACH:
      mUnitDataWidgets[aType] = new wkf::UnitlessTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   default:
      // error
      break;
   }
   switch (aType)
   {
   case eVEL_X:
   case eVEL_Y:
   case eVEL_Z:
   case eX:
   case eY:
   case eZ:
   case eACCEL_X:
   case eACCEL_Y:
   case eACCEL_Z:
      item->SetDefaultHidden();
   default:
      break;
   }
   if (item)
   {
      item->setData(0, wkf::UPDATER_ROLE, true);
      parent->addChild(item);
   }
}

std::unique_ptr<rv::PlotUpdater> RvPlatformData::Plugin::GetPlotUpdater(const QString&               aX,
                                                                        const QString&               aY,
                                                                        const wkf::PlatformGrouping& aGrouping,
                                                                        const QString&               aSubcategory) const
{
   return ut::make_unique<PlotUpdater>(aX, aY, aGrouping);
}
