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

#include "PakSerializeTypes.hpp"

#include "PakSerializeFwd.hpp"
// Keep as last include
#include "PakSerializeImpl.hpp"
#include "UtLog.hpp"

namespace PakSerialization
{
// std::string
void Save(PakO& aAr, std::string& aValue)
{
   uint32_t length = 0;
   if (aValue.length())
   {
      length = static_cast<uint32_t>(aValue.length());
      aAr& length;
      aAr& RawData((void*)aValue.data(), (int)length);
   }
   else
   {
      aAr& length;
   }
}

void Load(PakI& aAr, std::string& aValue)
{
   uint32_t length = 0;
   aValue.clear();
   aAr& length;
   if (length > 0)
   {
      static const int cBUFF_SIZE = 1024;
      char             buffer[cBUFF_SIZE];
      while (length > 0)
      {
         uint32_t readLength = cBUFF_SIZE;
         if (length < readLength)
         {
            readLength = length;
         }
         aAr& RawData(buffer, readLength);
         aValue.insert(aValue.end(), buffer, buffer + readLength);
         length -= readLength;
      }
   }
}


// GenMemIO
void Load(PakI& aAr, GenBuffer& aMemIO)
{
   int32_t bufferSize;
   aAr&    bufferSize;
   if (bufferSize > 0)
   {
      char* bufferPtr = new char[bufferSize];
      aAr.SerializeBuffer(bufferPtr, bufferSize);
      aMemIO.SetBuffer((char*)bufferPtr, bufferSize, true);
      aMemIO.SetPutPos(bufferSize);
   }
}

void Save(PakO& aAr, GenBuffer& aMemIO)
{
   int32_t bufferSize = static_cast<int32_t>(aMemIO.GetPutPos());
   aAr&    bufferSize;
   if (bufferSize > 0)
   {
      const char* bufferPtr = aMemIO.GetBuffer();
      aAr.SerializeBuffer((char*)bufferPtr, bufferSize);
   }
}

// Responsible for serializing log messages inside of a PakProcessor
void Save(PakO& aAr, ut::log::Message& aValue)
{
   // Types
   aAr << aValue.mTypes.size();
   for (const auto& type : aValue.mTypes)
   {
      aAr << type;
   }

   // Body
   aAr << aValue.mData;

   // Notes
   aAr << aValue.mNotes;
}

// Responsible for serializing log messages inside of a PakProcessor
void Load(PakI& aAr, ut::log::Message& aValue)
{
   // Types
   size_t typeCount = 0;
   aValue.mTypes.clear();
   aAr >> typeCount;

   for (size_t i = 0; i < typeCount; i++)
   {
      std::string type;
      aAr >> type;
      aValue.mTypes.Insert(std::move(type));
   }

   // Body
   aAr >> aValue.mData;

   // Notes
   aAr >> aValue.mNotes;
}
} // namespace PakSerialization
