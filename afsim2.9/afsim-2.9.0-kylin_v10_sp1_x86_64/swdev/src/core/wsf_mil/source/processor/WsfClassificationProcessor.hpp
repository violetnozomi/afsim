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

#ifndef WSFCLASSIFICATIONPROCESSOR_HPP
#define WSFCLASSIFICATIONPROCESSOR_HPP

#include "wsf_mil_export.h"

#include <string>

#include "WsfLinkedProcessor.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
class WsfTrackClassifier;


/** A parse-once re-use object super-class, to get to the track attributes.
 * The default is truth, which doesn't incur the cost of fully parsing the track.
 */
class WSF_MIL_EXPORT TrackAccessor
{
public:
   TrackAccessor()          = default;
   virtual ~TrackAccessor() = default;

   std::string GetTrueType() const { return mTrack == nullptr ? "???" : mTrack->GetTargetType().GetString(); }

   // int GetTrueTypeID() const
   //{
   //    return mTrack == 0 ? -1 : UtStringId::UnsafeGetNumber(mTrack->GetTargetType());
   // }
   WsfStringId GetTrueTypeID() const { return mTrack == nullptr ? WsfStringId() : mTrack->GetTargetType(); }

   void SetTrack(const WsfTrack& aTrack) { mTrack = &aTrack; }

protected:
   const WsfTrack* mTrack = nullptr;
};


struct ClassificationReport
{
   /** Used for sorting the list : 0.9, 0.7, 0.69, ...
    */
   struct ComparitorDescending
   {
      bool operator()(const ClassificationReport* i, const ClassificationReport* j)
      {
         return i->mTypeScore > j->mTypeScore;
      }
      bool operator()(const ClassificationReport& i, const ClassificationReport& j)
      {
         return i.mTypeScore > j.mTypeScore;
      }
   };

   WsfStringId mEntityTypeId;
   double      mTypeScore;
};
using ClassificationReportList = std::vector<ClassificationReport>;


//! A class that inputs a track, and outputs a list of possible entity types
//! for the track.  Each candidate entity type will have a probability associated
//! with it.  It can out many, one, or zero candidates.  There is currently
//! no script access for this class.
//!
class WSF_MIL_EXPORT WsfClassificationProcessor : public WsfLinkedProcessor
{
public:
   // Let the vector allocate memory
   // static void PrintClassList(ClassificationReportList& ClassificationReportList, int aTrueEntityType = -1);
   static void PrintClassList(ClassificationReportList& ClassificationReportList, WsfStringId aTrueEntityType);

   WsfClassificationProcessor(WsfScenario& aScenario);
   WsfClassificationProcessor(const WsfClassificationProcessor& aSrc);
   WsfClassificationProcessor& operator=(const WsfClassificationProcessor&) = delete;
   ~WsfClassificationProcessor() override;

   WsfProcessor* Clone() const override { return new WsfClassificationProcessor(*this); }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   void CreateClassList(const WsfTrack& aTrack, ClassificationReportList& aClassificationReportList) const;

private:
   WsfTrackClassifier* mClassifier = nullptr;
   bool                mTest       = false;
};

#endif
