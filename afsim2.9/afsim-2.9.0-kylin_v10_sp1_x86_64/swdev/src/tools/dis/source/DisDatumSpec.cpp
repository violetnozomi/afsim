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

#include "DisDatumSpec.hpp"

#include "DisFixedDatum.hpp"
#include "DisVariableDatum.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDatumSpec::DisDatumSpec() {}

DisDatumSpec::~DisDatumSpec()
{
   ClearAll();
}

void DisDatumSpec::ClearAll()
{
   ClearFixedDatums();
   ClearVariableDatums();
}

void DisDatumSpec::ClearFixedDatums()
{
   mFixedDatumVec.clear();
}

void DisDatumSpec::ClearVariableDatums()
{
   mVariableDatumVec.clear();
}

void DisDatumSpec::Get(GenI& aGenI)
{
   DisUint32 numFixedDatums;
   aGenI >> numFixedDatums;

   DisUint32 numVariableDatums;
   aGenI >> numVariableDatums;

   mFixedDatumVec.resize(numFixedDatums);
   for (unsigned int j = 0; j < numFixedDatums; ++j)
   {
      aGenI >> mFixedDatumVec[j];
   }

   mVariableDatumVec.resize(numVariableDatums);
   for (unsigned int j = 0; j < numVariableDatums; ++j)
   {
      aGenI >> mVariableDatumVec[j];
   }
}

void DisDatumSpec::Put(GenO& aGenO) const
{
   DisUint32 numFixedDatums = GetNumFixedDatums();
   aGenO << numFixedDatums;

   DisUint32 numVariableDatums = GetNumVariableDatums();
   aGenO << numVariableDatums;

   unsigned int j;
   for (j = 0; j < numFixedDatums; ++j)
   {
      aGenO << mFixedDatumVec[j];
   }

   for (j = 0; j < numVariableDatums; ++j)
   {
      aGenO << mVariableDatumVec[j];
   }
}

DisUint16 DisDatumSpec::GetLength() const
{
   DisUint16 byteCount = 8;

   std::vector<DisFixedDatum>::const_iterator iterFixed(mFixedDatumVec.begin());
   while (iterFixed != mFixedDatumVec.end())
   {
      byteCount += iterFixed->GetLength();
      ++iterFixed;
   }

   std::vector<DisVariableDatum>::const_iterator iterVariable(mVariableDatumVec.begin());
   while (iterVariable != mVariableDatumVec.end())
   {
      byteCount += iterVariable->GetLength();
      ++iterVariable;
   }
   return byteCount;
}

void DisDatumSpec::Stream(std::ostream& aStream) const
{
   aStream << "         => NumberFixedDatums = " << GetNumFixedDatums() << std::endl;
   for (unsigned int i = 0; i != GetNumFixedDatums(); ++i)
   {
      mFixedDatumVec[i].Stream(aStream);
   }
   aStream << "         => NumberVariableDatums = " << GetNumVariableDatums() << std::endl;
   for (unsigned int j = 0; j != GetNumVariableDatums(); ++j)
   {
      mVariableDatumVec[j].Stream(aStream);
   }
}

bool DisDatumSpec::IsValid() const
{
   bool isValid = true;

   std::vector<DisFixedDatum>::const_iterator fixedIter;
   std::vector<DisFixedDatum>::const_iterator endFixedIter = mFixedDatumVec.end();
   for (fixedIter = mFixedDatumVec.begin(); fixedIter != endFixedIter && isValid; ++fixedIter)
   {
      isValid = (*fixedIter).IsValid();
   }

   std::vector<DisVariableDatum>::const_iterator variableIter;
   std::vector<DisVariableDatum>::const_iterator endVariableIter = mVariableDatumVec.end();
   for (variableIter = mVariableDatumVec.begin(); variableIter != endVariableIter && isValid; ++variableIter)
   {
      isValid = (*variableIter).IsValid();
   }

   // Perform data validation.
   return isValid;
}
