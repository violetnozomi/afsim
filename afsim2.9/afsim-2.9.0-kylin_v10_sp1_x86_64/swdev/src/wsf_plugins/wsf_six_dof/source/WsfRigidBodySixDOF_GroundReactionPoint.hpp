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

#ifndef WSFRIGIDBODYSIXDOFGROUNDREACTIONPOINT_HPP
#define WSFRIGIDBODYSIXDOFGROUNDREACTIONPOINT_HPP

#include "wsf_six_dof_export.h"

#include <list>

#include "UtInput.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Object.hpp"

namespace wsf
{
namespace six_dof
{
// The RigidBodyGroundReactionPoint simulates a 'contact point' for ground reaction.
// This contact point may be a landing gear or could be a structural point on
// the aircraft.

class WSF_SIX_DOF_EXPORT RigidBodyGroundReactionPoint : public Object
{
public:
   friend class RigidBodyLandingGear;

   RigidBodyGroundReactionPoint()        = default;
   ~RigidBodyGroundReactionPoint()       = default;
   RigidBodyGroundReactionPoint& operator=(const RigidBodyGroundReactionPoint& other) = delete;

   RigidBodyGroundReactionPoint* Clone() const;

   bool ProcessInput(UtInput& aInput);
   bool Initialize();

   // The CalcGroundReactionNormalForceMoment function calculates the force and moment
   // produced by the normal force of the RigidBodyGroundReactionPoint. It returns true
   // if the point is in contact with the ground.
   bool CalcNormalForceMoment(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs, bool& aMaxCompressionExceeded, double aDeltaT_sec);

   // The CalcGroundReactionMaxFrictionForceMagnitude_lbs function calculates the
   // maximum friction force possible, in the specified friction direction,
   // using the most recently calculated normal force, and using the static coefficient
   // of friction. It returns max force in lbs.
   double CalcMaxFrictionForceMagnitude_lbs(const UtVec3dX& aDesiredForceBasedFrictionDirectionInSurfaceVecNED);

   // The CalcGroundReactionFrictionForceMomentWhileInMotion function calculates the
   // force and moment produced by the friction force of the RigidBodyGroundReactionPoint
   // when the vehicle is is motion.
   void CalcFrictionForceMomentWhileInMotion(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs);

   // The CalcGroundReactionFrictionForceMomentStartingToMove function calculates the
   // force and moment produced by the friction force of the RigidBodyGroundReactionPoint
   // when the vehicle is just starting to move. Since the vehicle was at rest, the
   // friction direction is force-based rather than velocity-based.
   void CalcFrictionForceMomentStartingToMove(UtVec3dX&       aForce_lbs,
                                              UtVec3dX&       aMoment_ftlbs,
                                              const UtVec3dX& aFrictionDirectionInSurfaceNED);

   // The CalcGroundReactionForceMoment function calculates the force and moment produced
   // by the RigidBodyGroundReactionPoint. The friction multiplier (optional) can be used to
   // limit friction to less than its maximum value. It returns true if the point is in
   // contact with the ground.
   bool CalcGroundReactionForceMoment(UtVec3dX&       aForce_lbs,
                                      UtVec3dX&       aMoment_ftlbs,
                                      bool&           aMaxCompressionExceeded,
                                      double          aDeltaT_sec,
                                      const UtVec3dX& aAdditiveVelocityNED_fps);

   // This returns the braking handle
   size_t GetBrakingControlHandle() const;

   // This sets the normalized value (0 = off, 1 = full application of brake) of the brake
   void SetBraking(double aNormalizedValue);

   // This returns the normalized value of the brake
   double GetBraking() const;

   // This returns the steering handle
   size_t GetSteeringControlHandle() const;

   // This sets the steering angle (positive to the right)
   void SetSteeringAngle_deg(double aSteeringAngle_deg);

   // This returns the steering angle (positive to the right)
   double GetSteeringAngle_deg() const;

   // This returns true if the reaction point is a landing gear (as opposed to
   // being a generic contact point)
   bool IsLandingGear() { return mIsLandingGear; }

   // This returns a group of data that is useful for debugging
   void GetSteeringData(double& aCurrentSteeringAngle_deg, double& aMaxSteeringAngle_deg) const;

   // This returns a group of data that is useful for debugging
   void GetData(UtVec3dX& aGearPos_m,
                UtVec3dX& aGearCompressionVec,
                UtVec3dX& aCurrentGearRollingVec,
                UtVec3dX& aCurrentForce_lbs,
                UtVec3dX& aCurrentMoment_ftlbs,
                UtVec3dX& aCurrentNormalForce_lbs,
                UtVec3dX& aCurrentFrictionForce_lbs,
                double&   aUncompressedLength_ft,
                double&   aMaximumCompression_ft,
                double&   aCurrentCompression_ft,
                double&   aNormalizedBrakingValue,
                double&   aSteeringAngle_deg,
                double&   aHeightAboveTerrain_ft,
                bool&     aIsLandingGear,
                bool&     aInContactWithGround,
                bool&     aMaxCompressionExceeded);

   // This returns a group of data that is useful for debugging
   void GetCompressionData(double& aUncompressedLength_ft,
                           double& aMaximumCompression_ft,
                           double& aCurrentCompression_ft,
                           double& aNormalizedBrakingValue,
                           double& aSteeringAngle_deg,
                           bool&   aInContactWithGround,
                           bool&   aMaxCompressionExceeded);

   bool NotMoving() const;

protected:
   RigidBodyGroundReactionPoint(const RigidBodyGroundReactionPoint& aSrc) = default;

   // This is used by CalcGroundReactionForceMoment
   bool SetDataWhenNotInContactWithGround(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs, bool& aMaxCompressionExceeded);
   // This is used by CalcGroundReactionForceMoment
   bool SetDataWhenTinyDeltaT(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs, bool& aMaxCompressionExceeded);

   // This is used by CalcGroundReactionForceMomentWithoutFriction
   bool ReturnWhenNotInContactWithGround(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs, bool& aMaxCompressionExceeded);
   // This is used by CalcGroundReactionForceMomentWithoutFriction
   bool ReturnWhenTinyDeltaT(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs, bool& aMaxCompressionExceeded);

   // The CalcFrictionForceMoment function calculates the force and moment
   // produced by the friction force of the RigidBodyGroundReactionPoint. It is
   // used by the CalcFrictionForceMomentWhileInMotion and the
   // CalcFrictionForceMomentStartingToMove functions.
   void CalcFrictionForceMoment(UtVec3dX&       aForce_lbs,
                                UtVec3dX&       aMoment_ftlbs,
                                const UtVec3dX& aFrictionDirectionInSurfaceNED,
                                bool            aUseForceBasedFriction);

   std::string mControlSurfaceName;
   std::string mSteeringControlSurfaceName;
   std::string mNWSAngleControlSurfaceName;
   size_t      mControlSurfaceHandle         = 0;
   size_t      mSteeringControlSurfaceHandle = 0;
   size_t      mNWSAngleControlSurfaceHandle = 0;
   bool        mNoseGear                     = false;
   bool        mNWS_Engaged                  = false;
   bool        mExternalForceBeingApplied    = false;

   std::string mBrakeControlName;
   size_t      mBrakeControlHandle = 0;

   // Relative positioning
   UtVec3dX mGearPos_m;
   UtVec3dX mGearCompressionVec;
   UtVec3dX mGearRollingVec;

   // Characteristics
   double mSpringConstant_lbs_per_ft   = 0.0;
   double mDamperConstant_lbs_per_fps  = 0.0;
   double mUncompressedLength_ft       = 0.0;
   double mMaximumCompression_ft       = 0.0;
   double mRollingFrictionCoefficient  = 0.02;
   double mBrakingFrictionCoefficient  = 0.5;
   double mScuffingFrictionCoefficient = 0.4;
   bool   mUseFriction                 = true;
   bool   mIsLandingGear               = false;

   // Control values
   double mNormalizedBrakingValue = 0.0;
   double mSteeringAngle_deg      = 0.0;

   // Current conditions
   UtVec3dX mCurrentGearVel_NED_mps;
   UtVec3dX mCurrentGearRollingVecNED;
   UtVec3dX mCurrentNormalForceBody_lbs;   // Body coords
   UtVec3dX mCurrentFrictionForceBody_lbs; // Body coords
   UtVec3dX mCurrentForce_lbs;
   UtVec3dX mCurrentMoment_ftlbs; // Formerly mAxleDirectionVector
   UtVec3dX mCurrentExternalForce_lbs;
   double   mCurrentCompression_ft      = 0.0;
   double   mCurrentCompressionRate_fps = 0.0;
   double   mHeightAboveTerrain_ft      = 1000000.0;
   bool     mInContactWithGround        = false;
   bool     mMaxCompressionExceeded     = false;
   bool     mIsValid                    = false; // Flag is false when gear are not down and locked
   bool     mNotMoving                  = false;

   // Last conditions
   double   mLastCompression_ft      = 0.0;
   double   mLastCompressionRate_fps = 0.0;
   UtVec3dX mLastNetGearVelInSurfaceNED_fps;
   bool     mLastNetGearVelInSurfaceNEDValid = false;
   double   mLastNormalForceMagnitude_lbs    = 0.0;

   // Rolling coefficient varies with surface. Based on "Performance of Light
   // Aircraft" by John T. Lowry, typical values are as follows:
   //      Dry smooth concrete ....... 0.02
   //      Broken dry asphalt ........ 0.03
   //      Hard dirt ................. 0.04
   //      Short grass ............... 0.05
   //      Wet concrete/asphalt ...... 0.05
   //      Tall grass ................ 0.1
   //      Soft field ................ 0.2
   //
   // According to NACA Report 583 "The Rolling Friction of Several Airplane Wheels and Tires
   // and the Effect of Rolling Friction on Takeoff" (Wetmore, 1937), the rolling friction
   // can be as low as 0.01.
   //
   // According to NASA Technical Report R-20 "Tire-to-Surface Friction-Coefficient
   // Measurements with a C-123B Airplane on Various Runway Surfaces" (Sawyer and Kolnick,
   // 1959), a braking friction coefficient of 0.8 is typical on dry runways.
   //
   // Braking coefficient typically varies with speed. Most aircraft encounter a decreasing
   // braking coefficient with speed. See: "Wet Runway Friction: Literature and Information
   // Review" prepared for Transportation Development Centre on behalf of Aerodrome Safety
   // Branch Transport Canada, August 2001, Prepared by: G. Comfort. This report indicates
   // that an F-4D (with a dry runway surface) would encounter a coefficient from ~0.5 at
   // zero speed to only ~0.2 at 130 kts. Since the curve was nearly linear, it implies that
   // braking would be essentially non-existent above 220 kts. A Convair 990, however,
   // varied between 0.5 and 0.4 over the same speed range, so larger aircraft may not be
   // as susceptible to speed effects on friction coefficient.
   //
   // A general rule of thumb is that the static coefficient of friction is roughly
   // 1.5x the dynamic coefficient of friction.
};
} // namespace six_dof
} // namespace wsf

#endif
