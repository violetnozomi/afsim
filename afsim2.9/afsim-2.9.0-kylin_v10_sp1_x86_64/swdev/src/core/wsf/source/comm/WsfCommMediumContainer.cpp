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

#include "WsfCommMediumContainer.hpp"

#include "UtStringIdLiteral.hpp"
#include "WsfCommMediumTypes.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

// ============================================================================
Container::Container(const Container& aSrc)
   : mScenario(aSrc.mScenario)
   , mValidMediums(aSrc.mValidMediums)
   , mForbiddenMediums(aSrc.mForbiddenMediums)
   , mDefaultMedium(aSrc.mDefaultMedium)
{
   // The container copy constructor handles copying of the medium based
   // on medium settings. Those mediums added explicitly to a parent are
   // considered shared, and every copy will be a reference to a single
   // medium. The exception is if the user specifically indicates that the
   // defined medium should not be shared.
   // Those mediums not designated as shared, such as implicit medium usage
   // via legacy input, or the lack of adding a medium with no input, results
   // in mediums that are not shared and considered unique to each container.
   // As such, shared mediums increment the shared_ptr reference, while non-shared
   // versions get a unique copy with a new shared_ptr.
   if (aSrc.mMedium)
   {
      if (aSrc.mMedium->IsMediumShared())
      {
         mMedium = aSrc.mMedium;
      }
      else
      {
         mMedium = ut::clone(aSrc.mMedium);
      }
   }
}

// ============================================================================
bool Container::ProcessContainerInput(UtInput&    aInput,
                                      InputAction aAction) // = cNONE
{
   switch (aAction)
   {
   case cADD:
   {
      return Types::Get(mScenario).GetManager().ProcessAddOrEditCommand(aInput, *this, true);
   }
   case cEDIT:
   {
      return Types::Get(mScenario).GetManager().ProcessAddOrEditCommand(aInput, *this, false);
   }
   case cDELETE:
   {
      return Types::Get(mScenario).GetManager().ProcessDeleteCommand(aInput, *this);
   }
   default:
   {
      // Processing input related directly to the container itself as a default.
      return ProcessInput(aInput);
   }
   }

   return false;
}

// ============================================================================
bool Container::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();

   // Support for legacy commands.
   // These are commands associated previously with wsf::comm::PhysicalLayer or
   // wsf::comm::PhysicalLayerRF (removed), and provide a pass-through of these
   // commands at the parent scope to the new usage via the medium object.
   // Note: "transfer_rate" is not included as a pass-through. This command
   // is retained to describe the theoretical maximum transfer of the hardware
   // being modeled by the comm object. Any usage of this command outside of the
   // the formal medium definition (legacy usage) is solely for usage by the comm
   // physical layer.
   if (command == "propagation_speed" || command == "packet_loss_time" || command == "snr_transfer_rate_table" ||
       command == "bit_error_probability" || command == "error_correction" || command == "bit_error_rate_ebno_table")
   {
      // At this scope and with this command, if a medium
      // exists, check that is unique to this container
      // (i.e. a default core type clone)
      if (mMedium)
      {
         if (mMedium->IsMediumShared())
         {
            // A shared medium is one explicitly added to the container
            // parent instance. In such cases, usage of legacy input
            // results in an error. Provide feedback to the user to
            // aid in understanding why there is a failure.
            auto out = ut::log::warning() << "Cannot set this command " << command << "at this scope.";
            out.AddNote() << "A medium instance has already been added.";
            out.AddNote() << "Modify the medium directly, or remove the medium being added.";
            myCommand = false;
         }
         else
         {
            // The medium already has an instance, and it is not
            // shared. Just pass the input on to the medium. This
            // may still fail if the default medium usage does not
            // support legacy commands.
            myCommand = mMedium->ProcessInput(aInput);
         }
      }
      else
      {
         // A medium does not exist. Send a request to add one
         // for this instance at this time since the user is attempting
         // to use these commands for legacy support.
         // Note that if the processing fails, the recently added instance
         // is removed.
         if (Types::Get(mScenario).AddDefaultMedium(*this))
         {
            myCommand = mMedium->ProcessInput(aInput);
            if (!myCommand)
            {
               SetMedium(nullptr);
            }
         }
      }
   }

   return myCommand;
}

// ============================================================================
bool Container::Initialize(double aSimTime)
{
   bool ok = true;

   if (mMedium != nullptr)
   {
      mMedium->SetSimulation(mSimPtr);
      ok &= mMedium->Initialize(aSimTime);
   }

   return ok;
}

// ============================================================================
void Container::AddValidMedium(TypeIdentifier aValue)
{
   auto it = mForbiddenMediums.find(aValue);
   if (it == std::end(mForbiddenMediums))
   {
      mValidMediums.insert(aValue);
   }
}

// ============================================================================
void Container::AddForbiddenMedium(TypeIdentifier aValue)
{
   auto result = mForbiddenMediums.insert(aValue);
   if (result.second)
   {
      auto it = mValidMediums.find(aValue);
      if (it != std::end(mValidMediums))
      {
         mValidMediums.erase(it);
      }
   }
}

// ============================================================================
bool Container::IsValidMedium(TypeIdentifier aValue) const
{
   return (mValidMediums.find(aValue) != std::end(mValidMediums));
}

// ============================================================================
void Container::SetDefaultMedium(TypeIdentifier aValue)
{
   auto it = mForbiddenMediums.find(aValue);
   if (it == std::end(mForbiddenMediums))
   {
      mDefaultMedium = aValue;
   }
}

// ============================================================================
bool Container::AddDefaultMedium()
{
   if (!mMedium)
   {
      return Types::Get(mScenario).AddDefaultMedium(*this);
   }

   return false;
}

WSF_EXPORT WsfStringId GetContainerComponentName()
{
   return UtStringIdLiteral("medium_container");
}
} // namespace medium
} // namespace comm
} // namespace wsf
