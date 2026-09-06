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

#ifndef WSFBAYESCLASSIFIER_HPP
#define WSFBAYESCLASSIFIER_HPP

#include "wsf_mil_export.h"

#include <map>

#include "WsfBayesClassification.hpp"
#include "WsfTrackClassifier.hpp"

//! A track classifier that is a Bayesian Classifier, which
//! is not the same thing as a Bayesian (network) engine.  It
//! uses configured data sources to determine probabilities of
//! being entity types
//!    This class creates a instantiation, which protects all
//! the utility classes it uses.
//!
class WSF_MIL_EXPORT WsfBayesClassifier : public WsfTrackClassifier
{
public:
   WsfBayesClassifier();
   WsfBayesClassifier(const WsfBayesClassifier& aSrc) = default;
   WsfBayesClassifier& operator=(const WsfBayesClassifier&) = delete;
   ~WsfBayesClassifier() override                           = default;
   WsfTrackClassifier* Clone() override;
   bool                ProcessInput(UtInput& aInput) override;
   virtual bool        Initialize(double aSimTime);
   TrackAccessor*      GetTrackAccessor(const WsfTrack& aTrack) override;

   /** Create a  list of probable classifications for the track.
    * This default impl creates a list of one, which contains truth.
    */
   void CreateClassList(const TrackAccessor& aTrack, ClassificationReportList& aClassificationReportList) override;

   void Classify(const TrackAccessor&            aTrackAccessor,
                 const ClassificationReportList& aClassificationList,
                 ClassificationReport&           aClassificationReport) override;

protected:
   bool ReadCsv(const std::string& aFileName);
   void AddDataRow(const std::string&                aEntityType,
                   const std::vector<ColumnPair>&    aAttributeNamePairs,
                   const std::vector<AttributePair>& aAttributeValuePairs);

   void Test() override;

private:
   friend class WsfBayesClassification;
   std::map<WsfStringId, WsfEntityPrototype*> mEntType2Proto;
};

#endif
