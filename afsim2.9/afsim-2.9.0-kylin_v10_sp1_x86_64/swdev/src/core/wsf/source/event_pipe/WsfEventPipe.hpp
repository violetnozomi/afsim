// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEVENTPIPE_HPP
#define WSFEVENTPIPE_HPP

#include <bitset>
#include <iosfwd>
#include <map>
#include <memory>
#include <unordered_map>

class UtPackSerializer;
#include "UtCallbackHolder.hpp"
#include "UtTextDocument.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfEventPipeEventIds.hpp"
#include "WsfEventPipeInput.hpp"
#include "WsfEventPipeInterface.hpp"
#include "WsfEventPipeLogger.hpp"
#include "WsfEventPipeOptions.hpp"
#include "WsfEventPipeOptionsCriteria.hpp"
#include "WsfScenarioExtension.hpp"

//! Scenario extension for event pipe.  Reads the input file.
class WSF_EXPORT WsfEventPipeExtension : public WsfScenarioExtension
{
public:
   static WsfEventPipeExtension* Find(const WsfScenario& aScenario);

   WsfEventPipeExtension();
   ~WsfEventPipeExtension() override = default;

   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

   // register a new option to be toggled by the 'enable' / 'disable' commands
   int RegisterEventOption(const std::string& aOptionName, bool aDefaultValue);

   //! Additional UtPack structures may be defined by other extensions.
   void AddSchema(const std::string& aSchemaText);

   //! Extensions generate callbacks when connecting a UtPackSerializer method
   //! to RegisterExtensionMessages. That callback is stored here.
   void AddCallback(std::unique_ptr<UtCallback> aCallbackPtr);

   const UtTextDocument& GetSchema() const { return mSchema; }

   std::vector<std::string>&       GetEventNames() { return mEventNames; }
   const std::vector<std::string>& GetEventNames() const { return mEventNames; }

   int                             GetEventId(const std::string& aName) const;
   const wsf::eventpipe::EventIds& GetEventIds() const { return mEventIds; }

   mutable UtCallbackListN<void(UtPackSerializer*)> RegisterExtensionMessages;

   //! This is used ONLY by WsfGrammarInterface::OutputForIDE to get events names and such for the IDE.
   using DetailSettingsMap = std::map<std::string, WsfEventPipeOptions>;
   const DetailSettingsMap& GetDetailSettings() const { return mData.mDetailSettings; }

private:
   WsfEventPipeInput   mData;
   WsfEventPipeOptions mDefaultPreset;
   bool                mDefaultPresetDefined;

   std::vector<std::string> mEventNames;
   wsf::eventpipe::EventIds mEventIds;

   // The UtPack schema from the built-in code as well as any extensions
   UtTextDocument mSchema;

   // Stores callbacks that are generated when connecting to RegisterExtensionMessages
   // This allows the callbacks to be cleaned up on destruction
   UtCallbackHolder mCallbacks;
};

#endif
