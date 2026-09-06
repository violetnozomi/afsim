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

// Stream insertion/extraction operators for DIS Messages.
#include "DisIO.hpp"

#include <cctype>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include "Dis.hpp"
#include "DisAttribute.hpp"
#include "DisAttributeRecordSet.hpp"
#include "DisBeam.hpp"
#include "DisBeamEnums.hpp"
#include "DisClockTime.hpp"
#include "DisDetonation.hpp"
#include "DisEmission.hpp"
#include "DisEmissionEnums.hpp"
#include "DisEntityId.hpp"
#include "DisEntityState.hpp"
#include "DisEntityStateEnums.hpp"
#include "DisEntityType.hpp"
#include "DisEventId.hpp"
#include "DisFalseTargetsAttributeRecord.hpp"
#include "DisFire.hpp"
#include "DisIOAction.hpp"
#include "DisIOBaseRecord.hpp"
#include "DisIOCommNodeRecord.hpp"
#include "DisIOCommunicationNodeIdRecord.hpp"
#include "DisIOEffectRecord.hpp"
#include "DisIOReport.hpp"
#include "DisJammingTechniqueEnums.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisRadioId.hpp"
#include "DisReceiver.hpp"
#include "DisRecordHeader.hpp"
#include "DisSignal.hpp"
#include "DisSystem.hpp"
#include "DisSystemEnums.hpp"
#include "DisTime.hpp"
#include "DisTrackJam.hpp"
#include "DisTransmitter.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

std::ostream& operator<<(std::ostream& aOut, const DisEntityId& aId)
{
   if (aOut.good())
   {
      aId.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisEntityType& aType)
{
   if (aOut.good())
   {
      aType.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisEventId& aId)
{
   if (aOut.good())
   {
      aId.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisRadioId& aRadio)
{
   if (aOut.good())
   {
      aRadio.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisPdu& aPdu)
{
   using namespace DisEnum::Pdu;

   if (aOut.good())
   {
      aPdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisEntityState& aEntityStatePdu)
{
   if (aOut.good())
   {
      aEntityStatePdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisFire& aFirePdu)
{
   if (aOut.good())
   {
      aFirePdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisDetonation& aDetonationPdu)
{
   if (aOut.good())
   {
      aDetonationPdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisTrackJam& aTrackJam)
{
   if (aOut.good())
   {
      aTrackJam.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisBeam& aBeam)
{
   if (aOut.good())
   {
      aBeam.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisSystem& aSystem)
{
   if (aOut.good())
   {
      aSystem.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisEmission& aEmissionPdu)
{
   if (aOut.good())
   {
      aEmissionPdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisTransmitter& aTransmitterPdu)
{
   if (aOut.good())
   {
      aTransmitterPdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisReceiver& aReceiverPdu)
{
   if (aOut.good())
   {
      aReceiverPdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisSignal& aSignalPdu)
{
   if (aOut.good())
   {
      aSignalPdu.Stream(aOut);
   }
   return aOut;
}

std::istream& operator>>(std::istream& aIn, DisRadioId& aId)
{
   if (aIn.good())
   {
      char      tmp;
      DisUint16 id[4];
      aIn >> id[0] >> tmp >> id[1] >> tmp >> id[2] >> tmp >> id[3];
      aId.Set(id[0], id[1], id[2], id[3]);
      if (aIn.fail() && Dis::ShowError(Dis::cIO_ERRORS))
      {
         ut::log::error() << "DisIO::operator>>(istream&, DisRadioId&) stream failed.";
      }
   }
   else if (Dis::ShowError(Dis::cIO_ERRORS))
   {
      ut::log::error() << "DisIO::operator>>(istream&, DisRadioId&) stream not good.";
   }
   return aIn;
}

std::istream& operator>>(std::istream& aIn, DisEntityType& aId)
{
   if (aIn.good())
   {
      std::string temp;
      aIn >> temp;
      int kind        = 0;
      int domain      = 0;
      int country     = 0;
      int category    = 0;
      int subcategory = 0;
      int specific    = 0;
      int extra       = 0;
      sscanf(temp.c_str(), "%d:%d:%d:%d:%d:%d:%d", &kind, &domain, &country, &category, &subcategory, &specific, &extra);
      aId.SetEntityKind(static_cast<DisUint8>(kind));
      aId.SetDomain(static_cast<DisUint8>(domain));
      aId.SetCountry(static_cast<DisUint16>(country));
      aId.SetCategory(static_cast<DisUint8>(category));
      aId.SetSubcategory(static_cast<DisUint8>(subcategory));
      aId.SetSpecific(static_cast<DisUint8>(specific));
      aId.SetExtra(static_cast<DisUint8>(extra));
   }

   return aIn;
}

std::ostream& operator<<(std::ostream& aOut, const DisClockTime& aTime)
{
   if (aOut)
   {
      aTime.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisAttribute& aAttributePdu)
{
   if (aOut)
   {
      aAttributePdu.Stream(aOut);
   }
   return aOut;
}


std::ostream& operator<<(std::ostream& aOut, const DisAttributeRecordSet& aRecords)
{
   if (aOut.good())
   {
      aRecords.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisRecordHeader& aRecordHeader)
{
   if (aOut.good())
   {
      aRecordHeader.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisFalseTargetsAttributeRecord& aRecord)
{
   if (aOut.good())
   {
      aRecord.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisIOAction& aIOActionPdu)
{
   if (aOut.good())
   {
      aIOActionPdu.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisIOCommNodeRecord& aIOCommNodeRecord)
{
   if (aOut.good())
   {
      aIOCommNodeRecord.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisIOCommunicationsNodeIdRecord& aIOCommunicationNodeIdRecord)
{
   if (aOut.good())
   {
      aIOCommunicationNodeIdRecord.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisIOEffectRecord& aIOEffectRecord)
{
   if (aOut.good())
   {
      aIOEffectRecord.Stream(aOut);
   }
   return aOut;
}

std::ostream& operator<<(std::ostream& aOut, const DisIOReport& aIOReport)
{
   if (aOut.good())
   {
      aIOReport.Stream(aOut);
   }
   return aOut;
}

// Static
// Function will skip over newlines, tabs, blanks, and commas to get to
// the DisEntityId
// Returns a positive number if successful
// Returns a zero if nothing on stream to read
// Reatuns a negative number if there is an error
int DisIO::ReadDisEntityId(std::istream& aIn, DisEntityId& aId)
{
   if (aIn.good())
   {
      int numFound = 0;

      char abyte;
      aIn.get(abyte);

      while (aIn.good())
      {
         if (isdigit(abyte))
         {
            aIn.putback(abyte);
            DisUint16 tempy;
            aIn >> tempy;

            if (numFound == 0)
            {
               aId.SetSite(tempy);
               numFound = 1;
            }
            else if (numFound == 1)
            {
               aId.SetApplication(tempy);
               numFound = 2;
            }
            else if (numFound == 2)
            {
               aId.SetEntity(tempy);
               return 1;
            }
         }
         else if (numFound == 0)
         {
            if (abyte != '\n' && abyte != '\t' && abyte != ',' && abyte != ' ')
            {
               return -1;
            }
         }
         else if (abyte != ':')
         {
            return -1;
         }
         aIn.get(abyte);
      }
   }
   return 0;
}
