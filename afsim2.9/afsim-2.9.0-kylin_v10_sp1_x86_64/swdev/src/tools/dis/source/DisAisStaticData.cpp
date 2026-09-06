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

#include "DisAisStaticData.hpp"

#include <algorithm>
#include <vector>

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

// Buffer sizes for text fields.
const int shipFlag_len    = 2;
const int callSign_len    = 7;
const int shipName_len    = 20;
const int bodyOfWater_len = 20;
const int destination_len = 20;

DisAisStaticData::DisAisStaticData()
   : shipFlag()
   , mmsi(0)
   , imo(0)
   , callSign()
   , vesselType(OTHER)
   , shipName()
   , length_m(0)
   , width_m(0)
   , draft_m(0.0)
   , bodyOfWater()
   , destination()
{
}

template<class Iter>
std::string trimTrailingNull(Iter begin, Iter end)
{
   return std::string(begin, find(begin, end, 0));
}

std::string getStr(GenI& aGenI, int length)
{
   std::vector<char> buffer(length, 0);
   aGenI.GetRaw(&buffer[0], length);
   return trimTrailingNull(buffer.begin(), buffer.end());
}

void DisAisStaticData::Get(GenI& aGenI)
{
   DisUint32 pad32;

   shipFlag = getStr(aGenI, shipFlag_len);
   aGenI >> mmsi;
   aGenI >> imo;
   callSign = getStr(aGenI, callSign_len);
   aGenI >> vesselType;
   shipName = getStr(aGenI, shipName_len);
   aGenI >> length_m;
   aGenI >> width_m;
   aGenI >> draft_m;
   bodyOfWater = getStr(aGenI, bodyOfWater_len);
   destination = getStr(aGenI, destination_len);
   aGenI >> pad32;
}

void putStr(GenO& aGenO, const std::string& str, int length)
{
   std::vector<char> buffer(length, 0);
   str.copy(&buffer[0], length); // limit number of characters to 'length'
   aGenO.PutRaw(&buffer[0], length);
}

void DisAisStaticData::Put(GenO& aGenO) const
{
   DisUint32 pad32(0);

   aGenO << GetRecordType();
   aGenO << GetRecordLength();
   putStr(aGenO, shipFlag, shipFlag_len);
   aGenO << mmsi;
   aGenO << imo;
   putStr(aGenO, callSign, callSign_len);
   aGenO << vesselType;
   putStr(aGenO, shipName, shipName_len);
   aGenO << length_m;
   aGenO << width_m;
   aGenO << draft_m;
   putStr(aGenO, bodyOfWater, bodyOfWater_len);
   putStr(aGenO, destination, destination_len);
   aGenO << pad32;
}

bool DisAisStaticData::IsValid() const
{
   return !ValidateScalar(draft_m);
}
