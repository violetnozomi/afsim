// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberEventResults.hpp"

#include "WsfCyberEngagement.hpp"
#include "WsfCyberEventResult.hpp"
#include "WsfCyberEventUtils.hpp"
#include "WsfCyberObserver.hpp"
#include "WsfEventOutput.hpp"
#include "WsfEventUtils.hpp"
#include "WsfSimulation.hpp"
#include "triggers/WsfCyberTrigger.hpp"

namespace wsf
{
namespace event
{
// ===================================================================================================
CyberAttackInitiated::CyberAttackInitiated(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberAttackInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_ATTACK_INITIATED", mEngagement) << std::endl;
}

// ===================================================================================================
void CyberAttackInitiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_ATTACK_INITIATED", mEngagement) << '\n';
}

// ===================================================================================================
CyberAttackSucceeded::CyberAttackSucceeded(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberAttackSucceeded::Print(std::ostream& aStream) const
{
   std::string statusReport = mEngagement.GetStatusReportSuccess() ? "Succeeded" : "Failed";
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_ATTACK_SUCCEEDED", mEngagement) << std::endl;
   aStream << "   Success: Threshold: "
           << utils::GetThresholdDraw(mEngagement.GetAttackSuccessThreshold(), mEngagement.GetAttackDraw()) << std::endl;
   aStream << "   Report: Status: " << statusReport << ' '
           << utils::GetThresholdDraw(mEngagement.GetStatusReportThreshold(), mEngagement.GetStatusReportDraw())
           << std::endl;
   aStream << "   Detect: "
           << utils::GetThresholdDraw(mEngagement.GetAttackDetectionThreshold(), mEngagement.GetAttackDetectionDraw())
           << std::endl;
}

// ===================================================================================================
void CyberAttackSucceeded::PrintCSV(std::ostream& aStream) const
{
   std::string statusReport = mEngagement.GetStatusReportSuccess() ? "Succeeded" : "Failed";
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_ATTACK_SUCCEEDED", mEngagement) << ','
           << mEngagement.GetAttackSuccessThreshold() << ',' << mEngagement.GetAttackDraw() << ',' << statusReport
           << ',' << mEngagement.GetStatusReportThreshold() << ',' << mEngagement.GetStatusReportDraw() << ','
           << mEngagement.GetAttackDetectionThreshold() << ',' << mEngagement.GetAttackDetectionDraw() << '\n';
}

// ===================================================================================================
CyberAttackFailed::CyberAttackFailed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberAttackFailed::Print(std::ostream& aStream) const
{
   std::string reason       = utils::GetAttackFailureReason(mEngagement);
   std::string reportStatus = utils::GetAttackReportStatus(mEngagement);
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_ATTACK_FAILED", mEngagement) << std::endl;
   aStream << "   Reason: " << reason << " Success: "
           << utils::GetThresholdDraw(mEngagement.GetAttackSuccessThreshold(), mEngagement.GetAttackDraw()) << std::endl;
   aStream << "   Report: Status: " << reportStatus << " "
           << utils::GetThresholdDraw(mEngagement.GetStatusReportThreshold(), mEngagement.GetStatusReportDraw())
           << std::endl;
}

// ===================================================================================================
void CyberAttackFailed::PrintCSV(std::ostream& aStream) const
{
   std::string reason       = utils::GetAttackFailureReason(mEngagement);
   std::string reportStatus = utils::GetAttackReportStatus(mEngagement);
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_ATTACK_FAILED", mEngagement) << ',' << reason << ','
           << mEngagement.GetAttackSuccessThreshold() << ',' << mEngagement.GetAttackDraw() << ',' << reportStatus
           << ',' << mEngagement.GetStatusReportThreshold() << ',' << mEngagement.GetStatusReportDraw() << '\n';
}

// ===================================================================================================
CyberAttackDetected::CyberAttackDetected(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberAttackDetected::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_ATTACK_DETECTED", mEngagement) << std::endl;
}

// ===================================================================================================
void CyberAttackDetected::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_ATTACK_DETECTED", mEngagement) << '\n';
}

// ===================================================================================================
CyberAttackAttributed::CyberAttackAttributed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberAttackAttributed::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_ATTACK_ATTRIBUTED", mEngagement) << std::endl;
}

// ===================================================================================================
void CyberAttackAttributed::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_ATTACK_ATTRIBUTED", mEngagement) << '\n';
}

// ===================================================================================================
CyberAttackRecovery::CyberAttackRecovery(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberAttackRecovery::Print(std::ostream& aStream) const
{
   std::string status = utils::GetImmunityStatus(mEngagement);
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_ATTACK_RECOVERY", mEngagement) << std::endl;
   aStream << "   Immunity: Status: " << status << " "
           << utils::GetThresholdDraw(mEngagement.GetImmunityThreshold(), mEngagement.GetImmunityDraw()) << std::endl;
}

// ===================================================================================================
void CyberAttackRecovery::PrintCSV(std::ostream& aStream) const
{
   std::string status = utils::GetImmunityStatus(mEngagement);
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_ATTACK_RECOVERY", mEngagement) << ',' << status << ','
           << mEngagement.GetImmunityThreshold() << ',' << mEngagement.GetImmunityDraw() << '\n';
}

// ===================================================================================================
CyberScanInitiated::CyberScanInitiated(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberScanInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_SCAN_INITIATED", mEngagement) << std::endl;
}

// ===================================================================================================
void CyberScanInitiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_SCAN_INITITATED", mEngagement) << '\n';
}

// ===================================================================================================
CyberScanSucceeded::CyberScanSucceeded(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberScanSucceeded::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_SCAN_SUCCEEDED", mEngagement) << std::endl;
   aStream << "   Scan_Detection: "
           << utils::GetThresholdDraw(mEngagement.GetScanDetectionThreshold(), mEngagement.GetScanDetectionDraw())
           << std::endl;
}

// ===================================================================================================
void CyberScanSucceeded::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_SCAN_SUCCEEDED", mEngagement) << ','
           << mEngagement.GetScanDetectionThreshold() << ',' << mEngagement.GetScanDetectionDraw() << '\n';
}

// ===================================================================================================
CyberScanFailed::CyberScanFailed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberScanFailed::Print(std::ostream& aStream) const
{
   std::string reason = utils::GetScanFailureReason(mEngagement);
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_SCAN_FAILED", mEngagement) << std::endl;
   aStream << "   Reason: " << reason << " Scan_Detection: "
           << utils::GetThresholdDraw(mEngagement.GetScanDetectionThreshold(), mEngagement.GetScanDetectionDraw())
           << std::endl;
}

// ===================================================================================================
void CyberScanFailed::PrintCSV(std::ostream& aStream) const
{
   std::string reason = utils::GetScanFailureReason(mEngagement);
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_SCAN_FAILED", mEngagement) << ',' << reason << ','
           << mEngagement.GetScanDetectionThreshold() << ',' << mEngagement.GetScanDetectionDraw() << '\n';
}

// ===================================================================================================
CyberScanDetected::CyberScanDetected(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberScanDetected::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_SCAN_DETECTED", mEngagement) << std::endl;
}

// ===================================================================================================
void CyberScanDetected::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_SCAN_DETECTED", mEngagement) << '\n';
}

// ===================================================================================================
CyberScanAttributed::CyberScanAttributed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings)
   : CyberResult(aSimTime, aEngagement, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberScanAttributed::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << utils::GetCyberEvent("CYBER_SCAN_ATTRIBUTED", mEngagement) << std::endl;
}

// ===================================================================================================
void CyberScanAttributed::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << utilsCSV::GetCyberEvent("CYBER_SCAN_ATTRIBUTED", mEngagement) << '\n';
}

// ===================================================================================================
CyberTriggerExecution::CyberTriggerExecution(double aSimTime, const wsf::cyber::trigger::Trigger& aTrigger, Settings aSettings)
   : CyberTriggerResult(aSimTime, aTrigger, std::move(aSettings), cNAME)
{
}

// ===================================================================================================
void CyberTriggerExecution::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << FormatOutput("CYBER_TRIGGER_EXECUTION") << std::endl;
}

// ===================================================================================================
void CyberTriggerExecution::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << FormatOutput("CYBER_TRIGGER_EXECUTION", true) << std::endl;
}

CyberTriggerEvaluation::CyberTriggerEvaluation(double                              aSimTime,
                                               const wsf::cyber::trigger::Trigger& aTrigger,
                                               bool                                aEvaluationResult,
                                               Settings                            aSettings)
   : CyberTriggerResult(aSimTime, aTrigger, std::move(aSettings), cNAME)
   , mEvaluationResult(aEvaluationResult)
{
}

// ===================================================================================================
std::string CyberTriggerEvaluation::FormatOutput(const std::string& aObserverType,
                                                 bool               aIsCSV) const // = false
{
   auto delim      = (aIsCSV) ? std::string(",") : std::string(" ");
   auto evalResult = (mEvaluationResult) ? std::string("true") : std::string("false");

   auto returnString = CyberTriggerResult::FormatOutput(aObserverType, aIsCSV);
   returnString += (delim + evalResult);

   return returnString;
}

// ===================================================================================================
void CyberTriggerEvaluation::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << FormatOutput("CYBER_TRIGGER_EVALUATION") << std::endl;
}

// ===================================================================================================
void CyberTriggerEvaluation::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << FormatOutput("CYBER_TRIGGER_EVALUATION", true) << std::endl;
}

} // namespace event
} // namespace wsf
