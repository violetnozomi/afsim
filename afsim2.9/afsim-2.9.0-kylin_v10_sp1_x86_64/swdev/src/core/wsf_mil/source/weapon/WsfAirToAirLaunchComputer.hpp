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

#ifndef WSFAIRTOAIRLAUNCHCOMPUTER_HPP
#define WSFAIRTOAIRLAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "UtAtmosphere.hpp"
class UtInput;
class UtInputBlock;
#include "WsfLaunchComputer.hpp"

//!   A WsfAirToAirLaunchComputer provides information supporting the decision to launch
//!   a guided airborne weapon against the track of another airborne platform.  The class
//!   is primarily just a shell to hold references to a shared data table.
//!
//!   Engagement Assumptions:  No embedded sensor model (WsfTrack only).  Both target and
//!   shooter are in level flight, and do not take the fight vertical.  Independent
//!   variables are:  Shooter altitude and Mach, Target altitude and Mach, Target Aspect
//!   (nose on = 0), Target Lead Angle.  Dependent Variables are:  R_max, R_ne (no escape),
//!   R_min, and times-of-flight for each.  R_ne assumption:  Target turns tail to missile
//!   w/<input-specified> G turn at the instant of launch.

class WSF_MIL_EXPORT WsfAirToAirLaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_AIR_TO_AIR_LAUNCH_COMPUTER"; }

   WsfAirToAirLaunchComputer(const UtAtmosphere& aAtmosphere);
   WsfAirToAirLaunchComputer& operator=(const WsfAirToAirLaunchComputer&) = delete;
   ~WsfAirToAirLaunchComputer() override                                  = default;

   WsfLaunchComputer* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;

   bool ToolOnlyInitialize(double aSimTime);

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

   bool LookupResult(const WsfTrack& aTrack,
                     double&         aRmax,
                     double&         aRmaxTOF,
                     double&         aRne,
                     double&         aRneTOF,
                     double&         aRmin,
                     double&         aRminTOF);

   bool LookupResult(double  aShooterInAlt,
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
                     double& aRminTOF);

   // =======================================================================

   class WSF_MIL_EXPORT ATA_InterceptTable
   {
   public:
      struct WSF_MIL_EXPORT Result
      {
         Result();
         void Reset();
         bool ProcessInput(UtInput& aInput);
         bool IsNull() const;
         void Stream(std::ostream& aStream) const;

         double mRmax;
         double mRmaxTOF;
         double mRne;
         double mRneTOF;
         double mRmin;
         double mRminTOF;
      };

      ATA_InterceptTable();

      size_t SizeOfTargetAspectSet() const { return mTargetLeads.size(); }
      size_t SizeOfTargetMachSet() const { return mTargetAspects.size() * SizeOfTargetAspectSet(); }
      size_t SizeOfTargetAltSet() const { return mTargetMachs.size() * SizeOfTargetMachSet(); }
      size_t SizeOfShooterMachSet() const { return mTargetAlts.size() * SizeOfTargetAltSet(); }
      size_t SizeOfShooterAltSet() const { return mShooterMachs.size() * SizeOfShooterMachSet(); }
      size_t SizeOfResultsSet() const { return mShooterAlts.size() * SizeOfShooterAltSet(); }

      bool IndexToIndices(size_t  aResultIndex,
                          size_t& aShooterAltIndex,
                          size_t& aShooterMachIndex,
                          size_t& aTargetAltIndex,
                          size_t& aTargetMachIndex,
                          size_t& aTargetAspectIndex,
                          size_t& aTargetLeadIndex) const;

      bool IndicesToIndex(size_t  aShooterAltIndex,
                          size_t  aShooterMachIndex,
                          size_t  aTargetAltIndex,
                          size_t  aTargetMachIndex,
                          size_t  aTargetAspectIndex,
                          size_t  aTargetLeadIndex,
                          size_t& aResultIndex) const;

      bool IndexToValues(size_t  aResultIndex,
                         double& aShooterAlt,
                         double& aShooterMach,
                         double& aTargetAlt,
                         double& aTargetMach,
                         double& aTargetAspect,
                         double& aTargetLead) const;

      bool ValuesToIndices(double  aShooterAlt,
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
                           size_t& aTargetLeadIndex) const;

      bool GetResult(size_t aResultIndex, Result& aResult) const;

      bool SetResult(size_t aResultIndex, const Result& aResult);

      bool FindNearestIndex(double aValue, const std::vector<double>& aArray, size_t& aIndex) const;

      double NoEscapeManeuverAcceleration() const { return mRNE_ManeuverAccel; }

      bool Load(const std::string& aFileName, bool aDebugEnabled);

      //! Write out the contents to capture in a file for later re-use.
      void Stream(std::ostream& aStream, const std::string& aResultsFilename) const;
      void StreamResults(std::ostream& aStream, const std::string& aResultsFilename) const;
      bool ProcessInput(UtInput& aInput);
      void ClearAll();

      const std::vector<double>& ShooterAlts() const { return mShooterAlts; }
      const std::vector<double>& ShooterMachs() const { return mShooterMachs; }
      const std::vector<double>& TargetAlts() const { return mTargetAlts; }
      const std::vector<double>& TargetMachs() const { return mTargetMachs; }
      const std::vector<double>& TargetAspects() const { return mTargetAspects; }
      const std::vector<double>& TargetLeads() const { return mTargetLeads; }

      size_t ResultsSize() const { return mResults.size(); }

   protected:
      void StreamIndependentBlock(std::ostream& aStream) const;
      bool IVArraysOk(UtInput& aInput);
      bool ArrayOk(UtInput& aInput, const std::vector<double>& aArray, const std::string& aArrayType, double aMinimumDelta) const;
      bool ProcessIndependentBlock(UtInputBlock& aInputBlock);
      void ProcessInterceptResults(UtInputBlock& aInputBlock);

      double              mRNE_ManeuverAccel;
      std::vector<double> mShooterAlts;
      std::vector<double> mShooterMachs;
      std::vector<double> mTargetAlts;
      std::vector<double> mTargetMachs;
      std::vector<double> mTargetAspects;
      std::vector<double> mTargetLeads;
      std::vector<Result> mResults;
   };

   // =======================================================================

   ATA_InterceptTable& GetTable() { return *mAirToAirTablePtr; }

   //! Function used by Weapon_Tools to test a newly generated object for correctness.
   bool Test();

protected:
   static constexpr double cNOT_SET{-1.0};

   bool InitializeTTIData() override;
   WsfAirToAirLaunchComputer(const WsfAirToAirLaunchComputer& aSrc);

private:
   std::shared_ptr<ATA_InterceptTable> mAirToAirTablePtr;

   UtAtmosphere mAtmosphere;
};

#endif
