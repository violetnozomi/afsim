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

#include "WsfRigidBodySixDOF_AeroCoreObject.hpp"

#include <cstdio>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"

wsf::six_dof::RigidBodyAeroCoreObject* wsf::six_dof::RigidBodyAeroCoreObject::Clone() const
{
   return new RigidBodyAeroCoreObject(*this);
}

bool wsf::six_dof::RigidBodyAeroCoreObject::ProcessInput(UtInput& aInput)
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

            auto tempModeObj = ut::make_unique<RigidBodyAeroCoreObject>();
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

bool wsf::six_dof::RigidBodyAeroCoreObject::ProcessCommonInput(UtInput&                 aInput,
                                                               const std::string&       aCommand,
                                                               RigidBodyAeroCoreObject* aObject)
{
   bool retVal = true;

   if (aCommand == "wing_chord_ft")
   {
      double value = 0.0;
      aInput.ReadValue(value);
      aObject->mWingChord_ft = value;
   }
   else if (aCommand == "wing_span_ft")
   {
      double value = 0.0;
      aInput.ReadValue(value);
      aObject->mWingSpan_ft = value;
   }
   else if (aCommand == "wing_area_sqft")
   {
      double value = 0.0;
      aInput.ReadValue(value);
      aObject->mWingArea_sqft = value;
   }
   else if (aCommand == "ref_area_sqft")
   {
      double value = 0.0;
      aInput.ReadValue(value);
      aObject->mRefArea_sqft = value;
      aObject->mRefLength_ft = std::sqrt(value);
      aObject->mUseRefArea   = true;
   }
   else if (aCommand == "aero_center_x")
   {
      double value_m = 0.0;
      aInput.ReadValueOfType(value_m, UtInput::cLENGTH);
      double value_ft = value_m * UtMath::cFT_PER_M;
      double x_ft     = 0.0;
      double y_ft     = 0.0;
      double z_ft     = 0.0;
      aObject->mAeroCenter_ft.Get(x_ft, y_ft, z_ft);
      x_ft = value_ft;
      aObject->mAeroCenter_ft.Set(x_ft, y_ft, z_ft);
   }
   else if (aCommand == "aero_center_y")
   {
      double value_m = 0.0;
      aInput.ReadValueOfType(value_m, UtInput::cLENGTH);
      double value_ft = value_m * UtMath::cFT_PER_M;
      double x_ft     = 0.0;
      double y_ft     = 0.0;
      double z_ft     = 0.0;
      aObject->mAeroCenter_ft.Get(x_ft, y_ft, z_ft);
      y_ft = value_ft;
      aObject->mAeroCenter_ft.Set(x_ft, y_ft, z_ft);
   }
   else if (aCommand == "aero_center_z")
   {
      double value_m = 0.0;
      aInput.ReadValueOfType(value_m, UtInput::cLENGTH);
      double value_ft = value_m * UtMath::cFT_PER_M;
      double x_ft     = 0.0;
      double y_ft     = 0.0;
      double z_ft     = 0.0;
      aObject->mAeroCenter_ft.Get(x_ft, y_ft, z_ft);
      z_ft = value_ft;
      aObject->mAeroCenter_ft.Set(x_ft, y_ft, z_ft);
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
   else if (aCommand == "cLq_alpha_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cLq_alpha_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

         aObject->mCLq_AlphaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cL_alphadot_alpha_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cL_alphadot_alpha_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

         aObject->mCL_AlphaDotAlphaMachTablePtr =
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
   else if (aCommand == "cyr_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cyr_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);

         aObject->mCYr_BetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cy_betadot_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cy_betadot_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);

         aObject->mCY_BetaDotBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cm_alpha_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cm_alpha_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 2);

         aObject->mCm_AlphaBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cmq_mach_table")
   {
      aObject->mCmq_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCmq_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Cmq",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "cmp_mach_table")
   {
      aObject->mCmp_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCmp_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Cmp",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "cm_alphadot_mach_table")
   {
      aObject->mCm_AlphaDotMachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCm_AlphaDotMachCurvePtr->ProcessInput(aInput,
                                                      UtInput::cNON_DIMENSIONAL,
                                                      "mach",
                                                      UtTable::ValueGE(0.0),
                                                      UtInput::cNON_DIMENSIONAL,
                                                      "cm_alphadot",
                                                      UtTable::NoCheck());
   }
   else if (aCommand == "cn_alpha_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cn_alpha_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 2);

         aObject->mCn_AlphaBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "cn_betadot_mach_table")
   {
      aObject->mCn_BetaDotMachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCn_BetaDotMachCurvePtr->ProcessInput(aInput,
                                                     UtInput::cNON_DIMENSIONAL,
                                                     "mach",
                                                     UtTable::ValueGE(0.0),
                                                     UtInput::cNON_DIMENSIONAL,
                                                     "Cn_betadot",
                                                     UtTable::NoCheck());
   }
   else if (aCommand == "cnr_mach_table")
   {
      aObject->mCnr_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCnr_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Cnr",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "cnp_mach_table")
   {
      aObject->mCnp_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCnp_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Cnp",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "cl_alpha_beta_mach_table")
   {
      UtInputBlock subBlock(aInput, "end_cl_alpha_beta_mach_table");
      do
      {
         UtTable::Parameters sParameters;
         sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
         sParameters.AddRealParameter("beta",
                                      UtInput::cANGLE,
                                      UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                      1);
         sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 2);

         aObject->mCl_AlphaBetaMachTablePtr =
            UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
      } while (subBlock.ReadCommand());
   }
   else if (aCommand == "clp_mach_table")
   {
      aObject->mClp_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mClp_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Clp",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "cl_alphadot_mach_table")
   {
      aObject->mCl_AlphaDotMachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCl_AlphaDotMachCurvePtr->ProcessInput(aInput,
                                                      UtInput::cNON_DIMENSIONAL,
                                                      "mach",
                                                      UtTable::ValueGE(0.0),
                                                      UtInput::cNON_DIMENSIONAL,
                                                      "CLq",
                                                      UtTable::NoCheck());
   }
   else if (aCommand == "cl_betadot_mach_table")
   {
      aObject->mCl_BetaDotMachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mCl_BetaDotMachCurvePtr->ProcessInput(aInput,
                                                     UtInput::cNON_DIMENSIONAL,
                                                     "mach",
                                                     UtTable::ValueGE(0.0),
                                                     UtInput::cNON_DIMENSIONAL,
                                                     "CLq",
                                                     UtTable::NoCheck());
   }
   else if (aCommand == "clr_mach_table")
   {
      aObject->mClr_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mClr_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Clr",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "clq_mach_table")
   {
      aObject->mClq_MachCurvePtr = ut::make_unique<UtTable::Curve>();
      aObject->mClq_MachCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "mach",
                                               UtTable::ValueGE(0.0),
                                               UtInput::cNON_DIMENSIONAL,
                                               "Clq",
                                               UtTable::NoCheck());
   }
   else if (aCommand == "use_reduced_frequency")
   {
      aObject->mUseReducedFrequency = aInput.ReadBool();
   }
   else
   {
      retVal = false;
   }
   return retVal;
}

bool wsf::six_dof::RigidBodyAeroCoreObject::Initialize()
{
   return true;
}

std::string wsf::six_dof::RigidBodyAeroCoreObject::GetModeName() const
{
   return mModeName;
}

void wsf::six_dof::RigidBodyAeroCoreObject::SetModeName(const std::string& aName)
{
   mModeName = aName;
}

const std::list<UtCloneablePtr<wsf::six_dof::RigidBodyAeroCoreObject>>*
wsf::six_dof::RigidBodyAeroCoreObject::GetSubModesList() const
{
   return &mSubModesList;
}

wsf::six_dof::RigidBodyAeroCoreObject* wsf::six_dof::RigidBodyAeroCoreObject::GetSubModeByName(const std::string& aName) const
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

double wsf::six_dof::RigidBodyAeroCoreObject::CLq_AlphaMach(double aMach, double aAlpha_rad)
{
   if (mCLq_AlphaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   return mCLq_AlphaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::CL_AlphaDotAlphaMach(double aMach, double aAlpha_rad)
{
   if (mCL_AlphaDotAlphaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   return mCL_AlphaDotAlphaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::CYr_BetaMach(double aMach, double aBeta_rad)
{
   if (mCYr_BetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   return mCYr_BetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::CY_BetaDotBetaMach(double aMach, double aBeta_rad)
{
   if (mCY_BetaDotBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   return mCY_BetaDotBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cm_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad)
{
   if (mCm_AlphaBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAlpha_rad;
   return mCm_AlphaBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cmq_Mach(double aMach)
{
   if (mCmq_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCmq_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cmp_Mach(double aMach)
{
   if (mCmp_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCmp_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::CmAlphaDotMach(double aMach)
{
   if (mCm_AlphaDotMachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCm_AlphaDotMachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cn_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad)
{
   if (mCn_AlphaBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAlpha_rad;
   return mCn_AlphaBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::CnBetaDotMach(double aMach)
{
   if (mCn_BetaDotMachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCn_BetaDotMachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cnr_Mach(double aMach)
{
   if (mCnr_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCnr_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cnp_Mach(double aMach)
{
   if (mCnp_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCnp_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cl_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad)
{
   if (mCl_AlphaBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAlpha_rad;
   return mCl_AlphaBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Clp_Mach(double aMach)
{
   if (mClp_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mClp_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cl_AlphaDotMach(double aMach)
{
   if (mCl_AlphaDotMachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCl_AlphaDotMachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Cl_BetaDotMach(double aMach)
{
   if (mCl_BetaDotMachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mCl_BetaDotMachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Clr_Mach(double aMach)
{
   if (mClr_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mClr_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::Clq_Mach(double aMach)
{
   if (mClq_MachCurvePtr == nullptr)
   {
      return 0.0;
   }

   return mClq_MachCurvePtr->Lookup(aMach);
}

double wsf::six_dof::RigidBodyAeroCoreObject::CalculateAeroCmArea(double aMach, double aAlpha_rad)
{
   double Cm = Cm_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   double pitch_moment = 0.0;

   if (mUseRefArea)
   {
      pitch_moment = Cm * mRefArea_sqft;
   }
   else
   {
      pitch_moment = Cm * mWingArea_sqft * mWingChord_ft;
   }

   return pitch_moment;
}

double wsf::six_dof::RigidBodyAeroCoreObject::CalculateAeroCLArea(double aMach, double aAlpha_rad)
{
   double CL = CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   double CL_Area = 0.0;

   if (mUseRefArea)
   {
      CL_Area = CL * mRefArea_sqft;
   }
   else
   {
      CL_Area = CL * mWingArea_sqft;
   }

   return CL_Area;
}

double wsf::six_dof::RigidBodyAeroCoreObject::CalculateAeroCdArea(double aMach, double aAlpha_rad)
{
   double Cd = Cd_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   double Cd_Area = 0.0;

   if (mUseRefArea)
   {
      Cd_Area = Cd * mRefArea_sqft;
   }
   else
   {
      Cd_Area = Cd * mWingArea_sqft;
   }

   return Cd_Area;
}

UtVec3dX wsf::six_dof::RigidBodyAeroCoreObject::GetAeroCenter_ft() const
{
   return mAeroCenter_ft;
}

double wsf::six_dof::RigidBodyAeroCoreObject::GetWingChord_ft() const
{
   return mWingChord_ft;
}

double wsf::six_dof::RigidBodyAeroCoreObject::GetWingSpan_ft() const
{
   return mWingSpan_ft;
}

double wsf::six_dof::RigidBodyAeroCoreObject::GetWingArea_sqft() const
{
   return mWingArea_sqft;
}

void wsf::six_dof::RigidBodyAeroCoreObject::CalculateCoreAeroFM(double          aDynPress_lbsqft,
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
                                                                double          aRadiusSizeFactor)
{
   // Get individual angular rates
   double yawRate_rps   = 0.0;
   double pitchRate_rps = 0.0;
   double rollRate_rps  = 0.0;
   aAngularRates_rps.Get(rollRate_rps, pitchRate_rps, yawRate_rps);

   double speedSafe_fps = std::max(aSpeed_fps, 1.0);

   double kq = pitchRate_rps / (2 * speedSafe_fps);
   double kr = yawRate_rps / (2 * speedSafe_fps);
   double kp = rollRate_rps / (2 * speedSafe_fps);
   double ka = aAlphaDot_rps / (2 * speedSafe_fps);
   double kb = aBetaDot_rps / (2 * speedSafe_fps);

   // Calculate the reduced frequencies
   double kLq = kq;
   double kLa = ka;
   double kYr = kr;
   double kYb = kb;

   if (mUseRefArea)
   {
      kLq *= mRefLength_ft;
      kLa *= mRefLength_ft;
      kYr *= mRefLength_ft;
      kYb *= mRefLength_ft;
   }
   else
   {
      kLq *= mWingChord_ft;
      kLa *= mWingChord_ft;
      kYr *= mWingSpan_ft;
      kYb *= mWingSpan_ft;
   }

   if (!mUseReducedFrequency)
   {
      kLq = pitchRate_rps;
      kLa = aAlphaDot_rps;
      kYr = yawRate_rps;
      kYb = aBetaDot_rps;
   }

   double CL          = CL_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);
   double CLq         = CLq_AlphaMach(aMach, aAlpha_rad) * kLq;
   double CL_alphadot = CL_AlphaDotAlphaMach(aMach, aAlpha_rad) * kLa;
   double Cd          = Cd_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);
   double CY          = CY_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);
   double CYr         = CYr_BetaMach(aMach, aBeta_rad) * kYr;
   double CY_betadot  = CY_BetaDotBetaMach(aMach, aBeta_rad) * kYb;

   // Adjust for size factor effects (size factor for parachutes, balloons, etc)
   // Size factor is radius based and we need area, so we square the factor.
   double areaMultiplier = aRadiusSizeFactor * aRadiusSizeFactor;

   if (mUseRefArea)
   {
      aLift_lbs      = aDynPress_lbsqft * (CL + CLq + CL_alphadot) * mRefArea_sqft * areaMultiplier;
      aDrag_lbs      = aDynPress_lbsqft * Cd * mRefArea_sqft * areaMultiplier;
      aSideForce_lbs = aDynPress_lbsqft * (CY + CYr + CY_betadot) * mRefArea_sqft * areaMultiplier;
   }
   else
   {
      aLift_lbs      = aDynPress_lbsqft * (CL + CLq + CL_alphadot) * mWingArea_sqft * areaMultiplier;
      aDrag_lbs      = aDynPress_lbsqft * Cd * mWingArea_sqft * areaMultiplier;
      aSideForce_lbs = aDynPress_lbsqft * (CY + CYr + CY_betadot) * mWingArea_sqft * areaMultiplier;
   }

   // Pitching moments
   double Cm          = 0.0;
   double CmQ         = 0.0;
   double CmP         = 0.0;
   double Cm_alphadot = 0.0;

   // Yawing moments
   double Cn         = 0.0;
   double CnR        = 0.0;
   double CnP        = 0.0;
   double Cn_betadot = 0.0;

   // Rolling moments
   double Cl          = 0.0;
   double ClP         = 0.0;
   double Cl_alphadot = 0.0;
   double Cl_betadot  = 0.0;
   double ClR         = 0.0;
   double ClQ         = 0.0;

   // Calculate the longitudinal, lateral, and directional reduced frequencies
   double kmq = kq;
   double kma = ka;
   double kmp = kp;

   double klq = kq;
   double kla = ka;
   double klr = kr;
   double klb = kb;
   double klp = kp;

   double knr = kr;
   double knb = kb;
   double knp = kp;

   if (mUseRefArea)
   {
      kmq *= mRefLength_ft;
      kma *= mRefLength_ft;
      kmp *= mRefLength_ft;

      klq *= mRefLength_ft;
      kla *= mRefLength_ft;
      klr *= mRefLength_ft;
      klb *= mRefLength_ft;
      klp *= mRefLength_ft;

      knr *= mRefLength_ft;
      knb *= mRefLength_ft;
      knp *= mRefLength_ft;
   }
   else
   {
      kmq *= mWingChord_ft;
      kma *= mWingChord_ft;
      kmp *= mWingChord_ft;

      klq *= mWingSpan_ft;
      kla *= mWingSpan_ft;
      klr *= mWingSpan_ft;
      klb *= mWingSpan_ft;
      klp *= mWingSpan_ft;

      knr *= mWingSpan_ft;
      knb *= mWingSpan_ft;
      knp *= mWingSpan_ft;
   }

   if (!mUseReducedFrequency)
   {
      kmq = pitchRate_rps;
      kma = aAlphaDot_rps;
      kmp = rollRate_rps;

      klq = pitchRate_rps;
      kla = aAlphaDot_rps;
      klr = yawRate_rps;
      klb = aBetaDot_rps;
      klp = rollRate_rps;

      knr = yawRate_rps;
      knb = aBetaDot_rps;
      knp = rollRate_rps;
   }

   // Note that derivatives use radians/sec for angular rates

   Cm          = Cm_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);
   CmQ         = Cmq_Mach(aMach) * kmq;
   CmP         = Cmp_Mach(aMach) * kmp;
   Cm_alphadot = CmAlphaDotMach(aMach) * kma;

   Cn         = Cn_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);
   CnR        = Cnr_Mach(aMach) * knr;
   CnP        = Cnp_Mach(aMach) * knp;
   Cn_betadot = CnBetaDotMach(aMach) * knb;

   Cl          = Cl_AlphaBetaMach(aMach, aAlpha_rad, aBeta_rad);
   ClP         = Clp_Mach(aMach) * klp;
   Cl_alphadot = Cl_AlphaDotMach(aMach) * kla;
   Cl_betadot  = Cl_BetaDotMach(aMach) * klb;
   ClR         = Clr_Mach(aMach) * klr;
   ClQ         = Clq_Mach(aMach) * klq;

   double yaw_moment   = 0.0;
   double pitch_moment = 0.0;
   double roll_moment  = 0.0;

   if (mUseRefArea)
   {
      yaw_moment   = aDynPress_lbsqft * (Cn + CnR + CnP + Cn_betadot) * mRefArea_sqft;
      pitch_moment = aDynPress_lbsqft * (Cm + CmQ + CmP + Cm_alphadot) * mRefArea_sqft;
      roll_moment  = aDynPress_lbsqft * (Cl + ClP + ClR + ClQ + Cl_alphadot + Cl_betadot) * mRefArea_sqft;
   }
   else
   {
      yaw_moment   = aDynPress_lbsqft * (Cn + CnR + CnP + Cn_betadot) * mWingArea_sqft * mWingSpan_ft;
      pitch_moment = aDynPress_lbsqft * (Cm + CmQ + CmP + Cm_alphadot) * mWingArea_sqft * mWingChord_ft;
      roll_moment = aDynPress_lbsqft * (Cl + ClP + ClR + ClQ + Cl_alphadot + Cl_betadot) * mWingArea_sqft * mWingSpan_ft;
   }

   aMoment_ftlbs.Set(roll_moment, pitch_moment, yaw_moment);
}

void wsf::six_dof::RigidBodyAeroCoreObject::CalculateAeroCoefficientVectors(double  aMach,
                                                                            double  aAlpha_rad,
                                                                            double& aCLArea,
                                                                            double& aCdArea,
                                                                            double& aCmArea,
                                                                            double  aRadiusSizeFactor)
{
   double CL = CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
   double Cd = Cd_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
   double Cm = Cm_AlphaBetaMach(aMach, aAlpha_rad, 0.0);

   // Adjust for size factor effects (size factor for parachutes, balloons, etc)
   // Size factor is radius based and we need area, so we square the factor.
   double areaMultiplier = aRadiusSizeFactor * aRadiusSizeFactor;

   if (mUseRefArea)
   {
      aCLArea = CL * mRefArea_sqft * areaMultiplier;
      aCdArea = Cd * mRefArea_sqft * areaMultiplier;
      aCmArea = Cm * mRefArea_sqft;
   }
   else
   {
      aCLArea = CL * mWingArea_sqft * areaMultiplier;
      aCdArea = Cd * mWingArea_sqft * areaMultiplier;
      aCmArea = Cm * mWingArea_sqft * mWingChord_ft;
   }
}

double wsf::six_dof::RigidBodyAeroCoreObject::CalculateLiftAtSpecifiedAlpha_lbs(double aDynPress_lbsqft,
                                                                                double aMach,
                                                                                double aAlpha_deg,
                                                                                double aRadiusSizeFactor)
{
   double alpha_rad = aAlpha_deg * UtMath::cRAD_PER_DEG;
   double lift_lbs  = 0;
   double CL        = CL_AlphaBetaMach(aMach, alpha_rad, 0.0);

   // Adjust for size factor effects (size factor for parachutes, balloons, etc)
   // Size factor is radius based and we need area, so we square the factor.
   double areaMultiplier = aRadiusSizeFactor * aRadiusSizeFactor;

   if (mUseRefArea)
   {
      lift_lbs = aDynPress_lbsqft * CL * mRefArea_sqft * areaMultiplier;
   }
   else
   {
      lift_lbs = aDynPress_lbsqft * CL * mWingArea_sqft * areaMultiplier;
   }

   return lift_lbs;
}
