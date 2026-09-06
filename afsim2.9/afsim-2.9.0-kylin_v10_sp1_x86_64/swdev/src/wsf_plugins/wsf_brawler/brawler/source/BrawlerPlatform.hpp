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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef BRAWLERPLATFORM_HPP
#define BRAWLERPLATFORM_HPP

#include "brawler_export.h"

#include "BrawlerMIND.hpp"
#include "BrawlerUtil.hpp"
#include "UtEntity.hpp"
#include "UtFortMat3.hpp"
#include "UtMatrix.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"

/******************************************************************************
 *  OOP representation of a Brawler aircraft.
 *
 *  all units in:
 *   feet/ nmi
 *   deg
 *   NED
 *   ECS
 *
 *  Has a UtEntity to maintain "Brawler state"
 *
 ******************************************************************************/
class BRAWLER_EXPORT BrawlerPlatform
{
public:
   // force inheritance? have pure abstract methods?
   BrawlerPlatform();
   BrawlerPlatform(const BrawlerPlatform& aSrc);
   BrawlerPlatform& operator=(const BrawlerPlatform&) = delete;
   virtual ~BrawlerPlatform()                         = default;
   void Init(UtEntity& state, ut::Random& random);
   // virtual void Update(float aSimTime);
   virtual double GetTime() = 0;

   virtual double MindUpdate(double aSimTime);

   // replicates valsti() and lines 552-587 of aeva3i()
   // called before any alternative evaluations for current update
   virtual void InitializeStatistics();
   // replicates valst()
   // called after each alternative evaluation for the current update
   virtual void UpdateStatistics(double value);
   // replicates valsth()
   // called after all alternative evaluations for current update
   virtual void CalculateStatistics();
   // replicates adj_tnl_mlt()
   void AdjustTunnelVisionMultipliers();

   UtVec3d PositionNED();
   // UtVec3d PositionECS();  //would always be (0,0,0)
   UtVec3d VelocityNED();
   UtVec3d VelocityECS();
   UtVec3d AccelerationNED();
   UtVec3d AccelerationECS();
   UtVec3d OrientationNED();

   double GetSpeed(); // ft/s
   double GetAlpha();

   UtVec3d RelativePositionNED(UtEntity& aTgt);
   UtVec3d RelativeVelocityNED(UtEntity& aTgt);
   UtVec3d RelativePositionECS(UtEntity& aTgt);
   UtVec3d RelativeVelocityECS(UtEntity& aTgt);

   // TODO - move any NED conversion functionality to BrawlerCoordinateConversion
   UtVec3d ConvertNEDtoECS(UtVec3d ned);   // this function needs its own mutable copy of 'ned', just pass by value
   UtVec3d ConvertNEDtoWind(UtVec3d ned);  // this function needs its own mutable copy of 'ned', just pass by value
   UtVec3d ConvertECStoNED(UtVec3d ecs);   // this function needs its own mutable copy of 'ecs', just pass by value
   UtVec3d ConvertECStoWind(UtVec3d ecs);  // this function needs its own mutable copy of 'ecs', just pass by value
   UtVec3d ConvertWindtoNED(UtVec3d wind); // this function needs its own mutable copy of 'wind', just pass by value
   UtVec3d ConvertWindtoECS(UtVec3d wind); // this function needs its own mutable copy of 'wind', just pass by value

   UtFortMat3d GetNEDToECSRotation();

   static UtVec3d PositionNED(UtEntity& aTgt);
   // static UtVec3d PositionECS(UtEntity&); //would always be (0,0,0)
   static UtVec3d VelocityNED(UtEntity& aTgt);
   static UtVec3d VelocityECS(UtEntity& aTgt);
   static UtVec3d AccelerationNED(UtEntity& aTgt);
   static UtVec3d AccelerationECS(UtEntity& aTgt);

   static UtVec3d RelativePositionNED(UtEntity& ref, UtEntity& aTgt);
   static UtVec3d RelativeVelocityNED(UtEntity& ref, UtEntity& aTgt);
   static UtVec3d RelativePositionECS(UtEntity& ref, UtEntity& aTgt);
   static UtVec3d RelativeVelocityECS(UtEntity& ref, UtEntity& aTgt);

   // TODO - move any NED conversion functionality to BrawlerCoordinateConversion
   static UtVec3d ConvertNEDtoLLA(UtVec3d ned);
   static UtVec3d ConvertNEDtoECS(UtEntity& ref, UtVec3d ned);
   static UtVec3d ConvertNEDtoWind(UtEntity& ref, UtVec3d ned);
   static UtVec3d ConvertECStoNED(UtEntity& ref, UtVec3d ecs);
   static UtVec3d ConvertECStoWind(UtEntity& ref, UtVec3d ecs);
   static UtVec3d ConvertWindtoNED(UtEntity& ref, UtVec3d wind);
   static UtVec3d ConvertWindtoECS(UtEntity& ref, UtVec3d wind);

   static double GetSpeed(UtEntity& aRef);
   static double GetAlpha(UtEntity& aRef);

   // double SlantRange(WsfTrack* aTgt, double defaultRange = 1000000 / ftnmi);
   double SlantRange(UtEntity& aTgt, double defaultRange = 1000000 / BRAWLER::ftnmi);

   virtual UtEntity* GetTarget() = 0;
   virtual double    FirstObservedTime(UtEntity* aTgt)
   {
      return 0;
   } // replaces "fstobs", returns -1 if not currently observed

   // return largest threat [missile] track
   virtual UtEntity* GetThreat() = 0;
   virtual int       GetNumberThreats() { return 0; }

   UtEntity& State() { return mMyState; }
   void      SetState(UtEntity& state) { mMyState = state; }

   UtEntity& NominalState() { return mMyNom; }
   void      SetNominalState(UtEntity& state) { mMyNom = state; }

   UtEntity& ProjectedState() { return mMyPro; }
   void      SetProjectedState(UtEntity& state) { mMyPro = state; }

   UtEntity& TargetNominalState() { return mTgNom; }
   void      SetTargetNominalState(UtEntity& state) { mTgNom = state; }

   virtual double GetConsciousnessEventTime() { return 1.0; }
   virtual double LastConsciousnessEventTime() { return 0.0; }

   // just use class a storehouse for these values?  (set from script behaviors?)
   // what are enumerations of each value???
   virtual void SetAlternative(int ilevel, int kalt, int icall,
                               int lcall); // replaces use of "altupk(altdsc,...)"

   virtual void GetAlternative(int& ilevel, int& kalt, int& icall,
                               int& lcall); // replaces use of "indupk(altdsc,...)"

   //! Projects current position forward based on the alternative
   //! and evaluates the result of the projection

   // brawler aero command type 1
   virtual double EvaluateRates(UtVec3d& aAngularRates,           // radian/sec
                                UtVec3d& aLongitudinalAccel) = 0; // meter/sec^2
   // brawler aero command type 2
   virtual double EvaluateHeadingSpeedAltitude(const double& aHeading,       // degrees
                                               const double& aSpeed,         // meters/sec
                                               const double& aAltitude,      // meters
                                               const double& aMaxGees,       // Gs
                                               const double& aMaxClimb) = 0; // meters/sec
   // brawler aero command type 3
   virtual double EvaluateTurnInPlaneWithSpeed(UtVec3d&      aPlane,            // World coordinates, meters
                                               const double& aGees,             // Meters/sec^2
                                               const double& aSpeed)       = 0; // meters/sec
   virtual double EvaluateTurnInPlaneWithThrottle(UtVec3d&      aPlane,         // World coordinates, meters
                                                  const double& aGees,          // Meters/sec^2
                                                  const double& aThrottle) = 0; // meters/sec
   // brawler aero command type 4
   virtual double EvaluateVectorWithSpeed(UtVec3d&      aVelocityVector,    // Directional unit vector
                                          const double& aMaxGees,           // Gs
                                          const double& aSpeed)       = 0;  // meters/sec
   virtual double EvaluateVectorWithThrottle(UtVec3d&      aVelocityVector, // Directional unit vector
                                             const double& aMaxGees,        // Gs
                                             const double& aThrottle) = 0;  //[0,3]

   //! Returns the value of the requested maneuver component
   double ManeuverValueComponent(BRAWLER::ManeuverValueComponentType aType) { return valcmp[(int)aType - 1]; }
   void   SetManeuverValueComponent(BRAWLER::ManeuverValueComponentType aType, double value)
   {
      valcmp[(int)aType - 1] = value;
   }

   double RawManeuverValueComponent(BRAWLER::ManeuverValueComponentType aType) { return rawvalcmp[(int)aType - 1]; }
   void   SetRawManeuverValueComponent(BRAWLER::ManeuverValueComponentType aType, double value)
   {
      rawvalcmp[(int)aType - 1] = value;
   }

   double MinAlternativeValue() { return altval_min; }
   void   SetMinAlternativeValue(double val) { altval_min = val; }

   //! Returns the value of the requested tunnel vision multiplier
   //! Tunnel Vision shares component types with maneuver
   // replaces "tnl_mult"
   double TunnelVisionMultiplier(BRAWLER::ManeuverValueComponentType aType) { return tnl_mult[(int)aType - 1]; }

   //! InherentBiasFaults replaces "bias_faults" from BRAWLER v7.5
   //! See BRAWLER User manual p. 3.2.1-74, read from SCNRIO file for AIRCRAFT FLIGHT DEFINITION
   //! Defines the bias for aggressiveness, mutual support, and airspeed maintenance.
   //! Default value is 1 (no effect). Values less than 1 will reduce the bias
   //! and values greater than 1 will increase the bias
   //! For the initial implementation this function will always return 1 (typical config value)
   //! TODO - read the three bias values from input file for each platform
   //! @param aType The bias type (aggressiveness, mutual support, maintain airspeed)
   //! @return the nominal range of the weapon in feet
   double InherentBiasFaults(BRAWLER::InherentBiasFaultTypes aType) { return 1.0; }


   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
   // section: small brawler computation methods that get replaced //////////
   //  FORMERLY PRIVATE - need access for scripts
   bool   SlowFlight();          // replaces "slow" from aeva3i and getslo()
   double OffensiveMultiplier(); // replaces use of "offmus" and "offmlt", Set to "ATTACK" mode for now
   double DefensiveMultiplier(); // replaces use of "defmus" and "defmlt"
   bool   BoreSight();           // replaces borsit() from borsit.f
   // double OffBoreSightAngle(WsfPlatform* aRef, WsfPlatform* aTgt); //replaces use of "obang" set in obangs() called
   // from minud/majud
   double OffBoreSightAngle(UtEntity& aRef,
                            UtEntity& aTgt); // replaces use of "obang" set in obangs() called from minud/majud
   double ProjectedTimeDelta();              // replaces use of "tproj3"  - see modsel.f

   double RollOverTopValue() { return valrot; }
   void   SetRollOverTopValue(double val) { valrot = val; }

   double RollOverTopTime() { return timrot; }
   void   SetRollOverTopTime(double val) { timrot = val; }

   BRAWLER::MissileModeState MissileMode() { return mMissileMode; }
   void                      SetMissileMode(BRAWLER::MissileModeState mode) { mMissileMode = mode; }

   UtVec3d EvasionDirection() { return mEvasionDirection; }
   void    SetEvasionDirection(UtVec3d dir) { mEvasionDirection = dir; }

   double LastComputedEngagementValue(UtEntity& aTgt); // replaces use of "valeff"

   double DegreeOfThreat(UtEntity& aTgt); // replaces use of "semmsl" or "mislse[]"
   double DegreeOfThreatCompliment(UtEntity& aTgt) { return (1.0 - DegreeOfThreat(aTgt)); } // replaces use of "csmmsl"

   // vec = directional unit vector, sflypp = ft/s
   void SuggestVectoredFlight(UtVec3d vec, double val, double spd)
   {
      vecfpp = vec;
      valfpp = val;
      sflypp = spd;
   }
   UtVec3d DirectionVectoredFlight() { return vecfpp; } // directional unit vector
   double  ValueVectoredFlight() { return valfpp; }
   double  SpeedVectoredFlight() { return sflypp; } // ft/s

   // bool DisEngage(); //replace "diseng" and evdisi()

   bool Damaged(UtEntity& aRef); // replace use of "int damage[mac]"

   double         TargetAngleAboveHorizon(UtEntity& aTgt);     // replaces "ppmohr" - used in mslenv.f (ovrhor)
   virtual double SensorGimbalElevationLimit() = 0;            // replaces "relmx"
   virtual double SensorGimbalAzimuthLimit()   = 0;            // replaces "razmx"
   double         WeaponSimTimeOfIntercept(UtEntity& aThreat); // replaces "mislta" computed in as1tgt.f

   double WeaponMinEnvelopeRange() { return 3000.0; }  // replaces use of "ppmrmn" - computed mslenv.f (rmin) (MSLI.MSL)
   double WeaponMaxEnvelopeRange() { return 45000.0; } // replaces use of "ppmrmx" - computed mslenv.f (rmax) (MSLI.MSL)
   BRAWLER::WeaponType WeaponType()
   {
      return BRAWLER::kndsrm;
   } // replaces use of "ppmknd" //weapon type selected (assume missiles for now -> set to 1 (short range [aim-9] missile))
   bool WeaponSeekerOnWhenFired() { return true; } // replace use of "on_at_lnch(i,plnch(mode)" //IR SEEKER ON AT LAUNCH
   // bool WeaponSeekerType(BRAWLER::WeaponSeekerType aType) { return (aType == irskr); }  //replaces use of "pp_skrs"
   // //LBM - force to IR seeker for now
   bool WeaponVerticalTrackCheck(UtEntity& aTgt); // replaces "ppmtrk" (envlvl_aim) (target above horizon or missile can
                                                  // track looking down)
   // bool CommandGuidedWeapon() { return false; }   //replaces use of "pp_comg"
   double WeaponSteeringError(UtEntity& aTgt); // replaces use of "ppmse" - computed in mslenv.f (se) --> envgeo.f (se)
   double WeaponSeekerMaxAngle()
   {
      return 0.7;
   } // replaces "ztamx" (units radians) //LBM - hard coded to: MSLI.MSL "ZTAMX" value

   double RangeRate(UtEntity& aRef, UtEntity& aTgt);              // replaces use of "rdotpr"
   double ExpectedValueDestroyed(UtEntity& aRef, UtEntity& aTgt); // replaces "vkexp" calculated in majud.f
   double ProbabilitySeen(UtEntity& looker, UtEntity& seen);      // replaces "pseen" from pbseen.f
   double ProbabilityOfSurvival(UtEntity& aRef, UtEntity* aTgt);  // replaces "psrv" or "psrvfl" calculated in majud.f
   double ProbabilityOfKill(UtEntity& killer, UtEntity& victim);  // replaces "pkil"
   double ForceRatio() { return fratio; }

   //! Set the current missile mode based on aircraft conditions.
   //! Similar function to mislmd.f in BRAWLER v7.5
   virtual void UpdateMissileMode() = 0;

   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
   // section: brawler values from user input ///////////////////////////////

   BrawlerMIND* GetMIND() { return &mMIND; }

   //! See BRAWLER User manual p. 3.2.1-39, read from SCNRIO file
   //! For the initial implementation this function will always return the typical
   //! short range value of 0.95 (blue)
   //! @note Typical short range value for red 0.75 !?
   double RangePercent(BRAWLER::WeaponType aType) { return 0.95; }

   //! AircraftValue replaces "valac" and "valme" from BRAWLER v7.5
   //! See BRAWLER User manual p. 3.2.1-66, read from SCNRIO file
   //! Intrinsic aircraft value. Used by both sides in determining the scale of many
   //! value component multipliers. Standard values are 10.0 for a fighter,
   //! 40.0 for a bomber, and 80.0 for an AWACS. The suggested range is [5, 100]
   //! For the initial implementation this function will always return 10 (fighter default)
   //! TODO - read a map of platform type and value from input and return that value based on platform type
   //! @param aRef Pointer to the platform
   //! @return intrinsic value of the platform (unitless)
   double AircraftValue(UtEntity& aRef) { return 10.0; } // replace valint[] for now as well

   //! CombatEffectiveness returns "cmbeff" from BRAWLER v7.5
   //! See BRAWLER User manual p. 3.2.1-66, read from SCNRIO file
   //! Used by both sides to compute the effective force ratio. As this is only
   //! used to form ratios between different aircraft in the simulation, the
   //! value assigned to an aircraft will reflect its effectiveness relative
   //! to the other aircraft in the scenario. A standard value is 1.0,
   //! Suggested range is [0.5, 2.0] but brawler does not actually check this
   //! TODO - read a map of platform type and value from input and return that value based on platform type
   //! @param aRef Pointer to the platform
   //! @return combat effectiveness value of the platform (unitless)
   double CombatEffectiveness(UtEntity& aRef) { return cmbeff; }

   //! FlightAggressiveness returns "aggfac" from BRAWLER v7.5
   //! See BRAWLER User manual p. 3.2.1-86, read from SCNRIO file for AIRCRAFT FLIGHT DEFINITION
   //! Default value is 1.0, Range is [0.49, 2.5]
   //! TODO - read value from input
   //! @return aggressiveness factor for this aircraft's flight (unitless)
   double FlightAggressiveness() { return aggfac; }

   //! MaxWeaponSteeringError replaces "ppm_semax = semax" from BRAWLER v7.5
   //! See BRAWLER User manual Table 3.3.3.2-17, read from .MSL SEEK block
   //! Steering error is defined as the angle between the launching aircraft's
   //! longitudinal axis (X-bodyaxis) and the line-of-sight to the predicted intercept.
   //! For initial implementation this function will always return 4.0000E-01 (typical config value)
   //! @return maximum steering error to fire in radians
   double MaxWeaponSteeringError() { return 0.4; } // read from input file

   //! MaxWeaponAspectOffsetAngle replaces "aoffmx" from BRAWLER v7.5
   //! See BRAWLER User manual Table 3.3.3.2-17, read from .MSL SEEK block
   //! Maximum aspect angle off tail to fire. A value of 0.0 indicates a
   //! tail aspect, a value of pi indicates a nose aspect.
   //! For initial implementation this function will always return 4.0 (typical config value)
   //! @return maximum  aspect angle off tail to fire in radians
   double MaxWeaponAspectOffsetAngle() { return 3.0; } // read from input file  (MSLI.MSL)

   //! Replaces lbit(rmsk,bitdwn)
   //! See BRAWLER User manual p. 3.3.2.4-12, read from .RDR SCOPE block
   //! For initial implementation this function will always return true (typical config value)
   bool LookDownShootCapable() { return true; } // read from input file

   /////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////

   virtual bool HaveWeapons();
   bool         Skip1v1();
   bool         NoAim();
   bool         InIntercept();
   bool         Ready();
   bool         OneVsOneOp();

   double TimeAllowedPerSectorSearch() { return mTimeAllowedPerSectorSearch; }
   void   SetTimeAllowedPerSectorSearch(double val) { mTimeAllowedPerSectorSearch = val; }

   static UtVec3d a_lim(const UtVec3d& x_in, const UtVec3d& v_in, const UtVec3d& a_in, double dt, double min_speed);

   static void projw(const UtVec3d& xin,
                     const UtVec3d& vin,
                     const UtVec3d& a,
                     double         dt,
                     UtVec3d&       xout,
                     UtVec3d&       vout,
                     UtVec3d&       aout,
                     UtFortMat3d&   rwout);

   UtVec3d GetTargetRoutePoint() { return rtepp; };

   void SetTargetRoutepointLLA(int aLat, int aLon, int aAlt_m);
   void SetTargetRoutepointLLA(UtVec3d aLLA);


   double Uniform() { return rnsq.Uniform<double>(); }

   ////////////////////////////////////////////////////////////////////////////
   //    VIRTUAL AERO QUERIES
   //
   // TODO - check if all (or which ones) are in body/wind axes
   // TODO - check where intelligent lookups of "best" values are taking place
   //        (e.g. best alpha for sustained Gs)

   // Forward Acceleration Functions
   // This provides the maximum acceleration in the body-x direction. It includes use of afterburner (if available)
   // and assumes that side-slip (beta) is negligible. It does not include any gravity effects.
   // See perfrm.f 163, "almaxg"
   virtual double MaxForwardAcceleration_fps2() = 0;

   // This provides the minimum acceleration in the body-x direction. It assumes a fully retarded throttle control
   // and full use of speed brakes (if available). It assumes that sideslip (beta) is negligible and does not include
   // any gravity effects.
   // See perfrm.f 166, "alming"
   virtual double MinForwardAcceleration_fps2() = 0;

   // This is the same as double MaxForwardAcceleration_fps2() but it includes the effect of gravity. Thus, for an
   // aircraft in level flight, MaxForwardAccelerationWithGravity_fps2() will produce the same value as
   // MaxForwardAcceleration_fps2(). For an climbing aircraft MaxForwardAccelerationWithGravity_fps2() will be less
   // and for a diving aircraft it will be more than MaxForwardAcceleration_fps2().
   // See perfrm.f 165, "almax"
   // Ref: GravityWindAxis[0] + ((MaxGrossThrustBodyAxis - MagnitudeOfDrag - PropulsionDragAtMaxGrossThrustBodyAxis) / AirCraftMass)
   virtual double MaxForwardAccelerationWithGravity_fps2() = 0;

   // This is the same as MinForwardAcceleration_fps2() but it includes the effect of gravity. Thus, for an aircraft
   // in level flight, MinForwardAccelerationWithGravity_fps2() will produce the same value as
   // MinForwardAcceleration_fps2(). For an climbing aircraft MinForwardAccelerationWithGravity_fps2() will be less
   // and for a diving aircraft it will be more than MinForwardAcceleration_fps2().
   // See perfrm.f 167, "almin"
   virtual double MinForwardAccelerationWithGravity_fps2() = 0;

   // Maximum Performance Turn Functions

   // TODO - verify proper use of this method and the MaxLimitedInstantaneousGLoad_g() method
   // This provides the maximum instantaneous g-load when performing a maximum performance turn. When performing this
   // turn, the platform may not be able to maintain its speed.
   // replaces gliftm() (only aero considered)
   // Lift = DynamicPressure * SurfaceArea * CoefficientLiftMax / Weight
   // TODO - verify which axes (wind axes right?)
   virtual double MaxInstantaneousGLoad_g() = 0;

   // This provides the minimum instantaneous turn radius when performing a maximum performance turn. When performing
   // this turn, the platform may not be able to maintain its speed.
   // Replaces "radmin"
   virtual double MinInstantaneousTurnRadius_ft() = 0;

   // This provides the maximum instantaneous turn rate when performing a maximum performance turn. When performing
   // this turn, the platform may not be able to maintain its speed.
   // Replaces use of "wmax" perfrm.f:  max_ang_rate = gmxsu*grav/spdnow(me)
   virtual double MaxInstantaneousTurnRate_rps() = 0;

   // This provides the maximum sustained g-load when performing a maximum performance turn. When performing this
   // turn, the platform will be able to maintain its speed if it uses full power (including afterburner if available).
   // This function uses iterative methods to arrive at a solution.
   // This replaces "gmxsut" and is based on susgmx = (liftLbs+thrust*sin(alfa))/weightLbs
   virtual double MaxSustainedGLoad_g() = 0;

   // This provides the minimum sustained turn radius when performing a maximum performance turn. When performing this
   // turn, the platform will be able to maintain its speed if it uses full power (including afterburner if available).
   // This function uses MaxSustainedGLoad_g() to arrive at a solution.
   virtual double MinSustainedTurnRadius_ft() = 0;

   // This provides the maximum instantaneous turn rate when performing a maximum performance turn. When performing
   // this turn, the platform will be able to maintain its speed if it uses full power (including afterburner if
   // available). This function uses MaxSustainedGLoad_g() to arrive at a solution.
   virtual double MaxSustainedTurnRate_rps() = 0;

   // Comfortable Turn Functions

   // This provides the g-load when performing a "tactical turn".
   // Under certain [safe] conditions this takes the value of MaxSustainedGLoad_g()
   // Under more threatening conditions this takes the value of  MaxInstantaneousGLoad_g()
   // Replaces "gmxsu" - conditionally switches between either:
   // MaxSustainedGLoad_g() or MaxInstantaneousGLoad_g()
   virtual double MaxTacticalGLoad_g() = 0;

   // This provides the minimum turn radius when performing a "tactical turn" (using gmxsu).
   // calculation based off of the current MaxTacticalGLoad_g()
   // Replaces "radmnp", set in perfrm:  = fmachp*fmachp*vsme*vsme/(gmxsu*grav)
   virtual double MinTacticalTurnRadius_ft() = 0;

   // This provides the maximum turn rate when performing a "tactical turn".
   // calculation based off of the current MaxTacticalGLoad_g()
   virtual double MaxTacticalTurnRate_rps() = 0;

   // Misc Performance Calculations

   // This provides the best "corning speed", meaning the speed at which the highest turn rate can be achieved at
   // the current altitude.
   // This function uses iterative methods to arrive at a solution. The iterations stop after 10 passes or when the
   // cornering speed bounds are within 0.01 mach of each other (approximately 10 ft/sec).
   // Replaces use of "cornrv" calculated in perfrm.f by corner(), which searches for
   // velocity between mach [0.1, 1.0] at current altitude that gives max possible turn rate.
   // MaxTurnRate = sqrt( gl * gl - 1.0) / CornerVelocity where:  gl = MaximumAvailableLift_g()
   virtual double CorneringVelocity_fps() = 0;

   // This provides *************************************
   // replaces use of "erate" aproj3.f erate = (accel-gravVec[0])*speed)/grav
   virtual double SpecificEnergyRate_fps() = 0;

   // This provides *************************************
   virtual double CalcTurnRate_rps(const double& aClMax,
                                   const double& aSpeed_fps,
                                   const double& aDynamicPressure_psf,
                                   const double& aWingArea_ft2) = 0;

   // Weights

   // This returns the current gross weight of the platform, including fuel and weapons.
   virtual double CurrentWeight_lbs() = 0;

   // This returns the empty weight of the platform. It is a constant for a given type of platform.
   virtual double EmptyWeight_lbs() = 0;

   // This returns the current fuel weight of the platform
   virtual double CurrentFuelWeight_lbs() = 0;

   // This returns the design weight for performing a max-g (maximum performance) turn. If the aircraft is at a
   // higher weight, it may not pull as many g's without over-stressing the airframe.
   // This replaces use of "miss_mass" in Brawler.
   virtual double DesignWeightForMaxG_lbs() = 0;

   // Structural Limits

   // This returns the structural g-limit for the airframe at its current weight. It is the maximum g's the aircraft
   // can pull, at the current weight, without over-stressing the airframe.
   // replaces table lookup of "gcap".
   virtual double CurrentStructuralGLimit_g() = 0;

   // This returns the structural g-limit for the airframe at the weight specified in DesignWeightForMaxG_lbs(). It
   // is the maximum g's the aircraft can pull, at the design weight, without over-stressing the airframe.
   // replaces table lookup of "gcap2".
   virtual double StructuralGLimitAtDesignWeight_g() = 0;

   // This returns the structure g-limit for the airframe when its weight is above the design limit. This is a
   // reduced, "general value", and does not take into account the actual current platform weight.
   virtual double StructuralGLimitAboveDesignWeight_g() = 0;

   // Data Functions

   // This provides the current dynamic pressure for the platform.
   virtual double DynamicPressure_psf() = 0;

   // This provides the current mach for the platform.
   virtual double Mach() = 0;

   // This provides the maximum lift coefficient for the platform at the current conditions of mach and altitude.
   virtual double GetMaxCL() = 0;

   // This provides the maximum lift coefficient for the platform at the specified mach.
   virtual double GetMaxCL(const double& aMach) = 0;

   // This provides the maximum lift coefficient for the platform at the specified altitude and mach.
   virtual double GetMaxCL(const double& aAlt_ft, const double& aMach) = 0;

   // This provides the angle of attack (alpha) at maximum lift coefficient for the platform at the current conditions
   // of mach and altitude.
   virtual double GetAlphaAtMaxCL_deg() = 0;

   // This provides the angle of attack (alpha) at maximum lift coefficient for the platform at the specified mach.
   virtual double GetAlphaAtMaxCL_deg(const double& aMach) = 0;

   // This provides the angle of attack (alpha) at maximum lift coefficient for the platform at the specified mach.
   virtual double GetAlphaAtMaxCL_deg(const double& aAlt_ft, const double& aMach) = 0;

   // ###########################################################################################

   // TODO - verify proper use of this method and the MaxInstantaneousGLoad_g() method
   //  This provides the maximum instantaneous g-load when performing a maximum performance turn. When performing this
   //  turn, the platform may not be able to maintain its speed. NOTE: Limited by structural G limits.
   //  replaces "gmxin" and "gldmx()" (aero + structural limits considered)
   virtual double MaxLimitedInstantaneousGLoad_g();

   // returns ft/sec^2 version of:  MaxLimitedInstantaneousGLoad_g().
   // replace use of "amxin", (perfrm.f 105)
   double MaximumInstantaneousLift_fps2() { return BRAWLER::grav * MaxLimitedInstantaneousGLoad_g(); }

   // Comments near where this function is used pertain to "LOS RATE". So, is this a turn rate?
   // If so, the new function would be MaxInstantaneousTurnRate_rps().
   // However, if it does pertain to line-of-sight rate, we currently lack any suitable replacement.
   // replaces use of "wmax" perfrm.f:  max_ang_rate = gmxsu*grav/spdnow(me)
   virtual double MaxAngularRate_rps() = 0;

   // Set this platform's job
   void SetJob(BRAWLER::JobType aJob) { mMyJob = aJob; }

   // Set this platform's mission
   void SetMission(BRAWLER::MissionType aMission) { mMission = aMission; }

   // Return this platform's job, as specified by the job type enum
   BRAWLER::JobType Job() { return mMyJob; }

   // Return this platform's mission, as specified by the mission type enum
   BRAWLER::MissionType Mission() { return mMission; }

   // Get this platforms route point value multiplier
   double RoutePointValueMultiplier() { return vmsnpp; }

   void SetFlightAggressiveness(double aFactor);
   void SetCombatEffectiveness(double aEffect);
   void SetForceRatio(double aRatio);

   // TODO - PRBDAS accessor functions

protected:
   // TODO - override all position/orientation calls in UtEntity that return a value with units
   //        example:   GetSpeed()  should return brawler ft/sec (not wsf met/sec)

   // nominal state (projected state forward) used for nominal queries
   UtEntity mMyState; // my current (truth) state
   UtEntity mMyNom;   // my nominal state (simple projection)
   UtEntity mTgNom;   // target nominal state (simple projection)
   UtEntity mMyPro;   // maneuver projected state

   void NominalProjection(double dt); // similar to svpred.f

private:
   BrawlerMIND mMIND; //!< Contains user input from MIND file

   //! Time in seconds allowed to search a sector of the sky
   //!  Equivalent to 'dtimax' defined in sencon.fi of BRAWLER v7.5, default valule of 10.0
   double mTimeAllowedPerSectorSearch;

   //! Tracks the missile mode state: Equivalent of mslmd defined in ppost.fi in BRAWLER v7.5
   BRAWLER::MissileModeState mMissileMode;

   //! Alternative identifiers
   int milevel;
   int mkalt;
   int micall;
   int mlcall;

   //! @name Alternative Initialization Values
   //! Values Set by InitializeAlternativeEvaluation() and then used elsewhere
   //! when processing Alternative Evaluation
   //!@{
   // bool skip_1v1;
   bool   tnl_reset;
   double tnl_p_reset;
   double pdraw;
   //!@}

   double aggfac;
   double cmbeff;
   double fratio;

   // LBM - miscellaneous values
   // bool l1opp;// = true;   //C     --L1OPP IS TRUE IF A GENUINE 1V1 SITUATION EXISTS
   // bool noaim;
   // bool wepons;
   // bool intrst;
   // bool ready;
   // bool lillum;
   // bool diseng;   //set by evdisi() in evdis.f called from aeva3i() in aeval3.f
   // int ppmknd; //weapon type selected (assume missiles for now -> set to 1 (short range missile))
   // set by selwpf() called from selwpn()

   double valrot; //= 0.0;
   double timrot; //= 0.0;

   double tseen; //=0

   int nalt;

   // valhst.fi
#define nvcp_used 20
#define nvalcp 20
   double vofutl;     //[mac]
   double vavutl;     //[mac]
   double valbar[20]; //[nvalcp]
   int    iacoff;
   int    mslmod;
   double altval_min;
   double s_w;
   double s_v[20];   //[nvalcp]
   double s_wv[20];  //[nvalcp]
   double s_vv[20];  //[nvalcp]
   double s_wvv[20]; //[nvalcp]

   double rawvalcmp[20]; // holds all raw maneuver value components (no scaling or modifications)
   // union {
   double valcmp[20]; // holds all maneuver value components (no union)
   // struct {
   // double voffef; //offensive evaluation
   // double vlospd; //low speed recovery evaluation
   // double vform;  //station keeping (formation)
   // double compr;  //route-point
   // double valdfx; //defensive evaluation //VALDFX IS THE PROBABILITY OF SURVIVING FOR EACH HOSTILE IN THE LOCAL CELL
   // CONVERTED TO THE NEGATIVE UTILITY OF NOT SURVIVING IN THE LOCAL CELL. double gaval;  //ground avoidance double
   // vvec;   //vector following double villum; //illumination evaluation double vmusup; //mutual support double valevd;
   // //evade evaluation double vmaim;  //missile aiming evaluation double vdisng; //disengage evaluation double sepval;
   // //separation double vescrt; //escort bomber double vsamz;  //SAM avoidance double vavbdd; //bad aspect avoidance
   // evaluation double hddval;    //hard-deck double terrval;   //terrain double vdew;      //directed energy weapon
   // double vtpt;      //third part targeting
   // };
   // };

   // mind2.fi
   bool   tnl_vis;
   double tnl_mult[nvalcp];

   // double tdseng;
   // bool desdis;   //= false;  //LBM - set to "ATTACK" mode for now
   // bool disbng;

   // mindpr.fi
   bool prbdas;


   // ppost.fi
   UtVec3d vecfpp;
   double  valfpp;
   double  sflypp;

   // UtTable::Curve*     mCLMaxVsMachTable;
   // UtTable::Curve*     mAlphaAtCLMaxVsMachTable;
   double mDesignWeightForMaxG_lbs;
   double mStructuralGLimit_g;
   double mStructuralGLimitAboveDesignWeight_g;
   double mSpeedbrakeCDA_sqft;

   UtVec3d mEvasionDirection;

   // typical brawler units:
   // position = feet
   // velocity = feet/sec
   // accel    = feet/sec^2
   // weight   = pounds
   // lift     = sometimes:  Gs,   other times:  feet/sec^2
   /////////////////////////////////////////////////////////////////////////

   ut::Random rnsq;

   BRAWLER::JobType     mMyJob;   // myjob
   BRAWLER::MissionType mMission; // misn

   double vmsnpp;

   UtVec3d rtepp;
};

#endif
