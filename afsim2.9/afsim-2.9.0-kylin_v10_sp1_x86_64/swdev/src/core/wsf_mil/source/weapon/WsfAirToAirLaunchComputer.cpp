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

#include "WsfAirToAirLaunchComputer.hpp"

#include <cassert>
#include <cmath>
#include <fstream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"

namespace
{
const double cDEFAULT_LATERAL_ACCEL_LIMIT = 9.0 * UtEarth::cACCEL_OF_GRAVITY;
}

WsfAirToAirLaunchComputer::WsfAirToAirLaunchComputer(const UtAtmosphere& aAtmosphere)
   : WsfLaunchComputer()
   , mAirToAirTablePtr(nullptr)
   , mAtmosphere(aAtmosphere)
{
}

WsfAirToAirLaunchComputer::WsfAirToAirLaunchComputer(const WsfAirToAirLaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mAirToAirTablePtr(aSrc.mAirToAirTablePtr)
   , mAtmosphere(aSrc.mAtmosphere)
{
}

WsfLaunchComputer* WsfAirToAirLaunchComputer::Clone() const
{
   return new WsfAirToAirLaunchComputer(*this);
}

// Base method is not needed, so re-implement a trivial one.
// protected virtual
bool WsfAirToAirLaunchComputer::InitializeTTIData()
{
   return true;
}

//! Accessor returns the Rmax, Rne, and Rmin values (and corresponding
//! times of flight) for the engagement condition nearest the supplied
//! input Track. Boolean return value indicates whether the conditions
//! are within the defined valid condition set (true) or extrapolated (false).
//!  @param aTrack    Target Track to intercept.
//!  @param aRmax     Tabular Output Maximum Time Of Flight value for the weapon (closest condition).
//!  @param aRmaxTOF  Tabular Output Maximum Time Of Flight value for the weapon (closest condition).
//!  @param aRne      Tabular Output No-Escape Range value for the weapon (closest condition).
//!  @param aRneTOF   Tabular Output No-Escape Time Of Flight  value for the weapon (closest condition).
//!  @param aRmin     Tabular Output Minimum Range value for the weapon (closest condition).
//!  @param aRminTOF  Tabular Output Minimum Time Of Flight Range value for the weapon (closest condition).
//!  @returns         Within valid table range (true), or not (false).
bool WsfAirToAirLaunchComputer::LookupResult(const WsfTrack& aTrack,
                                             double&         aRmax,
                                             double&         aRmaxTOF,
                                             double&         aRne,
                                             double&         aRneTOF,
                                             double&         aRmin,
                                             double&         aRminTOF)
{
   // To get a valid ATA engagement track, we really need
   // full position and velocity information
   if (!(aTrack.LocationValid() && aTrack.VelocityValid()))
   {
      return false;
   }

   // This implementation will assume that platform location
   // and velocity are known truthfully
   WsfPlatform* shooterPlatPtr = GetWeapon()->GetPlatform();
   double       shooterLat;
   double       shooterLon;
   double       shooterInAlt;
   shooterPlatPtr->GetLocationLLA(shooterLat, shooterLon, shooterInAlt);

   double shooterVelNED[3];
   shooterPlatPtr->GetVelocityNED(shooterVelNED);

   double shooterInHeading = atan2(shooterVelNED[1], shooterVelNED[0]);
   double shooterInSpeed   = shooterPlatPtr->GetSpeed();
   double shooterInMach    = shooterInSpeed / mAtmosphere.SonicVelocity(shooterInAlt);

   double targetLat;
   double targetLon;
   double targetInAlt;
   double targetVelNED[] = {0.0, 0.0, 0.0};
   aTrack.GetLocationLLA(targetLat, targetLon, targetInAlt);

   double targetVelWCS[3];
   aTrack.GetVelocityWCS(targetVelWCS);

   UtEntity targetEntity;
   targetEntity.SetLocationLLA(targetLat, targetLon, targetInAlt);
   targetEntity.ConvertWCSVectorToNED(targetVelNED, targetVelWCS);

   double targetInHeading = atan2(targetVelNED[1], targetVelNED[0]);
   double targetInSpeed   = UtVec3d::Magnitude(targetVelNED);
   double targetInMach    = targetInSpeed / mAtmosphere.SonicVelocity(targetInAlt);

   double targetRelLocECS[3];
   shooterPlatPtr->GetRelativeLocationECS(&targetEntity, targetRelLocECS);
   double targetAzimuth = atan2(targetRelLocECS[1], targetRelLocECS[0]);

   // Now we need to calculate whether the target "lead"
   // is crossing left to right or right to left:
   double targetRelVelECS[3];
   shooterPlatPtr->ConvertWCSVectorToECS(targetRelVelECS, targetVelWCS);
   double sign            = UtMath::Sign(targetRelVelECS[1]);
   double targetInLead    = sign * targetAzimuth;
   double targetInBearing = targetAzimuth + shooterInHeading;
   double targetInAspect  = fabs(UtMath::NormalizeAngleMinusPi_Pi(targetInHeading - targetInBearing - UtMath::cPI));

   double shooterOutAlt;
   double shooterOutMach;
   double targetOutAlt;
   double targetOutMach;
   double targetOutAspect;
   double targetOutLead;
   bool   isValid = LookupResult(shooterInAlt,
                               shooterInMach,
                               targetInAlt,
                               targetInMach,
                               targetInAspect,
                               targetInLead,
                               shooterOutAlt,
                               shooterOutMach,
                               targetOutAlt,
                               targetOutMach,
                               targetOutAspect,
                               targetOutLead,
                               aRmax,
                               aRmaxTOF,
                               aRne,
                               aRneTOF,
                               aRmin,
                               aRminTOF);

   if (DebugEnabled() && isValid)
   {
      auto out = ut::log::debug() << "Engagement Conditions:";
      out.AddNote() << "Shooter Alt: " << shooterInAlt;
      out.AddNote() << "Shooter Mach: " << shooterInMach;
      out.AddNote() << "Target Alt: " << targetInAlt;
      out.AddNote() << "Target Mach: " << targetInMach;
      out.AddNote() << "Target Aspect: " << static_cast<int>(targetInAspect * UtMath::cDEG_PER_RAD);
      out.AddNote() << "Target Lead: " << static_cast<int>(targetInLead * UtMath::cDEG_PER_RAD);
      { // RAII block
         auto note = out.AddNote() << "Yields:";
         note.AddNote() << "Rmin: " << aRmin;
         note.AddNote() << "RminTOF: " << aRminTOF;
         note.AddNote() << "Rne: " << aRne;
         note.AddNote() << "RneTOF: " << aRneTOF;
         note.AddNote() << "Rmax: " << aRmax;
         note.AddNote() << "RmaxTOF: " << aRmaxTOF;
      }
      if (aTrack.RangeValid())
      {
         out.AddNote() << "ACTUAL TARGET RANGE: " << aTrack.GetRange();
      }
      else
      {
         double range = UtVec3d::Magnitude(targetRelLocECS);
         out.AddNote() << "ACTUAL TARGET RANGE: " << range << " m";
      }
   }
   return isValid;
}

//! Accessor returns the Rmax, Rne, and Rmin values (and corresponding
//! times of flight) for the engagement condition nearest the supplied
//! input Track. Boolean return value indicates whether the conditions
//! are within the defined valid condition set (true) or extrapolated (false).
//!  @param aShooterInAlt    Input current shooter altitude.
//!  @param aShooterInMach   Input current shooter Mach number(speed).
//!  @param aTargetInAlt     Input current target Altitude.
//!  @param aTargetInMach    Input current target Speed.
//!  @param aTargetInAspect  Input current target Aspect Angle.
//!  @param aTargetInLead    Input current target Lead Angle
//!  @param aShooterOutAlt   Closest Tabulated shooter altitude to current conditions.
//!  @param aShooterOutMach  Closest Tabulated shooter Mach to current conditions.
//!  @param aTargetOutAlt    Closest Tabulated target Altitude to current conditions.
//!  @param aTargetOutMach   Closest Tabulated target Mach to current conditions.
//!  @param aTargetOutAspect Closest Tabulated target Aspect to current conditions.
//!  @param aTargetOutLead   Closest Tabulated target Lead angle to current conditions.
//!  @param aRmax            Tabular Output Maximum Time Of Flight value for the weapon (closest condition).
//!  @param aRmaxTOF         Tabular Output Maximum Time Of Flight value for the weapon (closest condition).
//!  @param aRne             Tabular Output No-Escape Range value for the weapon (closest condition).
//!  @param aRneTOF          Tabular Output No-Escape Time Of Flight value for the weapon (closest condition).
//!  @param aRmin            Tabular Output Minimum Range value for the weapon (closest condition).
//!  @param aRminTOF         Tabular Output Minimum Time Of Flight Range value for the weapon (closest condition).
//!  @returns                Within valid table range (true), or not (false).
bool WsfAirToAirLaunchComputer::LookupResult(double  aShooterInAlt,
                                             double  aShooterInMach,
                                             double  aTargetInAlt,
                                             double  aTargetInMach,
                                             double  aTargetInAspect,
                                             double  aTargetInLead,
                                             double& aShooterOutAlt,
                                             double& aShooterOutMach,
                                             double& aTargetOutAlt,
                                             double& aTargetOutMach,
                                             double& aTargetOutAspect,
                                             double& aTargetOutLead,
                                             double& aRmax,
                                             double& aRmaxTOF,
                                             double& aRne,
                                             double& aRneTOF,
                                             double& aRmin,
                                             double& aRminTOF)
{
   // Initialize all to cNOT_SET
   aRmax    = cNOT_SET;
   aRmaxTOF = cNOT_SET;
   aRne     = cNOT_SET;
   aRneTOF  = cNOT_SET;
   aRmin    = cNOT_SET;
   aRminTOF = cNOT_SET;

   size_t shooterAltIndex;
   size_t shooterMachIndex;
   size_t targetAltIndex;
   size_t targetMachIndex;
   size_t targetAspectIndex;
   size_t targetLeadIndex;
   size_t resultIndex;

   /*bool inRange = */ mAirToAirTablePtr->ValuesToIndices(aShooterInAlt,
                                                          aShooterInMach,
                                                          aTargetInAlt,
                                                          aTargetInMach,
                                                          aTargetInAspect,
                                                          aTargetInLead,
                                                          shooterAltIndex,
                                                          shooterMachIndex,
                                                          targetAltIndex,
                                                          targetMachIndex,
                                                          targetAspectIndex,
                                                          targetLeadIndex);

   aShooterOutAlt   = mAirToAirTablePtr->ShooterAlts()[shooterAltIndex];
   aShooterOutMach  = mAirToAirTablePtr->ShooterMachs()[shooterMachIndex];
   aTargetOutAlt    = mAirToAirTablePtr->TargetAlts()[targetAltIndex];
   aTargetOutMach   = mAirToAirTablePtr->TargetMachs()[targetMachIndex];
   aTargetOutAspect = mAirToAirTablePtr->TargetAspects()[targetAspectIndex];
   aTargetOutLead   = mAirToAirTablePtr->TargetLeads()[targetLeadIndex];

   bool confirmed = mAirToAirTablePtr->IndicesToIndex(shooterAltIndex,
                                                      shooterMachIndex,
                                                      targetAltIndex,
                                                      targetMachIndex,
                                                      targetAspectIndex,
                                                      targetLeadIndex,
                                                      resultIndex);

   ATA_InterceptTable::Result result;
   /*bool tripleCheck = */ mAirToAirTablePtr->GetResult(resultIndex, result);

   if (/*inRange &&*/ confirmed /*&& tripleCheck*/)
   {
      // checking for "inRange" basically rules out all queries that provide independent variables outside of the
      //  table's scope, even if just by a little. Comment out inRange for now.

      aRmax    = result.mRmax;
      aRmaxTOF = result.mRmaxTOF;
      aRne     = result.mRne;
      aRneTOF  = result.mRneTOF;
      aRmin    = result.mRmin;
      aRminTOF = result.mRminTOF;
      return true;
   }
   return false;
}

WsfAirToAirLaunchComputer::ATA_InterceptTable::ATA_InterceptTable()
   : mRNE_ManeuverAccel(cDEFAULT_LATERAL_ACCEL_LIMIT)
   , mShooterAlts()
   , mShooterMachs()
   , mTargetAlts()
   , mTargetMachs()
   , mTargetAspects()
   , mTargetLeads()
   , mResults()
{
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::Load(const std::string& aFileName, bool aDebugEnabled)
{
   bool myCommand = false;

   UtInput input;
   input.PushInput(ut::make_unique<UtInputFile>(aFileName));
   if (input.GetCommand() == "launch_computer_table")
   {
      myCommand = true;
      UtInputBlock inputBlock(input);
      while (inputBlock.ReadCommand())
      {
         if (!ProcessInput(inputBlock.GetInput()))
         {
            auto out = ut::log::error() << "Could not parse the file given.";
            out.AddNote() << "File: " << aFileName;
            myCommand = false;
         }
      }
   }
   return myCommand;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::GetResult(size_t aResultIndex, Result& aResult) const
{
   bool resultFound(false);
   if (aResultIndex < mResults.size())
   {
      aResult     = mResults[aResultIndex];
      resultFound = true;
   }
   return resultFound;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::SetResult(size_t aResultIndex, const Result& aResult)
{
   bool resultSet(false);
   if (aResultIndex < mResults.size())
   {
      mResults[aResultIndex] = aResult;
      resultSet              = true;
   }
   return resultSet;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::IndexToIndices(size_t  aResultIndex,
                                                                   size_t& aShooterAltIndex,
                                                                   size_t& aShooterMachIndex,
                                                                   size_t& aTargetAltIndex,
                                                                   size_t& aTargetMachIndex,
                                                                   size_t& aTargetAspectIndex,
                                                                   size_t& aTargetLeadIndex) const
{
   bool valid = true;

   size_t resultIndex = aResultIndex;

   aShooterAltIndex = resultIndex / SizeOfShooterAltSet();
   resultIndex -= aShooterAltIndex * SizeOfShooterAltSet();
   if (aShooterAltIndex >= mShooterAlts.size())
   {
      aShooterAltIndex = mShooterAlts.size() - 1;
      valid            = false;
   }

   aShooterMachIndex = resultIndex / SizeOfShooterMachSet();
   resultIndex -= aShooterMachIndex * SizeOfShooterMachSet();
   if (aShooterMachIndex >= mShooterMachs.size())
   {
      aShooterMachIndex = mShooterMachs.size() - 1;
      valid             = false;
   }

   aTargetAltIndex = resultIndex / SizeOfTargetAltSet();
   resultIndex -= aTargetAltIndex * SizeOfTargetAltSet();
   if (aTargetAltIndex >= mTargetAlts.size())
   {
      aTargetAltIndex = mTargetAlts.size() - 1;
      valid           = false;
   }

   aTargetMachIndex = resultIndex / SizeOfTargetMachSet();
   resultIndex -= aTargetMachIndex * SizeOfTargetMachSet();
   if (aTargetMachIndex >= mTargetMachs.size())
   {
      aTargetMachIndex = mTargetMachs.size() - 1;
      valid            = false;
   }

   aTargetAspectIndex = resultIndex / SizeOfTargetAspectSet();
   resultIndex -= aTargetAspectIndex * SizeOfTargetAspectSet();
   if (aTargetAspectIndex >= mTargetAspects.size())
   {
      aTargetAspectIndex = mTargetAspects.size() - 1;
      valid              = false;
   }

   aTargetLeadIndex = resultIndex;
   if (aTargetLeadIndex >= mTargetLeads.size())
   {
      aTargetLeadIndex = mTargetLeads.size() - 1;
      valid            = false;
   }

   return valid;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::IndicesToIndex(size_t  aShooterAltIndex,
                                                                   size_t  aShooterMachIndex,
                                                                   size_t  aTargetAltIndex,
                                                                   size_t  aTargetMachIndex,
                                                                   size_t  aTargetAspectIndex,
                                                                   size_t  aTargetLeadIndex,
                                                                   size_t& aResultIndex) const
{
   bool indexFound(false);
   aResultIndex = 0;
   if ((aShooterAltIndex < mShooterAlts.size()) && (aShooterMachIndex < mShooterMachs.size()) &&
       (aTargetAltIndex < mTargetAlts.size()) && (aTargetMachIndex < mTargetMachs.size()) &&
       (aTargetAspectIndex < mTargetAspects.size()) && (aTargetLeadIndex < mTargetLeads.size()))
   {
      aResultIndex = aShooterAltIndex * SizeOfShooterAltSet() + aShooterMachIndex * SizeOfShooterMachSet() +
                     aTargetAltIndex * SizeOfTargetAltSet() + aTargetMachIndex * SizeOfTargetMachSet() +
                     aTargetAspectIndex * SizeOfTargetAspectSet() + aTargetLeadIndex;
      indexFound = true;
   }
   return indexFound;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::IndexToValues(size_t  aResultIndex,
                                                                  double& aShooterAlt,
                                                                  double& aShooterMach,
                                                                  double& aTargetAlt,
                                                                  double& aTargetMach,
                                                                  double& aTargetAspect,
                                                                  double& aTargetLead) const
{
   size_t shooterAltIndex   = 0;
   size_t shooterMachIndex  = 0;
   size_t targetAltIndex    = 0;
   size_t targetMachIndex   = 0;
   size_t targetAspectIndex = 0;
   size_t targetLeadIndex   = 0;

   bool valid = IndexToIndices(aResultIndex,
                               shooterAltIndex,
                               shooterMachIndex,
                               targetAltIndex,
                               targetMachIndex,
                               targetAspectIndex,
                               targetLeadIndex);
   if (valid)
   {
      aShooterAlt   = mShooterAlts[shooterAltIndex];
      aShooterMach  = mShooterMachs[shooterMachIndex];
      aTargetAlt    = mTargetAlts[targetAltIndex];
      aTargetMach   = mTargetMachs[targetMachIndex];
      aTargetAspect = mTargetAspects[targetAspectIndex];
      aTargetLead   = mTargetLeads[targetLeadIndex];
   }
   return valid;
}

// protected
bool WsfAirToAirLaunchComputer::ATA_InterceptTable::FindNearestIndex(double                     aValue,
                                                                     const std::vector<double>& aArray,
                                                                     size_t&                    aIndex) const
{
   bool indexFound(false);
   aIndex = 0;
   if (aValue < aArray[0])
   {
      indexFound = true;
   }
   else if (aValue > aArray[aArray.size() - 1])
   {
      aIndex     = aArray.size() - 1;
      indexFound = true;
   }

   if (!indexFound)
   {
      while ((aIndex + 1 < aArray.size()) && (fabs(aValue - aArray[aIndex]) > fabs(aValue - aArray[aIndex + 1])))
      {
         ++aIndex;
      }
   }
   return indexFound;
}

void WsfAirToAirLaunchComputer::ATA_InterceptTable::ClearAll()
{
   mShooterAlts.clear();
   mShooterMachs.clear();
   mTargetAlts.clear();
   mTargetMachs.clear();
   mTargetAspects.clear();
   mTargetLeads.clear();
   mResults.clear();
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::ValuesToIndices(double  aShooterAlt,
                                                                    double  aShooterMach,
                                                                    double  aTargetAlt,
                                                                    double  aTargetMach,
                                                                    double  aTargetAspect,
                                                                    double  aTargetLead,
                                                                    size_t& aShooterAltIndex,
                                                                    size_t& aShooterMachIndex,
                                                                    size_t& aTargetAltIndex,
                                                                    size_t& aTargetMachIndex,
                                                                    size_t& aTargetAspectIndex,
                                                                    size_t& aTargetLeadIndex) const
{
   bool extrapolated(false);
   extrapolated |= FindNearestIndex(aShooterAlt, mShooterAlts, aShooterAltIndex);
   extrapolated |= FindNearestIndex(aShooterMach, mShooterMachs, aShooterMachIndex);
   extrapolated |= FindNearestIndex(aTargetAlt, mTargetAlts, aTargetAltIndex);
   extrapolated |= FindNearestIndex(aTargetMach, mTargetMachs, aTargetMachIndex);
   extrapolated |= FindNearestIndex(aTargetAspect, mTargetAspects, aTargetAspectIndex);
   extrapolated |= FindNearestIndex(aTargetLead, mTargetLeads, aTargetLeadIndex);
   return !extrapolated;
}

// virtual
bool WsfAirToAirLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "load_table")
   {
      // We are going to get a new table. Release the previous one.
      mAirToAirTablePtr = std::make_shared<ATA_InterceptTable>();

      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      if (!mAirToAirTablePtr->Load(fileName, DebugEnabled()))
      {
         throw UtInput::BadValue(aInput, "Unable to load file " + fileName);
      }
      WsfScenario::FromInput(aInput).GetSystemLog().WriteLogEntry("file " + fileName);
   }
   else if (command == "launch_computer_table")
   {
      // We are going to get a new table. Release the previous one.
      mAirToAirTablePtr = std::make_shared<ATA_InterceptTable>();

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!mAirToAirTablePtr->ProcessInput(inputBlock.GetInput()))
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfAirToAirLaunchComputer::ATA_InterceptTable::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "independent_variables")
   {
      UtInputBlock inputBlock(aInput);
      myCommand = ProcessIndependentBlock(inputBlock);
   }
   else if (command == "intercept_results")
   {
      UtInputBlock inputBlock(aInput);
      ProcessInterceptResults(inputBlock);
   }
   else if (command == "no_escape_maneuver")
   {
      aInput.ReadValueOfType(mRNE_ManeuverAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mRNE_ManeuverAccel, UtEarth::cACCEL_OF_GRAVITY);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}


//! Routine processes reading in the values to be used for independent variables
//! given between the "begin" and "end" block qualifiers.
// protected
bool WsfAirToAirLaunchComputer::ATA_InterceptTable::ProcessIndependentBlock(UtInputBlock& aInputBlock)
{
   bool blockEntered = false;

   while (aInputBlock.ReadCommand())
   {
      std::string command(aInputBlock.GetInput().GetCommand());

      if (command == "shooter_altitudes")
      {
         mShooterAlts.clear();
         UtInputBlock subBlock(aInputBlock.GetInput());
         while (subBlock.ReadCommand())
         {
            subBlock.GetInput().PushBack(subBlock.GetCommand());
            double value;
            subBlock.GetInput().ReadValueOfType(value, UtInput::cLENGTH);
            mShooterAlts.push_back(value);
            blockEntered = true;
         }
      }
      else if (command == "target_altitudes")
      {
         mTargetAlts.clear();
         UtInputBlock subBlock(aInputBlock.GetInput());
         while (subBlock.ReadCommand())
         {
            subBlock.GetInput().PushBack(subBlock.GetCommand());
            double value;
            subBlock.GetInput().ReadValueOfType(value, UtInput::cLENGTH);
            mTargetAlts.push_back(value);
            blockEntered = true;
         }
      }
      else if (command == "shooter_machs")
      {
         mShooterMachs.clear();
         UtInputBlock subBlock(aInputBlock.GetInput());
         while (subBlock.ReadCommand())
         {
            subBlock.GetInput().PushBack(subBlock.GetCommand());
            double value;
            subBlock.GetInput().ReadValue(value);
            mShooterMachs.push_back(value);
            blockEntered = true;
         }
      }
      else if (command == "target_machs")
      {
         mTargetMachs.clear();
         UtInputBlock subBlock(aInputBlock.GetInput());
         while (subBlock.ReadCommand())
         {
            subBlock.GetInput().PushBack(subBlock.GetCommand());
            double value;
            subBlock.GetInput().ReadValue(value);
            mTargetMachs.push_back(value);
            blockEntered = true;
         }
      }
      else if ((command == "target_aspect_angles") || (command == "target_aspects"))
      {
         mTargetAspects.clear();
         UtInputBlock subBlock(aInputBlock.GetInput());
         while (subBlock.ReadCommand())
         {
            subBlock.GetInput().PushBack(subBlock.GetCommand());
            double value;
            subBlock.GetInput().ReadValueOfType(value, UtInput::cANGLE);
            subBlock.GetInput().ValueInClosedRange(value, 0.0, UtMath::cPI);
            mTargetAspects.push_back(value);
            blockEntered = true;
         }
      }
      else if ((command == "target_leads") || (command == "target_lead_angles"))
      {
         mTargetLeads.clear();
         UtInputBlock subBlock(aInputBlock.GetInput());
         while (subBlock.ReadCommand())
         {
            subBlock.GetInput().PushBack(subBlock.GetCommand());
            double value;
            subBlock.GetInput().ReadValueOfType(value, UtInput::cANGLE);
            subBlock.GetInput().ValueInClosedRange(value, -UtMath::cPI, UtMath::cPI);
            mTargetLeads.push_back(value);
            blockEntered = true;
         }
      }
      else if (command == "target_lead_angle_limits")
      {
         mTargetLeads.clear();

         std::string word;
         aInputBlock.GetInput().ReadValue(word);
         aInputBlock.GetInput().StringEqual(word, "from");

         double angleMin(0.0);
         aInputBlock.GetInput().ReadValueOfType(angleMin, UtInput::cANGLE);

         aInputBlock.GetInput().ReadValue(word);
         aInputBlock.GetInput().StringEqual(word, "to");

         double angleMax(0.0);
         aInputBlock.GetInput().ReadValueOfType(angleMax, UtInput::cANGLE);
         aInputBlock.GetInput().ValueGreater(angleMax, angleMin);

         aInputBlock.GetInput().ReadValue(word);
         aInputBlock.GetInput().StringEqual(word, "by");

         const double EPSILON = 2.0 * UtMath::cRAD_PER_DEG; // two degrees
         double       angleStep(EPSILON);
         aInputBlock.GetInput().ReadValueOfType(angleStep, UtInput::cANGLE);
         aInputBlock.GetInput().ValueGreaterOrEqual(angleStep, EPSILON);

         double value = angleMin;
         while (value < (angleMax - EPSILON))
         {
            mTargetLeads.push_back(value);
            value += angleStep;
         }
         mTargetLeads.push_back(angleMax);
      }
   }

   // Check to see that all the IV arrays have valid data in them,
   // and that the steps are even (if required):
   return blockEntered && IVArraysOk(aInputBlock.GetInput());
}

//! Routine streams out the values to be used for independent variables
//! given between the "begin" and "end" block qualifiers.
// protected
void WsfAirToAirLaunchComputer::ATA_InterceptTable::StreamIndependentBlock(std::ostream& aStream) const
{
   aStream << "   independent_variables" << std::endl;

   size_t i;
   aStream << "      shooter_altitudes ";
   for (i = 0; i != mShooterAlts.size(); ++i)
   {
      aStream << mShooterAlts[i] * UtMath::cFT_PER_M << " ft ";
   }
   aStream << "end_shooter_altitudes" << std::endl;

   aStream << "      shooter_machs ";
   for (i = 0; i != mShooterMachs.size(); ++i)
   {
      aStream << mShooterMachs[i] << " ";
   }
   aStream << "end_shooter_machs" << std::endl;

   aStream << "      target_altitudes ";
   for (i = 0; i != mTargetAlts.size(); ++i)
   {
      aStream << mTargetAlts[i] * UtMath::cFT_PER_M << " ft ";
   }
   aStream << "end_target_altitudes" << std::endl;

   aStream << "      target_machs ";
   for (i = 0; i != mTargetMachs.size(); ++i)
   {
      aStream << mTargetMachs[i] << " ";
   }
   aStream << "end_target_machs" << std::endl;

   aStream << "      target_aspects ";
   for (i = 0; i != mTargetAspects.size(); ++i)
   {
      aStream << mTargetAspects[i] * UtMath::cDEG_PER_RAD << " deg ";
   }
   aStream << "end_target_aspects" << std::endl;

   aStream << "      target_leads ";
   for (i = 0; i != mTargetLeads.size(); ++i)
   {
      aStream << mTargetLeads[i] * UtMath::cDEG_PER_RAD << " deg ";
   }
   aStream << "end_target_leads" << std::endl;

   aStream << "   end_independent_variables" << std::endl;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::ArrayOk(UtInput&                   aInput,
                                                            const std::vector<double>& aArray,
                                                            const std::string&         aArrayType,
                                                            double                     aMinimumDelta) const
{
   // Array cannot be empty
   if (aArray.empty())
   {
      throw UtInput::BadValue(aInput, aArrayType + " cannot be empty; must have at least one value.");
   }

   // Check to see that arrays are increasing and evenly incremented:
   for (size_t i = 1; i < aArray.size(); ++i)
   {
      if (aArray[i] - aArray[i - 1] <= aMinimumDelta)
      {
         throw UtInput::BadValue(aInput, aArrayType + " not in increasing order, or too small a spread.");
      }
   }
   return true;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::IVArraysOk(UtInput& aInput)
{
   const double EPSILON = 2.0 * UtMath::cRAD_PER_DEG; // two degrees

   if (!ArrayOk(aInput, mShooterAlts, "shooter_altitudes", 100.0) ||
       !ArrayOk(aInput, mShooterMachs, "shooter_machs", 0.05) ||
       !ArrayOk(aInput, mTargetAlts, "target_altitudes", 100.0) || !ArrayOk(aInput, mTargetMachs, "target_machs", 0.05) ||
       !ArrayOk(aInput, mTargetAspects, "target_aspects", EPSILON) ||
       !ArrayOk(aInput, mTargetLeads, "target_leads", EPSILON))
   {
      throw UtInput::BadValue(aInput, "Unknown problem with Independent Values.");
   }
   return true;
}

//! Routine processes reading in the engagement condition values
//! given between the "begin" and "end" block qualifiers.
// protected
void WsfAirToAirLaunchComputer::ATA_InterceptTable::ProcessInterceptResults(UtInputBlock& aInputBlock)
{
   mResults.clear();
   while (aInputBlock.ReadCommand())
   {
      Result result;
      if (!result.ProcessInput(aInputBlock.GetInput()))
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Unknown parameter within intercept results block.");
      }
      mResults.push_back(result);
   }
}

//! Implementation of virtual base class method.
// virtual
double WsfAirToAirLaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   double Rmax    = cNOT_SET;
   double Rne     = cNOT_SET;
   double RneTOF  = cNOT_SET;
   double Rmin    = cNOT_SET;
   double RminTOF = cNOT_SET;
   double RmaxTOF = cNOT_SET;

   // Use Rmax as maximum TOF
   bool found = LookupResult(aTrack, Rmax, RmaxTOF, Rne, RneTOF, Rmin, RminTOF);
   if (found && (RmaxTOF != cNOT_SET))
   {
      return RmaxTOF + aLaunchDelayTime;
   }
   return cFOREVER;
}

// =================================================================================================
// virtual
//! Initialize the object.
//! @param aSimTime   The current simulation time.
//! @param aWeaponPtr The reference platform to use with default launch calculations.
//! @return 'true' if successful or 'false' if some sort of error occurred.
bool WsfAirToAirLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool success = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);
   if (GetTable().ResultsSize() != GetTable().SizeOfResultsSet())
   {
      success = false;
   }
   return success;
}

//! For use by the weapon_tools. Assures that selected ranges are monotonically increasing,
//! which is required for proper table lookups.
bool WsfAirToAirLaunchComputer::ToolOnlyInitialize(double aSimTime)
{
   return true;
}

bool WsfAirToAirLaunchComputer::Test()
{
   bool success = true;

   if (GetTable().ResultsSize() != GetTable().SizeOfResultsSet())
   {
      auto out = ut::log::info() << "WsfAirToAirLaunchComputer() has FAILED due to independent_variables set size "
                                    "being different than the intercept_results table size.";
      out.AddNote() << "independent_variables set size: " << GetTable().SizeOfResultsSet();
      out.AddNote() << "intercept_results table size: " << GetTable().ResultsSize();
      return false;
   }

   // Perform two tests:
   // First, pick a value off the edge of the data table,
   // and assure it is flagged as "not in range".
   // (NOTE :  This test result is now ignored, due to an
   //  extrapolation enabled by another software engineer.)

   double shooterInAlt   = -3000.0;
   double shooterInMach  = -1.0;
   double targetInAlt    = -3000.0;
   double targetInMach   = -1.0;
   double targetInAspect = -1.0;
   double targetInLead   = -1.0;

   double shooterOutAlt   = 0.0;
   double shooterOutMach  = 0.0;
   double targetOutAlt    = 0.0;
   double targetOutMach   = 0.0;
   double targetOutAspect = 0.0;
   double targetOutLead   = 0.0;

   double Rmax;
   double RmaxTOF;
   double Rmin;
   double RminTOF;
   double Rne;
   double RneTOF;

   bool inRange = LookupResult(shooterInAlt,
                               shooterInMach,
                               targetInAlt,
                               targetInMach,
                               targetInAspect,
                               targetInLead,
                               shooterOutAlt,
                               shooterOutMach,
                               targetOutAlt,
                               targetOutMach,
                               targetOutAspect,
                               targetOutLead,
                               Rmax,
                               RmaxTOF,
                               Rne,
                               RneTOF,
                               Rmin,
                               RminTOF);

   // inRange should be false
   if (false && inRange)
   {
      ut::log::info() << "WsfAirToAirLaunchComputer() has FAILED its first Test() method.";
      success = false;
   }

   // Second, pick a value inside the valid range of the table,
   // assure it is flagged as okay, and that the values are correct:
   size_t shooterAltIndex   = mAirToAirTablePtr->ShooterAlts().size() / 2;
   size_t shooterMachIndex  = mAirToAirTablePtr->ShooterMachs().size() / 2;
   size_t targetAltIndex    = mAirToAirTablePtr->TargetAlts().size() / 2;
   size_t targetMachIndex   = mAirToAirTablePtr->TargetMachs().size() / 2;
   size_t targetAspectIndex = mAirToAirTablePtr->TargetAspects().size() / 2;
   size_t targetLeadIndex   = mAirToAirTablePtr->TargetLeads().size() / 2;

   shooterInAlt   = mAirToAirTablePtr->ShooterAlts()[shooterAltIndex];
   shooterInMach  = mAirToAirTablePtr->ShooterMachs()[shooterMachIndex];
   targetInAlt    = mAirToAirTablePtr->TargetAlts()[targetAltIndex];
   targetInMach   = mAirToAirTablePtr->TargetMachs()[targetMachIndex];
   targetInAspect = mAirToAirTablePtr->TargetAspects()[targetAspectIndex];
   targetInLead   = mAirToAirTablePtr->TargetLeads()[targetLeadIndex];

   inRange = LookupResult(shooterInAlt,
                          shooterInMach,
                          targetInAlt,
                          targetInMach,
                          targetInAspect,
                          targetInLead,
                          shooterOutAlt,
                          shooterOutMach,
                          targetOutAlt,
                          targetOutMach,
                          targetOutAspect,
                          targetOutLead,
                          Rmax,
                          RmaxTOF,
                          Rne,
                          RneTOF,
                          Rmin,
                          RminTOF);

   // inRange should be true
   if (!inRange)
   {
      ut::log::info() << "WsfAirToAirLaunchComputer() has FAILED its second Test() method.";
      success = false;
   }
   return success;
}

void WsfAirToAirLaunchComputer::ATA_InterceptTable::Stream(std::ostream& aStream, const std::string& aResultsFilename) const
{
   aStream << "\n   "
           << "no_escape_maneuver " << mRNE_ManeuverAccel / UtEarth::cACCEL_OF_GRAVITY << " g \n"
           << std::endl;
   StreamIndependentBlock(aStream);
   StreamResults(aStream, aResultsFilename);
}

void WsfAirToAirLaunchComputer::ATA_InterceptTable::StreamResults(std::ostream&      aStream,
                                                                  const std::string& aResultsFilename) const
{
   aStream << "\n   "
           << "intercept_results";
   if (aResultsFilename.empty())
   {
      size_t shooterAltIndex   = 0;
      size_t shooterMachIndex  = 0;
      size_t targetAltIndex    = 0;
      size_t targetMachIndex   = 0;
      size_t targetAspectIndex = 0;
      size_t targetLeadIndex   = 0;

      size_t lastTargetAspectIndex = 999;
      size_t lastTargetMachIndex   = 999;
      size_t lastTargetAltIndex    = 999;
      size_t lastShooterMachIndex  = 999;
      size_t lastShooterAltIndex   = 999;

      for (size_t i = 0; i < mResults.size(); ++i)
      {
         // bool success = // ignore returned bool
         IndexToIndices(i, shooterAltIndex, shooterMachIndex, targetAltIndex, targetMachIndex, targetAspectIndex, targetLeadIndex);

         if ((targetAspectIndex != lastTargetAspectIndex) || (targetMachIndex != lastTargetMachIndex) ||
             (targetAltIndex != lastTargetAltIndex) || (shooterMachIndex != lastShooterMachIndex) ||
             (shooterAltIndex != lastShooterAltIndex))
         {
            lastTargetAspectIndex = targetAspectIndex;
            lastTargetMachIndex   = targetMachIndex;
            lastTargetAltIndex    = targetAltIndex;
            lastShooterMachIndex  = shooterMachIndex;
            lastShooterAltIndex   = shooterAltIndex;
            aStream << "\n      "
                    << "# Shooter Alt=" << mShooterAlts[shooterAltIndex] * UtMath::cFT_PER_M
                    << " ft, M=" << mShooterMachs[shooterMachIndex]
                    << ", Tgt Alt=" << mTargetAlts[targetAltIndex] * UtMath::cFT_PER_M
                    << " ft, M=" << mTargetMachs[targetMachIndex]
                    << ", Aspect=" << mTargetAspects[targetAspectIndex] * UtMath::cDEG_PER_RAD << " deg" << std::endl;
         }

         aStream << "      ";
         mResults[i].Stream(aStream);
         aStream << " # Lead=" << mTargetLeads[targetLeadIndex] * UtMath::cDEG_PER_RAD << " deg" << std::endl;
      }
   }
   else
   {
      aStream << "\n      include " << aResultsFilename << std::endl;
   }
   aStream << "   end_intercept_results" << std::endl;
}

// ================ Sub-Class Result ==========================

WsfAirToAirLaunchComputer::ATA_InterceptTable::Result::Result()
   : mRmax(cNOT_SET)
   , mRmaxTOF(cNOT_SET)
   , mRne(cNOT_SET)
   , mRneTOF(cNOT_SET)
   , mRmin(cNOT_SET)
   , mRminTOF(cNOT_SET)
{
}

void WsfAirToAirLaunchComputer::ATA_InterceptTable::Result::Reset()
{
   mRmax    = cNOT_SET;
   mRmaxTOF = cNOT_SET;
   mRne     = cNOT_SET;
   mRneTOF  = cNOT_SET;
   mRmin    = cNOT_SET;
   mRminTOF = cNOT_SET;
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::Result::IsNull() const
{
   return ((mRmax == cNOT_SET) && (mRne == cNOT_SET) && (mRmin == cNOT_SET));
}

void WsfAirToAirLaunchComputer::ATA_InterceptTable::Result::Stream(std::ostream& aStream) const
{
   aStream << "Rmin " << mRmin << " m " << mRminTOF << " s";
   aStream << " Rne " << mRne << " m " << mRneTOF << " s";
   aStream << " Rmax " << mRmax << " m " << mRmaxTOF << " s";
}

bool WsfAirToAirLaunchComputer::ATA_InterceptTable::Result::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "Rmin")
   {
      // Read the Rmin Values:
      aInput.ReadValueOfType(mRmin, UtInput::cLENGTH);
      aInput.ReadValueOfType(mRminTOF, UtInput::cTIME);

      // Throw away the Rne token, and Read the Rne Values:
      std::string unusedToken;
      aInput.ReadValue(unusedToken);

      aInput.ReadValueOfType(mRne, UtInput::cLENGTH);
      aInput.ReadValueOfType(mRneTOF, UtInput::cTIME);

      // Throw away the Rmax token, and Read the Rmax Values:
      aInput.ReadValue(unusedToken);

      aInput.ReadValueOfType(mRmax, UtInput::cLENGTH);
      aInput.ReadValueOfType(mRmaxTOF, UtInput::cTIME);

      myCommand = true;
   }
   return myCommand;
}
