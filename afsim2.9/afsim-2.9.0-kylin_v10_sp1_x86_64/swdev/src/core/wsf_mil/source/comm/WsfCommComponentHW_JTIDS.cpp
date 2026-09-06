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

#include "WsfCommComponentHW_JTIDS.hpp"

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommResult.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
ComponentHW_JTIDS* ComponentHW_JTIDS::Find(const Comm& aParent)
{
   ComponentHW_JTIDS* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<ComponentHW_JTIDS>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
ComponentHW_JTIDS* ComponentHW_JTIDS::FindOrCreate(Comm& aParent)
{
   ComponentHW_JTIDS* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new ComponentHW_JTIDS;
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
ComponentHW_JTIDS::ComponentHW_JTIDS()
   : wsf::comm::ComponentHW()
   , mMaximumRange(300.0 * UtMath::cM_PER_NM)
{
   // Define the nominal JTIDS terminal transmitter/receiver characteristics.

   // Normal power is 200 watts.  Some terminals can go to 1000 watts.
   mXmtrPtr->SetPower(200.0);

   // Mode 2 JTIDS utilizes 51 frequencies between 969 and 1206 MHz.  We'll just pick the middle.
   mXmtrPtr->SetFrequency(1087.5E+6);
   mXmtrPtr->SetBandwidth(237.0E+6);

   // This gives us a reception at 300 nm
   mRcvrPtr->SetDetectionThreshold(UtMath::DB_ToLinear(33.3));
}

// =================================================================================================
WsfComponent* ComponentHW_JTIDS::CloneComponent() const
{
   return new ComponentHW_JTIDS(*this);
}

// =================================================================================================
WsfStringId ComponentHW_JTIDS::GetComponentName() const
{
   return UtStringIdLiteral("comm_component_hw_jtids");
}

// =================================================================================================
const int* ComponentHW_JTIDS::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_HW_JTIDS, cWSF_COMPONENT_COMM_HW, cWSF_COMPONENT_COMM, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* ComponentHW_JTIDS::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_HW_JTIDS)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM_HW)
   {
      return (ComponentHW*)this;
   }
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return (Component*)this;
   }
   return nullptr;
}

// ============================================================================
// virtual
bool ComponentHW_JTIDS::Initialize(double aSimTime)
{
   bool ok = true;

   // Initialize the base component
   ok &= ComponentHW::Initialize(aSimTime);

   // Normal range is 300 nm and Extended range is 500 nm.
   mAntennaPtr->SetRangeLimits(0.0, mMaximumRange);

   return ok;
}

// =================================================================================================
// virtual
bool ComponentHW_JTIDS::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "maximum_range")
   {
      aInput.ReadValueOfType(mMaximumRange, UtInput::cLENGTH);
      aInput.ValueGreater(mMaximumRange, 0.0);
   }
   else
   {
      myCommand = ComponentHW::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool ComponentHW_JTIDS::CanReceiveFrom(double aSimTime, Comm* aXmtrPtr, Result& aResult)
{
   bool canReceive = false;
   aResult.SetCategory(mZoneAttenuationModifier);

   // Get the hardware component from the other comm device
   ComponentHW* xmtrHW_Ptr = ComponentHW::Find(*aXmtrPtr);
   if (xmtrHW_Ptr != nullptr)
   {
      WsfEM_Xmtr& xmtr = xmtrHW_Ptr->GetEM_Xmtr(0);
      xmtr.UpdatePosition(aSimTime);      // Ensure the transmitter position is current
      mRcvrPtr->UpdatePosition(aSimTime); // Ensure the receiver    position is current

      // Check the geometric constraints.

      if (aResult.BeginOneWayInteraction(&xmtr, mRcvrPtr.get(), true, true) == 0)
      {
         // We've passed the geometric tests.  Compute and check the signal to noise.

         aResult.SetTransmitterBeamPosition();
         aResult.SetReceiverBeamPosition();
         aResult.ComputeRF_OneWayPower();

         // Let components update the results.
         Component::AttemptToTransmit(*aXmtrPtr, aSimTime, aResult);
         Component::AttemptToReceive(*GetComm(), aSimTime, aResult);

         aResult.mSignalToNoise =
            mRcvrPtr->ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

         aResult.mCheckedStatus = Result::cSIGNAL_LEVEL;
         if ((aResult.mSignalToNoise >= mRcvrPtr->GetDetectionThreshold()) &&
             (aResult.mInterferenceFactor < 0.5)) // TODO_JAJ How should we really do this?
         {
            // Everything is good... make sure we're not masked by the terrain.
            if (!aResult.MaskedByTerrain())
            {
               canReceive = true;
            }
         }
         else
         {
            aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
         }
      }
   }

   if (aXmtrPtr->DebugEnabled() || GetComm()->DebugEnabled())
   {
      auto out = ut::log::debug() << "ComponentHW_JTIDS::CanReceiveFrom() Data Report:";
      aResult.Print(out);
      if (aResult.mFailedStatus == 0)
      {
         out.AddNote() << "Pr: " << UtMath::SafeLinearToDB(aResult.mRcvdPower) << " dBW";
         out.AddNote() << "Pj: " << UtMath::SafeLinearToDB(aResult.mInterferencePower) << " dBW";
         out.AddNote() << "S/N: " << UtMath::SafeLinearToDB(aResult.mSignalToNoise) << " dB";
         out.AddNote() << "S/N Req: " << UtMath::LinearToDB(mRcvrPtr->GetDetectionThreshold()) << " dB";
      }
   }

   return canReceive;
}

} // namespace comm
} // namespace wsf
