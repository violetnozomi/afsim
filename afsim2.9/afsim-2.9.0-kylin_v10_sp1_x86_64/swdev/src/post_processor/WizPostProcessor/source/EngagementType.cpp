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

#include "EngagementType.hpp"
#include "ui_EngagementType.h"

#include "OptionsDFH.hpp"
#include "OptionsET.hpp"
#include "OptionsGAH.hpp"
#include "OptionsPAPA.hpp"
#include "OptionsTD.hpp"
#include "OptionsThSm.hpp"
#include "OptionsTMS.hpp"
#include "OptionsTS.hpp"
#include "OptionsWE.hpp"

PostProcessor::EngagementType::EngagementType()
   : ReportWidget()
   , mUIPtr(new Ui::EngagementType)
   , mOptionName("track_event_history") // by default, the Geopmetric Action History radio button is selected
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mButtonGAH, &QRadioButton::pressed, this, &EngagementType::GAHPressedHandler);
   connect(mUIPtr->mButtonDFH, &QRadioButton::pressed, this, &EngagementType::DFHPressedHandler);
   connect(mUIPtr->mButtonET, &QRadioButton::pressed, this, &EngagementType::ETPressedHandler);
   connect(mUIPtr->mButtonPAPA, &QRadioButton::pressed, this, &EngagementType::PAPAPressedHandler);
   connect(mUIPtr->mButtonTS, &QRadioButton::pressed, this, &EngagementType::TSPressedHandler);
   connect(mUIPtr->mButtonTD, &QRadioButton::pressed, this, &EngagementType::TDPressedHandler);
   connect(mUIPtr->mButtonThSm, &QRadioButton::pressed, this, &EngagementType::ThSmPressedHandler);
   connect(mUIPtr->mButtonTMS, &QRadioButton::pressed, this, &EngagementType::TMSPressedHandler);

   connect(mUIPtr->mButtonEFL, &QRadioButton::pressed, this, &EngagementType::EFLPressedHandler);
   connect(mUIPtr->mButtonMFP, &QRadioButton::pressed, this, &EngagementType::MFPPressedHandler);
   connect(mUIPtr->mButtonIS, &QRadioButton::pressed, this, &EngagementType::ISPressedHandler);
   connect(mUIPtr->mButtonActing, &QRadioButton::pressed, this, &EngagementType::ActingPressedHandler);
   connect(mUIPtr->mButtonAction, &QRadioButton::pressed, this, &EngagementType::ActionPressedHandler);
   connect(mUIPtr->mButtonTErH, &QRadioButton::pressed, this, &EngagementType::TErHPressedHandler);
   connect(mUIPtr->mButtonTEvH, &QRadioButton::pressed, this, &EngagementType::TEvHPressedHandler);
   connect(mUIPtr->mButtonTStats, &QRadioButton::pressed, this, &EngagementType::TStatsPressedHandler);
   connect(mUIPtr->mButtonFPWH, &QRadioButton::pressed, this, &EngagementType::FPWHPressedHandler);
   connect(mUIPtr->mButtonWE, &QRadioButton::pressed, this, &EngagementType::WEPressedHandler);

   connect(mUIPtr->toolButton, &QToolButton::pressed, this, &EngagementType::emitReportOptionsPressed);

   mUIPtr->toolButton->setEnabled(false);

   mOptionWidgets = {new OptionsGAH(),
                     new OptionsDFH(),
                     new OptionsET(),
                     new OptionsPAPA(),
                     new OptionsTS(),
                     new OptionsThSm(),
                     new OptionsTD(),
                     new OptionsTMS(),
                     new OptionsWE()
   };
}

PostProcessor::EngagementType::~EngagementType()
{
   delete mUIPtr;
}

void PostProcessor::EngagementType::GAHPressedHandler()
{
   mOptionName = "geometric_action_history";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::DFHPressedHandler()
{
   mOptionName = "damage_factor_history";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::ETPressedHandler()
{
   mOptionName = "event_timing";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::PAPAPressedHandler()
{
   mOptionName = "papa";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::TSPressedHandler()
{
   mOptionName = "track_states";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::TDPressedHandler()
{
   mOptionName = "threat_deployment";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::ThSmPressedHandler()
{
   mOptionName = "threat_summary";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::TMSPressedHandler()
{
   mOptionName = "threat_mode_summary";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::EFLPressedHandler()
{
   mOptionName = "engagement_failure_log";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::MFPPressedHandler()
{
   mOptionName = "missile_flight_profile";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::ISPressedHandler()
{
   mOptionName = "interval_statistics";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::ActingPressedHandler()
{
   mOptionName = "summary_by_acting";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::ActionPressedHandler()
{
   mOptionName = "summary_by_action";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::TErHPressedHandler()
{
   mOptionName = "track_error_history";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::TEvHPressedHandler()
{
   mOptionName = "track_event_history";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::TStatsPressedHandler()
{
   mOptionName = "track_statistics";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::FPWHPressedHandler()
{
   mOptionName = "flight_profile_waypoint_history";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::EngagementType::WEPressedHandler()
{
   mOptionName = "weapon_expenditures";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::EngagementType::WriteData(std::ostream& aStream)
{
   aStream << "report_type " << mOptionName << "\n";
   for (auto w : mOptionWidgets)
   {
      if (mOptionName.compare(w->GetName()) == 0)
      {
         w->WriteData(aStream);
         break;
      }
   }
}

void PostProcessor::EngagementType::RestoreDefaults()
{
   mOptionName = "track_event_history";
   mUIPtr->mButtonTEvH->setChecked(true);
   mUIPtr->toolButton->setEnabled(true);
}
