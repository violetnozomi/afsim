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

#include "DisDataQueryDatumSpec.hpp"

#include "GenI.hpp"
#include "GenO.hpp"
#include "UtCast.hpp"

DisDataQueryDatumSpec::DisDataQueryDatumSpec() {}

DisDataQueryDatumSpec::~DisDataQueryDatumSpec() {}

void DisDataQueryDatumSpec::Get(GenI& aGenI)
{
   DisUint32 numFixedRecords;
   aGenI >> numFixedRecords;

   DisUint32 numVariableRecords;
   aGenI >> numVariableRecords;

   mFixedDatumVec.resize(numFixedRecords);
   for (unsigned int j = 0; j < numFixedRecords; ++j)
   {
      aGenI >> mFixedDatumVec[j];
   }

   mVariableDatumVec.resize(numVariableRecords);
   for (unsigned int j = 0; j < numVariableRecords; ++j)
   {
      aGenI >> mVariableDatumVec[j];
   }
}

void DisDataQueryDatumSpec::Put(GenO& aGenO) const
{
   DisUint32 numFixedRecords = GetNumFixedRecords();
   aGenO << numFixedRecords;

   DisUint32 numVariableRecords = GetNumVariableRecords();
   aGenO << numVariableRecords;

   for (unsigned int j = 0; j < numFixedRecords; ++j)
   {
      aGenO << mFixedDatumVec[j];
   }

   for (unsigned int j = 0; j < numVariableRecords; ++j)
   {
      aGenO << mVariableDatumVec[j];
   }
}

DisUint16 DisDataQueryDatumSpec::GetLength() const
{
   return ut::safe_cast<DisUint16, size_t>(8U + 4U * (mFixedDatumVec.size() + mVariableDatumVec.size()));
}

bool DisDataQueryDatumSpec::IsValid() const
{
   return true;
}
