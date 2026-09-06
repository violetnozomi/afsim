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

#include "SOSM_Interaction.hpp"

#include <algorithm>
#include <iomanip>

#include "SOSM_Sensor.hpp"
#include "SOSM_Target.hpp"
#include "UtAtmosphere.hpp"
#include "UtMath.hpp"

// cppcheck-suppress uninitMemberVar
SOSM_Interaction::SOSM_Interaction()
   : mTargetStateIndex(0)
   , mTransmittanceScaleFactor(1.0F)
   , mForegroundScaleFactor(1.0F)
   , mBackgroundScaleFactor(1.0F)
{
}

// virtual
void SOSM_Interaction::Print(ut::log::MessageStream& aMsgStream,
                             const UtAtmosphere&     aAtmosphere,
                             SOSM_Sensor*            aSensorPtr,
                             SOSM_Target*            aTargetPtr)
{
   UtAtmosphere atmosphere(aAtmosphere);
   float        in2_per_cm2 = 1.0F / 6.4516F;
   float        targetArea  = std::max(mBodyArea + mPlumeArea, 0.01F);
   float        area;
   float        intensity;
   float        azimuth;
   float        elevation;

   const SOSM_Target::StateBase* statePtr = aTargetPtr->StateEntry(mTargetStateIndex);
   aMsgStream << "Detection Results:";
   aMsgStream.AddNote() << "Target State: " << statePtr->mStateName;

   auto logNote = aMsgStream.AddNote() << "Observer Altitude: " << GetSensorAltitude() << " m";
   logNote.AddNote() << "Sensor Alt: " << GetSensorAltitude() * UtMath::cFT_PER_M << " ft";

   aMsgStream.AddNote() << "Observer->Target Elevation: " << GetAbsoluteTargetElevation() * UtMath::cDEG_PER_RAD << " deg";
   auto logSlantR = aMsgStream.AddNote() << "Observer->Target Range: " << GetSlantRange() << " m";
   logSlantR.AddNote() << "Slant Range in ft " << GetSlantRange() * UtMath::cFT_PER_M << " ft";
   logSlantR.AddNote() << "Slant Range in nm " << GetSlantRange() * UtMath::cFT_PER_M / UtMath::cFT_PER_NM << " nm";

   auto logTargAlt = aMsgStream.AddNote() << "Target Altitude: " << GetTargetAltitude() << " m";
   logTargAlt.AddNote() << "Target Altitude in ft: " << GetTargetAltitude() * UtMath::cFT_PER_M << " ft";

   float mach = static_cast<float>(GetTargetSpeed() / atmosphere.SonicVelocity(GetTargetAltitude()));
   aMsgStream.AddNote() << "Target Speed: Mach - " << mach;
   GetTargetToSensorAspect(azimuth, elevation);
   aMsgStream.AddNote() << "Target->Observer Azimuth: " << azimuth * UtMath::cDEG_PER_RAD << " deg";
   aMsgStream.AddNote() << "Target->Observer Elevation: " << elevation * UtMath::cDEG_PER_RAD << " deg";
   area                 = mBodyArea;
   auto logTargBodyData = aMsgStream.AddNote() << "Target Body Area: " << area << " cm^2";
   logTargBodyData.AddNote() << "Area in Meters: " << area * 1.0E-4F << " m^2";
   logTargBodyData.AddNote() << "Area in Inches: " << area * in2_per_cm2 << " in^2";

   area              = mPlumeArea;
   auto trgPlumeArea = aMsgStream.AddNote() << "Target Plume Area: " << area << " cm^2";
   trgPlumeArea.AddNote() << "Area in Meters: " << area * 1.0E-4F << " m^2";
   trgPlumeArea.AddNote() << "Area in Inches: " << area * in2_per_cm2 << " in^2";
   area = mBodyArea + mPlumeArea;

   auto trgTotalArea = aMsgStream.AddNote() << "Target Total Area: " << area << " cm^2";
   trgTotalArea.AddNote() << "Area in Meters: " << area * 1.0E-4F << " m^2";
   trgTotalArea.AddNote() << "Area in Inches: " << area * in2_per_cm2 << " in^2";
   intensity = mBodyIntensity;
   aMsgStream.AddNote() << "Target Body Intensity: " << std::scientific << std::setprecision(6) << intensity << " W/sr";
   intensity = mPlumeIntensity;
   aMsgStream.AddNote() << "Target Plume Intensity: " << std::scientific << std::setprecision(6) << intensity << " W/sr";
   intensity = mBodyIntensity + mPlumeIntensity;
   aMsgStream.AddNote() << "Target Total Intensity: " << std::scientific << std::setprecision(6) << intensity << " W/sr";
   aMsgStream.AddNote() << "Background Radiance (L_B): " << std::scientific << std::setprecision(6)
                        << mBackgroundRadiance << " W/cm^2/sr";
   aMsgStream.AddNote() << "Foreground Radiance (L_F): " << std::scientific << std::setprecision(6)
                        << mForegroundRadiance << " W/cm^2/sr";
   aMsgStream.AddNote() << "Background Intensity (I_B): " << std::scientific << std::setprecision(6)
                        << mBackgroundRadiance * targetArea << " W/sr";
   aMsgStream.AddNote() << "Foreground Intensity (I_F): " << std::scientific << std::setprecision(6)
                        << mForegroundRadiance * targetArea << " W/sr";
   aMsgStream.AddNote() << "Contrast Intensity (I_C): " << std::scientific << std::setprecision(6) << mContrastIntensity
                        << " W/sr";
   aMsgStream.AddNote() << "Transmittance (tau): " << std::fixed << std::setprecision(8) << mTransmittance;
}
