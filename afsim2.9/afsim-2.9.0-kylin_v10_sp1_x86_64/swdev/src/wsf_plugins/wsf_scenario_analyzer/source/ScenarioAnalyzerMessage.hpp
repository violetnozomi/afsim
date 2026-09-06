// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: ScenarioAnalyzerMessage.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/
#ifndef SCENARIOANALYZERMESSAGE_HPP
#define SCENARIOANALYZERMESSAGE_HPP

#include <iostream>
#include <vector>

#include "wsf_scenario_analyzer_lib_export.h"

#include "ScenarioFileLocation.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

//! Class for Scenario Analyzer messages that provides simple message building and writing
class WSF_SCENARIO_ANALYZER_LIB_EXPORT ScenarioAnalyzerMessage : public UtScriptAccessible
{
   public:
      static const int cNUM_SEVERITIES; //!< The number of severity levels
      enum SeverityLevel
      {
         cNOTE,                         //!< Message provides information
         cWARNING,                      //!< Message is a warning (may or may not be problematic)
         cERROR                         //!< Message is a critical error
      };

      ScenarioAnalyzerMessage();
      ScenarioAnalyzerMessage(const ScenarioAnalyzerMessage& aSrc);
      ScenarioAnalyzerMessage(const std::string&   aSuiteName,
                              const std::string&   aCheckName,
                              const std::string&   aDetails,
                              SeverityLevel        aSeverityLevel,
                              const std::vector<ScenarioFileLocation>& aLocations);

      ScenarioAnalyzerMessage(const std::string& aSuiteName,
                              const std::string& aCheckName,
                              const std::string& aDetails,
                              SeverityLevel      aSeverityLevel);

      ScenarioAnalyzerMessage(const std::string& aCategory,
                              const std::string& aDetails);

      void Clear();
      void ClearDetails();

      static const char* cSCRIPT_CLASS_NAME;
      const char* GetScriptClassName() const override { return cSCRIPT_CLASS_NAME; }

      bool IsSuccessfulResult() const                                           { return mSuccessful;   }
      std::string GetSuiteName() const                                          { return mSuiteName;    }
      std::string GetCheckName() const                                          { return mCheckName;    }
      std::string GetDetails() const                                            { return mDetails;      }
      SeverityLevel GetSeverity() const                                         { return mSeverity;     }
      const std::vector<ScenarioFileLocation>& GetScenarioFileLocations() const { return mLocations;    }

      void SetSuiteName(const std::string& aSuiteName)                    { mSuiteName = aSuiteName;         }
      void SetCheckName(const std::string& aCheckName)                    { mCheckName = aCheckName;         }
      void SetDetails(const std::string& aDetails)                        { mDetails = aDetails;             }
      void AddDetails(const std::string& aDetails)                        { mDetails += aDetails;            }
      void SetSeverity(SeverityLevel aSeverityLevel)                      { mSeverity = aSeverityLevel;      }
      void AddScenarioFileLocation(const ScenarioFileLocation& aLocation) { mLocations.push_back(aLocation); }

      void SetSuccessful();

      static std::vector<ScenarioAnalyzerMessage> ParseFromIStream(std::istream& aInputStream);

      static std::string SeverityString(SeverityLevel aSeverityLevel);
      static SeverityLevel SeverityLevelFromString(std::string aString);

      void Write(std::ostream& aOut = std::cout) const;

   protected:
      bool                              mSuccessful;        //!< The message represents a successful result
      std::string                       mSuiteName;         //!< Name of the suite
      std::string                       mCheckName;         //!< Name of the check
      std::string                       mDetails;           //!< Details of the message
      SeverityLevel                     mSeverity;          //!< Severity level of the message
      std::vector<ScenarioFileLocation> mLocations;         //!< Location of input blocks related to the message being generated
};

//! Class for accessing ScenarioAnalyzerMessage type in script
class ScriptScenarioAnalyzerMessageClass : public UtScriptClass
{
   public:
      ScriptScenarioAnalyzerMessageClass(UtScriptTypes* aTypesPtr);
      ~ScriptScenarioAnalyzerMessageClass() override = default;

      UT_DECLARE_SCRIPT_METHOD(Get);
      UT_DECLARE_SCRIPT_METHOD(GetSuiteName);
      UT_DECLARE_SCRIPT_METHOD(GetCheckName);
      UT_DECLARE_SCRIPT_METHOD(GetDetails);
      UT_DECLARE_SCRIPT_METHOD(GetSeverity);
      UT_DECLARE_SCRIPT_METHOD(GetRelatedFiles);
};

#endif