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

#ifndef WSFTRACKCLASSIFIER_HPP
#define WSFTRACKCLASSIFIER_HPP

#include "wsf_mil_export.h"

#include <map>

#include "WsfClassificationProcessor.hpp"

class TrackAccessor;
class UtInput;
class WsfTrack;

class WSF_MIL_EXPORT WsfTrackClassifier // Truth
{
public:
   WsfTrackClassifier();
   WsfTrackClassifier(const WsfTrackClassifier& aSrc) = default;
   WsfTrackClassifier& operator=(const WsfTrackClassifier&) = delete;
   virtual ~WsfTrackClassifier()                            = default;
   virtual WsfTrackClassifier* Clone()                      = 0;
   virtual bool                ProcessInput(UtInput& aInput);

   /** We need to parse a track once and re-use the data a lot.  Even for
    * a classifier subclass, the accessor is needed first for each
    * prototype to create the list; then it is needed for gating.
    * This function enables re-use.
    */
   virtual TrackAccessor* GetTrackAccessor(const WsfTrack& aTrack);

   /** Create a  list of probable classifications for the track.
    * This default implementation creates a list of one, which contains truth.
    */
   virtual void CreateClassList(const TrackAccessor& aTrack, ClassificationReportList& aClassificationReportList);

   virtual void Classify(const TrackAccessor&            aTrackAccessor,
                         const ClassificationReportList& aClassificationList,
                         ClassificationReport&           aClassificationReport);

protected:
   friend class WsfClassificationProcessor;
   virtual void Test() {}

   bool mDebug;
};

/** A rules based classifier, that gives a user positive control over
** any mis-classifications.
*/
class WSF_MIL_EXPORT WsfClassifierTabulated : public WsfTrackClassifier
{
public:
   struct ClassifierInst
   {
      WsfStringId mEntityTypeId;
      double      mTypeFrequency;
   };

   struct EntityMisIdInfo
   {
      double                   mPcid;
      ClassificationReportList mMisIdEnityList;
   };

   WsfClassifierTabulated();
   WsfClassifierTabulated(const WsfClassifierTabulated& aSrc) = default;
   WsfClassifierTabulated& operator=(const WsfClassifierTabulated&) = delete;
   ~WsfClassifierTabulated() override                               = default;
   WsfTrackClassifier* Clone() override;
   bool                ProcessInput(UtInput& aInput) override;
   bool                ProcessInputIdTable(UtInput& aInput);
   void CreateClassList(const TrackAccessor& aTrack, ClassificationReportList& aClassificationReportList) override;

   void Classify(const TrackAccessor&            aTrackAccessor,
                 const ClassificationReportList& aClassificationList,
                 // double aPcid,
                 ClassificationReport& aClassificationReport) override;


protected:
   // friend class WsfClassificationProcessor;
   void Test() override;

private:
   void AddEntityMID(WsfStringId& aKeyEntityTypeId, double aPcid, std::vector<ClassifierInst>& aMisIdList);

   // We use the same object for the misclassification database and for the output
   std::map<WsfStringId, EntityMisIdInfo> mMisIdtable;
};


#endif
