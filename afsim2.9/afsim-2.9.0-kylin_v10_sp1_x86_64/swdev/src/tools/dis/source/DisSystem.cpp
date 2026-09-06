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

#include "DisSystem.hpp"

#include "Dis.hpp"
#include "DisBaseSizes.hpp"
#include "DisEmission.hpp"
#include "DisSystemEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisSystem::DisSystem()
   : mReportedDataLength(0)
   , mReportedNumberOfBeams(0)
   , mName(0)
   , mFunction(0)
   , mNumber(0)
   , mLengthRead(0)
   , mParentEmission(nullptr)
{
   mLocation[0] = 0.0F;
   mLocation[1] = 0.0F;
   mLocation[2] = 0.0F;
}

DisSystem::DisSystem(const DisSystem& aSrc)
   : mReportedDataLength(aSrc.mReportedDataLength)
   , mReportedNumberOfBeams(aSrc.mReportedNumberOfBeams)
   , mName(aSrc.mName)
   , mFunction(aSrc.mFunction)
   , mNumber(aSrc.mNumber)
   , mBeamList(aSrc.mBeamList)
   , mLengthRead(aSrc.mLengthRead)
   , mParentEmission(nullptr) // Do NOT copy parent pointer!
{
   mLocation[0] = aSrc.mLocation[0];
   mLocation[1] = aSrc.mLocation[1];
   mLocation[2] = aSrc.mLocation[2];
}

DisUint8 DisSystem::GetDataLength() const
{
   // convert to 32-bit words
   DisUint16 length = GetLengthOctets() / 4;
   return length > 255 ? DisUint8(0) : DisUint8(length);
}

DisUint16 DisSystem::GetLengthOctets() const
{
   DisUint16 octetCount = static_cast<DisUint16>(cBASE_SYSTEM_SIZE);

   std::vector<DisBeam*>::const_iterator iter(mBeamList.GetBegin());
   while (iter != mBeamList.GetEnd())
   {
      octetCount += (*iter++)->GetLengthOctets();
   }
   return octetCount;
}

void DisSystem::Get(GenI& aGenI)
{
   DisUint16 pad16;

   RemoveAllBeams();

   aGenI >> mReportedDataLength;
   aGenI >> mReportedNumberOfBeams;
   aGenI >> pad16;
   aGenI >> mName;
   aGenI >> mFunction;
   aGenI >> mNumber;
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   mLengthRead = static_cast<DisUint16>(cBASE_SYSTEM_SIZE);
   if (mReportedNumberOfBeams != 0)
   {
      mBeamList.Reserve(mReportedNumberOfBeams);
      for (int i = 0; i < mReportedNumberOfBeams; ++i)
      {
         DisBeam* beam = new DisBeam();
         aGenI >> *beam;
         beam->SetParentSystem(this);
         mBeamList.Add(beam); // List takes ownership
         mLengthRead += beam->GetLengthRead();
      }
   }

   // Read and ignore any 'extra' data...

   if (mReportedDataLength > 0)
   {
      int extraOctets = (mReportedDataLength * 4) - mLengthRead;
      // Note: it is legal for mReportedDataLength to be 0 for large Systems.
      // So only adjust the amount we have read if the extra octets count is
      // positive.
      mLengthRead += extraOctets;
      while (extraOctets > 0)
      {
         DisUint8 tempOctet;
         aGenI >> tempOctet;
         --extraOctets;
      }
   }
}

void DisSystem::Put(GenO& aGenO) const
{
   DisUint16 pad16(0);

   aGenO << GetDataLength();
   aGenO << GetNumberOfBeams();
   aGenO << pad16;
   aGenO << mName;
   aGenO << mFunction;
   aGenO << mNumber;
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];

   for (auto iter = mBeamList.GetBegin(); iter != mBeamList.GetEnd(); iter++)
   {
      (**iter).Put(aGenO);
   }
}

void DisSystem::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisSystem::ToString() const
{
   using namespace std;
   std::ostringstream ss("");
   DisFloat32         locationX = 0.0;
   DisFloat32         locationY = 0.0;
   DisFloat32         locationZ = 0.0;

   GetLocation(locationX, locationY, locationZ);
   DisUint16 systemNumber = GetNumber();
   ss << "***** System " << systemNumber << " Data *****\n"
      << "System Data Length: " << static_cast<DisUint16>(GetDataLength());
   if (GetDataLength() > 0)
   {
      ss << " (32-bit words)\n";
   }
   else
   {
      ss << " (Large Emitter System)\n";
   }
   ss << "Number Of Beams:    " << static_cast<unsigned int>(GetNumberOfBeams()) << '\n';
   ss << "***** Emitter System Record *****\n"
      << "Emitter Name:     " << GetName() << " " << DisEnum::system::name::ToString(GetName()) << "\n"
      << "Emitter Function: " << static_cast<DisUint16>(GetFunction()) << " ("
      << DisEnum::system::function::ToString(GetFunction()) << ")\n"
      << "Emitter Number:   " << systemNumber << '\n'
      << "*********************************\n";

   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "Location (ECS): " << fixed << setprecision(0) << locationX << ", " << locationY << ", " << locationZ << " m\n";
   ss.imbue(tmp);
   for (auto it = mBeamList.GetBegin(); it != mBeamList.GetEnd(); it++)
   {
      ss << (**it).ToString() << '\n';
   }
   ss << "***** System " << systemNumber << " Data End *****";
   return ss.str();
}

bool DisSystem::CanAddBeam(const DisBeam* aBeamPtr, const DisEmission* aEmissionPtr) const
{
   if (GetNumberOfBeams() == Dis::GetEE_MAX_BEAM_THRSH())
   {
      // No more room
      return false;
   }

   // Calculate Size limit
   DisUint32 max_size = Dis::GetMAX_PDU_SIZE_OCTETS();
   if (!aEmissionPtr && mParentEmission)
   {
      aEmissionPtr = mParentEmission;
      // Factor in Current System already in PDU
      max_size += GetLengthOctets();
   }

   if (aEmissionPtr)
   {
      // Use Current PDU size
      max_size -= aEmissionPtr->GetLengthOctets();
   }
   // Subtract current size of this emitter system
   max_size -= GetLengthOctets();

   if (!aBeamPtr)
   {
      // Assume no targets for a beam
      max_size -= cBASE_BEAM_SIZE;
   }
   else
   {
      max_size -= aBeamPtr->GetLengthOctets();
   }
   return (max_size >= 0);
}

unsigned int DisSystem::RemoveBeamWithNoDelete(DisBeam* aBeam)
{
   unsigned int removed = mBeamList.RemoveWithNoDelete(aBeam);
   if (removed == 1)
   {
      aBeam->SetParentSystem(nullptr);
   }
   return removed;
}

void DisSystem::RemoveAllBeamsWithNoDelete()
{
   std::vector<DisBeam*>::const_iterator iter(mBeamList.GetBegin());
   while (iter != mBeamList.GetEnd())
   {
      (*iter++)->SetParentSystem(nullptr);
   }
   mBeamList.RemoveAllWithNoDelete();
}

bool DisSystem::IsValid() const
{
   // Note, a lot of people use alternate enumerations for Emitter Name, so
   // Not call the PDU invalid if it doesn't match the SISO Enumerations.
   // if(! DisEnum::system::name::IsValid(mName))         return false;
   if (!DisEnum::system::function::IsValid(mFunction))
   {
      return false;
   }
   if (mNumber == Dis::cNO_EMITTER || mNumber == Dis::cALL_EMITTERS)
   {
      return false;
   }
   if (!ValidateScalarTuple(mLocation))
   {
      return false;
   }

   std::vector<DisBeam*>::const_iterator iter(mBeamList.GetBegin());
   while (iter != mBeamList.GetEnd())
   {
      if (!(*iter++)->IsValid())
      {
         return false;
      }
   }

   return true;
}
