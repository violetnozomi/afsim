// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfTRIMSIM_Processor.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtLog.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtTime.hpp"
#include "UtVectorUtil.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfCovariance.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

// ============================================================================
WsfTRIMSIM_Processor::WsfTRIMSIM_Processor(WsfScenario& aScenario)
   : WsfSensorProcessor(aScenario)
   , mMinimumDetections(cDEFAULT_MINIMUM_DETECTIONS)
{
}

// ============================================================================
WsfProcessor* WsfTRIMSIM_Processor::Clone() const
{
   return new WsfTRIMSIM_Processor(*this);
}

// ============================================================================
bool WsfTRIMSIM_Processor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "minimum_detections")
   {
      aInput.ReadValue(mMinimumDetections);
      aInput.ValueGreaterOrEqual(mMinimumDetections, 2U);
   }
   else
   {
      myCommand = WsfSensorProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// ============================================================================
void WsfTRIMSIM_Processor::TurnOn(double aSimTime)
{
   // Check the number of available sensors and notify if not present
   CheckSensors();

   // Call base class
   WsfSensorProcessor::TurnOn(aSimTime);
}

//============================================================================
bool WsfTRIMSIM_Processor::AttemptToDetect(double           aSimTime,
                                           WsfSensor*       aSensorPtr,
                                           WsfPlatform*     aTargetPtr,
                                           WsfSensorResult& aResult)
{
   bool detected = false;

   // Return if not first sensor in the list
   if (aSensorPtr != mSensorPtrs[0])
   {
      return false;
   }

   // Make sure the Detection List is cleared for this set of attempts
   ClearDetections();

   // Check the primary sensor's result that was just ran for a detection.
   if (aResult.Detected())
   {
      // Load the detection information into the Detection List
      AddDetection(aSimTime, aSensorPtr, aTargetPtr->GetIndex(), aResult);

      // Go through the other secondary sensors, i.e. index 1...N
      for (size_t i = 1; i < mSensorPtrs.size(); ++i)
      {
         WsfSensor*     sensor  = mSensorPtrs[i];
         WsfSensorMode* modePtr = sensor->GetCurrentMode();

         // Check to see if the secondary sensor is even turned on
         if ((modePtr != nullptr) && sensor->IsTurnedOn())
         {
            // Temporary settings and results from secondary nodes; do not need to retain
            WsfSensor::Settings settings;
            settings.mModeIndex  = modePtr->GetModeIndex();
            settings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);

            WsfSensorResult result;
            result.Reset(settings);
            result.SetCategory(sensor->GetZoneAttenuationModifier());

            if (modePtr->AttemptToDetect(aSimTime, aTargetPtr, settings, result))
            {
               // Load the detection information into the Detection List
               AddDetection(aSimTime, sensor, aTargetPtr->GetIndex(), result);
            }
         }
      }

      // See if there are enough total detections
      // Process the TDOA Detections from the sensors and develop a combined measurement
      unsigned int minimumNumberOfDetections = mMinimumDetections;
      auto         errorModelPtr =
         dynamic_cast<const WsfTRIMSIM_ErrorModel*>(aSensorPtr->GetModeEntry(aResult.mModeIndex)->GetErrorModel());
      if ((errorModelPtr != nullptr) && (errorModelPtr->GetGroundTargetAltitudeError() >= 0.0) &&
          ((aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND) ||
           (aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)))
      {
         minimumNumberOfDetections = mMinimumDetections - 1;
      }

      if (GetDetectionCount() < minimumNumberOfDetections)
      {
         // Failed status set and Pd is reduced.
         aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         aResult.mPd = 0.0;

         if (DebugEnabled())
         {
            auto logger = ut::log::warning() << "Not enough valid detections to perform a TDOA computation.";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Required detections: " << minimumNumberOfDetections;
            logger.AddNote() << "Number of detections = " << GetDetectionCount();
         }
      }
      else
      {
         detected = true;

         if (DebugEnabled())
         {
            auto logger = ut::log::debug() << "Performing a TDOA computation.";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Detections = " << GetDetectionCount();
         }
      }
   }

   return detected;
}

// ============================================================================
//! TRIMSIM/SUPPRESSOR translated code to generate the TDOA Errors.
//! These errors are applied to the primary sensor's detection information
bool WsfTRIMSIM_Processor::ComputeMeasurementErrors(WsfSensor& aSensor)
{
   auto errorModelPtr = dynamic_cast<const WsfTRIMSIM_ErrorModel*>(aSensor.GetCurrentMode()->GetErrorModel());
   if ((errorModelPtr == nullptr) || (!errorModelPtr->ComputeErrors()))
   {
      auto logger =
         ut::log::warning() << "The primary TDOA sensor did not have any non-zero errors defined or was not found.";
      logger.AddNote() << "The TDOA error generation code will not be used. All 'errors' will be set to zero.";
      return false;
   }

   WsfTRIMSIM_SensorComponent* trimsimCompPtr =
      static_cast<WsfTRIMSIM_SensorComponent*>(aSensor.GetComponents().FindComponent("#trimsim"));

   // Set the measurement information invalid
   trimsimCompPtr->SetMeasurementIsValid(false);

   // Get the number of platforms and the number of measurements
   unsigned int numPlatforms    = static_cast<unsigned int>(mDetectionList.size());
   unsigned int numMeasurements = numPlatforms - 1u;

   // Quick return check
   if (numPlatforms == 0)
   {
      return false;
   }

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      PrintTDOAInfo(&aSensor);
   }

   // Set H, B matrices
   UtMatrixd H(numMeasurements, WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE);                // [nM,nS]
   UtMatrixd B(numMeasurements, WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE * numPlatforms); // [nM,nE x nP]

   // Set the E matrix; uses input error information; size = cERROR_SIZE x number of platforms
   int                 size_E = WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE * numPlatforms;
   std::vector<double> E;
   UtVectorUtil::Set(E, size_E - 1, 0.0, 0.0);

   // First, process the primary TDOA's detection information
   // Use the relative position vector and compute the range and unit vector
   double unitVectorP2T_NED[3] = {0.0};
   UtVec3d::Set(unitVectorP2T_NED, mDetectionList[0].mTrueUnitVecNED);

   double primaryPlatVelocityNED[3] = {0.0};
   UtVec3d::Set(primaryPlatVelocityNED, mDetectionList[0].mSensorVelNED);

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      auto logger = ut::log::debug() << "TDOA:";
      logger.AddNote() << "Number of platforms: " << numPlatforms;
      logger.AddNote() << "Number of measurements [nM]: " << numMeasurements;
      logger.AddNote() << "M2T [ENU]: " << unitVectorP2T_NED[1] << " " << unitVectorP2T_NED[0] << " "
                       << -unitVectorP2T_NED[2];
   }

   // Load the primary sensor errors into the E vector
   std::vector<double> errors(errorModelPtr->GetErrors());
   E[0] = errors[0];
   E[1] = errors[1];
   E[2] = errors[2];
   E[3] = errors[3];
   E[4] = errors[4];
   E[5] = errors[5];
   E[6] = 0.0; // For now leave these effects of Atmospheric Refraction at ZERO

   int numErrors = WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE;
   // Go through each secondary detection
   for (unsigned int i = 1; i < numPlatforms; ++i)
   {
      // Add the secondary sensor errors to the E-array
      errors.clear();
      errors.resize(WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE, 0.0);
      errorModelPtr =
         dynamic_cast<const WsfTRIMSIM_ErrorModel*>(mDetectionList[i].mSensorPtr->GetCurrentMode()->GetErrorModel());
      if (errorModelPtr != nullptr)
      {
         errors = (errorModelPtr->GetErrors());
      }
      E[numErrors]     = errors[0];
      E[numErrors + 1] = errors[1];
      E[numErrors + 2] = errors[2];
      E[numErrors + 3] = errors[3];
      E[numErrors + 4] = errors[4];
      E[numErrors + 5] = errors[5];
      E[numErrors + 6] = 0.0; // For now leave these effects of Atmospheric Refraction at ZERO

      numErrors = i * WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE + WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE;

      double unitVectorS2T_NED[3] = {0.0};
      UtVec3d::Set(unitVectorS2T_NED, mDetectionList[i].mTrueUnitVecNED);

      double gammaNED[3] = {0.0};
      UtVec3d::Subtract(gammaNED, unitVectorS2T_NED, unitVectorP2T_NED);

      double scaleFactor     = 1.0 / UtVec3d::Magnitude(gammaNED);
      double unitGammaNED[3] = {0.0};
      UtVec3d::Multiply(unitGammaNED, gammaNED, scaleFactor);

      if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
      {
         auto logger = ut::log::debug() << "TDOA:";
         logger.AddNote() << "S2T [ENU]: " << unitVectorS2T_NED[1] << " " << unitVectorS2T_NED[0] << " "
                          << -unitVectorS2T_NED[2];
         logger.AddNote() << "Scale Factor: " << scaleFactor;
      }

      // Store in the Measurement Vector - H
      // In TrimSim this is the A matrix
      H(i - 1, 0) = unitGammaNED[0];
      H(i - 1, 1) = unitGammaNED[1];
      H(i - 1, 2) = unitGammaNED[2];

      // TDOA observation matrix
      // PRIMARY
      unsigned int l_index = 0;
      B(i - 1, l_index)    = scaleFactor * unitVectorP2T_NED[0];
      B(i - 1, ++l_index)  = scaleFactor * unitVectorP2T_NED[1];
      B(i - 1, ++l_index)  = scaleFactor * unitVectorP2T_NED[2];

      double timeDifference =
         -scaleFactor * (UtVec3d::DotProduct(unitVectorP2T_NED, primaryPlatVelocityNED) - UtMath::cLIGHT_SPEED);

      B(i - 1, ++l_index) = timeDifference;
      B(i - 1, ++l_index) = timeDifference;
      B(i - 1, ++l_index) = timeDifference;
      B(i - 1, ++l_index) = 1.0;

      // SECONDARY

      double secondaryPlatVelocityNED[3] = {0.0};
      UtVec3d::Set(secondaryPlatVelocityNED, mDetectionList[i].mSensorVelNED);

      l_index             = (i - 1) * (l_index + 1) + WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE;
      B(i - 1, l_index)   = -scaleFactor * unitVectorS2T_NED[0];
      B(i - 1, ++l_index) = -scaleFactor * unitVectorS2T_NED[1];
      B(i - 1, ++l_index) = -scaleFactor * unitVectorS2T_NED[2];

      timeDifference =
         scaleFactor * (UtVec3d::DotProduct(unitVectorS2T_NED, secondaryPlatVelocityNED) - UtMath::cLIGHT_SPEED);
      B(i - 1, ++l_index) = timeDifference;
      B(i - 1, ++l_index) = timeDifference;
      B(i - 1, ++l_index) = timeDifference;
      B(i - 1, ++l_index) = -1.0;

   } // for secondary sensor detection

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      PrintMatrix("TDOA: (H) Measurement transition [nM,nS]", H);
   }

   // Create the measurement error covariance matrix e^2
   UtMatrixd E2(size_E, size_E);
   for (int i = 0; i < numErrors; ++i)
   {
      E2(i, i) = E[i] * E[i];
   }

   // Compute the measurement error covariance matrix R = B * diag(E^2) * B'
   UtMatrixd temp1;
   temp1.Multiply(B, E2);

   UtMatrixd B_transpose;
   B_transpose.Transpose(B);

   UtMatrixd R(numMeasurements, numMeasurements);
   R.Multiply(temp1, B_transpose);

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      PrintMatrix("TDOA: (R) Measurement error covariance [nM,nM]", R);
   }

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      // Get H matrix in ENU for ease of comparison
      UtMatrixd H_enu(numMeasurements, WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE);
      for (unsigned int i = 0; i < numMeasurements; ++i)
      {
         H_enu(i, 0) = H(i, 1);
         H_enu(i, 1) = H(i, 0);
         H_enu(i, 2) = -H(i, 2);
      }
      PrintMatrix("TDOA: (H_enu) [nM,nS]", H_enu);

      UtMatrixd B_enu(numMeasurements, WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE * numPlatforms);
      for (unsigned int j = 0; j < numMeasurements; ++j)
      {
         unsigned int numCol = WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE * numPlatforms;
         for (unsigned int i = 0; i < numCol; ++i)
         {
            int remainder = (i % WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE);
            if (remainder == 0)
            {
               B_enu(j, i) = B(j, i + 1);
            }
            else if (remainder == 1)
            {
               B_enu(j, i) = B(j, i - 1);
            }
            else if (remainder == 2)
            {
               B_enu(j, i) = -B(j, i);
            }
            if (remainder == 3)
            {
               B_enu(j, i) = B(j, i + 1) * UtMath::cFT_PER_M;
            }
            else if (remainder == 4)
            {
               B_enu(j, i) = B(j, i - 1) * UtMath::cFT_PER_M;
            }
            else if (remainder == 5)
            {
               B_enu(j, i) = B(j, i) * UtMath::cFT_PER_M;
            }
            else
            {
               B_enu(j, i) = B(j, i);
            }
         }
      }
      PrintMatrix("TDOA: (B_enu)", B_enu);

      std::vector<double> E_enu;
      UtVectorUtil::Set(E_enu, size_E - 1, 0.0, 0.0);
      for (int i = 0; i < size_E - 1; ++i)
      {
         if ((i % WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE) == 0)
         {
            E_enu[i] = E[i + 1] * UtMath::cFT_PER_M;
         }
         else if ((i % (WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE)) == 1)
         {
            E_enu[i] = E[i - 1] * UtMath::cFT_PER_M;
         }
         else if ((i % (WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE)) == 2)
         {
            E_enu[i] = -E[i] * UtMath::cFT_PER_M;
         }
         else
         {
            E_enu[i] = E[i];
         }
      }

      UtMatrixd E2_enu(size_E, size_E);
      if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
      {
         auto logger = ut::log::debug() << "TDOA: (E2_enu)";
         for (int i = 0; i < numErrors; ++i)
         {
            E2_enu(i, i) = E_enu[i] * E_enu[i];
            logger.AddNote() << std::setprecision(4) << E2_enu(i, i);
         }
      }

      UtMatrixd temp1_enu;
      temp1_enu.Multiply(B_enu, E2_enu);

      UtMatrixd B_transpose_enu;
      B_transpose_enu.Transpose(B_enu);

      UtMatrixd R_enu(numMeasurements, numMeasurements);
      R_enu.Multiply(temp1_enu, B_transpose_enu);

      PrintMatrix("TDOA: (R_enu) [nM,nM]", R_enu);

      // inv(R)   (nM,nM)
      UtMatrixd R_invert;
      try
      {
         R_invert.Invert(R_enu);
      }
      catch (UtMatrixd::SingularMatrixError&)
      {
         auto logger = ut::log::warning() << "Measurement error covariance matrix (R) is not invertible";
         logger.AddNote() << "T = " << GetSimulation()->GetSimTime();
         logger.AddNote() << "Platform: " << aSensor.GetPlatform()->GetName();
         return false;
      }

      PrintMatrix("TDOA: (R_invert) [nM,nM]", R_invert);

      // H'       (nS,nM)
      UtMatrixd H_transpose;
      H_transpose.Transpose(H_enu);

      // temp matrix (nS,nM)
      // UtMatrixd pilf1;
      // pilf1.Multiply(H_transpose,R_invert);

      // Compute the measurement update to the information matrix (nS,nS)
      // UtMatrixd pilf;
      // pilf.Multiply(pilf1,H_enu);

      // UtMatrixd P_enu;
      // P_enu.Invert(pilf);

      // PrintMatrix("TDOA: (inv P_enu) ", pilf);
      // PrintMatrix("TDOA: (P_enu) ", P_enu);
   }

   // Get linear transformation matrix using Cholesky Decomposition (C)
   UtMatrixd C;
   bool      result(false);
   C.CholeskyDecomposition(R, result);

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      PrintMatrix("TDOA: (C) Cholesky factorization [nM,nM]", C);
   }

   if ((result == false) && (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG)))
   {
      auto logger = ut::log::warning() << "Cholesky Decomposition failed.";
      logger.AddNote() << "R matrix is not positive definite.";
      return false;
   }

   // Monte-Carlo for uncorrelated random variable with unit variance
   UtMatrixd   uv(numMeasurements);
   ut::Random& rnd = GetSimulation()->GetRandom();
   for (unsigned int i = 0; i < numMeasurements; ++i)
   {
      uv(i) = rnd.Normal();
   }

   // Get the measurement matrix (Z)
   UtMatrixd Z(numMeasurements);
   for (unsigned int i = 0; i < numMeasurements; ++i)
   {
      for (unsigned int j = 0; j <= i; ++j)
      {
         // Weight and add to construct correlated random variable
         Z(i) += C(i, j) * uv(j);
      }
   }

   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      PrintMatrix("TDOA: (Z) Measurement [nM]", Z);
   }

   // Save the measurement information
   trimsimCompPtr->SetMeasurement(Z);
   trimsimCompPtr->SetMeasurementErrorCovariance(R);
   trimsimCompPtr->SetMeasurementTransition(H);
   trimsimCompPtr->SetMeasurementIsValid(true);

   return true;
}

// ============================================================================
void WsfTRIMSIM_Processor::AddDetection(double           aSimTime,
                                        WsfSensor*       aSensorPtr,
                                        size_t           aTargetPlatformIndex,
                                        WsfSensorResult& aResult)
{
   Detection detection;
   detection.mSensorPtr           = aSensorPtr;
   detection.mTargetPlatformIndex = aTargetPlatformIndex;
   UtVec3d::Set(detection.mSensorLocWCS, aResult.mRcvrLoc.mLocWCS);
   aSensorPtr->GetPlatform()->GetVelocityNED(detection.mSensorVelNED);
   aSensorPtr->GetPlatform()->GetVelocityWCS(detection.mSensorVelWCS);
   detection.mTimeOfDetection = aSimTime;

   double trueUnitVecWCS[3] = {0.0};
   if (aResult.mRcvrToTgt.mRange >= 0.0)
   {
      detection.mTrueRange = aResult.mRcvrToTgt.mRange;
      UtVec3d::Set(trueUnitVecWCS, aResult.mRcvrToTgt.mTrueUnitVecWCS);
   }
   else if (aResult.mRcvrToXmtr.mRange >= 0.0)
   {
      // Passive receiver detection.
      detection.mTrueRange = aResult.mRcvrToXmtr.mRange;
      UtVec3d::Set(trueUnitVecWCS, aResult.mRcvrToXmtr.mTrueUnitVecWCS);
   }
   else
   {
      assert((aResult.mRcvrToTgt.mRange >= 0.0) || (aResult.mRcvrToXmtr.mRange >= 0.0));
   }
   aSensorPtr->GetEM_Rcvr(0).GetAntenna()->ConvertWCS_VectorToNED(trueUnitVecWCS, detection.mTrueUnitVecNED);
   mDetectionList.push_back(detection);
}

// ============================================================================
//! Copy constructor (for Clone()).
WsfTRIMSIM_Processor::WsfTRIMSIM_Processor(const WsfTRIMSIM_Processor& aSrc)
   : WsfSensorProcessor(aSrc)
   , mMinimumDetections(aSrc.mMinimumDetections)
   , mDetectionList(aSrc.mDetectionList)
   , mSensorPtrs()
{
}

// ============================================================================
void WsfTRIMSIM_Processor::CheckSensors()
{
   // Check to see if there are enough nodes to do TDOA;
   // This should at least be the minimum number of detections
   // minus one (special case for ground detections).
   // If not, demote them all to cSTAND_ALONE
   if (mSensorPtrs.size() < (mMinimumDetections - 1))
   {
      if (DebugEnabled())
      {
         auto logger = ut::log::warning() << "Not enough nodes to perform a TRIMSIM computation.";
         logger.AddNote() << "All TRIMSIM sensors are being 'demoted' to STAND_ALONE Mode.";
         logger.AddNote() << "T = " << GetSimulation()->GetSimTime();
         logger.AddNote() << "Number of nodes: " << mSensorPtrs.size();
         logger.AddNote() << "Required detections: " << mMinimumDetections;
      }

      // Set all sensors to be self-controlled
      for (auto& sensorPtr : mSensorPtrs)
      {
         // Save the state of whether the sensor is currently on
         bool sensorIsTurnedOn(sensorPtr->IsTurnedOn());

         // Have to turn sensor off to change some settings
         if (sensorIsTurnedOn)
         {
            sensorPtr->TurnOff(GetSimulation()->GetSimTime());
         }

         sensorPtr->SetIsSlave(GetSimulation()->GetSimTime(), false); // cannot be changed if sensor is on
         sensorPtr->SetIsPrivate(false);

         // Turn back on if necessary
         if (sensorIsTurnedOn)
         {
            // Turn the sensors back on and schedule updates
            GetSimulation()->TurnPartOn(GetSimulation()->GetSimTime(), sensorPtr);
         }
      }
   }
}

//============================================================================
bool WsfTRIMSIM_Processor::SensorAdded(WsfSensor* aSensorPtr)
{
   bool added = false;
   if (aSensorPtr->IsClassPassive())
   {
      if (find(mSensorPtrs.begin(), mSensorPtrs.end(), aSensorPtr) == mSensorPtrs.end())
      {
         // Sensor and platform was not found, add them
         mSensorPtrs.push_back(aSensorPtr);

         if (mSensorPtrs.size() == 1)
         {
            // if this is the first sensor added then add the TRIMSIM component to it
            WsfTRIMSIM_SensorComponent* trimsimComponent = WsfTRIMSIM_SensorComponent::FindOrCreate(*aSensorPtr);
            trimsimComponent->SetProcessor(this);
         }
         else
         {
            // Set all other sensors to externally controlled, as they will be controlled by
            // the processor via the component. This cannot be changed if sensor is on.
            aSensorPtr->SetIsSlave(GetSimulation()->GetSimTime(), true);

            // Mark the sensor as private so DIS/HLA will not send emissions PDU for the constituent.
            // (They will be combined into the composite).
            aSensorPtr->SetIsPrivate(true);
         }

         if (IsTurnedOn())
         {
            CheckSensors();
         }
      }
      added = true;
   }
   else
   {
      auto logger = ut::log::warning() << "Cannot add the non-passive type sensor to TRIMSIM processor.";
      logger.AddNote() << "Processor Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Processor: " << GetName();
      logger.AddNote() << "Sensor Platform: " << aSensorPtr->GetPlatform()->GetName();
      logger.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }
   return added;
}

//============================================================================
bool WsfTRIMSIM_Processor::SensorRemoved(WsfSensor* aSensorPtr)
{
   bool removed = false;
   auto spi     = find(mSensorPtrs.begin(), mSensorPtrs.end(), aSensorPtr);
   if (spi != mSensorPtrs.end())
   {
      // Sensor and platform was found, remove them
      mSensorPtrs.erase(spi);

      aSensorPtr->GetComponents().DeleteComponent("#trimsim", cWSF_COMPONENT_TRIMSIM_SENSOR);
      aSensorPtr->SetIsSlave(GetSimulation()->GetSimTime(), false); // cannot be changed if sensor is on
      aSensorPtr->SetIsPrivate(false);

      if (!mSensorPtrs.empty())
      {
         // Add the TRIMSIM component to the first sensor in the list
         // even if it is already added we can repeat without any issues.
         WsfTRIMSIM_SensorComponent* trimsimComponent = WsfTRIMSIM_SensorComponent::FindOrCreate(*mSensorPtrs[0]);
         trimsimComponent->SetProcessor(this);
      }

      if (IsTurnedOn())
      {
         CheckSensors();
      }
      removed = true;
   }
   return removed;
}

// ============================================================================
void WsfTRIMSIM_Processor::PrintTDOAInfo(WsfSensor* aSensorPtr)
{
   WsfSimulation& sim    = *GetSimulation();
   auto           logger = ut::log::info() << "TDOA_ERROR_UPDATE:";
   logger.AddNote() << "T = " << UtTime(sim.GetSimTime(), UtTime::FmtHMS + 2);
   logger.AddNote() << "Platform: " << aSensorPtr->GetPlatform()->GetName();

   WsfPlatform* tgtPlatformPtr = sim.GetPlatformByIndex(mDetectionList[0].mTargetPlatformIndex);
   if (mDetectionList[0].mTargetPlatformIndex == 0)
   {
      logger.AddNote() << "Target Platform: UNKNOWN";
   }
   else
   {
      if ((tgtPlatformPtr != nullptr) && (tgtPlatformPtr->GetNameId() != 0))
      {
         logger.AddNote() << "Target Platform: " << tgtPlatformPtr->GetName();
      }
   }
   logger.AddNote() << "Sensor: " << aSensorPtr->GetName();

   WsfPlatform* sensorPlatformPtr = aSensorPtr->GetPlatform();
   double       lla[3]            = {0.0};
   sensorPlatformPtr->GetLocationLLA(lla[0], lla[1], lla[2]);

   // Originator
   {
      auto originatorNote = logger.AddNote() << "Originator:";
      originatorNote.AddNote() << "Lat: " << UtLatPos(lla[0], UtAngle::FmtDMS + 2);
      originatorNote.AddNote() << "Lon: " << UtLonPos(lla[1], UtAngle::FmtDMS + 2);
      originatorNote.AddNote() << "Alt: " << std::setprecision(8) << lla[2] << " m";
   }

   // Target
   if (tgtPlatformPtr != nullptr)
   {
      tgtPlatformPtr->GetLocationLLA(lla[0], lla[1], lla[2]);

      auto tgtTruthNote = logger.AddNote() << "Truth:";
      tgtTruthNote.AddNote() << "Lat: " << UtLatPos(lla[0], UtAngle::FmtDMS + 2);
      tgtTruthNote.AddNote() << "Lon: " << UtLonPos(lla[1], UtAngle::FmtDMS + 2);
      tgtTruthNote.AddNote() << "Alt: " << std::setprecision(8) << lla[2] << " m";

      double targetLocWCS[3] = {0.0};
      tgtPlatformPtr->GetLocationWCS(targetLocWCS);
      double targetLocNED[3] = {0.0};
      sensorPlatformPtr->ConvertWCSToNED(targetLocWCS, targetLocNED);

      double bearing = atan2(targetLocNED[1], targetLocNED[0]);
      bearing        = UtMath::NormalizeAngle0_TwoPi(bearing);
      double angle   = bearing * UtMath::cDEG_PER_RAD;
      if (fabs(angle) < 1.0E-12)
      {
         angle = 0.0;
      }

      double range     = UtVec3d::Magnitude(targetLocNED);
      double elevation = 0.0;
      if (range > 0.0)
      {
         elevation = asin(-targetLocNED[2] / range);
      }
      angle = elevation * UtMath::cDEG_PER_RAD;
      if (fabs(angle) < 1.0E-12)
      {
         angle = 0.0;
      }

      tgtTruthNote.AddNote() << "Range: " << UtVec3d::Magnitude(targetLocNED) << " m";
      tgtTruthNote.AddNote() << "Bearing: " << angle << " deg";
      tgtTruthNote.AddNote() << "Elevation: " << angle << " deg";
   }
}

// ============================================================================
//! Convenience routine to easily print TDOA algorithm matrices
// static
void WsfTRIMSIM_Processor::PrintMatrix(const std::string& aTitle, UtMatrixd& aMatrix)
{
   auto         logger = ut::log::info() << aTitle << std::setprecision(6);
   unsigned int nRows(aMatrix.GetRows());
   unsigned int nCols(aMatrix.GetCols());
   for (unsigned int kk = 0; kk < nRows; ++kk)
   {
      auto rowNote = logger.AddNote();
      for (unsigned int jj = 0; jj < nCols; ++jj)
      {
         rowNote << std::setw(8) << aMatrix(kk, jj) << "  ";
      }
   }
}

// ============================================================================
// ============================================================================
namespace
{
class TRIMSIMComponentFactory : public WsfComponentFactory<WsfSensor>
{
public:
   void PreInput(WsfSensor& aParent) override
   {
      // This must always present on RF sensors.
      // It isn't needed on acoustic or optical sensors.
      if (aParent.GetClass() & WsfSensor::cRADIO)
      {
         WsfTRIMSIM_SensorComponent::FindOrCreate(aParent);
      }
   }
};
} // namespace

// ============================================================================
//! Register the component factory that handles input for this component.
void WsfTRIMSIM_SensorComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<TRIMSIMComponentFactory>());
}

// ============================================================================
//! Find the instance of this component attached to the specified sensor.
WsfTRIMSIM_SensorComponent* WsfTRIMSIM_SensorComponent::Find(const WsfSensor& aSensor)
{
   WsfTRIMSIM_SensorComponent* componentPtr(nullptr);
   aSensor.GetComponents().FindByRole<WsfTRIMSIM_SensorComponent>(componentPtr);
   return componentPtr;
}

// ============================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
WsfTRIMSIM_SensorComponent* WsfTRIMSIM_SensorComponent::FindOrCreate(WsfSensor& aSensor)
{
   WsfTRIMSIM_SensorComponent* componentPtr = Find(aSensor);
   if (componentPtr == nullptr)
   {
      componentPtr = new WsfTRIMSIM_SensorComponent;
      // Determine if the parent is of type WsfRadarSensor. This may be needed during input processing
      // and it needs to be done here. It cannot be done in 'ComponentParentChanged' because the RTTI
      // information is not fully available at that time. (JAJ)
      aSensor.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// ============================================================================
WsfTRIMSIM_SensorComponent::WsfTRIMSIM_SensorComponent()
   : // mTDOA_Mode(WsfTRIMSIM_Processor::cSTAND_ALONE),
   mMeasurement()
   , mMeasurementErrorCovariance()
   , mMeasurementTransition()
   , mMeasurementIsValid(false)
   , mTRIMSIM_ProcessorPtr(nullptr)
{
}

// ============================================================================
WsfTRIMSIM_SensorComponent::WsfTRIMSIM_SensorComponent(const WsfTRIMSIM_SensorComponent& aSrc)
   : mMeasurement(aSrc.mMeasurement)
   , mMeasurementErrorCovariance(aSrc.mMeasurementErrorCovariance)
   , mMeasurementTransition(aSrc.mMeasurementTransition)
   , mMeasurementIsValid(aSrc.mMeasurementIsValid)
   , mTRIMSIM_ProcessorPtr(nullptr)
{
}

// ============================================================================
WsfComponent* WsfTRIMSIM_SensorComponent::CloneComponent() const
{
   return new WsfTRIMSIM_SensorComponent(*this);
}

// ============================================================================
WsfStringId WsfTRIMSIM_SensorComponent::GetComponentName() const
{
   return "#trimsim";
}

// ============================================================================
const int* WsfTRIMSIM_SensorComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_TRIMSIM_SENSOR, cWSF_COMPONENT_SENSOR_COMPONENT, cWSF_COMPONENT_NULL};
   return roles;
}

// ============================================================================
void* WsfTRIMSIM_SensorComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_TRIMSIM_SENSOR)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_SENSOR_COMPONENT)
   {
      return (WsfSensorComponent*)this;
   }
   return nullptr;
}

// ============================================================================
void WsfTRIMSIM_SensorComponent::PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   mTRIMSIM_ProcessorPtr->AttemptToDetect(aSimTime, GetSensor(), aTargetPtr, aResult);
}

// ============================================================================
void WsfTRIMSIM_SensorComponent::UpdateTrack(double           aSimTime,
                                             WsfTrack*        aTrackPtr,
                                             WsfPlatform*     aTargetPtr,
                                             WsfSensorResult& aResult)
{
   // Quick check for return
   if (aTrackPtr == nullptr)
   {
      return;
   }

   WsfSensor* sensorPtr = GetSensor(); // ease of use

   auto errorModelPtr =
      dynamic_cast<const WsfTRIMSIM_ErrorModel*>(GetSensor()->GetModeEntry(aResult.mModeIndex)->GetErrorModel());
   if ((errorModelPtr != nullptr) && (!errorModelPtr->ComputeErrors()))
   {
      auto logger = ut::log::warning() << "The primary TDOA sensor did not have any non - zero errors defined.";
      logger.AddNote() << "The TDOA error generation code will not be used. All 'errors' will be set to zero.";
      return;
   }
   mTRIMSIM_ProcessorPtr->ComputeMeasurementErrors(*sensorPtr);

   // Quick check for return
   if (!mMeasurementIsValid)
   {
      return;
   }

   // inv(R)   (nM,nM)
   UtMatrixd R_invert;
   try
   {
      R_invert.Invert(mMeasurementErrorCovariance);
   }
   catch (UtMatrixd::SingularMatrixError&)
   {
      auto logger = ut::log::warning() << "Measurement error covariance matrix (R) is not invertible";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Platform: " << sensorPtr->GetPlatform()->GetName();
      return;
   }

   // H'       (nS,nM)
   UtMatrixd H_transpose;
   H_transpose.Transpose(mMeasurementTransition);

   // temp matrix (nS,nM)
   UtMatrixd pilf1;
   pilf1.Multiply(H_transpose, R_invert);

   // Compute the measurement update to the information matrix (nS,nS)
   UtMatrixd pilf;
   pilf.Multiply(pilf1, mMeasurementTransition);

   // Update the track information matrix

   WsfAttributeContainer& auxData = aTrackPtr->GetAuxData();
   if (!auxData.AttributeExists("tdoa_information_matrix"))
   {
      // Add a zero filled matrix to the track
      UtMatrixd tempZeroInfoMatrix(WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE, WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE);
      if ((H_transpose.GetCols() < WsfTRIMSIM_Processor::cDEFAULT_MINIMUM_DETECTIONS) &&
          ((errorModelPtr != nullptr) && (errorModelPtr->GetGroundTargetAltitudeError() >= 0.0)) &&
          ((aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND) ||
           (aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)))
      {
         // Initialize the information matrix to specify a
         // known height error of 1 meter by setting the last element to 1
         unsigned int nSlot               = WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE - 1u;
         tempZeroInfoMatrix(nSlot, nSlot) = errorModelPtr->GetGroundTargetAltitudeError();
      }

      auxData.Assign("tdoa_information_matrix", tempZeroInfoMatrix);
   }

   UtMatrixd piOld;
   auxData.GetAttribute("tdoa_information_matrix").Get(piOld);

   UtMatrixd pi;
   pi = piOld + pilf;
   auxData.Assign("tdoa_information_matrix", pi);

   // Update information state vector due to measurement (nS,1)

   UtMatrixd pixlf;
   pixlf.Multiply(pilf1, mMeasurement);

   if (!auxData.AttributeExists("tdoa_information_state_vector"))
   {
      // Add a zero filled matrix to the track
      UtMatrixd tempZeroInfoVector(WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE);
      auxData.Assign("tdoa_information_state_vector", tempZeroInfoVector);
   }
   UtMatrixd pixOld;
   auxData.GetAttribute("tdoa_information_state_vector").Get(pixOld);

   UtMatrixd pix;
   pix = pixOld + pixlf;
   auxData.Assign("tdoa_information_state_vector", pix);

   // Invert pi to get p  (nS,nS)
   UtMatrixd P;
   try
   {
      P.Invert(pi);
   }
   catch (UtMatrixd::SingularMatrixError&)
   {
      auto logger = ut::log::warning() << "State Estimation Covariance (P) is not invertible";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Platform: " << sensorPtr->GetPlatform()->GetName();
      return;
   }

   if (mTRIMSIM_ProcessorPtr->DebugEnabled() && (mTRIMSIM_ProcessorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      WsfTRIMSIM_Processor::PrintMatrix("TDOA: (P-1) Information Matrix [nM,nM]", pi);
      WsfTRIMSIM_Processor::PrintMatrix("TDOA: (P) State Estimation Covariance [nM,nM]", P);
   }

   // Multiply pix by p to get the (e,n,u) states X (nS,1)
   UtMatrixd X;
   X.Multiply(P, pix);

   if (mTRIMSIM_ProcessorPtr->DebugEnabled() && (mTRIMSIM_ProcessorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      WsfTRIMSIM_Processor::PrintMatrix("TDOA: (X) State Estimation Vector [nS]", X);
   }

   // Update sensor track with measurement
   // Convert the measurement error from NED to RBE
   double tdoaErrorsNED[3] = {X(0), X(1), X(2)};
   if (mTRIMSIM_ProcessorPtr->DebugEnabled() && (mTRIMSIM_ProcessorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      auto logger = ut::log::debug() << "TDOA: ";
      logger.AddNote() << "T= " << aSimTime;
      logger.AddNote() << "Errors NED: " << tdoaErrorsNED[0] << " " << tdoaErrorsNED[1] << " " << tdoaErrorsNED[2];
   }

   // Convert the TDOA errors from NED to WCS
   double tdoaErrorsWCS[3] = {0.0};
   sensorPtr->GetEM_Rcvr(0).GetAntenna()->ConvertNED_VectorToWCS(tdoaErrorsNED, tdoaErrorsWCS);

   if (mTRIMSIM_ProcessorPtr->DebugEnabled() && (mTRIMSIM_ProcessorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      auto logger = ut::log::debug() << "TDOA: ";
      logger.AddNote() << "T=" << aSimTime;
      logger.AddNote() << "Errors WCS: " << tdoaErrorsWCS[0] << " " << tdoaErrorsWCS[1] << " " << tdoaErrorsWCS[2];
   }

   // Get the relative target location in WCS
   double relativeTgtLocWCS[3];
   if (aResult.mRcvrToTgt.mRange >= 0.0)
   {
      sensorPtr->GetRelativeLocationWCS(aResult.mRcvrToTgt.mTrueAz,
                                        aResult.mRcvrToTgt.mTrueEl,
                                        aResult.mRcvrToTgt.mRange,
                                        relativeTgtLocWCS);
   }
   else if (aResult.mRcvrToXmtr.mRange >= 0.0)
   {
      // Passive receiver detection.
      sensorPtr->GetRelativeLocationWCS(aResult.mRcvrToXmtr.mTrueAz,
                                        aResult.mRcvrToXmtr.mTrueEl,
                                        aResult.mRcvrToXmtr.mRange,
                                        relativeTgtLocWCS);
   }
   else
   {
      assert((aResult.mRcvrToTgt.mRange >= 0.0) || (aResult.mRcvrToXmtr.mRange >= 0.0));
      UtVec3d::Set(relativeTgtLocWCS, 0.0);
   }

   // Update the reported location in WCS by applying the errors to the relative location
   UtVec3d::Add(relativeTgtLocWCS, relativeTgtLocWCS, tdoaErrorsWCS);
   double reportedLocWCS[3];
   sensorPtr->GetLocationWCS(relativeTgtLocWCS, reportedLocWCS);
   aResult.mMeasurement.SetLocationWCS(reportedLocWCS);

   // Save the RBE errors in the result
   ApplyMeasurementErrors(aSimTime, aTrackPtr, aTargetPtr, aResult);

   if (mTRIMSIM_ProcessorPtr->DebugEnabled() && (mTRIMSIM_ProcessorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      auto logger = ut::log::debug() << "TDOA:";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Range Error: " << aResult.mMeasurement.GetRangeError();
      logger.AddNote() << "Bearing Error: " << aResult.mMeasurement.GetBearingError();
      logger.AddNote() << "Elevation Error: " << aResult.mMeasurement.GetElevationError();
   }

   // Add the list of detecting platforms to the track
   std::string detectorStr;
   for (size_t i = 0; i < mTRIMSIM_ProcessorPtr->GetDetectionCount(); ++i)
   {
      detectorStr += ' ' + mTRIMSIM_ProcessorPtr->GetDetections()[i].mSensorPtr->GetPlatform()->GetName() + "." +
                     mTRIMSIM_ProcessorPtr->GetDetections()[i].mSensorPtr->GetName();
   }
   auxData.Assign("tdoa_detectors", detectorStr);

   // Do this last!!
   // Insert covariance data in sensor track
   aTrackPtr->SetTrackType(WsfTrack::cFILTERED_SENSOR);
   ConvertMatrix(P, aTrackPtr);


   // Apply the TDOA measurement information.
   // Currently we don't know if the result location data are actually valid.
   // todo-mjm this should change once WsfMeasurement replaces Sensor::Result
   // Also, this check has to be done after the call above to WsfSensorMode::UpdateTrack as
   // the location validity flag will be set false.
   aResult.mMeasurement.GetLocationWCS(reportedLocWCS);
   if ((UtVec3d::MagnitudeSquared(reportedLocWCS) > 1.0) && (!aTrackPtr->LocationValid()))
   {
      aTrackPtr->SetLocationWCS(reportedLocWCS);
      aTrackPtr->SetWCS_LocationValid(true);
   }
}

// ============================================================================
void WsfTRIMSIM_SensorComponent::CorrectForAltitude(double       aTargetLocWCS[3], // in / out
                                                    WsfPlatform* aTargetPtr,
                                                    double       aDefaultAltitude)
{
   if ((aTargetPtr != nullptr) && ((aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND) ||
                                   (aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)))
   {
      double lat, lon, alt = 0.0;
      UtEllipsoidalEarth::ConvertECEFToLLA(aTargetLocWCS, lat, lon, alt);
      alt = aDefaultAltitude;
      wsf::Terrain terrain(wsf::TerrainInterface::Get(GetSensor()->GetScenario()));
      if (terrain.IsEnabled())
      {
         float elev = 0.0;
         terrain.GetElevApprox(lat, lon, elev);
         alt = elev;
      }
      UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, alt, aTargetLocWCS);
   }
}

// ============================================================================
//! Converts and NED matrix to a WCS matrix
void WsfTRIMSIM_SensorComponent::ConvertMatrix(UtMatrixd& aMatrix, WsfTrack* aTrackPtr)
{
   // Calculate the WCS to NED Transform
   double snrLocationWCS[3];
   GetSensor()->GetPlatform()->GetLocationWCS(snrLocationWCS);
   double platformLocationErrorWCS[3];
   GetSensor()->GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
   UtVec3d::Add(snrLocationWCS, snrLocationWCS, platformLocationErrorWCS);

   double lat, lon, alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(snrLocationWCS, lat, lon, alt);

   double locWCS[3];
   double WCS_ToNED_Transform[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, locWCS);

   WsfCovariance covar(aMatrix);
   WsfCovariance::ConvertFromFrameA_ToFrameB(covar, WCS_ToNED_Transform);

   aTrackPtr->SetStateCovarianceValid(true);
   aTrackPtr->SetStateCovariance(GetSimulation()->GetSimTime(), covar);
}

// ============================================================================
// private
void WsfTRIMSIM_SensorComponent::ApplyMeasurementErrors(double           aSimTime,
                                                        WsfTrack*        aTrackPtr,
                                                        WsfPlatform*     aTargetPtr,
                                                        WsfSensorResult& aResult)
{
   // Get truth
   double truthRange(0.0);
   double truthBearingRelative(0.0);
   double truthElevationRelative(0.0);
   double truthLocationWCS[3] = {0.0};
   if (aResult.mRcvrToTgt.mRange >= 0.0)
   {
      truthRange             = aResult.mRcvrToTgt.mRange;
      truthBearingRelative   = aResult.mRcvrToTgt.mTrueAz;
      truthElevationRelative = aResult.mRcvrToTgt.mTrueEl;
      UtVec3d::Set(truthLocationWCS, aResult.mTgtLoc.mLocWCS);
   }
   else if (aResult.mRcvrToXmtr.mRange >= 0.0)
   {
      // Passive receiver detection.
      truthRange             = aResult.mRcvrToXmtr.mRange;
      truthBearingRelative   = aResult.mRcvrToXmtr.mTrueAz;
      truthElevationRelative = aResult.mRcvrToXmtr.mTrueEl;
      UtVec3d::Set(truthLocationWCS, aResult.mXmtrLoc.mLocWCS);
   }

   // Set the result with truth
   WsfMeasurement& measurement = aResult.mMeasurement;
   measurement.SetUpdateTime(GetSimulation()->GetSimTime());
   measurement.SetRange(truthRange);
   measurement.SetRangeValid(true);
   measurement.SetBearing(truthBearingRelative);
   measurement.SetBearingValid(true);
   measurement.SetElevation(truthElevationRelative);
   measurement.SetElevationValid(true);

   // Apply the error
   double measuredRange(0.0);
   double measuredBearing(0.0);
   double measuredElevation(0.0);
   double reportedLocWCS[3];
   measurement.GetLocationWCS(reportedLocWCS);
   UtMeasurementUtil::RangeBearingElevation(aResult.mRcvrLoc.mLocWCS,
                                            reportedLocWCS,
                                            measuredRange,
                                            measuredBearing,
                                            measuredElevation);

   // Range
   measurement.SetRangeError(0.0);
   // if (mReportsRange || mReportsLocation)
   if (aTrackPtr->RangeValid() || aTrackPtr->LocationValid())
   {
      measurement.SetRangeError(measuredRange - truthRange);
      measurement.SetRange(measuredRange);
      measurement.SetRangeValid(true);
   }

   // Azimuth
   measurement.SetBearingError(0.0);
   // if (mReportsBearing || mReportsLocation)
   if (aTrackPtr->BearingValid() || aTrackPtr->LocationValid())
   {
      double tempRange(0.0);
      double trueBearing(0.0);
      double trueElevation(0.0);
      UtMeasurementUtil::RangeBearingElevation(aResult.mRcvrLoc.mLocWCS, truthLocationWCS, tempRange, trueBearing, trueElevation);

      measurement.SetBearingError(measuredBearing - trueBearing);
      measurement.SetBearing(measurement.GetBearing() + measurement.GetBearingError());
   }

   // Elevation
   measurement.SetElevationError(0.0);
   // if (mReportsElevation || mReportsLocation)
   if (aTrackPtr->ElevationValid() || aTrackPtr->LocationValid())
   {
      double tempRange(0.0);
      double trueBearing(0.0);
      double trueElevation(0.0);
      UtMeasurementUtil::RangeBearingElevation(aResult.mRcvrLoc.mLocWCS, truthLocationWCS, tempRange, trueBearing, trueElevation);

      measurement.SetElevationError(measuredElevation - trueElevation);
      measurement.SetElevation(measurement.GetElevation() + measurement.GetElevationError());
   }

   WsfSensorMode* modePtr = GetSensor()->GetModeEntry(aResult.mModeIndex);

   measurement.GetLocationWCS(reportedLocWCS);
   modePtr->UpdateTrackPosition(aTrackPtr, aResult, reportedLocWCS);

   // The following provide information for higher-fidelity tracking operation.
   if (measurement.SensorAzimuthValid()) // Should always be true
   {
      aTrackPtr->SetSensorAzimuthValid(true);
      aTrackPtr->SetSensorAzimuth(measurement.GetSensorAzimuth());
      aTrackPtr->SetSensorAzimuthError(measurement.GetSensorAzimuthError());
   }

   if (measurement.SensorElevationValid()) // Should always be true
   {
      aTrackPtr->SetSensorElevationValid(true);
      aTrackPtr->SetSensorElevation(measurement.GetSensorElevation());
      aTrackPtr->SetSensorElevationError(measurement.GetSensorElevationError());
   }
}

// ============================================================================
// ============================================================================

const unsigned int WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE = 7;
const unsigned int WsfTRIMSIM_ErrorModel::cTDOA_STATE_SIZE = 3;

// ============================================================================
WsfTRIMSIM_ErrorModel::WsfTRIMSIM_ErrorModel()
   : wsf::SensorErrorModelBase()
   , mComputeErrors(false)
   , mErrors()
   , mGroundTargetAltitudeError(-1.0)
{
   // Initialize the error count
   mErrors.resize(WsfTRIMSIM_ErrorModel::cTDOA_ERROR_SIZE, 0.0);
}

// ============================================================================
WsfTRIMSIM_ErrorModel* WsfTRIMSIM_ErrorModel::Clone() const
{
   return new WsfTRIMSIM_ErrorModel(*this);
}

// ============================================================================
//! Factory method for WsfSensorErrorModelTypes.
//! This is called by WsfSensorErrorModelTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
std::unique_ptr<wsf::SensorErrorModelBase> WsfTRIMSIM_ErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if ((aTypeName == "WSF_TRIMSIM_ERROR") || // WSF_TRIMSIM_ERROR is undocumented and will be removed in the future.
       (aTypeName == "trimsim_error"))
   {
      return ut::make_unique<WsfTRIMSIM_ErrorModel>();
   }
   return nullptr;
}

bool WsfTRIMSIM_ErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "north_position_error_sigma")
   {
      aInput.ReadValueOfType(mErrors[0], UtInput::cLENGTH);
   }
   else if (command == "east_position_error_sigma")
   {
      aInput.ReadValueOfType(mErrors[1], UtInput::cLENGTH);
   }
   else if (command == "down_position_error_sigma")
   {
      aInput.ReadValueOfType(mErrors[2], UtInput::cLENGTH);
   }
   else if ((command == "platform_position_errors") ||
            (command == "platform_position_errors_NED")) // left for backward compatibility
   {
      double northErr;
      double eastErr;
      double downErr;
      aInput.ReadValueOfType(northErr, UtInput::cLENGTH);
      aInput.ReadValueOfType(eastErr, UtInput::cLENGTH);
      aInput.ReadValueOfType(downErr, UtInput::cLENGTH);

      // Put this data into the error array (NED)
      mErrors[0] = northErr;
      mErrors[1] = eastErr;
      mErrors[2] = downErr;
   }
   else if ((command == "reference_time_error") || (command == "reference_timing_error")) // left for backward compatibility
   {
      double refTimingError;
      aInput.ReadValueOfType(refTimingError, UtInput::cTIME);

      mErrors[3] = refTimingError;
   }
   else if ((command == "inter_system_time_delay") ||
            (command == "intersystem_time_delay")) // left for backward compatibility
   {
      double timeDelay;
      aInput.ReadValueOfType(timeDelay, UtInput::cTIME);

      mErrors[4] = timeDelay;
   }
   else if (command == "sensor_timing_error")
   {
      double timingError;
      aInput.ReadValueOfType(timingError, UtInput::cTIME);

      mErrors[5] = timingError;
   }
   else if (command == "atmospheric_refraction_residual")
   {
      double atmosResidual;
      aInput.ReadValue(atmosResidual);

      mErrors[6] = atmosResidual;
   }
   else if (command == "ground_target_altitude_error")
   {
      aInput.ReadValueOfType(mGroundTargetAltitudeError, UtInput::cLENGTH);
   }
   else
   {
      myCommand = wsf::SensorErrorModelBase::ProcessInput(aInput);
   }
   return myCommand;
}

// ============================================================================
bool WsfTRIMSIM_ErrorModel::Initialize(WsfSensorMode* aSensorModePtr)
{
   bool ok = wsf::SensorErrorModelBase::Initialize(aSensorModePtr);

   // If errors are defined and anything is non-zero...set a flag that will the call the error generation code.
   mComputeErrors = false;
   if ((!mErrors.empty()) && ((mErrors[0] != 0.0) || (mErrors[1] != 0.0) || (mErrors[2] != 0.0) ||
                              (mErrors[3] != 0.0) || (mErrors[4] != 0.0) || (mErrors[5] != 0.0) || (mErrors[6] != 0.0)))
   {
      mComputeErrors = true;
   }
   else if (aSensorModePtr->GetSensor()->DebugEnabled())
   {
      auto logger = ut::log::warning() << "The main TDOA sensor did not have any non-zero errors defined.";
      logger.AddNote() << "The TDOA error generation code will not be used. All 'errors' will be set to zero.";
   }
   return ok;
}
