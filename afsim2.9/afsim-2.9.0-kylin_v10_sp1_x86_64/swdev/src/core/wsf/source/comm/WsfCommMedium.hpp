// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMMEDIUM_HPP
#define WSFCOMMMEDIUM_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class UtInput;
#include "UtCloneablePtr.hpp"
#include "UtMath.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfCommMediumMessageStatus.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"
#include "WsfCommMessage.hpp"
#include "WsfObject.hpp"
#include "WsfRandomVariable.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptObjectClass.hpp"

namespace wsf
{
namespace comm
{

class Layer;

namespace medium
{

//! A simple enum class used as a return value from some public API calls
//! to provide more information on result of a transmission attempt using
//! a medium.
enum class TransmissionResult : size_t
{
   cFAILURE_UNSPECIFIED,
   cSUCCESS,
   cFAILURE_CHANNEL_LIMIT_REACHED,
   cFAILURE_NONEXISTENT_RECEIVER,
   cFAILURE_UNCONNECTED_RECEIVER,
   cFAILURE_EM_INTERACTION
};

//! The Mode class represents a functional state of a medium.
//! Each medium can be made up of one or more Mode objects that
//! allow the medium to change its behavior depending on the mode
//! selected. This allows for unique handling of each message
//! dependent on user defined criteria (e.g. the recipient,
//! distance, channel utilization, etc.)
class WSF_EXPORT Mode : public UtScriptAccessible
{
public:
   Mode() = default;
   Mode(const std::string& aName)
      : mName(aName)
   {
   }
   virtual ~Mode()             = default;
   Mode(const Mode& aSrc)      = default;
   virtual Mode* Clone() const = 0;

   const char* GetScriptClassName() const override { return "WsfCommMediumMode"; }

   virtual bool ProcessInput(UtInput& aInput) { return false; };

   virtual double GetTransmissionTime(double aSimTime, Comm& aXmtr, Layer* aLayerPtr, Message& aMessage) = 0;

   virtual double GetPropagationTime(double aSimTime, Comm& aXmtr, Comm& aRcvr, Message& aMessage) = 0;

   virtual double GetPacketLossTime(double aSimTime, Message& aMessage) = 0;

   const std::string& GetName() const { return mName; }
   void               SetName(const std::string& aName) { mName = aName; }

private:
   std::string mName{};
};

//! The base Medium Mode script class.
class WSF_EXPORT ScriptMediumModeClass : public UtScriptClass
{
public:
   ScriptMediumModeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~ScriptMediumModeClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetName);
};

//! The medium class is an interface for a generic object
//! used to determine the capability, capacity, and duration
//! of messages being passed between comm objects. It may handle
//! one or more messages simultaneously (multiplex), and may have
//! multiple defined Modes to change the behavior of the medium.
//!
//! A single medium is associated with a utilizing object. However,
//! multiple objects may share a medium, s.t. all users of the medium
//! use the same object to facilitate message propagation.
class WSF_EXPORT Medium : public WsfObject
{
public:
   using ModeIt       = std::vector<std::unique_ptr<Mode>>::iterator;
   using const_ModeIt = std::vector<std::unique_ptr<Mode>>::const_iterator;

   Medium(WsfScenario& aScenario);
   ~Medium() override = default;
   Medium(const Medium& aSrc);
   Medium& operator=(Medium& aRhs);

   const char* GetScriptClassName() const override { return "WsfCommMedium"; }

   Medium*      Clone() const override = 0;
   bool         ProcessInput(UtInput& aInput) override;
   virtual bool Initialize(double aSimTime);

   void           SetSimulation(WsfSimulation* aSimPtr) { mSimPtr = aSimPtr; }
   WsfSimulation* GetSimulation() const { return mSimPtr; }

   //! @name Transmission and Propagation Methods
   //! These methods handle the initiation and termination of transmission
   //! and propagation of a message/data/signal. They are required for correct
   //! implementation of medium types.
   //@{
   virtual TransmissionResult TransmitMessage(double aSimTime, Layer* aLayerPtr, Message& aMessage, Comm& aXmtr) = 0;

   virtual void EndTransmission(double aSimTime, const Message::Identifier& aIdentifier) = 0;

   virtual void PropagateMessage(double aSimTime, const Message::Identifier& aIdentifier) = 0;

   virtual void EndPropagation(double aSimTime, const Message::Identifier& aIdentifier) = 0;
   //@}

   virtual TypeIdentifier GetMediumIdentifier() const = 0;

   size_t                  GetNumModes() const { return mModes.size(); }
   size_t                  GetCurrentModeIndex() const { return mModeIndex; }
   std::pair<size_t, bool> GetModeIndex(const std::string& aModeName) const;
   size_t                  GetNumChannels() const { return mChannels; }
   const std::string&      GetActiveModeName() const;
   bool                    IsMediumShared() const { return mShared; }
   bool                    UserDirectedSharing() const { return mUseSharing; }
   size_t                  GetCurrentTransmissions() const;
   size_t                  GetNumMessageStatuses() const { return mMessages.size(); }

   //! Mode Set Methods
   //! These methods change the current active mode of the medium.
   //! The SetActiveMode methods execute immediately, where the delayed
   //! versions execute via simulation event scheduling. The delayed
   //! calls allow processing the order of calls also delayed by
   //! event scheduling (such as call to send a message), where
   //! multiple calls to change the mode in a single time frame
   //! with the non-delayed methods would just result in the last
   //! called method being applied (as multiple calls would resolve
   //! prior to a non-scheduled event).
   //@{
   bool SetActiveMode(const std::string& aModeName);
   bool SetActiveMode(size_t aIndex);
   void SetActiveModeDelayed(const std::string& aModeName);
   void SetActiveModeDelayed(size_t aIndex);
   //@}

   void SetIsShared(bool aIsShared) { mShared = aIsShared; }

   //! Gets a currently handled message and its status by provision of
   //! the message serial number or by index.
   //@{
   MessageStatus* GetMessageStatus(const Message::Identifier& aIdentifier);
   MessageStatus* GetMessageStatusByIndex(size_t aIndex);
   //@}

   //! Returns a default constructed mode of the type associated with
   //! this medium implementation.
   virtual std::unique_ptr<Mode> CreateDefaultMode() const = 0;

   Mode* GetMode(const std::string& aModeName) const;
   Mode* GetMode(size_t aModeIndex) const;

   //! Optional user defined script method overload for mode selection
   //! at the time of transmission.
   std::string ExecuteModeOnTransmit(double aSimTime, const Message& aMessage, const Comm& aXmtr, const Comm& aRcvr);

protected:
   void RemoveMessageStatus(const Message::Identifier& aIdentifier);
   void SetNumChannels(size_t aNumChannels) { mChannels = aNumChannels; }

   //! Returns the mode to be used with a message transmission, either by
   //! getting the currently active mode, or from a user defined
   //! script method overload.
   Mode* GetModeForTransmission(double aSimTime, Comm& aXmtr, Comm& aRcvr, Message& aMessage);

   //! This container holds all messages currently being handled by this medium,
   //! along with their status and relevant bookkeeping.
   //! @note A message indicated to abort must be set before the delivery event.
   //! @note This object is not retained after any delivery event, regardless if
   //! indicated to actually deliver the message or not. As such, any message
   //! retained by the medium is assumed to be actively propagating.
   std::unordered_map<Message::Identifier, MessageStatus> mMessages{};

   //! The modes associated with this medium.
   //! The base class adds the base Mode object as
   //! the "default" mode.
   //! Pointers are used for potential polymorphism
   //! of the Mode object.
   std::vector<ut::CloneablePtr<Mode>> mModes{};

   //! This is a special flag for indicating the origin of
   //! this medium. When set to true, it indicates the medium
   //! is of a type that was explicitly added to its owner
   //! based on user input (such as "add medium"), and can
   //! potentially be shared among multiple containers. If set
   //! to false, it indicates this medium is added as a default
   //! instance, and unique to this container.
   //!
   //! This is necessary in order to support legacy input, and
   //! to provide useful feedback to users to avoid confusion
   //! when providing input at varying scopes for medium usage.
   bool mShared{false};

   //! Debug flag.
   bool mDebug{false};

   //! A shared pointer for correct termination of events when the owning medium
   //! is removed from the simulation prior to event execution, using wsf::comm::GenericEvent.
   std::shared_ptr<int> mEventPtr{nullptr};

private:
   size_t         mChannels{std::numeric_limits<size_t>::max()};
   size_t         mModeIndex{0U};
   std::string    mDefaultModeName{"default"};
   WsfScenario&   mScenario;
   WsfSimulation* mSimPtr{nullptr};

   //! The user may indicate that this medium should not be shared
   //! in any way, and that any resulting addition of a medium of this
   //! type to medium containers should always result in a unique instantiation
   //! of the medium. Setting mUseSharing to false allows the usage of
   //! the medium definition in the traditional AFSIM sense, that types
   //! provide the variables and each instance is unique. This is different
   //! from the mShared variable which is toggled only based on programmatic
   //! usage of the medium, s.t. setting mUseSharing to false will cause
   //! mShared to never be true.
   bool mUseSharing{true};

   std::unique_ptr<WsfScriptContext> mContextPtr{nullptr};

   //! User defined mode selection script method overload.
   UtScript* mModeOnTransmit{nullptr};
};

class WSF_EXPORT ScriptMediumClass : public WsfScriptObjectClass
{
public:
   ScriptMediumClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~ScriptMediumClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(ModeCount);
   UT_DECLARE_SCRIPT_METHOD(CurrentModeIndex);
   UT_DECLARE_SCRIPT_METHOD(CurrentModeName);
   UT_DECLARE_SCRIPT_METHOD(GetCurrentMode);
   UT_DECLARE_SCRIPT_METHOD(GetMode_1);
   UT_DECLARE_SCRIPT_METHOD(GetMode_2);
   UT_DECLARE_SCRIPT_METHOD(ChannelCount);

   UT_DECLARE_SCRIPT_METHOD(TransmissionCount);
   UT_DECLARE_SCRIPT_METHOD(MessageStatusCount);
   UT_DECLARE_SCRIPT_METHOD(GetMessageStatus);
   UT_DECLARE_SCRIPT_METHOD(GetMessageStatusByIndex);
   UT_DECLARE_SCRIPT_METHOD(CheckValidMessageStatus);

   UT_DECLARE_SCRIPT_METHOD(IsShared);

   UT_DECLARE_SCRIPT_METHOD(SetMode_1);
   UT_DECLARE_SCRIPT_METHOD(SetMode_2);
};

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
