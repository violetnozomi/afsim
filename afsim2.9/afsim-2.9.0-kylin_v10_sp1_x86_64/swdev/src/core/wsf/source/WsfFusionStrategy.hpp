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

#ifndef WSFFUSIONSTRATEGY_HPP
#define WSFFUSIONSTRATEGY_HPP

#include "wsf_export.h"

#include <string>

class UtInput;
#include "UtMatrix.hpp"
#include "WsfException.hpp"
class WsfLocalSensorResult;
class WsfLocalTrack;
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
class WsfProcessor;
class WsfSensorResult;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfTrack;
class WsfTrackManager;
#include "WsfTypes.hpp"

//!   This class defines the interface for a fusion strategy.
//!   It is to be used and configured by the WsfTrackManager or a specialized WsfProcessor.
//!   A factory method called WsfFusionStrategy::Create is provided to return new instances of existing
//!   correlation object prototypes.
//!
//!   Currently the only implemented concrete instance is WsfDefaultFusion.
//!   In order to create new fusion objects, the implementor should do the following:
//!     <ul>
//!     <li>  Re-implement WsfFusionStrategy::UpdateLocalTrackFromNonLocalTrack, the method that implements the fusion algorithm.
//!     <li>  Re-implement WsfFusionStrategy::ReviewTrack.
//!     <li>  Re-implement WsfFusionStrategy::Clone.
//!     <li>  Add a prototype in WsfFusionStrategy::GetTypes(), in the static initialization section;
//!           or, add a call to WsfFusionStrategy::GetTypes::Add (see WsfObjectTypeList<T>::Add()) elsewhere in the code.
//!     </ul>
//!   @note It is suggested that the developer prefer the use of WsfFusionStrategy::Create over Clone() as the former
//!   (factory) method ties in the desired instance of WsfTrackManager.
//!
class WSF_EXPORT WsfFusionStrategy : public WsfObject
{
public:
   ~WsfFusionStrategy() override = default;

   //! Create a clone of "this" object.
   WsfFusionStrategy* Clone() const override = 0; // pure virtual

   //! Process input from a common source.
   bool ProcessInput(UtInput& aInput) override { return false; }

   //! Perform any necessary initialization.
   virtual bool Initialize(WsfTrackManager* aTrackManagerPtr)
   {
      mTrackManagerPtr = aTrackManagerPtr;
      return true;
   };

   //! Perform any necessary initialization.
   virtual bool Initialize(WsfProcessor* aProcessorPtr)
   {
      mProcessorPtr = aProcessorPtr;
      return true;
   };

   //! Return whether debug is enabled for this instance.
   virtual bool DebugEnabled() const;

   //! Fuse a non-local track with an existing local track.
   //! @returns true if the local track was updated with non-local track data or false if the
   //!          non-local track data was out-of-date.
   virtual bool UpdateLocalTrackFromNonLocalTrack(double          aSimTime,
                                                  WsfLocalTrack&  aLocalTrack,
                                                  const WsfTrack& aNonLocalTrack) = 0;

   //! Fuse a non-local result with an existing local result.
   //! @returns true if the local result was updated with non-local result data or false if the
   //!          non-local result data was out-of-date.
   virtual bool UpdateLocalResultFromNonLocalResult(double                 aSimTime,
                                                    WsfLocalSensorResult&  aLocalResult,
                                                    const WsfSensorResult& aNonLocalResult) = 0;

   //! Allow the fusion strategy to perform initial fusion from an non-local track.
   //! @returns true if the local track was updated with non-local track data.
   virtual bool InitializeLocalTrackFromNonLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   };

   //! Allow the fusion strategy to perform initial fusion from an non-local result.
   //! @returns true if the local result was updated with non-local result data.
   //! @note This is currently not used but included for completeness.
   virtual bool InitializeLocalResultFromNonLocalResult(double                 aSimTime,
                                                        WsfLocalSensorResult&  aLocalResult,
                                                        const WsfSensorResult& aNonLocalResult)
   {
      return false;
   };

   //! Review a track, adjusting parameters based on available information.
   //! Usually these are IFF and track quality.  This review is usually performed after
   //! a track de-correlation.
   virtual void ReviewTrack(WsfLocalTrack& aLocalTrack) = 0;

   virtual bool UpdateNonKinematicData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

   static bool UpdateNonKinematicData(double           aSimTime,
                                      WsfTrackManager& aTracManager,
                                      WsfLocalTrack&   aLocalTrack,
                                      const WsfTrack&  aNonLocalTrack);

   using FusionStrategyTypes = WsfObjectTypeList<WsfFusionStrategy>;

   WsfSimulation* GetSimulation() const;
   double         GetSimTime();
   WsfPlatform*   GetPlatform() const;

   //! Return a reference to the track manager being accessed internally by this object.
   WsfTrackManager&       GetTrackManager() { return *mTrackManagerPtr; }
   const WsfTrackManager& GetTrackManager() const { return *mTrackManagerPtr; }

   //! Return a reference to the processor being accessed internally by this object.
   WsfProcessor& GetProcessor() { return *mProcessorPtr; }

protected:
   WsfFusionStrategy();
   WsfFusionStrategy(const WsfFusionStrategy& aSrc);
   WsfFusionStrategy& operator=(const WsfFusionStrategy&) = delete;

   //! Set the track manager reference.
   void SetTrackManager(WsfTrackManager& aTrackManager) { mTrackManagerPtr = &aTrackManager; }

   //! Set the processor reference.
   void SetProcessor(WsfProcessor& aProcessor) { mProcessorPtr = &aProcessor; }

   virtual bool UpdateIFF_Status(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateSide(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateType(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateSpatialDomain(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateTrackQuality(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateSignalToNoise(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateSignalList(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   virtual bool UpdateAuxData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
   {
      return false;
   }

   // Some fusion-specific utilities that derived classes may use:
   static void FuseEstimates(const UtMatrixd& aEstimate1,
                             const UtMatrixd& aEstimate2,
                             const UtMatrixd& aCovar1,
                             const UtMatrixd& aCovar2,
                             UtMatrixd&       aEstimateOut,
                             UtMatrixd&       aCovarOut);

   static void NormalizeAzRangeEstimate(const UtMatrixd& aEstimate,
                                        const UtMatrixd& aCovar,
                                        const double&    aElevation,
                                        UtMatrixd&       aNormEstimate,
                                        UtMatrixd&       aNormCovar);

   static void NormalizeElRangeEstimate(const UtMatrixd& aEstimate,
                                        const UtMatrixd& aCovar,
                                        const double&    aAzimuth,
                                        UtMatrixd&       aNormEstimate,
                                        UtMatrixd&       aNormCovar);

private:
   static FusionStrategyTypes* mTypesPtr;

   WsfTrackManager* mTrackManagerPtr;
   WsfProcessor*    mProcessorPtr;
};

#endif
