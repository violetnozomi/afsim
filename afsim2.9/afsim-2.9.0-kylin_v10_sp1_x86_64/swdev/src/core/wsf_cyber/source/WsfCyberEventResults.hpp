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

#ifndef WSFCYBEREVENTRESULTS_HPP
#define WSFCYBEREVENTRESULTS_HPP

#include "wsf_cyber_export.h"

#include <ostream>
#include <sstream>

#include "WsfCyberEventResult.hpp"

namespace wsf
{
namespace event
{
// ===================================================================================================
class WSF_CYBER_EXPORT CyberAttackInitiated : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_ATTACK_INITIATED";
   CyberAttackInitiated(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberAttackSucceeded : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_ATTACK_SUCCEEDED";
   CyberAttackSucceeded(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberAttackFailed : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_ATTACK_FAILED";
   CyberAttackFailed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberAttackDetected : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_ATTACK_DETECTED";
   CyberAttackDetected(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberAttackAttributed : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_ATTACK_ATTRIBUTED";
   CyberAttackAttributed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberAttackRecovery : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_ATTACK_RECOVERY";
   CyberAttackRecovery(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberScanInitiated : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_SCAN_INITIATED";
   CyberScanInitiated(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberScanSucceeded : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_SCAN_SUCCEEDED";
   CyberScanSucceeded(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberScanFailed : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_SCAN_FAILED";
   CyberScanFailed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberScanDetected : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_SCAN_DETECTED";
   CyberScanDetected(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberScanAttributed : public CyberResult
{
public:
   static constexpr const char* cNAME = "CYBER_SCAN_ATTRIBUTED";
   CyberScanAttributed(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberTriggerExecution : public CyberTriggerResult
{
public:
   static constexpr const char* cNAME = "CYBER_TRIGGER_EXECUTION";
   CyberTriggerExecution(double aSimTime, const wsf::cyber::trigger::Trigger& aTrigger, Settings aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberTriggerEvaluation : public CyberTriggerResult
{
public:
   static constexpr const char* cNAME = "CYBER_TRIGGER_EVALUATION";
   CyberTriggerEvaluation(double                              aSimTime,
                          const wsf::cyber::trigger::Trigger& aTrigger,
                          bool                                aEvaluationResult,
                          Settings                            aSettings = Settings());

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   bool GetEvaluationResult() const { return mEvaluationResult; }

protected:
   std::string FormatOutput(const std::string& aObserverType, bool aIsCSV = false) const override;

   bool mEvaluationResult;
};

} // namespace event
} // namespace wsf

#endif
