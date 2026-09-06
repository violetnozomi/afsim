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

#ifndef WSFPOINTMASSSIXDOFAEROCOREOBJECT_HPP
#define WSFPOINTMASSSIXDOFAEROCOREOBJECT_HPP

#include "WsfSixDOF_AeroCoreObject.hpp"

namespace wsf
{
namespace six_dof
{
// The PointMassAeroCoreObject provides the main aerodynamics for an object.
// It includes various aero parameters and stability derivatives. It may
// include one or more aero modes, which provide configuration-dependent aero.
// Modes also provide a means to remove any aero effects, such as while a subobject
// is internally carried. A different mode can then be used when 'free'.

class WSF_SIX_DOF_EXPORT PointMassAeroCoreObject : public AeroCoreObject
{
public:
   PointMassAeroCoreObject()  = default;
   ~PointMassAeroCoreObject() = default;

   PointMassAeroCoreObject& operator=(const PointMassAeroCoreObject& other) = delete;

   PointMassAeroCoreObject* Clone() const;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);

   const std::list<UtCloneablePtr<PointMassAeroCoreObject>>* GetSubModesList() const;
   PointMassAeroCoreObject*                                  GetSubModeByName(const std::string& aName) const;

   // The CalculateAeroFM function calculates the lift, drag, and
   // side force magnitudes and the moment vector.
   void CalculateCoreAeroFM(double  aDynPress_lbsqft,
                            double  aMach,
                            double  aAlpha_rad,
                            double  aBeta_rad,
                            double  aSpeedbrakeLever,
                            double  aFlapsLever,
                            double  aSpoilersLever,
                            double& aLift_lbs,
                            double& aDrag_lbs,
                            double& aSideForce_lbs,
                            double& aMaximumRollAcceleration_rps2,
                            double& aMaximumPitchAcceleration_rps2,
                            double& aMaximumYawAcceleration_rps2,
                            double& aAlphaStabilizingFrequency_rps,
                            double& aBetaStabilizingFrequency_rps,
                            double& aRollStabilizingFrequency_rps,
                            double  aRadiusSizeFactor = 1.0);

   // The CalculateAeroFM function calculates the lift
   double CalculateLiftAtSpecifiedAlpha_lbs(double aDynPress_lbsqft,
                                            double aMach,
                                            double aAlpha_deg,
                                            double aRadiusSizeFactor = 1.0);

   // This calculates the lift coefficient (CL), drag coefficient (Cd) and
   // pitching moment coefficient (Cm) including reference area effects but not
   // including dynamic pressure effects.
   void CalculateAeroCoefficientVectors(double  aMach,
                                        double  aAlpha_rad,
                                        double& aCLArea,
                                        double& aCdArea,
                                        double& aCmArea,
                                        double  aRadiusSizeFactor = 1.0);

   // Non-trim effector deltas
   double SpeedbrakeDeltaCd_Mach(double aMach);
   double FlapsDeltaCd_Mach(double aMach);
   double FlapsDeltaCL_Mach(double aMach);
   double SpoilersDeltaCd_Mach(double aMach);
   double SpoilersDeltaCL_Mach(double aMach);

   // Roll dynamics
   double MaximumRollAcceleration_Mach(double aMach);
   double RollStabilizingFrequency_Mach(double aMach);

   // Pitch dynamics
   double MaximumPitchAcceleration_Mach(double aMach);
   double AlphaStabilizingFrequency_Mach(double aMach);

   // Yaw dynamics
   double MaximumYawAcceleration_Mach(double aMach);
   double BetaStabilizingFrequency_Mach(double aMach);

protected:
   static bool ProcessCommonInput(UtInput& aInput, const std::string& aCommand, PointMassAeroCoreObject* aObject);

   PointMassAeroCoreObject(const PointMassAeroCoreObject& aSrc) = default;

   std::list<UtCloneablePtr<PointMassAeroCoreObject>> mSubModesList;

   // High-drag / high-lift effectors for aircraft
   UtCloneablePtr<UtTable::Table> mFlapsDeltaCL_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mFlapsDeltaCd_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mSpoilersDeltaCL_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mSpoilersDeltaCd_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mSpeedbrakeDeltaCd_MachTablePtr{nullptr};

   // Rotation control dynamics (deg/s/s)
   UtCloneablePtr<UtTable::Table> mMaximumRollAcceleration_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mMaximumPitchAcceleration_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mMaximumYawAcceleration_MachTablePtr{nullptr};

   // Rotation plant dynamics (Hz)
   UtCloneablePtr<UtTable::Table> mAlphaStabilizingFrequency_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mBetaStabilizingFrequency_MachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mRollStabilizingFrequency_MachTablePtr{nullptr};
};
} // namespace six_dof
} // namespace wsf

#endif
