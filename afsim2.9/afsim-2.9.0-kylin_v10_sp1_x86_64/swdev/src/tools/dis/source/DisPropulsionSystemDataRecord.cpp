// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisPropulsionSystemDataRecord.hpp"

#include "DisValidationUtils.hpp"

DisPropulsionSystemDataRecord::DisPropulsionSystemDataRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisPropulsionSystemDataRecord::DisPropulsionSystemDataRecord(const DisPropulsionSystemDataRecord& aSrc)
   : mPowerSettings(aSrc.mPowerSettings)
   , mEngineRpm(aSrc.mEngineRpm)
{
}

DisPropulsionSystemDataRecord* DisPropulsionSystemDataRecord::Clone() const
{
   return new DisPropulsionSystemDataRecord(*this);
}

DisFloat32 DisPropulsionSystemDataRecord::GetPowerSettings() const
{
   return mPowerSettings;
}

DisFloat32 DisPropulsionSystemDataRecord::GetEngineRpm() const
{
   return mEngineRpm;
}

void DisPropulsionSystemDataRecord::SetPowerSettings(DisFloat32 aPowerSettingsValue)
{
   mPowerSettings = aPowerSettingsValue;
}

void DisPropulsionSystemDataRecord::SetEngineRpm(DisFloat32 aEngineRpmValue)
{
   mEngineRpm = aEngineRpmValue;
}

void DisPropulsionSystemDataRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisPropulsionSystemDataRecord::Put(GenO& aGenO)
{
   aGenO << mPowerSettings;
   aGenO << mEngineRpm;
}

void DisPropulsionSystemDataRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mPowerSettings;
   aGenI >> mEngineRpm;
}

DisUint16 DisPropulsionSystemDataRecord::GetLength()
{
   return cLENGTH;
}

std::string DisPropulsionSystemDataRecord::ToString() const
{
   std::string returnString = "   Engine Rpm     = " + std::to_string(mEngineRpm) + "\n";
   returnString += "   Power Settings = " + std::to_string(mPowerSettings) + "\n";
   return returnString;
}

void DisPropulsionSystemDataRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

bool DisPropulsionSystemDataRecord::IsValid() const
{
   return ValidateScalar(mPowerSettings) && ValidateScalar(mEngineRpm);
}
