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

#ifndef DIS_DATA_QUERY_DATUM_SPEC_HPP
#define DIS_DATA_QUERY_DATUM_SPEC_HPP

#include "dis_export.h"

#include <vector>

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDataQueryDatumSpec
{
public:
   DisDataQueryDatumSpec();
   ~DisDataQueryDatumSpec();

   // Allow compiler to build copy constructor and operator=
   // DisDataQueryDatumSpec(const DisDataQueryDatumSpec& aSrc);
   // DisDataQueryDatumSpec& operator=(const DisDataQueryDatumSpec& aRhs);

   // Enumerations for Datum ID in the Datum Specification Record
   // (EBV-DOC 7.1)
   enum
   {
      UpiD_IDENTIFICATION       = 10000,
      UpiD_ENTITY_TYPE          = 11000,
      UpiD_WAYPOINT_TYPE        = 31010,
      UpiD_LATITUDE             = 31300,
      UpiD_LONGITUDE            = 31400,
      UpiD_ALTITUDE             = 31600,
      UpiD_VELOCITY_X           = 36100,
      UpiD_VELOCITY_Y           = 36200,
      UpiD_VELOCITY_Z           = 36300,
      UpiD_SPARE                = 41001,
      UpiD_MISSION_ID           = 42100,
      UpiD_MISSION_TYPE         = 42200,
      UpiD_ENTITIES             = 43200,
      UpiD_APPLICATION_RATE     = 45000,
      UpiD_APPLICATION_TIME     = 45005,
      UpiD_APPLICATION_TIMESTEP = 45010,
      UpiD_FEEDBACK_TIME        = 45020,
      UpiD_SIMULATION_RATE      = 45030,
      UpiD_SIMULATION_TIME      = 45040,
      UpiD_SIMULATION_TIMESTEP  = 45050,
      UpiD_TIME_LATENCY         = 45070,
      UpiD_TIME_SCHEME          = 45080,
      UpiD_TIME                 = 52000
   };

   // Accessors

   DisUint32 GetNumFixedRecords() const;
   DisUint32 GetNumVariableRecords() const;

   const std::vector<DisEnum32>& GetFixedDatums() const;
   const std::vector<DisEnum32>& GetVariableDatums() const;

   // Mutators
   // Datums can be added in two ways.  One way is by passing a vector of datums.
   // The second way is by passing one datum at a time.

   void AddFixedDatum(const DisEnum32& aFixedDatum);
   void AddVariableDatum(const DisEnum32& aVariableDatum);

   void ClearFixedDatums();
   void ClearVariableDatums();

   // Returns length of class in bytes
   DisUint16 GetLength() const;

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

private:
   std::vector<DisEnum32> mFixedDatumVec;
   std::vector<DisEnum32> mVariableDatumVec;
};

// Accessors

inline DisUint32 DisDataQueryDatumSpec::GetNumFixedRecords() const
{
   return static_cast<unsigned int>(mFixedDatumVec.size());
}

inline DisUint32 DisDataQueryDatumSpec::GetNumVariableRecords() const
{
   return static_cast<unsigned int>(mVariableDatumVec.size());
}

inline const std::vector<DisEnum32>& DisDataQueryDatumSpec::GetFixedDatums() const
{
   return mFixedDatumVec;
}

inline const std::vector<DisEnum32>& DisDataQueryDatumSpec::GetVariableDatums() const
{
   return mVariableDatumVec;
}

// Mutators

inline void DisDataQueryDatumSpec::AddFixedDatum(const DisEnum32& aFixedDatum)
{
   mFixedDatumVec.push_back(aFixedDatum);
}

inline void DisDataQueryDatumSpec::AddVariableDatum(const DisEnum32& aVariableDatum)
{
   mVariableDatumVec.push_back(aVariableDatum);
}

inline void DisDataQueryDatumSpec::ClearFixedDatums()
{
   mFixedDatumVec.clear();
}

inline void DisDataQueryDatumSpec::ClearVariableDatums()
{
   mVariableDatumVec.clear();
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisDataQueryDatumSpec& aDisDataQueryDatumSpec)
{
   aDisDataQueryDatumSpec.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisDataQueryDatumSpec& aDisDataQueryDatumSpec)
{
   aDisDataQueryDatumSpec.Get(aGenI);
   return aGenI;
}

#endif
