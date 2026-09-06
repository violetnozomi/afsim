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

#include "DisMinefieldData.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

DisMinefieldData::DisMinefieldData()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::MinefieldData);
   SetProtocolFamily(DisEnum::Pdu::Family::Minefield);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisMinefieldData::DisMinefieldData(const DisMinefieldData& aSrc)
   : DisPdu(aSrc)
   , mMinefieldId(aSrc.mMinefieldId)
   , mRequestingSimId(aSrc.mRequestingSimId)
   , mMinefieldSequenceNumber(aSrc.mMinefieldSequenceNumber)
   , mMinefieldRequestID(aSrc.mMinefieldRequestID)
   , mPduSequenceNumber(aSrc.mPduSequenceNumber)
   , mPduCount(aSrc.mPduCount)
   , mMineCount(aSrc.mMineCount)
   , mSensorTypesCount(aSrc.mSensorTypesCount)
   , mDataFilter(aSrc.mDataFilter)
   , mMineType(aSrc.mMineType)
   , mSensorTypeRecords(aSrc.mSensorTypeRecords)
   , mMineLocations(aSrc.mMineLocations)
   , mGroundBurialDepthOffsets(aSrc.mGroundBurialDepthOffsets)
   , mWaterBurialDepthOffsets(aSrc.mWaterBurialDepthOffsets)
   , mSnowBurialDepthOffsets(aSrc.mSnowBurialDepthOffsets)
   , mMineOrientations(aSrc.mMineOrientations)
   , mThermalContrasts(aSrc.mThermalContrasts)
   , mReflectances(aSrc.mReflectances)
   , mMineEmplacementTimes(aSrc.mMineEmplacementTimes)
   , mMineEntityNumbers(aSrc.mMineEntityNumbers)
   , mFusings(aSrc.mFusings)
   , mScalarDetectionCoefficients(aSrc.mScalarDetectionCoefficients)
   , mPaintSchemes(aSrc.mPaintSchemes)
   , mTripDetonationWireCounts(aSrc.mTripDetonationWireCounts)
   , mVertexCounts(aSrc.mVertexCounts)
   , mVertices(aSrc.mVertices)
{
   GetLength();
}

DisMinefieldData::DisMinefieldData(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisMinefieldData::DisMinefieldData(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

void DisMinefieldData::AddMine(MineData& aMine)
{
   mMineLocations.push_back(aMine.location);
   mGroundBurialDepthOffsets.push_back(aMine.groundBurialOffset);
   mWaterBurialDepthOffsets.push_back(aMine.waterBurialOffset);
   mSnowBurialDepthOffsets.push_back(aMine.snowBurialOffset);
   mMineOrientations.push_back(aMine.orientation);
   mThermalContrasts.push_back(aMine.thermalContrast);
   mReflectances.push_back(aMine.reflectance);
   mMineEmplacementTimes.push_back(aMine.emplacmentTime);
   mMineEntityNumbers.push_back(aMine.entityNumber);
   mFusings.push_back(aMine.fusing);
   for (const auto& it : aMine.scalarDetectionCoefficient)
   {
      mScalarDetectionCoefficients.push_back(it);
   }

   mPaintSchemes.push_back(aMine.paintScheme);
   mTripDetonationWireCounts.push_back(aMine.tripDetonationWires);

   for (const auto& it : aMine.verticesCount)
   {
      mVertexCounts.push_back(it);
   }

   for (const auto& it : aMine.vertices)
   {
      mVertices.push_back(it);
   }

   mMineCount++;
}

void DisMinefieldData::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisMinefieldData::GetLength()
{
   DisUint16 bits = (DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS) * 8;
   DisUint8  N    = mMineCount;
   DisUint8  M    = mSensorTypesCount;
   DisUint8  S    = N * M;
   DisUint8  U    = 0;
   for (auto& it : mTripDetonationWireCounts)
   {
      U += it;
   }

   DisUint8 V = 0;
   if (mTripDetonationWireCounts.size())
   {
      for (int n = 0; n < N; n++)
      {
         for (int i = 0; i < mTripDetonationWireCounts[n]; i++)
         {
            V += mVertexCounts[i];
         }
      }
   }

   DisUint8 P1 = (DisUint8)(2 * (ceil(M / 2.0) * 2 - M));
   DisUint8 P2 =
      (DisUint8)(ceil((2 * N + 2 * N * DataFilterBitValue(8) + S * DataFilterBitValue(9) + N * DataFilterBitValue(10)) / 4.0) *
                    4 -
                 (2 * N + 2 * N * DataFilterBitValue(8) + S * DataFilterBitValue(9) + N * DataFilterBitValue(10)));

   DisUint8 P3 = (DisUint8)(ceil(N * DataFilterBitValue(7) / 4.0) * 4 - N * DataFilterBitValue(7));
   DisUint8 P4 = (DisUint8)(ceil(U / 4.0) * 4 - U);

   // Vector of DataFilter boolean values
   std::vector<DisUint8> F;
   F.reserve(cDATAFILTER_FIELDS_USED);
   int i = 0;
   std::generate_n(std::back_inserter(F), cDATAFILTER_FIELDS_USED, [&] { return DataFilterBitValue(i++); });

   DisUint8 G = F[0] | F[1] | F[2];
   DisUint8 R = F[0] | F[1] | F[2] | F[3];

   bits += 16 * M + 8 * P1 + 96 * N + 32 * N * G + 32 * N * F[2] + 96 * N * R + 32 * N * F[4] + 32 * N * F[5] +
           64 * N * F[6] + 16 * N + 16 * N * F[8] + 8 * S * F[9] + 8 * N * F[10] + 8 * P2 + 8 * N * F[7] + 8 * P3 +
           8 * U * F[7] + 8 * P4 * F[7] + 96 * V * F[7];

   DisUint16 octets = bits / 8;
   SetLength(octets);

   return octets;
}

void DisMinefieldData::GetMemberData(GenI& aGenI)
{
   DisUint8 pad8 = 0;

   aGenI >> mMinefieldId;
   aGenI >> mRequestingSimId;
   aGenI >> mMinefieldSequenceNumber;
   aGenI >> mMinefieldRequestID;
   aGenI >> mPduSequenceNumber;
   aGenI >> mPduCount;
   aGenI >> mMineCount;
   aGenI >> mSensorTypesCount;
   aGenI >> pad8;
   aGenI >> mDataFilter;
   aGenI >> mMineType;

   DisUint8 N = mMineCount;
   DisUint8 M = mSensorTypesCount;
   DisUint8 S = N * M;
   DisUint8 U = 0, V = 0;

   DisUint8 P1 = (DisUint8)(2 * (ceil(M / 2.0) * 2 - M));
   DisUint8 P2 =
      (DisUint8)(ceil((2 * N + 2 * N * DataFilterBitValue(8) + S * DataFilterBitValue(9) + N * DataFilterBitValue(10)) / 4.0) *
                    4 -
                 (2 * N + 2 * N * DataFilterBitValue(8) + S * DataFilterBitValue(9) + N * DataFilterBitValue(10)));

   DisUint8 P3 = (DisUint8)(ceil(N * DataFilterBitValue(7) / 4.0) * 4 - N * DataFilterBitValue(7));
   DisUint8 P4 = 0;

   std::vector<DisUint8> F;
   F.reserve(cDATAFILTER_FIELDS_USED);

   // Vector of DataFilter boolean values

   int i = 0;
   std::generate_n(std::back_inserter(F), cDATAFILTER_FIELDS_USED, [&] { return DataFilterBitValue(i++); });

   DisUint8 G = F[0] | F[1] | F[2];
   DisUint8 R = F[0] | F[1] | F[2] | F[3];

   mSensorTypeRecords.reserve(mSensorTypesCount);
   for (int i = 0; i < mSensorTypesCount; i++)
   {
      DisEnum16 in;
      aGenI >> in;
      mSensorTypeRecords.push_back(in);
   }

   // Get (padBytes) amount of Padding bytes from input buffer
   auto GetPad = [&](char padBytes)
   {
      DisUint8 pad;
      for (int i = 0; i < padBytes; i++)
      {
         aGenI >> pad;
      }
   };

   //// Padding to 32 bit boundary
   GetPad(P1);

   mMineLocations.reserve(mMineCount);
   mGroundBurialDepthOffsets.reserve(mMineCount);
   mWaterBurialDepthOffsets.reserve(mMineCount);
   mSnowBurialDepthOffsets.reserve(mMineCount);
   mMineOrientations.reserve(mMineCount);
   mThermalContrasts.reserve(mMineCount);
   mReflectances.reserve(mMineCount);
   mMineEmplacementTimes.reserve(mMineCount);
   mMineEntityNumbers.reserve(mMineCount);
   mFusings.reserve(mMineCount);
   mPaintSchemes.reserve(mMineCount);

   DisFloat32 float32 = 0;
   DisFloat32 x32     = 0;
   DisFloat32 y32     = 0;
   DisFloat32 z32     = 0;
   DisUint16  uInt16  = 0;
   DisUint8   uInt8   = 0;

   for (int i = 0; i < N; i++)
   {
      // Mine location
      aGenI >> x32 >> y32 >> z32;
      mMineLocations.emplace_back(x32, y32, z32);
   }

   //  Burial offsets
   if (G) // Ground
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> float32;
         mGroundBurialDepthOffsets.push_back(float32);
      }
   }
   if (F[1]) // Water
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> float32;
         mWaterBurialDepthOffsets.push_back(float32);
      }
   }
   if (F[2]) // Snow
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> float32;
         mSnowBurialDepthOffsets.push_back(float32);
      }
   }
   if (R) // Mine Orientation
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> x32 >> y32 >> z32;
         mMineOrientations.emplace_back(x32, y32, z32);
      }
   }
   if (F[4]) // Thermal Contrast
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> float32;
         mThermalContrasts.push_back(float32);
      }
   }
   if (F[5]) // Reflectance
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> float32;
         mReflectances.push_back(float32);
      }
   }
   if (F[6]) // Mine Emplacment Time
   {
      for (int i = 0; i < N; i++)
      {
         MineEmplacementTime time;
         aGenI >> time.hour >> time.timePastHour;
         mMineEmplacementTimes.push_back(time);
      }
   }
   for (int i = 0; i < N; i++)
   {
      // Mine Entity Number
      aGenI >> uInt16;
      mMineEntityNumbers.push_back(uInt16);
   }

   if (F[8]) // Fusing
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> uInt16;
         mFusings.push_back(uInt16);
      }
   }
   if (F[9]) // Scalar Detection Coefficient
   {
      for (int i = 0; i < S; i++)
      {
         aGenI >> uInt8;
         mScalarDetectionCoefficients.push_back(uInt8);
      }
   }
   if (F[10]) // Paint Scheme
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> uInt8;
         mPaintSchemes.push_back(uInt8);
      }
   }

   //// Padding to 32 bit boundary
   GetPad(P2);

   if (F[7]) // Number of Trip/Detonation Wires
   {
      for (int i = 0; i < N; i++)
      {
         aGenI >> uInt8;
         mTripDetonationWireCounts.push_back(uInt8);
      }
   }

   // U
   for (auto& it : mTripDetonationWireCounts)
   {
      U += it;
   }

   // P4
   P4 = (DisUint8)(ceil(U / 4.0) * 4 - U);

   //// Padding to 32 bit boundary
   GetPad(P3);

   if (F[7]) // Vertex Counts
   {
      for (int i = 0; i < U; i++)
      {
         aGenI >> uInt8;
         mVertexCounts.push_back(uInt8);
      }
   }
   // V
   for (int n = 0; n < N; n++)
   {
      for (int i = 0; i < mTripDetonationWireCounts[n]; i++)
      {
         V += mVertexCounts[i];
      }
   }

   //// Padding to 32 bit boundary
   GetPad(P4);

   if (F[7]) // Vertices
   {
      for (int i = 0; i < V; i++)
      {
         aGenI >> x32 >> y32 >> z32;
         mVertices.emplace_back(x32, y32, z32);
      }
   }

   //// Skip 'extra' data
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint16 lengthRead   = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisMinefieldData::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint8 pad8 = 0;
   DisUint8 N    = mMineCount;
   DisUint8 M    = mSensorTypesCount;
   DisUint8 S    = N * M;
   DisUint8 U = 0, V = 0;

   DisUint8 P1 = (DisUint8)(2 * (ceil(M / 2.0) * 2 - M));
   DisUint8 P2 =
      (DisUint8)(ceil((2 * N + 2 * N * DataFilterBitValue(8) + S * DataFilterBitValue(9) + N * DataFilterBitValue(10)) / 4.0) *
                    4 -
                 (2 * N + 2 * N * DataFilterBitValue(8) + S * DataFilterBitValue(9) + N * DataFilterBitValue(10)));

   DisUint8 P3 = (DisUint8)(ceil(N * DataFilterBitValue(7) / 4.0) * 4 - N * DataFilterBitValue(7));
   DisUint8 P4 = 0;

   // Vector of DataFilter boolean values
   std::vector<DisUint8> F;
   F.reserve(cDATAFILTER_FIELDS_USED);

   int i = 0;
   std::generate_n(std::back_inserter(F), cDATAFILTER_FIELDS_USED, [&] { return DataFilterBitValue(i++); });

   DisUint8 G = F[0] | F[1] | F[2];
   DisUint8 R = F[0] | F[1] | F[2] | F[3];

   aGenO << mMinefieldId;
   aGenO << mRequestingSimId;
   aGenO << mMinefieldSequenceNumber;
   aGenO << mMinefieldRequestID;
   aGenO << mPduSequenceNumber;
   aGenO << mPduCount;
   aGenO << mMineCount;
   aGenO << mSensorTypesCount;
   aGenO << pad8;
   aGenO << mDataFilter;
   aGenO << mMineType;

   for (const auto& it : mSensorTypeRecords)
   {
      aGenO << it;
   }

   // Get (padBytes) amount of Padding bytes from input buffer
   auto PutPad = [&](char padBytes)
   {
      DisUint8 pad = 0;
      for (int i = 0; i < padBytes; i++)
      {
         aGenO << pad;
      }
   };

   //// Padding to 32 bit boundary
   PutPad(P1);

   for (const auto& it : mMineLocations)
   {
      // Mine location
      aGenO << it[0] << it[1] << it[2];
   }

   //  Burial offsets
   if (G) // Ground
   {
      for (const auto& it : mGroundBurialDepthOffsets)
      {
         aGenO << it;
      }
   }
   if (F[1]) // Water
   {
      for (const auto& it : mWaterBurialDepthOffsets)
      {
         aGenO << it;
      }
   }
   if (F[2]) // Snow
   {
      for (const auto& it : mSnowBurialDepthOffsets)
      {
         aGenO << it;
      }
   }
   if (R) // Mine Orientation
   {
      for (const auto& it : mMineOrientations)
      {
         aGenO << it[0] << it[1] << it[2];
      }
   }
   if (F[4]) // Thermal Contrast
   {
      for (const auto& it : mThermalContrasts)
      {
         aGenO << it;
      }
   }
   if (F[5]) // Reflectance
   {
      for (const auto& it : mReflectances)
      {
         aGenO << it;
      }
   }
   if (F[6]) // Mine Emplacment Time
   {
      for (const auto& it : mMineEmplacementTimes)
      {
         aGenO << it.hour;
         aGenO << it.timePastHour;
      }
   }

   // Mine Entity Number
   for (const auto& it : mMineEntityNumbers)
   {
      aGenO << it;
   }

   if (F[8]) // Fusing
   {
      for (const auto& it : mFusings)
      {
         aGenO << it;
      }
   }
   if (F[9]) // Scalar Detection Coefficient
   {
      for (const auto& it : mScalarDetectionCoefficients)
      {
         aGenO << it;
      }
   }
   if (F[10]) // Paint Scheme
   {
      for (const auto& it : mPaintSchemes)
      {
         aGenO << it;
      }
   }

   //// Padding to 32 bit boundary
   PutPad(P2);

   if (F[7]) // Number of Trip/Detonation Wires
   {
      for (const auto& it : mTripDetonationWireCounts)
      {
         aGenO << it;
      }
   }

   // U
   for (auto& it : mTripDetonationWireCounts)
   {
      U += it;
   }
   // V
   for (int n = 0; n < N; n++)
   {
      for (int i = 0; i < mTripDetonationWireCounts[n]; i++)
      {
         V += mVertexCounts[i];
      }
   }
   // P4
   P4 = (DisUint8)(ceil(U / 4.0) * 4 - U);

   //// Padding to 32 bit boundary
   PutPad(P3);

   if (F[7]) // Vertex Counts
   {
      for (const auto& it : mVertexCounts)
      {
         aGenO << it;
      }
   }

   //// Padding to 32 bit boundary
   PutPad(P4);

   if (F[7]) // Vertices
   {
      for (const auto& it : mVertices)
      {
         aGenO << it[0] << it[1] << it[2];
      }
   }
}

bool DisMinefieldData::IsValid() const
{
   bool isValid = DisPdu::IsValid() && mMinefieldId.IsValid() && mRequestingSimId.IsValid() && mMineType.IsValid();
   for (int i = 0; i < static_cast<int>(mGroundBurialDepthOffsets.size()); i++)
   {
      isValid &= ValidateScalar(mGroundBurialDepthOffsets[i]);
   }

   for (int i = 0; i < static_cast<int>(mWaterBurialDepthOffsets.size()); i++)
   {
      isValid &= ValidateScalar(mWaterBurialDepthOffsets[i]);
   }

   for (int i = 0; i < static_cast<int>(mSnowBurialDepthOffsets.size()); i++)
   {
      isValid &= ValidateScalar(mSnowBurialDepthOffsets[i]);
   }

   for (int i = 0; i < static_cast<int>(mThermalContrasts.size()); i++)
   {
      isValid &= ValidateScalar(mThermalContrasts[i]);
   }

   for (int i = 0; i < static_cast<int>(mReflectances.size()); i++)
   {
      isValid &= ValidateScalar(mReflectances[i]);
   }

   for (int i = 0; i < static_cast<int>(mMineLocations.size()); i++)
   {
      isValid &= ValidateScalar(mMineLocations[i][0]) && ValidateScalar(mMineLocations[i][1]) &&
                 ValidateScalar(mMineLocations[i][2]);
   }
   for (int i = 0; i < static_cast<int>(mMineOrientations.size()); i++)
   {
      isValid &= ValidateScalar(mMineOrientations[i][0]) && ValidateScalar(mMineOrientations[i][1]) &&
                 ValidateScalar(mMineOrientations[i][2]);
   }
   for (int i = 0; i < static_cast<int>(mVertices.size()); i++)
   {
      isValid &= ValidateScalar(mVertices[i][0]) && ValidateScalar(mVertices[i][1]) && ValidateScalar(mVertices[i][2]);
   }
   return isValid;
}

std::string DisMinefieldData::GetStringId() const
{
   return "DisMinefieldData";
}

int DisMinefieldData::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::MinefieldData);
}

DisMinefieldData* DisMinefieldData::Clone() const
{
   return new DisMinefieldData(*this);
}

void DisMinefieldData::AddSensorTypeRecord(DisEnum16 aSensorType)
{
   mSensorTypeRecords.push_back(aSensorType);
   mSensorTypesCount++;
}

bool DisMinefieldData::DataFilterBitValue(DisUint32 aVal)
{
   DisUint32 mask = 1;
   return (mDataFilter >> aVal) & mask;
}

const bool DisMinefieldData::GetMine(DisUint16 aIndex, DisMinefieldData::MineData& aReturnMine) const
{
   bool mineAtIndexFound = false;

   if (aIndex >= 0 && aIndex < static_cast<int>(mMineCount))
   {
      mineAtIndexFound               = true;
      aReturnMine.location           = mMineLocations[aIndex];
      aReturnMine.groundBurialOffset = mGroundBurialDepthOffsets[aIndex];
      aReturnMine.waterBurialOffset  = mWaterBurialDepthOffsets[aIndex];
      aReturnMine.snowBurialOffset   = mSnowBurialDepthOffsets[aIndex];
      aReturnMine.orientation        = mMineOrientations[aIndex];
      aReturnMine.thermalContrast    = mThermalContrasts[aIndex];
      aReturnMine.reflectance        = mReflectances[aIndex];
      aReturnMine.emplacmentTime     = mMineEmplacementTimes[aIndex];
      aReturnMine.entityNumber       = mMineEntityNumbers[aIndex];
      aReturnMine.fusing             = mFusings[aIndex];

      aReturnMine.scalarDetectionCoefficient.resize(mSensorTypesCount);

      // copy range of scalar coefficients for mine aIndex
      std::copy_n(mScalarDetectionCoefficients.begin() + (aIndex * mSensorTypesCount),
                  mSensorTypesCount,
                  aReturnMine.scalarDetectionCoefficient.begin());

      aReturnMine.paintScheme         = mPaintSchemes[aIndex];
      aReturnMine.tripDetonationWires = mTripDetonationWireCounts[aIndex];

      int total = 0;

      // add all mines trip wire counts up until this mine (index) to get
      // location in mVerticesCount vector where this mine's data starts
      for (int i = 0; i < aIndex; i++)
      {
         total += mTripDetonationWireCounts[i];
      }

      // total--; // adjust for zero based indexing
      aReturnMine.verticesCount.resize(mTripDetonationWireCounts[aIndex]);
      std::copy_n(mVertexCounts.begin() + total, mTripDetonationWireCounts[aIndex], aReturnMine.verticesCount.begin());

      // get total vertices for trips wires of previous mines in vector
      int vertexTotal = 0;
      for (int i = 0; i < total; i++)
      {
         vertexTotal += mVertexCounts[i];
      }

      // get total vertices for this mine
      total = 0;
      for (const auto& it : aReturnMine.verticesCount)
      {
         total += it;
      }
      aReturnMine.vertices.resize(total);

      // copy range of vertices for this mine's tripwires
      std::copy_n(mVertices.begin() + vertexTotal, total, aReturnMine.vertices.begin());
   }

   return mineAtIndexFound;
}
