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


#include "DisUASystem.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisUASystem::DisUASystem()
   : mDataLength(20)
   , // 160 bits / 8 bits = 20 bytes
   mBeamCount(0)
   , mAcousticName(0)
   , mFunction(0)
   , mAcousticId(0)
   , mLengthRead(0)
{
   mLocation[0] = 0.0F;
   mLocation[1] = 0.0F;
   mLocation[2] = 0.0F;
}

DisUASystem::~DisUASystem()
{
   // The destructor for "mBeamList" will take of itself and the
   // pointers it owns.
}

DisUint8 DisUASystem::GetDataLength()
{
   GetLength();
   return mDataLength;
}

DisUint16 DisUASystem::GetLength()
{
   // Update count
   UpdateBeamCount();

   DisUint16 octetCount = 20; // 160 bits

   std::vector<DisUABeam*>::const_iterator beamIter;
   std::vector<DisUABeam*>::const_iterator endBeamIter = GetUABeamEnd();

   for (beamIter = GetUABeamBegin(); beamIter != endBeamIter; ++beamIter)
   {
      octetCount += (*beamIter)->GetLength();
   }
   mDataLength = static_cast<DisUint8>(octetCount / 4); // length in 32 bit words
   return octetCount;
}

void DisUASystem::Get(GenI& aGenI)
{
   DisUint16 pad16;

   RemoveAllBeams();

   aGenI >> mDataLength;
   aGenI >> mBeamCount;
   aGenI >> pad16;
   aGenI >> mAcousticName;
   aGenI >> mFunction;
   aGenI >> mAcousticId;
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   mLengthRead = 20; // length of base
   if (mBeamCount != 0)
   {
      mBeamList.Reserve(mBeamCount);
      for (int i = 0; i < mBeamCount; ++i)
      {
         DisUABeam* beam = new DisUABeam();
         aGenI >> *beam;
         mBeamList.Add(beam); // List takes ownership
         mLengthRead += beam->GetLengthRead();
      }
   }

   // Read and ignore any 'extra' data...

   int extraOctets = (mDataLength * 4) - mLengthRead;
   mLengthRead += static_cast<DisUint16>(extraOctets);
   while (extraOctets > 0)
   {
      DisUint8 tempOctet;
      aGenI >> tempOctet;
      --extraOctets;
   }
}

void DisUASystem::Put(GenO& aGenO)
{
   DisUint16 pad16(0);

   // mDataLength and mBeamCount *should* be accurate because GetLength()
   // *should* have been called determine the length for outputting
   // the Emission PDU.

   aGenO << mDataLength;
   aGenO << mBeamCount;
   aGenO << pad16;
   aGenO << mAcousticName;
   aGenO << mFunction;
   aGenO << mAcousticId;
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];

   std::vector<DisUABeam*>::const_iterator beamIter;
   std::vector<DisUABeam*>::const_iterator endBeamIter = GetUABeamEnd();

   for (beamIter = GetUABeamBegin(); beamIter != endBeamIter; ++beamIter)
   {
      aGenO << (**beamIter);
   }
}

bool DisUASystem::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mLocation[0]))
      isValid = false;
   if (!ValidateScalar(mLocation[1]))
      isValid = false;
   if (!ValidateScalar(mLocation[2]))
      isValid = false;

   std::vector<DisUABeam*>::const_iterator iter(mBeamList.GetBegin());
   while (iter != mBeamList.GetEnd())
   {
      if (!(*iter++)->IsValid())
         isValid = false;
   }

   return isValid;
}
