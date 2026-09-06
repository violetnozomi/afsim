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

#ifndef WSFEVENTUTILS_HPP
#define WSFEVENTUTILS_HPP

#include "wsf_export.h"

#include <map>
#include <ostream>

class UtAttributeContainer;
class UtEntity;

#include "WsfCovariance.hpp"
#include "WsfEM_Interaction.hpp"
class WsfEM_XmtrRcvr;
#include "WsfEventResult.hpp"
class WsfMessage;
class WsfPlatform;
class WsfPlatformPart;
class WsfSensor;
class WsfSensorResult;
class WsfSimulation;
class WsfTrack;

namespace wsf
{
namespace comm
{
class Comm;

namespace router
{
class Router;
}

} // namespace comm
} // namespace wsf

namespace wsf
{
namespace event
{

//! event_output utilities for printing of text based event results
namespace utils
{

WSF_EXPORT void PrintLLA(std::ostream& aStream, double aLat, double aLon, double aAlt, int aLatLonFormat);

WSF_EXPORT std::string ContinueChar(bool aPrintSingleLinePerEvent);

WSF_EXPORT void PrintTime(std::ostream& aStream, double aSimTime, int aTimeFormat);
WSF_EXPORT void PrintTime(std::ostream& aStream, WsfSimulation& aSimulation, int aTimeFormat);

WSF_EXPORT void PrintDateTime(std::ostream& aStream, const WsfSimulation& aSimulation);
WSF_EXPORT void PrintDateTime(std::ostream& aStream, double aSimTime, const WsfSimulation& aSimulation);

WSF_EXPORT std::string MakePrintableString(const std::string& aInput);

WSF_EXPORT double PrintableAngle(double aAngle);

WSF_EXPORT void PrintCommData(std::ostream& aStream, wsf::comm::Comm* aCommPtr, bool aShowAddress = true);

WSF_EXPORT void PrintRouterData(std::ostream& aStream, wsf::comm::router::Router* aRouterPtr);

WSF_EXPORT void PrintEM_Interaction(std::ostream& aStream, WsfEM_Interaction& aResult, const Settings& aSettings);

WSF_EXPORT void PrintEM_AbsTargetData(std::ostream&                          aStream,
                                      const WsfEM_Interaction&               aResult,
                                      const char*                            aPrefix,
                                      WsfPlatform*                           aTgtPtr,
                                      const WsfEM_Interaction::LocationData& aLocData,
                                      const Settings&                        aSettings);

WSF_EXPORT void PrintEM_RelTargetData(std::ostream&                          aStream,
                                      const WsfEM_Interaction&               aResult,
                                      const char*                            aPrefix,
                                      WsfPlatform*                           aTgtPtr,
                                      const WsfEM_Interaction::RelativeData& aRelData,
                                      bool                                   aPrintSingleLinePerEvent);

WSF_EXPORT void PrintEM_AbsXmtrRcvrData(std::ostream&                          aStream,
                                        const WsfEM_Interaction&               aResult,
                                        const char*                            aPrefix,
                                        WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                                        const WsfEM_Interaction::LocationData& aLocData,
                                        const Settings&                        aSettings);

WSF_EXPORT void PrintEM_RelXmtrRcvrData(std::ostream&                          aStream,
                                        const WsfEM_Interaction&               aResult,
                                        const char*                            aPrefix,
                                        WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                                        const WsfEM_Interaction::RelativeData& aRelData,
                                        bool                                   aPrintSingleLinePerEvent);

WSF_EXPORT void PrintEM_BeamData(std::ostream&                      aStream,
                                 const char*                        aPrefix,
                                 WsfEM_XmtrRcvr*                    aXmtrRcvrPtr,
                                 const WsfEM_Interaction::BeamData& aBeamData,
                                 bool                               aPrintSingleLinePerEvent);

WSF_EXPORT void PrintEM_LocationData(std::ostream&                          aStream,
                                     const char*                            aPrefix,
                                     WsfPlatform*                           aPlatformPtr,
                                     const WsfEM_Interaction::LocationData& aLocationData,
                                     const Settings&                        aSettings);

WSF_EXPORT void PrintEM_Range(std::ostream& aStream,
                              const char*   aPrefix,
                              double        aRange,
                              bool          aPrintSingleLinePerEvent,
                              bool          aWriteEndOfLine = true);

WSF_EXPORT void PrintLocationDataLLA(std::ostream& aStream, double aLat, double aLon, double aAlt, int aLatLonFormat);

WSF_EXPORT void PrintLocationData(std::ostream& aStream, UtEntity* aPlatformPtr, const Settings& aSettings);

WSF_EXPORT void PrintLocationDataECI(std::ostream& aStream, const double aLocationECI[3]);

WSF_EXPORT void PrintNonLocationData(std::ostream&      aStream,
                                     WsfPlatform*       aPlatformPtr,
                                     const std::string& aIndent,
                                     const Settings&    aSettings);

WSF_EXPORT void PrintMessageData(std::ostream&     aStream,
                                 double            aSimTime,
                                 const WsfMessage& aMessage,
                                 WsfSimulation&    aSimulation,
                                 const Settings&   aSettings);

WSF_EXPORT void PrintPlatformEvent(std::ostream& aStream, WsfPlatform* aPlatformPtr, bool aShowPs, const Settings& aSettings);

WSF_EXPORT void
PrintSensorResult(std::ostream& aStream, WsfSensor* aSensorPtr, WsfSensorResult& aResult, const Settings& aSettings);

WSF_EXPORT void PrintSystemOffOnEvent(std::ostream&      aStream,
                                      double             aSimTime,
                                      const std::string& aEvent,
                                      const std::string& aSystemType,
                                      WsfPlatformPart*   aSystemPtr,
                                      const Settings&    aSettings);

WSF_EXPORT void PrintAuxData(std::ostream& aStream, const UtAttributeContainer* aAuxDataPtr);

WSF_EXPORT void PrintTrackData(std::ostream&        aStream,
                               double               aSimTime,
                               const WsfTrack*      aTrackPtr,
                               const WsfSimulation& aSimulation,
                               const Settings&      aSettings);

WSF_EXPORT void PrintTrackCovarianceData(std::ostream&        aStream,
                                         double               aSimTime,
                                         const std::string&   aCovarianceType,
                                         const WsfCovariance* aCovariancePtr,
                                         const WsfTrack*      aTrackPtr,
                                         bool                 aPrintSingleLinePerEvent);

WSF_EXPORT void PrintTrackDataBrief(std::ostream& aStream, double aSimTime, const WsfTrack* aTrackPtr, int aTimeFormat);

WSF_EXPORT void PrintTrackTargetName(std::ostream& aStream, const WsfTrack* aTrackPtr, const WsfSimulation& aSimulation);

WSF_EXPORT void PrintEvent(std::ostream&      aStream,
                           double             aSimTime,
                           const std::string& aSubjectName,
                           const std::string& aEventName,
                           const std::string& aObjectName,
                           const Settings&    aSettings,
                           const std::string& aAdditionalInformation = "");

} // namespace utils

//! csv_event_output utilities for printing of CSV based event results
namespace utilsCSV
{

WSF_EXPORT void PrintTime(std::ostream& aStream, WsfSimulation& aSimulation);
WSF_EXPORT void PrintTime(std::ostream& aStream, double aSimTime);

WSF_EXPORT void PrintDateTime(std::ostream& aStream, const WsfSimulation& aSimulation);
WSF_EXPORT void PrintDateTime(std::ostream& aStream, double aSimTime, const WsfSimulation& aSimulation);

WSF_EXPORT void PrintLLA(std::ostream& aStream, double aLat, double aLon, double aAlt);

WSF_EXPORT std::string MakePrintableString(const std::string& aInput);

WSF_EXPORT void PrintEM_Interaction(std::ostream& aStream, WsfEM_Interaction& aResult);

WSF_EXPORT void PrintEM_AbsTargetData(std::ostream&                          aStream,
                                      const WsfEM_Interaction&               aResult,
                                      WsfPlatform*                           aTgtPtr,
                                      const WsfEM_Interaction::LocationData& aLocData);

WSF_EXPORT void PrintEM_RelTargetData(std::ostream&                          aStream,
                                      const WsfEM_Interaction&               aResult,
                                      WsfPlatform*                           aTgtPtr,
                                      const WsfEM_Interaction::RelativeData& aRelData);

WSF_EXPORT void PrintEM_AbsXmtrRcvrData(std::ostream&                          aStream,
                                        const WsfEM_Interaction&               aResult,
                                        WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                                        const WsfEM_Interaction::LocationData& aLocData);

WSF_EXPORT void PrintEM_RelXmtrRcvrData(std::ostream&                          aStream,
                                        const WsfEM_Interaction&               aResult,
                                        WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                                        const WsfEM_Interaction::RelativeData& aRelData);

WSF_EXPORT void PrintEM_BeamData(std::ostream&                      aStream,
                                 WsfEM_XmtrRcvr*                    aXmtrRcvrPtr,
                                 const WsfEM_Interaction::BeamData& aBeamData);

WSF_EXPORT void PrintEM_LocationData(std::ostream&                          aStream,
                                     WsfPlatform*                           aPlatformPtr,
                                     const WsfEM_Interaction::LocationData& aLocationData);

WSF_EXPORT void PrintEM_Range(std::ostream& aStream, double aRange, bool aWriteEndOfLine = true);

WSF_EXPORT void PrintLocationDataLLA(std::ostream& aStream, double aLat, double aLon, double aAlt);

WSF_EXPORT void PrintLocationData(std::ostream& aStream, UtEntity* aPlatformPtr);

WSF_EXPORT void PrintLocationDataECI(std::ostream& aStream, const double aLocationECI[3]);

WSF_EXPORT void PrintNonLocationData(std::ostream& aStream, WsfPlatform* aPlatformPtr, const std::string& aIndent);

WSF_EXPORT void PrintMessageData(std::ostream&             aStream,
                                 double                    aSimTime,
                                 const WsfMessage&         aMessage,
                                 WsfSimulation&            aSimulation,
                                 Settings::MessagePrintMap mMessagePrinters,
                                 bool                      aPrintTrackDataBrief);

WSF_EXPORT void PrintPlatformEvent(std::ostream& aStream, WsfPlatform* aPlatformPtr, bool aShowPs);

WSF_EXPORT void PrintSensorResult(std::ostream& aStream, WsfSensor* aSensorPtr, WsfSensorResult& aResult);

WSF_EXPORT void PrintSystemOffOnEvent(std::ostream&      aStream,
                                      double             aSimTime,
                                      const std::string& aEvent,
                                      const std::string& aSystemType,
                                      WsfPlatformPart*   aSystemPtr);

WSF_EXPORT void PrintAuxData(std::ostream& aStream, const UtAttributeContainer* aAuxDataPtr);

WSF_EXPORT void PrintTrackData(std::ostream&        aStream,
                               double               aSimTime,
                               const WsfTrack*      aTrackPtr,
                               const WsfSimulation& aSimulation,
                               bool                 aPrintTrackDataBrief = false);

WSF_EXPORT void PrintTrackCovarianceData(std::ostream&       aStream,
                                         double              aSimTime,
                                         const std::string&  aCovarianceType,
                                         const UtCovariance* aCovariancePtr,
                                         const WsfTrack*     aTrackPtr);

WSF_EXPORT void PrintTrackTargetName(std::ostream& aStream, const WsfTrack* aTrackPtr, WsfSimulation& aSimulation);

WSF_EXPORT void PrintEvent(std::ostream&      aStream,
                           double             aSimTime,
                           const std::string& aSubjectName,
                           const std::string& aEventName,
                           const std::string& aObjectName,
                           const std::string& aAdditionalInformation = "");

} // namespace utilsCSV

} // namespace event
} // namespace wsf

#endif
