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

#include "DisTSPI.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisTSPI::DisTSPI()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::TSPI);
   SetProtocolFamily(DisEnum::Pdu::Family::LiveEntity);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisTSPI::DisTSPI(const DisTSPI& aSrc)
   : DisPdu(aSrc)
   , mEntityID(aSrc.mEntityID)
   , mTSPIFlag(aSrc.mTSPIFlag)
   , mReferencePoint(aSrc.mReferencePoint)
   , mEntityLocation(aSrc.mEntityLocation)
   , mEntityLinearVelocity(aSrc.mEntityLinearVelocity)
   , mEntityOrientation(aSrc.mEntityOrientation)
   , mPositionError(aSrc.mPositionError)
   , mOrientationError(aSrc.mOrientationError)
   , mDeadReckoningAlgorithm(aSrc.mDeadReckoningAlgorithm)
   , mEntityLinearAcceleration(aSrc.mEntityLinearAcceleration)
   , mEntityAngularVelocity(aSrc.mEntityAngularVelocity)
   , mMeasuredSpeed(aSrc.mMeasuredSpeed)
   , mSystemSpecificDataLength(aSrc.mSystemSpecificDataLength)
   , mSystemSpecificData(aSrc.mSystemSpecificData)
{
   SetPduType(DisEnum::Pdu::Type::TSPI);
   SetProtocolFamily(DisEnum::Pdu::Family::LiveEntity);
   GetLength();
}

DisTSPI::DisTSPI(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisTSPI::DisTSPI(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisTSPI* DisTSPI::Clone() const
{
   return new DisTSPI(*this);
}

bool DisTSPI::IsValid() const
{
   return DisPdu::IsValid() && mEntityID.IsValid();
}

void DisTSPI::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisTSPI::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   DisUint8  mask   = 1;

   if (mask & mTSPIFlag) // Entity Linear Velocity field
   {
      octets += 6;
   }
   if ((mask << 1) & mTSPIFlag) // Entity Orientation field
   {
      octets += 3;
   }
   if ((mask << 2) & mTSPIFlag) // Position Error field
   {
      octets += 4;
   }
   if ((mask << 3) & mTSPIFlag) // Orientation Error field
   {
      octets += 6;
   }
   if ((mask << 4) & mTSPIFlag) // Dead Reckoning Parameter field
   {
      if (mDeadReckoningAlgorithm == 4 || mDeadReckoningAlgorithm == 8)
      {
         octets += 7;
      }
      else if (mDeadReckoningAlgorithm == 5 || mDeadReckoningAlgorithm == 9)
      {
         octets += 4;
      }
      else if (mDeadReckoningAlgorithm == 3 || mDeadReckoningAlgorithm == 7)
      {
         octets += 4;
      }
      else
      {
         octets += 1;
      }
   }
   if ((mask << 5) & mTSPIFlag) // Measured Speed field
   {
      octets += 2;
   }
   if ((mask << 6) & mTSPIFlag) // System-Specific Data field
   {
      octets += mSystemSpecificDataLength + 1;
   }
   SetLength(octets);
   return octets;
}

void DisTSPI::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   DisUint8 mask = 1;

   mEntityID.Put(aGenO);
   aGenO << mTSPIFlag;
   aGenO << mReferencePoint << mEntityLocation[0] << mEntityLocation[1] << mEntityLocation[2];

   if (mask & mTSPIFlag) // Entity Linear Velocity field
   {
      aGenO << mEntityLinearVelocity[0] << mEntityLinearVelocity[1] << mEntityLinearVelocity[2];
   }
   if ((mask << 1) & mTSPIFlag) // Entity Orientation field
   {
      aGenO << mEntityOrientation[0] << mEntityOrientation[1] << mEntityOrientation[2];
   }
   if ((mask << 2) & mTSPIFlag) // Position Error field
   {
      aGenO << mPositionError[0] << mPositionError[1];
   }
   if ((mask << 3) & mTSPIFlag) // Orientation Error field
   {
      aGenO << mOrientationError[0] << mOrientationError[1] << mOrientationError[2];
   }
   if ((mask << 4) & mTSPIFlag) // Dead Reckoning Parameter field
   {
      if (mDeadReckoningAlgorithm == 4 || mDeadReckoningAlgorithm == 8)
      {
         aGenO << mDeadReckoningAlgorithm;
         aGenO << mEntityLinearAcceleration[0] << mEntityLinearAcceleration[1] << mEntityLinearAcceleration[2];
         aGenO << mEntityAngularVelocity[0] << mEntityAngularVelocity[1] << mEntityAngularVelocity[2];
      }
      else if (mDeadReckoningAlgorithm == 5 || mDeadReckoningAlgorithm == 9)
      {
         aGenO << mDeadReckoningAlgorithm;
         aGenO << mEntityLinearAcceleration[0] << mEntityLinearAcceleration[1] << mEntityLinearAcceleration[2];
      }
      else if (mDeadReckoningAlgorithm == 3 || mDeadReckoningAlgorithm == 7)
      {
         aGenO << mDeadReckoningAlgorithm;
         aGenO << mEntityAngularVelocity[0] << mEntityAngularVelocity[1] << mEntityAngularVelocity[2];
      }
      else
      {
         aGenO << mDeadReckoningAlgorithm;
      }
   }
   if ((mask << 5) & mTSPIFlag) // Measured Speed field
   {
      aGenO << mMeasuredSpeed;
   }

   if ((mask << 6) & mTSPIFlag) // System-Specific Data field
   {
      aGenO << mSystemSpecificDataLength;
      // 01000000
      for (const auto& it : mSystemSpecificData)
      {
         aGenO << it;
      }
   }
   // 10000000 reserved for flag continuation
}

void DisTSPI::GetMemberData(GenI& aGenI)
{
   DisUint8 mask = 1;

   mEntityID.Get(aGenI);
   aGenI >> mTSPIFlag;
   aGenI >> mReferencePoint >> mEntityLocation[0] >> mEntityLocation[1] >> mEntityLocation[2];

   if (mask & mTSPIFlag) // Entity Linear Velocity field
   {
      aGenI >> mEntityLinearVelocity[0] >> mEntityLinearVelocity[1] >> mEntityLinearVelocity[2];
   }
   if ((mask << 1) & mTSPIFlag) // Entity Orientation field
   {
      aGenI >> mEntityOrientation[0] >> mEntityOrientation[1] >> mEntityOrientation[2];
   }
   if ((mask << 2) & mTSPIFlag) // Position Error field
   {
      aGenI >> mPositionError[0] >> mPositionError[1];
   }
   if ((mask << 3) & mTSPIFlag) // Orientation Error field
   {
      aGenI >> mOrientationError[0] >> mOrientationError[1] >> mOrientationError[2];
   }
   if ((mask << 4) & mTSPIFlag) // Dead Reckoning Parameter field
   {
      if (mDeadReckoningAlgorithm == 4 || mDeadReckoningAlgorithm == 8)
      {
         aGenI >> mDeadReckoningAlgorithm;
         aGenI >> mEntityLinearAcceleration[0] >> mEntityLinearAcceleration[1] >> mEntityLinearAcceleration[2];
         aGenI >> mEntityAngularVelocity[0] >> mEntityAngularVelocity[1] >> mEntityAngularVelocity[2];
      }
      else if (mDeadReckoningAlgorithm == 5 || mDeadReckoningAlgorithm == 9)
      {
         aGenI >> mDeadReckoningAlgorithm;
         aGenI >> mEntityLinearAcceleration[0] >> mEntityLinearAcceleration[1] >> mEntityLinearAcceleration[2];
      }
      else if (mDeadReckoningAlgorithm == 3 || mDeadReckoningAlgorithm == 7)
      {
         aGenI >> mDeadReckoningAlgorithm;
         aGenI >> mEntityAngularVelocity[0] >> mEntityAngularVelocity[1] >> mEntityAngularVelocity[2];
      }
      else
      {
         aGenI >> mDeadReckoningAlgorithm;
      }
   }
   if ((mask << 5) & mTSPIFlag) // Measured Speed field
   {
      aGenI >> mMeasuredSpeed;
   }

   if ((mask << 6) & mTSPIFlag) // System-Specific Data field
   {
      aGenI >> mSystemSpecificDataLength;
      mSystemSpecificData.reserve(mSystemSpecificDataLength);

      for (int i = 0; i < mSystemSpecificDataLength; i++)
      {
         unsigned char data = 0;
         aGenI >> data;
         mSystemSpecificData.push_back(data);
      }
   }
}

std::string DisTSPI::ToString() const
{
   DisUint8    mask         = 1;
   std::string returnString = DisPdu::ToString();
   returnString += "===== DisTSPI =====\n";
   returnString += "Entity ID: " + mEntityID.ToString() + "\n";
   returnString += "TSPI Flag: " + std::to_string(mTSPIFlag) + "\n";
   returnString += "Reference Point: " + std::to_string(mReferencePoint) + "\n";
   returnString += "Entity Location: " + std::to_string(mEntityLocation[0]) + ", " +
                   std::to_string(mEntityLocation[1]) + ", " + std::to_string(mEntityLocation[2]) + "\n";

   if (mask & mTSPIFlag) // Entity Linear Velocity field
   {
      returnString += "Entity Linear Velocity: " + std::to_string(mEntityLinearVelocity[0]) + ", " +
                      std::to_string(mEntityLinearVelocity[1]) + ", " + std::to_string(mEntityLinearVelocity[2]) + "\n";
   }
   if ((mask << 1) & mTSPIFlag) // Entity Orientation field
   {
      returnString += "Entity Orientation: " + std::to_string(mEntityOrientation[0]) + ", " +
                      std::to_string(mEntityOrientation[1]) + ", " + std::to_string(mEntityOrientation[2]) + "\n";
   }
   if ((mask << 2) & mTSPIFlag) // Position Error field
   {
      returnString +=
         "PositionError: " + std::to_string(mPositionError[0]) + ", " + std::to_string(mPositionError[1]) + "\n";
   }
   if ((mask << 3) & mTSPIFlag) // Orientation Error field
   {
      returnString += "Orientation Error: " + std::to_string(mOrientationError[0]) + ", " +
                      std::to_string(mOrientationError[1]) + ", " + std::to_string(mOrientationError[2]) + "\n";
   }
   if ((mask << 4) & mTSPIFlag) // Dead Reckoning Parameter field
   {
      returnString += "Dead Reckoning Algorithm: " + std::to_string(mDeadReckoningAlgorithm) + "\n";
      returnString += "Entity Linear Acceleration: " + std::to_string(mEntityLinearAcceleration[0]) + ", " +
                      std::to_string(mEntityLinearAcceleration[1]) + ", " +
                      std::to_string(mEntityLinearAcceleration[2]) + "\n";
      returnString += "Entity Angular Velocity: " + std::to_string(mEntityAngularVelocity[0]) + ", " +
                      std::to_string(mEntityAngularVelocity[1]) + ", " + std::to_string(mEntityAngularVelocity[2]) + "\n";
   }
   if ((mask << 5) & mTSPIFlag) // Measured Speed field
   {
      returnString += "Measured Speed: " + std::to_string(mMeasuredSpeed) + "\n";
   }

   returnString += "System Specific Data Length: " + std::to_string(mSystemSpecificDataLength) + "\n";

   if ((mask << 6) & mTSPIFlag) // System-Specific Data field
   {
      for (int i = 0; i < mSystemSpecificDataLength; i++)
      {
         returnString += "System Specific Data " + std::to_string(i) + ": " + std::to_string(mSystemSpecificData[i]) + "\n";
      }
   }
   returnString += "=== End DisTSPI ===\n";
   return returnString;
}

void DisTSPI::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}
