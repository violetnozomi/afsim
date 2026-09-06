//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef DISPROPULSIONSYSTEMDATARECORD_HPP
#define DISPROPULSIONSYSTEMDATARECORD_HPP

#include "DisRecordHeader.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**
 * Record specifying propulsion system data
 */
class DIS_EXPORT DisPropulsionSystemDataRecord
{
public:
   DisPropulsionSystemDataRecord() = default;
   DisPropulsionSystemDataRecord(GenI& aGenI);
   DisPropulsionSystemDataRecord(const DisPropulsionSystemDataRecord& aSrc);
   virtual ~DisPropulsionSystemDataRecord() = default;

   DisPropulsionSystemDataRecord* Clone() const;

   // Getters
   DisFloat32 GetPowerSettings() const;
   DisFloat32 GetEngineRpm() const;
   DisUint16  GetLength();
   // Setters

   void SetPowerSettings(DisFloat32 aPowerSettingsValue);
   void SetEngineRpm(DisFloat32 aEngineRpmValue);

   std::string ToString() const;
   void        Stream(std::ostream& aStream) const;

   // Input / Output
   virtual void Get(GenI& aGenI);
   virtual void Put(GenO& aGenO);

   bool IsValid() const;

private:
   void GetMemberData(GenI& aGenI);

   //! Power setting of the propulsion system
   DisFloat32 mPowerSettings = 0;

   //! Engine speed in RPM
   DisFloat32 mEngineRpm = 0;
   DisUint16  cLENGTH    = 8;
};
#endif
