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

#ifndef P6DOFAEROCOREOBJECT_H
#define P6DOFAEROCOREOBJECT_H

#include "p6dof_export.h"

#include <list>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class UtInput;

// The P6DofAeroCoreObject provides the main aerodynamics for an object.
// It includes various aero parameters and stability derivatives. It may
// include one or more aero modes, which provide configuration-dependent aero.
// Modes also provide a means to remove any aero effects, such as while a subobject
// is internally carried. A different mode can then be used when 'free'.

class P6DOF_EXPORT P6DofAeroCoreObject
{
public:
   P6DofAeroCoreObject();
   ~P6DofAeroCoreObject() = default;

   P6DofAeroCoreObject& operator=(const P6DofAeroCoreObject& other) = delete;

   P6DofAeroCoreObject* Clone() const;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);

   bool Initialize();

   // SetModeName is used to change the aero mode
   void        SetModeName(const std::string& aName);
   std::string GetModeName() const;

   const std::list<UtCloneablePtr<P6DofAeroCoreObject>>* GetSubModesList() const;
   P6DofAeroCoreObject*                                  GetSubModeByName(const std::string& aName) const;

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
   double GetRefArea_sqft() const;

   // Lift
   double CL_AlphaMach(double aMach, double aAlpha_rad); // TODO DEPRECATED ****************************** DEPRECATED
   double CL_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double CLq_AlphaMach(double aMach, double aAlpha_rad);
   double CL_AlphaDotAlphaMach(double aMach, double aAlpha_rad);

   // Drag
   double Cd_AlphaMach(double aMach, double aAlpha_rad); // TODO DEPRECATED ****************************** DEPRECATED
   double Cd_BetaMach(double aMach, double aBeta_rad);   // TODO DEPRECATED ****************************** DEPRECATED
   double Cd_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);

   // Side force
   double CY_BetaMach(double aMach, double aBeta_rad); // TODO DEPRECATED ****************************** DEPRECATED
   double CY_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double CYr_BetaMach(double aMach, double aBeta_rad);
   double CY_BetaDotBetaMach(double aMach, double aBeta_rad);

   // Pitching moments
   double Cm_AlphaMach(double aMach, double aAlpha_rad); // TODO DEPRECATED ****************************** DEPRECATED
   double Cm_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double Cmq_Mach(double aMach);
   double Cmp_Mach(double aMach);
   double CmAlphaDotMach(double aMach);

   // Yawing moments
   double Cn_BetaMach(double aMach, double aBeta_rad); // TODO DEPRECATED ****************************** DEPRECATED
   double Cn_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);
   double CnBetaDotMach(double aMach);
   double Cnr_Mach(double aMach);
   double Cnp_Mach(double aMach);

   // Rolling moments
   double Cl_BetaMach(double aMach, double aBeta_rad); // TODO DEPRECATED ****************************** DEPRECATED
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

   // This is deprecated. This is currently use to allow "legacy" stability derivatives.
   // However, it may be removed (along with legacy stability derivatives in later versions.
   bool UseLegacyAero() const { return mUseLegacy; }

protected:
   static bool ProcessCommonInput(UtInput& aInput, const std::string& aCommand, P6DofAeroCoreObject* aObject);

   P6DofAeroCoreObject(const P6DofAeroCoreObject& aSrc) = default;

   std::string mModeName = "DEFAULT";

   std::list<UtCloneablePtr<P6DofAeroCoreObject>> mSubModesList;

   double mWingChord_ft  = 0.0;
   double mWingSpan_ft   = 0.0;
   double mWingArea_sqft = 0.0;
   double mRefArea_sqft  = 0.0;
   double mRefLength_ft  = 0.0;

   // When mUseRefArea is true, mRefArea_sqft is used instead of mWingArea_sqft, mWingSpan_ft, and mWingChord_ft
   bool mUseRefArea = false;

   UtVec3dX mAeroCenter_ft;

   // This "legacy" flag allows the use of older aero stability derivatives, which are deprecated and should no longer be used
   bool mUseLegacy    = false; //! @deprecated
   bool mUseLegacySet = false;

   // This "reduced frequency" flag allows the use of reduced frequency rather than angular rates to compute aerodynamic derivatives
   bool mUseReducedFrequency    = true;
   bool mUseReducedFrequencySet = false;

   // Lift
   UtCloneablePtr<UtTable::Table> mCL_AlphaMachTablePtr{nullptr}; //! @deprecated
   UtCloneablePtr<UtTable::Table> mCL_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCLq_AlphaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCL_AlphaDotAlphaMachTablePtr{nullptr};

   // Drag
   UtCloneablePtr<UtTable::Table> mCd_AlphaMachTablePtr{nullptr}; //! @deprecated
   UtCloneablePtr<UtTable::Table> mCd_BetaMachTablePtr{nullptr};  //! @deprecated
   UtCloneablePtr<UtTable::Table> mCd_AlphaBetaMachTablePtr{nullptr};

   // Side force
   UtCloneablePtr<UtTable::Table> mCY_BetaMachTablePtr{nullptr}; //! @deprecated
   UtCloneablePtr<UtTable::Table> mCY_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCYr_BetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCY_BetaDotBetaMachTablePtr{nullptr};

   // Pitching moments
   UtCloneablePtr<UtTable::Table> mCm_AlphaMachTablePtr{nullptr}; //! @deprecated
   UtCloneablePtr<UtTable::Table> mCm_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCmq_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCmp_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCm_AlphaDotMachCurvePtr{nullptr};

   // Yawing moments
   UtCloneablePtr<UtTable::Table> mCn_BetaMachTablePtr{nullptr}; //! @deprecated
   UtCloneablePtr<UtTable::Table> mCn_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCn_BetaDotMachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCnr_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCnp_MachCurvePtr{nullptr};

   // Rolling moments
   UtCloneablePtr<UtTable::Table> mCl_BetaMachTablePtr{nullptr}; //! @deprecated
   UtCloneablePtr<UtTable::Table> mCl_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mClp_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCl_AlphaDotMachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCl_BetaDotMachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mClr_MachCurvePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mClq_MachCurvePtr{nullptr};
};

#endif
