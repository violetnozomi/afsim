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
#ifndef RVSIXDOFDATAINTERFACE_HPP
#define RVSIXDOFDATAINTERFACE_HPP

#include <set>

#include <QMutex>
#include <QObject>

#include "RvSixDOF_DataUpdater.hpp"

namespace rv
{
class ResultData;
}

namespace RvSixDOF_Data
{
class Interface : public QObject
{
   Q_OBJECT

public:
   explicit Interface(QObject* parent);
   ~Interface() override = default;

   /** Set the platform of interest, to see data about it.
    * @param aPlatformName platform of interest.
    */
   void SetPlatformOfInterest(const std::string& aPlatformName);

   /** Advance time and read new data.
    * @param aData data to read from
    */
   void AdvanceTimeRead(const rv::ResultData& aData);

   /** This is a container for SixDOF event pipe messages. */
   struct SixDOF_DataContainer
   {
      std::string  m_name;
      unsigned int m_platformIndex;

      // MsgSixDOF_CoreData
      double m_altitude_m;
      double m_vertSpeed_fpm;
      double m_KCAS;
      double m_KTAS;
      double m_mach;
      double m_heading_rad;
      double m_pitch_rad;
      double m_roll_rad;
      double m_gLoad;
      double m_alpha_rad;
      double m_gAvail;
      double m_speedbrakeAngle_rad;

      // MsgSixDOF_Kinematic
      double m_beta_rad;
      double m_alphaDot_rps;
      double m_betaDot_rps;
      double m_yawRate_rps;
      double m_pitchRate_rps;
      double m_rollRate_rps;
      double m_dynamicPressure_pa;
      double m_nx;
      double m_ny;
      double m_flightPathAngle_rad;
      double m_centerGravity_x;
      double m_centerGravity_y;
      double m_centerGravity_z;

      // MsgSixDOF_EngineFuel
      // Bit Flags:
      // Bit 0: afterburner_on
      // Bit 1: producing_thrust
      // Bit 2: engine_operating
      // Bit 3: engine_smoking
      // Bit 4: contrailing
      // Bit 5: weightOnWheels
      typedef UtPackBitset<1> Bitset;
      Bitset                  mBits;
      double                  m_fuelFlow_pph;
      double                  m_fuelInternal_kg;
      double                  m_fuelExternal_kg;
      double                  m_fuelInternalRemaining_kg;
      double                  m_fuelExternalRemaining_kg;
      double                  m_totalWeight_kg;
      double                  m_jokerFuelState_kg;
      double                  m_bingoFuelState_kg;

      // MsgSixDOF_Autopilot
      std::string m_activePilot;
      std::string m_lateralModeName;
      double      m_lateralModeValue;
      std::string m_verticalModeName;
      double      m_verticalModeValue;
      std::string m_speedModeName;
      double      m_speedModeValue;
      std::string m_currentManeuver;

      // MsgSixDOF_AutopilotLimits
      double m_pitchGLoadMin;
      double m_pitchGLoadMax;
      double m_alphaMin;
      double m_alphaMax;
      double m_pitchRateMin;
      double m_pitchRateMax;
      double m_verticalSpeedMin;
      double m_verticalSpeedMax;
      double m_yawGLoadMax;
      double m_betaMax;
      double m_yawRateMax;
      double m_rollRateMax;
      double m_bankAngleMax;
      double m_forwardAccelMin;
      double m_forwardAccelMax;
      double m_taxiSpeedMax;
      double m_taxiYawRateMax;

      // MsgSixDOF_ControlInputs
      double m_stickBack;
      double m_stickRight;
      double m_rudderRight;
      double m_throttle;
      double m_speedBrakeLever;
      double m_flapsLever;
      double m_landingGearLever;
      double m_spoilersLever;
      double m_thrustReverserLever;

      // MsgSixDOF_ControlSurfaces
      UtPackList<std::string> m_surfaceNames;
      UtPackList<double>      m_surfaceAngles;

      // MsgSixDOF_ForceMoment
      double m_liftForce_nt;
      double m_dragForce_nt;
      double m_thrustForce_nt;
      double m_sideForce_nt;
      double m_yawMoment;
      double m_pitchMoment;
      double m_rollMoment;
      double m_yawOmegaDotLimit;
      double m_pitchOmegaDotLimit;
      double m_rollOmegaDotLimit;

      /** Is this container valid? (Check for a platform name to see if we've loaded) */
      bool Valid() const { return !m_name.empty(); }
   };

   /** Get the data container, mSixDOF_Data. */
   const SixDOF_DataContainer GetSixDOF_Data();

   /** Get if SixDOF Data is being populated. */
   bool IsSixDOF_Visible();
   /** Get if Core Data is being populated. */
   bool IsCoreDataVisible();
   /** Get if Kinematic is being populated. */
   bool IsKinematicVisible();
   /** Get if Engine Fuel is being populated. */
   bool IsEngineFuelVisible();
   /** Get if Autopilot is being populated. */
   bool IsAutopilotVisible();
   /** Get if Autopilot Limits is being populated. */
   bool IsAutopilotLimitsVisible();
   /** Get if Control Inputs is being populated. */
   bool IsControlInputsVisible();
   /** Get if Control Surfaces is being populated. */
   bool IsControlSurfacesVisible();
   /** Get if Force Moment is being populated. */
   bool IsForceMomentVisible();
   /** Get if Force Rotation is being populated. */
   bool IsForceRotationVisible();

private:
   /** Enum for platform lookup. */
   enum
   {
      LOOKUP_REQUIRED = 0,
      SCENARIO        = -1
   };

   mutable QMutex       mMutex;
   SixDOF_DataContainer mSixDOF_Data;
   int                  mIndex;

   // Visibility values
   bool mSixDOF_Visible;
   bool mCoreDataVisible;
   bool mKinematicVisible;
   bool mEngineFuelVisible;
   bool mAutopilotVisible;
   bool mAutopilotLimitsVisible;
   bool mControlInputsVisible;
   bool mControlSurfacesVisible;
   bool mForceMomentVisible;
   bool mForceRotationVisible;
};
} // namespace RvSixDOF_Data
#endif // RVSIXDOFDATAINTERFACE_HPP
