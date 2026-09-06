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

#include "WsfRigidBodySixDOF_AeroMovableObject.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3dX.hpp"

wsf::six_dof::RigidBodyAeroMovableObject* wsf::six_dof::RigidBodyAeroMovableObject::Clone() const
{
   return new RigidBodyAeroMovableObject(*this);
}

void wsf::six_dof::RigidBodyAeroMovableObject::SetReferenceArea(double aRefArea_sqft)
{
   mUseExternalRefArea   = true;
   mUseInternalRefArea   = false;
   mRefExternalArea_sqft = aRefArea_sqft;
}

void wsf::six_dof::RigidBodyAeroMovableObject::SetReferenceAreas(double aWingChord_ft, double aWingSpan_ft, double aWingArea_sqft)
{
   mRefWingChord_ft  = aWingChord_ft;
   mRefWingSpan_ft   = aWingSpan_ft;
   mRefWingArea_sqft = aWingArea_sqft;
}

bool wsf::six_dof::RigidBodyAeroMovableObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "aero_component")
   {
      UtInputBlock block(aInput, "end_aero_component");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "type")
         {
            std::string value;
            aInput.ReadValue(value);
            mName = value;
         }
         else if (localCommand == "ref_area_sqft")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mRefInternalArea_sqft = value;
            mUseInternalRefArea   = true;
         }
         else if (localCommand == "cL_angle_alpha_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cL_angle_alpha_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCL_AngleAlphaMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cd_angle_alpha_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cd_angle_alpha_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCd_AngleAlphaMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cd_angle_beta_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cd_angle_beta_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("beta",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                            1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCd_AngleBetaMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cd_angle_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cd_angle_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            1);

               mCd_AngleMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cy_angle_beta_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cy_angle_beta_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("beta",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                            1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCY_AngleBetaMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cm_angle_alpha_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cm_angle_alpha_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCm_AngleAlphaMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cn_angle_beta_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cn_angle_beta_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("beta",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                            1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCn_AngleBetaMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cl_angle_alpha_beta_table")
         {
            UtInputBlock subBlock(aInput, "end_cl_angle_alpha_beta_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("beta",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                            0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            2);

               mCl_AngleAlphaBetaTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cmq_angle_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cmq_angle_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            1);

               mCmq_AngleMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "cnr_angle_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_cnr_angle_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            1);

               mCnr_AngleMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "clp_angle_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_clp_angle_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            1);

               mClp_AngleMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "clq_angle_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_clq_angle_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            1);

               mClq_AngleMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "clr_angle_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_clr_angle_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("surface_angle",
                                            UtInput::cANGLE,
                                            UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI),
                                            1);

               mClr_AngleMachTablePtr =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "use_reduced_frequency")
         {
            mUseReducedFrequency = aInput.ReadBool();
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RigidBodyAeroMovableObject::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::RigidBodyAeroMovableObject::Initialize()
{
   return true;
}

std::string wsf::six_dof::RigidBodyAeroMovableObject::GetName() const
{
   return mName;
}

double wsf::six_dof::RigidBodyAeroMovableObject::CL_AngleAlphaMach(double aMach, double aAlpha_rad, double aAngle_deg) const
{
   if (mCL_AngleAlphaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCL_AngleAlphaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cd_AngleAlphaMach(double aMach, double aAlpha_rad, double aAngle_deg) const
{
   if (mCd_AngleAlphaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCd_AngleAlphaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cd_AngleBetaMach(double aMach, double aBeta_rad, double aAngle_deg) const
{
   if (mCd_AngleBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCd_AngleBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cd_AngleMach(double aMach, double aAngle_deg) const
{
   if (mCd_AngleMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCd_AngleMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::CY_AngleBetaMach(double aMach, double aBeta_rad, double aAngle_deg) const
{
   if (mCY_AngleBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCY_AngleBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cm_AngleAlphaMach(double aMach, double aAlpha_rad, double aAngle_deg) const
{
   if (mCm_AngleAlphaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCm_AngleAlphaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cmq_AngleMach(double aMach, double aAngle_deg) const
{
   if (mCmq_AngleMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCmq_AngleMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cn_AngleBetaMach(double aMach, double aBeta_rad, double aAngle_deg) const
{
   if (mCn_AngleBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCn_AngleBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cnr_AngleMach(double aMach, double aAngle_deg) const
{
   if (mCnr_AngleMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCnr_AngleMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Cl_AngleAlphaBeta(double aAlpha_rad, double aBeta_rad, double aAngle_deg) const
{
   if (mCl_AngleAlphaBetaTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aBeta_rad;
   lookupArgs[1] = aAlpha_rad;
   lookupArgs[2] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mCl_AngleAlphaBetaTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Clp_AngleMach(double aMach, double aAngle_deg) const
{
   if (mClp_AngleMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mClp_AngleMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Clq_AngleMach(double aMach, double aAngle_deg) const
{
   if (mClq_AngleMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mClq_AngleMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroMovableObject::Clr_AngleMach(double aMach, double aAngle_deg) const
{
   if (mClr_AngleMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAngle_deg * UtMath::cRAD_PER_DEG;
   return mClr_AngleMachTablePtr->Lookup(lookupArgs);
}

void wsf::six_dof::RigidBodyAeroMovableObject::CalculateMovableAeroFM(double          aDynPress_lbsqft,
                                                                      double          aMach,
                                                                      double          aSpeed_fps,
                                                                      double          aAlpha_rad,
                                                                      double          aBeta_rad,
                                                                      const UtVec3dX& aAngularRates_rps,
                                                                      UtVec3dX&       aMoment_ftlbs,
                                                                      double&         aLift_lbs,
                                                                      double&         aDrag_lbs,
                                                                      double&         aSideForce_lbs,
                                                                      double&         aLiftFactor,
                                                                      double          aAngle_deg)
{
   // Get individual angular rates
   double yawRate_rps   = 0.0;
   double pitchRate_rps = 0.0;
   double rollRate_rps  = 0.0;
   aAngularRates_rps.Get(rollRate_rps, pitchRate_rps, yawRate_rps);

   double CL       = 0;
   double Cd_a     = 0;
   double Cd_b     = 0;
   double Cd       = 0;
   double CY       = 0;
   double Cm       = 0;
   double Cn       = 0;
   double Cl       = 0;
   double CLFactor = 1.0; // Default is 1

   // Note that these functions use alpha/beta in radians and angle in degrees

   // Lift
   CL += CL_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);

   // Drag -- note that only one of these next three should be used
   Cd_a += Cd_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);
   Cd_b += Cd_AngleBetaMach(aMach, aBeta_rad, aAngle_deg);
   Cd += Cd_AngleMach(aMach, aAngle_deg);

   // Side force
   CY += CY_AngleBetaMach(aMach, aBeta_rad, aAngle_deg);

   // Yaw control
   Cn += Cn_AngleBetaMach(aMach, aBeta_rad, aAngle_deg);

   // Pitch control
   Cm += Cm_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);

   // Roll control
   Cl += Cl_AngleAlphaBeta(aAlpha_rad, aBeta_rad, aAngle_deg);

   double speedSafe_fps = std::max(aSpeed_fps, 1.0);

   // Rate-based derivatives
   double kmq = pitchRate_rps / (2 * speedSafe_fps);
   double knr = yawRate_rps / (2 * speedSafe_fps);
   double klp = rollRate_rps / (2 * speedSafe_fps);
   double klq = pitchRate_rps / (2 * speedSafe_fps);
   double klr = yawRate_rps / (2 * speedSafe_fps);

   if (mUseExternalRefArea)
   {
      kmq *= std::sqrt(mRefExternalArea_sqft);
      knr *= std::sqrt(mRefExternalArea_sqft);
      klp *= std::sqrt(mRefExternalArea_sqft);
      klq *= std::sqrt(mRefExternalArea_sqft);
      klr *= std::sqrt(mRefExternalArea_sqft);
   }
   else if (mUseInternalRefArea)
   {
      kmq *= std::sqrt(mRefInternalArea_sqft);
      knr *= std::sqrt(mRefInternalArea_sqft);
      klp *= std::sqrt(mRefInternalArea_sqft);
      klq *= std::sqrt(mRefInternalArea_sqft);
      klr *= std::sqrt(mRefInternalArea_sqft);
   }
   else
   {
      kmq *= mRefWingChord_ft;
      knr *= mRefWingSpan_ft;
      klp *= mRefWingSpan_ft;
      klq *= mRefWingSpan_ft;
      klr *= mRefWingSpan_ft;
   }

   if (!mUseReducedFrequency)
   {
      kmq = pitchRate_rps;
      knr = yawRate_rps;
      klp = rollRate_rps;
      klq = pitchRate_rps;
      klr = yawRate_rps;
   }

   Cm += Cmq_AngleMach(aMach, aAngle_deg) * kmq;
   Cn += Cnr_AngleMach(aMach, aAngle_deg) * knr;
   Cl += Clp_AngleMach(aMach, aAngle_deg) * klp;
   Cl += Clq_AngleMach(aMach, aAngle_deg) * klq;
   Cl += Clr_AngleMach(aMach, aAngle_deg) * klr;

   // Set the deprecated spoiler effect
   aLiftFactor = CLFactor;

   if (mUseExternalRefArea)
   {
      aLift_lbs      = aDynPress_lbsqft * CL * mRefExternalArea_sqft;
      aDrag_lbs      = aDynPress_lbsqft * (Cd_a + Cd_b + Cd) * mRefExternalArea_sqft;
      aSideForce_lbs = aDynPress_lbsqft * CY * mRefExternalArea_sqft;
   }
   else if (mUseInternalRefArea)
   {
      aLift_lbs      = aDynPress_lbsqft * CL * mRefInternalArea_sqft;
      aDrag_lbs      = aDynPress_lbsqft * (Cd_a + Cd_b + Cd) * mRefInternalArea_sqft;
      aSideForce_lbs = aDynPress_lbsqft * CY * mRefInternalArea_sqft;
   }
   else
   {
      aLift_lbs      = aDynPress_lbsqft * CL * mRefWingArea_sqft;
      aDrag_lbs      = aDynPress_lbsqft * (Cd_a + Cd_b + Cd) * mRefWingArea_sqft;
      aSideForce_lbs = aDynPress_lbsqft * CY * mRefWingArea_sqft;
   }

   double yaw_moment   = 0.0;
   double pitch_moment = 0.0;
   double roll_moment  = 0.0;

   if (mUseExternalRefArea)
   {
      yaw_moment   = aDynPress_lbsqft * Cn * mRefExternalArea_sqft;
      pitch_moment = aDynPress_lbsqft * Cm * mRefExternalArea_sqft;
      roll_moment  = aDynPress_lbsqft * Cl * mRefExternalArea_sqft;
   }
   else if (mUseInternalRefArea)
   {
      yaw_moment   = aDynPress_lbsqft * Cn * mRefInternalArea_sqft;
      pitch_moment = aDynPress_lbsqft * Cm * mRefInternalArea_sqft;
      roll_moment  = aDynPress_lbsqft * Cl * mRefInternalArea_sqft;
   }
   else
   {
      yaw_moment   = aDynPress_lbsqft * Cn * mRefWingArea_sqft * mRefWingSpan_ft;
      pitch_moment = aDynPress_lbsqft * Cm * mRefWingArea_sqft * mRefWingChord_ft;
      roll_moment  = aDynPress_lbsqft * Cl * mRefWingArea_sqft * mRefWingSpan_ft;
   }

   aMoment_ftlbs.Set(roll_moment, pitch_moment, yaw_moment);
}

double wsf::six_dof::RigidBodyAeroMovableObject::CalculateMovableAeroCmArea(double aMach,
                                                                            double aAlpha_rad,
                                                                            double aAngle_deg) const
{
   double pitch_moment = 0.0;

   double Cm = Cm_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);

   if (mUseExternalRefArea)
   {
      pitch_moment = Cm * mRefExternalArea_sqft;
   }
   else if (mUseInternalRefArea)
   {
      pitch_moment = Cm * mRefInternalArea_sqft;
   }
   else
   {
      pitch_moment = Cm * mRefWingArea_sqft * mRefWingChord_ft;
   }

   return pitch_moment;
}

double wsf::six_dof::RigidBodyAeroMovableObject::CalculateMovableAeroCLArea(double aMach,
                                                                            double aAlpha_rad,
                                                                            double aAngle_deg) const
{
   double CL      = CL_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);
   double CL_Area = 0.0;

   if (mUseExternalRefArea)
   {
      CL_Area = CL * mRefExternalArea_sqft;
   }
   else if (mUseInternalRefArea)
   {
      CL_Area = CL * mRefInternalArea_sqft;
   }
   else
   {
      CL_Area = CL * mRefWingArea_sqft;
   }

   return CL_Area;
}

double wsf::six_dof::RigidBodyAeroMovableObject::CalculateMovableAeroCL(double aMach, double aAlpha_rad, double aAngle_deg) const
{
   return CL_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);
}

double wsf::six_dof::RigidBodyAeroMovableObject::CalculateMovableAeroCdArea(double aMach,
                                                                            double aAlpha_rad,
                                                                            double aAngle_deg) const
{
   // Note: There are three types of Cd:  Cd-Alpha-Mach, Cd-Beta-Mach, and Cd-Mach
   // We need to choose the most appropriate. We will give priority in order
   // of Alpha-Mach, Beta-Mach, and Mach.

   double Cd = 0.0;

   if (mCd_AngleAlphaMachTablePtr != nullptr)
   {
      Cd = Cd_AngleAlphaMach(aMach, aAlpha_rad, aAngle_deg);
   }
   else if (mCd_AngleBetaMachTablePtr != nullptr)
   {
      Cd = Cd_AngleBetaMach(aMach, 0.0, aAngle_deg); // We assume beta = zero
   }
   else if (mCd_AngleMachTablePtr != nullptr)
   {
      Cd = Cd_AngleMach(aMach, aAngle_deg);
   }

   double Cd_Area = 0.0;

   if (mUseExternalRefArea)
   {
      Cd_Area = Cd * mRefExternalArea_sqft;
   }
   else if (mUseInternalRefArea)
   {
      Cd_Area = Cd * mRefInternalArea_sqft;
   }
   else
   {
      Cd_Area = Cd * mRefWingArea_sqft;
   }

   return Cd_Area;
}

void wsf::six_dof::RigidBodyAeroMovableObject::SetExtendedValue(double aExtendedValue)
{
   mExtendedValue = aExtendedValue;
}

void wsf::six_dof::RigidBodyAeroMovableObject::SetAngle_deg(double aAngle_deg)
{
   mAngle_deg = aAngle_deg;
}

void wsf::six_dof::RigidBodyAeroMovableObject::SetControlSurfaceHandle(size_t aHandle)
{
   mControlSurfaceHandle = aHandle;
}

size_t wsf::six_dof::RigidBodyAeroMovableObject::GetControlSurfaceHandle() const
{
   return mControlSurfaceHandle;
}
