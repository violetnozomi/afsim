// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// References:
// 1) "Laser Communications in Space", Stephen G. Lambert & William L. Casey, Artech House, 1995.
// 2) "Predicting the Performance of Linear Optical Detectors in Free Space Laser Communication Links",
//    Thomas C Farrell, Air Force Research Laboratory, Space Vehicles Directorate 3550 Aberdeen Ave., SE,
//    Kirtland AFB, NM 87117-5776, Unpublished.

#include "WsfCommComponentHW_LASER.hpp"

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommResult.hpp"
#include "WsfLASER_CommXmtrComponent.hpp"
#include "WsfLASER_RcvrComponent.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
ComponentHW_LASER* ComponentHW_LASER::Find(const Comm& aParent)
{
   ComponentHW_LASER* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<ComponentHW_LASER>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
ComponentHW_LASER* ComponentHW_LASER::FindOrCreate(Comm& aParent)
{
   ComponentHW_LASER* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new ComponentHW_LASER(aParent.GetScenario());
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
ComponentHW_LASER::ComponentHW_LASER(const WsfScenario& aScenario)
   : wsf::comm::ComponentHW()
   , mTurbulence(aScenario)
   , mTurbulenceTransmissionFactor(0.0)
   , // Set to zero so that the turbulence model is used by default instead.
   mAttenuationTransmissionFactor(1.0)
   , mAeroOpticTransmissionFactor(1.0)
   , mBackgroundSpectralRadiance(0.0)
   , mBackgroundSpectralIrradiance(0.0)
   , mShowLinkBudget(false)
{
   mXmtrPtr->GetComponents().AddComponent(new WsfLASER_CommXmtrComponent());
   mRcvrPtr->GetComponents().AddComponent(new WsfLASER_RcvrComponent());
}

// =================================================================================================
WsfComponent* ComponentHW_LASER::CloneComponent() const
{
   return new ComponentHW_LASER(*this);
}

// =================================================================================================
WsfStringId ComponentHW_LASER::GetComponentName() const
{
   return UtStringIdLiteral("comm_component_hw_laser");
}

// =================================================================================================
const int* ComponentHW_LASER::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_HW_LASER, cWSF_COMPONENT_COMM_HW, cWSF_COMPONENT_COMM, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* ComponentHW_LASER::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_HW_LASER)
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

// =================================================================================================
// virtual
bool ComponentHW_LASER::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (mTurbulence.ProcessInput(aInput))
   {
   }
   else if (command == "attenuation")
   {
      myCommand = mXmtrPtr->ProcessInput(aInput);
   }
   else if (command == "aero_optic_transmission_factor")
   {
      aInput.ReadValue(mAeroOpticTransmissionFactor);
      aInput.ValueInClosedRange(mAeroOpticTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "aero_optic_loss")
   {
      aInput.ReadValueOfType(mAeroOpticTransmissionFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mAeroOpticTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "attenuation_transmission_factor")
   {
      aInput.ReadValue(mAttenuationTransmissionFactor);
      aInput.ValueInClosedRange(mAttenuationTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "attenuation_loss")
   {
      aInput.ReadValueOfType(mAttenuationTransmissionFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mAttenuationTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "turbulence_transmission_factor")
   {
      aInput.ReadValue(mTurbulenceTransmissionFactor);
      aInput.ValueInClosedRange(mTurbulenceTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "turbulence_loss")
   {
      aInput.ReadValueOfType(mTurbulenceTransmissionFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mTurbulenceTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "show_link_budget")
   {
      aInput.ReadValue(mShowLinkBudget);
   }
   else if (command == "background_radiance")
   {
      aInput.ReadValueOfType(mBackgroundSpectralRadiance, UtInput::cSPECTRAL_RADIANCE);
      aInput.ValueGreaterOrEqual(mBackgroundSpectralRadiance, 0.0);
   }
   else if (command == "background_irradiance")
   {
      aInput.ReadValueOfType(mBackgroundSpectralIrradiance, UtInput::cSPECTRAL_IRRADIANCE);
      aInput.ValueGreaterOrEqual(mBackgroundSpectralIrradiance, 0.0);
   }
   else
   {
      myCommand = ComponentHW::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool ComponentHW_LASER::CanSendTo(double aSimTime, Comm* aRcvrPtr, Result& aResult)
{
   bool canSendTo = false;
   aResult.SetCategory(mZoneAttenuationModifier);

   // Get the hardware component from the other comm device (must be of type ComponentLASER_HW):
   auto* commRcvrHW_Ptr = aRcvrPtr->GetComponents().GetComponent<ComponentHW_LASER>();
   if (commRcvrHW_Ptr != nullptr)
   {
      auto& rcvr = commRcvrHW_Ptr->GetEM_Rcvr(0);
      rcvr.UpdatePosition(aSimTime);      // Ensure the receiver position is current
      mXmtrPtr->UpdatePosition(aSimTime); // Ensure the transmitter position is current

      if (aResult.BeginOneWayInteraction(mXmtrPtr.get(), &rcvr, true, true) == 0)
      {
         if (mXmtrPtr->GetPower() > 1.0E-30) // See comment in WsfCommComponentHW constructor
         {
            aResult.SetTransmitterBeamPosition();
            aResult.SetReceiverBeamPosition();
            aResult.ComputeUndefinedGeometry();

            // Let components update the results.
            Component::AttemptToTransmit(*GetComm(), aSimTime, aResult);
            Component::AttemptToReceive(*aRcvrPtr, aSimTime, aResult);

            // Compute the received energy from the transmitted pulse
            // per eq. 3.75 of ref. (1),
            // Pr = Pt * Gt * Lt * Latm * Lturb * Lao * Lr * Gr * Lrec

            // Compute Pt, the energy of the pulse from the laser at aperture (J)
            // Propagate through the transmit optics
            auto&  laserXmtr = *(mXmtrPtr->GetComponents().GetComponent<WsfLASER_CommXmtrComponent>());
            double Pt        = mXmtrPtr->GetPeakPower(); // peak power

            // The antenna gain is precomputed in the laser transmitter component, and includes transmitter losses
            // (optical wavefront loss and optical element losses)
            double Gt = laserXmtr.GetAntennaGain();

            // Account for pointing loss:
            Pt *= laserXmtr.GetPointingTransmission();

            // Compute the range loss.
            double R  = std::max(aResult.mXmtrToRcvr.mRange, 1.0);
            double Lr = pow(mXmtrPtr->GetWavelength() / UtMath::cFOUR_PI / R, 2);

            // Account for atmospheric losses from the transmitter to the target.
            double Latm = mAttenuationTransmissionFactor;
            if (Latm == 1.0) // Not set (or set to 1) during input.
            {
               Latm = aResult.ComputeAttenuationFactor(WsfEM_Interaction::cXMTR_TO_RCVR);
            }

            // Compute the effect due to turbulent beam spread.
            double Lturb = mTurbulenceTransmissionFactor;
            if (Lturb == 0.0) // Not set in input; use the physical turbulence model.
            {
               Lturb                            = 1.0;
               static const double cMAX_HEIGHT  = 50000.0; // 50 km (arbitrary value)
               double              myHeight     = GetPlatform()->GetAltitude();
               double              targetHeight = aRcvrPtr->GetPlatform()->GetAltitude();

               // Only compute turbulent beam spread effect if some of the path length is below cMAX_HEIGHT
               if ((myHeight < cMAX_HEIGHT) || (targetHeight < cMAX_HEIGHT))
               {
                  double tba =
                     2.0 * mTurbulence.BeamSpreadAngle(R,
                                                       targetHeight,
                                                       myHeight,
                                                       mXmtrPtr->GetWavelength()); // (function returns half angle; we
                                                                                   // want full angle (*2.0).

                  // Loss factor due to turbulent beam spread that is the ratio of the spot size due to diffraction
                  // alone, to that due to diffraction and turbulence.
                  Lturb *= 1.0 / (1.0 + pow(tba / laserXmtr.GetBeamwidth(), 2));
               }
            }

            // Take into account aero-optic turbulence
            double Lao = mAeroOpticTransmissionFactor;

            // The receiver gain is computed by the receiver component:
            auto&  laserRcvr = *(rcvr.GetComponents().GetComponent<WsfLASER_RcvrComponent>());
            double Gr_m2     = 4.0 * UtMath::cPI * pow(mXmtrPtr->GetWavelength(), -2); // Gain per square meter.

            // Lrec, the pointing loss factor, is taken into account when computing the detection data, so it is not
            // used here. Lt is precomputed in the transmit antenna gain and is not used here. Received power (eq.
            // e.105)
            double Pr_m2 = Pt * Gt * Latm * Lturb * Lao * Lr * Gr_m2; // * Lrec, as applicable below.

            WsfLASER_RcvrComponent::DetectionData dd = laserRcvr.ComputeDetectionData(0.0,
                                                                                      Pr_m2,
                                                                                      mBackgroundSpectralRadiance,
                                                                                      mBackgroundSpectralIrradiance,
                                                                                      mXmtrPtr->GetPulseWidth());

            aResult.mSignalToNoise  = dd.mSignalCount / dd.mNoiseCount; // per pulse.
            aResult.mRcvdPower      = dd.mPower;
            aResult.mRcvrNoisePower = dd.mNoiseCount * UtMath::cELEMENTARY_CHARGE / mXmtrPtr->GetPulseWidth() /
                                      laserRcvr.GetResponsivity(); // el * coulombs/el / s / A/W -> A * W/A -> W

            aResult.mCheckedStatus = Result::cSIGNAL_LEVEL;
            if (aResult.mSignalToNoise >= rcvr.GetDetectionThreshold())
            {
               // Signal level success
               canSendTo = true;
            }
            else
            {
               aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
            }

            // Display the link budget if specified.
            if (mShowLinkBudget && rcvr.GetSimulation()->IsActive()) // ... but do not display during initialization.
            {
               double xmtrPeakPower    = mXmtrPtr->GetPeakPower();
               double xmtrPeakPower_dB = UtMath::LinearToDB(xmtrPeakPower);
               double xmtrPower        = mXmtrPtr->GetPower();
               double xmtrPower_dB     = UtMath::LinearToDB(xmtrPower);
               double xmtrOptLoss = laserXmtr.GetOpticsTransmissionFactor() * laserXmtr.GetWavefrontTransmissionFactor();
               double xmtrOptLoss_dB  = UtMath::LinearToDB(xmtrOptLoss);
               double xmtrOptGain     = laserXmtr.GetTransmitAntennaGain();
               double xmtrOptGain_dB  = UtMath::LinearToDB(xmtrOptGain);
               double pointingLoss    = laserXmtr.GetPointingTransmission();
               double pointingLoss_dB = UtMath::LinearToDB(pointingLoss);
               double rcvrGain        = Gr_m2 * UtMath::cPI * pow(laserRcvr.GetApertureDiameter() / 2.0, 2);
               double rcvrGain_dB     = UtMath::LinearToDB(rcvrGain);
               double signal          = dd.mPower;
               double signal_dB       = UtMath::LinearToDB(signal);
               double noise           = aResult.mRcvrNoisePower;
               double noise_dB        = UtMath::LinearToDB(noise);
               double rcvrOptLoss     = laserRcvr.GetOpticsTransmissionFactor();
               double rcvrOptLoss_dB  = UtMath::LinearToDB(rcvrOptLoss);
               double pathLoss_dB     = UtMath::LinearToDB(Lr);
               double atmLoss_dB      = UtMath::LinearToDB(Latm);
               double Lturb_dB        = UtMath::LinearToDB(Lturb);
               double Lao_dB          = UtMath::LinearToDB(Lao);
               double sn_dB           = UtMath::LinearToDB(aResult.mSignalToNoise);

               auto out = ut::log::info() << "Link budget:";
               out.AddNote() << "Link Terms: " << GetComm()->GetPlatform()->GetName() << ":" << GetComm()->GetName()
                             << " -> " << aRcvrPtr->GetPlatform()->GetName() << ":" << aRcvrPtr->GetName();
               out.AddNote() << "Peak Transmitted Power: " << xmtrPeakPower << " W (" << xmtrPeakPower_dB << " dbW)";
               out.AddNote() << "Average Transmitted Power: " << xmtrPower << " W (" << xmtrPower_dB << " dbW)";
               out.AddNote() << "Xmtr Optical Transmission (Loss): " << xmtrOptLoss << " (" << xmtrOptLoss_dB << " db)";
               out.AddNote() << "Pointing Error Transmission (Loss): " << pointingLoss << " (" << pointingLoss_dB << " db)";
               out.AddNote() << "Xmtr Antenna Gain: " << xmtrOptGain << " (" << xmtrOptGain_dB << " db)";
               out.AddNote() << "Effective Iso. Rad. Power (EIRP): "
                             << " (" << xmtrPower_dB + xmtrOptLoss_dB + pointingLoss_dB + xmtrOptGain_dB << " dbW)";
               out.AddNote() << "Atm. Transmission (Loss): " << Latm << " (" << atmLoss_dB << " db)";
               out.AddNote() << "Turbulence Path Transmission (Loss): " << Lturb << " (" << Lturb_dB << " db)";
               out.AddNote() << "Aero-optic Transmission (Loss): " << Lao << " (" << Lao_dB << " db)";
               out.AddNote() << "Path Transmission (Loss): " << Lr << " (" << pathLoss_dB << " db)";
               out.AddNote() << "Total Channel Loss: "
                             << " (" << atmLoss_dB + Lturb_dB + Lao_dB + pathLoss_dB << " db)";
               out.AddNote() << "Recv Antenna Gain: " << rcvrGain << " (" << rcvrGain_dB << " db)";
               out.AddNote() << "Recv Optical Transmission (Loss): " << rcvrOptLoss << " (" << rcvrOptLoss_dB << " db)";
               out.AddNote() << "Total Rx Gain / Loss: "
                             << " (" << rcvrGain_dB + rcvrOptLoss_dB << " db)";
               out.AddNote() << "Signal Power: " << signal << " W (" << signal_dB << " dbW)";
               out.AddNote() << "Noise Power: " << noise << " W (" << noise_dB << " dbW)";
               out.AddNote() << "S/N: " << aResult.mSignalToNoise << " (" << sn_dB << " db)";
            }
         }
      }

      // Check for terrain masking if we have made it this far...
      if (canSendTo && aResult.MaskedByTerrain())
      {
         canSendTo = false;
      }
   }

   if (GetComm()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Comm Report:";
      aResult.Print(out);
      if (aResult.mFailedStatus == 0)
      {
         out.AddNote() << "S/N Req: " << UtMath::LinearToDB(mRcvrPtr->GetDetectionThreshold()) << " dB";
         out.AddNote() << "Sent: " << canSendTo;
      }
   }

   return canSendTo;
}

} // namespace comm
} // namespace wsf
