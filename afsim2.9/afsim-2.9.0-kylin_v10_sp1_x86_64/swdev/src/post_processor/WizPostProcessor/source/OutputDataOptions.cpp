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

#include "OutputDataOptions.hpp"
#include <ostream>
#include "ui_OutputDataOptions.h"

PostProcessor::OutputDataOptions::OutputDataOptions()
   : ReportWidget()
   , mUIPtr(new Ui::OutputDataOptions)
{
   mUIPtr->setupUi(this);
}

PostProcessor::OutputDataOptions::~OutputDataOptions()
{
   delete mUIPtr;
}

void PostProcessor::OutputDataOptions::WriteData(std::ostream& aStream)
{
   std::string options = "";
   if (mUIPtr->mCheckBoxPos->isChecked())
   {
      options += "   Position\n";
   }
   if (mUIPtr->mCheckBoxLatLon->isChecked())
   {
      options += "   LatLon\n";
   }
   if (mUIPtr->mCheckBoxAltMSL->isChecked())
   {
      options += "   AltitudeMSL\n";
   }
   if (mUIPtr->mCheckBoxAltAGL->isChecked())
   {
      options += "   AltitudeAGL\n";
   }
   if (mUIPtr->mCheckBoxElev->isChecked())
   {
      options += "   GroundElevation\n";
   }
   if (mUIPtr->mCheckBoxOrientation->isChecked())
   {
      options += "   Orientation\n";
   }
   if (mUIPtr->mCheckBoxHeading->isChecked())
   {
      options += "   Heading\n";
   }
   if (mUIPtr->mCheckBoxRoll->isChecked())
   {
      options += "   Roll\n";
   }
   if (mUIPtr->mCheckBoxPitch->isChecked())
   {
      options += "   Pitch\n";
   }
   if (mUIPtr->mCheckBoxVel->isChecked())
   {
      options += "   Velocity\n";
   }
   if (mUIPtr->mCheckBoxSpeed->isChecked())
   {
      options += "   Speed\n";
   }
   if (mUIPtr->mCheckBoxGR->isChecked())
   {
      options += "   GroundRate\n";
   }
   if (mUIPtr->mCheckBoxAR->isChecked())
   {
      options += "   AltitudeRate\n";
   }
   if (mUIPtr->mCheckBoxAFV->isChecked())
   {
      options += "   AccelerationFromVelocity\n";
   }
   if (mUIPtr->mCheckBoxAFP->isChecked())
   {
      options += "   AccelerationFromPosition\n";
   }
   if (mUIPtr->mCheckBoxFC->isChecked())
   {
      options += "   FuelConsumption\n";
   }
   if (mUIPtr->mCheckBoxFR->isChecked())
   {
      options += "   FuelRemaining\n";
   }
   aStream << "output_data_options\n" << options << "end_output_data_options\n";
}

void PostProcessor::OutputDataOptions::RestoreDefaults()
{
   mUIPtr->mCheckBoxPitch->setChecked(false);
   mUIPtr->mCheckBoxLatLon->setChecked(true);
   mUIPtr->mCheckBoxAltMSL->setChecked(true);
   mUIPtr->mCheckBoxAltAGL->setChecked(false);
   mUIPtr->mCheckBoxElev->setChecked(false);
   mUIPtr->mCheckBoxOrientation->setChecked(false);
   mUIPtr->mCheckBoxHeading->setChecked(false);
   mUIPtr->mCheckBoxRoll->setChecked(false);
   mUIPtr->mCheckBoxPitch->setChecked(false);
   mUIPtr->mCheckBoxVel->setChecked(false);
   mUIPtr->mCheckBoxSpeed->setChecked(false);
   mUIPtr->mCheckBoxGR->setChecked(false);
   mUIPtr->mCheckBoxAR->setChecked(false);
   mUIPtr->mCheckBoxAFV->setChecked(false);
   mUIPtr->mCheckBoxAFP->setChecked(false);
   mUIPtr->mCheckBoxFC->setChecked(false);
   mUIPtr->mCheckBoxFR->setChecked(false);
}
