// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <map>
#include <QTextStream>
#include <string>
#include <vector>

#include "AeroBody.hpp"
#include "GeometryEngine.hpp"
#include "P6DofMassProperties.hpp"
#include "UtAtmosphere.hpp"
#include "UtDCM.hpp"
#include "UtVec3dX.hpp"
#include "VehicleAero.hpp"
#include "VehicleControls.hpp"
#include "VehicleGeometry.hpp"
#include "VehicleLandingGear.hpp"
#include "VehiclePropulsion.hpp"

namespace Designer
{
   class AeroBody;
   class AeroDish;
   class AeroLandingGear;
   class AeroNacelle;
   class AeroSpeedBrake;
   class AeroSurface;
   class AeroWing;
   class GeometryBody;
   class GeometryDish;
   class GeometryLandingGear;
   class GeometryNacelle;
   class GeometrySpeedBrake;
   class GeometrySurface;
   class GeometryWing;
}

// Vehicle is used to generate AFSIM P6DOF data files.

namespace Designer
{
   class Vehicle
   {
      public:
         Vehicle();
         Vehicle(const Vehicle&) = delete;                 // Deleted copy constructor
         Vehicle(Vehicle&&) = default;
         Vehicle& operator = (const Vehicle&) = delete;    // Deleted copy assignment operator
         Vehicle& operator=(Vehicle&&) = delete;
         ~Vehicle();

         // Sets the vehicle type string, which is typically "Aircraft" or "Weapon"
         void SetVehicleType(std::string& aVehicleTypeString) { mVehicleTypeString = aVehicleTypeString; }

         // Returns the vehicle type string, which is typically "Aircraft" or "Weapon"
         std::string GetVehicleType() const { return mVehicleTypeString; }

         // Sets the vehicle name string
         void SetVehicleName(const std::string& aVehicleNameString) { mVehicleNameString = aVehicleNameString; }

         // Returns the vehicle name string
         std::string GetVehicleName() const { return mVehicleNameString; }

         enum VehicleControlConfig
         {
            cCONFIG_STABS_ONE_TAIL,           // Ailerons, stabs, single tail
            cCONFIG_STABS_TWO_TAILS,          // Ailerons, stabs, two tails
            cCONFIG_STABS_ONE_VENTRAL_TAIL,   // Ailerons, stabs, single ventral tail
            cCONFIG_STABS_TWO_VENTRAL_TAILS,  // Ailerons, stabs, two ventral tails
            cCONFIG_STABS_UPPER_LOWER_TAILS,  // Ailerons, stabs, single tail, single ventral tail
            cCONFIG_VTAIL,                    // Ailerons, v-tail
            cCONFIG_INVERTED_VTAIL,           // Ailerons, inverted v-tail
            cCONFIG_CANARDS_ONE_TAIL,         // Ailerons, canards, single tail
            cCONFIG_CANARDS_TWO_TAILS,        // Ailerons, canards, two tails
            cCONFIG_CANARDS_STABS_ONE_TAIL,   // Ailerons, canards, stabs, single tail
            cCONFIG_CANARDS_STABS_TWO_TAILS,  // Ailerons, canards, stabs, two tails
            cCONFIG_TAIL_FINS,                // Tail control fins
            cCONFIG_MID_FINS,                 // Mid control fins
            cCONFIG_CANARD_FINS,              // Canard control fins
            cCONFIG_FLYING_WING,              // Dragerons, Elevons
            cCONFIG_STABS_FLYING_WING,
            cNO_CONTROL                       // No controls
         };

         // Sets the vehicle control configuration
         void SetVehicleControlConfiguration(VehicleControlConfig aControlConfig) { mControlConfig = aControlConfig; }

         // Returns the vehicle type string, which is typically "Aircraft" or "Weapon"
         VehicleControlConfig GetVehicleControlConfiguration() const { return mControlConfig; }
         QString GetVehicleControlConfigurationString() const;


         // Returns true if the vehicle is an aircraft
         bool IsAircraft();

         // Returns true if the vehicle is an weapon
         bool IsWeapon();

         void SetBankToTurnControl() {mUseBankToTurnControl = true;}
         void SetSkidToTurnControl() {mUseBankToTurnControl = false;}

         // Returns true if the vehicle uses bank-to-turn control
         bool UsesBankToTurnControl() { return mUseBankToTurnControl; }
         UtAtmosphere GetAtmosphere() const { return mAtmosphere; }

         // This sets the altitude (ft) used in testing
         void SetVehicleAltitude(double aVehicleAltitude_ft);

         // Returns the altitude (ft) used in testing
         double GetVehicleAltitude_ft() { return mVehicleAltitude_ft; }

         // This sets the Mach used in testing
         void SetTestingMach(double aMach);

         // This returns the preferred Mach used in testing
         double GetTestingMach();

         // This sets the maximum Mach for data files
         void SetAeroDataMaxMach(double aMach);

         double GetAeroDataMaxMach() { return mMaxTableMach; }

         // This sets the maximum beta (deg) for data files
         void SetAeroDataMaxBeta(double aBeta_deg);

         // This sets the minimum/maximum alpha (deg) for data files
         void SetAeroDataAlphas(double aAlphaMin_deg, double aAlphaMax_deg);

         void SetAeroAlphaBetaSymmetry(bool aAlphaSymmetry, bool aBetaSymmetry);

         // This informs the VehicleAero about the testing data and aero table limits
         void SetTestingDataAndAeroTableLimits();

         // This will generate the various AeroObject data based on the GeometryObjects.
         // Note: It will clear any previous AeroObject data.
         void GenerateAeroDataFromGeometry();

         // This will calculate the aerodynamics for the vehicle, including
         // both base aero (non-controls) and controls aero
         void CalculateVehicleAerodynamics();

         // Note that a positive yaw (to the right) generates a negative beta angle
         void SetVehicleAlphaBeta(double aAlpha_deg, double aBeta_deg);

         // Provides the current yaw and pitch (in degrees)
         void GetVehicleAlphaBeta(double& aAlpha_deg, double& aBeta_deg);

         // This sets the vehicle yaw, pitch, and roll rates in degrees/sec
         void SetVehicleYawPitchRollRates(double aYawRate_dps, double aPitchRate_dps, double aRollRate_dps);

         // This gets the vehicle yaw, pitch, and roll rates in degrees/sec
         void GetVehicleYawPitchRollRates(double& aYawRate_dps, double& aPitchRate_dps, double& aRollRate_dps);

         // This sets the vehicle parameters pertaining to speed
         void SetVehicleSpeedParameters(double aSpeed_fps,
                                        double aMach,
                                        double aKTAS,
                                        double aKCAS,
                                        double aDynamicPressure_psf,
                                        double aAirDensity_slugs_ft3);

         // This should be called after calling SetVehicleAlphaBeta, SetVehicleYawPitchRollRates,
         // and SetVehicleSpeedParameters. It updates the main aero calculation vectors.
         void CalculateAeroAttitudeVectors();

         // This provides the current vehicle parameters pertaining to speed
         void GetVehicleSpeedParameters(double& aSpeed_fps,
                                        double& aMach,
                                        double& aKTAS,
                                        double& aKCAS,
                                        double& aDynamicPressure_psf,
                                        double& aAirDensity_slugs_ft3);

         VehicleGeometry& GetGeometry() { return mGeometry; }
         VehicleAero* GetAero() { return mAero; }
         VehiclePropulsion* GetPropulsion() { return &mPropulsion; }
         VehicleControls& GetControls() { return mControls; }

         // This is called to update the GUI
         void Update();

         // This calculates to mass and rotational inertia for the full vehicle
         void CalcMassAndInertia();

         // This will activate/deactivate debug mode
         void SetDebugMode(bool aDebugActive);

         // Returns true if debug mode is active
         bool DebugMode();

         // This performs an "audit" to check that all geometry objects are fully
         // defined, identifying any issues that exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit();

      protected:

         // Generates data for AeroBody based on GeometryBody, plus optional
         // data for a fuselage canopy. If successful, it will add the new
         // AeroObject(s) to the VehicleAero aero object list. More than one
         // object will be added if symmetry exists.
         void GenerateAeroBody(GeometryBody* aGeometryObject,
                               bool          aIncludeCanopy = false,
                               double        aCanopyArea_ft2 = 0.0);

         // Generates data for AeroDish based on GeometryDish.
         // If successful, it will add the new AeroObject(s) to the VehicleAero
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroDish(GeometryDish* aGeometryObject);

         // Generates data for AeroLandingGear based on GeometryLandingGear.
         // If successful, it will add the new AeroObject(s) to the VehicleAero
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroLandingGear(GeometryLandingGear* aGeometryObject);

         // Generates data for AeroNacelle based on GeometryNacelle.
         // If successful, it will add the new AeroObject(s) to the VehicleAero
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroNacelle(GeometryNacelle* aGeometryObject);

         // Generates data for AeroSpeedBrake based on GeometrySpeedBrake.
         // If successful, it will add the new AeroObject(s) to the VehicleAero
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroSpeedBrake(GeometrySpeedBrake* aGeometryObject);

         // Generates data for AeroSurface based on GeometrySurface.
         // If successful, it will add the new AeroObject(s) to the VehicleAero
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroSurface(GeometrySurface* aGeometryObject);

         // Generates data for AeroWing based on GeometryWing.
         // If successful, it will add the new AeroObject(s) to the VehicleAero
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroWing(GeometryWing* aGeometryObject);

         // Generates data for AeroEngine based on GeometryEngine.
         // If successful, it will add the new AeroObject(s) to the VehiclePropulsion
         // aero object list. More than one object will be added if symmetry exists.
         void GenerateAeroEngine(GeometryEngine* aGeometryObject);

         // Creates an AeroEngine with the specified characteristics and adds it to the propulsion object
         void CreateAeroEngine(const std::string&         aEngineInstanceName,
                               const std::string&         aModelName,
                               GeometryEngine::EngineType aEngineType,
                               UtVec3dX                   aRefPoint_ft,
                               double                     aYaw_deg,
                               double                     aPitch_deg,
                               double                     aRoll_deg,
                               double                     aLength_ft,
                               double                     aDiameter_ft,
                               double                     aMass_lbs,
                               double                     aThrustOffset_ft,
                               double                     aIgnitionDelay_sec,
                               bool                       aLatchFuelInjection,
                               bool                       aUseProportionalThrottle,
                               double                     aMinProportionalThrottle);

      private:
         VehicleGeometry      mGeometry;
         P6DofMassProperties  mMassProperties;
         VehicleControls      mControls;
         VehiclePropulsion    mPropulsion;
         VehicleAero*         mAero                        = nullptr;
         VehicleLandingGear*  mLandingGear                 = nullptr;
         UtAtmosphere         mAtmosphere;
         VehicleControlConfig mControlConfig               = cNO_CONTROL;
         bool                 mUseBankToTurnControl        = false;
         double               mVehicleAltitude_ft          = 10000.0;   // Default alt of 10kft
         double               mVehicleAlpha_deg            = 0.0;
         double               mVehicleBeta_deg             = 0.0;
         double               mVehicleYawRate_dps          = 0.0;
         double               mVehiclePitchRate_dps        = 0.0;
         double               mVehicleRollRate_dps         = 0.0;
         double               mVehicleSpeed_fps            = 0.0;
         double               mVehicleMach                 = 0.0;
         double               mVehicleKTAS                 = 0.0;
         double               mVehicleKCAS                 = 0.0;
         double               mVehicleDynamicPressure_psf  = 0.0;
         double               mAirDensity_slugs_ft3        = 0.0;
         bool                 mDebugModeActive             = false;
         double               mIgnitionDelay_sec           = 0.0;
         std::string          mVehicleTypeString;
         std::string          mVehicleNameString;
         std::string          mAeroBasePath;

         // Set the max/min data ranges
         double               mVehicleTestMach             = 0.85;
         double               mMaxTableMach                = 3.0;
         double               mMaxTableBetaDataPts_deg     = 89.0;
         double               mMinTableAlphaDataPts_deg    = -179.0;
         double               mMaxTableAlphaDataPts_deg    = 179.0;

         // Set the alpha/beta symmetry booleans
         bool                 mAlphaSymmetry               = false;
         bool                 mBetaSymmetry                = false;
   };
}
#endif // !VEHICLE_HPP
