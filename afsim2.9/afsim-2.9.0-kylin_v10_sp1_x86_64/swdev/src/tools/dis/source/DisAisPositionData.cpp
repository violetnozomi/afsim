// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2016 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisAisPositionData.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisAisPositionData::DisAisPositionData()
   : navStatus(AT_ANCHOR)
   , latitude_deg(0.0)
   , longitude_deg(0.0)
   , mmsi(0)
   , speed_kts(0.0)
   , course_deg(0.0)
   , turnRate_deg_sec(0.0)
{
}

void DisAisPositionData::Get(GenI& aGenI)
{
   DisUint8 pad8;

   aGenI >> navStatus;
   aGenI >> pad8;
   aGenI >> latitude_deg;
   aGenI >> longitude_deg;
   aGenI >> mmsi;
   aGenI >> speed_kts;
   aGenI >> course_deg;
   aGenI >> turnRate_deg_sec;
}

void DisAisPositionData::Put(GenO& aGenO) const
{
   DisUint8 pad8(0);

   aGenO << GetRecordType();
   aGenO << GetRecordLength();
   aGenO << navStatus;
   aGenO << pad8;
   aGenO << latitude_deg;
   aGenO << longitude_deg;
   aGenO << mmsi;
   aGenO << speed_kts;
   aGenO << course_deg;
   aGenO << turnRate_deg_sec;
}

bool DisAisPositionData::IsValid() const
{
   return ValidateScalar(latitude_deg) && ValidateScalar(longitude_deg) && ValidateScalar(speed_kts) &&
          ValidateScalar(course_deg) && ValidateScalar(turnRate_deg_sec);
}
