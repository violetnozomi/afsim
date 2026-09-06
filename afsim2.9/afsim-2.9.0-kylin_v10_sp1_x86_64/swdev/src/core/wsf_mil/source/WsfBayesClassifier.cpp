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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfBayesClassifier.hpp"

#include <fstream>
#include <sstream>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "UtWallClock.hpp"
#include "WsfTrackList.hpp"

namespace
{
void AssertLocalBayesClassifier(bool aTruth, const char* aMsg)
{
   if (!aTruth)
   {
      ut::log::error() << "AssertLocalBayesClassifier: " << aMsg;
      assert(aTruth);
   }
}
} // namespace


WsfBayesClassifier::WsfBayesClassifier()
   : WsfTrackClassifier()
   , mEntType2Proto()
{
}

// virtual
WsfTrackClassifier* WsfBayesClassifier::Clone()
{
   return new WsfBayesClassifier(*this);
}

// virtual
TrackAccessor* WsfBayesClassifier::GetTrackAccessor(const WsfTrack& aTrack)
{
   WsfTrackAccessorDeep* ta = new WsfTrackAccessorDeep();
   ta->SetTrack(aTrack);
   return ta;
}

// virtual
bool WsfBayesClassifier::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "prototype_tables")
   {
      UtInputBlock blockOutter(aInput,
                               "end_"
                               "prototype_tables");
      std::string  tableName;
      while (blockOutter.ReadCommand())
      {
         // "table" is a trivial tag/key, to allow getting at quoted values
         command = aInput.GetCommand();
         if ("table" != command)
         {
            AssertLocalBayesClassifier(false, ("Only 'table' is allowed here: " + command).c_str());
            // throw UtInput::BadValue(aInput, "Not allowed - including self-type into mis ID list!");
            throw UtInput::UnknownCommand(aInput);
         }
         aInput.ReadValueQuoted(tableName);
         if (mDebug)
         {
            auto out = ut::log::debug() << "BayesBase:ProcessInput: About to read table.";
            out.AddNote() << "Table: " << tableName;
         }
         ReadCsv(tableName);
      }
   }
   else
   {
      myCommand = WsfTrackClassifier::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfBayesClassifier::Initialize(double aSimTime)
{
   // mCorrelation.Initialize(&GetPlatform()->GetTrackManager());
   // mDraw.SetId(GetPlatform()->GetNameId());
   // return WsfTrackClassifier::Initialize(aSimTime);
   return false;
}

// virtual
void WsfBayesClassifier::CreateClassList(const TrackAccessor&      aTrackAccessor,
                                         ClassificationReportList& aClassificationReportList)
{
   // for (map<int, EntityPrototype*>::iterator it = mEntType2Proto.begin();
   for (auto& it : mEntType2Proto)
   {
      // AssertLocalBayesClassifier(false, "Bayes still needs to create list??");
      ClassificationReport report;
      report.mEntityTypeId = it.second->mTypeNameID;
      // report.mTypeScore = it->second->GetEllipticalGate(aTrackAccessor);
      report.mTypeScore = it.second->GetProbabilityDistribution(aTrackAccessor);
      ////report.mTypeScore = mProtoes[i]->GetProbabilityDistribution(aTrack);

      aClassificationReportList.push_back(report);
   }

   // Normalize across clusters.  See Gender BC.
   double sum = 0.0;
   for (auto& i : aClassificationReportList)
   {
      sum += i.mTypeScore;
   }

   AssertLocalBayesClassifier(sum != 0.0, "???");

   for (auto& i : aClassificationReportList)
   {
      i.mTypeScore /= sum;
   }

   // Sort the list.
   ClassificationReport::ComparitorDescending comparitor; //.9,.7,.6,...
   std::sort(aClassificationReportList.begin(), aClassificationReportList.end(), comparitor);
}

/** The realistic Bayes classifier just selects the best candidate.  First
 * a call to createList.  Then that list is passed in here.
 */
// virtual
void WsfBayesClassifier::Classify(const TrackAccessor&            aTrackAccessor,
                                  const ClassificationReportList& aCandidateEntitylist,
                                  ClassificationReport&           aClassificationReport)
{
   // int truthEntityTypeId = aTrack.GetTargetType();
   // std::string truthEntityType = WsfStringId::GetString(aTrack.GetTargetType());

   std::vector<const ClassificationReport*> candidateEntitylistSorted;

   for (const auto& i : aCandidateEntitylist)
   {
      candidateEntitylistSorted.push_back(&i);
   }

   ClassificationReport::ComparitorDescending comparitor; //.9,.7,.6,...
   std::sort(candidateEntitylistSorted.begin(), candidateEntitylistSorted.end(), comparitor);

   // The best will be at the top of the list, although it doesn't address equal
   // probabilities.
   const ClassificationReport* bestIdReport = candidateEntitylistSorted[0];

   // We don't need to calculate 95% if already inside 1 sigma
   // Also normalize, without losing above.
   // sum = s1 + s2 ...
   // s1 /= sum

   // See if the best isn't good enough.  That is, the best is closest, but the
   //  given "point" may be outside the best protos 95% ellipse.
   // Calculate the chance of UNK
   double ed95 = mEntType2Proto[bestIdReport->mEntityTypeId]->GetEllipticalGate95Percent(aTrackAccessor);
   if (ed95 > 1.0)
   {
      aClassificationReport.mEntityTypeId = bestIdReport->mEntityTypeId;
      aClassificationReport.mTypeScore    = bestIdReport->mTypeScore;
   }
   else
   {
      aClassificationReport.mEntityTypeId = WsfStringId("UNKOWN");
      aClassificationReport.mTypeScore    = -1;
   }
}

//! Read a data table to populate the entity prototypes
//! It's possible and common to use multiple tables to populate
//! the same entity type; the attributes will be merged.
bool WsfBayesClassifier::ReadCsv(const std::string& aFileName)
{
   std::ifstream ifs(aFileName);
   if (!ifs.is_open() || ifs.fail())
   {
      auto out = ut::log::error() << "BayesBase:ReadCsv: Unable to open table file.";
      out.AddNote() << "File: " << aFileName;
      return false;
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "BayesBase:ReadCsv: Parsing file.";
      out.AddNote() << "File: " << aFileName;
   }
   UtWallClock runClock;
   runClock.ResetClock();
   double startTime = runClock.GetClock();

   std::string line;
   int         row = 1;

   const static char delim = ',';

   std::vector<ColumnPair>    attributeNamePairs;
   std::vector<AttributePair> attributeValuePairs;

   // 1 lines of comments (header)
   if (!getline(ifs, line))
   {
      auto out = ut::log::error() << "BayesBase::ReadCsv: Failed to read 1st (0th) column in row.";
      out.AddNote() << "Row: " << row;
      throw UtException("BayesBase:ReadCsv reading row failed!  Couldn't read 1st (0th) column");
   }

   std::stringstream lineStream(line);
   std::string       typeHeader;
   if (!std::getline(lineStream, typeHeader, delim))
   {
      throw UtException("BayesBase:ReadCsv Couldn't read typeHeader.  col = ?0");
   }

   ColumnPair pair;
   while (std::getline(lineStream, pair.mFirst, delim))
   {
      if (!std::getline(lineStream, pair.mSecond, delim))
      {
         throw UtException("BayesBase:ReadCsv Couldn't read 2nd in pair.  col = ?");
      }

      // pair.mFirst = Trim(pair.mFirst);
      // pair.mSecond = Trim(pair.mSecond);
      UtStringUtil::TrimWhiteSpace(pair.mFirst);
      UtStringUtil::TrimWhiteSpace(pair.mSecond);

      // See IFT for formal and multi-modal.
      AssertLocalBayesClassifier((pair.mFirst == pair.mSecond) || (pair.mSecond == "sigma"),
                                 "Column headers not formatted correctly");

      // Copy it
      attributeNamePairs.push_back(pair);
   }
   row++; // For debugging

   if (mDebug)
   {
      auto out = ut::log::debug() << "BayesBase:ReadCsv: Parsed column pairs.";
      out.AddNote() << "Count: " << attributeNamePairs.size();
   }
   //
   // NormalizeAttNames(attributeNamePairs);

   // GetLine pulls the line off and leaves the
   // stream at the beginning at the next line.  So the
   // next redirection below pulls the data off the new line

   try
   {
      while (std::getline(ifs, line))
      {
         std::stringstream lineStream2(line);

         // Get the prototype name
         std::string entityType;
         if (!std::getline(lineStream2, entityType, delim))
         {
            auto out = ut::log::error() << "BayesBase::ReadCsv: Failed to read 1st (0th) column in row.";
            out.AddNote() << "Row: " << row;
            throw UtException("BayesBase:ReadCsv reading row failed!  Couldn't read 1st (0th) column");
         }
         // entityType = Trim(entityType);
         UtStringUtil::TrimWhiteSpace(entityType);

         attributeValuePairs.clear();

         std::string   cell;
         AttributePair valuePair;
         while (std::getline(lineStream2, cell, delim))
         {
            valuePair.mFirst = atof(cell.c_str());
            if (!std::getline(lineStream2, cell, delim))
            {
               throw UtException("BayesBase:ReadCsv reading value pair failed!  Couldn't read 1st (0th) column");
            }
            valuePair.mSecond = atof(cell.c_str());
            attributeValuePairs.push_back(valuePair);
         }

         // Ok, we have a row entry for an entity.
         AddDataRow(entityType, attributeNamePairs, attributeValuePairs);

         // Progress indication
         if (mDebug && (row % 100000 == 0))
         {
            auto out = ut::log::debug() << "BayesBase::ReadCsv: Parsing.";
            out.AddNote() << "Row: " << row;
         }
         row++;
      }
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "BayesBase::ReadCsv: Caught exception.";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      out.AddNote() << "Row: " << row;
      throw;
   }

   ifs.close();

   if (mDebug)
   {
      double rt  = runClock.GetClock() - startTime;
      auto   out = ut::log::debug() << "BayesBase:ReadCsv: Finished parsing file.";
      out.AddNote() << "File: " << aFileName;
      out.AddNote() << "Rows Read (including header): " << (row - 2); // row 1 = 1, i++ upon last row
      out.AddNote() << "Run Time: " << rt << " sec";
   }
   return true;
}

//! The given lists are matched in order, i.e., name[0] equates to value[0]. FYI
//! The name list is re-used for all rows.
void WsfBayesClassifier::AddDataRow(const std::string&                aEntityType,
                                    const std::vector<ColumnPair>&    aAttributeNamePairs,
                                    const std::vector<AttributePair>& aAttributeValuePairs)
{
   WsfStringId         entityTypeId(aEntityType);
   WsfEntityPrototype* ep = mEntType2Proto[entityTypeId];
   if (ep == nullptr)
   {
      ep                           = new WsfEntityPrototype;
      ep->mTypeName                = aEntityType;
      ep->mTypeNameID              = entityTypeId;
      mEntType2Proto[entityTypeId] = ep;
   }

   for (size_t i = 0; i < aAttributeNamePairs.size(); ++i)
   {
      const ColumnPair& columnNamePair = aAttributeNamePairs[i];
      if (columnNamePair.mFirst == columnNamePair.mSecond)
      {
         // The EntityProtype manages compounds or multi modals.
         ep->AddAttributeProtoRange(columnNamePair.mFirst, aAttributeValuePairs[i]);
      }
      else if (columnNamePair.mSecond == "sigma")
      {
         ep->AddAttributeProtoSigma(columnNamePair.mFirst, aAttributeValuePairs[i]);
      }
      else
      {
         // This should also be testing right during parsing, before a call here.
         AssertLocalBayesClassifier(false, "Not formatted correctly.");
      }
   }
}

void WsfBayesClassifier::Test()
{
   // Use each prototype to generate a track.  Then use that and assure
   // that track maps to the best in the class list and class.
   // for (map<int, EntityPrototype*>::iterator it = mEntType2Proto.begin();
   for (auto& it : mEntType2Proto)
   {
      // int entityTypeId = it->first;
      WsfStringId entityTypeId = it.first;
      // const std::string& entityType = entityTypeId.GetString();
      WsfEntityPrototype* proto = it.second;

      WsfTrack t;
      t.SetTargetType(entityTypeId);

      WsfAttributeMeasure* atFreq = nullptr;
      WsfAttributeMeasure* atPri  = nullptr;
      WsfAttributeMeasure* atPw   = nullptr;

      int attcnt = 0;
      for (auto& itAt : proto->mName2Attribase)
      {
         const std::string&                attName = itAt.first;
         WsfCompoundAttributeMeasure*      cm      = itAt.second;
         std::vector<WsfAttributeMeasure*> mm      = cm->mMultiModes;
         WsfAttributeMeasure*              am      = mm[0];
         // double value = am->mAverage;
         // double sigma = sqrt(am->mVariance);
         if (attName == "alt")
         {
            double LLA[3] = {40, -90, am->mAverage};
            t.SetLocationLLA(LLA[0], LLA[1], LLA[2]);
            t.SetLLA_LocationValid(true);
            attcnt++;
         }
         else if (attName == "freq")
         {
            atFreq = am;
            // WsfTrack::Signal s(am->mAverage - sigma, am->mAverage + sigma);
            // t.AddSignal(s);
            attcnt++;
         }
         else if (attName == "pri")
         {
            atPri = am;
            attcnt++;
         }
         else if (attName == "pw")
         {
            atPw = am;
            attcnt++;
         }
         else if (attName == "speed")
         {
         }
         else if (attName == "iff")
         {
         }
         else
         {
            AssertLocalBayesClassifier(false, "Prototype has unexpected attribute!");
         }
      }
      if ((atFreq != nullptr) || (atPri != nullptr) || (atPw != nullptr))
      {
         WsfTrack::Signal s;
         if (atFreq != nullptr)
         {
            double sigma = sqrt(atFreq->mVariance);
            s            = WsfTrack::Signal(atFreq->mAverage - sigma, atFreq->mAverage + sigma);
         }

         if (atPw != nullptr)
         {
            s.mPulseWidth = atPw->mAverage;
         }

         if (atPri != nullptr)
         {
            s.mPulseRepetitionInterval = atPri->mAverage;
         }

         t.AddSignal(s);
      }

      TrackAccessor*        ta  = this->GetTrackAccessor(t);
      WsfTrackAccessorDeep* tad = dynamic_cast<WsfTrackAccessorDeep*>(ta);
      AssertLocalBayesClassifier(attcnt == (int)tad->GetAttributeCount(), "Unexpected: All attributes not accessed !");

      ClassificationReportList classificationReportList;
      ////-----------------
      this->CreateClassList(*ta, classificationReportList);
      ////-----------------

      if (true)
      {
         WsfClassificationProcessor::PrintClassList(classificationReportList, entityTypeId);
      }

      ClassificationReport cr;
      //-----------------
      this->Classify(*ta, classificationReportList, cr);
      //-----------------

      WsfStringId UNKmEntityTypeId("UNKOWN");
      // aClassificationReportReport.mTypeScore = -1;

      AssertLocalBayesClassifier(cr.mTypeScore > 0.0, "???");
      // Very arguably, making a track from a vague cluster can
      // mean the classification will fall outside of it.  But
      // test is only called for our data set.
      AssertLocalBayesClassifier(cr.mEntityTypeId == entityTypeId, "???");
   }
}
