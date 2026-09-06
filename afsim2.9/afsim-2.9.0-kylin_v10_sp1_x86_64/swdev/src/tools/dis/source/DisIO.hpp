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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

// Stream insertion/extraction operators for several DIS types.

#ifndef DISIO_HPP
#define DISIO_HPP

#include "dis_export.h"

#include <iosfwd>
class DisEntityId;
class DisEntityType;
class DisEventId;
class DisRadioId;
class DisPdu;
class DisEntityState;
class DisFire;
class DisDetonation;
class DisTrackJam;
class DisBeam;
class DisSystem;
class DisEmission;
class DisTransmitter;
class DisReceiver;
class DisClockTime;
class DisSignal;
class DisAttribute;
class DisAttributeRecordSet;
class DisRecordHeader;
class DisFalseTargetsAttributeRecord;
class DisIOAction;
class DisIOCommNodeRecord;
class DisIOCommunicationsNodeIdRecord;
class DisIOEffectRecord;
class DisIOReport;

DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisEntityId& aId);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisEntityType& aType);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisEventId& aId);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisRadioId& aId);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisPdu& aPdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisEntityState& aEntityStatePdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisFire& aFirePdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisDetonation& aDetonationPdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisTrackJam& aTrackJam);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisBeam& aBeam);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisSystem& aSystem);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisEmission& aEmissionPdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisTransmitter& aTransmitterPdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisReceiver& aReceiverPdu);
DIS_EXPORT std::istream& operator>>(std::istream& aIn, DisRadioId& aId);
DIS_EXPORT std::istream& operator>>(std::istream& aIn, DisEntityType& aId);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisClockTime& atime);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisSignal& aSignalPdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisAttribute& aAttributePdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisAttributeRecordSet& aRecords);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisRecordHeader& aRecordHeader);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisFalseTargetsAttributeRecord& aRecord);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisIOAction& aIOActionPdu);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisIOCommNodeRecord& aIOCommNodeRecord);
DIS_EXPORT std::ostream& operator<<(std::ostream&                          aOut,
                                    const DisIOCommunicationsNodeIdRecord& aIOCommunicationNodeIdRecord);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisIOEffectRecord& aIOEffectRecord);
DIS_EXPORT std::ostream& operator<<(std::ostream& aOut, const DisIOReport& aIOReport);

class DIS_EXPORT DisIO
{
public:
   // Function will skip over newlines, tabs, blanks, and commas to get to
   // the DisEntityId
   // Returns a positive number if successful
   // Returns a zero if nothing on stream to read
   // Returns a negative number if there is an error
   static int ReadDisEntityId(std::istream& aIn, DisEntityId& aId);
};

#endif
