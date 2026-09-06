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

#include "VehicleAero.hpp"

#include <exception>
#include <locale>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProgressDialog>
#include <QString>
#include <QTextStream>
#include <stdio.h>

#include "AeroBody.hpp"
#include "AeroDish.hpp"
#include "AeroFullFlyingSurface.hpp"
#include "AeroHingedSurface.hpp"
#include "AeroLandingGear.hpp"
#include "AeroNacelle.hpp"
#include "AeroSpeedBrake.hpp"
#include "AeroSurface.hpp"
#include "AeroWing.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "P6DofUtils.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "Vehicle.hpp"

namespace Designer
{

namespace
{
   struct Coeffs
   {
      double CL;
      double Cd;
      double CY;
      double Cm;
      double Cn;
      double Cl;

      Coeffs(double aCL, double aCd, double aCY, double aCm, double aCn, double aCl)
         : CL(aCL)
         , Cd(aCd)
         , CY(aCY)
         , Cm(aCm)
         , Cn(aCn)
         , Cl(aCl) {}
   };
}

VehicleAero::VehicleAero(Vehicle* aParentVehicle, UtAtmosphere aAtmosphere)
   : mParentVehicle(aParentVehicle)
   , mAtmosphere(aAtmosphere)
{
}

void VehicleAero::ShowProgress(QProgressDialog& aProgressDialog)
{
   double fraction = static_cast<double>(mCurrentNumberOfAeroTests) /
                     static_cast<double>(mTotalNumberOfAeroTests);
   int iProgressValue = static_cast<int>(10000.0 * fraction);
   aProgressDialog.setValue(iProgressValue);

   // Need to process event for this to be seen right away
   QCoreApplication::processEvents();
}

void VehicleAero::CalculateP6DOFVehicleCoreAerodynamics(QProgressDialog& aProgressDialog)
{
   // Ensure all core aero data is cleared
   mCoreAero.ClearAllData();

   CalcP6DOFCoreAeroDataUsingAlphaAndBetaSweep(aProgressDialog);

   CalcP6DOFCoreAeroDataUsingPitchRateSweep(aProgressDialog);
   CalcP6DOFCoreAeroDataUsingAlphaAndPitchRateSweep(aProgressDialog);
   CalcP6DOFCoreAeroDataUsingYawRateSweep(aProgressDialog);
   CalcP6DOFCoreAeroDataUsingBetaAndYawRateSweep(aProgressDialog);
   CalcP6DOFCoreAeroDataUsingRollRateSweep(aProgressDialog);
}

void VehicleAero::CalculatePM6DOFVehicleCoreAerodynamics(QProgressDialog& aProgressDialog)
{
    CalcPM6DOFCoreAeroDataUsingAlphaAndBetaSweep(aProgressDialog);

    // TODO: Create any other necessary table data
}

void VehicleAero::CalculateGMVehicleCoreAerodynamics(QProgressDialog& aProgressDialog)
{
   CalcGMCoreAeroData(aProgressDialog);

   CalcVehicleAspectRatio(aProgressDialog);
}

void VehicleAero::CalculateP6DOFVehicleAllMovableAerodynamics(QProgressDialog& aProgressDialog)
{
   aProgressDialog.setLabelText(QString("Calculating movable aerodynamics ..."));
   ShowProgress(aProgressDialog);

   // Ensure all movable surface aero data is cleared
   for (auto& item : mMovableAeroList)
   {
      item.ClearAllData();
   }

   // Loop through movable parts
   for (auto& element : mMovableAeroList)
   {
      VehicleAeroMovable& aero = element;

      CalculateP6DOFComponentMovableAerodynamics(aero, aProgressDialog);
   }
}

void VehicleAero::CalculateP6DOFComponentMovableAerodynamics(VehicleAeroMovable& aAero,
                                                        QProgressDialog&    aProgressDialog)
{
   AeroObject* aeroObj = aAero.GetAeroObject();
   if (aeroObj == nullptr)
   {
      return;
   }

   std::string labelString = "Calculating aerodynamics for\n" + aAero.GetName();
   aProgressDialog.setLabelText(QString(labelString.c_str()));
   ShowProgress(aProgressDialog);

   std::vector<double> angleDataPts_deg;

      bool dataValid = CalculateVectorOfAngles(aAero, angleDataPts_deg);
      if (!dataValid)
      {
         return;
      }
   
   CalcP6DOFMovableAeroDataUsingAngleAndAlphaSweep(aAero, angleDataPts_deg, aProgressDialog);
   CalcP6DOFMovableAeroDataUsingAngleAndBetaSweep(aAero, angleDataPts_deg, aProgressDialog);
   CalcP6DOFMovableAeroDataUsingAngleSweep(aAero, angleDataPts_deg, aProgressDialog);

   // Note that this one uses the testing Mach
   CalcP6DOFMovableAeroDataUsingAngleAndAlphaBetaSweep(aAero, angleDataPts_deg, mVehicleTestMach, aProgressDialog);

   ShowProgress(aProgressDialog);
}

void VehicleAero::GenerateMovableAeroList()
{
   for (auto& aeroObjElement : mAeroObjMap)
   {
      AeroObject* aeroObj = aeroObjElement.second;

      if (aeroObj->IsMovable())
      {
         AddMovableAero(aeroObj);
      }
   }
}

void VehicleAero::SetAeroMultipliers()
{
   if (mParentVehicle->IsAircraft())
   {
      mCnBetadotMult  = 1.0;
      mCnpMult        = 1.0;
      mCnrMult        = 1.0; // Yaw damping 5x
      mCmAlphadotMult = 1.0;
      mCmpMult        = 1.0;
      mCmqMult        = 1.0; // Pitch damping 5x
      mClpMult        = 1.0; // Roll damping 5x
      mClrMult        = 1.0;
      mClBetadotMult  = 1.0;
      mClqMult        = 1.0;
      mClAlphadotMult = 1.0;
      mCnMovableMult  = 1.0;
      mCmMovableMult  = 1.0;
      mClMovableMult  = 1.0; // Roll command 5x

      mDerivativeMultipliersSet = true;
   }
   else if (mParentVehicle->IsWeapon())
   {
      mCnBetadotMult  = 1.0;
      mCnpMult        = 1.0;
      mCnrMult        = 1.0; // Yaw damping 5x
      mCmAlphadotMult = 1.0;
      mCmpMult        = 1.0;
      mCmqMult        = 1.0; // Pitch damping 5x
      mClpMult        = 1.0; // Roll damping 5x
      mClrMult        = 1.0;
      mClBetadotMult  = 1.0;
      mClqMult        = 1.0;
      mClAlphadotMult = 1.0;
      mCnMovableMult  = 1.0;
      mCmMovableMult  = 1.0;
      mClMovableMult  = 1.0;

      mDerivativeMultipliersSet = true;
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The vehicle needs to be either an aircraft or a weapon!");
   }
}

void VehicleAero::Populate2D_Table(AeroTable2d&        aTable,
                                   double              aReferenceMach,
                                   double              aReferenceValue,
                                   std::vector<double> aOutputMachs)
{
   // This function generates aerodynamic data at a range of Mach numbers.
   // Here, we assume that a property observed or assumed at aReferenceValue
   // at a Mach number of aReferenceMach will vary according to a
   // Prandtl-Glauert relation at an arbitrary Mach number Y. This neglects
   // effects like center-of-pressure shifts, but provides values that should
   // be a reasonable baseline for users.

   // Populate 2D table
   aTable.Clear();

   // Use BrawlerMover's atmosphere to retrieve speed of sound

   // This uses The Prandtl-Glauert correction, clipped
   // in the transonic range of Mach 0.9-1.5
   auto forceFactor = [](double aMach)
   {
      auto beta = [](double aMach)
      {
         return sqrt(fabs(1.0 - aMach * aMach));
      };

      if (aMach > 0.9 && aMach < 1.5)
      {
         return 1.0 / UtMath::Lerp(aMach, 0.9, 1.5, beta(0.9), beta(1.5));
      }

      return 1.0 / beta(aMach);
   };

   for (double mach : aOutputMachs)
   {
      double correctedValue = aReferenceValue * forceFactor(mach) / forceFactor(aReferenceMach);

      aTable.AddData(mach, correctedValue);
   }
}

QStringList VehicleAero::Audit()
{
   if (!mDerivativeMultipliersSet)
   {
      MoverCreatorMainWindow().CreateErrorBox("Aero multipliers are not set!");
   }

   QStringList errors;
   for (auto& aeroObj : mAeroObjMap)
   {
      errors.append(aeroObj.second->Audit());
   }

   return errors;
}

bool VehicleAero::SetVehicleAeroReferenceArea()
{
   if (mParentVehicle->IsAircraft())
   {
      GeometryWing* wing = mParentVehicle->GetGeometry().GetWing();
      if (wing != nullptr)
      {
         mVehicleWingArea_ft2         = wing->GetPlanformArea_ft2();
         mVehicleWingSpan_ft          = wing->GetSpan_ft();
         mVehicleWingChord_ft         = 0.5 * (wing->GetRootChord_ft() + wing->GetTipChord_ft());
         mVehicleRefArea_ft2          = 0.0;
         mVehicleRefLength_ft         = 0.0;
         mUseRefAreaInsteadOfWingArea = false;
         return true;
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Aircraft lacks a wing!");
         return false;
      }
   }
   else if (mParentVehicle->IsWeapon())
   {
      // First, check if the weapon has a wing -- if so, use wing area reference
      GeometryWing* wing = mParentVehicle->GetGeometry().GetWing();
      if (wing != nullptr)
      {
         mVehicleWingArea_ft2         = wing->GetPlanformArea_ft2();
         mVehicleWingSpan_ft          = wing->GetSpan_ft();
         mVehicleWingChord_ft         = 0.5 * (wing->GetRootChord_ft() + wing->GetTipChord_ft());
         mVehicleRefArea_ft2          = 0.0;
         mVehicleRefLength_ft         = 0.0;
         mUseRefAreaInsteadOfWingArea = false;
         return true;
      }

      // The weapon lacks a wing (pretty typical), so check if the weapon has a body
      GeometryBody* body = mParentVehicle->GetGeometry().GetBody();
      if (body != nullptr)
      {
         mVehicleWingArea_ft2         = 0.0;
         mVehicleWingSpan_ft          = 0.0;
         mVehicleWingChord_ft         = 0.0;
         double averageRadius_ft      = 0.5 * ((0.5 * body->GetWidth_ft()) + (0.5 * body->GetHeight_ft()));
         mVehicleRefArea_ft2          = UtMath::cPI * (averageRadius_ft * averageRadius_ft);
         mVehicleRefLength_ft         = std::sqrt(mVehicleRefArea_ft2);
         mUseRefAreaInsteadOfWingArea = true;
         return true;
      }

      MoverCreatorMainWindow().CreateErrorBox("Weapon has neither a body nor a wing!");
      return false;
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return false;
   }

   return false;
}
/*
void VehicleAero::SimpleAeroTest(double aMach, double aAlpha_deg, double aBeta_deg)
{
   double mach = aMach;
   double ktas = mAtmosphere->CalcKtasFromMach(mVehicleAltitude_ft, mach);
   double kcas = mAtmosphere->CalcKcasFromMach(mVehicleAltitude_ft, mach);
   double speed_fps = mAtmosphere->CalcFpsFromMach(mVehicleAltitude_ft, mach);
   double dynamicPressure_psf = mAtmosphere->CalcDynamicPressure_lbft2(mVehicleAltitude_ft, speed_fps);
   double density_slugs_ft3 = mAtmosphere->AirDensityAtAltitude(mVehicleAltitude_ft);
   double alpha_deg = aAlpha_deg;
   double beta_deg = aBeta_deg;
   double vehicleYawRate_dps = 0.0;
   double vehiclePitchRate_dps = 0.0;
   double vehicleRollRate_dps = 0.0;
   double totalLift_lbs = 0.0;
   double totalDrag_lbs = 0.0;
   double totalSide_lbs = 0.0;
   UtVec3dX totalMomentVec_ftlbs;
   double CL = 0.0;
   double Cd = 0.0;
   double CY = 0.0;
   double Cl = 0.0;
   double Cm = 0.0;
   double Cn = 0.0;

   CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas, dynamicPressure_psf, density_slugs_ft3,
                        alpha_deg, beta_deg, vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                        totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs, CL, Cd, CY, Cl, Cm, Cn);

   printf("SimpleAeroTest -- Mach = %5.2lf Alpha = %7.2lf deg Beta = %7.2lf deg    CL = %7.3lf CY = %7.3lf Cm = %7.3lf Cn = %7.3lf\n\n",
          mach, alpha_deg, beta_deg, CL, CY, Cm, Cn);
}
*/

// This will enable/disable the appropriate tables corresponding to the mover type
void VehicleAero::EnableDisableTables(bool aGenerateP6DOFMover, bool aGenerateGuidedMover, bool aGeneratePointMassMover)
{
   EnableDisableP6DOFTables(aGenerateP6DOFMover);
   EnableDisablePointMassTables(aGeneratePointMassMover);
   EnableDisableGMTables(aGenerateGuidedMover);
}


void VehicleAero::EnableDisableP6DOFTables(bool aEnableP6DOF)
{
   if (aEnableP6DOF)
   {
      mCoreAero.EnableCL_AlphaBetaMachTable();
      mCoreAero.EnableCLq_AlphaMachTable();
      mCoreAero.EnableCL_AlphaDotAlphaMachTable();
      mCoreAero.EnableCd_AlphaBetaMachTable();
      mCoreAero.EnableCY_AlphaBetaMachTable();
      mCoreAero.EnableCYr_BetaMachTable();
      mCoreAero.EnableCY_BetaDotBetaMachTable();
      mCoreAero.EnableCm_AlphaBetaMachTable();
      mCoreAero.EnableCmq_MachTable();
      mCoreAero.EnableCmp_MachTable();
      mCoreAero.EnableCm_AlphaDotMachTable();
      mCoreAero.EnableCn_AlphaBetaMachTable();
      mCoreAero.EnableCn_BetaDotMachTable();
      mCoreAero.EnableCnr_MachTable();
      mCoreAero.EnableCnp_MachTable();
      mCoreAero.EnableCl_AlphaBetaMachTable();
      mCoreAero.EnableClp_MachTable();
      mCoreAero.EnableCl_AlphaDotMachTable();
      mCoreAero.EnableClr_MachTable();
      mCoreAero.EnableCl_BetaDotMachTable();
      mCoreAero.EnableClq_MachTable();

      for (auto& movableAero : mMovableAeroList)
      {
         movableAero.EnableCL_AngleAlphaMachTablePtr();
         movableAero.EnableCd_AngleAlphaMachTablePtr();
         movableAero.EnableCd_AngleBetaMachTablePtr();
         movableAero.EnableCd_AngleMachTablePtr();
         movableAero.EnableCY_AngleBetaMachTablePtr();
         movableAero.EnableCm_AngleAlphaMachTablePtr();
         movableAero.EnableCmq_AngleMachTablePtr();
         movableAero.EnableCn_AngleBetaMachTablePtr();
         movableAero.EnableCnr_AngleMachTablePtr();
         movableAero.EnableCl_AngleAlphaBetaTablePtr();
         movableAero.EnableClp_AngleMachTablePtr();
         movableAero.EnableClq_AngleMachTablePtr();
         movableAero.EnableClr_AngleMachTablePtr();
      }
   }
   else
   {
      mCoreAero.DisableCL_AlphaBetaMachTable();
      mCoreAero.DisableCLq_AlphaMachTable();
      mCoreAero.DisableCL_AlphaDotAlphaMachTable();
      mCoreAero.DisableCd_AlphaBetaMachTable();
      mCoreAero.DisableCY_AlphaBetaMachTable();
      mCoreAero.DisableCYr_BetaMachTable();
      mCoreAero.DisableCY_BetaDotBetaMachTable();
      mCoreAero.DisableCm_AlphaBetaMachTable();
      mCoreAero.DisableCmq_MachTable();
      mCoreAero.DisableCmp_MachTable();
      mCoreAero.DisableCm_AlphaDotMachTable();
      mCoreAero.DisableCn_AlphaBetaMachTable();
      mCoreAero.DisableCn_BetaDotMachTable();
      mCoreAero.DisableCnr_MachTable();
      mCoreAero.DisableCnp_MachTable();
      mCoreAero.DisableCl_AlphaBetaMachTable();
      mCoreAero.DisableClp_MachTable();
      mCoreAero.DisableCl_AlphaDotMachTable();
      mCoreAero.DisableClr_MachTable();
      mCoreAero.DisableCl_BetaDotMachTable();
      mCoreAero.DisableClq_MachTable();

      for (auto& movableAero : mMovableAeroList)
      {
         movableAero.DisableCL_AngleAlphaMachTablePtr();
         movableAero.DisableCd_AngleAlphaMachTablePtr();
         movableAero.DisableCd_AngleBetaMachTablePtr();
         movableAero.DisableCd_AngleMachTablePtr();
         movableAero.DisableCY_AngleBetaMachTablePtr();
         movableAero.DisableCm_AngleAlphaMachTablePtr();
         movableAero.DisableCmq_AngleMachTablePtr();
         movableAero.DisableCn_AngleBetaMachTablePtr();
         movableAero.DisableCnr_AngleMachTablePtr();
         movableAero.DisableCl_AngleAlphaBetaTablePtr();
         movableAero.DisableClp_AngleMachTablePtr();
         movableAero.DisableClq_AngleMachTablePtr();
         movableAero.DisableClr_AngleMachTablePtr();
      }
   }
}

void VehicleAero::EnableDisablePointMassTables(bool aEnablePM6DOF)
{
    if (aEnablePM6DOF)
    {
        mCoreAero.EnableTrimCL_AlphaBetaMachTable();
        mCoreAero.EnableTrimCd_AlphaBetaMachTable();
        mCoreAero.EnableTrimCY_AlphaBetaMachTable();
        mCoreAero.EnableRollAccelCapability_MachTable();
        mCoreAero.EnablePitchAccelCapability_MachTable();
        mCoreAero.EnableYawAccelCapability_MachTable();
        mCoreAero.EnableRollStabilityFrequency_MachTable();
        mCoreAero.EnablePitchStabilityFrequency_MachTable();
        mCoreAero.EnableYawStabilityFrequency_MachTable();

        if (GetParentVehicle()->GetVehicleType() == "Aircraft")
        {
           mCoreAero.EnableSpeedbrakeDeltaCD_MachTable();
           mCoreAero.EnableFlapsDeltaCL_MachTable();
           mCoreAero.EnableFlapsDeltaCD_MachTable();
           mCoreAero.EnableSpoilersDeltaCL_MachTable();
           mCoreAero.EnableSpoilersDeltaCD_MachTable();
        }
    }
    else
    {
        mCoreAero.DisableTrimCL_AlphaBetaMachTable();
        mCoreAero.DisableTrimCd_AlphaBetaMachTable();
        mCoreAero.DisableTrimCY_AlphaBetaMachTable();
        mCoreAero.DisableRollAccelCapability_MachTable();
        mCoreAero.DisablePitchAccelCapability_MachTable();
        mCoreAero.DisableYawAccelCapability_MachTable();
        mCoreAero.DisableRollStabilityFrequency_MachTable();
        mCoreAero.DisablePitchStabilityFrequency_MachTable();
        mCoreAero.DisableYawStabilityFrequency_MachTable();
        mCoreAero.DisableSpeedbrakeDeltaCD_MachTable();
        mCoreAero.DisableFlapsDeltaCL_MachTable();
        mCoreAero.DisableFlapsDeltaCD_MachTable();
        mCoreAero.DisableSpoilersDeltaCL_MachTable();
        mCoreAero.DisableSpoilersDeltaCD_MachTable();
    }
}

void VehicleAero::EnableDisableGMTables(bool aEnableGM)
{
   if (aEnableGM)
   {
      mCoreAero.EnableCd_MachTable();
   }
   else
   {
      mCoreAero.DisableCd_MachTable();
   }
}


void VehicleAero::CalculateFullVehicleAerodynamics()
{
   if (!SetVehicleAeroReferenceArea())
   {
      MoverCreatorMainWindow().CreateErrorBox("Unable to set the reference area for the specified vehicle!");
      return;
   }

   if (!mDerivativeMultipliersSet)
   {
      MoverCreatorMainWindow().CreateErrorBox("Aero multipliers are not set!");
      return;
   }

   // Setup a progress dialog to show aero calc progress
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   GeometryWidget* geometryWidget = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   QProgressDialog progressDialog(QString("Setup aerodynamics..."), QString(), 0, 10000, geometryWidget);
   progressDialog.setWindowModality(Qt::WindowModal);
   progressDialog.setWindowTitle(QString("Calculating Aerodynamics"));
   progressDialog.setMinimumDuration(0);
   progressDialog.setFixedSize(QSize(400, 100));
   progressDialog.setValue(0);

   mCurrentNumberOfAeroTests = 0;

   mTotalNumberOfAeroTests = CalcNumberOfAeroTests();

   progressDialog.setLabelText(QString("Calculating core aerodynamics ..."));
   QCoreApplication::processEvents();

   // Ensure all core aero data is cleared
   mCoreAero.ClearAllData();
   for (auto& movableAero : mMovableAeroList)
   {
      movableAero.ClearAllData();
   }

   if (MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GenerateP6DOFMover() ||
       MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GenerateRB6DOFMover() ||
       MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GeneratePM6DOFMover())
   {
      // RB6DOF needs this data, and PM6DOF derives reduced-order data in turn
      CalculateP6DOFVehicleCoreAerodynamics(progressDialog);
      CalculateP6DOFVehicleAllMovableAerodynamics(progressDialog);
   }

   if (MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GeneratePM6DOFMover())
   {
       CalculatePM6DOFVehicleCoreAerodynamics(progressDialog);
   }

   if (MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GenerateGuidedMover())
   {
      CalculateGMVehicleCoreAerodynamics(progressDialog);
   }

   ApplyAllModifiers();

   progressDialog.setValue(10000);
}

void VehicleAero::CalcAeroData(double    aAltitude_ft,
                               double    aSpeed_fps,
                               double    aMach,
                               double    aKTAS,
                               double    aKCAS,
                               double    aDynamicPressure_psf,
                               double    aDensity_slugs_ft3,
                               double    aVehicleAlpha_deg,
                               double    aVehicleBeta_deg,
                               double    aVehicleYawRate_dps,
                               double    aVehiclePitchRate_dps,
                               double    aVehicleRollRate_dps,
                               double&   aTotalLift_lbs,
                               double&   aTotalDrag_lbs,
                               double&   aTotalSide_lbs,
                               UtVec3dX& aTotalMomentVector_ftlbs)
{
   // Zero the total F&M data
   aTotalLift_lbs = 0.0;
   aTotalDrag_lbs = 0.0;
   aTotalSide_lbs = 0.0;
   aTotalMomentVector_ftlbs.Set(0.0, 0.0, 0.0);

   // Set the vehicle alpha/beta
   mParentVehicle->SetVehicleAlphaBeta(aVehicleAlpha_deg, aVehicleBeta_deg);

   // Set the vehicle rates
   mParentVehicle->SetVehicleYawPitchRollRates(aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps);

   // Set vehicle speed-related data
   mParentVehicle->SetVehicleSpeedParameters(aSpeed_fps, aMach, aKTAS, aKCAS, aDynamicPressure_psf, aDensity_slugs_ft3);

   // Update the aero vectors
   mParentVehicle->CalculateAeroAttitudeVectors();

   // This will loop through all subobjects and calculate the total F&M
   CalcForcesAndMomentsOnSubobjects(aAltitude_ft,
                                    aSpeed_fps,
                                    aMach,
                                    aKTAS,
                                    aKCAS,
                                    aDynamicPressure_psf,
                                    aDensity_slugs_ft3,
                                    aVehicleAlpha_deg,
                                    aVehicleBeta_deg,
                                    aVehicleYawRate_dps,
                                    aVehiclePitchRate_dps,
                                    aVehicleRollRate_dps,
                                    aTotalLift_lbs,
                                    aTotalDrag_lbs,
                                    aTotalSide_lbs,
                                    aTotalMomentVector_ftlbs);
}

void VehicleAero::SetVehicleAlphaBeta(double aAlpha_deg, double aBeta_deg)
{
   mVehicleAlpha_deg = aAlpha_deg;
   mVehicleBeta_deg  = aBeta_deg;

   // Loop through aero objects and set the vehicle's alpha/beta
   for (auto& aeroObj : mAeroObjMap)
   {
      aeroObj.second->SetVehicleAlphaBeta(aAlpha_deg, aBeta_deg);
   }
}

void VehicleAero::SetVehicleYawPitchRollRates(double aYawRate_dps, double aPitchRate_dps, double aRollRate_dps)
{
   mVehicleYawRate_dps   = aYawRate_dps;
   mVehiclePitchRate_dps = aPitchRate_dps;
   mVehicleRollRate_dps  = aRollRate_dps;

   // Loop through aero objects and set the vehicle's rates
   for (auto& aeroObj : mAeroObjMap)
   {
      aeroObj.second->SetVehicleYawPitchRollRates(aYawRate_dps, aPitchRate_dps, aRollRate_dps);
   }
}

void VehicleAero::SetVehicleSpeedParameters(double aSpeed_fps,
                                            double aMach,
                                            double aKTAS,
                                            double aKCAS,
                                            double aDynamicPressure_psf,
                                            double aAirDensity_slugs_ft3)
{
   mVehicleSpeed_fps           = aSpeed_fps;
   mVehicleMach                = aMach;
   mVehicleKTAS                = aKTAS;
   mVehicleKCAS                = aKCAS;
   mVehicleDynamicPressure_psf = aDynamicPressure_psf;
   mAirDensity_slugs_ft3       = aAirDensity_slugs_ft3;

   // Loop through aero objects and set the vehicle's speed
   for (auto& aeroObj : mAeroObjMap)
   {
      aeroObj.second->SetVehicleSpeedParameters(aSpeed_fps, aMach, aKTAS, aKCAS,
                                                aDynamicPressure_psf, aAirDensity_slugs_ft3);
   }
}

void VehicleAero::SetTestingMach(double aMach)
{
   if (aMach > 0)
   {
      mVehicleTestMach = aMach;
   }
}

void VehicleAero::SetAeroDataMaxMach(double aMach)
{
   if (aMach > 0)
   {
      mMaxTableMach_HundredthsMach = static_cast<int>(aMach * 100.0);
   }
}

void VehicleAero::SetAeroDataMaxBeta(double aBeta_deg)
{
   if (aBeta_deg > 0)
   {
      mMinTableBetaDataPts_deg = static_cast<int>(-aBeta_deg);
      mMaxTableBetaDataPts_deg = static_cast<int>(aBeta_deg);
   }
}

void VehicleAero::SetAeroDataAlphas(double aAlphaMin_deg, double aAlphaMax_deg)
{
   if (aAlphaMin_deg < aAlphaMax_deg)
   {
      mMinTableAlphaDataPts_deg = static_cast<int>(aAlphaMin_deg);
      mMaxTableAlphaDataPts_deg = static_cast<int>(aAlphaMax_deg);
   }
}

void VehicleAero::CalculateAeroAttitudeVectors()
{
   // Loop through aero objects
   for (auto& aeroObj : mAeroObjMap)
   {
      aeroObj.second->CalculateAeroAttitudeVectors();
   }
}

size_t VehicleAero::AddAeroObject(AeroObject* aAeroObject)
{
   ++mMaxMapIndex;
   mAeroObjMap[mMaxMapIndex] = aAeroObject;
   return mMaxMapIndex;
}

const std::vector<AeroObject*> VehicleAero::GetAeroObjectBasedOnType(const std::string& aAeroTypeString)
{
   std::vector<AeroObject*> list;

   for (auto& aeroObj : mAeroObjMap)
   {
      AeroObject* aeroObject = aeroObj.second;

      if (aAeroTypeString == aeroObject->AeroObjType())
      {
         // Add it to the list
         list.push_back(aeroObject);
      }
   }

   return list;
}

const std::vector<AeroObject*> VehicleAero::GetAeroObjectsGeneratedFrom(const std::string& aGeometryObjectName)
{
   std::vector<AeroObject*> list;

   for (auto& aeroObj : mAeroObjMap)
   {
      AeroObject* aeroObject = aeroObj.second;

      std::string generatedFromName = aeroObject->GetGeneratedFromName();

      if (generatedFromName == aGeometryObjectName)
      {
         // Add it to the list
         list.push_back(aeroObject);
      }
   }

   return list;
}

void VehicleAero::CalcForcesAndMomentsOnSubobjects(double    aAltitude_ft,
                                                   double    aSpeed_fps,
                                                   double    aMach,
                                                   double    aKTAS,
                                                   double    aKCAS,
                                                   double    aDynamicPressure_psf,
                                                   double    aDensity_slugs_ft3,
                                                   double    aVehicleAlpha_deg,
                                                   double    aVehicleBeta_deg,
                                                   double    aVehicleYawRate_dps,
                                                   double    aVehiclePitchRate_dps,
                                                   double    aVehicleRollRate_dps,
                                                   double&   aTotalLift_lbs,
                                                   double&   aTotalDrag_lbs,
                                                   double&   aTotalSide_lbs,
                                                   UtVec3dX& aMomentVector_ftlbs)
{
   // Loop through the aero objects and calculate F&M
   for (auto& aeroObj : mAeroObjMap)
   {
      UtVec3dX liftBodyRefVector_lbs;
      UtVec3dX dragBodyRefVector_lbs;
      UtVec3dX sideBodyRefVector_lbs;
      UtVec3dX momentBodyRef_ftlbs;
      UtVec3dX forcePoint_ft;

      aeroObj.second->CalcForcesAndMoments(aDynamicPressure_psf,
                                           aMach,
                                           liftBodyRefVector_lbs,
                                           dragBodyRefVector_lbs,
                                           sideBodyRefVector_lbs,
                                           momentBodyRef_ftlbs,
                                           forcePoint_ft);


      P6DofForceAndMomentsObject liftBodyZeroVector_lbs;
      P6DofForceAndMomentsObject dragBodyZeroVector_lbs;
      P6DofForceAndMomentsObject sideBodyZeroVector_lbs;
      P6DofForceAndMomentsObject momentBodyZero_ftlbs;

      // Set the reference point at the object's aero reference point
      liftBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
      dragBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
      sideBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
      momentBodyZero_ftlbs.MoveRefPoint_ft(forcePoint_ft);

      // Clear F&M data
      liftBodyZeroVector_lbs.ClearForcesAndMoments();
      dragBodyZeroVector_lbs.ClearForcesAndMoments();
      sideBodyZeroVector_lbs.ClearForcesAndMoments();
      momentBodyZero_ftlbs.ClearForcesAndMoments();

      // Add the body-relative data at the reference point
      UtVec3dX zeroVec(0.0, 0.0, 0.0);
      liftBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(liftBodyRefVector_lbs, zeroVec);
      dragBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(dragBodyRefVector_lbs, zeroVec);
      sideBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(sideBodyRefVector_lbs, zeroVec);
      momentBodyZero_ftlbs.AddForceAndMomentAtReferencePoint(zeroVec, momentBodyRef_ftlbs);

      // Move the reference point to the origin as the "zero point, rather than using
      // each object's aero force center

      UtVec3dX zeroPointReference(0.0, 0.0, 0.0);
      liftBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
      dragBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
      sideBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
      momentBodyZero_ftlbs.MoveRefPoint_ft(zeroPointReference);

      // At this point, all F&M are based on a reference point of the origin, as desired

      // Now, we calculate the total moment, including all induced moments from all F&M contributors

      UtVec3dX partialMoment(0.0, 0.0, 0.0);
      UtVec3dX totalMomentBodyZero_ftlbs(0.0, 0.0, 0.0);

      partialMoment.Set(0.0, 0.0, 0.0);
      UtVec3dX tempLiftBodyZeroVector_lbs(0.0, 0.0, 0.0);
      liftBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempLiftBodyZeroVector_lbs, partialMoment);
      totalMomentBodyZero_ftlbs += partialMoment;

      partialMoment.Set(0.0, 0.0, 0.0);
      UtVec3dX tempDragBodyZeroVector_lbs(0.0, 0.0, 0.0);
      dragBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempDragBodyZeroVector_lbs, partialMoment);
      totalMomentBodyZero_ftlbs += partialMoment;

      partialMoment.Set(0.0, 0.0, 0.0);
      UtVec3dX tempSideBodyZeroVector_lbs(0.0, 0.0, 0.0);
      sideBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempSideBodyZeroVector_lbs, partialMoment);
      totalMomentBodyZero_ftlbs += partialMoment;

      UtVec3dX zeroForce(0.0, 0.0, 0.0);
      partialMoment.Set(0.0, 0.0, 0.0);
      momentBodyZero_ftlbs.GetForceAndMomentAtCurrentRefPoint(zeroForce, partialMoment);
      totalMomentBodyZero_ftlbs += partialMoment;

      // We now have the body-relative forces at the zero point. We also have the
      // total moment (in body coordinates) at the zero point.

      // We now need to determine the magnitude of the forces along the primary lift, drag, and
      // side force directions, rather than in body space.

      UtVec3dX liftVectorDirection(0.0, 0.0, -1.0);
      UtVec3dX dragVectorDirection(-1.0, 0.0, 0.0);
      UtVec3dX sideVectorDirection(0.0, 1.0, 0.0);

      // Transform the body relative vectors into the primary reference

      // Note negative beta
      UtDCM dcm(-aVehicleBeta_deg * UtMath::cRAD_PER_DEG, aVehicleAlpha_deg * UtMath::cRAD_PER_DEG, 0.0);

      UtVec3dX tempLiftVectorPrimary_lbs = dcm.InverseTransform(tempLiftBodyZeroVector_lbs);
      UtVec3dX tempDragVectorPrimary_lbs = dcm.InverseTransform(tempDragBodyZeroVector_lbs);
      UtVec3dX tempSideVectorPrimary_lbs = dcm.InverseTransform(tempSideBodyZeroVector_lbs);

      // Get forces along the primary lift vector direction
      double objectLift_lbs = liftVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
                              liftVectorDirection.Dot(tempDragVectorPrimary_lbs) +
                              liftVectorDirection.Dot(tempSideVectorPrimary_lbs);

      // Get forces along the primary drag vector direction
      double objectDrag_lbs = dragVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
                              dragVectorDirection.Dot(tempDragVectorPrimary_lbs) +
                              dragVectorDirection.Dot(tempSideVectorPrimary_lbs);

      // Get forces along the primary side vector direction
      double objectSide_lbs = sideVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
                              sideVectorDirection.Dot(tempDragVectorPrimary_lbs) +
                              sideVectorDirection.Dot(tempSideVectorPrimary_lbs);

      // Get the total moment (in body coordinates)
      UtVec3dX objectMoment_ftlbs = totalMomentBodyZero_ftlbs;

      // Add object data to total data
      aTotalLift_lbs += objectLift_lbs;
      aTotalDrag_lbs += objectDrag_lbs;
      aTotalSide_lbs += objectSide_lbs;
      aMomentVector_ftlbs += objectMoment_ftlbs;
   }
}

bool VehicleAero::AeroObjectNameIsUnique(const std::string& aName)
{

   return !std::any_of(mAeroObjMap.begin(), mAeroObjMap.end(), 
                     [&](const std::pair<size_t, AeroObject*>& aeroObj) {return aeroObj.second->GetName() == aName; });

}

std::string VehicleAero::MakeNameUnique(const std::string& aBaseName)
{
   if (AeroObjectNameIsUnique(aBaseName))
   {
      return aBaseName;
   }

   QString currentName(aBaseName.c_str());
   QString originalName(aBaseName.c_str());

   int currentValue = 1;
   bool done = false;
   while (!done)
   {
      if (!AeroObjectNameIsUnique(currentName.toStdString()))
      {
         // Current name is not unique, so modify it and try again

         // Reset name to original
         currentName = originalName;

         // Append the current value to the name string
         currentName.append(QString::number(currentValue));

         // Increment the value for next time
         ++currentValue;
      }
      else
      {
         done = true;
      }
   }

   // Return the unique name
   return currentName.toStdString();
}

bool VehicleAero::MovableAeroNameIsUnique(std::string& aName)
{
   // Loop through the aero objects
   for (auto& movableAero : mMovableAeroList)
   {
      if (movableAero.GetName() == aName)
      {
         return false;
      }
   }

   return true;
}

void VehicleAero::AddMovableAero(AeroObject* aAeroObject)
{
   if (aAeroObject->AeroObjType() == "AeroWing")
   {
      AeroWing* aeroWing = dynamic_cast<AeroWing*>(aAeroObject);
      if (aeroWing == nullptr)
      {
         return;
      }
      if (aeroWing->AileronsPresent())
      {
         std::string name = aeroWing->GetAilerons()->GetName();
         if (!MovableAeroNameIsUnique(name))
         {
            MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
            return;
         }

         mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));

      }
      if (aeroWing->DrageronsPresent())
      {
         std::string name = aeroWing->GetDrageronsTop()->GetName();
         if (!MovableAeroNameIsUnique(name))
         {
            MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
            return;
         }

         mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));

         name = aeroWing->GetDrageronsBottom()->GetName();
         if (!MovableAeroNameIsUnique(name))
         {
            MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
            return;
         }

         mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));
      }
      if (aeroWing->SpoileronsPresent())
      {
         std::string name = aeroWing->GetSpoilerons()->GetName();
         if (!MovableAeroNameIsUnique(name))
         {
            MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
            return;
         }

         mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));
      }
      if (aeroWing->SpoilersPresent())
      {
         std::string name = aeroWing->GetSpoilers()->GetName();
         if (!MovableAeroNameIsUnique(name))
         {
            MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
            return;
         }

         mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));
      }
      if (aeroWing->ElevonsPresent())
      {
         std::string name = aeroWing->GetElevons()->GetName();
         if (!MovableAeroNameIsUnique(name))
         {
            MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
            return;
         }
         mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));

      }
   }
   else
   {
      std::string name = aAeroObject->GetName();
      // Verify name is unique prior to adding
      if (!MovableAeroNameIsUnique(name))
      {
         MoverCreatorMainWindow().CreateErrorBox("The movable aero part name is already taken!");
         return;
      }

      mMovableAeroList.push_back(VehicleAeroMovable(name, aAeroObject));
   }

}

VehicleAeroMovable* VehicleAero::GetMovableAero(const std::string& aName)
{
   // Loop through the aero objects
   for (auto& movableAero : mMovableAeroList)
   {
      if (movableAero.GetName() == aName)
      {
         // Match found
         return &movableAero;
      }
   }
   return nullptr;

   //auto movableAero = std::find(mMovableAeroList.begin(), mMovableAeroList.end(), aName);
   //return ((movableAero == mMovableAeroList.end()) ? nullptr : &movableAero->second);

}

std::vector<VehicleAeroMovable>& VehicleAero::GetMovableAeroList()
{
   return mMovableAeroList;
}

double VehicleAero::ReferenceAreaOfComponent_sqft(AeroObject* aAeroObject)
{
   double refArea_ft2 = 0.0;

   std::string typeName = aAeroObject->AeroObjType();

   if (typeName == "AeroWing")
   {
      auto wing = static_cast<AeroWing*>(aAeroObject);
      refArea_ft2 = wing->GetPlanformArea_ft2();
   }
   else if (typeName == "AeroSurface")
   {
      auto surface = static_cast<AeroSurface*>(aAeroObject);
      refArea_ft2 = surface->GetPlanformArea_ft2();
   }
   else if (typeName == "AeroBody")
   {
      auto body = static_cast<AeroBody*>(aAeroObject);
      refArea_ft2 = body->GetCrossSectionalArea_sqft();
   }
   else if (typeName == "AeroDish")
   {
      auto dish = static_cast<AeroDish*>(aAeroObject);
      refArea_ft2 = dish->GetCrossSectionalArea_sqft();
   }
   else if (typeName == "AeroLandingGear")
   {
      auto gear = static_cast<AeroLandingGear*>(aAeroObject);
      refArea_ft2 = gear->GetCrossSectionalArea_sqft();
   }
   else if (typeName == "AeroNacelle")
   {
      auto nacelle = static_cast<AeroNacelle*>(aAeroObject);
      refArea_ft2 = nacelle->GetCrossSectionalArea_sqft();
   }
   else if (typeName == "AeroSpeedBrake")
   {
      auto brake = static_cast<AeroSpeedBrake*>(aAeroObject);
      refArea_ft2 = brake->GetCrossSectionalArea_sqft();
   }
   else
   {
      ut::log::error() << "Unknown aero object type in VehicleAero::ReferenceAreaOfComponent_sqft().";
   }

   return refArea_ft2;
}

void VehicleAero::CalcP6DOFCoreAeroDataUsingAlphaAndBetaSweep(QProgressDialog& aProgressDialog)
{
   ShowProgress(aProgressDialog);

   // Enable the various tables
   mCoreAero.EnableCL_AlphaBetaMachTable();
   mCoreAero.EnableCd_AlphaBetaMachTable();
   mCoreAero.EnableCY_AlphaBetaMachTable();
   mCoreAero.EnableCm_AlphaBetaMachTable();
   mCoreAero.EnableCn_AlphaBetaMachTable();
   mCoreAero.EnableCl_AlphaBetaMachTable();

   // Get pointers to the various tables
   AeroTable4d* tableCLVsAlphaBetaMach = mCoreAero.GetCL_AlphaBetaMachTablePtr();
   AeroTable4d* tableCdVsAlphaBetaMach = mCoreAero.GetCd_AlphaBetaMachTablePtr();
   AeroTable4d* tableCYVsAlphaBetaMach = mCoreAero.GetCY_AlphaBetaMachTablePtr();
   AeroTable4d* tableCmVsAlphaBetaMach = mCoreAero.GetCm_AlphaBetaMachTablePtr();
   AeroTable4d* tableCnVsAlphaBetaMach = mCoreAero.GetCn_AlphaBetaMachTablePtr();
   AeroTable4d* tableClVsAlphaBetaMach = mCoreAero.GetCl_AlphaBetaMachTablePtr();

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Make Mach entry in tables
      tableCLVsAlphaBetaMach->AddSecondaryTable(mach);
      tableCdVsAlphaBetaMach->AddSecondaryTable(mach);
      tableCYVsAlphaBetaMach->AddSecondaryTable(mach);
      tableCmVsAlphaBetaMach->AddSecondaryTable(mach);
      tableCnVsAlphaBetaMach->AddSecondaryTable(mach);
      tableClVsAlphaBetaMach->AddSecondaryTable(mach);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      // betaAlphaCoeffs -> (beta_deg, [alpha_deg, {CL, Cd, CY, Cm, Cn, Cl}])
      std::map<double, std::map<double, Coeffs>> betaAlphaCoeffs;
      for (auto& betaVal : mBetaDataPts_deg)
      {
         // Only proceed if beta is greater than or equal to the minimum
         if (betaVal >= mMinTableBetaDataPts_deg)
         {
            // If beta is greater than the maximum, stop.
            // If the vehicle is symmetrical in beta, 
            // we only need to do the negative half,
            // so stop once we reach positive beta.
            if ((betaVal > mMaxTableBetaDataPts_deg) ||
               (mBetaSymmetry && (betaVal > 0)))
            {
               break;
            }

            double beta_deg = static_cast<double>(betaVal);

            // alphaCoeffs -> [alpha_deg, {CL, Cd, CY, Cm, Cn, Cl}]
            // For the current beta, save off coefficients for all alphas.
            std::map<double, Coeffs> alphaCoeffs;
            for (auto& alphaVal : mAlphaDataPts_deg)
            {
               // Only proceed if alpha is greater than or equal to the minimum
               if (alphaVal >= mMinTableAlphaDataPts_deg)
               {
                  // If alpha is greater than the maximum, stop.
                  // If the vehicle is symmetrical in alpha, 
                  // we only need to do the negative half,
                  // so stop once we reach positive alpha.
                  if ((alphaVal > mMaxTableAlphaDataPts_deg)||
                     (mAlphaSymmetry && (alphaVal > 0)))
                  {
                     break;
                  }

                  double alpha_deg = static_cast<double>(alphaVal);

                  double vehicleYawRate_dps   = 0.0;
                  double vehiclePitchRate_dps = 0.0;
                  double vehicleRollRate_dps  = 0.0;
                  double totalLift_lbs        = 0.0;
                  double totalDrag_lbs        = 0.0;
                  double totalSide_lbs        = 0.0;
                  UtVec3dX totalMomentVec_ftlbs;

                  double CL = 0.0;
                  double Cd = 0.0;
                  double CY = 0.0;
                  double Cl = 0.0;
                  double Cm = 0.0;
                  double Cn = 0.0;

                  CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                       dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                       vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                                       totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                                       CL, Cd, CY, Cl, Cm, Cn);

                  alphaCoeffs.emplace(alpha_deg, Coeffs(CL, Cd, CY, Cm, Cn, Cl));

                  // If the vehicle is symmetrical in alpha, 
                  // add a corresponding entry for the positive half.
                  // Don't add it if alpha == 0 because we will have added it twice.
                  if (mAlphaSymmetry && (alphaVal != 0))
                  {
                     alphaCoeffs.emplace(-alpha_deg, Coeffs(-CL, Cd, CY, -Cm, Cn, Cl));                     
                  }
               }
            } // End alpha loop

            betaAlphaCoeffs.emplace(beta_deg, alphaCoeffs);

            // If the vehicle is symmetrical in beta, 
            // add a corresponding entry for the positive half.
            // Don't add it if beta == 0 because we will have added it twice.
            if (mBetaSymmetry && betaVal != 0)
            {
               for (auto& coeffs : alphaCoeffs)
               {
                  coeffs.second.CY *= -1;
                  coeffs.second.Cn *= -1;
                  coeffs.second.Cl *= -1;
               }
               betaAlphaCoeffs.emplace(-beta_deg, alphaCoeffs);
            }
         }
      }

      // Now, we have all coefficients for every alpha/beta.
      // Add the coefficients in betaAlphaCoeffs to our tables. 
      for (const auto& alphaCoeffs : betaAlphaCoeffs) // alphaCoeffs is a std::pair<double, std::map<double, Coeffs>>
      {
         double beta_deg = alphaCoeffs.first; // beta_deg is the key in the map betaAlphaCoeffs

         // Make Beta entry in tables
         tableCLVsAlphaBetaMach->AddPrimaryTable(beta_deg);
         tableCdVsAlphaBetaMach->AddPrimaryTable(beta_deg);
         tableCYVsAlphaBetaMach->AddPrimaryTable(beta_deg);
         tableCmVsAlphaBetaMach->AddPrimaryTable(beta_deg);
         tableCnVsAlphaBetaMach->AddPrimaryTable(beta_deg);
         tableClVsAlphaBetaMach->AddPrimaryTable(beta_deg);

         for (const auto& coeffs : alphaCoeffs.second) // coeffs is a std::pair<double, Coeffs>
         {
            double alpha_deg = coeffs.first; // alpha_deg is the key in the map alphaCoeffs.second

            // Make Alpha entry in tables
            tableCLVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.CL);
            tableCdVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.Cd);
            tableCYVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.CY);
            tableCmVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.Cm);
            tableCnVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.Cn);
            tableClVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.Cl);

            mCurrentNumberOfAeroTests += 6; // Six tests
         }
      }
      ShowProgress(aProgressDialog);
   }

   tableCLVsAlphaBetaMach->UpdateModifiedData();
   tableCdVsAlphaBetaMach->UpdateModifiedData();
   tableCYVsAlphaBetaMach->UpdateModifiedData();
   tableCmVsAlphaBetaMach->UpdateModifiedData();
   tableCnVsAlphaBetaMach->UpdateModifiedData();
   tableClVsAlphaBetaMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFCoreAeroDataUsingPitchRateSweep(QProgressDialog& aProgressDialog)
{
   // Enable the various tables
   mCoreAero.EnableCmq_MachTable();
   mCoreAero.EnableClq_MachTable();
   mCoreAero.EnableCm_AlphaDotMachTable();
   mCoreAero.EnableCl_AlphaDotMachTable();

   // Get pointers to the various tables
   AeroTable2d* tableCmqVsMach = mCoreAero.GetCmq_MachTablePtr();
   AeroTable2d* tableClqVsMach = mCoreAero.GetClq_MachTablePtr();
   AeroTable2d* tableCm_AlphaDotVsMach = mCoreAero.GetCm_AlphaDotMachTablePtr();
   AeroTable2d* tableCl_AlphaDotVsMach = mCoreAero.GetCl_AlphaDotMachTablePtr();

   AeroTable4d* Cm_table = mCoreAero.GetCm_AlphaBetaMachTablePtr();
   AeroTable4d* Cl_table = mCoreAero.GetCl_AlphaBetaMachTablePtr();

   // We use a hundredth of a deg/sec as the nominal pitch rate
   double q_dps = 0.01;
   double q_rps = q_dps * UtMath::cRAD_PER_DEG;

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      double vehicleYawRate_dps   = 0.0;
      double vehiclePitchRate_dps = q_dps;
      double vehicleRollRate_dps  = 0.0;
      double totalLift_lbs        = 0.0;
      double totalDrag_lbs        = 0.0;
      double totalSide_lbs        = 0.0;
      double alpha_deg            = 0.0;
      double beta_deg             = 0.0;
      UtVec3dX totalMomentVec_ftlbs;

      // First, calculate the nominal conditions
      double nominalCl = 0.0;
      double nominalCm = 0.0;

      Cm_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCm);
      Cl_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCl);

      // Next, calculate the current conditions
      double CL = 0.0;
      double Cd = 0.0;
      double CY = 0.0;
      double Cl = 0.0;
      double Cm = 0.0;
      double Cn = 0.0;

      CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                           dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                           vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                           totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                           CL, Cd, CY, Cl, Cm, Cn);
      
      // Determine the incremental change in coefficients
      double deltaCm = Cm - nominalCm;
      double deltaCl = Cl - nominalCl;

      double k = q_rps / (2 * speed_fps);

      // Calculate the reduced frequency
      double kmq = mUseRefAreaInsteadOfWingArea ? k * mVehicleRefLength_ft : k * mVehicleWingChord_ft;
      double klq = mUseRefAreaInsteadOfWingArea ? k * mVehicleRefLength_ft : k * mVehicleWingSpan_ft;

      // Divide by the reduced frequency
      double Cmq = deltaCm / kmq;
      double Clq = deltaCl / klq;

      // Pitch damping moment due to pitch rate
      tableCmqVsMach->AddData(mach, Cmq * mCmqMult);

      // Pitch damping moment due to alpha dot
      tableCm_AlphaDotVsMach->AddData(mach, Cmq * mCmAlphadotMult);

      // Roll damping moment due to pitch rate
      tableClqVsMach->AddData(mach, Clq * mClqMult);

      // Roll damping moment due to alpha dot
      tableCl_AlphaDotVsMach->AddData(mach, Clq * mClAlphadotMult);

      mCurrentNumberOfAeroTests += 4; // Four tests
   }

   ShowProgress(aProgressDialog);

   tableCmqVsMach->UpdateModifiedData();
   tableCm_AlphaDotVsMach->UpdateModifiedData();
   tableClqVsMach->UpdateModifiedData();
   tableCl_AlphaDotVsMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFCoreAeroDataUsingAlphaAndPitchRateSweep(QProgressDialog& aProgressDialog)
{
   // Enable the various tables
   mCoreAero.EnableCLq_AlphaMachTable();
   mCoreAero.EnableCL_AlphaDotAlphaMachTable();

   // Get pointers to the various tables
   AeroTable3d* tableCLqVsAlphaMach = mCoreAero.GetCLq_AlphaMachTablePtr();
   AeroTable3d* tableCL_AlphaDotVsAlphaMach = mCoreAero.GetCL_AlphaDotAlphaMachTablePtr();

   AeroTable4d* CL_table = mCoreAero.GetCL_AlphaBetaMachTablePtr();

   // We use a hundredth of a deg/sec as the nominal pitch rate
   double q_dps = 0.01;
   double q_rps = q_dps * UtMath::cRAD_PER_DEG;

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Make Mach entry in table
      tableCLqVsAlphaMach->AddTable(mach);
      tableCL_AlphaDotVsAlphaMach->AddTable(mach);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      // alphaCLqMap -> [alpha_deg, CLq]
      std::map<double, double> alphaCLqMap;
      for (auto& alphaVal : mAlphaDataPts_deg)
      {
         // Only proceed if alpha is greater than or equal to the minimum
         if (alphaVal >= mMinTableAlphaDataPts_deg)
         {
            // If alpha is greater than the maximum, stop.
            // If the vehicle is symmetrical in alpha, 
            // we only need to do the negative half,
            // so stop once we reach positive alpha.
            if ((alphaVal > mMaxTableAlphaDataPts_deg) ||
               (mAlphaSymmetry && (alphaVal > 0)))
            {
               break;
            }

            double alpha_deg = alphaVal;

            double vehicleYawRate_dps   = 0.0;
            double vehiclePitchRate_dps = q_dps;
            double vehicleRollRate_dps  = 0.0;
            double totalLift_lbs        = 0.0;
            double totalDrag_lbs        = 0.0;
            double totalSide_lbs        = 0.0;
            double beta_deg             = 0.0;
            UtVec3dX totalMomentVec_ftlbs;

            // First, calculate the nominal conditions
            double nominalCL = 0.0;

            CL_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCL);

            // Next, calculate the current conditions
            double CL = 0.0;
            double Cd = 0.0;
            double CY = 0.0;
            double Cl = 0.0;
            double Cm = 0.0;
            double Cn = 0.0;

            CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                 dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                 vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                                 totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                                 CL, Cd, CY, Cl, Cm, Cn);

            // Determine the incremental change in coefficients
            double deltaCL = CL - nominalCL;

            // Calculate the reduced frequency
            double k = q_rps / (2 * speed_fps);

            if (mUseRefAreaInsteadOfWingArea)
            {
               k *= mVehicleRefLength_ft;
            }
            else
            {
               k *= mVehicleWingChord_ft;
            }

            // Divide by the reduced frequency
            double CLq = deltaCL / k;

            alphaCLqMap.emplace(alpha_deg, CLq);
            // If the vehicle is symmetrical in alpha, 
            // add a corresponding entry for the positive half.
            // Don't add it if alpha == 0 because we will have added it twice.
            if (mAlphaSymmetry && (alphaVal != 0))
            {
               alphaCLqMap.emplace(-alpha_deg, CLq);
            }
         }
      }
      // Now, we have all coefficients for every alpha
      // Add the coefficients in alphaCLqMap to our tables. 
      for (const auto& alphaCLq : alphaCLqMap)
      {
         double alpha_deg = alphaCLq.first;
         double CLq = alphaCLq.second;

         tableCLqVsAlphaMach->AddData(alpha_deg, CLq);
         tableCL_AlphaDotVsAlphaMach->AddData(alpha_deg, CLq);
         mCurrentNumberOfAeroTests += 2; // Two tests
      }
   }

   ShowProgress(aProgressDialog);

   tableCLqVsAlphaMach->UpdateModifiedData();
   tableCL_AlphaDotVsAlphaMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFCoreAeroDataUsingYawRateSweep(QProgressDialog& aProgressDialog)
{
   // Enable the various tables
   mCoreAero.EnableCnr_MachTable();
   mCoreAero.EnableClr_MachTable();
   mCoreAero.EnableCn_BetaDotMachTable();
   mCoreAero.EnableCl_BetaDotMachTable();

   // Get pointers to the various tables
   AeroTable2d* tableCnrVsMach = mCoreAero.GetCnr_MachTablePtr();
   AeroTable2d* tableClrVsMach = mCoreAero.GetClr_MachTablePtr();
   AeroTable2d* tableCn_BetaDotVsMach = mCoreAero.GetCn_BetaDotMachTablePtr();
   AeroTable2d* tableCl_BetaDotVsMach = mCoreAero.GetCl_BetaDotMachTablePtr();

   AeroTable4d* Cn_table = mCoreAero.GetCn_AlphaBetaMachTablePtr();
   AeroTable4d* Cl_table = mCoreAero.GetCl_AlphaBetaMachTablePtr();

   // We use a hundredth of a deg/sec as the nominal yaw rate
   double r_dps = 0.01;
   double r_rps = r_dps * UtMath::cRAD_PER_DEG;

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      double vehicleYawRate_dps   = r_dps;
      double vehiclePitchRate_dps = 0.0;
      double vehicleRollRate_dps  = 0.0;
      double totalLift_lbs        = 0.0;
      double totalDrag_lbs        = 0.0;
      double totalSide_lbs        = 0.0;
      double alpha_deg            = 0.0;
      double beta_deg             = 0.0;
      UtVec3dX totalMomentVec_ftlbs;

      // First, calculate the nominal conditions

      double nominalCl = 0.0;
      double nominalCn = 0.0;

      Cn_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCn);
      Cl_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCl);

      // Next, calculate the current conditions
      double CL = 0.0;
      double Cd = 0.0;
      double CY = 0.0;
      double Cl = 0.0;
      double Cm = 0.0;
      double Cn = 0.0;

      CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                           dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                           vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                           totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                           CL, Cd, CY, Cl, Cm, Cn);

      // Determine the incremental change in coefficients
      double deltaCn = Cn - nominalCn;
      double deltaCl = Cl - nominalCl;

      // Calculate the reduced frequency
      double k = r_rps / (2 * speed_fps);

      if (mUseRefAreaInsteadOfWingArea)
      {
         k *= mVehicleRefLength_ft;
      }
      else
      {
         k *= mVehicleWingSpan_ft;
      }

      // Divide by the reduced frequency
      double Cnr = deltaCn / k;
      double Clr = deltaCl / k;

      // Make entry in tables

      // Yaw damping moment due to yaw rate
      tableCnrVsMach->AddData(mach, Cnr * mCnrMult);

      // Roll damping moment due to yaw rate
      tableClrVsMach->AddData(mach, Clr * mClrMult);

      // Yaw damping moment due to beta dot
      tableCn_BetaDotVsMach->AddData(mach, -Cnr * mCnBetadotMult); // Note negative for beta-dot

      // Roll damping moment due to beta dot
      tableCl_BetaDotVsMach->AddData(mach, -Clr * mClBetadotMult); // Note negative for beta-dot

      mCurrentNumberOfAeroTests += 4; // Four tests
   }

   ShowProgress(aProgressDialog);

   tableCnrVsMach->UpdateModifiedData();
   tableClrVsMach->UpdateModifiedData();
   tableCn_BetaDotVsMach->UpdateModifiedData();
   tableCl_BetaDotVsMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFCoreAeroDataUsingBetaAndYawRateSweep(QProgressDialog& aProgressDialog)
{
   // Enable the various tables
   mCoreAero.EnableCYr_BetaMachTable();
   mCoreAero.EnableCY_BetaDotBetaMachTable();

   // Get pointers to the various tables
   AeroTable3d* tableCYrVsBetaMach = mCoreAero.GetCYr_BetaMachTablePtr();
   AeroTable3d* tableCY_BetaDotVsBetaMach = mCoreAero.GetCY_BetaDotBetaMachTablePtr();

   AeroTable4d* CY_table = mCoreAero.GetCY_AlphaBetaMachTablePtr();

   // We use a hundredth of a deg/sec as the nominal yaw rate
   double r_dps = 0.01;
   double r_rps = r_dps * UtMath::cRAD_PER_DEG;

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Make Mach entry in table
      tableCYrVsBetaMach->AddTable(mach);
      tableCY_BetaDotVsBetaMach->AddTable(mach);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      // betaCYrMap -> [beta_deg, CYr]
      std::map<double, double> betaCYrMap;

      for (auto& betaVal : mBetaDataPts_deg)
      {
         // Only proceed if beta is greater than or equal to the minimum
         if (betaVal >= mMinTableBetaDataPts_deg)
         {
            // If beta is greater than the maximum, stop.
            // If the vehicle is symmetrical in beta, 
            // we only need to do the negative half,
            // so stop once we reach positive beta.
            if ((betaVal > mMaxTableBetaDataPts_deg) ||
               (mBetaSymmetry && (betaVal > 0)))
            {
               break;
            }

            double beta_deg = static_cast<double>(betaVal);

            double vehicleYawRate_dps   = r_dps;
            double vehiclePitchRate_dps = 0.0;
            double vehicleRollRate_dps  = 0.0;
            double totalLift_lbs        = 0.0;
            double totalDrag_lbs        = 0.0;
            double totalSide_lbs        = 0.0;
            double alpha_deg            = 0.0;
            UtVec3dX totalMomentVec_ftlbs;

            // First, calculate the nominal conditions
            double nominalCY = 0.0;

            CY_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCY);

            // Next, calculate the current conditions
            double CL = 0.0;
            double Cd = 0.0;
            double CY = 0.0;
            double Cl = 0.0;
            double Cm = 0.0;
            double Cn = 0.0;

            CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                 dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                 vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                                 totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                                 CL, Cd, CY, Cl, Cm, Cn);

            // Determine the incremental change in coefficients
            double deltaCY = CY - nominalCY;

            // Calculate the reduced frequency
            double k = r_rps / (2 * speed_fps);

            if (mUseRefAreaInsteadOfWingArea)
            {
               k *= mVehicleRefLength_ft;
            }
            else
            {
               k *= mVehicleWingSpan_ft;
            }

            // Divide by the reduced frequency
            double CYr = deltaCY / k;

            betaCYrMap.emplace(beta_deg, CYr);

            // If the vehicle is symmetrical in beta, 
            // add a corresponding entry for the positive half.
            // Don't add it if beta == 0 because we will have added it twice.
            if (mBetaSymmetry && betaVal != 0)
            {
               betaCYrMap.emplace(-beta_deg, CYr);
            }

         }
      }
      // Now, we have all coefficients for every beta.
      // Add the coefficients in betaCYrMap to our tables. 
      for (const auto& betaCYr : betaCYrMap)
      {
         double beta_deg = betaCYr.first;
         double CYr = betaCYr.second;

         // Make beta entry in tables
         tableCYrVsBetaMach->AddData(beta_deg, CYr);
         tableCY_BetaDotVsBetaMach->AddData(beta_deg, -CYr); // Note negative for beta-dot

         mCurrentNumberOfAeroTests += 2; // Two tests
      }
   }

   ShowProgress(aProgressDialog);

   tableCYrVsBetaMach->UpdateModifiedData();
   tableCY_BetaDotVsBetaMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFCoreAeroDataUsingRollRateSweep(QProgressDialog& aProgressDialog)
{
   // Enable the various tables
   mCoreAero.EnableCmp_MachTable();
   mCoreAero.EnableCnp_MachTable();
   mCoreAero.EnableClp_MachTable();

   // Get pointers to the various tables
   AeroTable2d* tableCmpVsMach = mCoreAero.GetCmp_MachTablePtr();
   AeroTable2d* tableCnpVsMach = mCoreAero.GetCnp_MachTablePtr();
   AeroTable2d* tableClpVsMach = mCoreAero.GetClp_MachTablePtr();

   AeroTable4d* Cm_table = mCoreAero.GetCm_AlphaBetaMachTablePtr();
   AeroTable4d* Cn_table = mCoreAero.GetCn_AlphaBetaMachTablePtr();
   AeroTable4d* Cl_table = mCoreAero.GetCl_AlphaBetaMachTablePtr();

   // We use a hundredth of a deg/sec as the roll pitch rate
   double p_dps = 0.01;
   double p_rps = p_dps * UtMath::cRAD_PER_DEG;

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      double vehicleYawRate_dps   = 0.0;
      double vehiclePitchRate_dps = 0.0;
      double vehicleRollRate_dps  = p_dps;
      double totalLift_lbs        = 0.0;
      double totalDrag_lbs        = 0.0;
      double totalSide_lbs        = 0.0;
      double alpha_deg            = 0.0;
      double beta_deg             = 0.0;
      UtVec3dX totalMomentVec_ftlbs;

      // First, calculate the nominal conditions
      double nominalCl = 0.0;
      double nominalCm = 0.0;
      double nominalCn = 0.0;

      Cm_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCm);
      Cn_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCn);
      Cl_table->GetInterpolatedDependentValueModified(alpha_deg, beta_deg, mach, nominalCl);

      // Next, calculate the current conditions
      double CL = 0.0;
      double Cd = 0.0;
      double CY = 0.0;
      double Cl = 0.0;
      double Cm = 0.0;
      double Cn = 0.0;

      CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                           dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                           vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                           totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                           CL, Cd, CY, Cl, Cm, Cn);

      // Determine the incremental change in coefficients
      double deltaCm = Cm - nominalCm;
      double deltaCn = Cn - nominalCn;
      double deltaCl = Cl - nominalCl;

      // Calculate the reduced frequency
      double k = p_rps / (2 * speed_fps);
      double k_span  = mUseRefAreaInsteadOfWingArea ? k * mVehicleRefLength_ft : k * mVehicleWingSpan_ft;
      double k_chord = mUseRefAreaInsteadOfWingArea ? k * mVehicleRefLength_ft : k * mVehicleWingChord_ft;

      // Note: knp = klp = k_span
      //             kmp = k_chord

      // Divide by the reduced frequency
      double Cmp = deltaCm / k_chord;
      double Cnp = deltaCn / k_span;
      double Clp = deltaCl / k_span;

      // Pitch damping moment due to roll rate
      tableCmpVsMach->AddData(mach, Cmp * mCmpMult);

      // Yaw damping moment due to roll rate
      tableCnpVsMach->AddData(mach, Cnp * mCnpMult);

      // Roll damping moment due to roll rate
      tableClpVsMach->AddData(mach, Clp * mClpMult);

      mCurrentNumberOfAeroTests += 3; // Three tests
   }

   ShowProgress(aProgressDialog);

   tableCmpVsMach->UpdateModifiedData();
   tableCnpVsMach->UpdateModifiedData();
   tableClpVsMach->UpdateModifiedData();
}

void VehicleAero::CalcPM6DOFCoreAeroDataUsingAlphaAndBetaSweep(QProgressDialog& aProgressDialog)
{
    // Enable the various tables
    EnableDisablePointMassTables(true);

    // Get pointers to the various tables
    AeroTable4d* tableCLVsAlphaBetaMach = mCoreAero.GetTrimCL_AlphaBetaMachTablePtr();
    AeroTable4d* tableCdVsAlphaBetaMach = mCoreAero.GetTrimCd_AlphaBetaMachTablePtr();
    AeroTable4d* tableCYVsAlphaBetaMach = mCoreAero.GetTrimCY_AlphaBetaMachTablePtr();

    AeroTable2d* tableRollAccelCapabilityVsMach  = mCoreAero.GetRollAccelCapability_MachTable();
    AeroTable2d* tablePitchAccelCapabilityVsMach = mCoreAero.GetPitchAccelCapability_MachTable();
    AeroTable2d* tableYawAccelCapabilityVsMach   = mCoreAero.GetYawAccelCapability_MachTable();

    AeroTable2d* tableRollStabilityFrequencyVsMach  = mCoreAero.GetRollStabilityFrequency_MachTable();
    AeroTable2d* tablePitchStabilityFrequencyVsMach = mCoreAero.GetPitchStabilityFrequency_MachTable();
    AeroTable2d* tableYawStabilityFrequencyVsMach   = mCoreAero.GetYawStabilityFrequency_MachTable();

    AeroTable2d* tableSpeedbrakeDeltaCDVsMach = mCoreAero.GetSpeedbrakeDeltaCD_MachTable();
    AeroTable2d* tableFlapsDeltaCLVsMach      = mCoreAero.GetFlapsDeltaCD_MachTable();
    AeroTable2d* tableFlapsDeltaCDVsMach      = mCoreAero.GetFlapsDeltaCL_MachTable();
    AeroTable2d* tableSpoilersDeltaCLVsMach   = mCoreAero.GetSpoilersDeltaCD_MachTable();
    AeroTable2d* tableSpoilersDeltaCDVsMach   = mCoreAero.GetSpoilersDeltaCL_MachTable();

    double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

    std::vector<double> machDataPoints;
    for (auto& machVal : mMachDataPts_HundredthsMach)
    {
       if (machVal > mMaxTableMach_HundredthsMach)
       {
          break;
       }

       machDataPoints.push_back(0.01 * static_cast<double>(machVal));
    }

    // These are placeholder assumptions for aerodynamic response and should
    // be replaced by data informed by the higher-fidelity RB6DOF
    if (GetParentVehicle()->IsAircraft())
    {
       Populate2D_Table(*tableRollAccelCapabilityVsMach, 0.8, 180.0, machDataPoints);
       Populate2D_Table(*tablePitchAccelCapabilityVsMach, 0.8, 80.0, machDataPoints);
       Populate2D_Table(*tableYawAccelCapabilityVsMach, 0.8, 60.0, machDataPoints);

       Populate2D_Table(*tableRollStabilityFrequencyVsMach, 0.8, 1.0, machDataPoints);
       Populate2D_Table(*tablePitchStabilityFrequencyVsMach, 0.8, 0.05, machDataPoints);
       Populate2D_Table(*tableYawStabilityFrequencyVsMach, 0.8, 10.0, machDataPoints);

       Populate2D_Table(*tableSpeedbrakeDeltaCDVsMach, 0.8, 0.1, machDataPoints);
       Populate2D_Table(*tableFlapsDeltaCLVsMach, 0.8, 0.15, machDataPoints);
       Populate2D_Table(*tableFlapsDeltaCDVsMach, 0.8, 0.15, machDataPoints);
       Populate2D_Table(*tableSpoilersDeltaCLVsMach, 0.8, -0.1, machDataPoints);
       Populate2D_Table(*tableSpoilersDeltaCDVsMach, 0.8, 0.15, machDataPoints);
    }
    else if (GetParentVehicle()->IsWeapon())
    {
       Populate2D_Table(*tableRollAccelCapabilityVsMach, 0.8, 360.0, machDataPoints);
       Populate2D_Table(*tablePitchAccelCapabilityVsMach, 0.8, 120.0, machDataPoints);
       Populate2D_Table(*tableYawAccelCapabilityVsMach, 0.8, 120.0, machDataPoints);

       Populate2D_Table(*tableRollStabilityFrequencyVsMach, 0.8, 1.0, machDataPoints);
       Populate2D_Table(*tablePitchStabilityFrequencyVsMach, 0.8, 2.0, machDataPoints);
       Populate2D_Table(*tableYawStabilityFrequencyVsMach, 0.8, 2.0, machDataPoints);
    }

    for (auto& mach : machDataPoints)
    {
        // Make Mach entry in tables
        tableCLVsAlphaBetaMach->AddSecondaryTable(mach);
        tableCdVsAlphaBetaMach->AddSecondaryTable(mach);
        tableCYVsAlphaBetaMach->AddSecondaryTable(mach);

        // Calculate speed, in feet per second
        double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

        // Calculate speed, in knots (knots true airspeed)
        double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

        // Calculate knots calibrated airspeed
        double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

        // Calculate dynamic pressure, in lbf per square foot
        double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

        // Calculate density, in slugs per cubic foot
        double density_kgm3 = mAtmosphere.Density(altitude_m);
        double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

        // betaAlphaCoeffs -> (beta_deg, [alpha_deg, {CL, Cd, CY, Cm, Cn, Cl}])
        std::map<double, std::map<double, Coeffs>> betaAlphaCoeffs;
        for (auto& betaVal : mBetaDataPts_deg)
        {
            // Only proceed if beta is greater than or equal to the minimum
            if (betaVal >= mMinTableBetaDataPts_deg)
            {
                // If beta is greater than the maximum, stop.
                // If the vehicle is symmetrical in beta, 
                // we only need to do the negative half,
                // so stop once we reach positive beta.
                if ((betaVal > mMaxTableBetaDataPts_deg) ||
                    (mBetaSymmetry && (betaVal > 0)))
                {
                    break;
                }

                double beta_deg = static_cast<double>(betaVal);

                // alphaCoeffs -> [alpha_deg, {CL, Cd, CY, Cm, Cn, Cl}]
                // For the current beta, save off coefficients for all alphas.
                std::map<double, Coeffs> alphaCoeffs;
                for (auto& alphaVal : mAlphaDataPts_deg)
                {
                    // Only proceed if alpha is greater than or equal to the minimum
                    if (alphaVal >= mMinTableAlphaDataPts_deg)
                    {
                        // If alpha is greater than the maximum, stop.
                        // If the vehicle is symmetrical in alpha, 
                        // we only need to do the negative half,
                        // so stop once we reach positive alpha.
                        if ((alphaVal > mMaxTableAlphaDataPts_deg) ||
                            (mAlphaSymmetry && (alphaVal > 0)))
                        {
                            break;
                        }

                        double alpha_deg = static_cast<double>(alphaVal);

                        double vehicleYawRate_dps = 0.0;
                        double vehiclePitchRate_dps = 0.0;
                        double vehicleRollRate_dps = 0.0;
                        double totalLift_lbs = 0.0;
                        double totalDrag_lbs = 0.0;
                        double totalSide_lbs = 0.0;
                        UtVec3dX totalMomentVec_ftlbs;

                        double CL = 0.0;
                        double Cd = 0.0;
                        double CY = 0.0;
                        double Cl = 0.0;
                        double Cm = 0.0;
                        double Cn = 0.0;

                        CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                            dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                            vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                            totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                            CL, Cd, CY, Cl, Cm, Cn);

                        alphaCoeffs.emplace(alpha_deg, Coeffs(0.9 * CL, 1.0 * Cd, 0.9 * CY, Cm, Cn, Cl));

                        // If the vehicle is symmetrical in alpha, 
                        // add a corresponding entry for the positive half.
                        // Don't add it if alpha == 0 because we will have added it twice.
                        if (mAlphaSymmetry && (alphaVal != 0))
                        {
                            alphaCoeffs.emplace(-alpha_deg, Coeffs(-CL, Cd, CY, -Cm, Cn, Cl));
                        }
                    }
                } // End alpha loop

                betaAlphaCoeffs.emplace(beta_deg, alphaCoeffs);

                // If the vehicle is symmetrical in beta, 
                // add a corresponding entry for the positive half.
                // Don't add it if beta == 0 because we will have added it twice.
                if (mBetaSymmetry && betaVal != 0)
                {
                    for (auto& coeffs : alphaCoeffs)
                    {
                        coeffs.second.CY *= -1;
                        coeffs.second.Cn *= -1;
                        coeffs.second.Cl *= -1;
                    }
                    betaAlphaCoeffs.emplace(-beta_deg, alphaCoeffs);
                }
            }
        }

        // Now, we have all coefficients for every alpha/beta.
        // Add the coefficients in betaAlphaCoeffs to our tables. 
        for (const auto& alphaCoeffs : betaAlphaCoeffs) // alphaCoeffs is a std::pair<double, std::map<double, Coeffs>>
        {
            double beta_deg = alphaCoeffs.first; // beta_deg is the key in the map betaAlphaCoeffs

            // Make Beta entry in tables
            tableCLVsAlphaBetaMach->AddPrimaryTable(beta_deg);
            tableCdVsAlphaBetaMach->AddPrimaryTable(beta_deg);
            tableCYVsAlphaBetaMach->AddPrimaryTable(beta_deg);

            for (const auto& coeffs : alphaCoeffs.second) // coeffs is a std::pair<double, Coeffs>
            {
                double alpha_deg = coeffs.first; // alpha_deg is the key in the map alphaCoeffs.second

                // Make Alpha entry in tables
                tableCLVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.CL);
                tableCdVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.Cd);
                tableCYVsAlphaBetaMach->AddData(alpha_deg, coeffs.second.CY);

                mCurrentNumberOfAeroTests += 3; // Six tests
            }
        }
        ShowProgress(aProgressDialog);
    }

    tableCLVsAlphaBetaMach->UpdateModifiedData();
    tableCdVsAlphaBetaMach->UpdateModifiedData();
    tableCYVsAlphaBetaMach->UpdateModifiedData();
    tableRollAccelCapabilityVsMach->UpdateModifiedData();
    tablePitchAccelCapabilityVsMach->UpdateModifiedData();
    tableYawAccelCapabilityVsMach->UpdateModifiedData();
    tableRollStabilityFrequencyVsMach->UpdateModifiedData();
    tablePitchStabilityFrequencyVsMach->UpdateModifiedData();
    tableYawStabilityFrequencyVsMach->UpdateModifiedData();
    if (tableSpeedbrakeDeltaCDVsMach) { tableSpeedbrakeDeltaCDVsMach->UpdateModifiedData(); }
    if (tableFlapsDeltaCLVsMach) { tableFlapsDeltaCLVsMach->UpdateModifiedData(); }
    if (tableFlapsDeltaCDVsMach) { tableFlapsDeltaCDVsMach->UpdateModifiedData(); }
    if (tableSpoilersDeltaCLVsMach) { tableSpoilersDeltaCLVsMach->UpdateModifiedData(); }
    if (tableSpoilersDeltaCDVsMach) { tableSpoilersDeltaCDVsMach->UpdateModifiedData(); }
}

void VehicleAero::CalcP6DOFMovableAeroDataUsingAngleAndAlphaSweep(VehicleAeroMovable&  aAero,
                                                             std::vector<double>& aAngleDataPts_deg,
                                                             QProgressDialog&     aProgressDialog)
{

   AeroObject* aeroObj = aAero.GetAeroObject();

   // Enable the various tables
   aAero.EnableCL_AngleAlphaMachTablePtr();
   aAero.EnableCm_AngleAlphaMachTablePtr();

   // Get pointers to the various tables
   AeroTable4d* tableCL_AngleAlphaMach = aAero.GetCL_AngleAlphaMachTablePtr();
   AeroTable4d* tableCm_AngleAlphaMach = aAero.GetCm_AngleAlphaMachTablePtr();

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   // Mach, alpha and angle
   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Make Mach entry in tables
      tableCL_AngleAlphaMach->AddSecondaryTable(mach);
      tableCm_AngleAlphaMach->AddSecondaryTable(mach);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      for (auto& alphaVal : mAlphaDataPts_deg)
      {
         if (alphaVal >= mMinTableAlphaDataPts_deg)
         {
            if (alphaVal > mMaxTableAlphaDataPts_deg)
            {
               break;
            }

            double alpha_deg = static_cast<double>(alphaVal);

            // Make Beta entry in tables
            tableCL_AngleAlphaMach->AddPrimaryTable(alpha_deg);
            tableCm_AngleAlphaMach->AddPrimaryTable(alpha_deg);

            double vehicleYawRate_dps   = 0.0;
            double vehiclePitchRate_dps = 0.0;
            double vehicleRollRate_dps  = 0.0;
            double beta_deg             = 0.0;

            double deltaCL = 0.0;
            double deltaCd = 0.0;
            double deltaCY = 0.0;
            double deltaCm = 0.0;
            double deltaCn = 0.0;
            double deltaCl = 0.0;
            // Sweep the control angle
            for (auto& angle_deg : aAngleDataPts_deg)
            {

               aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                     dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                     vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                                     deltaCL, deltaCd, deltaCY, deltaCm, deltaCn, deltaCl);

               // Make angle entry in tables -- note angle is in degrees
               tableCL_AngleAlphaMach->AddData(angle_deg, deltaCL);
               tableCm_AngleAlphaMach->AddData(angle_deg, deltaCm * mCmMovableMult);

               mCurrentNumberOfAeroTests += 2; // Two tests

            }
         }
      }

      ShowProgress(aProgressDialog);
   }

   // Zero the control angle
   aeroObj->SetControlAngle_deg(0.0);
   aeroObj->CalculateAeroAttitudeVectors();

   tableCL_AngleAlphaMach->UpdateModifiedData();
   tableCm_AngleAlphaMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFMovableAeroDataUsingAngleAndBetaSweep(VehicleAeroMovable&  aAero,
                                                            std::vector<double>& aAngleDataPts_deg,
                                                            QProgressDialog&     aProgressDialog)
{
   AeroObject* aeroObj = aAero.GetAeroObject();

   // Enable the various tables
   aAero.EnableCY_AngleBetaMachTablePtr();
   aAero.EnableCn_AngleBetaMachTablePtr();

   // Get pointers to the various tables
   AeroTable4d* tableCY_AngleBetaMach = aAero.GetCY_AngleBetaMachTablePtr();
   AeroTable4d* tableCn_AngleBetaMach = aAero.GetCn_AngleBetaMachTablePtr();

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   // Mach, beta and angle
   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Make Mach entry in tables
      tableCY_AngleBetaMach->AddSecondaryTable(mach);
      tableCn_AngleBetaMach->AddSecondaryTable(mach);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      for (auto& betaVal : mBetaDataPts_deg)
      {
         if (betaVal >= mMinTableBetaDataPts_deg)
         {
            if (betaVal > mMaxTableBetaDataPts_deg)
            {
               break;
            }

            double beta_deg = static_cast<double>(betaVal);

            // Make Beta entry in tables
            tableCY_AngleBetaMach->AddPrimaryTable(beta_deg);
            tableCn_AngleBetaMach->AddPrimaryTable(beta_deg);

            double vehicleYawRate_dps   = 0.0;
            double vehiclePitchRate_dps = 0.0;
            double vehicleRollRate_dps  = 0.0;
            double alpha_deg            = 0.0;

            double deltaCL = 0.0;
            double deltaCd = 0.0;
            double deltaCY = 0.0;
            double deltaCm = 0.0;
            double deltaCn = 0.0;
            double deltaCl = 0.0;

            // Sweep the control angle
            for (auto& angle_deg : aAngleDataPts_deg)
            {
               aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                           dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                           vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                                           deltaCL, deltaCd, deltaCY, deltaCm, deltaCn, deltaCl);

               // Make angle entry in tables -- note angle is in degrees
               tableCY_AngleBetaMach->AddData(angle_deg, deltaCY);
               tableCn_AngleBetaMach->AddData(angle_deg, deltaCn * mCnMovableMult);

               mCurrentNumberOfAeroTests += 2; // Two tests

            }
         }
      }

      ShowProgress(aProgressDialog);
   }

   // Zero the control angle
   aeroObj->SetControlAngle_deg(0.0);
   aeroObj->CalculateAeroAttitudeVectors();

   tableCY_AngleBetaMach->UpdateModifiedData();
   tableCn_AngleBetaMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFMovableAeroDataUsingAngleSweep(VehicleAeroMovable&  aAero,
                                                     std::vector<double>& aAngleDataPts_deg,
                                                     QProgressDialog&     aProgressDialog)
{
   AeroObject* aeroObj = aAero.GetAeroObject();

   // Enable the various tables
   aAero.EnableCd_AngleMachTablePtr();
   aAero.EnableCmq_AngleMachTablePtr();
   aAero.EnableCnr_AngleMachTablePtr();
   aAero.EnableClp_AngleMachTablePtr();
   aAero.EnableClq_AngleMachTablePtr();
   aAero.EnableClr_AngleMachTablePtr();

   // Get pointers to the various tables
   AeroTable3d* tableCd_AngleMach  = aAero.GetCd_AngleMachTablePtr();
   AeroTable3d* tableCmq_AngleMach = aAero.GetCmq_AngleMachTablePtr();
   AeroTable3d* tableCnr_AngleMach = aAero.GetCnr_AngleMachTablePtr();
   AeroTable3d* tableClp_AngleMach = aAero.GetClp_AngleMachTablePtr();
   AeroTable3d* tableClq_AngleMach = aAero.GetClq_AngleMachTablePtr();
   AeroTable3d* tableClr_AngleMach = aAero.GetClr_AngleMachTablePtr();

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   // Mach and angle only
   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      double mach = 0.01 * static_cast<double>(machVal);

      // Make Mach entry in tables
      tableCd_AngleMach->AddTable(mach);
      tableCmq_AngleMach->AddTable(mach);
      tableCnr_AngleMach->AddTable(mach);
      tableClp_AngleMach->AddTable(mach);
      tableClq_AngleMach->AddTable(mach);
      tableClr_AngleMach->AddTable(mach);

      // Calculate speed, in feet per second
      double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Calculate density, in slugs per cubic foot
      double density_kgm3 = mAtmosphere.Density(altitude_m);
      double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

      double rate_dps = 0.01;
      double rate_rps = rate_dps * UtMath::cRAD_PER_DEG;

      double k = rate_rps / (2 * speed_fps);
      double k_span =  (mUseRefAreaInsteadOfWingArea ? k * mVehicleRefLength_ft : k * mVehicleWingSpan_ft);
      double k_chord = (mUseRefAreaInsteadOfWingArea ? k * mVehicleRefLength_ft : k * mVehicleWingChord_ft);

      // Note: klp = klq = klr = knr = k_span 
      //                         kmq = k_chord

      double alpha_deg = 0.0;
      double beta_deg  = 0.0;
      UtVec3dX totalMomentVec_ftlbs;

      // Delta-coefficients between
      // a)        0 control angle and 0 angular rates
      // b) non-zero control angle and 0 angular rates
      double deltaCL = 0.0;
      double deltaCd = 0.0;
      double deltaCY = 0.0;
      double deltaCm = 0.0;
      double deltaCn = 0.0;
      double deltaCl = 0.0;

      // Delta-coefficients between
      // a)        0 control angle and non-zero roll rate
      // b) non-zero control angle and non-zero roll rate
      double deltaCL_p = 0.0;
      double deltaCd_p = 0.0;
      double deltaCY_p = 0.0;
      double deltaCm_p = 0.0;
      double deltaCn_p = 0.0;
      double deltaCl_p = 0.0;

      // Delta-coefficients between
      // a)        0 control angle and non-zero pitch rate
      // b) non-zero control angle and non-zero pitch rate
      double deltaCL_q = 0.0;
      double deltaCd_q = 0.0;
      double deltaCY_q = 0.0;
      double deltaCm_q = 0.0;
      double deltaCn_q = 0.0;
      double deltaCl_q = 0.0;

      // Delta-coefficients between
      // a)        0 control angle and non-zero yaw rate
      // b) non-zero control angle and non-zero yaw rate
      double deltaCL_r = 0.0;
      double deltaCd_r = 0.0;
      double deltaCY_r = 0.0;
      double deltaCm_r = 0.0;
      double deltaCn_r = 0.0;
      double deltaCl_r = 0.0;

      // Sweep the control angle
      for (auto& angle_deg : aAngleDataPts_deg)
      {
         aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                     dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                     0.0, 0.0, 0.0,
                                     deltaCL, deltaCd, deltaCY, deltaCm, deltaCn, deltaCl);

         // -------- Rate-based derivatives ---------

         aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                     dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                     rate_dps, 0.0, 0.0,
                                     deltaCL_r, deltaCd_r, deltaCY_r, deltaCm_r, deltaCn_r, deltaCl_r);

         aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                     dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                     0.0, rate_dps, 0.0,
                                     deltaCL_q, deltaCd_q, deltaCY_q, deltaCm_q, deltaCn_q, deltaCl_q);

         aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                                     dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                     0.0, 0.0, rate_dps,
                                     deltaCL_p, deltaCd_p, deltaCY_p, deltaCm_p, deltaCn_p, deltaCl_p);

         double dCmq = (deltaCm_q - deltaCm) / k_chord;
         double dCnr = (deltaCn_r - deltaCn) / k_span;
         double dClp = (deltaCl_p - deltaCl) / k_span;
         double dClq = (deltaCl_q - deltaCl) / k_span;
         double dClr = (deltaCl_r - deltaCl) / k_span;


         // Make angle entry in tables -- note angle is in degrees
         tableCd_AngleMach->AddData(angle_deg, deltaCd);
         tableCmq_AngleMach->AddData(angle_deg, dCmq);
         tableCnr_AngleMach->AddData(angle_deg, dCnr);
         tableClp_AngleMach->AddData(angle_deg, dClp);
         tableClq_AngleMach->AddData(angle_deg, dClq);
         tableClr_AngleMach->AddData(angle_deg, dClr);

         mCurrentNumberOfAeroTests += 1; // One test

      }

      ShowProgress(aProgressDialog);
   }

   // Zero the control angle
   aeroObj->SetControlAngle_deg(0.0);
   aeroObj->CalculateAeroAttitudeVectors();

   tableCd_AngleMach->UpdateModifiedData();
   tableCmq_AngleMach->UpdateModifiedData();
   tableCnr_AngleMach->UpdateModifiedData();
   tableClp_AngleMach->UpdateModifiedData();
   tableClq_AngleMach->UpdateModifiedData();
   tableClr_AngleMach->UpdateModifiedData();
}

void VehicleAero::CalcP6DOFMovableAeroDataUsingAngleAndAlphaBetaSweep(VehicleAeroMovable&  aAero,
                                                                 std::vector<double>& aAngleDataPts_deg,
                                                                 double               aMach,
                                                                 QProgressDialog&     aProgressDialog)
{
   AeroObject* aeroObj = aAero.GetAeroObject();

   // Zero the control angle
   aeroObj->SetControlAngle_deg(0.0);
   aeroObj->CalculateAeroAttitudeVectors();

   // Enable the various tables
   aAero.EnableCl_AngleAlphaBetaTablePtr();

   // Get pointers to the various tables
   AeroTable4d* tableCl_AngleAlphaBeta = aAero.GetCl_AngleAlphaBetaTablePtr();

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   // Alpha/Beta but no mach
   for (auto& betaVal : mBetaDataPts_deg)
   {
      if (betaVal >= mMinTableBetaDataPts_deg)
      {
         if (betaVal > mMaxTableBetaDataPts_deg)
         {
            break;
         }

         double beta_deg = static_cast<double>(betaVal);

         // Make beta entry in tables
         tableCl_AngleAlphaBeta->AddSecondaryTable(beta_deg);

         // Calculate speed, in feet per second
         double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, aMach);

         // Calculate speed, in knots (knots true airspeed)
         double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

         // Calculate knots calibrated airspeed
         double kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

         // Calculate dynamic pressure, in lbf per square foot
         double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

         // Calculate density, in slugs per cubic foot
         double density_kgm3 = mAtmosphere.Density(altitude_m);
         double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

         for (auto& alphaVal : mAlphaDataPts_deg)
         {
            if (alphaVal >= mMinTableAlphaDataPts_deg)
            {
               if (alphaVal > mMaxTableAlphaDataPts_deg)
               {
                  break;
               }

               double alpha_deg = static_cast<double>(alphaVal);

               // Make alpha entry in tables
               tableCl_AngleAlphaBeta->AddPrimaryTable(alpha_deg);

               double vehicleYawRate_dps   = 0.0;
               double vehiclePitchRate_dps = 0.0;
               double vehicleRollRate_dps  = 0.0;
               UtVec3dX totalMomentVec_ftlbs;

               double deltaCL = 0.0;
               double deltaCd = 0.0;
               double deltaCY = 0.0;
               double deltaCm = 0.0;
               double deltaCn = 0.0;
               double deltaCl = 0.0;

               // Sweep the control angle
               for (auto& angle_deg : aAngleDataPts_deg)
               {
                  aeroObj->CalcAeroCoeffDelta(aAero.GetName(), angle_deg, mVehicleAltitude_ft, speed_fps, aMach, ktas, kcas,
                                              dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                                              vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                                              deltaCL, deltaCd, deltaCY, deltaCm, deltaCn, deltaCl);

                  // Make angle entry in tables -- note angle is in degrees
                  tableCl_AngleAlphaBeta->AddData(angle_deg, deltaCl * mClMovableMult);

                  mCurrentNumberOfAeroTests += 1; // One test
               }
            }
         }

         ShowProgress(aProgressDialog);
      }
   }

   // Zero the control angle
   aeroObj->SetControlAngle_deg(0.0);
   aeroObj->CalculateAeroAttitudeVectors();

   tableCl_AngleAlphaBeta->UpdateModifiedData();
}

void VehicleAero::CalcGMCoreAeroData(QProgressDialog& aProgressDialog)
{
   ShowProgress(aProgressDialog);

   double vehicleYawRate_dps   = 0.0;
   double vehiclePitchRate_dps = 0.0;
   double vehicleRollRate_dps  = 0.0;
   double mach                 = 0.0;
   double ktas                 = 0.0;
   double kcas                 = 0.0;
   double speed_fps            = 0.0;
   double dynamicPressure_psf  = 0.0;
   double alpha_deg            = 0.0;
   double beta_deg             = 0.0;

   // ------------------------- Calculate Cd vs Mach ------------------------- //

   // Enable the various tables
   mCoreAero.EnableCd_MachTable();

   // Get pointers to the various tables
   AeroTable2d* tableCdVsMach = mCoreAero.GetCd_MachTablePtr();

   double altitude_m = mVehicleAltitude_ft * UtMath::cM_PER_FT;

   // Calculate density, in slugs per cubic foot
   double density_kgm3 = mAtmosphere.Density(altitude_m);
   double density_slugs_ft3 = UtMath::cSLUG_FT3_PER_KG_M3 * density_kgm3;

   for (auto& machVal : mMachDataPts_HundredthsMach)
   {
      if (machVal > mMaxTableMach_HundredthsMach)
      {
         break;
      }

      mach = 0.01 * static_cast<double>(machVal);

      // Calculate speed, in feet per second
      speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

      // Calculate speed, in knots (knots true airspeed)
      ktas = UtMath::cNMPH_PER_FPS * speed_fps;

      // Calculate knots calibrated airspeed
      kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

      // Calculate dynamic pressure, in lbf per square foot
      dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

      // Next, calculate the current conditions
      double CL = 0.0;
      double Cd = 0.0;
      double CY = 0.0;
      double Cl = 0.0;
      double Cm = 0.0;
      double Cn = 0.0;

      double totalLift_lbs = 0.0;
      double totalDrag_lbs = 0.0;
      double totalSide_lbs = 0.0;

      UtVec3dX totalMomentVec_ftlbs;

      CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                           dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                           vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                           totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                           CL, Cd, CY, Cl, Cm, Cn);

      tableCdVsMach->AddData(mach, Cd);

      mCurrentNumberOfAeroTests += 1;

      ShowProgress(aProgressDialog);
   }
   tableCdVsMach->UpdateModifiedData();

   // --------------------------- Calculate CL_max --------------------------- //

   // Use the Testing Mach to calculate CL_max
   mach = mParentVehicle->GetTestingMach();

   // Calculate speed, in feet per second
   speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

   // Calculate speed, in knots (knots true airspeed)
   ktas = UtMath::cNMPH_PER_FPS * speed_fps;

   // Calculate knots calibrated airspeed
   kcas = mAtmosphere.CalcKcasFromFps(altitude_m, speed_fps);

   // Calculate dynamic pressure, in lbf per square foot
   dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

   double CL_max = 0.0;
   double CL_prev = 0.0;
   double Cd_at_CL_max = 0.0;

   for (auto& alphaVal : mAlphaDataPts_deg)
   {
      // For simplicity, we will only look at the positive half of the lift curve
      if (alphaVal >= 0.0)
      {
         alpha_deg = static_cast<double>(alphaVal);

         double totalLift_lbs = 0.0;
         double totalDrag_lbs = 0.0;
         double totalSide_lbs = 0.0;
         UtVec3dX totalMomentVec_ftlbs;

         double CL = 0.0;
         double Cd = 0.0;
         double CY = 0.0;
         double Cl = 0.0;
         double Cm = 0.0;
         double Cn = 0.0;

         CalcAeroCoefficients(mVehicleAltitude_ft, speed_fps, mach, ktas, kcas,
                              dynamicPressure_psf, density_slugs_ft3, alpha_deg, beta_deg,
                              vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps,
                              totalLift_lbs, totalDrag_lbs, totalSide_lbs, totalMomentVec_ftlbs,
                              CL, Cd, CY, Cl, Cm, Cn);

         if (CL > CL_max)
         {
            CL_max = std::abs(CL);
            Cd_at_CL_max = Cd;
         }
         if (CL < CL_prev)
         {
            break;
         }
         CL_prev = CL;
      }
   } // End alpha loop

   mCLMax = CL_max;
   mCdAtCLMax = Cd_at_CL_max;
}

void VehicleAero::CalculateVehicleAspectRatio()
{
   // An aspect ratio at the vehicle level is required for guided movers
   // We will calculate this using the equation Cd = Cd0 + k (CL - CL0)^2
   // Where k = 1 / (pi * e * AR)
   // We will assume that e (the oswalds factor) is the AFSIM default of 0.95
   // Cd0 is the zero-lift coefficient.
   // For Cd, we will use the drag coefficient at the alpha that produces CL_max
   // Note that usually, CL0 will be zero for missiles because of symmetry.
   // For CL, we will use CL_max
   // All of these coefficients will be computed at the user-specified testing mach of the vehicle
   // Source: Introduction to Flight. Anderson, John D. Fifth Edition. McGraw-Hill

   // From the above equations, AR can be computed as:
   // AR = (CL - CL0)^2 / ((Cd - Cd0)*pi*e)

   double oswalds_factor = 0.95; // "e" in the above equations

   // Get CL vs alpha at beta = 0 and Mach = Testing Mach
   double mach = mParentVehicle->GetTestingMach();

   // Find the lift at the alpha that produces minimum drag (this is CL0)
   // For now, assume this is 0
   double CL0 = 0.0;

   // Get Cd at the alpha we just determined, and Mach = Testing Mach (this is Cd0)
   double Cd0 = 0.0;
   mCoreAero.GetCd_MachTablePtr()->GetInterpolatedDependentValueModified(mach, Cd0);

   // AR = (CL - CL0)^2 / ((Cd - Cd0)*pi*e)
   mAspectRatio = ((mCLMax - CL0) * (mCLMax - CL0)) / ((mCdAtCLMax - Cd0) * UtMath::cPI * oswalds_factor);
   ut::log::info() << "mCLMax = " << mCLMax;
   ut::log::info() << "mCdAtCLMax = " << mCdAtCLMax;
   ut::log::info() << "mAspectRatio = " << mAspectRatio;
}

void VehicleAero::CalcVehicleAspectRatio(QProgressDialog& aProgressDialog)
{
   ShowProgress(aProgressDialog);

   CalculateVehicleAspectRatio();
}

void VehicleAero::CalcAeroCoefficients(double    aAltitude_ft,
                                       double    aSpeed_fps,
                                       double    aMach,
                                       double    aKTAS,
                                       double    aKCAS,
                                       double    aDynamicPressure_psf,
                                       double    aDensity_slugs_ft3,
                                       double    aVehicleAlpha_deg,
                                       double    aVehicleBeta_deg,
                                       double    aVehicleYawRate_dps,
                                       double    aVehiclePitchRate_dps,
                                       double    aVehicleRollRate_dps,
                                       double&   aTotalLift_lbs,
                                       double&   aTotalDrag_lbs,
                                       double&   aTotalSide_lbs,
                                       UtVec3dX& aTotalMomentVector_ftlbs,
                                       double&   aCL,
                                       double&   aCd,
                                       double&   aCY,
                                       double&   aCl,
                                       double&   aCm,
                                       double&   aCn)
{
   CalcAeroData(aAltitude_ft, aSpeed_fps, aMach, aKTAS, aKCAS,
                aDynamicPressure_psf, aDensity_slugs_ft3, aVehicleAlpha_deg, aVehicleBeta_deg,
                aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps,
                aTotalLift_lbs, aTotalDrag_lbs, aTotalSide_lbs, aTotalMomentVector_ftlbs);

   aCL = aTotalLift_lbs / aDynamicPressure_psf;
   aCd = aTotalDrag_lbs / aDynamicPressure_psf;
   aCY = aTotalSide_lbs / aDynamicPressure_psf;
   aCl = aTotalMomentVector_ftlbs.X() / aDynamicPressure_psf;
   aCm = aTotalMomentVector_ftlbs.Y() / aDynamicPressure_psf;
   aCn = aTotalMomentVector_ftlbs.Z() / aDynamicPressure_psf;

   if (mUseRefAreaInsteadOfWingArea)
   {
      if (mVehicleRefArea_ft2 > 0)
      {
         aCL /= mVehicleRefArea_ft2;
         aCd /= mVehicleRefArea_ft2;
         aCY /= mVehicleRefArea_ft2;
         aCl /= mVehicleRefArea_ft2;
         aCm /= mVehicleRefArea_ft2;
         aCn /= mVehicleRefArea_ft2;
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle has a zero reference area!");
      }
   }
   else
   {
      if (mVehicleWingArea_ft2 > 0)
      {
         aCL /= mVehicleWingArea_ft2;
         aCd /= mVehicleWingArea_ft2;
         aCY /= mVehicleWingArea_ft2;
         aCl /= (mVehicleWingArea_ft2 * mVehicleWingSpan_ft);
         aCm /= (mVehicleWingArea_ft2 * mVehicleWingChord_ft);
         aCn /= (mVehicleWingArea_ft2 * mVehicleWingSpan_ft);
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle has a zero wing area!");
      }
   }
}

int VehicleAero::CalcNumberOfAeroTests()
{
   int totalNumTests = 0;

   // Calculate number of tests for Core Aero

   int numTestsAlphaBetaMach = CalcBaseNumberOfAeroTests(true, true);
   int numTestsBetaMach      = CalcBaseNumberOfAeroTests(false, true);
   int numTestsOnlyMach      = CalcBaseNumberOfAeroTests(false, false);

   bool enableP6DOF = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GenerateP6DOFMover();
   bool enableRB6DOF = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GenerateRB6DOFMover();
   bool enableGM = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GenerateGuidedMover();

   if (enableP6DOF || enableRB6DOF)
   {
      // CL_AlphaBetaMach
      totalNumTests += numTestsAlphaBetaMach;

      // GetCLq_AlphaMachTablePtr        in CalcCoreAeroDataUsingAlphaAndPitchRateSweep

      // CLq_AlphaMach
      totalNumTests += CalcBaseNumberOfAeroTests(true, false);

      // CL_AlphaDotAlphaMach
      totalNumTests += CalcBaseNumberOfAeroTests(true, false);

      // Cd_AlphaBetaMach
      totalNumTests += numTestsAlphaBetaMach;

      // CY_AlphaBetaMach
      totalNumTests += numTestsAlphaBetaMach;

      // CYr_BetaMach
      totalNumTests += numTestsBetaMach;

      // CY_BetaDotBetaMach
      totalNumTests += numTestsBetaMach;

      // Cm_AlphaBetaMach
      totalNumTests += numTestsAlphaBetaMach;

      // Cmq_Mach
      totalNumTests += numTestsOnlyMach;

      // Cmp_Mach
      totalNumTests += numTestsOnlyMach;

      // Cm_AlphaDotMach
      totalNumTests += numTestsOnlyMach;

      // Cn_AlphaBetaMach
      totalNumTests += numTestsAlphaBetaMach;

      // Cn_BetaDotMach
      totalNumTests += numTestsOnlyMach;

      // Cnr_Mach
      totalNumTests += numTestsOnlyMach;

      // Cnp_Mach
      totalNumTests += numTestsOnlyMach;

      // Cl_AlphaBetaMach
      totalNumTests += numTestsAlphaBetaMach;

      // Clp_Mach
      totalNumTests += numTestsOnlyMach;

      // Cl_AlphaDotMach
      totalNumTests += numTestsOnlyMach;

      // Cl_BetaDotMach
      totalNumTests += numTestsOnlyMach;

      // Clr_Mach
      totalNumTests += numTestsOnlyMach;

      // Clq_Mach
      totalNumTests += numTestsOnlyMach;

   }

   if (enableGM)
   {
      // Cd_Mach
      totalNumTests += numTestsOnlyMach;
   }

   int numBaseAeroTests = totalNumTests;
   auto outStream = ut::log::info() << "Aerodynamic tests calculation summary for " << mParentVehicle->GetVehicleName() << ":";

   outStream.AddNote() << "BaseAero: " << numBaseAeroTests << " tests";

   if (enableP6DOF || enableRB6DOF)
   {
      // Calculate number of tests for Movable Aero
      for (auto& movableAero : mMovableAeroList)
      {
         AeroObject* aeroObj = movableAero.GetAeroObject();
         if (aeroObj != nullptr)
         {
            std::string typeName = aeroObj->AeroObjType();

            if ((typeName == "AeroFullFlyingSurface") || (typeName == "AeroHingedSurface") || (typeName == "AeroLandingGear") || (typeName == "AeroSpeedBrake") || (typeName == "AeroWing"))
            {
               std::vector<double> angleDataPts_deg;
               bool valid = CalculateVectorOfAngles(movableAero, angleDataPts_deg);
               if (valid)
               {
                  int numAnglePts = static_cast<int>(angleDataPts_deg.size());
                  int numTests    = 0;

                  // CL_AngleAlphaMach
                  numTests += CalcNumberOfMovableAeroTests(true, true, false, numAnglePts);

                  // Cd_AngleMach
                  numTests += CalcNumberOfMovableAeroTests(true, false, false, numAnglePts);

                  // CY_AngleBetaMach
                  numTests += CalcNumberOfMovableAeroTests(true, false, true, numAnglePts);

                  // Cm_AngleAlphaMach
                  numTests += CalcNumberOfMovableAeroTests(true, true, false, numAnglePts);

                  // Cn_AngleBetaMach
                  numTests += CalcNumberOfMovableAeroTests(true, false, true, numAnglePts);

                  // Cl_AngleAlphaBeta
                  numTests += CalcNumberOfMovableAeroTests(false, true, true, numAnglePts);

                  totalNumTests += numTests;

                  outStream.AddNote() << aeroObj->GetName() << ": " << numAnglePts << " angles and " << numTests << " tests";
               }
            }
         }
      }
   }
   outStream.AddNote() << "Total: " << totalNumTests << " tests";

   return totalNumTests;
}

int VehicleAero::CalcBaseNumberOfAeroTests(bool aUseAlphaSweep, bool aUseBetaSweep, bool aUseMachSweep)
{
   // If aUseMachSweep is true, count all mach values less than the max; else the count is 0.
   int numberOfMachTests  = aUseMachSweep ? std::count_if(mMachDataPts_HundredthsMach.begin(), mMachDataPts_HundredthsMach.end(), 
                            [this](int mach) {return mach <= mMaxTableMach_HundredthsMach; }) : 0;

   // If aUseAlphaSweep is true, count all alpha values in the range [mMinTableAlphaDataPts_deg, mMaxTableAlphaDataPts_deg];
   // else the count is 0.
   int numberOfAlphaTests = aUseAlphaSweep ? std::count_if(mAlphaDataPts_deg.begin(), mAlphaDataPts_deg.end(), 
                            [this](double alpha) {return alpha >= mMinTableAlphaDataPts_deg && alpha <= mMaxTableAlphaDataPts_deg; }) : 0;

   // If aUseBetaSweep is true, count all beta values in the range [mMinTableBetaDataPts_deg, mMaxTableBetaDataPts_deg];
   // else the count is 0.
   int numberOfBetaTests = aUseBetaSweep ? std::count_if(mBetaDataPts_deg.begin(), mBetaDataPts_deg.end(),
                            [this](double beta) {return beta >= mMinTableBetaDataPts_deg && beta <= mMaxTableBetaDataPts_deg; }) : 0;

   int numberOfTests = 0;

   if (numberOfMachTests > 0)
   {
      if (numberOfAlphaTests > 0)
      {
         if (numberOfBetaTests > 0)
         {
            numberOfTests = numberOfMachTests * numberOfAlphaTests * numberOfBetaTests;
         }
         else
         {
            numberOfTests = numberOfMachTests * numberOfAlphaTests;
         }
      }
      else
      {
         if (numberOfBetaTests > 0)
         {
            numberOfTests = numberOfMachTests * numberOfBetaTests;
         }
         else
         {
            numberOfTests = numberOfMachTests;
         }
      }
   }
   else
   {
      if (numberOfAlphaTests > 0)
      {
         if (numberOfBetaTests > 0)
         {
            numberOfTests = numberOfAlphaTests * numberOfBetaTests;
         }
         else
         {
            numberOfTests = numberOfAlphaTests;
         }
      }
      else
      {
         if (numberOfBetaTests > 0)
         {
            numberOfTests = numberOfBetaTests;
         }
         else
         {
            numberOfTests = 0;
         }
      }
   }

   return numberOfTests;
}

int VehicleAero::CalcNumberOfMovableAeroTests(bool aUseMachSweep,
                                              bool aUseAlphaSweep,
                                              bool aUseBetaSweep,
                                              int  aNumberOfAngleTests)
{
   int numberOfTests = CalcBaseNumberOfAeroTests(aUseAlphaSweep, aUseBetaSweep, aUseMachSweep);

   if (aNumberOfAngleTests > 0)
   {
      if (numberOfTests > 0)
      {
         numberOfTests *= aNumberOfAngleTests;
      }
      else
      {
         numberOfTests = aNumberOfAngleTests;
      }
   }

   return numberOfTests;
}

bool VehicleAero::CalculateVectorOfControlSurfaceAngles(double aMinAngle_deg, double aMaxAngle_deg, std::vector<double>& aAngleDataPts_deg)
{
   // Ensure that the vector is clear, so it can be populated
   aAngleDataPts_deg.clear();

   // First do the negative side to zero (inclusive)
   bool firstTime = true;
   for (std::vector<int>::reverse_iterator iter = mBaseControlSurfaceDataPts_deg.rbegin(); iter != mBaseControlSurfaceDataPts_deg.rend(); ++iter)
   {
      int negVal = -(*iter);
      double val = static_cast<double>(negVal);
      if (aMinAngle_deg < val)
      {
         if (firstTime)
         {
            aAngleDataPts_deg.push_back(aMinAngle_deg);
            firstTime = false;
         }
         else
         {
            aAngleDataPts_deg.push_back(val);
         }
      }
   }

   // Now, do the positive side greater than zero
   for (std::vector<int>::iterator iter = mBaseControlSurfaceDataPts_deg.begin(); iter != mBaseControlSurfaceDataPts_deg.end(); ++iter)
   {
      int posVal = *iter;
      double val = static_cast<double>(posVal);
      if ((aMaxAngle_deg < val) && (posVal > 0))
      {
         aAngleDataPts_deg.push_back(aMaxAngle_deg);
         break;
      }
      else
      {
         if ((val < aMaxAngle_deg) && (posVal > 0))
         {
            aAngleDataPts_deg.push_back(val);
         }
      }
   }

   if (aAngleDataPts_deg.size() >= 2)
   {
      return true;
   }

   return false;
}



bool VehicleAero::CalculateVectorOfAngles(VehicleAeroMovable&  aAero,
                                          std::vector<double>& aAngleDataPts_deg)
{
   // Ensure that the vector is clear, so it can be populated
   aAngleDataPts_deg.clear();

   AeroObject* aeroObj = aAero.GetAeroObject();
   if (aeroObj != nullptr)
   {
      std::string typeName = aeroObj->AeroObjType();
      if (typeName == "AeroWing")
      {
         AeroWing* wing = dynamic_cast<AeroWing*>(aeroObj);
         if (wing != nullptr)
         {
            if (aAero.GetName() == "Aileron_Right" || aAero.GetName() == "Aileron_Left")
            {
               double minAngle_deg = wing->GetAilerons()->GetControlSurfaceMinAngle_deg();
               double maxAngle_deg = wing->GetAilerons()->GetControlSurfaceMaxAngle_deg();
               return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
            }
            else if (aAero.GetName() == "Spoileron_Right" || aAero.GetName() == "Spoileron_Left")
            {
               double minAngle_deg = wing->GetSpoilerons()->GetControlSurfaceMinAngle_deg();
               double maxAngle_deg = wing->GetSpoilerons()->GetControlSurfaceMaxAngle_deg();
               return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
            }
            else if (aAero.GetName() == "Spoiler_Right" || aAero.GetName() == "Spoiler_Left")
            {
               double minAngle_deg = wing->GetSpoilers()->GetControlSurfaceMinAngle_deg();
               double maxAngle_deg = wing->GetSpoilers()->GetControlSurfaceMaxAngle_deg();
               return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
            }
            else if (aAero.GetName() == "Drageron_Right_Top" || aAero.GetName() == "Drageron_Left_Top")
            {
               double minAngle_deg = wing->GetDrageronsTop()->GetControlSurfaceMinAngle_deg();
               double maxAngle_deg = wing->GetDrageronsTop()->GetControlSurfaceMaxAngle_deg();
               return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
            }
            else if (aAero.GetName() == "Drageron_Right_Bottom" || aAero.GetName() == "Drageron_Left_Bottom")
            {
               double minAngle_deg = wing->GetDrageronsBottom()->GetControlSurfaceMinAngle_deg();
               double maxAngle_deg = wing->GetDrageronsBottom()->GetControlSurfaceMaxAngle_deg();
               return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
            }
            if (aAero.GetName() == "Elevon_Right" || aAero.GetName() == "Elevon_Left")
            {
               double minAngle_deg = wing->GetElevons()->GetControlSurfaceMinAngle_deg();
               double maxAngle_deg = wing->GetElevons()->GetControlSurfaceMaxAngle_deg();
               return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
            }
         }
      }
      if (typeName == "AeroFullFlyingSurface")
      {
         AeroFullFlyingSurface* object = static_cast<AeroFullFlyingSurface*>(aeroObj);

            // We have a movable surface (either eControlMotionControlSurface or
            // eControlMotionFullFlying), so we add it to the list

         double minAngle_deg = object->GetControlSurfaceMinAngle_deg();
         double maxAngle_deg = object->GetControlSurfaceMaxAngle_deg();

         return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
      }
      else if (typeName == "AeroHingedSurface")
      {
         AeroHingedSurface* object = static_cast<AeroHingedSurface*>(aeroObj);

         // We have a movable surface (either eControlMotionControlSurface or
         // eControlMotionFullFlying), so we add it to the list

         double minAngle_deg = object->GetControlSurfaceMinAngle_deg();
         double maxAngle_deg = object->GetControlSurfaceMaxAngle_deg();

         return CalculateVectorOfControlSurfaceAngles(minAngle_deg, maxAngle_deg, aAngleDataPts_deg);
      }
      else if (typeName == "AeroLandingGear")
      {
         AeroLandingGear* object = static_cast<AeroLandingGear*>(aeroObj);

         // Landing gear are assumed to be retractable/extendable (as opposed to fixed),
         // so we add it to the list

         double maxAngle_deg = object->GetMaxAngle_deg();

         // Use 4 steps for landing gear (does not need to be very precise)
         double deltaAngle_deg = maxAngle_deg / 4.0;

         // Landing gear always start at zero and move to the maximum angle (typically 90 deg)
         for (double angle_deg = 0.0; angle_deg <= maxAngle_deg; angle_deg += deltaAngle_deg)
         {
            aAngleDataPts_deg.push_back(angle_deg);
         }

         return true;
      }
      else if (typeName == "AeroSpeedBrake")
      {
         AeroSpeedBrake* object = static_cast<AeroSpeedBrake*>(aeroObj);

         // Speed brakes are assumed to be retractable/extendable (as opposed to fixed),
         // so we add it to the list

         double maxAngle_deg = object->GetMaxAngle_deg();

         // Use 10 steps for flaps
         double deltaAngle_deg = maxAngle_deg / 10.0;

         // Speed brakes always start at zero and move to the maximum angle
         for (double angle_deg = 0.0; angle_deg <= maxAngle_deg; angle_deg += deltaAngle_deg)
         {
            aAngleDataPts_deg.push_back(angle_deg);
         }

         return true;
      }

      // Note that wings and other aero object types will return false (below)
   }

   return false;
}

bool VehicleAero::CalculateWingVectorOfAngles(VehicleAeroMovable&  aAero,
                                              std::vector<double>& aAileronAngleDataPts_deg,
                                              std::vector<double>& aFlapsAngleDataPts_deg,
                                              std::vector<double>& aLeadingEdgeFlapsAngleDataPts_deg)
{
   // Ensure that the vectors are clear, so they can be populated
   aAileronAngleDataPts_deg.clear();
   aFlapsAngleDataPts_deg.clear();
   aLeadingEdgeFlapsAngleDataPts_deg.clear();

   /* TODO -- FUTURE_IMPROVEMENT_NOTE
   AeroObject* aeroObj = aAero.GetAeroObject();
   if (aeroObj != nullptr)
   {
      std::string typeName = aeroObj->AeroObjType();

      if (typeName == "AeroWing")
      {
         AeroWing* object = static_cast<AeroWing*>(aeroObj);

         // double AileronMinAngle_deg = object->GetAileronMinAngle_deg();
         // double AileronMaxAngle_deg = object->GetAileronMaxAngle_deg();
         // bool aileronResult = CalculateVectorOfControlSurfaceAngles(AileronMinAngle_deg, AileronMaxAngle_deg, aAileronAngleDataPts_deg);
         //
         // double flapsMinAngle_deg = object->GetFlapMinAngle_deg();
         // double flapsMaxAngle_deg = object->GetFlapMaxAngle_deg();
         // bool flapsResult = CalculateVectorOfControlSurfaceAngles(flapsMinAngle_deg, flapsMaxAngle_deg, aFlapsAngleDataPts_deg);
         //
         // double leadingEdgeFlapsMinAngle_deg = object->GetLeadingEdgeFlapMinAngle_deg();
         // double leadingEdgeFlapsMaxAngle_deg = object->GetLeadingEdgeFlapMaxAngle_deg();
         // bool flapsResult = CalculateVectorOfControlSurfaceAngles(leadingEdgeFlapsMinAngle_deg, leadingEdgeFlapsMaxAngle_deg, aLeadingEdgeFlapsAngleDataPts_deg);

         // return true;
      }

      // Note that other non-wing aero objects (include surfaces) types will return false (below)
   }
   */

   return false;
}

void VehicleAero::OutputInterpolatedTestDataForAlphaBetaMach(AeroTable4d* aTable,
                                                             FILE*        aFile,
                                                             double       aMach,
                                                             int          aAlphaMin_deg,
                                                             int          aAlphaMax_deg,
                                                             int          aAlphaStep_deg,
                                                             int          aBetaMin_deg,
                                                             int          aBetaMax_deg,
                                                             int          aBetaStep_deg)
{
   if (aFile == nullptr) { return; }

   double value = 0.0;
   fprintf(aFile, "Alpha (deg)");

   for (int ibeta_deg = aBetaMin_deg; ibeta_deg <= aBetaMax_deg; ibeta_deg += aBetaStep_deg)
   {
      fprintf(aFile, ", CL at Beta (%4d deg)", ibeta_deg);
   }
   fprintf(aFile, "\n");

   for (int ialpha_deg = aAlphaMin_deg; ialpha_deg <= aAlphaMax_deg; ialpha_deg += aAlphaStep_deg)
   {
      double alpha_deg = (double)ialpha_deg;
      fprintf(aFile, "%.10lf", alpha_deg);
      for (int ibeta_deg = aBetaMin_deg; ibeta_deg <= aBetaMax_deg; ibeta_deg += aBetaStep_deg)
      {
         double alpha_rad = (double)ialpha_deg * UtMath::cRAD_PER_DEG;
         double beta_rad = (double)ibeta_deg * UtMath::cRAD_PER_DEG;

         aTable->GetInterpolatedDependentValueBase(alpha_rad, beta_rad, aMach, value);
         fprintf(aFile, ", %.10lf", value);
      }
      fprintf(aFile, "\n");
   }

   fprintf(aFile, "\n");
}

void VehicleAero::OutputRawNearestTestDataForAlphaBetaMach(AeroTable4d* aTable,
                                                           FILE*        aFile,
                                                           double       aMach,
                                                           int          aBetaMin_deg,
                                                           int          aBetaMax_deg,
                                                           int          aBetaStep_deg)
{
   if (aFile == nullptr) { return; }

   std::vector<std::pair<double, double>> tempDataSet;
   std::vector<std::vector<std::pair<double, double>>> betaDataSetVector;

   fprintf(aFile, "Alpha");
   for (int iBeta_deg = aBetaMin_deg; iBeta_deg <= aBetaMax_deg; iBeta_deg += aBetaStep_deg)
   {
      double beta_rad = UtMath::cRAD_PER_DEG * (double)iBeta_deg;

      tempDataSet.clear();

      bool dataValid = aTable->GetPrimaryBaseDataNearest(beta_rad, aMach, tempDataSet);
      if (dataValid)
      {
         betaDataSetVector.push_back(tempDataSet);
      }

      fprintf(aFile, ", %d", iBeta_deg);
   }
   fprintf(aFile, "\n");

   int numAlphaPts = (int)betaDataSetVector.at(0).size();
   for (int iAlpha = 0; iAlpha < numAlphaPts; ++iAlpha)
   {
      double alpha_deg = betaDataSetVector.at(0).at(iAlpha).first * UtMath::cDEG_PER_RAD;
      fprintf(aFile, "%.10lf", alpha_deg);

      for (auto& item : betaDataSetVector)
      {
         double value = item.at(iAlpha).second;
         fprintf(aFile, ", %.10lf", value);
      }
      fprintf(aFile, "\n");
   }

   fprintf(aFile, "\n");
}

void VehicleAero::OutputInterpolatedTestDataFor3dTableMach(AeroTable3d* aTable,
                                                           FILE*        aFile,
                                                           double       aMach,
                                                           int          aPrimaryMin,
                                                           int          aPrimaryMax,
                                                           int          aPrimaryStep)
{
   if (aFile == nullptr) { return; }

   double value = 0.0;

   for (int iPrimary_deg = aPrimaryMin; iPrimary_deg <= aPrimaryMax; iPrimary_deg += aPrimaryStep)
   {
      double primary_deg = (double)iPrimary_deg;

      double primary_rad = (double)iPrimary_deg * UtMath::cRAD_PER_DEG;

      aTable->GetInterpolatedDependentValueBase(primary_rad, aMach, value);
      fprintf(aFile, "%.10f, %.10lf\n", primary_deg, value);
   }

   fprintf(aFile, "\n");
}

void VehicleAero::OutputRawNearestTestDataFor3dTableMach(AeroTable3d* aTable,
                                                         FILE*        aFile,
                                                         double       aMach)
{
   if (aFile == nullptr) { return; }

   std::vector<std::pair<double, double>> tempDataSet;

   tempDataSet.clear();

   bool dataValid = aTable->GetPrimaryBaseDataNearest(aMach, tempDataSet);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : tempDataSet)
   {
      double alpha_deg = item.first * UtMath::cDEG_PER_RAD;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", alpha_deg, value);
   }

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCL_AlphaBetaMach(FILE* aFile,
                                                            double aMach,
                                                            int    aAlphaMin_deg,
                                                            int    aAlphaMax_deg,
                                                            int    aAlphaStep_deg,
                                                            int    aBetaMin_deg,
                                                            int    aBetaMax_deg,
                                                            int    aBetaStep_deg)
{
   AeroTable4d* table = mCoreAero.GetCL_AlphaBetaMachTablePtr();

   OutputInterpolatedTestDataForAlphaBetaMach(table,
                                              aFile,
                                              aMach,
                                              aAlphaMin_deg,
                                              aAlphaMax_deg,
                                              aAlphaStep_deg,
                                              aBetaMin_deg,
                                              aBetaMax_deg,
                                              aBetaStep_deg);

   fprintf(aFile, "\n");
   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCLq_AlphaMach(FILE*  aFile,
                                                         double aMach,
                                                         int    alphaMin_deg,
                                                         int    alphaMax_deg,
                                                         int    alphaStep_deg)
{
   AeroTable3d* table = mCoreAero.GetCLq_AlphaMachTablePtr();

   OutputInterpolatedTestDataFor3dTableMach(table, aFile, aMach, alphaMin_deg, alphaMax_deg, alphaStep_deg);
   fprintf(aFile, "\n");
   OutputRawNearestTestDataFor3dTableMach(table, aFile, aMach);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCL_AlphaDotAlphaMach(FILE*  aFile,
                                                                double aMach,
                                                                int    alphaMin_deg,
                                                                int    alphaMax_deg,
                                                                int    alphaStep_deg)
{
   AeroTable3d* table = mCoreAero.GetCL_AlphaDotAlphaMachTablePtr();

   OutputInterpolatedTestDataFor3dTableMach(table, aFile, aMach, alphaMin_deg, alphaMax_deg, alphaStep_deg);
   fprintf(aFile, "\n");
   OutputRawNearestTestDataFor3dTableMach(table, aFile, aMach);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCd_AlphaBetaMach(FILE*  aFile,
                                                            double aMach,
                                                            int    aAlphaMin_deg,
                                                            int    aAlphaMax_deg,
                                                            int    aAlphaStep_deg,
                                                            int    aBetaMin_deg,
                                                            int    aBetaMax_deg,
                                                            int    aBetaStep_deg)
{
   AeroTable4d* table = mCoreAero.GetCd_AlphaBetaMachTablePtr();

   OutputInterpolatedTestDataForAlphaBetaMach(table,
                                              aFile,
                                              aMach,
                                              aAlphaMin_deg,
                                              aAlphaMax_deg,
                                              aAlphaStep_deg,
                                              aBetaMin_deg,
                                              aBetaMax_deg,
                                              aBetaStep_deg);

   fprintf(aFile, "\n");
   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCY_AlphaBetaMach(FILE*  aFile,
                                                            double aMach,
                                                            int    aAlphaMin_deg,
                                                            int    aAlphaMax_deg,
                                                            int    aAlphaStep_deg,
                                                            int    aBetaMin_deg,
                                                            int    aBetaMax_deg,
                                                            int    aBetaStep_deg)
{
   AeroTable4d* table = mCoreAero.GetCY_AlphaBetaMachTablePtr();

   OutputInterpolatedTestDataForAlphaBetaMach(table,
                                              aFile,
                                              aMach,
                                              aAlphaMin_deg,
                                              aAlphaMax_deg,
                                              aAlphaStep_deg,
                                              aBetaMin_deg,
                                              aBetaMax_deg,
                                              aBetaStep_deg);

   fprintf(aFile, "\n");
   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCYr_BetaMach(FILE*  aFile,
                                                        double aMach,
                                                        int    aBetaMin_deg,
                                                        int    aBetaMax_deg,
                                                        int    aBetaStep_deg)
{
   AeroTable3d* table = mCoreAero.GetCYr_BetaMachTablePtr();

   OutputInterpolatedTestDataFor3dTableMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);
   fprintf(aFile, "\n");
   OutputRawNearestTestDataFor3dTableMach(table, aFile, aMach);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCY_BetaDotBetaMach(FILE*  aFile,
                                                              double aMach,
                                                              int    aBetaMin_deg,
                                                              int    aBetaMax_deg,
                                                              int    aBetaStep_deg)
{
   AeroTable3d* table = mCoreAero.GetCY_BetaDotBetaMachTablePtr();

   OutputInterpolatedTestDataFor3dTableMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);
   fprintf(aFile, "\n");
   OutputRawNearestTestDataFor3dTableMach(table, aFile, aMach);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCm_AlphaBetaMach(FILE*  aFile,
                                                            double aMach,
                                                            int    aAlphaMin_deg,
                                                            int    aAlphaMax_deg,
                                                            int    aAlphaStep_deg,
                                                            int    aBetaMin_deg,
                                                            int    aBetaMax_deg,
                                                            int    aBetaStep_deg)
{
   AeroTable4d* table = mCoreAero.GetCm_AlphaBetaMachTablePtr();

   OutputInterpolatedTestDataForAlphaBetaMach(table,
                                              aFile,
                                              aMach,
                                              aAlphaMin_deg,
                                              aAlphaMax_deg,
                                              aAlphaStep_deg,
                                              aBetaMin_deg,
                                              aBetaMax_deg,
                                              aBetaStep_deg);
   fprintf(aFile, "\n");
   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);
   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCmq_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCmq_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1 ; mach < 3.0 ; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCmp_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCmp_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.02)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCm_AlphaDotMach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCm_AlphaDotMachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCn_AlphaBetaMach(FILE*  aFile,
                                                            double aMach,
                                                            int    aAlphaMin_deg,
                                                            int    aAlphaMax_deg,
                                                            int    aAlphaStep_deg,
                                                            int    aBetaMin_deg,
                                                            int    aBetaMax_deg,
                                                            int    aBetaStep_deg)
{
   AeroTable4d* table = mCoreAero.GetCn_AlphaBetaMachTablePtr();

   OutputInterpolatedTestDataForAlphaBetaMach(table,
                                              aFile,
                                              aMach,
                                              aAlphaMin_deg,
                                              aAlphaMax_deg,
                                              aAlphaStep_deg,
                                              aBetaMin_deg,
                                              aBetaMax_deg,
                                              aBetaStep_deg);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, -80, 80, 10);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, -80, 0, 10);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, 0, 80, 10);

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCn_BetaDotMach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCn_BetaDotMachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCnr_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCnr_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10f\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCnp_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCnp_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCl_AlphaBetaMach(FILE*  aFile,
                                                            double aMach,
                                                            int    aAlphaMin_deg,
                                                            int    aAlphaMax_deg,
                                                            int    aAlphaStep_deg,
                                                            int    aBetaMin_deg,
                                                            int    aBetaMax_deg,
                                                            int    aBetaStep_deg)
{
   AeroTable4d* table = mCoreAero.GetCl_AlphaBetaMachTablePtr();

   OutputInterpolatedTestDataForAlphaBetaMach(table,
                                              aFile,
                                              aMach,
                                              aAlphaMin_deg,
                                              aAlphaMax_deg,
                                              aAlphaStep_deg,
                                              aBetaMin_deg,
                                              aBetaMax_deg,
                                              aBetaStep_deg);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, aBetaMin_deg, aBetaMax_deg, aBetaStep_deg);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, -80, 80, 10);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, -80, 0, 10);

   fprintf(aFile, "\n");

   OutputRawNearestTestDataForAlphaBetaMach(table, aFile, aMach, 0, 80, 10);

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroClp_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetClp_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroClr_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetClr_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroClq_Mach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetClq_MachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCl_AlphaDotMach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCl_AlphaDotMachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

void VehicleAero::OutputTestDataForBaseAeroCl_BetaDotMach(FILE* aFile)
{
   if (aFile == nullptr) { return; }

   AeroTable2d* table = mCoreAero.GetCl_BetaDotMachTablePtr();

   std::vector<std::pair<double, double>> dataset;

   for (double mach = 0.1; mach < 3.0; mach += 0.1)
   {
      double value = 0.0;
      table->GetInterpolatedDependentValueBase(mach, value);
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   dataset.clear();
   bool dataValid = table->GetBaseData(dataset);
   if (!dataValid)
   {
      return;
   }

   for (auto& item : dataset)
   {
      double mach = item.first;
      double value = item.second;
      fprintf(aFile, "%.10lf, %.10lf\n", mach, value);
   }
   fprintf(aFile, "\n");

   fprintf(aFile, "\n");
}

FILE* VehicleAero::OpenFileForOutput(std::string aFilename)
{
   FILE* fOutput = fopen(aFilename.c_str(), "w");
   if (fOutput == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Unable to write to " + QString::fromStdString(aFilename) + "!");
   }
   return fOutput;
}

void VehicleAero::ApplyAllModifiers()
{
   try
   {
      std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
      if (curDoc)
      {
         QJsonObject mainObject = curDoc->object();
         if (mainObject.contains("modified_aero"))
         {
            QJsonObject modifiers = mainObject["modified_aero"].toObject();
            ApplyCoreAeroModifiers(modifiers);
            ApplyMovableAeroModifiers(modifiers);
         }
      }
   }
   catch (std::runtime_error& err)
   {
      QMessageBox::critical(nullptr, QString("Input File Error"), QString(err.what()), QMessageBox::Ok);
      throw;
   }
}

void VehicleAero::ApplyCoreAeroModifiers(const QJsonObject& aModifiers)
{
   auto coreIter = aModifiers.constFind("Core Aero");
   if (coreIter != aModifiers.constEnd())
   {
      QJsonObject coreMods = coreIter->toObject();
      // Loop over Core Aero tables with mods.
      for (auto table = coreMods.constBegin(); table != coreMods.constEnd(); ++table)
      {
         QString tableName = table.key();
         AeroTableBase* tablePtr = GetCoreAero().GetTableByName(tableName);
         if (tablePtr)
         {
            QJsonObject tableModification = table->toObject();
            tablePtr->ApplyModifier(tableModification);
            tablePtr->UpdateModifiedData();
         }
      }
   }
}

void VehicleAero::ApplyMovableAeroModifiers(const QJsonObject& aModifiers)
{
   for (auto component = aModifiers.constBegin(); component != aModifiers.constEnd(); ++component)
   {
      std::string componentName = component.key().toStdString();
      VehicleAeroMovable* movingComponent = GetMovableAero(componentName);
      if (movingComponent)
      {
         QJsonObject mods = component->toObject();
         for (auto table = mods.constBegin(); table != mods.constEnd(); ++table)
         {
            QString tableName = table.key();
            AeroTableBase* tablePtr = movingComponent->GetTableByName(tableName);
            if (tablePtr)
            {
               QJsonObject tableModification = table->toObject();
               tablePtr->ApplyModifier(tableModification);
               tablePtr->UpdateModifiedData();
            }
         }
      }
   }
}

void Designer::VehicleAero::SetAeroAlphaBetaSymmetry(bool aAlphaSymmetry, bool aBetaSymmetry)
{
   mBetaSymmetry = aBetaSymmetry;
   mAlphaSymmetry = aAlphaSymmetry;
}

} // namespace Designer
