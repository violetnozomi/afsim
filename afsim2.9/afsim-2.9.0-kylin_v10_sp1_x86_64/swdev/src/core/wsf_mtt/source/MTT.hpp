// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_MTT_HPP
#define WSF_MTT_HPP

#include <map>
#include <string>

#include "MTT_ActiveTrack.hpp"
class MTT_Measurement;
#include "MTT_Parameters.hpp"
class MTT_PerceivedCluster;
#include "MTT_Tracks.hpp"
#include "WsfPerfectCorrelation.hpp"
class MTT_Parameters;
class MTT_Track;

#include "UtMatrix.hpp"
class UtInput;

class WsfPlatform;
class WsfTrackId;
class WsfTrackManager;
class WsfMTT_CSV_EventOutput;
class WsfMTT_EventOutput;
#include "WsfMTT_ReferencePoint.hpp"
class WsfMTT_Interface;

//! A container for data related to the Suppressor Multi-Target Tracker (MTT).
//! This container stores data that may be related to both correlation and fusion,
//! or neither, and they are necessary but redundant parts of the implementation
//! (i.e., the functionality exists in the track manager, but data and code are
//! necessary for functionality of the MTT.
//! The architecture is for the correlator to own the MTT instance.
//! That way ProcessInput can be called, and the object can be cloned.
//! The correlator then calls "Register;" At that point the fuser has
//! access to it by calling "GetMTT."

class MTT
{
public:
   MTT();
   MTT(const MTT& aSrc);
   MTT& operator=(const MTT&) = delete;
   ~MTT()                     = default;

   MTT* Clone() const { return new MTT(*this); }

   bool ProcessInput(UtInput& aInput); // for general configuration

   WsfMTT_Interface* GetInterface() const { return mMTT_InterfacePtr; }

   MTT_Tracks& GetTracks() // meant to be non-const.
   {
      return mTracks;
   }

   const MTT_Parameters& GetParameters() const { return mParameters; }

   MTT_Parameters& GetParameters() // non-const version
   {
      return mParameters;
   }

   void SetParameters(const MTT_Parameters& aParameters) { mParameters = aParameters; }

   // void SetDebug(bool aDebug) { mDebug = aDebug; }

   // void SetName(const std::string& aName) { mName = aName; }  // To be used in debug output.

   void SetTrackManager(WsfTrackManager* aTrackManager);

   void SaveData(MTT_PerceivedCluster* aNonLocalTrackPtr, MTT_ActiveTrack::Data* aTempData, MTT_ActiveTrack* aLocalTrackPtr);

   bool RemoveSavedData(int                     aMTT_TrackId,
                        MTT_PerceivedCluster*&  aNonLocalTrackPtr, // out
                        MTT_ActiveTrack::Data*& aTempData,
                        MTT_ActiveTrack*&       aLocalTrackPtr); // out

   // Methods converted from FORTRAN
   // Individually converted mtt routines
   // The following two methods are extracted from the mtttfs method
   MTT_ActiveTrack* MTT_Correlate(double                      aSimTime,
                                  const MTT_PerceivedCluster& aNonlocalTrack,
                                  MTT_ActiveTrack::Data&      aTempActiveData);

   void MTT_Fuse(double                      aSimTime,
                 const MTT_PerceivedCluster& aNonlocalTrack,
                 MTT_ActiveTrack&            aLocalTrack,
                 MTT_ActiveTrack::Data&      aTempActiveData);

   void mttc2a(const MTT_CandidateTrack::Data& aCandidateTrackData, MTT_ActiveTrack::Data& aActiveTrackData, bool& aError);

   MTT_ActiveTrack* mttcat(double aSimTime, const MTT_PerceivedCluster& aMeasurement);

   int mttcct(double                      aSimTime,
              const MTT_PerceivedCluster& aMeasurement,
              MTT_ActiveTrack*&           aActiveTrackPtr); // in/out; nonzero if correlated.

   void mttpvf(const UtMatrixd& aVerticalCovariance, // 2x2 (covar, not info?)
               MTT_Track&       aCandidateTrack);

   void mttdps_Correlation(const double&               aSimTime,
                           const MTT_PerceivedCluster& aMeasurement,
                           MTT_ActiveTrack*&           aTrackPtr); // out

   void mttdps_Fusion(const double&               aSimTime,
                      const MTT_PerceivedCluster& aMeasurement,
                      MTT_ActiveTrack*            aTrackPtr); // out

   void mttet(double aSimTime, const MTT_PerceivedCluster& aMeasurement);

   void mttgtm(const MTT_ActiveTrack::Data& aActiveTrackData,
               const MTT_Track&             aTrackHeader,
               const MTT_Measurement&       aMeasurement,
               const double&                aDT,
               double&                      aHypothesisTestValue, // out
               double&                      aInnovationValue);                         // out

   void mttgtt(const MTT_ActiveTrack::Data& aActiveTrackData,
               const MTT_ActiveTrack::Data& aTempActiveTrackData,
               double                       aDt,
               double&                      aTestValue,
               double&                      aInnovationValue);

   void mttlfp(double aDt, UtMatrixd& aCovarianceMatrix, UtMatrixd& aStateVector);

   void mttlfm(UtMatrixd&             aStraightFlightStateVector,
               UtMatrixd&             aStraightFlightErrorCovarianceMatrix,
               const UtMatrixd&       aVerticalErrorCovarianceMatrix,
               const UtMatrixd&       aVerticalFlightInformationState,
               const MTT_Track&       aTrack,
               const MTT_Measurement& aMeasurement,
               double                 aDt,
               double&                aStraightFlightLikelihood);

   void mttlfq(const UtMatrixd&       aStraightFlightState,
               const UtMatrixd&       aStraightFlightErrorCovariance,
               const UtMatrixd&       aVerticalErrorCovariance,
               const UtMatrixd&       aVerticalFlightState,
               const MTT_Track&       aTrack,
               const MTT_Measurement& aMeasurement,
               double                 aDT,
               double&                aHypothesisTestValue, // out
               double&                aInnovationValue);                   // out

   void mtttfe(const MTT_PerceivedCluster&  aMeasurement,
               const MTT_ActiveTrack::Data& aUpdateData, // mst387
               MTT_Track&                   aTrack,      // ptr384
               double                       aUpdateInterval,
               double                       aSimTime);

   void mttflf(UtMatrixd&       aFilterStates,   // in/out
               UtMatrixd&       aHorizontalInfo, // in/out
               const UtMatrixd& aFilterStatesUpdate,
               const UtMatrixd& aHorizontalInfoUpdate,
               const UtMatrixd& aPrevFilterStates,
               const UtMatrixd& aPrevHorizInfoMatrix,
               double           aDt,
               double&          aLikelihoodValue); // out

   void mttftf(UtMatrixd&       aFilterStates,   // in/out
               UtMatrixd&       aHorizontalInfo, // in/out
               const UtMatrixd& aFilterStatesUpdate,
               const UtMatrixd& aHorizontalInfoUpdate,
               const UtMatrixd& aPrevFilterStates,
               const UtMatrixd& aPrevHorizInfoMatrix,
               double           aDt,
               double&          aLikelihoodValue); // out

   void mtttfm(UtMatrixd&             aTurningFlightStateVector,
               UtMatrixd&             aTurningFlightErrorCovarianceMatrix,
               const UtMatrixd&       aVerticalErrorCovarianceMatrix,
               const UtMatrixd&       aVerticalFlightInformationState,
               const MTT_Track&       aTrack,
               const MTT_Measurement& aMeasurement,
               double                 aDt,
               double&                aTurningFlightLikelihood);

   void mtttfp(double aDt, UtMatrixd& aCovarianceMatrix, UtMatrixd& aStateVector);

   void mtttfq(const UtMatrixd&       aTurningFlightState,
               const UtMatrixd&       aTurningFlightErrorCovariance,
               const UtMatrixd&       aVerticalErrorCovariance,
               const UtMatrixd&       aVerticalFlightState,
               const MTT_Track&       aTrack,
               const MTT_Measurement& aMeasurement,
               const double&          aDT,
               double&                aHypothesisTestValue, // out
               double&                aInnovationValue);                   // out

   void mtttfs_Correlation(double                      aSimTime,
                           const MTT_PerceivedCluster& aNonlocalTrack,
                           MTT_ActiveTrack::Data&      aTempActiveData,
                           MTT_ActiveTrack*&           aLocalTrackPtr);

   void mtttfs_Fusion(double                      aSimTime,
                      const MTT_PerceivedCluster& aNonlocalTrack,
                      MTT_ActiveTrack::Data&      aTempActiveData,
                      MTT_ActiveTrack*&           aLocalTrackPtr);

   void mttuat(double aSimTime, const MTT_PerceivedCluster& aNonLocalTrack, MTT_Track& aActiveTrack);

   void mttuhf(double aSimTime, const MTT_PerceivedCluster& aMeasurement, MTT_ActiveTrack*& aTrackPtr);

   void mttuhi(double           aDt,
               const UtMatrixd& aHorizontalAcceleration,    // 2x1
               UtMatrixd&       aHorizontalInfoMatrix,      // 4x4
               UtMatrixd&       aHorizontalState,           // 4x1
               const UtMatrixd& aConnectionMatrix,          // 2x4
               const UtMatrixd& aConnectionMatrixTranspose, // 4x2
               const UtMatrixd& aDxDyCovariance,            // 2x2
               double           aSensorLocationX,
               double           aSensorLocationY,
               double           aDx,
               double           aDy,
               int              aTrackType);

   void mttuvi(double           aDt,
               const UtMatrixd& aVerticalInfoFilter,          // 2x1, W
               UtMatrixd&       aVerticalInfoMatrix,          // 2x2, PI
               UtMatrixd&       aVerticalState,               // 2x1, XI
               const UtMatrixd& aVerticalCandidateInfoFilter, // 2x2, RINV
               double           aSensorLocationZ,             // RDRZ
               double           aTargetLocationZ,             // DZ
               unsigned         aRangeValid,                  // IRNG
               unsigned         aElevationValid,              // IEL
               int              aTrackType);                               // TRKTYP

   void mttuvp(double           aDt,
               const UtMatrixd& aVerticalInfoFilter, // 2x1, W
               UtMatrixd&       aVerticalInfoMatrix, // 2x2, PI
               UtMatrixd&       aVerticalState,      // 2x1, XI
               int              aTrackType);                      // TRKTYP

   void mttalt(const MTT_ActiveTrack* aLocalTrackPtr,
               double&                aAlt); // out

   WsfPerfectCorrelation mHF_Correlator;

private:
   WsfMTT_Interface* mMTT_InterfacePtr;
   MTT_Parameters    mParameters;
   MTT_Tracks        mTracks;
   bool              mDebug;
   std::string       mName;
   WsfTrackManager*  mManagerPtr;

   // Misc options not covered by MTT_Parameters
   bool mCorrelateAllSources;

   struct SavedData // Between correlation and fusion calls
   {
      MTT_PerceivedCluster*  mNonLocalTrackPtr;
      MTT_ActiveTrack::Data* mTempDataPtr;
      MTT_ActiveTrack*       mLocalTrackPtr;
   };

   using TrackIdToSavedDataMap = std::map<int, SavedData>;
   TrackIdToSavedDataMap mTrackIdToSavedDataMap;
};

#endif
