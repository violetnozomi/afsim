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

#ifndef WSFCYBERTRIGGERMANAGER_HPP
#define WSFCYBERTRIGGERMANAGER_HPP

#include "wsf_cyber_export.h"

#include <unordered_map>

#include "triggers/WsfCyberTrigger.hpp"

class WsfSimulation;

namespace wsf
{
namespace cyber
{
namespace trigger
{

//! The cyber trigger manager handles the addition of global triggers, either via
//! scenario input or those created dynamically during a simulation run via the
//! the script context. Note that triggers created and used in the platform
//! context are natively handled with their coupling to the platform instance,
//! and are not maintained via the Manager.
class WSF_CYBER_EXPORT Manager final
{
public:
   Manager(WsfSimulation& aSimulation);
   ~Manager()                   = default;
   Manager(const Manager& aSrc) = delete;
   Manager& operator=(const Manager& aRhs) = delete;
   Manager(Manager&& aSrc)                 = default;
   Manager& operator=(Manager&& aRhs) = default;

   bool Initialize();

   Trigger* GetTrigger(const std::string& aTriggerName) const;

   //! @name Trigger Management Methods
   //@{
   bool AddManagedTrigger(double aSimTime, std::unique_ptr<Trigger> aTriggerPtr);
   bool RemoveManagedTrigger(const std::string& aTriggerName);
   //@}

private:
   using TriggerMap = std::unordered_map<std::string, std::unique_ptr<Trigger>>;

   //! Internally used method to initialize triggers for event scheduling.
   bool InitializeTrigger(double aSimTime, Trigger& aTrigger) const;

   //! Required non-const simulation reference for WsfScriptContext usage by associated triggers
   //! and event scheduling.
   WsfSimulation& mSimulation;

   //! All of the global triggers, either created via scenario input or via script at runtime.
   //! These are managed by a string indicating their name and type, as no two scripts of the
   //! same name and type may exist simultaneously.
   TriggerMap mMap{};
};

} // namespace trigger
} // namespace cyber
} // namespace wsf

#endif
