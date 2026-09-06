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

#ifndef WSFRIGIDBODYSIXDOFAEROCOREOBJECT_HPP
#define WSFRIGIDBODYSIXDOFAEROCOREOBJECT_HPP

#include "WsfSixDOF_AeroCoreObject.hpp"

// The RigidBodyAeroCoreObject provides the main aerodynamics for an object.
// It includes various aero parameters and stability derivatives. It may
// include one or more aero modes, which provide configuration-dependent aero.
// Modes also provide a means to remove any aero effects, such as while a subobject
// is internally carried. A different mode can then be used when 'free'.

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RigidBodyAeroCoreObject : public AeroCoreObject
{
public:
   RigidBodyAeroCoreObject()  = default;
   ~RigidBodyAeroCoreObject() = default;

   RigidBodyAeroCoreObject& operator=(const RigidBodyAeroCoreObject& other) = delete;

   RigidBodyAeroCoreObject* Clone() const;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);

   bool Initialize() override;

   // SetModeName is used to change the aero mode
   void        SetModeName(const std::string& aName);
   std::string GetModeName() const;

   const std::list<UtCloneablePtr<RigidBodyAeroCoreObject>>* GetSubModesList() const;
   RigidBodyAeroCoreObject*                                  GetSubModeByName(const std::string& aName) const;

   // The CalculateAeroFM function calculates the lift, drag, and
   // side force magnitudes and the moment vector.
   void CalculateCoreAeroFM(double          aDynPress_lbsqft,
                            double          aMach,
                            double          aSpeed_fps,
                            double          aAlpha_rad,
                            double          aBeta_rad,
                            double          aAlphaDot_rps,
                            double          aBetaDot_rps,
                            const UtVec3dX& aAngularRates_rps,
                            UtVec3dX&       aMoment_ftlbs,
                            double&         aLift_lbs,
                            double&         aDrag_lbs,
                            double&         aSideForce_lbs,
                            double          aRadiusSizeFactor = 1.0);

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

   UtVec3dX GetAeroCenter_ft() const;

   double GetWingChord_ft() const;
   double GetWingSpan_ft() const;
   double GetWingArea_sqft() const;

   // Lift
   double CLq_AlphaMach(double aMach, double aAlpha_rad);
   double CL_AlphaDotAlphaMach(double aMach, double aAlpha_rad);

   // Drag

   // Side force
   double CYr_BetaMach(double aMach, double aBeta_rad);
   double CY_BetaDotBetaMach(double aMach, double aBeta_rad);

   // Pitching moments
   double Cm_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double Cmq_Mach(double aMach);
   double Cmp_Mach(double aMach);
   double CmAlphaDotMach(double aMach);

   // Yawing moments
   double Cn_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double CnBetaDotMach(double aMach);
   double Cnr_Mach(double aMach);
   double Cnp_Mach(double aMach);

   // Rolling moments
   double Cl_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double Clp_Mach(double aMach);
   double Cl_AlphaDotMach(double aMach);
   double Cl_BetaDotMach(double aMach);
   double Clr_Mach(double aMach);
   double Clq_Mach(double aMach);

   // The CalculateAeroCmArea function is called to calculate the pitching moment (Cm)
   // including reference area effects but not including dynamic pressure effects.
   double CalculateAeroCmArea(double aMach, double aAlpha_rad);

   // The CalculateAeroCLArea function is called to calculate the lift coefficient (CL)
   // including reference area effects but not including dynamic pressure effects.
   double CalculateAeroCLArea(double aMach, double aAlpha_rad);

   // The CalculateAeroCdArea function is called to calculate the drag coefficient (Cd)
   // including reference area effects but not including dynamic pressure effects.
   double CalculateAeroCdArea(double aMach, double aAlpha_rad);

   // This returns true if ref area is used instead of wing area, wing span, and wing chord
   bool UsesRefArea() const { return mUseRefArea; }

protected:
   static bool ProcessCommonInput(UtInput& aInput, const std::string& aCommand, RigidBodyAeroCoreObject* aObject);

   RigidBodyAeroCoreObject(const RigidBodyAeroCoreObject& aSrc) = default;

   std::list<UtCloneablePtr<RigidBodyAeroCoreObject>> mSubModesList;

   double mWingChord_ft  = 0.0;
   double mWingSpan_ft   = 0.0;
   double mWingArea_sqft = 0.0;
   double mRefLength_ft  = 0.0;

   // When mUseRefArea is true, mRefArea_sqft is used instead of mWingArea_sqft, mWingSpan_ft, and mWingChord_ft
   bool mUseRefArea = false;

   UtVec3dX mAeroCenter_ft;

   // This "reduced frequency" flag allows the use of reduced frequency rather than angular rates to compute aerodynamic derivatives
   bool mUseReducedFrequency = true;

   // Lift
   UtCloneablePtr<UtTable::Table> mCLq_AlphaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCL_AlphaDotAlphaMachTablePtr{nullptr};

   // Drag

   // Side force
   UtCloneablePtr<UtTable::Table> mCYr_BetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCY_BetaDotBetaMachTablePtr{nullptr};

   // Pitching moments
   UtCloneablePtr<UtTable::Table> mCm_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCmq_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCmp_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCm_AlphaDotMachCurvePtr{nullptr};

   // Yawing moments
   UtCloneablePtr<UtTable::Table> mCn_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCn_BetaDotMachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCnr_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCnp_MachCurvePtr{nullptr};

   // Rolling moments
   UtCloneablePtr<UtTable::Table> mCl_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mClp_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCl_AlphaDotMachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCl_BetaDotMachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mClr_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mClq_MachCurvePtr{nullptr};
};
} // namespace six_dof
} // namespace wsf

#endif
