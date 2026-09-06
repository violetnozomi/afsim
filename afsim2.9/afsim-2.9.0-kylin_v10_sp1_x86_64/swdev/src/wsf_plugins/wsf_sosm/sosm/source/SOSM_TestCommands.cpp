// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_TestCommands.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#include "SOSM_Atmosphere.hpp"
#include "SOSM_BlackBody.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Sensor.hpp"
#include "SOSM_SensorTarget.hpp"
#include "SOSM_SimpleInteraction.hpp"
#include "SOSM_Target.hpp"
#include "SOSM_Utility.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

// ================================================================================================
// static
bool SOSM_TestCommands::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() != "test")
   {
      return false;
   }

   std::string testName;
   aInput.ReadValue(testName);
   if (testName == "blackbody_plot")
   {
      Test_BlackBodyPlot();
   }
   else if (testName == "simple_target_plot")
   {
      Test_SimpleTargetPlot();
   }
   else if (testName == "query_atmosphere")
   {
      Test_QueryAtmosphere(aInput);
   }
   else if (testName == "query_target")
   {
      Test_QueryTarget(aInput);
   }
   else if (testName == "query_detect")
   {
      Test_QueryDetect(aInput);
   }
   else
   {
      throw UtInput::BadValue(aInput, "Invalid test: " + testName);
   }
   return true;
}


// ================================================================================================
// Produce a plot file for comparing against the plot in Hudson's book.
// private static
void SOSM_TestCommands::Test_BlackBodyPlot()
{
   std::ofstream out("blackbody.plt");
   for (int iTemperature = 500; iTemperature <= 900; iTemperature += 100)
   {
      double temperature = iTemperature;
      out << "# Temperature: " << temperature << " deg-K" << std::endl;
      for (int i = 100; i < 1500; ++i)
      {
         double wavelength_um = i * 0.01;
         double exitance      = SOSM_BlackBody::SpectralRadiantExitance(temperature, wavelength_um);
         out << wavelength_um << ' ' << exitance << '\n';
      }
      out << "\n\n";
   }
}

// ================================================================================================
// Replicate the plot of the simple target model from the Joe Samocha's PowerPoint.
// private static
void SOSM_TestCommands::Test_SimpleTargetPlot()
{
   std::unique_ptr<SOSM_Sensor> mwSensorPtr(mManagerPtr->AllocateSensor("MWIR_SENSOR"));
   std::unique_ptr<SOSM_Sensor> lwSensorPtr(mManagerPtr->AllocateSensor("LWIR_SENSOR"));
   std::unique_ptr<SOSM_Target> targetPtr(mManagerPtr->AllocateTarget("TARGET"));
   if ((mwSensorPtr.get() == nullptr) || (lwSensorPtr.get() == nullptr) || (targetPtr.get() == nullptr))
   {
      ut::log::error() << "Necessary object types do not exist!";
      return;
   }

   SOSM_SimpleInteraction interaction;

   std::ofstream out("simple_target.plt");
   UtAtmosphere  atm(mManagerPtr->GetUtAtmosphere());
   double        alt = 9144.0; // 30 kft
   double        sos = atm.SonicVelocity(alt);
   for (int iMach = 000; iMach <= 400; ++iMach)
   {
      double mach  = iMach * 0.01;
      double speed = mach * sos;
      interaction.SetTargetAltitude(static_cast<float>(alt));
      interaction.SetTargetSpeed(static_cast<float>(speed));
      targetPtr->SelectState(interaction);

      float mwIntensity;
      float mwArea;
      mwSensorPtr->ComputeIntegratedTargetData(targetPtr.get(), interaction, mwIntensity, mwArea);

      float lwIntensity;
      float lwArea;
      lwSensorPtr->ComputeIntegratedTargetData(targetPtr.get(), interaction, lwIntensity, lwArea);

      out << mach << ' ' << mwIntensity << ' ' << lwIntensity << '\n';
   }
}

// ================================================================================================
// Query the default atmosphere
// private static
void SOSM_TestCommands::Test_QueryAtmosphere(UtInput& aInput)
{
   double       snrAlt        = 0.0;
   double       snrToTgtEl    = 0.0;
   double       snrToTgtRange = 0.0;
   UtInputBlock inputBlock(aInput);
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "sensor_altitude")
      {
         aInput.ReadValueOfType(snrAlt, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(snrAlt, 0.0);
      }
      else if (command == "sensor_to_target_range")
      {
         aInput.ReadValueOfType(snrToTgtRange, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(snrToTgtRange, 0.0);
      }
      else if (command == "sensor_to_target_elevation")
      {
         aInput.ReadValueOfType(snrToTgtEl, UtInput::cANGLE);
         aInput.ValueInClosedRange(snrToTgtEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   std::unique_ptr<SOSM_Atmosphere> atmospherePtr(mManagerPtr->AllocateDefaultAtmosphere());
   if (atmospherePtr.get() == nullptr)
   {
      ut::log::error() << "Unable to create instance of default atmosphere.";
      return;
   }
   auto logInfo = ut::log::info() << "Testing Query Atmosphere:";

   auto logNote = logInfo.AddNote() << "Atmosphere: " << atmospherePtr->GetTypeName();
   logInfo.AddNote() << "Origin: " << atmospherePtr->Origin();
   logInfo.AddNote() << "Increment: " << atmospherePtr->Increment();
   logInfo.AddNote() << "Count: " << atmospherePtr->Count();

   SOSM_Selector selector(*atmospherePtr);

   std::unique_ptr<SOSM_Sensor> sensorPtr(mManagerPtr->AllocateSensor("SENSOR"));
   if (sensorPtr.get() != nullptr)
   {
      selector.Intersect(*atmospherePtr, *sensorPtr);
      auto logSensor = logInfo.AddNote() << "Sensor: " << sensorPtr->GetTypeName();
      logSensor.AddNote() << "Origin: " << sensorPtr->Origin();
      logSensor.AddNote() << "Increment: " << sensorPtr->Increment();
      logSensor.AddNote() << "Count: " << sensorPtr->Count();
   }

   logInfo.AddNote() << "Snr Alt: " << snrAlt << " m; " << snrAlt * UtMath::cFT_PER_M << " ft";
   logInfo.AddNote() << "Snr->Tgt El: " << snrToTgtEl * UtMath::cDEG_PER_RAD << " deg";
   logInfo.AddNote() << "Snr->Tgt Range: " << snrToTgtRange << " m; ";
   logInfo.AddNote() << snrToTgtRange * UtMath::cFT_PER_M << " ft; ";
   logInfo.AddNote() << snrToTgtRange * UtMath::cFT_PER_M / UtMath::cFT_PER_NM << " nm";

   SOSM_SimpleInteraction interaction;
   std::vector<float>     bgRadiance;
   std::vector<float>     fgRadiance;
   std::vector<float>     fgTransmittance;

   interaction.SetSensorAltitude(static_cast<float>(snrAlt));
   interaction.SetAbsoluteTargetElevation(static_cast<float>(snrToTgtEl));
   interaction.SetSlantRange(static_cast<float>(snrToTgtRange));
   atmospherePtr->ComputeAtmosphereData(selector, interaction, bgRadiance, fgRadiance, fgTransmittance);


   SOSM_Utility::PrintSpectralVector(logInfo, selector, bgRadiance, "Background Radiance (W/cm^2/sr/um)");
   SOSM_Utility::PrintSpectralVector(logInfo, selector, fgRadiance, "Foreground Radiance (W/cm^2/sr/um)");
   SOSM_Utility::PrintSpectralVector(logInfo, selector, fgTransmittance, "Foreground Transmittance");

   if (sensorPtr.get() != nullptr)
   {
      float backgroundRadiance_xR;
      float foregroundRadiance_xR;
      float foregroundTransmittance_xR;
      sensorPtr->ComputeIntegratedAtmosphericData(interaction,
                                                  backgroundRadiance_xR,
                                                  foregroundRadiance_xR,
                                                  foregroundTransmittance_xR);
      auto logThis = logInfo.AddNote() << "Integrated Values";
      logThis.AddNote() << "Background Radiance: " << backgroundRadiance_xR << " W/cm^2/sr";
      logThis.AddNote() << "Foreground Radiance: " << foregroundRadiance_xR << " W/cm^2/sr";
      logThis.AddNote() << "Foreground Transmittance: " << foregroundTransmittance_xR;
   }
}

// ================================================================================================
// private static
void SOSM_TestCommands::Test_QueryTarget(UtInput& aInput)
{
   double       tgtAlt      = 0.0;
   double       tgtSpeed    = 0.0;
   double       tgtThrottle = 0.5;
   double       tgtToSnrAz  = 0.0;
   double       tgtToSnrEl  = 0.0;
   UtInputBlock inputBlock(aInput);
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "target_altitude")
      {
         aInput.ReadValueOfType(tgtAlt, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(tgtAlt, 0.0);
      }
      else if (command == "target_speed")
      {
         aInput.ReadValueOfType(tgtSpeed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(tgtSpeed, 0.0);
      }
      else if (command == "target_mach")
      {
         aInput.ReadValue(tgtSpeed);
         aInput.ValueGreater(tgtSpeed, 0.0);
         tgtSpeed = -tgtSpeed;
      }
      else if (command == "target_throttle")
      {
         aInput.ReadValue(tgtThrottle);
         aInput.ValueInClosedRange(tgtThrottle, 0.0, 1.0);
      }
      else if (command == "target_to_sensor_azimuth")
      {
         aInput.ReadValueOfType(tgtToSnrAz, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtToSnrAz, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "target_to_sensor_elevation")
      {
         aInput.ReadValueOfType(tgtToSnrEl, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtToSnrEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   std::unique_ptr<SOSM_Target> targetPtr(mManagerPtr->AllocateTarget("TARGET"));
   std::unique_ptr<SOSM_Sensor> sensorPtr(mManagerPtr->AllocateSensor("SENSOR"));
   if ((sensorPtr.get() == nullptr) || (targetPtr.get() == nullptr))
   {
      ut::log::error() << "Either TARGET or SENSOR type not defined!";
      return;
   }

   auto logTrgSens = ut::log::info() << "Target and sensor data.";
   { // RAII block
      auto note = logTrgSens.AddNote() << "Target spectrum:";
      note.AddNote() << "Origin: " << targetPtr->Origin();
      note.AddNote() << "Increment: " << targetPtr->Increment();
      note.AddNote() << "Count: " << targetPtr->Count();
   }

   { // RAII block
      auto note = logTrgSens.AddNote() << "Sensor spectrum:";
      note.AddNote() << "Origin: " << sensorPtr->Origin();
      note.AddNote() << "Increment: " << sensorPtr->Increment();
      note.AddNote() << "Count: " << sensorPtr->Count();
   }

   SOSM_SimpleInteraction interaction;
   UtAtmosphere           atm(mManagerPtr->GetUtAtmosphere());

   interaction.SetTargetAltitude(static_cast<float>(tgtAlt));
   if (tgtSpeed < 0.0)
   {
      tgtSpeed = -tgtSpeed * atm.SonicVelocity(tgtAlt);
   }
   interaction.SetTargetSpeed(static_cast<float>(tgtSpeed));
   interaction.SetTargetThrottle(static_cast<float>(tgtThrottle));
   interaction.SetTargetToSensorAspect(static_cast<float>(tgtToSnrAz), static_cast<float>(tgtToSnrEl));

   logTrgSens.AddNote() << "Tgt Alt: " << tgtAlt << " m; " << tgtAlt * UtMath::cFT_PER_M << " ft";
   logTrgSens.AddNote() << "Tgt->Snr Az: " << tgtToSnrAz * UtMath::cDEG_PER_RAD << " deg";
   logTrgSens.AddNote() << "Tgt->Snr El: " << tgtToSnrEl * UtMath::cDEG_PER_RAD << " deg";
   { // RAII block
      auto note = logTrgSens.AddNote() << "Tgt Speed: " << tgtSpeed << " m/s";
      note.AddNote() << tgtSpeed * UtMath::cNMPH_PER_MPS << " kts";
      note.AddNote() << "Mach: " << tgtSpeed / atm.SonicVelocity(tgtAlt);
   }
   logTrgSens.AddNote() << "Tgt Throttle: " << tgtThrottle;
   std::vector<float> bodyIntensity;
   std::vector<float> plumeIntensity;
   float              bodyArea;
   float              plumeArea;
   size_t             stateIndex = targetPtr->SelectState(interaction);
   logTrgSens.AddNote() << "State: " << targetPtr->StateEntry(stateIndex)->mStateName;
   targetPtr->ComputeRadiantIntensity(*sensorPtr, interaction, bodyIntensity, bodyArea, plumeIntensity, plumeArea);
   { // RAII block
      auto note = logTrgSens.AddNote() << "Body:";
      note.AddNote() << "Projected Area: " << bodyArea << " cm^2";
      SOSM_Utility::PrintSpectralVector(note, *sensorPtr, bodyIntensity, "Intensity (W/sr/um)");
   }
   { // RAII block
      auto note = logTrgSens.AddNote() << "Plume:";
      note.AddNote() << "Projected Area: " << plumeArea << " cm^2";
      SOSM_Utility::PrintSpectralVector(note, *sensorPtr, plumeIntensity, "Intensity (W/sr/um)");
   }
}

// ================================================================================================
// private static
void SOSM_TestCommands::Test_QueryDetect(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_test");
   std::string  command;
   double       tgtLat      = 0.0;
   double       tgtLon      = 0.0;
   double       tgtAlt      = 0.0;
   double       tgtHeading  = 0.0;
   double       tgtYaw      = 0.0;
   double       tgtPitch    = 0.0;
   double       tgtRoll     = 0.0;
   double       tgtSpeed    = 0.0;
   double       tgtThrottle = 0.5;

   double snrLat     = 0.0;
   double snrLon     = 0.0;
   double snrAlt     = 0.0;
   double snrHeading = -UtMath::cPI;
   double snrYaw     = 0.0;
   double snrPitch   = 0.0;
   double snrRoll    = 0.0;

   double tgtToSnrRange = -1.0;
   double tgtToSnrAz    = 0.0;
   double tgtToSnrEl    = 0.0;
   double snrToTgtRange = -1.0;
   double snrToTgtAz    = 0.0;
   double snrToTgtEl    = 0.0;

   while (inputBlock.ReadCommand(command))
   {
      if (command == "target_position")
      {
         aInput.ReadValueOfType(tgtLat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(tgtLon, UtInput::cLONGITUDE);
      }
      else if (command == "target_altitude")
      {
         aInput.ReadValueOfType(tgtAlt, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(tgtAlt, 0.0);
      }
      else if (command == "target_speed")
      {
         aInput.ReadValueOfType(tgtSpeed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(tgtSpeed, 0.0);
      }
      else if (command == "target_mach")
      {
         aInput.ReadValue(tgtSpeed);
         aInput.ValueGreater(tgtSpeed, 0.0);
         tgtSpeed = -tgtSpeed;
      }
      else if (command == "target_throttle")
      {
         aInput.ReadValue(tgtThrottle);
         aInput.ValueInClosedRange(tgtThrottle, 0.0, 1.0);
      }
      else if (command == "target_heading")
      {
         aInput.ReadValueOfType(tgtHeading, UtInput::cANGLE);
         if (tgtHeading < 0.0)
         {
            tgtHeading += UtMath::cTWO_PI;
         }
         aInput.ValueInClosedRange(tgtHeading, 0.0, UtMath::cTWO_PI);
      }
      else if (command == "target_yaw")
      {
         aInput.ReadValueOfType(tgtYaw, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtYaw, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "target_pitch")
      {
         aInput.ReadValueOfType(tgtPitch, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtPitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else if (command == "target_roll")
      {
         aInput.ReadValueOfType(tgtRoll, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtRoll, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "sensor_position")
      {
         aInput.ReadValueOfType(snrLat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(snrLon, UtInput::cLONGITUDE);
      }
      else if (command == "sensor_altitude")
      {
         aInput.ReadValueOfType(snrAlt, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(snrAlt, 0.0);
      }
      else if (command == "sensor_heading")
      {
         aInput.ReadValueOfType(snrHeading, UtInput::cANGLE);
         if (tgtHeading < 0.0)
         {
            snrHeading += UtMath::cTWO_PI;
         }
         aInput.ValueInClosedRange(snrHeading, 0.0, UtMath::cTWO_PI);
      }
      else if (command == "sensor_yaw")
      {
         aInput.ReadValueOfType(snrYaw, UtInput::cANGLE);
         aInput.ValueInClosedRange(snrYaw, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "sensor_pitch")
      {
         aInput.ReadValueOfType(snrPitch, UtInput::cANGLE);
         aInput.ValueInClosedRange(snrPitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else if (command == "sensor_roll")
      {
         aInput.ReadValueOfType(snrRoll, UtInput::cANGLE);
         aInput.ValueInClosedRange(snrRoll, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "sensor_to_target_range")
      {
         aInput.ReadValueOfType(snrToTgtRange, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(snrToTgtRange, 0.0);
      }
      else if (command == "sensor_to_target_azimuth")
      {
         aInput.ReadValueOfType(snrToTgtAz, UtInput::cANGLE);
         aInput.ValueInClosedRange(snrToTgtAz, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "sensor_to_target_elevation")
      {
         aInput.ReadValueOfType(snrToTgtEl, UtInput::cANGLE);
         aInput.ValueInClosedRange(snrToTgtEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else if (command == "target_to_sensor_range")
      {
         aInput.ReadValueOfType(tgtToSnrRange, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(tgtToSnrRange, 0.0);
      }
      else if (command == "target_to_sensor_azimuth")
      {
         aInput.ReadValueOfType(tgtToSnrAz, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtToSnrAz, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "target_to_sensor_elevation")
      {
         aInput.ReadValueOfType(tgtToSnrEl, UtInput::cANGLE);
         aInput.ValueInClosedRange(tgtToSnrEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   std::unique_ptr<SOSM_SensorTarget> pairPtr(mManagerPtr->AllocateSensorTargetPair("SENSOR", "TARGET"));
   if ((pairPtr->GetSensor() == nullptr) || (pairPtr->GetTarget() == nullptr))
   {
      ut::log::error() << "Either TARGET or SENSOR type not defined!";
      return;
   }

   // Build the interaction object.

   SOSM_SimpleInteraction interaction;

   if (tgtSpeed < 0.0)
   {
      UtAtmosphere atm(mManagerPtr->GetUtAtmosphere());
      tgtSpeed = -tgtSpeed * atm.SonicVelocity(tgtAlt);
   }
   interaction.SetTargetSpeed(static_cast<float>(tgtSpeed));
   interaction.SetTargetThrottle(static_cast<float>(tgtThrottle));

   // Add the heading and yaw to come up with a composite heading angle.
   tgtHeading = UtMath::NormalizeAngleMinusPi_Pi(tgtHeading + tgtYaw);
   snrHeading = UtMath::NormalizeAngleMinusPi_Pi(snrHeading + snrYaw);

   if (tgtToSnrRange > 0.0)
   {
      // Compute the location of the sensor with respect to the target.
      UtEntity tgt;
      tgt.SetLocationLLA(tgtLat, tgtLon, tgtAlt);
      tgt.SetOrientationNED(tgtHeading, tgtPitch, tgtRoll);
      double tgtToSnrLocWCS[3];
      tgt.GetRelativeLocationWCS(tgtToSnrAz, tgtToSnrEl, tgtToSnrRange, tgtToSnrLocWCS);
      double snrLocWCS[3];
      tgt.GetLocationWCS(snrLocWCS);
      UtVec3d::Add(snrLocWCS, snrLocWCS, tgtToSnrLocWCS);
      UtEntity snr;
      snr.SetLocationWCS(snrLocWCS);
      snr.GetLocationLLA(snrLat, snrLon, snrAlt);
   }
   else if (snrToTgtRange > 0.0)
   {
      // Compute the location of the target with respect to the sensor.
      UtEntity snr;
      snr.SetLocationLLA(snrLat, snrLon, snrAlt);
      snr.SetOrientationNED(snrHeading, snrPitch, snrRoll);
      double snrToTgtLocWCS[3];
      snr.GetRelativeLocationWCS(snrToTgtAz, snrToTgtEl, snrToTgtRange, snrToTgtLocWCS);
      double tgtLocWCS[3];
      snr.GetLocationWCS(tgtLocWCS);
      UtVec3d::Add(tgtLocWCS, tgtLocWCS, snrToTgtLocWCS);
      UtEntity tgt;
      tgt.SetLocationWCS(tgtLocWCS);
      tgt.GetLocationLLA(tgtLat, tgtLon, tgtAlt);
   }

   // Compute the rest of the geometry.

   interaction.ComputeGeometry(snrLat,
                               snrLon,
                               static_cast<float>(snrAlt),
                               static_cast<float>(snrHeading),
                               static_cast<float>(snrPitch),
                               static_cast<float>(snrRoll),
                               tgtLat,
                               tgtLon,
                               static_cast<float>(tgtAlt),
                               static_cast<float>(tgtHeading),
                               static_cast<float>(tgtPitch),
                               static_cast<float>(tgtRoll));

   // Perform the detection attempt.

   float targetIrradiance = pairPtr->ComputeTargetIrradiance(interaction);
   float pd               = pairPtr->ComputeProbabilityOfDetection(targetIrradiance, interaction);

   if (mManagerPtr->DebugLevel() == 0) // Following is already printed if debug enabled
   {
      auto logDebug = ut::log::debug() << "SOSM_TestCommands::Test_QueryDetect";
      interaction.Print(logDebug, mManagerPtr->GetUtAtmosphere(), pairPtr->GetSensor(), pairPtr->GetTarget());
      logDebug.AddNote() << "Target Irradiance: " << targetIrradiance << " W/cm^2";
      logDebug.AddNote() << "Pd: " << pd;
   }
}

SOSM_TestCommands::SOSM_TestCommands(SOSM_Manager* aManagerPtr)
{
   mManagerPtr = aManagerPtr;
}
