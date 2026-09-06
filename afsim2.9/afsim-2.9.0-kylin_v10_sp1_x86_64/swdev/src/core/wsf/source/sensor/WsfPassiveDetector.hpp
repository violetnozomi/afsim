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

#ifndef WSFPASSIVEDETECTOR_HPP
#define WSFPASSIVEDETECTOR_HPP

#include "wsf_export.h"

#include <map>

#include "TblLookup.hpp"
#include "WsfSensor.hpp"

//! The detector for a passive type sensor.
//!
//! The fundamental purpose for this class is to take the signal from the current detection chance
//! and determine the probability of detecting it.
class WSF_EXPORT WsfPassiveDetector
{
public:
   enum SignalType
   {
      cST_CONTINUOUS,
      cST_PULSED
   };

   //! Type definition for the frequency dependent detection sensitivities / thresholds
   typedef std::vector<std::pair<double, double>> SensitivityThresholdList;

   //! Type definition of the mapping of the PassiveSignalType enumeration to the Sensitivity / Threshold
   typedef std::map<SignalType, SensitivityThresholdList> SignalTypeToSensThreshTable;

   // NOTE: Using default constructor, copy constructor and destructor.

   bool Initialize();

   bool ProcessInput(UtInput& aInput);

   void ComputeDetectability(WsfSensorResult& aResult);

   bool SetDetectionSensitivity(SignalType aSignalType, double aFrequency, double aSensitivity);

   bool SetDetectionThreshold(SignalType aSignalType, double aFrequency, double aThreshold);

   //! @name External Services access.
   //! These methods support the External Services and are not part of the WSF public interface.
   //@{
   size_t GetDetectionThresholdCount(SignalType aSignalType) const;
   void GetDetectionThresholdEntry(SignalType aSignalType, size_t aIndex, double& aFrequency, double& aThreshold) const;

   size_t GetDetectionSensitivityCount(SignalType aSignalType) const;
   void GetDetectionSensitivityEntry(SignalType aSignalType, size_t aIndex, double& aFrequency, double& aSensitivity) const;
   //@}

private:
   bool AddDetectionThreshold(SignalType aSignalType, double aFrequency, double aThreshold);

   bool AddDetectionSensitivity(SignalType aSignalType, double aFrequency, double aSensitivity);

   double GetDetectionThreshold(SignalType aSignalType, double aFrequency) const;

   double GetDetectionSensitivity(SignalType aSignalType, double aFrequency) const;

   void ClearDetectionTables();
   void ClearDetectionThresholdTable();
   void ClearDetectionSensitivityTable();

   bool AddDetectionThresholdSensitivity(SensitivityThresholdList& aDetectionThreshSensList,
                                         double                    aFrequency,
                                         double                    aThreshSens);

   double GetDetectionThresholdSensitivity(const SensitivityThresholdList& aDetectionThreshSensList, double aFrequency) const;

   //! Sensitivity list table
   SignalTypeToSensThreshTable mDetectionThresholdTable;

   //! Threshold list table
   SignalTypeToSensThreshTable mDetectionSensitivityTable;

   //! Pd vs. Signal Level (expressed as a ratio to the threshold of detection)
   TblIndVarU<double> mSignalTable;
   TblDepVar1<double> mPdTable;
};

#endif
