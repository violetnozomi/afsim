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
*   File: ScenarioAnalyzerMessage.cpp
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
#include "ScenarioAnalyzerMessage.hpp"

#include "ScenarioAnalyzerUtilities.hpp"
#include "UtMemory.hpp"
#include "WsfException.hpp"

#include <algorithm>
#include <sstream>

const char* ScenarioAnalyzerMessage::cSCRIPT_CLASS_NAME = "ScenarioAnalyzerMessage";

// NOTE: This must change if any more items are added to
//       ScenarioAnalyzerMessage::SeverityLevel enum group.
const int ScenarioAnalyzerMessage::cNUM_SEVERITIES = 3;

namespace
{
static const std::string cSCENARIO_ANALYZER = "SCENARIO_ANALYZER";
static const std::string cSUCCESS_MESSAGE = "Scenario passed this check.";
}

//! Construct a default ScenarioAnalyzerMessage as a note
ScenarioAnalyzerMessage::ScenarioAnalyzerMessage()
   : mSuccessful(false),
     mSeverity(cNOTE)
     /* Use defaults for others*/
{
}

//! Construct a ScenarioAnalyzerMessage from another
ScenarioAnalyzerMessage::ScenarioAnalyzerMessage(const ScenarioAnalyzerMessage& aSrc)
   : mSuccessful(aSrc.mSuccessful),
     mSuiteName(aSrc.mSuiteName),
     mCheckName(aSrc.mCheckName),
     mDetails(aSrc.mDetails),
     mSeverity(aSrc.mSeverity),
     mLocations(aSrc.mLocations.begin(), aSrc.mLocations.end())
{
}

//! Construct a ScenarioAnalyzerMessage with a full set of details
ScenarioAnalyzerMessage::ScenarioAnalyzerMessage(const std::string& aSuiteName,
                                                 const std::string& aCheckName,
                                                 const std::string& aDetails,
                                                 SeverityLevel      aSeverityLevel,
                                                 const std::vector<ScenarioFileLocation>& aLocations)
   : mSuccessful(false),
     mSuiteName(aSuiteName),
     mCheckName(aCheckName),
     mDetails(aDetails),
     mSeverity(aSeverityLevel),
     mLocations(aLocations.begin(), aLocations.end())
{
}

//! Construct a ScenarioAnalyzerMessage without related files
ScenarioAnalyzerMessage::ScenarioAnalyzerMessage(const std::string& aSuiteName,
                                                 const std::string& aCheckName,
                                                 const std::string& aDetails,
                                                 SeverityLevel      aSeverityLevel)
   : ScenarioAnalyzerMessage(aSuiteName, aCheckName, aDetails, aSeverityLevel, {})
{
}

//! Construct a ScenarioAnalyzerMessage that is a basic session note
ScenarioAnalyzerMessage::ScenarioAnalyzerMessage(const std::string& aCategory,
                                                 const std::string& aDetails)
   : ScenarioAnalyzerMessage("SESSION_NOTE", aCategory, aDetails, cNOTE, {})
{
}

//! Clear all message details, and set severity back to default
void ScenarioAnalyzerMessage::Clear()
{
   ClearDetails();
   mSuiteName.clear();
   mCheckName.clear();
   mSeverity = cNOTE;
}

//! Clear only message details and locations- useful when using one
//! message object and wanting to clear unshared details.
void ScenarioAnalyzerMessage::ClearDetails()
{
   mDetails.clear();
   mLocations.clear();
}

//! Set default details and type for a successful message
void ScenarioAnalyzerMessage::SetSuccessful()
{
   mDetails = cSUCCESS_MESSAGE;
   mSuccessful = true;
   mSeverity = cNOTE;
}

//! Attempt to parse messages out of an input stream
//! NOTE: This must change when ScenarioAnalyzerMessage::Write format changes!!
std::vector<ScenarioAnalyzerMessage> ScenarioAnalyzerMessage::ParseFromIStream(std::istream& aInputStream)
{
   std::vector<ScenarioAnalyzerMessage> messages;
   std::string inputLine;
   while (std::getline(aInputStream, inputLine))
   {
      if (inputLine.find(cSCENARIO_ANALYZER) != std::string::npos)
      {
         std::stringstream ss(inputLine);
         std::string tmp, iSeverity, iDetails, iSuite, iCheck;

         ss >> tmp;            // Read 'SCENARIO_ANALYZER'
         ss >> iSeverity;      // Read severity
         ss >> tmp;            // Read ":"

         // Get details (looping, because it could be more than one word)
         while (std::getline(ss, tmp, ' '))
         {
            if (tmp.empty()) { continue; }
            if (tmp.find('[') != std::string::npos) { break; }
            iDetails += tmp + " ";
         }

         // Get suite name (looping, because it could be more than one word)
         while (std::getline(ss, tmp, ' '))
         {
            if (tmp.empty()) { continue; }
            if (tmp.find(':') != std::string::npos) { break; }
            iSuite += tmp + " ";
         }

         // Get check name (looping, because it could be more than one word)
         while (std::getline(ss, tmp, ' '))
         {
            if (tmp.empty()) { continue; }
            if (tmp.find(']') != std::string::npos) { break; }
            iCheck += tmp + " ";
         }

         ScenarioAnalyzerMessage message;
         message.SetSeverity(SeverityLevelFromString(iSeverity));
         message.SetSuiteName(iSuite.substr(0, iSuite.size() - 1));   // remove extra " "
         message.SetCheckName(iCheck.substr(0, iCheck.size() - 1));   // remove extra " "
         message.SetDetails(iDetails.substr(0, iDetails.size() - 1)); // remove extra " "

         if (message.GetDetails() == cSUCCESS_MESSAGE)
         {
            message.SetSuccessful();
         }

         // Parse locations- if none are appended, we should stop
         // reading at this location, since ss will be empty.
         ss >> tmp;      // Read [Locations:
         while (std::getline(ss, tmp, ' '))
         {
            if (tmp.empty()) { continue; }
            if (tmp.find(']') != std::string::npos) { break; }

            // tmp should be name:type
            size_t split = tmp.find(':');

            ScenarioFileLocation location("", "");
            location.mName = tmp.substr(0, split);
            location.mType = tmp.substr(split + 1, tmp.size());
            message.AddScenarioFileLocation(location);
         }

         messages.push_back(message);
      }
   }
   return messages;
}

//! Get the severity level as a string.
//static
std::string ScenarioAnalyzerMessage::SeverityString(SeverityLevel aSeverityLevel)
{
   switch (aSeverityLevel)
   {
      case cNOTE:
         return "NOTE";
      case cWARNING:
         return "WARNING";
      case cERROR:
         return "ERROR";
      default:
         return "";
   }
}

//! Get the severity level from a string.
//static
ScenarioAnalyzerMessage::SeverityLevel ScenarioAnalyzerMessage::SeverityLevelFromString(std::string aString)
{
   for (auto& c : aString) { c = toupper(c); }

   if (aString == "NOTE")
   {
      return cNOTE;
   }
   if (aString == "WARNING")
   {
      return cWARNING;
   }
   if (aString == "ERROR")
   {
      return cERROR;
   }

   throw WsfException("ERROR: Cannot convert " + aString + " to a ScenarioAnalyzerMessage SeverityLevel type.");
}

//! Write the message to an output stream. Standard output stream is used
//! if no output stream is specified by the caller.
//! NOTE: ScenarioAnalyzerMessage::ParseFromIStream must change when this format changes!!
void ScenarioAnalyzerMessage::Write(std::ostream& aOut) const
{
   aOut << cSCENARIO_ANALYZER << " " << SeverityString(mSeverity)
        << " : " << mDetails
        << " [ " << mSuiteName
        << " : " << mCheckName
        << " ] ";

   if (!mLocations.empty())
   {
      aOut << "[Locations: ";
      auto printVector = [&aOut](const ScenarioFileLocation & aLocation)
      {
         aOut << " " << aLocation.mName << ":" << aLocation.mType;
      };

      std::for_each(mLocations.begin(), mLocations.end(), printVector);
      aOut << " ]";
   }
   aOut << "\n";
}

// ****************************************************************************
//                        Scenario Analyzer Script Class
// ****************************************************************************
ScriptScenarioAnalyzerMessageClass::ScriptScenarioAnalyzerMessageClass(UtScriptTypes* aTypesPtr)
   : UtScriptClass(ScenarioAnalyzerMessage::cSCRIPT_CLASS_NAME, aTypesPtr)
{
   SetClassName(ScenarioAnalyzerMessage::cSCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<Get>());
   AddMethod(ut::make_unique<GetSuiteName>());
   AddMethod(ut::make_unique<GetCheckName>());
   AddMethod(ut::make_unique<GetDetails>());
   AddMethod(ut::make_unique<GetSeverity>());
   AddMethod(ut::make_unique<GetRelatedFiles>());
}

UT_DEFINE_SCRIPT_METHOD(ScriptScenarioAnalyzerMessageClass, ScenarioAnalyzerMessage, Get, 0, "string", "")
{
   ScenarioAnalyzerMessage& message = *aObjectPtr;

   std::stringstream ss;
   message.Write(ss);
   aReturnVal.SetString(ss.str());
}

UT_DEFINE_SCRIPT_METHOD(ScriptScenarioAnalyzerMessageClass, ScenarioAnalyzerMessage, GetSuiteName, 0, "string", "")
{
   ScenarioAnalyzerMessage& message = *aObjectPtr;
   aReturnVal.SetString(message.GetSuiteName());
}

UT_DEFINE_SCRIPT_METHOD(ScriptScenarioAnalyzerMessageClass, ScenarioAnalyzerMessage, GetCheckName, 0, "string", "")
{
   ScenarioAnalyzerMessage& message = *aObjectPtr;
   aReturnVal.SetString(message.GetCheckName());
}

UT_DEFINE_SCRIPT_METHOD(ScriptScenarioAnalyzerMessageClass, ScenarioAnalyzerMessage, GetDetails, 0, "string", "")
{
   ScenarioAnalyzerMessage& message = *aObjectPtr;
   aReturnVal.SetString(message.GetDetails());
}

UT_DEFINE_SCRIPT_METHOD(ScriptScenarioAnalyzerMessageClass, ScenarioAnalyzerMessage, GetSeverity, 0, "string", "")
{
   ScenarioAnalyzerMessage& message = *aObjectPtr;
   aReturnVal.SetString(ScenarioAnalyzerMessage::SeverityString(message.GetSeverity()));
}

UT_DEFINE_SCRIPT_METHOD(ScriptScenarioAnalyzerMessageClass, ScenarioAnalyzerMessage, GetRelatedFiles, 0, "Array<string>", "")
{
   ScenarioAnalyzerMessage& message = *aObjectPtr;
   const std::vector<ScenarioFileLocation>& locations = message.GetScenarioFileLocations();

   auto tmpListPtr = new std::vector<UtScriptData>();
   for (const auto& location : locations)
   {
      std::stringstream ss;
      ss << location.mType << ":" << location.mName;
      tmpListPtr->push_back(UtScriptData(ss.str()));
   }
   aReturnVal.SetPointer(new UtScriptRef(tmpListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
