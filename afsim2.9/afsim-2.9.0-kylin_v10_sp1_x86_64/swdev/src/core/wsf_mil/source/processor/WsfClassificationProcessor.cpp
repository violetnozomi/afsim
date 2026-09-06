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

#include "WsfClassificationProcessor.hpp"

class WsfBayesClassification;


#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfBayesClassification.hpp"
#include "WsfTrackClassifier.hpp"


namespace
{
void AssertLocalClassificationProcessor(bool aTruth, const char* aMsg)
{
   if (!aTruth)
   {
      ut::log::error() << "AssertLocalClassificationProcessor: " << aMsg;
      assert(aTruth);
   }
}
} // namespace

WsfClassificationProcessor::WsfClassificationProcessor(WsfScenario& aScenario)
   : WsfLinkedProcessor(aScenario)
   , mClassifier(nullptr)
   , mTest(false)
{
}

WsfClassificationProcessor::WsfClassificationProcessor(const WsfClassificationProcessor& aSrc)
   : WsfLinkedProcessor(aSrc)
   , mClassifier(nullptr)
   , mTest(aSrc.mTest)
{
   if (aSrc.mClassifier != nullptr)
   {
      mClassifier = aSrc.mClassifier->Clone();
   }
}

// virtual
WsfClassificationProcessor::~WsfClassificationProcessor()
{
   delete mClassifier;
}

// virtual
bool WsfClassificationProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "classifier")
   {
      UtInputBlock blockOutter(aInput,
                               "end_"
                               "classifier");

      // See which type
      std::string type;
      aInput.ReadCommand(type);

      if (!(type == "rules" || type == "bayes"))
      {
         // throw UtInput::BadValue(aInput, "Not allowed - including self-type into mis ID list!");
         throw UtInput::UnknownCommand(aInput);
      }

      delete mClassifier;
      mClassifier = nullptr;

      if (type == "rules")
      {
         mClassifier = new WsfClassifierTabulated();
      }
      else if (type == "bayes")
      {
         mClassifier = WsfBayesClassification::CreateClassifier();
      }
      else
      {
         // throw UtInput::BadValue(aInput, "Not allowed - including self-type into mis ID list!");
         throw UtInput::UnknownCommand(aInput);
      }

      while (blockOutter.ReadCommand())
      {
         myCommand = mClassifier->ProcessInput(aInput); // blockOutter.GetInput());
         if (!myCommand)
         {
            AssertLocalClassificationProcessor(myCommand, ("Classifier says not its command! Type = " + type).c_str());
            throw UtInput::BadValue(aInput, ("Command not allowed! " + aInput.GetCommand()).c_str());
         }
      }
      ////mClassifier->Initialize();

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfClassificationProcessor: Successfully populated classifier.";
         out.AddNote() << "Type: " << type;
      }

      if (mTest)
      {
         { // RAII block
            auto out = ut::log::debug() << "WsfClassificationProcessor: Testing classifier.";
            out.AddNote() << "Type: " << type;
         }
         mClassifier->Test();
      }
   }
   else if (command == "test")
   {
      mTest = true;
   }
   else
   {
      myCommand = WsfLinkedProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfClassificationProcessor::Initialize(double aSimTime)
{
   return WsfLinkedProcessor::Initialize(aSimTime);
}

// virtual
bool WsfClassificationProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool processed = true;
   return processed;
}

void WsfClassificationProcessor::CreateClassList(const WsfTrack&           aTrack,
                                                 ClassificationReportList& aClassificationReportList) const
{
   TrackAccessor* trackAccessor = mClassifier->GetTrackAccessor(aTrack);
   mClassifier->CreateClassList(*trackAccessor, aClassificationReportList);
   delete trackAccessor;
}

// static
// void WsfClassificationProcessor::PrintClassList(ClassificationReportList& ClassificationReportList, int
// aTrueEntityType/* = -1*/)
void WsfClassificationProcessor::PrintClassList(ClassificationReportList& ClassificationReportList,
                                                WsfStringId               aTrueEntityType)
{
   // string trueEntityType = WsfStringId::GetString(aTrueEntityType);
   const std::string& trueEntityType = aTrueEntityType.GetString();
   auto out = ut::log::info() << "WsfClassificationProcessor: Classifier ran for True Type and Reported Candidates.";
   out.AddNote() << "True Type: " << trueEntityType;
   out.AddNote() << "Reported Candidates: " << ClassificationReportList.size();
   for (ClassificationReport& report : ClassificationReportList)
   {
      // string tp = WsfStringId::GetString(report.mEntityTypeId);
      std::string tp   = report.mEntityTypeId.GetString();
      auto        note = out.AddNote() << "Type: " << tp;
      note.AddNote() << "P(e): " << report.mTypeScore;
   }
}
