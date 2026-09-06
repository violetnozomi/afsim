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

#include "TrajectoryParticipants.hpp"
#include <ostream>
#include "ui_TrajectoryParticipants.h"

PostProcessor::TrajectoryParticipants::TrajectoryParticipants()
   : ReportWidget()
   , mUIPtr(new Ui::TrajectoryParticipants)
{
   mUIPtr->setupUi(this);
}

PostProcessor::TrajectoryParticipants::~TrajectoryParticipants()
{
   delete mUIPtr;
}

void PostProcessor::TrajectoryParticipants::WriteData(std::ostream& aStream)
{
   aStream << "participant_type";
   if (mUIPtr->mButtonPlatforms->isChecked())
   {
      aStream << " Platforms\n";
   }
   else if (mUIPtr->mButtonMissiles->isChecked())
   {
      aStream << " Missiles\n";
   }
   else if (mUIPtr->mButtonBoth->isChecked())
   {
      aStream << " Both\n";
   }

   aStream << "participants";
   if (mUIPtr->mButtonAll->isChecked())
   {
      aStream << " all_participants\n";
   }
   else if (mUIPtr->mButtonAllAir->isChecked())
   {
      aStream << " all_air_participants\n";
   }
   else if (mUIPtr->mButtonSelected->isChecked())
   {
      aStream << " selected_participants\n";
   }
}

void PostProcessor::TrajectoryParticipants::RestoreDefaults()
{
   // TODO
}
