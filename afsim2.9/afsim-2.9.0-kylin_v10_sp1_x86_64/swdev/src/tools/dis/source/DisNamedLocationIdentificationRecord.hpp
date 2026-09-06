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

#ifndef DISNAMEDLOCATIONIDENTIFICATIONRECORD_HPP
#define DISNAMEDLOCATIONIDENTIFICATIONRECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Information about the discrete positional relationship of the part entity with respect
 * to the its host entity shall be specified by a Named Location Identification record.
 */
class DIS_EXPORT DisNamedLocationIdentificationRecord
{
public:
   DisNamedLocationIdentificationRecord() = default;
   DisNamedLocationIdentificationRecord(DisEnum16 aStationName, DisUint16 aStationNumber);
   DisNamedLocationIdentificationRecord(GenI& aGenI);
   DisNamedLocationIdentificationRecord(const DisNamedLocationIdentificationRecord& aSrc);
   ~DisNamedLocationIdentificationRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   DisNamedLocationIdentificationRecord* Clone() const;

   // Logging
   void        Stream(std::ostream& aStream);
   std::string ToString() const;

   // Getters
   DisEnum16 GetStationName() const;
   DisUint16 GetStationNumber() const;

   // Setters
   void SetStationName(DisEnum16 aStationName);
   void SetStationNumber(DisUint16 aStationNumber);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisNamedLocationIdentificationRecord& aRhs) const;
   bool operator!=(const DisNamedLocationIdentificationRecord& aRhs) const;
   bool operator<(const DisNamedLocationIdentificationRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! The signed magnitude value of the entity's absolute velocity vector. Negative values indicate that
   DisEnum16 mStationName = 0;

   //! Specifies the number of the particular wing station, cargo hold, and so on, at which the part is attached to
   DisUint16 mStationNumber = 0;

   //! Size of local record in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 4;
};
#endif
