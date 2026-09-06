// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALCONJUNCTIONPROCESSOR_HPP
#define WSFORBITALCONJUNCTIONPROCESSOR_HPP

#include <memory>
#include <vector>

class UtInput;
class UtOrbitalPropagatorBase;
class WsfLocalTrack;
class WsfMessage;
#include "WsfOrbitalConjunctionAssessment.hpp"
class WsfScenario;
#include "WsfStringId.hpp"
#include "script/WsfScriptProcessor.hpp"

//! A processor that will examine tracks and search for possible conjunctions between spacecraft.
class WsfOrbitalConjunctionProcessor : public WsfScriptProcessor
{
public:
   explicit WsfOrbitalConjunctionProcessor(const WsfScenario& aScenario);
   WsfOrbitalConjunctionProcessor(const WsfScenario& aScenario, const std::string& aPartClass, const std::string& aPartName);
   ~WsfOrbitalConjunctionProcessor() override = default;

   WsfOrbitalConjunctionProcessor& operator=(const WsfOrbitalConjunctionProcessor& aOther) = delete;

   const char*   GetScriptClassName() const override { return "WsfOrbitalConjunctionProcessor"; }
   WsfProcessor* Clone() const override;
   bool          ProcessInput(UtInput& aInput) override;
   bool          Initialize(double aSimTime) override;
   void          Update(double aSimTime) override;

protected:
   WsfOrbitalConjunctionProcessor(const WsfOrbitalConjunctionProcessor& aSrc);

private:
   void CategorizeLocalTracks(std::vector<WsfLocalTrack*>& aPrimaries, std::vector<WsfLocalTrack*>& aSecondaries);
   void RunPairs(std::vector<WsfLocalTrack*>::const_iterator aPrimary,
                 std::vector<WsfLocalTrack*>::const_iterator aBegin,
                 std::vector<WsfLocalTrack*>::const_iterator aEnd,
                 std::vector<WsfOrbitalConjunctionRecord>&   aRecords);
   void RunPrimaryPrimary(const std::vector<WsfLocalTrack*>&        aPrimaries,
                          std::vector<WsfOrbitalConjunctionRecord>& aRecords);
   void RunPrimarySecondary(const std::vector<WsfLocalTrack*>&        aPrimaries,
                            const std::vector<WsfLocalTrack*>&        aSecondaries,
                            std::vector<WsfOrbitalConjunctionRecord>& aRecords);
   void SortRecords(std::vector<WsfOrbitalConjunctionRecord>& aRecords);

   WsfOrbitalConjunctionAssessment::Options mOptions;        //! The options for this processor.
   double                                   mSearchInterval; //! Time interval over which to search for conjunctions.
   std::unique_ptr<UtOrbitalPropagatorBase> mPropPtr;        //! Prototype for object state prediction.
   bool                                     mDebug;          //! Print out debugging information.
   std::set<WsfStringId>                    mPrimaries;      //! If the name of the primary was specified.
   UtScript* mOnConjunctionPredictedPtr;                     //! Script to call once predictions are complete.
};


#endif // WSFORBITALCONJUNCTIONPROCESSOR_HPP
