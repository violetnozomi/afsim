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

#ifndef WSFCORRELATIONSTRATEGY_HPP
#define WSFCORRELATIONSTRATEGY_HPP

#include "wsf_export.h"

#include <map>

class UtInput;
#include "UtScriptBasicTypes.hpp"
#include "WsfException.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
#include "script/WsfScriptObjectClass.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"
#include "WsfTrackListFwd.hpp"
class WsfPlatform;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfTrack;
class WsfTrackManager;

//!   This class defines the interface for a correlation strategy.
//!   It is to be used and configured by the WsfTrackManager.
//!   A factory method called WsfCorrelationStrategy::Create is provided to return new instances of existing
//!   correlation object prototypes.
//!
//!   In order to create new correlation objects, the implementor should do the following:
//!     <ul>
//!     <li>  Re-implement WsfCorrelationStrategy::CorrelateImpl, the method that implements the correlation algorithm.
//!     <li>  Re-implement WsfCorrelationStrategy::Clone.
//!     <li>  Add a prototype in "WsfCorrelationStrategy::GetTypes", in the static initialization section;
//!           or, add a call to "WsfCorrelationStrategy::GetTypes::Add" (see "WsfObjectTypeList::Add" ) elsewhere in the
//!           code.
//!     </ul>
//!   @note It is suggested that the developer prefer the use of WsfCorrelationStrategy::Create over Clone() as the
//!   former (factory) method ties in the desired instance of WsfTrackManager.
//!
class WSF_EXPORT WsfCorrelationStrategy : public WsfObject
{
public:
   ~WsfCorrelationStrategy() override = default;

   //! Create a clone of "this" object.
   WsfCorrelationStrategy* Clone() const override = 0; // pure virtual

   //! Process input from a common source.
   bool ProcessInput(UtInput& aInput) override { return false; }

   //! Perform any necessary initialization.
   virtual bool Initialize(WsfTrackManager* aTrackManagerPtr)
   {
      mTrackManagerPtr = aTrackManagerPtr;
      return true;
   };

   void NewTrackCorrelation(const WsfTrack& aRawTrack, const WsfLocalTrack& aLocalTrack);

   //! Given a non-local track update (or measurement), find the track in the track list
   //! that correlates with the given track or measurement.
   WsfLocalTrack* Correlate(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList);

   const WsfTrackId& GetCorrelatedTrackId(const WsfTrackId& aRawTrackId) const;

   virtual void Decorrelate(const WsfTrackId& aRawTrackId);

   void NotifyOfLocalTrackCorrelation(double aSimTime, const WsfLocalTrack& aLocalTrack, const WsfTrackId& aRawTrackId);

   typedef WsfObjectTypeList<WsfCorrelationStrategy> CorrelationStrategyTypes;

   //! Return a reference to the track manager being accessed internally by this object.
   WsfTrackManager& GetTrackManager() { return *mTrackManagerPtr; }

   void           SetTrackManager(WsfTrackManager* aTrackManagerPtr) { mTrackManagerPtr = aTrackManagerPtr; }
   WsfSimulation* GetSimulation() const;

   bool TestCorrelation(const WsfTrack& aTrack1, const WsfTrack& aTrack2);

protected:
   WsfCorrelationStrategy();
   WsfCorrelationStrategy(const WsfCorrelationStrategy& aSrc);
   WsfCorrelationStrategy& operator=(const WsfCorrelationStrategy& aRhs);

   //! Given a non-local track update (or measurement), find the track in the track list
   //! that correlates with the given track or measurement (pure virtual method).
   virtual WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) = 0;

   std::map<WsfTrackId, WsfTrackId> mCorrelationMap;

private:
   WsfTrackManager* mTrackManagerPtr;
};

//! The following script class is intended to be used for unit testing only.
class WsfScriptCorrelationClass : public WsfScriptObjectClass
{
public:
   WsfScriptCorrelationClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(TestCorrelation); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(Correlate);       // NO_DOC | FOR_TEST_ONLY
};

#endif
