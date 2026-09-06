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

#include "WsfTrackClassifier.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfTrack.hpp"

namespace
{
void AssertLocalTrackClassifier(bool aTruth, const char* aMsg)
{
   if (!aTruth)
   {
      ut::log::error() << "AssertLocalTrackClassifier: " << aMsg;
      assert(aTruth);
   }
}
} // namespace


WsfTrackClassifier::WsfTrackClassifier()
   : mDebug(false)
{
}

// virtual
bool WsfTrackClassifier::ProcessInput(UtInput& aInput)
{
   // This is the default truth, which has no commands
   bool myCommand = false;
   return myCommand;
}

/** Create a  list of probable classifications for the track.
 * This default implementation creates a list of one, which contains truth.
 */
// virtual
void WsfTrackClassifier::CreateClassList(const TrackAccessor& aTrack, ClassificationReportList& aClassificationReportList)
{
   AssertLocalTrackClassifier(aClassificationReportList.empty(), "Inputting a empty list is not expected!");

   ClassificationReport truthReport;
   // Chicken-egg: what if someone wants to use a classifier to mis-classify
   // the track, and that track is the input?
   // truthReport->mEntityTypeId = WsfStringId::GetString(aTrack.GetTargetType());
   truthReport.mEntityTypeId = aTrack.GetTrueTypeID();
   truthReport.mTypeScore    = 1.0;
   aClassificationReportList.push_back(truthReport);
}

// virtual
TrackAccessor* WsfTrackClassifier::GetTrackAccessor(const WsfTrack& aTrack)
{
   TrackAccessor* ta = new TrackAccessor;
   ta->SetTrack(aTrack);
   return ta;
}

/** After a list of probabilities is created, it can be passed
 * in select "one" classification for the given track.
 * It builds a PDF based on the inputted list, and selects
 * appropriately.
 *   It was thought this was a super function, but Bayes is real and
 * chooses the best based on a given perception.
 */
// virtual
void WsfTrackClassifier::Classify(const TrackAccessor&            aTrackAccessor,
                                  const ClassificationReportList& aClassificationList,
                                  ClassificationReport&           aClassificationReport)
{
   // Report truth
   aClassificationReport.mEntityTypeId = aTrackAccessor.GetTrueTypeID();
   aClassificationReport.mTypeScore    = 1.0;
}


WsfClassifierTabulated::WsfClassifierTabulated()
   : WsfTrackClassifier()
   , mMisIdtable()
{
}

// virtual
WsfTrackClassifier* WsfClassifierTabulated::Clone()
{
   return new WsfClassifierTabulated(*this);
}

bool WsfClassifierTabulated::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "pcid")
   {
      double pcid = 1.0;
      aInput.ReadValue(pcid);
      aInput.ValueInClosedRange(pcid, 0.0, 1.0);
      // aInput.ReadValue(mFuseAllCollects);
   }
   else if (command == "identification_table")
   {
      UtInputBlock blockOutter(aInput,
                               "end_"
                               "identification_table");
      // Read entity elements
      while (blockOutter.ReadCommand())
      {
         ProcessInputIdTable(aInput);
      }
   }
   else
   {
      myCommand = WsfTrackClassifier::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfClassifierTabulated::ProcessInputIdTable(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "entity")
   {
      UtInputBlock blockOut(aInput, "end_entity");
      std::string  keyEntityType;
      WsfStringId  keyEntityTypeId;
      double       pcid = 1.0;
      while (blockOut.ReadCommand())
      {
         command = blockOut.GetCommand();
         if (command == "type")
         {
            aInput.ReadValue(keyEntityType);
            keyEntityTypeId = WsfStringId(keyEntityType);
         }
         else if (command == "pcid")
         {
            aInput.ReadValue(pcid);
            aInput.ValueInClosedRange(pcid, 0.0, 1.0);
         }
         else if (command == "misclass")
         {
            std::vector<ClassifierInst> aMisIdList;
            UtInputBlock                block(aInput, "end_misclass");
            while (block.ReadCommand())
            {
               std::string typeType = block.GetCommand();
               double      typeFrequency;
               aInput.ReadValue(typeFrequency);
               aInput.ValueGreater(typeFrequency, 0.0);

               ClassifierInst inst;
               inst.mEntityTypeId = WsfStringId(typeType);
               if (inst.mEntityTypeId == keyEntityTypeId)
               {
                  AssertLocalTrackClassifier(false, "Not allowed - including self-type into mis ID list!");
                  throw UtInput::BadValue(aInput, "Not allowed - including self-type into mis ID list!");
               }
               inst.mTypeFrequency = typeFrequency;
               aMisIdList.push_back(inst);
            }
            AddEntityMID(keyEntityTypeId, pcid, aMisIdList);
         }
      }
   }
   else
   {
      myCommand = false; // WsfLinkedProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// private:
void WsfClassifierTabulated::AddEntityMID(WsfStringId& aKeyEntityTypeId, double aPcid, std::vector<ClassifierInst>& aMisIdList)
{
   AssertLocalTrackClassifier(!aMisIdList.empty(), "Called with empty aMisIdList!");

   // Allocate an object space - no new.
   EntityMisIdInfo&          entityMisInfo      = mMisIdtable[aKeyEntityTypeId];
   ClassificationReportList& classificationList = entityMisInfo.mMisIdEnityList;

   if (!classificationList.empty())
   {
      AssertLocalTrackClassifier(classificationList.empty(), "Only one entry per key entity!");
      throw UtException("Only one entry per entity!");
   }
   entityMisInfo.mPcid = aPcid;

   //---------------------------------
   // Normalize the scores.
   size_t n   = aMisIdList.size();
   double sum = 0.0;
   for (size_t i = 0; i < n; ++i)
   {
      // the normalized score for this entity
      sum += aMisIdList[i].mTypeFrequency;
   }

   // The candidate list needs to contain some probality
   // of what it really is.  We use the pcid to scale the misid.
   // WARNING: For the single classifier, it will roll, say
   // misclass, but then find the correct id.
   {
      ClassifierInst inst;
      inst.mEntityTypeId = aKeyEntityTypeId;
      // Tricky.  We need to scale others in misid list by
      // increasing the sum
      if (1 - aPcid != 0 && sum / (1 - aPcid) < 1000000)
      {
         size_t new_sum      = (size_t)(sum / (1 - aPcid) + 0.5);
         inst.mTypeFrequency = new_sum - sum;
         sum += inst.mTypeFrequency;
         aMisIdList.push_back(inst);
      }
      else
      {
         // This block should matter much.  If pcid
         // is 1.0, the caller will just report the truth list.
         inst.mTypeFrequency = 1000000;
         sum += inst.mTypeFrequency;
         aMisIdList.push_back(inst);
      }
   }

   for (auto& i : aMisIdList)
   {
      // the normalized score for this entity
      double score = i.mTypeFrequency / sum;

      ClassificationReport misreport;
      misreport.mEntityTypeId = i.mEntityTypeId;
      misreport.mTypeScore    = score;
      classificationList.push_back(misreport);
   }

   // Store the list as sorted.
   std::sort(classificationList.begin(), classificationList.end(), ClassificationReport::ComparitorDescending());
}

// virtual
void WsfClassifierTabulated::CreateClassList(const TrackAccessor& aTrack, ClassificationReportList& aClassificationReportList)
{
   // int truthEntityTypeId = aTrack.GetTrueTypeID();
   WsfStringId truthEntityTypeId = aTrack.GetTrueTypeID();
   // string truthEntityType = aTrack.GetTrueType();

   // Note: The mMisIdtable already constains the true ID, so this function
   // simply copies the list.

   // Note: the map does the allocation and will return an object
   // even if not mapped yet.  We simple tell if its used by looking
   // at the list size.
   EntityMisIdInfo&          entityMisInfo      = mMisIdtable[truthEntityTypeId];
   ClassificationReportList& classificationList = entityMisInfo.mMisIdEnityList;

   if (classificationList.empty())
   {
      // Just report truth?
      WsfTrackClassifier::CreateClassList(aTrack, aClassificationReportList);
   }
   else
   {
      // This is it - we just copy our list of probabilities
      // to be consumed as is.  We don't need to select "the"
      // one, and do that with the correct PDF.
      for (auto i : classificationList)
      {
         aClassificationReportList.push_back(i);
      }
   }
}

// virtual
void WsfClassifierTabulated::Test()
{
   WsfTrack t;
   for (auto& it : mMisIdtable)
   {
      WsfStringId      entityTypeId = it.first;
      EntityMisIdInfo& mid          = it.second;
      // This makes it the true ID
      t.SetTargetType(entityTypeId);
      TrackAccessor* ta = this->GetTrackAccessor(t);

      ClassificationReportList ClassificationReportList;
      //-----------------
      this->CreateClassList(*ta, ClassificationReportList);
      //-----------------

#ifdef PRINT_THE_TEST_CLASS_LIST
      PrintClassList(ClassificationReportList, entityTypeId);
#endif
      AssertLocalTrackClassifier(mid.mMisIdEnityList.size() == ClassificationReportList.size(),
                                 "List sizes should be the same IFF low p() not pruned.");

      bool containsTrue = false;

      double sop = 0.0;
      for (ClassificationReport& reportR : ClassificationReportList)
      {
         sop += reportR.mTypeScore;
      }
      AssertLocalTrackClassifier(UtMath::NearlyEqual(sop, 1.0, 0.0001), "Sum of probabilities should be 1.0");

      // Assure the list is copy of the inputs
      for (size_t i = 0; i < ClassificationReportList.size(); ++i)
      {
         ClassificationReport& reportR = ClassificationReportList[i];
         ClassificationReport& reportT = mid.mMisIdEnityList[i];
         // string tp = WsfStringId::GetString(reportR.mEntityTypeId);
         // string tp = reportR.mEntityTypeId.GetString();
         ////ut::log::info() << "   type = " << tp << ", P(e) = " << report.mTypeScore << endl;
         AssertLocalTrackClassifier(reportR.mEntityTypeId == reportT.mEntityTypeId, "???");
         AssertLocalTrackClassifier(reportR.mTypeScore == reportT.mTypeScore, "???");

         // Actually, we could just check the base - list.
         if (entityTypeId == reportR.mEntityTypeId)
         {
            containsTrue = true;
         }
      }
      AssertLocalTrackClassifier(containsTrue, "List should contains True type, even is 0.0% ... UNLESS pruned!");

      ClassificationReport cr;
      //-----------------
      this->Classify(*ta, ClassificationReportList, cr);
      //-----------------

      AssertLocalTrackClassifier(cr.mTypeScore > 0.0, "???");

      delete ta;
      ta = nullptr;
   }
}

// virtual
void WsfClassifierTabulated::Classify(const TrackAccessor&            aTrackAccessor,
                                      const ClassificationReportList& aClassificationList,
                                      ClassificationReport&           aClassificationReport)
{
   // int truthEntityTypeId = aTrackAccessor.GetTrueTypeID();
   WsfStringId truthEntityTypeId = aTrackAccessor.GetTrueTypeID();
   // string truthEntityType = aTrackAccessor.GetTrueType();

   double r = ut::Random().Uniform<double>();

   // If empty OR the true type is the only entry, return true type.
   if (aClassificationList.size() < 2)
   {
      // Just report truth.
      aClassificationReport.mEntityTypeId = truthEntityTypeId;
      aClassificationReport.mTypeScore    = 1.0;
   }
   else
   {
      // Q: Can we make this general and move it up so bayes can
      //    use it too, for the sim misclass?
      // A: No. Bayes uses most probable
      //--------------------------------------------------------------------
      // This section could be persisted.  Now, the PDF is being built every call.

      std::vector<const ClassificationReport*> aClassificationListMinus; // minus truth

      // Exclude the true entity, because if the roll was for a cid,
      // we wouldn't be here.  Ie allowing truth in here
      // will skew the results, and make truth more probable
      // than the pcid.
      for (const auto& i : aClassificationList)
      {
         if (i.mEntityTypeId == truthEntityTypeId)
         {
            continue;
         }

         // Ignore low probability one.
         if (i.mTypeScore < 0.0001)
         {
            continue;
         }

         aClassificationListMinus.push_back(&i);
      }

      // We are pruning low p(), so the list may be 1 or empty; if
      // the user authored it that way.  If so, return now?  We
      // could be the dyn-pdf will work.  The only thing we can't
      // do is an empty list....
      // This happens when pcid = 1.0.  Truth is pruned above, and then
      // so are the low others
      ////AssertLocalTrackClassifier(aClassificationListMinus.size() != 0, "Unexpected empty list");
      if (aClassificationListMinus.empty())
      {
         aClassificationReport.mEntityTypeId = truthEntityTypeId;
         aClassificationReport.mTypeScore    = 1.0;
         return;
      }

      //++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // Create a Dynamic PDF and draw from it
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // Invert the min, and use as multiplier
      double minVal = 1.0;
      for (auto& classificationListMinus : aClassificationListMinus)
      {
         // Ignore low probability one.
         if (classificationListMinus->mTypeScore < 0.0001)
         {
            continue;
         }
         minVal = std::min(minVal, classificationListMinus->mTypeScore);
      }
      // When we use s against the min, it will be 1.  For equal dist,
      // all will 1 and # part will be 5
      double s = 1.0 / minVal;

      int                                      rep = 0;
      std::vector<const ClassificationReport*> dynPdf;
      size_t                                   numParts = 100;
      while (dynPdf.size() < numParts)
      {
         for (size_t i = 0; i < aClassificationListMinus.size(); ++i, ++rep)
         {
            ////// A naivee implementation of PDF.  Populate x many times so a random
            ////// draw gives the inputted distribution. warning: we don't look
            ////// for a common denominator, so inputting 10000, 10000, 2000
            ////// won't be pretty ;}
            ////// Q: Do we still need to normalize?
            ////// A: Yes, because it's still the reported pid, used by TI
            size_t np = (size_t)(s * aClassificationListMinus[i]->mTypeScore);
            // System.out.println(cn + " Adding k particles = " + np);
            for (size_t j = 0; j < np; ++j)
            {
               const ClassificationReport* cr = aClassificationListMinus[i];
               dynPdf.push_back(cr);
            }
         }
      }
      //--------------------------------------------------------------------

      // It shouldn't hurt to use the same r
      size_t rint = (size_t)(dynPdf.size() * r);
      AssertLocalTrackClassifier(rint < dynPdf.size(), "???");

      const ClassificationReport* misId   = dynPdf[rint];
      aClassificationReport.mEntityTypeId = misId->mEntityTypeId;
      aClassificationReport.mTypeScore    = misId->mTypeScore;
   }
}
