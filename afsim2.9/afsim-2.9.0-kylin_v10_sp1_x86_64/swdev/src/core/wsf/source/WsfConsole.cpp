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

#include "WsfConsole.hpp"

#include <algorithm>
#include <cstdio>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtLogPublisher.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfSimulation.hpp"
#include "observer/WsfSimulationObserver.hpp"

static constexpr std::size_t cINDENT_SIZE = 4;

void WSF_EXPORT Register_console_output(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("console_output"))
   {
      aApplication.RegisterExtension("console_output",
                                     ut::make_unique<WsfDefaultApplicationExtension<wsf::console::ScenarioExtension>>());
   }
}

wsf::console::OutputSettings::OutputSettings()
   : mTypes(ut::log::Message::PresetDebug())
{
}

wsf::console::SubscriberBase::SubscriberBase(const OutputSettings& aSettings) noexcept
   : ut::log::SubscriberBase(aSettings.mTypes)
   , mSingleLine(aSettings.mSingleLine)
   , mForceFlushing(aSettings.mForceFlushing)
{
}

void wsf::console::SubscriberBase::UpdateSettings(const OutputSettings& aSettings)
{
   ResetSubscriptions(aSettings.mTypes);

   mSingleLine    = aSettings.mSingleLine;
   mForceFlushing = aSettings.mForceFlushing;
}

wsf::console::OutputSettings wsf::console::SubscriberBase::GetSettings() const noexcept
{
   OutputSettings retval;
   retval.mTypes         = GetSubscriptions();
   retval.mSingleLine    = mSingleLine;
   retval.mForceFlushing = mForceFlushing;
   return retval;
}

bool wsf::console::SubscriberBase::AddMessageP(const ut::log::Message& aMessage) noexcept
{
   bool          requiresFlush = mForceFlushing;
   std::ostream& stream        = GetStream();
   if (mSingleLine)
   {
      requiresFlush |= WriteSingleLine(aMessage, stream);
      stream << "\n";
   }
   else
   {
      requiresFlush |= ut::log::Publisher::DefaultFormat(aMessage, stream);
   }
   FlushStream(requiresFlush);

   return true;
}

bool wsf::console::SubscriberBase::WriteSingleLine(const ut::log::Message& aMessage, std::ostream& aStream, int aDepth) const noexcept
{
   using ut::log::Message;

   bool requiresFlush = false;

   if (aMessage.mTypes.empty())
   {
      // Do nothing. Optimization path.
   }
   else if (aMessage.mTypes.Contains(Message::Fatal()))
   {
      aStream << "***** FATAL: ";
      requiresFlush = true;
   }
   else if (aMessage.mTypes.Contains(Message::Error()))
   {
      aStream << "***** ERROR: ";
      requiresFlush = true;
   }
   else if (aMessage.mTypes.Contains(Message::Warning()))
   {
      aStream << "***** WARNING: ";
      requiresFlush = true;
   }
   else if (aMessage.mTypes.Contains(Message::Info()))
   {
      // no tag
   }
   else if (aMessage.mTypes.Contains(Message::Debug()))
   {
      // The analyst team has requested that debug messages not be marked up.
      /* std::cout << "***** DEBUG: "; */
   }
   else if (aMessage.mTypes.Contains(Message::Developer()))
   {
      aStream << "***** DEVELOPER: ";
   }
   else if (aMessage.mTypes.Contains(Message::Format()))
   {
      return false;
   }

   aStream << aMessage.mData;

   if (!aMessage.mData.empty())
   {
      if (!aMessage.mNotes.empty() && aDepth > 0)
      {
         aStream << "\n" << std::string(cINDENT_SIZE * aDepth, ' ');
      }
      else if (aMessage.mData.back() == '.' || aMessage.mData.back() == ':')
      {
         aStream << ' ';
      }
      else
      {
         aStream << "; ";
      }
   }

   bool indent = false;

   for (const Message& note : aMessage.mNotes)
   {
      if (indent)
      {
         aStream << "\n" << std::string(cINDENT_SIZE * aDepth, ' ');
      }
      requiresFlush |= WriteSingleLine(note, aStream, aDepth + 1);
      indent = !note.mNotes.empty();
   }

   return requiresFlush;
}

wsf::console::ConsoleSubscriber::ConsoleSubscriber(const OutputSettings& aSettings) noexcept
   : SubscriberBase(aSettings)
{
   // Save the list of previous console subscriptions
   mPreviousConsoleSubscriptions = ut::log::Publisher::GetConsoleSubscriptions();
   // Take console output responsibility from the ut::log::Publisher
   ut::log::Publisher::ResetConsoleSubscriptions();
}

wsf::console::ConsoleSubscriber::~ConsoleSubscriber() noexcept
{
   // Restore the list of previous console subscriptions to return the console output responsibility to the ut::log::Publisher
   ut::log::Publisher::ResetConsoleSubscriptions(mPreviousConsoleSubscriptions);
   ut::log::Publisher::UnregisterSubscriber(this);
}

void wsf::console::ConsoleSubscriber::FlushStream(bool aFlush)
{
   // Writing to cout will result in an infinite loop if the stream is being redirected.
   // Instead, we write directly to stdout.
   ut::log::Publisher::WriteRawToConsole(mStream.str(), aFlush);
   mStream.str(std::string());
}

wsf::console::FileSubscriber::~FileSubscriber() noexcept
{
   ut::log::Publisher::UnregisterSubscriber(this);
}

void wsf::console::FileSubscriber::OpenFile(const std::string& aFileName)
{
   auto lock = LockMutex();
   if (aFileName != mFileName)
   {
      mFileName = aFileName;

      mStream.close();
      mStream.clear();
      mStream.open(mFileName);
      if (!mStream.is_open())
      {
         auto out = ut::log::warning() << "Unable to open file.";
         out.AddNote() << "File: " << aFileName;
      }
   }
}

void wsf::console::FileSubscriber::FlushStream(bool aFlush)
{
   if (aFlush)
   {
      mStream.flush();
   }
}

bool wsf::console::ScenarioExtension::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   if (command == "console_output")
   {
      UtInputBlock inputBlock{aInput};

      while (inputBlock.ReadCommand(command))
      {
         if (command == "file")
         {
            ProcessInputFile(aInput);
         }
         else
         {
            ProcessInputSettings(aInput, mConsoleSettings);
         }
      }

      return true;
   }
   return false;
}

void wsf::console::ScenarioExtension::ProcessInputFile(UtInput& aInput)
{
   std::string fileName;
   aInput.ReadValueQuoted(fileName);
   fileName                     = aInput.SubstitutePathVariables(fileName);
   OutputSettings& fileSettings = mFileSettings[fileName];

   UtInputBlock block(aInput, "end_file");

   while (block.ReadCommand())
   {
      ProcessInputSettings(aInput, fileSettings);
   }
}

// static
void wsf::console::ScenarioExtension::ProcessInputSettings(UtInput& aInput, OutputSettings& aSettings)
{
   std::string command = aInput.GetCommand();
   if (command == "single_line")
   {
      aSettings.mSingleLine = aInput.ReadBool();
   }
   else if (command == "force_flushing")
   {
      aSettings.mForceFlushing = aInput.ReadBool();
   }
   else if (command == "preset")
   {
      std::string preset;
      aInput.ReadCommand(preset);
      if (preset == "none")
      {
         aSettings.mTypes.clear();
      }
      else if (preset == "low")
      {
         aSettings.mTypes = ut::log::Message::PresetWarnings();
      }
      else if (preset == "medium")
      {
         aSettings.mTypes = ut::log::Message::PresetRelease();
      }
      else if (preset == "high")
      {
         aSettings.mTypes = ut::log::Message::PresetDebug();
      }
      else
      {
         throw UtInput::BadValue(aInput, preset + " does not name a preset.");
      }
   }
   else
   {
      ProcessInputType(aInput, aSettings.mTypes);
   }
}

// static
void wsf::console::ScenarioExtension::ProcessInputType(UtInput& aInput, ut::log::TypeList& aTypes)
{
   std::string command = aInput.GetCommand();
   if (command == "type")
   {
      aInput.ReadCommand(command);
      if (command == "raw")
      {
         throw UtInput::BadValue(aInput, "'raw' names a reserved type");
      }
   }
   else
   {
      static constexpr const char* types[] = {"fatal", "error", "warning", "info", "debug", "developer"};
      auto                         it      = std::find(std::begin(types), std::end(types), command);
      if (it == std::end(types))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (aInput.ReadBool())
   {
      aTypes.Insert(command);
      if (command == "info")
      {
         aTypes.Insert("format");
      }
   }
   else
   {
      aTypes.Remove(command);
   }
}

void wsf::console::ScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   auto it = mFileSubscribers.begin();
   for (const auto& file : mFileSettings)
   {
      auto fileName = aSimulation.SubstituteOutputFileVariables(file.first);
      it->OpenFile(fileName);
      it++;
   }
}

bool wsf::console::ScenarioExtension::Complete()
{
   mConsoleSubscriber.emplace(mConsoleSettings);

   for (const auto& file : mFileSettings)
   {
      mFileSubscribers.emplace_back(file.second);
   }

   return true;
}
