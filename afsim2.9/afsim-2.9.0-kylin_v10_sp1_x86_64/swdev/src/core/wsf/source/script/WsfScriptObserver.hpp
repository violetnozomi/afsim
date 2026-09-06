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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFSCRIPTOBSERVERCLASS_HPP
#define WSFSCRIPTOBSERVERCLASS_HPP

#include "wsf_export.h"

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "UtCallbackHolder.hpp"
#include "UtCompilerAttributes.hpp"
class UtInput;
class UtScript;
#include "UtScriptData.hpp"
#include "UtScriptDataPack.hpp"
#include "WsfApplication.hpp"
class WsfCallback;
class WsfFuel;
class WsfFuelingOperation;
class WsfImage;
class WsfLocalTrack;
class WsfMessage;
class WsfMover;
class WsfPlatform;
#include "WsfPlatformObserver.hpp"
class WsfProcessor;
#include "WsfScenarioExtension.hpp"
class WsfSensor;
class WsfSensorMode;
class WsfSensorResult;
class WsfScriptContext;
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"
class WsfTask;
class WsfTrack;
class WsfTrackId;

namespace wsf
{
namespace comm
{
class Address;
class Comm;
class Network;
class Result;

namespace router
{
class ProtocolInterface;
class Router;
} // namespace router
} // namespace comm
} // namespace wsf

class WSF_EXPORT WsfScriptObserverExtension : public WsfScenarioExtension
{
public:
   bool ProcessInput(UtInput& aInput) override;

   void SimulationCreated(WsfSimulation& aSimulation) override;

   using EventMap = std::map<std::string, std::set<std::string>>;
   EventMap mEnabledEvents;
};

//! WsfScriptObserver allows script methods to be registered
//! with observer callbacks
class WSF_EXPORT WsfScriptObserver : public WsfSimulationExtension
{
public:
   static WsfScriptObserver* Find(const WsfSimulation& aSimulation);

   WsfScriptObserver(WsfSimulation& aSimulation, const WsfScriptObserverExtension& aScenario);
   ~WsfScriptObserver() override = default;

   void AddedToSimulation() override;

   bool Initialize() override;
   void Start() override;

   using ScriptList = std::vector<UtScript*>;

   struct EventData
   {
      UtCallback* mCallbackPtr = nullptr;
      bool        mEnabled     = false;
      std::string mDefaultScriptName;
      std::string mExpectedArgumentTypes;
      ScriptList  mScriptList;
   };

   using EventList       = std::vector<EventData>;
   using EventToIndexMap = std::map<std::string, int>;

   WSF_DEPRECATED int AddEvent(const std::string&          aEventName,
                               std::unique_ptr<UtCallback> aCallbackPtr,
                               const std::string&          aDefaultScriptName,
                               const std::string&          aExpectedArgumentTypes)
   {
      return AddEventP(aEventName, std::move(aCallbackPtr), aDefaultScriptName, aExpectedArgumentTypes);
   }

   template<typename... Args>
   using Observer = UtCallbackListN<void(double, Args...)>;

   //! Register an event, providing a function that will pack the arguments for the script.
   //! @tparam EnableGuard            Enables the event guard for multi-threading safety (default false)
   //! @param  aEventName             The name of the event
   //! @param  aObserver              The observer which will trigger the script
   //! @param  aDefaultScriptName     The name of the user script to execute, if none is specified
   //! @param  aExpectedArgumentTypes The signature of the script
   //! @param  aArgPacker             Callable object to pack the arguments for the script. Should have signature:
   //!                                void fun(UtScriptDataPacker&, Args...)
   template<bool EnableGuard = false, typename... Args, typename ArgPacker>
   void AddEvent(const std::string& aEventName,
                 Observer<Args...>& aObserver,
                 const std::string& aDefaultScriptName,
                 const std::string& aExpectedArgumentTypes,
                 const ArgPacker&   aArgPacker);

   //! Register an event using the default argument packer, which will pack script arguments
   //! directly from the Observer in order (see UtScriptDataPack). Rather than a separate
   //! overload, this argument packer could have been provided as the default value for the
   //! ArgPacker in the 5-argument version above. However, a bug (#53837) in gcc versions < 4.9
   //! gives a parse error when expanding a parameter pack in a function argument default value.
   template<bool EnableGuard = false, typename... Args>
   void AddEvent(const std::string& aEventName,
                 Observer<Args...>& aObserver,
                 const std::string& aDefaultScriptName,
                 const std::string& aExpectedArgumentTypes)
   {
      AddEvent<EnableGuard>(aEventName,
                            aObserver,
                            aDefaultScriptName,
                            aExpectedArgumentTypes,
                            UtStd::Bind(&UtScriptDataPack<Args...>));
   }

   int GetEventIndex(const std::string& aEventName) const;

   ScriptList& GetScriptList(int aIndex) { return mEvents[aIndex].mScriptList; }

   const EventList& GetEvents() const { return mEvents; }

   const EventToIndexMap& GetEventIndexes() const { return mEventToIndex; }

   void EventEntry();

   void EventExit();

   // RAII type for beginning and ending event processing
   class EventGuard final
   {
   public:
      EventGuard() = default;
      explicit EventGuard(WsfScriptObserver& aScriptObserver)
         : mScriptObserver(&aScriptObserver)
      {
         mScriptObserver->EventEntry();
      }

      EventGuard(const EventGuard&) = delete;
      EventGuard& operator=(const EventGuard&) = delete;

      EventGuard(EventGuard&& aOther) { std::swap(mScriptObserver, aOther.mScriptObserver); }
      EventGuard& operator=(EventGuard&&) = delete;

      ~EventGuard()
      {
         if (mScriptObserver)
         {
            mScriptObserver->EventExit();
         }
      }

   private:
      WsfScriptObserver* mScriptObserver = nullptr;
   };

   bool EnableOrDisable(bool aEnable, const std::string& aCallbackName, const std::string& aScriptName);

   bool EnableOrDisable(bool aEnable, const std::string& aEventName, const std::string& aScriptName, EventData& aEventData);

   const EventToIndexMap GetEventMap() const { return mEventToIndex; }

   // Helper execution functions which find the global context from any other context
   void ExecuteScriptList(double aSimTime, const std::vector<UtScript*>& aScriptList);

   void ExecuteScriptList(double                        aSimTime,
                          const std::vector<UtScript*>& aScriptList,
                          UtScriptData&                 aScriptRetVal,
                          UtScriptDataList&             aScriptArgs);

protected:
   //! Copy constructor is declared but not defined to prevent use.
   WsfScriptObserver(const WsfScriptObserver& aSrc) = delete;

   //! Container to hold registered callbacks
   UtCallbackHolder mCallbacks;

private:
   int AddEventP(const std::string&          aEventName,
                 std::unique_ptr<UtCallback> aCallbackPtr,
                 const std::string&          aDefaultScriptName,
                 const std::string&          aExpectedArgumentTypes);

   bool                 mInitializeSuccess{false};
   EventList            mEvents;
   EventToIndexMap      mEventToIndex;
   std::recursive_mutex mMutex;

   const WsfScriptObserverExtension* mExtensionPtr{nullptr};
};

namespace detail
{
using EventGuard = WsfScriptObserver::EventGuard;
// Conditional factory method for creating an EventGuard
template<bool B>
CPP17_NODISCARD inline EventGuard make_event_guard(WsfScriptObserver&)
{
   return EventGuard{};
}
template<>
CPP17_NODISCARD inline EventGuard make_event_guard<true>(WsfScriptObserver& aScriptObserver)
{
   return EventGuard{aScriptObserver};
}
} // namespace detail

template<bool EnableGuard, typename... Args, typename ArgPacker>
void WsfScriptObserver::AddEvent(const std::string& aEventName,
                                 Observer<Args...>& aObserver,
                                 const std::string& aDefaultScriptName,
                                 const std::string& aExpectedArgumentTypes,
                                 const ArgPacker&   aArgPacker)
{
   auto callback = [=](double aSimTime, Args&&... args)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs{GetSimulation().GetApplication().GetScriptTypes()};
      auto               eg = detail::make_event_guard<EnableGuard>(*this);
      aArgPacker(scriptArgs, std::forward<Args>(args)...);
      if (!scriptArgs.mDataList.empty() || aExpectedArgumentTypes.empty())
      { // If expected arguments weren't packed, don't execute scripts
         ExecuteScriptList(aSimTime, GetScriptList(GetEventIndex(aEventName)), scriptRetVal, scriptArgs);
      }
   };
   AddEventP(aEventName, aObserver.Connect(callback), aDefaultScriptName, aExpectedArgumentTypes);
}

#endif
