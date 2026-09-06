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

#include "WsfPointMassSixDOF_AeroCoreObject.hpp"

#include <cstdio>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"

wsf::six_dof::PointMassAeroCoreObject* wsf::six_dof::PointMassAeroCoreObject::Clone() const
{
   return new PointMassAeroCoreObject(*this);
}

bool wsf::six_dof::PointMassAeroCoreObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "aero_data")
   {
      UtInputBlock block(aInput, "end_aero_data");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         // First check to see if the command is common between aero_data and aero_mode
         if (ProcessCommonInput(aInput, localCommand, this))
         {
            // do nothing, already handled in ProcessCommonInput()
         }
         else if (localCommand == "aero_mode")
         {
            std::string modeName;
            aInput.ReadValue(modeName);

            auto tempModeObj = ut::make_unique<PointMassAeroCoreObject>();
            tempModeObj->SetModeName(modeName);
            mSubModesList.emplace_back(std::move(tempModeObj));

            UtInputBlock subBlock(aInput, "end_aero_mode");
            while (subBlock.ReadCommand())
            {
               std::string subCommand = block.GetCommand();
               if (!ProcessCommonInput(aInput, subCommand, mSubModesList.back().Pointer()))
               {
                  throw UtInput::UnknownCommand(aInput);
               }
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::PointMassAeroCoreObject::ProcessCommonInput(UtInput&                 aInput,
                                                               const std::string&       aCommand,
                                                               PointMassAeroCoreObject* aObject)
{
   bool retVal = true;

   if (aCommand == "ref_area_sqft")
   {
      double value = 0.0;
      aInput.ReadValue(value);
      aObject->mRefArea_sqft = value;
   }
   else if (aCommand == "cL_alpha_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cL_alpha_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 2);

         aObject->mCL_AlphaBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cd_alpha_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cd_alpha_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 2);

         aObject->mCd_AlphaBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cy_alpha_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cy_alpha_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 2);

         aObject->mCY_AlphaBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "maximum_roll_acceleration_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_maximum_roll_acceleration_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mMaximumRollAcceleration_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "maximum_pitch_acceleration_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_maximum_pitch_acceleration_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mMaximumPitchAcceleration_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "maximum_yaw_acceleration_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_maximum_yaw_acceleration_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mMaximumYawAcceleration_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "alpha_stabilizing_frequency_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_alpha_stabilizing_frequency_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 0);

         aObject->mAlphaStabilizingFrequency_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "beta_stabilizing_frequency_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_beta_stabilizing_frequency_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 0);

         aObject->mBetaStabilizingFrequency_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "roll_stabilizing_frequency_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_roll_stabilizing_frequency_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 0);

         aObject->mRollStabilizingFrequency_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "speedbrake_dcd_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_speedbrake_dcd_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mSpeedbrakeDeltaCd_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "flaps_dcl_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_flaps_dcl_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mFlapsDeltaCL_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "flaps_dcd_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_flaps_dcd_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mFlapsDeltaCd_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "spoilers_dcl_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_spoilers_dcl_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mSpoilersDeltaCL_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "spoilers_dcd_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_spoilers_dcd_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);

         aObject->mSpoilersDeltaCd_MachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else
   {
      retVal = false;
   }
   return retVal;
}

const std::list<UtCloneablePtr<wsf::six_dof::PointMassAeroCoreObject>>*
wsf::six_dof::PointMassAeroCoreObject::GetSubModesList() const
{
   return &mSubModesList;
}

wsf::six_dof::PointMassAeroCoreObject* wsf::six_dof::PointMassAeroCoreObject::GetSubModeByName(const std::string& aName) const
{
   auto iter = mSubModesList.begin();
   while (iter != mSubModesList.end())
   {
      if ((*iter)->GetModeName() == aName)
      {
         return (*iter);
      }
      ++iter;
   }
   return nullptr;
}

double wsf::six_dof::PointMassAeroCoreObject::MaximumRollAcceleration_Mach(double aMach)
{
   if (mMaximumRollAcceleration_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mMaximumRollAcceleration_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::MaximumPitchAcceleration_Mach(double aMach)
{
   if (mMaximumPitchAcceleration_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mMaximumPitchAcceleration_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::MaximumYawAcceleration_Mach(double aMach)
{
   if (mMaximumYawAcceleration_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mMaximumYawAcceleration_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::AlphaStabilizingFrequency_Mach(double aMach)
{
   if (mAlphaStabilizingFrequency_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mAlphaStabilizingFrequency_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::BetaStabilizingFrequency_Mach(double aMach)
{
   if (mBetaStabilizingFrequency_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mBetaStabilizingFrequency_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::RollStabilizingFrequency_Mach(double aMach)
{
   if (mRollStabilizingFrequency_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mRollStabilizingFrequency_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::SpeedbrakeDeltaCd_Mach(double aMach)
{
   if (mSpeedbrakeDeltaCd_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mSpeedbrakeDeltaCd_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::SpoilersDeltaCd_Mach(double aMach)
{
   if (mSpoilersDeltaCd_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mSpoilersDeltaCd_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::SpoilersDeltaCL_Mach(double aMach)
{
   if (mSpoilersDeltaCL_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mSpoilersDeltaCL_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::FlapsDeltaCd_Mach(double aMach)
{
   if (mFlapsDeltaCd_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mFlapsDeltaCd_MachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::PointMassAeroCoreObject::FlapsDeltaCL_Mach(double aMach)
{
   if (mFlapsDeltaCL_MachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[1];
   lookupArgs[0] = aMach;
   return mFlapsDeltaCL_MachTablePtr->Lookup(lookupArgs);
}

void wsf::six_dof::PointMassAeroCoreObject::CalculateCoreAeroFM(double  aDynPress_lbsqft,
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
                                                                double  aRadiusSizeFactor)
{
   double CL = CL_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad) + aSpoilersLever * SpoilersDeltaCL_Mach(aMach) +
               aFlapsLever * FlapsDeltaCL_Mach(aMach);
   double Cd = Cd_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad) + aSpeedbrakeLever * SpeedbrakeDeltaCd_Mach(aMach) +
               aSpoilersLever * SpoilersDeltaCd_Mach(aMach) + aFlapsLever * FlapsDeltaCd_Mach(aMach);
   double CY = CY_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);

   // Adjust for size factor effects (size factor for parachutes, balloons, etc)
   // Size factor is radius based and we need area, so we square the factor.
   double areaMultiplier = aRadiusSizeFactor * aRadiusSizeFactor;

   aLift_lbs      = aDynPress_lbsqft * CL * mRefArea_sqft * areaMultiplier;
   aDrag_lbs      = aDynPress_lbsqft * Cd * mRefArea_sqft * areaMultiplier;
   aSideForce_lbs = aDynPress_lbsqft * CY * mRefArea_sqft * areaMultiplier;

   double alphaLimited_rad = UtMath::Limit(aAlpha_rad, UtMath::cPI_OVER_2);
   double betaLimited_rad  = UtMath::Limit(aBeta_rad, UtMath::cPI_OVER_2);
   double cosAlphaTotal    = cos(alphaLimited_rad) * cos(betaLimited_rad);

   // Modify aero contribution as a function of the angle of attack vector.
   // As we approach and exceed 90 degrees pitch, pitch control authority should vanish.
   // As we approach and exceed 90 degrees yaw, yaw control authority should vanish.
   // As we approach and exceed 90 degrees total angle of attack, roll control authority should vanish.
   // This is a coarse, one-size-fits-all approximation, and will not model precise stall effects.
   double maximumRollAcceleration_dps2  = MaximumRollAcceleration_Mach(aMach) * cosAlphaTotal;
   double maximumPitchAcceleration_dps2 = MaximumPitchAcceleration_Mach(aMach) * cosAlphaTotal;
   double maximumYawAcceleration_dps2   = MaximumYawAcceleration_Mach(aMach) * cosAlphaTotal;

   aMaximumRollAcceleration_rps2  = UtMath::cRAD_PER_DEG * maximumRollAcceleration_dps2;
   aMaximumPitchAcceleration_rps2 = UtMath::cRAD_PER_DEG * maximumPitchAcceleration_dps2;
   aMaximumYawAcceleration_rps2   = UtMath::cRAD_PER_DEG * maximumYawAcceleration_dps2;

   if (aMaximumRollAcceleration_rps2 < 0.0)
   {
      aMaximumRollAcceleration_rps2 = 0.0;
   }
   if (aMaximumPitchAcceleration_rps2 < 0.0)
   {
      aMaximumPitchAcceleration_rps2 = 0.0;
   }
   if (aMaximumYawAcceleration_rps2 < 0.0)
   {
      aMaximumYawAcceleration_rps2 = 0.0;
   }

   double alphaStabilizingFrequency_Hz = AlphaStabilizingFrequency_Mach(aMach);
   double betaStabilizingFrequency_Hz  = BetaStabilizingFrequency_Mach(aMach);
   double rollStabilizingFrequency_Hz  = RollStabilizingFrequency_Mach(aMach);

   aAlphaStabilizingFrequency_rps = UtMath::cTWO_PI * alphaStabilizingFrequency_Hz;
   aBetaStabilizingFrequency_rps  = UtMath::cTWO_PI * betaStabilizingFrequency_Hz;
   aRollStabilizingFrequency_rps  = UtMath::cTWO_PI * rollStabilizingFrequency_Hz;
}

void wsf::six_dof::PointMassAeroCoreObject::CalculateAeroCoefficientVectors(double  aMach,
                                                                            double  aAlpha_rad,
                                                                            double& aCLArea,
                                                                            double& aCdArea,
                                                                            double& /* aCmArea */,
                                                                            double aRadiusSizeFactor)
{
   double CL = CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
   double Cd = Cd_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   // Adjust for size factor effects (size factor for parachutes, balloons, etc)
   // Size factor is radius based and we need area, so we square the factor.
   double areaMultiplier = aRadiusSizeFactor * aRadiusSizeFactor;

   aCLArea = CL * mRefArea_sqft * areaMultiplier;
   aCdArea = Cd * mRefArea_sqft * areaMultiplier;
}

double wsf::six_dof::PointMassAeroCoreObject::CalculateLiftAtSpecifiedAlpha_lbs(double aDynPress_lbsqft,
                                                                                double aMach,
                                                                                double aAlpha_deg,
                                                                                double aRadiusSizeFactor)
{
   // Adjust for size factor effects (size factor for parachutes, balloons, etc)
   // Size factor is radius based and we need area, so we square the factor.
   double areaMultiplier = aRadiusSizeFactor * aRadiusSizeFactor;

   double alpha_rad = aAlpha_deg * UtMath::cRAD_PER_DEG;
   double CL        = CL_AlphaBetaMach(aMach, alpha_rad, 0.0);
   double lift_lbs  = aDynPress_lbsqft * CL * mRefArea_sqft * areaMultiplier;

   return lift_lbs;
}
