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


#include "DisFixedDatum.hpp"

#include "GenI.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenO.hpp"

DisFixedDatum::DisFixedDatum()
   : mId(0)
   , mDataBuf(new GenMemIO(GenBuf::BigEndian, 4))
{
}

DisFixedDatum::DisFixedDatum(DisEnum32 aId, const unsigned char aData[4])
   : mId(aId)
   , mDataBuf(new GenMemIO(GenBuf::BigEndian, 4))
{
   *mDataBuf << aData[0];
   *mDataBuf << aData[1];
   *mDataBuf << aData[2];
   *mDataBuf << aData[3];
}

DisFixedDatum::DisFixedDatum(const DisFixedDatum& aRhs)
   : mId(aRhs.mId)
   , mDataBuf(new GenMemIO(GenBuf::BigEndian, 4))
{
   for (unsigned int i = 0; i < 4; ++i)
   {
      *mDataBuf << aRhs.mDataBuf->GetBuffer()[i];
   }
}

DisFixedDatum::~DisFixedDatum()
{
   delete mDataBuf;
}

DisFixedDatum& DisFixedDatum::operator=(const DisFixedDatum& aRhs)
{
   mId = aRhs.mId;
   mDataBuf->Reset();
   for (unsigned int i = 0; i < 4; ++i)
   {
      *mDataBuf << aRhs.mDataBuf->GetBuffer()[i];
   }
   return *this;
}

void DisFixedDatum::Get(GenI& aGenI)
{
   aGenI >> mId;
   mDataBuf->Reset();
   unsigned char c = 0;
   for (unsigned int i = 0; i < 4; ++i)
   {
      aGenI >> c;
      *mDataBuf << c;
   }
}

void DisFixedDatum::Put(GenO& aGenO) const
{
   aGenO << mId;

   for (unsigned int i = 0; i < 4; ++i)
   {
      aGenO << mDataBuf->GetBuffer()[i];
   }
}

bool DisFixedDatum::IsValid() const
{
   return true;
}

void DisFixedDatum::Stream(std::ostream& aStream) const
{
   GenIConvertBigEndian converter;
   DisEnum32            id = GetId();
   unsigned char        buffer[4];
   GetData(buffer);
   float value;
   converter.GetFromBuf(buffer, value);
   double viewable = value;
   aStream << "            => DisUint32 = " << id << ", as float(?) = " << viewable << std::endl;
}
