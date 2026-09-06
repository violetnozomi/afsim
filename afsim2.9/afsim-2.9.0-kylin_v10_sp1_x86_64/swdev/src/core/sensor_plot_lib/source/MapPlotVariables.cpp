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

#include "MapPlotVariables.hpp"

#include <algorithm>
#include <cmath>
#include <typeinfo>

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtScriptDataPack.hpp"
#include "Utility.hpp"
#include "WsfEW_Result.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSAR_Sensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

namespace
{
// Convert an angle in radians to degrees and make it 'printable'.
// If the result is very close to zero it will be forced to zero.
inline double ConvertAndMakePrintableAngle(double aAngleRad)
{
   double angleDeg = aAngleRad * UtMath::cDEG_PER_RAD;
   return (fabs(angleDeg) < 1.0E-12) ? 0.0 : angleDeg;
}
} // namespace

namespace
{
// A helper function to get the global script context
inline WsfScriptContext& GetContext(WsfSimulation& aSimulation)
{
   return aSimulation.GetScriptContext();
}

inline void ExecuteScript(double             aSimTime,
                          WsfSimulation&     aSimulation,
                          const std::string& aScriptName,
                          UtScriptData&      aScriptRetVal,
                          UtScriptDataList&  aScriptArgs)
{
   GetContext(aSimulation).ExecuteScript(aSimTime, aScriptName, aScriptRetVal, aScriptArgs);
}

inline UtScript* FindScript(WsfSimulation& aSimulation, const std::string& aScriptName)
{
   return GetContext(aSimulation).FindScript(aScriptName);
}

inline bool ValidateScript(WsfSimulation& aSimulation, UtScript* aScriptPtr)
{
   return GetContext(aSimulation)
      .ValidateScript(aScriptPtr, "double", "WsfPlatform, WsfSensor, WsfPlatform, WsfSensorInteraction, double");
}
} // namespace


// =========================== pd =======================================================
class pd_MPV : public MapPlotVariable
{
public:
   pd_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~pd_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (MapPlotVariables::CheckSucceeded(aResult) && (aResult.mPd >= aResult.mRequiredPd))
      {
         value = std::max(aValue, aResult.mPd);
      }
      return value;
   }
};

// =========================== detection_threshold ======================================
class detection_threshold_MPV : public MapPlotVariable
{
public:
   detection_threshold_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E+30))
   {
   }
   ~detection_threshold_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mDetectionThreshold > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mDetectionThreshold);
         if (aValue != GetInitialValue())
         {
            value = std::min(aValue, value);
         }
      }
      return value;
   }
};

// =========================== required_rcs =============================================
class required_rcs_MPV : public MapPlotVariable
{
public:
   required_rcs_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E+30))
   {
   }
   ~required_rcs_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if ((aResult.mSignalToNoise > 0.0) && (aResult.mRadarSig > 0.0))
      {
         double signalToNoiseRequired = Utility::GetDetectionReference(aSensorPtr, mDetectionReference);
         value = UtMath::LinearToDB(aResult.mRadarSig * (signalToNoiseRequired / aResult.mSignalToNoise));
         if (aValue != GetInitialValue())
         {
            value = std::min(value, aValue);
         }
      }
      return value;
   }

   bool ProcessInput(UtInput& aInput) override
   {
      bool        ok = true;
      std::string command(aInput.GetCommand());
      if (command == "detection_reference")
      {
         aInput.ReadValueOfType(mDetectionReference, UtInput::cRATIO);
         aInput.ValueGreater(mDetectionReference, 0.0);
      }
      else
      {
         ok = false;
      }
      return ok;
   }

private:
   double mDetectionReference{UtMath::DB_ToLinear(12.8)};
};

// =========================== required_jamming_power ====================================
class required_jamming_power_MPV : public MapPlotVariable
{
public:
   required_jamming_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~required_jamming_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mRcvdPower > 0.0)
      {
         // Get the pointer to the jammer platform and system.
         mJammerXmtrPtr->SetFrequency(aResult.GetReceiver()->GetFrequency());
         mJammerXmtrPtr->SetBandwidth(aResult.GetReceiver()->GetBandwidth());
         mJammerXmtrPtr->SetPower(1.0);
         mJammerXmtrPtr->Initialize(*aSimulationPtr);

         WsfEM_Interaction jammerInteraction;
         if (jammerInteraction.BeginOneWayInteraction(mJammerXmtrPtr, aResult.GetReceiver(), false, true) == 0)
         {
            // Set the beam positions.
            jammerInteraction.SetReceiverBeamPosition(aResult.mRcvrBeam);
            jammerInteraction.SetTransmitterBeamPosition();

            // Compute the power from the jammer as seen by the receiver.
            double jammerPowerReceived = jammerInteraction.ComputeRF_OneWayPower();

            if (jammerPowerReceived > 0.0)
            {
               double referenceJammerPower = mJToS_Reference * aResult.mRcvdPower;
               double requiredJammerPower  = referenceJammerPower / jammerPowerReceived;
               requiredJammerPower         = UtMath::LinearToDB(requiredJammerPower);
               value                       = std::max(aValue, requiredJammerPower);
            }
         }
      }
      return value;
   }

   bool ProcessInput(UtInput& aInput) override
   {
      bool        myCommand = true;
      std::string command(aInput.GetCommand());
      if (command == "jamming_platform_name")
      {
         std::string pltfrmName;
         aInput.ReadValue(pltfrmName);
         mJammingPlatformId = pltfrmName;
      }
      else if (command == "jammer_to_signal_reference")
      {
         aInput.ReadValueOfType(mJToS_Reference, UtInput::cRATIO);
         aInput.ValueGreater(mJToS_Reference, 0.0);
      }
      else
      {
         myCommand = false;
      }
      return myCommand;
   }

   bool Initialize(WsfSimulation& aSimulation, const SharedData& aSharedData) override
   {
      bool ok = MapPlotVariable::Initialize(aSimulation, aSharedData);
      // Setup the jamming platform that is used for the variable output 'required_jammer_power'.
      if (mJammingPlatformId == nullptr)
      {
         mJammingPlatformId = "TARGET";
      }

      WsfPlatform* jammerPltfrmPtr = aSimulation.GetPlatformByName(mJammingPlatformId);
      if (jammerPltfrmPtr != nullptr)
      {
         WsfWeapon* weaponPtr = jammerPltfrmPtr->GetComponent<WsfWeapon>("SENSOR_PLOT_JAMMER");
         if (weaponPtr == nullptr)
         {
            auto jammerWeaponPtr = ut::make_unique<WsfRF_Jammer>(aSimulation.GetScenario());
            jammerWeaponPtr->SetName("SENSOR_PLOT_JAMMER");

            // The transmitter must have some basic parameters defined or Initialize will issue error messages ...
            // ... and they can't simply be 'Set' because the mode doesn't yet exist.
            // These parameters are just placeholders as they will get replaced later.
            UtInput xmtrParams;
            xmtrParams.SetAuxData(0, static_cast<void*>(const_cast<WsfScenario*>(&aSimulation.GetScenario())));
            xmtrParams.PushInputString("transmitter power 1 w frequency 1 ghz bandwidth 1 mhz end_transmitter");
            jammerWeaponPtr->ProcessInput(xmtrParams);
            // Minimize scope of non-owning reference, which is necessary to initialize the jammer
            // after ownership has been transferred to the platform (via AddComponent)
            {
               auto jammerRefPtr = jammerWeaponPtr.get();
               jammerPltfrmPtr->AddComponent(jammerWeaponPtr.release());
               jammerRefPtr->Initialize(0.0);

               mJammerXmtrPtr = &(jammerRefPtr->GetMasterXmtr());
            }
         }
      }
      else
      {
         auto out = ut::log::error() << "Could not find jammer platform.";
         out.AddNote() << "Jammer Platform: " << mJammingPlatformId.GetString();
      }
      ok &= mJammerXmtrPtr != nullptr;
      return ok;
   }

private:
   WsfStringId mJammingPlatformId{nullptr};
   WsfEM_Xmtr* mJammerXmtrPtr{nullptr};
   double      mJToS_Reference{1.0};
};

// =========================== radar_signature ==========================================
class radar_signature_MPV : public MapPlotVariable
{
public:
   radar_signature_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~radar_signature_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mRadarSig > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mRadarSig);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== optical_signature ========================================
class optical_signature_MPV : public MapPlotVariable
{
public:
   optical_signature_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~optical_signature_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mOpticalSig > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mOpticalSig);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== infrared_signature =======================================
class infrared_signature_MPV : public MapPlotVariable
{
public:
   infrared_signature_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~infrared_signature_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mInfraredSig > 0.0)
      {
         value = std::max(aValue, aResult.mInfraredSig);
      }
      return value;
   }
};

// =========================== signal_power =============================================
class signal_power_MPV : public MapPlotVariable
{
public:
   signal_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~signal_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mRcvdPower > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mRcvdPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== clutter_power ============================================
class clutter_power_MPV : public MapPlotVariable
{
public:
   clutter_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~clutter_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mRcvdPower > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mClutterPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== noise_power ==============================================
class noise_power_MPV : public MapPlotVariable
{
public:
   noise_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~noise_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mRcvrNoisePower > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mRcvrNoisePower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== interference_power =======================================
class interference_power_MPV : public MapPlotVariable
{
public:
   interference_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~interference_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mInterferencePower > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mInterferencePower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== interference_to_signal ===================================
class interference_to_signal_MPV : public MapPlotVariable
{
public:
   interference_to_signal_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~interference_to_signal_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if ((aResult.mRcvdPower > 0.0) && (aResult.mInterferencePower > 0.0))
      {
         value = UtMath::LinearToDB((aResult.mInterferencePower) / aResult.mRcvdPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== interference_to_mds ======================================
class interference_to_mds_MPV : public MapPlotVariable
{
public:
   interference_to_mds_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~interference_to_mds_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if ((aResult.mDetectionThreshold > 0.0) && (aResult.mRcvrNoisePower > 0.0) && (aResult.mInterferencePower))
      {
         value =
            UtMath::LinearToDB((aResult.mInterferencePower) / (aResult.mRcvrNoisePower * aResult.mDetectionThreshold));
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== jammer_power =============================================
class jammer_power_MPV : public MapPlotVariable
{
public:
   jammer_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~jammer_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((ewResult->mNoiseJammerPower > 0.0) || (ewResult->mPulseJammerPower > 0.0))
      {
         value = UtMath::LinearToDB(ewResult->mNoiseJammerPower + ewResult->mPulseJammerPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== jammer_to_signal =========================================
class jammer_to_signal_MPV : public MapPlotVariable
{
public:
   jammer_to_signal_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~jammer_to_signal_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((aResult.mRcvdPower > 0.0) && ((ewResult->mNoiseJammerPower > 0.0) || (ewResult->mPulseJammerPower > 0.0)))
      {
         value = UtMath::LinearToDB((ewResult->mNoiseJammerPower + ewResult->mPulseJammerPower) / aResult.mRcvdPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== jammer_to_mds ============================================
class jammer_to_mds_MPV : public MapPlotVariable
{
public:
   jammer_to_mds_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~jammer_to_mds_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((aResult.mDetectionThreshold > 0.0) && (aResult.mRcvrNoisePower > 0.0) &&
          ((ewResult->mNoiseJammerPower > 0.0) || (ewResult->mPulseJammerPower > 0.0)))
      {
         value = UtMath::LinearToDB((ewResult->mNoiseJammerPower + ewResult->mPulseJammerPower) /
                                    (aResult.mRcvrNoisePower * aResult.mDetectionThreshold));
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== noise_jammer_power =======================================
class noise_jammer_power_MPV : public MapPlotVariable
{
public:
   noise_jammer_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~noise_jammer_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if (ewResult->mNoiseJammerPower > 0.0)
      {
         value = UtMath::LinearToDB(ewResult->mNoiseJammerPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== noise_jammer_to_signal ===================================
class noise_jammer_to_signal_MPV : public MapPlotVariable
{
public:
   noise_jammer_to_signal_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~noise_jammer_to_signal_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((aResult.mRcvdPower > 0.0) && (ewResult->mNoiseJammerPower > 0.0))
      {
         value = UtMath::LinearToDB(ewResult->mNoiseJammerPower / aResult.mRcvdPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== pulse_jammer_power =======================================
class pulse_jammer_power_MPV : public MapPlotVariable
{
public:
   pulse_jammer_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~pulse_jammer_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if (ewResult->mPulseJammerPower > 0.0)
      {
         value = UtMath::LinearToDB(ewResult->mPulseJammerPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== pulse_jammer_to_signal ===================================
class pulse_jammer_to_signal_MPV : public MapPlotVariable
{
public:
   pulse_jammer_to_signal_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~pulse_jammer_to_signal_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((aResult.mRcvdPower > 0.0) && (ewResult->mPulseJammerPower > 0.0))
      {
         value = UtMath::LinearToDB(ewResult->mPulseJammerPower / aResult.mRcvdPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== coherent_jammer_power ====================================
class coherent_jammer_power_MPV : public MapPlotVariable
{
public:
   coherent_jammer_power_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~coherent_jammer_power_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if (ewResult->mCoherentJammerPower > 0.0)
      {
         value = UtMath::LinearToDB(ewResult->mCoherentJammerPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== coherent_jammer_to_noise =================================
class coherent_jammer_to_noise_MPV : public MapPlotVariable
{
public:
   coherent_jammer_to_noise_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~coherent_jammer_to_noise_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((ewResult->mCoherentJammerPower > 0.0) && (aResult.mRcvrNoisePower > 0.0))
      {
         value = UtMath::LinearToDB(ewResult->mCoherentJammerPower / aResult.mRcvrNoisePower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== coherent_jammer_to_signal ================================
class coherent_jammer_to_signal_MPV : public MapPlotVariable
{
public:
   coherent_jammer_to_signal_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~coherent_jammer_to_signal_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // If there wasn't a detection the EW Result may not be defined so need to FindOrCreate
      WsfEW_Result* ewResult = &WsfEW_Result::FindOrCreate(aResult);
      double        value    = aValue;
      if ((ewResult->mCoherentJammerPower > 0.0) && (aResult.mRcvdPower > 0.0))
      {
         value = UtMath::LinearToDB(ewResult->mCoherentJammerPower / aResult.mRcvdPower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== signal_to_noise ==========================================
class signal_to_noise_MPV : public MapPlotVariable
{
public:
   signal_to_noise_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~signal_to_noise_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if ((aResult.mRcvrNoisePower > 0.0) && (aResult.mRcvdPower > 0.0))
      {
         value = UtMath::LinearToDB(aResult.mRcvdPower / aResult.mRcvrNoisePower);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== signal_to_interference ===================================
class signal_to_interference_MPV : public MapPlotVariable
{
public:
   signal_to_interference_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~signal_to_interference_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mSignalToNoise > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mSignalToNoise);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== signal_at_target =========================================
class signal_at_target_MPV : public MapPlotVariable
{
public:
   signal_at_target_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~signal_at_target_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mPowerDensityAtTarget > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mPowerDensityAtTarget);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== pixel_count ==============================================
class pixel_count_MPV : public MapPlotVariable
{
public:
   pixel_count_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~pixel_count_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      return std::max(aValue, static_cast<double>(aResult.mPixelCount));
   }
};

// =========================== background_radient_intensity =============================
class background_radient_intensity_MPV : public MapPlotVariable
{
public:
   background_radient_intensity_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~background_radient_intensity_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mSignalToNoise > 0.0)
      {
         value = std::max(aValue, aResult.mBackgroundRadiantIntensity);
      }
      return value;
   }
};

// =========================== contrast_radient_intensity ===============================
class contrast_radient_intensity_MPV : public MapPlotVariable
{
public:
   contrast_radient_intensity_MPV()
      : MapPlotVariable(-999999.0)
   {
   }
   ~contrast_radient_intensity_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mSignalToNoise > 0.0)
      {
         value = std::max(aValue, aResult.mContrastRadiantIntensity);
      }
      return value;
   }
};

// =========================== attenuation_factor =======================================
class attenuation_factor_MPV : public MapPlotVariable
{
public:
   attenuation_factor_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~attenuation_factor_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      // TODO - needs to be evaluated.
      if (aResult.mAbsorptionFactor > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mAbsorptionFactor);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== propagation_factor =======================================
class propagation_factor_MPV : public MapPlotVariable
{
public:
   propagation_factor_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~propagation_factor_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mPropagationFactor > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mPropagationFactor);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== transmit_antenna_gain ====================================
class transmit_antenna_gain_MPV : public MapPlotVariable
{
public:
   transmit_antenna_gain_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~transmit_antenna_gain_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mXmtrBeam.mGain > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mXmtrBeam.mGain);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== receive_antenna_gain =====================================
class receive_antenna_gain_MPV : public MapPlotVariable
{
public:
   receive_antenna_gain_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~receive_antenna_gain_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.mRcvrBeam.mGain > 0.0)
      {
         value = UtMath::LinearToDB(aResult.mRcvrBeam.mGain);
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== transmit_gain_propagation_factor =========================
class transmit_gain_propagation_factor_MPV : public MapPlotVariable
{
public:
   transmit_gain_propagation_factor_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~transmit_gain_propagation_factor_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if ((aResult.mPropagationFactor > 0.0) && (aResult.mXmtrBeam.mGain > 0.0))
      {
         value = UtMath::LinearToDB(aResult.mXmtrBeam.mGain * sqrt(aResult.mPropagationFactor));
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== receive_gain_propagation_factor ==========================
class receive_gain_propagation_factor_MPV : public MapPlotVariable
{
public:
   receive_gain_propagation_factor_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~receive_gain_propagation_factor_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if ((aResult.mPropagationFactor > 0.0) && (aResult.mRcvrBeam.mGain > 0.0))
      {
         value = UtMath::LinearToDB(aResult.mRcvrBeam.mGain * sqrt(aResult.mPropagationFactor));
         value = std::max(aValue, value);
      }
      return value;
   }
};

// =========================== down_range ===============================================
class down_range_MPV : public MapPlotVariable
{
public:
   down_range_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~down_range_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      return mSharedDataPtr->GetDownRange();
   }
};

// =========================== cross_range ==============================================
class cross_range_MPV : public MapPlotVariable
{
public:
   cross_range_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~cross_range_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      return mSharedDataPtr->GetCrossRange();
   }
};

// =========================== ground_range =============================================
class ground_range_MPV : public MapPlotVariable
{
public:
   ground_range_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~ground_range_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (((aResult.mRcvrToTgt.mRange > 0.0) && (aResult.GetTarget() != nullptr)) ||
          ((aResult.mRcvrToXmtr.mRange > 0.0) && (aResult.GetTarget() == nullptr)))
      {
         // Approximate
         double groundRange = sqrt((pow(mSharedDataPtr->GetDownRange(), 2)) + (pow(mSharedDataPtr->GetCrossRange(), 2)));
         value              = std::max(aValue, (groundRange * mSharedDataPtr->GetRangeScale()));
      }
      return value;
   }
};

// =========================== slant_range ==============================================
class slant_range_MPV : public MapPlotVariable
{
public:
   slant_range_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~slant_range_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         if (aResult.mRcvrToTgt.mRange > 0.0)
         {
            value = std::max(aValue, aResult.mRcvrToTgt.mRange * mSharedDataPtr->GetRangeScale());
         }
      }
      else
      {
         if (aResult.mRcvrToXmtr.mRange > 0.0)
         {
            value = std::max(aValue, aResult.mRcvrToXmtr.mRange * mSharedDataPtr->GetRangeScale());
         }
      }
      return value;
   }
};

// =========================== azimuth_angle ============================================
class azimuth_angle_MPV : public MapPlotVariable
{
public:
   azimuth_angle_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~azimuth_angle_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         if (aResult.mRcvrToTgt.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mRcvrToTgt.mAz);
         }
      }
      else
      {
         if (aResult.mRcvrToXmtr.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mRcvrToXmtr.mAz);
         }
      }
      return value;
   }
};

// =========================== elevation_angle ==========================================
class elevation_angle_MPV : public MapPlotVariable
{
public:
   elevation_angle_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~elevation_angle_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         if (aResult.mRcvrToTgt.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mRcvrToTgt.mEl);
         }
      }
      else
      {
         if (aResult.mRcvrToXmtr.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mRcvrToXmtr.mEl);
         }
      }
      return value;
   }
};

// =========================== target_to_sensor_ground_range ============================
class target_to_sensor_ground_range_MPV : public MapPlotVariable
{
public:
   target_to_sensor_ground_range_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_to_sensor_ground_range_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor/target function - no min/max on result
      double value = aValue;
      if (((aResult.mTgtToRcvr.mRange > 0.0) && (aResult.GetTarget() != nullptr)) ||
          ((aResult.mXmtrToRcvr.mRange > 0.0) && (aResult.GetTarget() == nullptr)))
      {
         double groundRange = sqrt((pow(mSharedDataPtr->GetDownRange(), 2)) + (pow(mSharedDataPtr->GetCrossRange(), 2)));
         value              = std::max(aValue, groundRange * mSharedDataPtr->GetRangeScale());
      }
      return value;
   }
};

// =========================== target_to_sensor_slant_range =============================
class target_to_sensor_slant_range_MPV : public MapPlotVariable
{
public:
   target_to_sensor_slant_range_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_to_sensor_slant_range_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         if (aResult.mTgtToRcvr.mRange > 0.0)
         {
            value = std::max(aValue, aResult.mTgtToRcvr.mRange * mSharedDataPtr->GetRangeScale());
         }
      }
      else
      {
         if (aResult.mXmtrToRcvr.mRange > 0.0)
         {
            value = std::max(aValue, aResult.mXmtrToRcvr.mRange * mSharedDataPtr->GetRangeScale());
         }
      }
      return value;
   }
};

// =========================== target_to_sensor_azimuth_angle ===========================
class target_to_sensor_azimuth_angle_MPV : public MapPlotVariable
{
public:
   target_to_sensor_azimuth_angle_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_to_sensor_azimuth_angle_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor/target function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         if (aResult.mTgtToRcvr.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mTgtToRcvr.mAz);
         }
      }
      else
      {
         if (aResult.mXmtrToRcvr.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mXmtrToRcvr.mAz);
         }
      }
      return value;
   }
};

// =========================== target_to_sensor_elevation_angle =========================
class target_to_sensor_elevation_angle_MPV : public MapPlotVariable
{
public:
   target_to_sensor_elevation_angle_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_to_sensor_elevation_angle_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor/target function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         if (aResult.mTgtToRcvr.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mTgtToRcvr.mEl);
         }
      }
      else
      {
         if (aResult.mXmtrToRcvr.mRange > 0.0)
         {
            value = ConvertAndMakePrintableAngle(aResult.mXmtrToRcvr.mEl);
         }
      }
      return value;
   }
};

// =========================== sensor_latitude ==========================================
class sensor_latitude_MPV : public MapPlotVariable
{
public:
   sensor_latitude_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_latitude_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor/target function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         double notUsed;
         aResult.GetReceiver()->GetPlatform()->GetLocationLLA(aValue, notUsed, notUsed);
         value = aValue;
      }
      return value;
   }
};

// =========================== sensor_longitude =========================================
class sensor_longitude_MPV : public MapPlotVariable
{
public:
   sensor_longitude_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_longitude_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor/target function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         double notUsed;
         aResult.GetReceiver()->GetPlatform()->GetLocationLLA(notUsed, aValue, notUsed);
         value = aValue;
      }
      return value;
   }
};

// =========================== sensor_altitude ==========================================
class sensor_altitude_MPV : public MapPlotVariable
{
public:
   sensor_altitude_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_altitude_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         double notUsed;
         aResult.GetReceiver()->GetPlatform()->GetLocationLLA(notUsed, notUsed, aValue);
         value = aValue;
      }
      return value;
   }
};

// =========================== sensor_speed =============================================
class sensor_speed_MPV : public MapPlotVariable
{
public:
   sensor_speed_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_speed_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         value = aResult.GetReceiver()->GetPlatform()->GetSpeed();
      }
      return value;
   }
};

// =========================== sensor_heading ===========================================
class sensor_heading_MPV : public MapPlotVariable
{
public:
   sensor_heading_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_heading_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         double notUsed;
         aResult.GetReceiver()->GetPlatform()->GetOrientationNED(aValue, notUsed, notUsed);
         value = ConvertAndMakePrintableAngle(aValue);
         value = UtMath::NormalizeAngle0_360(value);
      }
      return value;
   }
};

// =========================== sensor_pitch =============================================
class sensor_pitch_MPV : public MapPlotVariable
{
public:
   sensor_pitch_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_pitch_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         double notUsed;
         aResult.GetReceiver()->GetPlatform()->GetOrientationNED(notUsed, aValue, notUsed);
         value = ConvertAndMakePrintableAngle(aValue);
      }
      return value;
   }
};

// =========================== sensor_roll ==============================================
class sensor_roll_MPV : public MapPlotVariable
{
public:
   sensor_roll_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~sensor_roll_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetReceiver() != nullptr)
      {
         double notUsed;
         aResult.GetReceiver()->GetPlatform()->GetOrientationNED(notUsed, notUsed, aValue);
         value = ConvertAndMakePrintableAngle(aValue);
      }
      return value;
   }
};

// =========================== target_latitude ==========================================
class target_latitude_MPV : public MapPlotVariable
{
public:
   target_latitude_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_latitude_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         double notUsed;
         aResult.GetTarget()->GetLocationLLA(aValue, notUsed, notUsed);
         value = aValue;
      }
      return value;
   }
};

// =========================== target_longitude =========================================
class target_longitude_MPV : public MapPlotVariable
{
public:
   target_longitude_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_longitude_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         double notUsed;
         aResult.GetTarget()->GetLocationLLA(notUsed, aValue, notUsed);
         value = aValue;
      }
      return value;
   }
};

// =========================== target_altitude ==========================================
class target_altitude_MPV : public MapPlotVariable
{
public:
   target_altitude_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_altitude_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         double notUsed;
         aResult.GetTarget()->GetLocationLLA(notUsed, notUsed, value);
      }
      return value;
   }
};

// =========================== target_speed =============================================
class target_speed_MPV : public MapPlotVariable
{
public:
   target_speed_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_speed_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         value = aResult.GetTarget()->GetSpeed();
      }
      return value;
   }
};

// =========================== target_heading ===========================================
class target_heading_MPV : public MapPlotVariable
{
public:
   target_heading_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_heading_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         double notUsed;
         aResult.GetTarget()->GetOrientationNED(aValue, notUsed, notUsed);
         value = ConvertAndMakePrintableAngle(aValue);
         value = UtMath::NormalizeAngle0_360(value);
      }
      return value;
   }
};

// =========================== target_pitch =============================================
class target_pitch_MPV : public MapPlotVariable
{
public:
   target_pitch_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_pitch_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         double notUsed;
         aResult.GetTarget()->GetOrientationNED(notUsed, aValue, notUsed);
         value = ConvertAndMakePrintableAngle(aValue);
      }
      return value;
   }
};

// =========================== target_roll ==============================================
class target_roll_MPV : public MapPlotVariable
{
public:
   target_roll_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~target_roll_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double value = aValue;
      if (aResult.GetTarget() != nullptr)
      {
         double notUsed;
         aResult.GetTarget()->GetOrientationNED(notUsed, notUsed, aValue);
         value = ConvertAndMakePrintableAngle(aValue);
      }
      return value;
   }
};

// =========================== angle ====================================================
class angle_MPV : public MapPlotVariable
{
public:
   angle_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~angle_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = aValue;
      // Single sensor function - no min/max on result
      // Compute the angle between the uncued sensor boresight and the target.
      // This is primarily used with AESA systems where one wants to know the angle between
      // the normal to the array face and the target.
      if (aResult.GetTarget() != nullptr)
      {
         WsfEM_Antenna* antennaPtr = aSensorPtr->GetEM_Rcvr(0).GetAntenna();
         // Get the WCS unit vector from the sensor to the target
         double snrToTgtLocWCS[3];
         aSensorPtr->GetRelativeLocationWCS(aResult.GetTarget(), snrToTgtLocWCS);
         double snrToTgtLocACS[3];
         antennaPtr->ConvertWCS_VectorToACS(snrToTgtLocWCS, snrToTgtLocACS);
         UtVec3d::Normalize(snrToTgtLocACS);
         double steeringAngle = acos(snrToTgtLocACS[0]);
         value                = ConvertAndMakePrintableAngle(steeringAngle);
      }
      return value;
   }
};

// =========================== grazing_angle ============================================
class grazing_angle_MPV : public MapPlotVariable
{
public:
   grazing_angle_MPV()
      : MapPlotVariable(-999.0)
   {
   }
   ~grazing_angle_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Compute ground-range & grazing angle wrt the antenna
      double relLocNED[3];
      if (aResult.GetTarget() != nullptr)
      {
         aResult.GetReceiver()->GetAntenna()->GetRelativeLocationNED(aResult.GetTarget(), relLocNED);
      }
      else
      {
         double xmtrLocation[3];
         aResult.GetTransmitter()->GetAntenna()->GetLocationWCS(xmtrLocation);
         aResult.GetReceiver()->GetAntenna()->GetRelativeLocationNED(xmtrLocation, relLocNED);
      }

      double rho;
      rho = sqrt((relLocNED[0] * relLocNED[0]) + (relLocNED[1] * relLocNED[1]));

      // Compute the grazing angle
      double grazingAngle;
      if (rho != 0.0)
      {
         grazingAngle = -(atan2(-rho, relLocNED[2]) + UtMath::cPI_OVER_2);
      }
      else
      {
         grazingAngle = UtMath::cPI_OVER_2;
         if (relLocNED[2] > 0.0)
         {
            grazingAngle = -UtMath::cPI_OVER_2;
         }
      }
      return ConvertAndMakePrintableAngle(grazingAngle);
   }
};

// =========================== sar_dwell_time ===========================================
class sar_dwell_time_MPV : public MapPlotVariable
{
public:
   sar_dwell_time_MPV()
      : MapPlotVariable(1.0E+10)
   {
   }
   ~sar_dwell_time_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double         value  = GetInitialValue();
      WsfSAR_Sensor* sarPtr = dynamic_cast<WsfSAR_Sensor*>(aSensorPtr);
      if (sarPtr != nullptr)
      {
         double dwellTime;
         double resolution;
         double clutterToNoise;
         double dopplerFactor;
         int    failureReasons;
         sarPtr->CanImageNow(aSimTime, dwellTime, resolution, clutterToNoise, dopplerFactor, failureReasons);
         value = dwellTime;
      }
      return std::min(aValue, value);
   }
};

// =========================== sar_resolution ===========================================
class sar_resolution_MPV : public MapPlotVariable
{
public:
   sar_resolution_MPV()
      : MapPlotVariable(1.0E+10)
   {
   }
   ~sar_resolution_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double         value  = GetInitialValue();
      WsfSAR_Sensor* sarPtr = dynamic_cast<WsfSAR_Sensor*>(aSensorPtr);
      if (sarPtr != nullptr)
      {
         double dwellTime;
         double resolution;
         double clutterToNoise;
         double dopplerFactor;
         int    failureReasons;
         sarPtr->CanImageNow(aSimTime, dwellTime, resolution, clutterToNoise, dopplerFactor, failureReasons);
         value = resolution;
      }
      return std::min(aValue, value);
   }
};

// =========================== sar_clutter_to_noise =====================================
class sar_clutter_to_noise_MPV : public MapPlotVariable
{
public:
   sar_clutter_to_noise_MPV()
      : MapPlotVariable(UtMath::LinearToDB(1.0E-30))
   {
   }
   ~sar_clutter_to_noise_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double         value  = 1.0;
      WsfSAR_Sensor* sarPtr = dynamic_cast<WsfSAR_Sensor*>(aSensorPtr);
      if (sarPtr != nullptr)
      {
         double dwellTime;
         double resolution;
         double clutterToNoise;
         double dopplerFactor;
         int    failureReasons;
         sarPtr->CanImageNow(aSimTime, dwellTime, resolution, clutterToNoise, dopplerFactor, failureReasons);
         value = clutterToNoise;
      }
      return std::max(aValue, UtMath::LinearToDB(value));
   }
};

// =========================== sar_doppler_foldover =====================================
class sar_doppler_foldover_MPV : public MapPlotVariable
{
public:
   sar_doppler_foldover_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~sar_doppler_foldover_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      // Single sensor function - no min/max on result
      double         value  = 1.0;
      WsfSAR_Sensor* sarPtr = dynamic_cast<WsfSAR_Sensor*>(aSensorPtr);
      if (sarPtr != nullptr)
      {
         double dwellTime;
         double resolution;
         double clutterToNoise;
         double dopplerFactor;
         int    failureReasons;
         sarPtr->CanImageNow(aSimTime, dwellTime, resolution, clutterToNoise, dopplerFactor, failureReasons);
         value = dopplerFactor;
      }
      return value;
   }
};

// =========================== sar_can_image ============================================
class sar_can_image_MPV : public MapPlotVariable
{
public:
   sar_can_image_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~sar_can_image_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double value = 0.0;

      double         dwellTime;
      double         resolution;
      double         clutterToNoise;
      double         dopplerFactor;
      int            failureReasons;
      WsfSAR_Sensor* sarPtr = dynamic_cast<WsfSAR_Sensor*>(aSensorPtr);
      if ((sarPtr != nullptr) &&
          sarPtr->CanImageNow(aSimTime, dwellTime, resolution, clutterToNoise, dopplerFactor, failureReasons))
      {
         value = 1.0;
      }
      return std::max(aValue, value);
   }
};

// =========================== time =====================================================
class time_MPV : public MapPlotVariable
{
public:
   time_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~time_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      return aSimTime;
   }
};

// =========================== masking_status ===========================================
class masking_status_MPV : public MapPlotVariable
{
public:
   masking_status_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~masking_status_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double       value = 0.0;
      unsigned int horizonMaskingFlags =
         WsfEM_Interaction::cRCVR_HORIZON_MASKING | WsfEM_Interaction::cXMTR_HORIZON_MASKING;
      unsigned int terrainMaskingFlags =
         WsfEM_Interaction::cRCVR_TERRAIN_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING;

      if ((aResult.mCheckedStatus & horizonMaskingFlags) == 0)
      {
         aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
         // Ensure we always check horizon when doing a masking status plot.
         // This condition is far less likely to be hit than terrain as
         // horizon is checked fairly early in the detection process
         if (WsfEM_Interaction::MaskedByHorizon(aResult.GetReceiver(), aResult.GetTarget(), aResult.mEarthRadiusScale))
         {
            aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
         }
      }

      if ((aResult.mCheckedStatus & terrainMaskingFlags) == 0)
      {
         // Ensure we always check terrain when doing a masking status plot.
         // Since Terrain is the most expensive and last check, there are
         // many other things that will cause the detection to fail first
         // and the terrain check to NOT be done
         aResult.MaskedByTerrain();
      }

      // Evaluate both horizon and terrain mask, but do terrain mask check
      // second as it overrides horizon mask
      if ((horizonMaskingFlags & aResult.mFailedStatus) != 0)
      {
         value = 1.0;
      }

      if ((terrainMaskingFlags & aResult.mFailedStatus) != 0)
      {
         value = 2.0;
      }
      return std::max(aValue, value);
   }
};

// =========================== land_cover ===============================================
class land_cover_MPV : public MapPlotVariable
{
public:
   land_cover_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~land_cover_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double lat       = aResult.mTgtLoc.mLat;
      double lon       = aResult.mTgtLoc.mLon;
      int    landCover = aSimulationPtr->GetScenario().GetEnvironment().GetLandCover(lat, lon);
      return static_cast<double>(landCover);
   }
};

// =========================== terrain_elevation ========================================
class terrain_elevation_MPV : public MapPlotVariable
{
public:
   terrain_elevation_MPV()
      : MapPlotVariable(0.0)
   {
   }
   ~terrain_elevation_MPV() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double       lat = aResult.mTgtLoc.mLat;
      double       lon = aResult.mTgtLoc.mLon;
      wsf::Terrain terrain(aSimulationPtr->GetTerrainInterface());
      float        elev;
      terrain.GetElevInterp(lat, lon, elev);
      return elev;
   }
};

// =========================== script ===================================================
class script_MPV : public MapPlotVariable
{
public:
   script_MPV(const std::string& aScriptName, double aVarInitValue)
      : MapPlotVariable(aVarInitValue)
      , mScriptName(aScriptName)
   {
   }
   ~script_MPV() override = default;

   bool Initialize(WsfSimulation& aSimulation, const SharedData& aSharedData) override
   {
      bool      ok        = MapPlotVariable::Initialize(aSimulation, aSharedData);
      UtScript* scriptPtr = FindScript(aSimulation, mScriptName);
      if (scriptPtr == nullptr)
      {
         ok       = false;
         auto out = ut::log::error() << "Unable to find script.";
         out.AddNote() << "Script: " << mScriptName;
      }
      else if (!ValidateScript(aSimulation, scriptPtr))
      {
         ok = false;

         auto out = ut::log::error() << "Script validation error occurred.";
         out.AddNote() << "Script: " << mScriptName;
         out.AddNote() << "Expected Signature: double(WsfPlatform, WsfSensor, WsfPlatform, WsfSensorInteraction)";
      }
      return ok;
   }

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulationPtr,
                   double           aValue,
                   WsfSensor*       aSensorPtr,
                   WsfSensorResult& aResult) const override
   {
      double             value = aValue;
      UtScriptData       retVal(value);
      UtScriptDataPacker scriptArgs(aSensorPtr->GetScenario().GetScriptTypes());
      scriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aResult.GetTarget();
      scriptArgs.Push(&aResult, "WsfSensorInteraction");
      scriptArgs << aValue;
      ExecuteScript(aSimTime, *aSensorPtr->GetSimulation(), mScriptName, retVal, scriptArgs);
      return retVal.GetDouble();
   }

private:
   std::string mScriptName;
};

// ======================================================================================
MapPlotVariables::MapPlotVariables() // Registering this function as capability
{
   RegisterMapPlotVariable("pd", std::make_shared<pd_MPV>());
   RegisterMapPlotVariable("detection_threshold", std::make_shared<detection_threshold_MPV>());
   RegisterMapPlotVariable("required_rcs", std::make_shared<required_rcs_MPV>());
   RegisterMapPlotVariable("rcs_required", mVariableMap.at("required_rcs"));
   RegisterMapPlotVariable("required_jamming_power", std::make_shared<required_jamming_power_MPV>());
   RegisterMapPlotVariable("required_jammer_power", mVariableMap.at("required_jamming_power"));
   RegisterMapPlotVariable("jamming_power_required", mVariableMap.at("required_jamming_power"));
   RegisterMapPlotVariable("jammer_power_required", mVariableMap.at("required_jamming_power"));
   RegisterMapPlotVariable("radar_signature", std::make_shared<radar_signature_MPV>());
   RegisterMapPlotVariable("optical_signature", std::make_shared<optical_signature_MPV>());
   RegisterMapPlotVariable("infrared_signature", std::make_shared<infrared_signature_MPV>());
   RegisterMapPlotVariable("signal_power", std::make_shared<signal_power_MPV>());
   RegisterMapPlotVariable("clutter_power", std::make_shared<clutter_power_MPV>());
   RegisterMapPlotVariable("noise_power", std::make_shared<noise_power_MPV>());
   RegisterMapPlotVariable("interference_power", std::make_shared<interference_power_MPV>());
   RegisterMapPlotVariable("interference_to_signal", std::make_shared<interference_to_signal_MPV>());
   RegisterMapPlotVariable("interference_to_mds", std::make_shared<interference_to_mds_MPV>());
   RegisterMapPlotVariable("jammer_power", std::make_shared<jammer_power_MPV>());
   RegisterMapPlotVariable("jammer_to_signal", std::make_shared<jammer_to_signal_MPV>());
   RegisterMapPlotVariable("jammer_to_mds", std::make_shared<jammer_to_mds_MPV>());
   RegisterMapPlotVariable("noise_jammer_power", std::make_shared<noise_jammer_power_MPV>());
   RegisterMapPlotVariable("noise_jammer_to_signal", std::make_shared<noise_jammer_to_signal_MPV>());
   RegisterMapPlotVariable("pulse_jammer_power", std::make_shared<pulse_jammer_power_MPV>());
   RegisterMapPlotVariable("pulse_jammer_to_signal", std::make_shared<pulse_jammer_to_signal_MPV>());
   RegisterMapPlotVariable("coherent_jammer_power", std::make_shared<coherent_jammer_power_MPV>());
   RegisterMapPlotVariable("coherent_jammer_to_noise", std::make_shared<coherent_jammer_to_noise_MPV>());
   RegisterMapPlotVariable("coherent_jammer_to_signal", std::make_shared<coherent_jammer_to_signal_MPV>());
   RegisterMapPlotVariable("signal_to_noise", std::make_shared<signal_to_noise_MPV>());
   RegisterMapPlotVariable("signal_to_interference", std::make_shared<signal_to_interference_MPV>());
   RegisterMapPlotVariable("signal_at_target", std::make_shared<signal_at_target_MPV>());
   RegisterMapPlotVariable("pixel_count", std::make_shared<pixel_count_MPV>());
   RegisterMapPlotVariable("background_radiant_intensity", std::make_shared<background_radient_intensity_MPV>());
   RegisterMapPlotVariable("contrast_radiant_intensity", std::make_shared<contrast_radient_intensity_MPV>());
   RegisterMapPlotVariable("attenuation_factor", std::make_shared<attenuation_factor_MPV>());
   RegisterMapPlotVariable("propagation_factor", std::make_shared<propagation_factor_MPV>());
   RegisterMapPlotVariable("transmit_antenna_gain", std::make_shared<transmit_antenna_gain_MPV>());
   RegisterMapPlotVariable("receive_antenna_gain", std::make_shared<receive_antenna_gain_MPV>());
   RegisterMapPlotVariable("transmit_gain_propagation_factor", std::make_shared<transmit_gain_propagation_factor_MPV>());
   RegisterMapPlotVariable("receive_gain_propagation_factor", std::make_shared<receive_gain_propagation_factor_MPV>());
   RegisterMapPlotVariable("down_range", std::make_shared<down_range_MPV>());
   RegisterMapPlotVariable("cross_range", std::make_shared<cross_range_MPV>());
   RegisterMapPlotVariable("ground_range", std::make_shared<ground_range_MPV>());
   RegisterMapPlotVariable("slant_range", std::make_shared<slant_range_MPV>());
   RegisterMapPlotVariable("azimuth_angle", std::make_shared<azimuth_angle_MPV>());
   RegisterMapPlotVariable("elevation_angle", std::make_shared<elevation_angle_MPV>());
   RegisterMapPlotVariable("target_to_sensor_ground_range", std::make_shared<target_to_sensor_ground_range_MPV>());
   RegisterMapPlotVariable("target_to_sensor_slant_range", std::make_shared<target_to_sensor_slant_range_MPV>());
   RegisterMapPlotVariable("target_to_sensor_azimuth_angle", std::make_shared<target_to_sensor_azimuth_angle_MPV>());
   RegisterMapPlotVariable("target_to_sensor_elevation_angle", std::make_shared<target_to_sensor_elevation_angle_MPV>());
   RegisterMapPlotVariable("sensor_latitude", std::make_shared<sensor_latitude_MPV>());
   RegisterMapPlotVariable("sensor_longitude", std::make_shared<sensor_longitude_MPV>());
   RegisterMapPlotVariable("sensor_altitude", std::make_shared<sensor_altitude_MPV>());
   RegisterMapPlotVariable("sensor_speed", std::make_shared<sensor_speed_MPV>());
   RegisterMapPlotVariable("sensor_heading", std::make_shared<sensor_heading_MPV>());
   RegisterMapPlotVariable("sensor_pitch", std::make_shared<sensor_pitch_MPV>());
   RegisterMapPlotVariable("sensor_roll", std::make_shared<sensor_roll_MPV>());
   RegisterMapPlotVariable("target_latitude", std::make_shared<target_latitude_MPV>());
   RegisterMapPlotVariable("target_longitude", std::make_shared<target_longitude_MPV>());
   RegisterMapPlotVariable("target_altitude", std::make_shared<target_altitude_MPV>());
   RegisterMapPlotVariable("target_speed", std::make_shared<target_speed_MPV>());
   RegisterMapPlotVariable("target_heading", std::make_shared<target_heading_MPV>());
   RegisterMapPlotVariable("target_pitch", std::make_shared<target_pitch_MPV>());
   RegisterMapPlotVariable("target_roll", std::make_shared<target_roll_MPV>());
   RegisterMapPlotVariable("angle", std::make_shared<angle_MPV>());
   RegisterMapPlotVariable("grazing_angle", std::make_shared<grazing_angle_MPV>());
   RegisterMapPlotVariable("sar_dwell_time", std::make_shared<sar_dwell_time_MPV>());
   RegisterMapPlotVariable("sar_resolution", std::make_shared<sar_resolution_MPV>());
   RegisterMapPlotVariable("sar_clutter_to_noise", std::make_shared<sar_clutter_to_noise_MPV>());
   RegisterMapPlotVariable("sar_doppler_foldover", std::make_shared<sar_doppler_foldover_MPV>());
   RegisterMapPlotVariable("sar_can_image", std::make_shared<sar_can_image_MPV>());
   RegisterMapPlotVariable("time", std::make_shared<time_MPV>());
   RegisterMapPlotVariable("masking_status", std::make_shared<masking_status_MPV>());
   RegisterMapPlotVariable("land_cover", std::make_shared<land_cover_MPV>());
   RegisterMapPlotVariable("terrain_elevation", std::make_shared<terrain_elevation_MPV>());
}

// =================================================================================================
bool MapPlotVariables::ProcessInput(UtInput& aInput)
{
   // Pass through all variables as each variable may need some of the data with same input command
   // TODO - This has issues though that need addressed as the input is popped off and the
   //        next MPV will not pick up the same command. Need to think about how multiple
   //        MPVs will process common data they may need to share properly. Perhaps moving common data
   //        back into this class and sharing it out somehow is better.
   bool myCommand = false;
   for (auto& mapPlotVar : mVariableMap)
   {
      if (mapPlotVar.second->ProcessInput(aInput))
      {
         myCommand = true;
         break;
      }
   }
   return myCommand;
}

// =================================================================================================
bool MapPlotVariables::ProcessScriptVariableInput(const std::string& aVarName, double aVarInitValue)
{
   auto iter = mVariableMap.find(aVarName);
   if (iter != mVariableMap.end()) // Check if user is reusing the name of a default variable.
   {
      mVariableMap.erase(iter);
      mVarLabels.erase(std::remove(mVarLabels.begin(), mVarLabels.end(), aVarName));

      auto out = ut::log::warning() << "Script variable overwrites an existing map plot variable type.";
      // If possible, the name of the script should be added here as a note.
      out.AddNote() << "Variable: " << aVarName;
   }
   RegisterMapPlotVariable(aVarName, std::make_shared<script_MPV>(aVarName, aVarInitValue));
   AddVariable(aVarName);
   return true;
}

// =================================================================================================
bool MapPlotVariables::Initialize(WsfSimulation& aSimulation)
{
   bool ok        = true;
   mSimulationPtr = &aSimulation;
   if (mVarLabels.empty())
   {
      AddVariable("pd");
   }
   // Only initialize the variables which are turned on for safety
   for (const auto& varLabel : mVarLabels)
   {
      ok &= mVariableMap.at(varLabel)->Initialize(aSimulation, *this);
   }
   return ok;
}

// =================================================================================================
void MapPlotVariables::InitializeValues(WsfSimulation&             aSimulation,
                                        std::vector<double>&       aVarValues,
                                        const std::vector<double>& aUserMinValues)
{
   aVarValues.resize(mVarLabels.size() + aUserMinValues.size());
   size_t j = 0;
   for (const auto& varLabel : mVarLabels)
   {
      aVarValues[j] = mVariableMap.at(varLabel)->GetInitialValue();
      ++j;
   }

   for (double userMinValue : aUserMinValues)
   {
      aVarValues[j] = userMinValue;
      ++j;
   }
}

// =================================================================================================
void MapPlotVariables::AddVariable(const std::string& aVarLabel)
{
   if (mVariableMap.find(aVarLabel) == std::end(mVariableMap))
   {
      throw std::invalid_argument("Invalid variable name");
   }
   mVarLabels.push_back(aVarLabel);
}

// =================================================================================================
void MapPlotVariables::Evaluate(double aSimTime, WsfSensor* aSensorPtr, WsfSensorResult& aResult, std::vector<double>& aVarValues)
{
   std::transform(mVarLabels.begin(),
                  mVarLabels.end(),
                  aVarValues.begin(),
                  aVarValues.begin(),
                  [&](const std::string& aLabel, const double aVarValue)
                  { return mVariableMap.at(aLabel)->Evaluate(aSimTime, mSimulationPtr, aVarValue, aSensorPtr, aResult); });
}

// =================================================================================================
size_t MapPlotVariables::GetLabelPosition(const std::string& aLabel) const
{
   auto itr = std::find(mVarLabels.begin(), mVarLabels.end(), aLabel);
   if (itr != mVarLabels.end())
   {
      return static_cast<size_t>(std::distance(mVarLabels.begin(), itr));
   }
   return ut::npos;
}

// =================================================================================================
bool MapPlotVariables::CheckSucceeded(WsfSensorResult& aResult)
{
   return ((aResult.mCheckedStatus != 0) && (aResult.mFailedStatus == 0));
}

// =================================================================================================
void MapPlotVariables::RegisterMapPlotVariable(const std::string& aVarName, std::shared_ptr<MapPlotVariable> aVariablePtr)
{
   auto valid = mVariableMap.emplace(aVarName, aVariablePtr);
   if (!valid.second)
   {
      throw UtException("***** ERROR: Attempting to register multiple '" + aVarName + "' plot variable extensions.");
   }
}
