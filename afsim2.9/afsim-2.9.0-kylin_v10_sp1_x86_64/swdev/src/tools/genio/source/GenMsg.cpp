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

#include "GenMsg.hpp"

#include <iomanip>
#include <iostream>

#include "GenI.hpp"
#include "GenO.hpp"
#include "GenTime.hpp"
#include "UtLog.hpp"

const int GenMsg::baseLengthS = 24;

GenMsg::GenMsg()
   : timeM(0.0)
   , lengthInHeaderM(0)
   , familyVersionM(1)
   , familyTypeM(0)
   , versionM(0)
   , typeM(0)
{
}

GenMsg::GenMsg(short familyTypeA, short typeA, short versionA)
   : timeM(0.0)
   , lengthInHeaderM(0)
   , familyVersionM(1)
   , familyTypeM(familyTypeA)
   , versionM(versionA)
   , typeM(typeA)
{
}

GenMsg::GenMsg(double timeA, short familyTypeA, short typeA, short versionA)
   : timeM(timeA)
   , lengthInHeaderM(0)
   , familyVersionM(1)
   , familyTypeM(familyTypeA)
   , versionM(versionA)
   , typeM(typeA)
{
}


GenMsg::GenMsg(GenI& aGenI)
{
   GenMsg::GetGenMsgData(aGenI);
}

GenMsg::~GenMsg() {}

// Static
int GenMsg::GetBaseLength()
{
   return baseLengthS;
}

// Returns the name of the class
// It is highly recommended that users deriving from this class implement
// their own version of this function that would return the correct name.
std::string GenMsg::GetClassName() const
{
   static std::string defaultName("GenMsg");
   return defaultName;
}

// Virtual
int GenMsg::DoIt()
{
   return 0;
}

// Virtual
unsigned int GenMsg::IsValid()
{
   // Return a non zero if valid
   return 1;
}


// Virtual
void GenMsg::HandleReadError(ut::log::MessageStream& aStream)
{
   aStream.AddNote() << "Wall Clock: " << GenTime::GetWallClockTime();
   aStream << std::setw(12);
   aStream.AddNote() << "Time: " << GetTime();
   aStream.AddNote() << "Length: " << GetLengthInHeader();
   aStream.AddNote() << "Version: " << GetVersion();
   aStream.AddNote() << "Type: " << GetType();
}

void GenMsg::Get(GenI& genIA)
{
   GetGenMsgData(genIA);
   GetClassData(genIA);
   CheckMismatchedSize(genIA);
}

void GenMsg::CheckMismatchedSize(GenI& genIA)
{
   // Check that length declared in the header is equal to
   // requirement for the PDU.  If size is mismatched,
   // the status is set.
   if (lengthInHeaderM != GetLength())
   {
      auto out = ut::log::error() << "Mismatched size in GenIO.";
      out.AddNote() << "Wall Clock: " << GenTime::GetWallClockTime();
      out.AddNote() << "Class: " << GetClassName();
      out.AddNote() << "Interface: " << genIA.GetName();
      out.AddNote() << "Declared Length: " << lengthInHeaderM;
      out.AddNote() << "Actual Length: " << GetLength();
      genIA.SetInputStatus(GenBuf::MismatchedSize);
   }
}

void GenMsg::Put(GenO& genOA) const
{
   genOA << familyVersionM;
   genOA << familyTypeM;
   genOA << versionM;
   genOA << typeM;
   genOA << timeM;

   // Will use a local variable here just in case we want to
   // get rid of lengthM someday, because we don't really need it.
   int lengthL = GetLength();
   genOA << lengthL;

   int pad = 0;
   genOA << pad;

   PutClassData(genOA);
}

//
// Private
//

void GenMsg::GetGenMsgData(GenI& genIA)
{
   genIA >> familyVersionM;
   genIA >> familyTypeM;
   genIA >> versionM;
   genIA >> typeM;
   genIA >> timeM;
   genIA >> lengthInHeaderM;

   int pad;
   genIA >> pad;
}

int GenMsg::GetLength() const
{
   lengthM = baseLengthS + GetClassLength();
   return lengthM;
}

// Virtual
int GenMsg::GetClassLength() const
{
   // Derived classes will typically override this function
   return 0;
}

// Virtual
void GenMsg::GetClassData(GenI& /* genIA */)
{
   // Derived classes will typically override this function
}

// Virtual
void GenMsg::PutClassData(GenO& /* genOA */) const
{
   // Derived classes will typically override this function
}
