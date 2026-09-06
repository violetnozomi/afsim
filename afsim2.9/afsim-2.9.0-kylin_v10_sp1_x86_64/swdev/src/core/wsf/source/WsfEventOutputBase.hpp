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

#ifndef WSF_EVENT_OUTPUT_BASE
#define WSF_EVENT_OUTPUT_BASE

#include "wsf_export.h"

#include <fstream>
#include <memory>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfEventResult.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

namespace wsf
{
namespace event
{
namespace output
{
struct Data
{
   virtual bool ProcessInput(UtInput& aInput);
   virtual ~Data() = default;

   std::map<std::string, bool> mToggledEvents;
   std::string                 mFileName;
   bool                        mFlushOutput{false};
   Settings                    mSettings;
};

//! A simulation extension that serves as an abstract base class for observers which write an event output file
class WSF_EXPORT SimulationExtension : public WsfSimulationExtension
{
public:
   SimulationExtension(Data& aData)
      : mData(aData)
   {
   }
   bool PrepareExtension() override;

   //! Derived classes can override this method to be notified when
   //! the 'SimulationComplete' event is fired, but before EventOutput
   //! shuts down (i.e. closes its output stream).
   virtual void OnSimulationComplete(double aSimTime) {}

   bool OpenFile(const std::string& aFileName);

   //! Returns the name of the file in which the events are output
   const std::string& GetFileName() const { return mData.mFileName; }

   std::ostream& StreamRef() const { return *mCurrentStream; }

   bool IsEnabled(const std::string& aEventName) const;

   void AddEvent(const std::string& aEventName, std::unique_ptr<UtCallback> aCallbackPtr);

   //! Register an event that will be produced upon invocation of the specified Observer
   //! @tparam RESULT is the subclass of wsf::event::Result which will be printed
   //! @tparam Args are types passed by the Observer, which should be deduced from the aObserver argument
   //! @param aEventName is the name of the event as it will appear in the output
   //! @param aObserver  is the Observer whose invocation will trigger this event
   template<typename RESULT, typename... Args>
   void AddEvent(const std::string& aEventName, UtCallbackListN<void(Args...)>& aObserver)
   {
      auto fn = UtStd::Bind(&SimulationExtension::ObserverCallback<RESULT, Args...>, this);
      AddEvent(aEventName, aObserver.Connect(fn));
   }

   void AddEventAlias(const std::string& aAliasEventName, const std::string& aCurrentEventName);

   bool EnableOrDisableEvent(const std::string& aEventName, bool aEnable);

   void RegisterMessagePrinter(WsfStringId aMessageType, const Settings::MessagePrintFunction& aFunction);

   Settings& GetSettings() { return mData.mSettings; }

   //! Print an event result in the desired format. Must be implemented by derived classes.
   virtual void PrintEvent(const Result& aResult) const = 0;

   class EventGuard
   {
   public:
      EventGuard(SimulationExtension& aEventOutput)
         : mEventOutput(aEventOutput)
      {
         mEventOutput.EventEntry();
      }
      ~EventGuard() { mEventOutput.EventExit(); }

   private:
      SimulationExtension& mEventOutput;
   };

private:
   void AddedToSimulation() override;

   void EventEntry();
   void EventExit();

   //! Allows derived classes to define custom behavior when an event is enabled or disabled
   virtual void EnableOrDisableEventP(const std::string& aEventName, bool aEnable) {}

   void SetStream(std::ostream* aStreamPtr);
   bool StreamIsOpen() const { return mCurrentStream != nullptr; }

   //! @name Callbacks from the observer notification process.
   //@{
   //! Callback template that constructs and forwards arguments to a derived Result, and prints the Result.
   //! Not intended to be used directly, but invoked through the AddEvent function template.
   //! @tparam RESULT is the subclass of wsf::event::Result which will be printed
   //! @tparam Args are types forwarded to the Result constructor, which should be deduced from arguments
   //! @param  args are forwarded arguments
   template<typename RESULT, typename... Args>
   void ObserverCallback(Args&&... args)
   {
      EventGuard guard(*this);
      RESULT     result(std::forward<Args>(args)..., mData.mSettings);
      PrintEvent(result);
   }

   void SimulationStarting();

   void SimulationComplete(double aSimTime);

   //@}

   struct EventData
   {
      std::unique_ptr<UtCallback> mCallbackPtr{nullptr};
      bool                        mEnabled{false};
   };

   // The following maps a message name to the event
   using EventMap = std::map<std::string, EventData>;
   EventMap         mEvents;
   UtCallbackHolder mCallbacks;

   using EventAliases = std::map<std::string, std::string>;
   EventAliases         mEventAliases;
   std::ostream*        mCurrentStream{nullptr};
   std::ofstream        mFileStream;
   bool                 mIsInitialized{false};
   std::recursive_mutex mMutex;
   Data&                mData;
};

//! A base class template for scenario extensions that process "event output"
//! configuration and register a corresponding simulation extension.
//! @tparam DATA specifies the data type that stores the configuration,
//! which must derive from Data and can provide a ProcessInput function to read input.
//! @tparam SIM_EXTENSION specifies the simulation extension that is
//! registered, which must derive from SimulationExtension and whose constructor
//! must take a single argument of type DATA.
template<typename DATA, typename SIM_EXTENSION>
class ScenarioExtension : public WsfScenarioExtension
{
   static_assert(std::is_base_of<Data, DATA>::value, "Template type DATA must be subclass of wsf::event::output::Data");
   static_assert(std::is_base_of<SimulationExtension, SIM_EXTENSION>::value,
                 "Template type SIM_EXTENSION must be subclass of wsf::event::output::SimulationExtension");

public:
   ScenarioExtension()
      : mData(ut::make_unique<DATA>())
   {
   }
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

private:
   std::unique_ptr<Data> mData;
};

template<typename DATA, typename SIM_EXTENSION>
bool ScenarioExtension<DATA, SIM_EXTENSION>::ProcessInput(UtInput& aInput)
{
   bool myCommand{false};
   if (aInput.GetCommand() == GetExtensionName())
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      inputBlock.ProcessInput(mData.get());
   }
   return myCommand;
}

template<typename DATA, typename SIM_EXTENSION>
void ScenarioExtension<DATA, SIM_EXTENSION>::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<SIM_EXTENSION>(dynamic_cast<DATA&>(*mData)));
}

} // namespace output
} // namespace event
} // namespace wsf
#endif
