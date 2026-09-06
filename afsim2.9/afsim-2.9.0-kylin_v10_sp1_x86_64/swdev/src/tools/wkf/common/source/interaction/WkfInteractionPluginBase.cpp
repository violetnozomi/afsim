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
#include "WkfInteractionPluginBase.hpp"

#include "VaUtils.hpp"
#include "WkfAttachmentInteraction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfInteractionPrefObject.hpp"
#include "WkfInteractionPrefWidget.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::InteractionPluginBase::InteractionPluginBase(const QString&       aPluginName,
                                                  const size_t         aUniqueId,
                                                  const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueId, aRoles)
   , mPrefWidgetPtr(new InteractionPrefWidget)
{
   QTreeWidgetItem* parent   = RegisterOptionGroup(nullptr, "Interaction Lines");
   QTreeWidgetItem* incoming = RegisterOptionGroup(parent, "Incoming");
   mIdLookup[RegisterOption(incoming, "Detect", "in.Detect")->type()] =
      std::make_pair(InteractionPrefData::cDETECT, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Sensor Track", "in.Sensor Track")->type()] =
      std::make_pair(InteractionPrefData::cTRACK, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Local Track", "in.Local Track")->type()] =
      std::make_pair(InteractionPrefData::cLOCALTRACK, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Fire", "in.Fire")->type()] =
      std::make_pair(InteractionPrefData::cFIRE, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Kill", "in.Kill")->type()] =
      std::make_pair(InteractionPrefData::cKILL, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Jam Request", "in.Jam")->type()] =
      std::make_pair(InteractionPrefData::cJAM, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Comm", "in.Comm")->type()] =
      std::make_pair(InteractionPrefData::cMESSAGE, AttachmentInteraction::eINCOMING);
   mIdLookup[RegisterOption(incoming, "Task", "in.Task")->type()] =
      std::make_pair(InteractionPrefData::cTASK, AttachmentInteraction::eINCOMING);

   QTreeWidgetItem* outgoing = RegisterOptionGroup(parent, "Outgoing");
   mIdLookup[RegisterOption(outgoing, "Detect", "out.Detect")->type()] =
      std::make_pair(InteractionPrefData::cDETECT, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Sensor Track", "out.Sensor Track")->type()] =
      std::make_pair(InteractionPrefData::cTRACK, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Local Track", "out.Local Track")->type()] =
      std::make_pair(InteractionPrefData::cLOCALTRACK, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Fire", "out.Fire")->type()] =
      std::make_pair(InteractionPrefData::cFIRE, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Kill", "out.Kill")->type()] =
      std::make_pair(InteractionPrefData::cKILL, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Jam Request", "out.Jam")->type()] =
      std::make_pair(InteractionPrefData::cJAM, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Comm", "out.Comm")->type()] =
      std::make_pair(InteractionPrefData::cMESSAGE, AttachmentInteraction::eOUTGOING);
   mIdLookup[RegisterOption(outgoing, "Task", "out.Task")->type()] =
      std::make_pair(InteractionPrefData::cTASK, AttachmentInteraction::eOUTGOING);
}

wkf::InteractionPluginBase::~InteractionPluginBase() = default;

void wkf::InteractionPluginBase::ResetOptionStates()
{
   wkf::Scenario* scenPtr = vaEnv.GetStandardScenario();
   if (scenPtr)
   {
      const std::map<unsigned int, wkf::Platform*>& platMap = scenPtr->GetIndexPlatformMap();
      for (auto& it : platMap)
      {
         auto* interactionPtr = it.second->FindFirstAttachmentOfType<AttachmentInteraction>();
         if (interactionPtr)
         {
            it.second->RemoveAttachment(interactionPtr->GetUniqueId());
         }
      }
   }
}

void wkf::InteractionPluginBase::SetPlatformOptionState(int aType, bool aState, Platform* aPlatformPtr)
{
   auto* interactionPtr = aPlatformPtr->FindFirstAttachmentOfType<AttachmentInteraction>();
   if (!interactionPtr)
   {
      interactionPtr = vespa::make_attachment<AttachmentInteraction>(*aPlatformPtr,
                                                                     vaEnv.GetStandardViewer(),
                                                                     "WkfAttachmentInteraction",
                                                                     GetPrefObject());
      AddRegisteredTypes(*interactionPtr);

      vespa::VaAttachment::LoadAttachment(*interactionPtr);
   }

   interactionPtr->ShowInteractionOfType(mIdLookup[aType], aState);
}

Qt::CheckState wkf::InteractionPluginBase::GetPlatformOptionState(int aType, const Platform* aPlatformPtr) const
{
   const auto& it = mIdLookup.find(aType);
   if (it != mIdLookup.end())
   {
      auto* interactionPtr = aPlatformPtr->FindFirstAttachmentOfType<AttachmentInteraction>();
      if (interactionPtr)
      {
         return (interactionPtr->InteractionsOfTypeAreShown(it->second)) ? Qt::Checked : Qt::Unchecked;
      }
   }
   return Qt::Unchecked;
}

QList<wkf::PrefWidget*> wkf::InteractionPluginBase::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

QColor wkf::InteractionPluginBase::GetTypeColor(const std::string& aType) const
{
   return GetPrefObject()->GetColor(QString::fromStdString(aType));
}

const wkf::InteractionPrefObject* wkf::InteractionPluginBase::GetPrefObject() const
{
   return mPrefWidgetPtr->GetPreferenceObject();
}

void wkf::InteractionPluginBase::RegisterStateCard(const QString& aName, const QString& aIcon, const QString& aGroup)
{
   QTreeWidgetItem* parent    = RegisterOptionGroup(nullptr, "Interaction Lines");
   QTreeWidgetItem* incoming  = RegisterOptionGroup(parent, "Incoming");
   QTreeWidgetItem* groupitem = RegisterOptionGroup(incoming, aGroup);

   mIdLookup[RegisterOption(groupitem, aName, "in." + aName)->type()] =
      std::make_pair(aName.toStdString(), AttachmentInteraction::eINCOMING);
   mCardMap[aName] = aIcon;
   groupitem->setExpanded(false);
}

void wkf::InteractionPluginBase::RegisterInteraction(const QString& aName,
                                                     const QString& aGroup,
                                                     const QString& aHoverText,
                                                     const QColor&  aColor)

{
   QTreeWidgetItem* parent       = RegisterOptionGroup(nullptr, "Interaction Lines");
   QTreeWidgetItem* incoming     = RegisterOptionGroup(parent, "Incoming");
   QTreeWidgetItem* groupitem_in = RegisterOptionGroup(incoming, aGroup);

   mIdLookup[RegisterOption(groupitem_in, aName, "in." + aName)->type()] =
      std::make_pair(aName.toStdString(), AttachmentInteraction::eINCOMING);
   groupitem_in->setExpanded(false);

   QTreeWidgetItem* outgoing      = RegisterOptionGroup(parent, "Outgoing");
   QTreeWidgetItem* groupitem_out = RegisterOptionGroup(outgoing, aGroup);

   mIdLookup[RegisterOption(groupitem_out, aName, "out." + aName)->type()] =
      std::make_pair(aName.toStdString(), AttachmentInteraction::eOUTGOING);
   groupitem_out->setExpanded(false);

   mInteractionTypeMap[aName] = std::make_pair(aHoverText, aColor);
   mPrefWidgetPtr->GetPreferenceObject()->RegisterInteraction(aName, aGroup, aColor);
}

void wkf::InteractionPluginBase::AddRegisteredTypes(wkf::AttachmentInteraction& aInteraction)
{
   for (auto& card : mCardMap)
   {
      aInteraction.AddCard(card.first, card.second);
   }
   for (auto& interaction : mInteractionTypeMap)
   {
      aInteraction.AddInteractionType(interaction.first, interaction.second.first);
   }
}
