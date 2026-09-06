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

#ifndef WSF_CONSOLE_HPP
#define WSF_CONSOLE_HPP

#include "wsf_export.h"

#include <fstream>
#include <list>
#include <map>

#include "UtInput.hpp"
#include "UtLogSubscriber.hpp"
#include "UtOptional.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"

namespace wsf
{
namespace console
{
//! Settings are read from the file into a WsfConsoleSettings object
//!    then given to a subscriber to initialize it.
struct OutputSettings
{
   OutputSettings();

   //! Which types to subscribe to.
   ut::log::TypeList mTypes;

   //! Format messages are enabled if Info messages are enabled.
   bool mSingleLine = false;

   //! Perform a I/O flush at the end of every log entry.
   bool mForceFlushing = true;
};

//! Contains the logic for formatting a message for file or for console.
class WSF_EXPORT SubscriberBase : public ut::log::SubscriberBase
{
public:
   //! Creates and registers the subscriber.
   explicit SubscriberBase(const OutputSettings& aSettings) noexcept;

   //! Sets the settings.
   void UpdateSettings(const OutputSettings& aSettings);
   //! Returns the current settings.
   OutputSettings GetSettings() const noexcept;

private:
   //! Method called immediately after aMessage is published.
   //! Formats aMessage and writes it to GetStream().
   bool AddMessageP(const ut::log::Message& aMessage) noexcept final;

   //! Returns true if message types (fatal, error, warning) require flushing.
   //! @param aMessage is the message to write to stream.
   //! @param aStream is the stream to write to.
   //! @param aDepth is the potential indentation depth of the message.
   bool WriteSingleLine(const ut::log::Message& aMessage, std::ostream& aStream, int aDepth = 0) const noexcept;

   //! Returns the stream used for preparing the text for output.
   virtual std::ostream& GetStream() = 0;
   //! Flushes the stream to file.
   virtual void FlushStream(bool aFlush) = 0;

   bool mSingleLine    = false;
   bool mForceFlushing = true;
};

//! Writes received messages to stdout.
class WSF_EXPORT ConsoleSubscriber final : public SubscriberBase
{
public:
   //! Disables default console output.
   explicit ConsoleSubscriber(const OutputSettings& aSettings) noexcept;

   ~ConsoleSubscriber() noexcept override;

private:
   //! Returns the stream used to prepare the text for output.
   std::ostream& GetStream() override { return mStream; }
   //! Writes mStream to stdout and clears the stream.
   void FlushStream(bool aFlush) override;

   std::ostringstream mStream;
   //! Keep track of the list of previous console subscriptions as to transfer console output responsibility from the
   //! ut::log::Publisher to the WsfConsole and vice versa
   ut::log::TypeList mPreviousConsoleSubscriptions;
};

//! Writes received messages to file on disk.
class WSF_EXPORT FileSubscriber final : public SubscriberBase
{
public:
   using SubscriberBase::SubscriberBase;

   ~FileSubscriber() noexcept override;

   //! Opens the file given by aFileName.
   //! If that file is already open, does nothing.
   void OpenFile(const std::string& aFileName);

private:
   //! Returns the file stream being written to.
   std::ostream& GetStream() override { return mStream; }
   //! Flushes the file stream to file.
   void FlushStream(bool aFlush) override;

   std::string   mFileName;
   std::ofstream mStream;
};

//! Reads the input for console_output.
class WSF_EXPORT ScenarioExtension final : public WsfScenarioExtension
{
public:
   ScenarioExtension() = default;

   ScenarioExtension(const ScenarioExtension&) = delete;
   ScenarioExtension& operator=(const ScenarioExtension&) = delete;

   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
   bool Complete() override;

private:
   void        ProcessInputFile(UtInput& aInput);
   static void ProcessInputSettings(UtInput& aInput, OutputSettings& aSettings);
   static void ProcessInputType(UtInput& aInput, ut::log::TypeList& aTypes);

   OutputSettings                        mConsoleSettings;
   std::map<std::string, OutputSettings> mFileSettings;

   //! Using ut::optional for delayed initialization.
   ut::optional<ConsoleSubscriber> mConsoleSubscriber;
   std::list<FileSubscriber>       mFileSubscribers;
};
} // namespace console
} // namespace wsf

#endif
