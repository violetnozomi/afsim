// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "OptionsTS.hpp"
#include "ui_OptionsTS.h"

PostProcessor::OptionsTS::OptionsTS()
   : OptionWidget("track_states")
   , mUIPtr(new Ui::OptionsTS)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsTS::~OptionsTS()
{
   delete mUIPtr;
}

void PostProcessor::OptionsTS::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   if (mUIPtr->mButtonAH->isChecked())
   {
      aStream << "   AsItHappens\n";
   }
   else if (mUIPtr->mButtonAI->isChecked())
   {
      aStream << "   AtInterval\n";
   }

   aStream << "end_options\n";

   // Track Options
   std::string trackOptions = "";
   if (mUIPtr->mCheckBoxIFF->isChecked())
   {
      trackOptions += "IFF\n";
   }
   if (mUIPtr->mCheckBoxNCTR->isChecked())
   {
      trackOptions += "   NCTR\n";
   }
   if (mUIPtr->mCheckBoxTT->isChecked())
   {
      trackOptions += "   TrackType\n";
   }
   if (mUIPtr->mCheckBoxTQ->isChecked())
   {
      trackOptions += "   TrackQuality\n";
   }
   if (mUIPtr->mCheckBoxSP->isChecked())
   {
      trackOptions += "   SourcePlatform\n";
   }
   if (mUIPtr->mCheckBoxDT->isChecked())
   {
      trackOptions += "   DominantTarget\n";
   }
   if (mUIPtr->mCheckBoxCT->isChecked())
   {
      trackOptions += "   ConstituentTargets\n";
   }
   if (mUIPtr->mCheckBoxSC->isChecked())
   {
      trackOptions += "   SensorClass\n";
   }
   if (mUIPtr->mCheckBoxIPP->isChecked())
   {
      trackOptions += "   ImpactPointPrediction\n";
   }
   if (mUIPtr->mCheckBoxLPP->isChecked())
   {
      trackOptions += "   LaunchPointPrediction\n";
   }

   aStream << "track_options\n" << trackOptions << "end_track_options\n";

   // Measurement Options
   std::string measurementOptions = "";
   if (mUIPtr->mCheckBoxLLAT->isChecked())
   {
      measurementOptions += "   LLATruth\n";
   }
   if (mUIPtr->mCheckBoxLLAP->isChecked())
   {
      measurementOptions += "   LLAPerceived\n";
   }
   if (mUIPtr->mCheckBoxXYZT->isChecked())
   {
      measurementOptions += "   XYZTruth\n";
   }
   if (mUIPtr->mCheckBoxXYZP->isChecked())
   {
      measurementOptions += "   XYZPerceived\n";
   }
   if (mUIPtr->mCheckBoxVT->isChecked())
   {
      measurementOptions += "   VelocityTruth\n";
   }
   if (mUIPtr->mCheckBoxVP->isChecked())
   {
      measurementOptions += "   VelocityPerceived\n";
   }
   if (mUIPtr->mCheckBoxSpT->isChecked())
   {
      measurementOptions += "   SpeedTruth\n";
   }
   if (mUIPtr->mCheckBoxSpP->isChecked())
   {
      measurementOptions += "   SpeedPerceived\n";
   }
   if (mUIPtr->mCheckBoxHT->isChecked())
   {
      measurementOptions += "   HeadingTruth\n";
   }
   if (mUIPtr->mCheckBoxHP->isChecked())
   {
      measurementOptions += "   HeadingPerceived\n";
   }
   if (mUIPtr->mCheckBoxSRT->isChecked())
   {
      measurementOptions += "   SlantRangeTruth\n";
   }
   if (mUIPtr->mCheckBoxSRP->isChecked())
   {
      measurementOptions += "   SlantRangePerceived\n";
   }
   if (mUIPtr->mCheckBoxGRT->isChecked())
   {
      measurementOptions += "   GroundRangeTruth\n";
   }
   if (mUIPtr->mCheckBoxGRP->isChecked())
   {
      measurementOptions += "   GroundRangePerceived\n";
   }
   if (mUIPtr->mCheckBoxAAT->isChecked())
   {
      measurementOptions += "   AzimuthTruth\n";
   }
   if (mUIPtr->mCheckBoxAAP->isChecked())
   {
      measurementOptions += "   AzimuthPerceived\n";
   }
   if (mUIPtr->mCheckBoxEAT->isChecked())
   {
      measurementOptions += "   ElevationAngleTruth\n";
   }
   if (mUIPtr->mCheckBoxEAP->isChecked())
   {
      measurementOptions += "   ElevationAnglePerceived\n";
   }
   aStream << "measurement_options\n" << measurementOptions << "end_measurement_options\n";
}

void PostProcessor::OptionsTS::RestoreDefaults()
{
   mUIPtr->mButtonAH->setChecked(true);

   mUIPtr->mCheckBoxIFF->setChecked(true);
   mUIPtr->mCheckBoxNCTR->setChecked(true);
   mUIPtr->mCheckBoxTT->setChecked(true);
   mUIPtr->mCheckBoxTQ->setChecked(false);
   mUIPtr->mCheckBoxSP->setChecked(false);
   mUIPtr->mCheckBoxDT->setChecked(true);
   mUIPtr->mCheckBoxCT->setChecked(true);
   mUIPtr->mCheckBoxSC->setChecked(false);
   mUIPtr->mCheckBoxIPP->setChecked(false);
   mUIPtr->mCheckBoxLPP->setChecked(false);

   mUIPtr->mCheckBoxLLAT->setChecked(false);
   mUIPtr->mCheckBoxLLAP->setChecked(false);
   mUIPtr->mCheckBoxXYZT->setChecked(false);
   mUIPtr->mCheckBoxXYZP->setChecked(false);
   mUIPtr->mCheckBoxVT->setChecked(false);
   mUIPtr->mCheckBoxVP->setChecked(false);
   mUIPtr->mCheckBoxSpT->setChecked(false);
   mUIPtr->mCheckBoxSpP->setChecked(false);
   mUIPtr->mCheckBoxHT->setChecked(false);
   mUIPtr->mCheckBoxHP->setChecked(false);
   mUIPtr->mCheckBoxSRT->setChecked(false);
   mUIPtr->mCheckBoxSRP->setChecked(false);
   mUIPtr->mCheckBoxGRT->setChecked(false);
   mUIPtr->mCheckBoxGRP->setChecked(false);
   mUIPtr->mCheckBoxAAT->setChecked(false);
   mUIPtr->mCheckBoxAAP->setChecked(false);
   mUIPtr->mCheckBoxEAT->setChecked(false);
   mUIPtr->mCheckBoxEAP->setChecked(false);
}