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

#ifndef PAKGENI_ARCHIVE_HPP
#define PAKGENI_ARCHIVE_HPP

#include "packetio_export.h"

#include <string>

#include "GenI.hpp"

class PakGenO_Archive;

//! Serializes data from a GenI
class PACKETIO_EXPORT PakGenI_Archive
{
public:
   static const bool       cIS_OUTPUT = false;
   typedef PakGenO_Archive OutputArchive;
   typedef PakGenI_Archive InputArchive;

   PakGenI_Archive(GenI& aGenIPtr);

   GenI& GetGenI() const { return *mGenIPtr; }

   void IgnoreBytes(int aNumBytes);

   template<typename T>
   void Serialize(T& aVal)
   {
      mGenIPtr->Get(aVal);
   }

   void SerializeBuffer(char* aBuffer, int aSize);

   void SerializeString(std::string& aString, int aBytes);

protected:
   GenI* mGenIPtr;
};


#endif
