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

#ifndef DISMINEFIELDDATA_HPP
#define DISMINEFIELDDATA_HPP

#include "dis_export.h"

#include <iostream>
#include <vector>

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisPdu.hpp"
#include "DisTypes.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * The Minefield Data PDU shall provide information on individual mines contained
 * within a minefield.
 */
class DIS_EXPORT DisMinefieldData : public DisPdu
{
public:
   DisMinefieldData();
   DisMinefieldData(const DisMinefieldData& aSrc);
   DisMinefieldData(const DisPdu& aPdu, GenI& aGenI);
   explicit DisMinefieldData(GenI& aGenI);

   DisMinefieldData* Clone() const override;

   //! Clock Time record
   struct MineEmplacementTime
   {
      DisInt32  hour         = 0;
      DisUint32 timePastHour = 0;
   };

   //! Data record representing an individual mine
   struct MineData
   {
      UtVec3<DisFloat32>              location;
      DisFloat32                      groundBurialOffset = 0.0f;
      DisFloat32                      waterBurialOffset  = 0.0f;
      DisFloat32                      snowBurialOffset   = 0.0f;
      UtVec3<DisFloat32>              orientation;
      DisFloat32                      thermalContrast = 0.0f;
      DisFloat32                      reflectance     = 0.0f;
      MineEmplacementTime             emplacmentTime;
      DisUint16                       entityNumber = 0;
      DisUint16                       fusing       = 0;
      std::vector<DisUint8>           scalarDetectionCoefficient;
      DisEnum8                        paintScheme         = 0;
      DisUint8                        tripDetonationWires = 0;
      std::vector<DisUint8>           verticesCount;
      std::vector<UtVec3<DisFloat32>> vertices;
   };

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors
   const DisEntityId&   GetMineFieldId() const;
   const DisEntityId&   GetRequestingSimId() const;
   DisUint16            GetMinefieldSequenceNumber() const;
   DisUint8             GetMinefieldRequestID() const;
   DisUint32            GetRequestId() = delete;
   DisUint8             GetPduSequenceNumber() const;
   DisUint8             GetPduCount() const;
   DisUint8             GetMineCount() const;
   DisUint8             GetSensorTypesCount() const;
   DisUint32            GetDataFilter() const;
   const DisEntityType& GetMineType() const;
   const bool           GetMine(DisUint16 aMineNumber, DisMinefieldData::MineData& aReturnMine) const;

   // Mutators
   void SetMinefieldId(const DisEntityId& aMinefieldId);
   void SetRequestingSimId(const DisEntityId& aRequestingSimId);
   void SetMinefieldRequestID(const DisUint8 aMinefieldRequestID);
   void SetMinefieldSequenceNumber(DisUint16 aMinefieldSequenceNumber);
   void SetPduSequenceNumber(DisUint8 aPduSequenceNumber);
   void SetPduCount(DisUint8 aPduCount);
   void SetDataFilter(DisUint32 aDataFilter);
   void SetMineType(const DisEntityType& aMineType);

   void AddSensorTypeRecord(DisEnum16 aSensorType);
   void AddMine(MineData& aMine);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Return iterators to SensorType vector.
   std::vector<DisEnum16>::iterator       GetSensorTypeRecordsBegin();
   std::vector<DisEnum16>::const_iterator GetSensorTypeRecordsBegin() const;
   std::vector<DisEnum16>::iterator       GetSensorTypeRecordsEnd();
   std::vector<DisEnum16>::const_iterator GetSensorTypeRecordsEnd() const;

   // Return iterators to MineLocations vector.
   std::vector<UtVec3<DisFloat32>>::iterator       GetMineLocationsBegin();
   std::vector<UtVec3<DisFloat32>>::const_iterator GetMineLocationsBegin() const;
   std::vector<UtVec3<DisFloat32>>::iterator       GetMineLocationsEnd();
   std::vector<UtVec3<DisFloat32>>::const_iterator GetMineLocationsEnd() const;

   // Return iterators to GroundBurialDepthOffsets vector.
   std::vector<DisFloat32>::iterator       GetGroundBurialDepthOffsetsBegin();
   std::vector<DisFloat32>::const_iterator GetGroundBurialDepthOffsetsBegin() const;
   std::vector<DisFloat32>::iterator       GetGroundBurialDepthOffsetsEnd();
   std::vector<DisFloat32>::const_iterator GetGroundBurialDepthOffsetsEnd() const;

   // Return iterators to WaterBurialDepthOffsets vector.
   std::vector<DisFloat32>::iterator       GetWaterBurialDepthOffsetsBegin();
   std::vector<DisFloat32>::const_iterator GetWaterBurialDepthOffsetsBegin() const;
   std::vector<DisFloat32>::iterator       GetWaterBurialDepthOffsetsEnd();
   std::vector<DisFloat32>::const_iterator GetWaterBurialDepthOffsetsEnd() const;

   // Return iterators to SnowBurialDepthOffsets vector.
   std::vector<DisFloat32>::iterator       GetSnowBurialDepthOffsetsBegin();
   std::vector<DisFloat32>::const_iterator GetSnowBurialDepthOffsetsBegin() const;
   std::vector<DisFloat32>::iterator       GetSnowBurialDepthOffsetsEnd();
   std::vector<DisFloat32>::const_iterator GetSnowBurialDepthOffsetsEnd() const;

   // Return iterators to MineOrientations vector.
   std::vector<UtVec3<DisFloat32>>::iterator       GetMineOrientationsBegin();
   std::vector<UtVec3<DisFloat32>>::const_iterator GetMineOrientationsBegin() const;
   std::vector<UtVec3<DisFloat32>>::iterator       GetMineOrientationsEnd();
   std::vector<UtVec3<DisFloat32>>::const_iterator GetMineOrientationsEnd() const;

   // Return iterators to ThermalContrasts vector.
   std::vector<DisFloat32>::iterator       GetThermalContrastsBegin();
   std::vector<DisFloat32>::const_iterator GetThermalContrastsBegin() const;
   std::vector<DisFloat32>::iterator       GetThermalContrastsEnd();
   std::vector<DisFloat32>::const_iterator GetThermalContrastsEnd() const;

   // Return iterators to Reflectances vector.
   std::vector<DisFloat32>::iterator       GetReflectancesBegin();
   std::vector<DisFloat32>::const_iterator GetReflectancesBegin() const;
   std::vector<DisFloat32>::iterator       GetReflectancesEnd();
   std::vector<DisFloat32>::const_iterator GetReflectancesEnd() const;

   // Return iterators to MineEmplacementTimes vector.
   std::vector<DisMinefieldData::MineEmplacementTime>::iterator       GetMineEmplacementTimesBegin();
   std::vector<DisMinefieldData::MineEmplacementTime>::const_iterator GetMineEmplacementTimesBegin() const;
   std::vector<DisMinefieldData::MineEmplacementTime>::iterator       GetMineEmplacementTimesEnd();
   std::vector<DisMinefieldData::MineEmplacementTime>::const_iterator GetMineEmplacementTimesEnd() const;

   // Return iterators to MineEntityNumbers vector.
   std::vector<DisUint16>::iterator       GetMineEntityNumbersBegin();
   std::vector<DisUint16>::const_iterator GetMineEntityNumbersBegin() const;
   std::vector<DisUint16>::iterator       GetMineEntityNumbersEnd();
   std::vector<DisUint16>::const_iterator GetMineEntityNumbersEnd() const;

   // Return iterators to Fusings vector.
   std::vector<DisUint16>::iterator       GetFusingsBegin();
   std::vector<DisUint16>::const_iterator GetFusingsBegin() const;
   std::vector<DisUint16>::iterator       GetFusingsEnd();
   std::vector<DisUint16>::const_iterator GetFusingsEnd() const;

   // Return iterators to ScalarDetectionCoefficients vector.
   std::vector<DisUint8>::iterator       GetScalarDetectionCoefficientsBegin();
   std::vector<DisUint8>::const_iterator GetScalarDetectionCoefficientsBegin() const;
   std::vector<DisUint8>::iterator       GetScalarDetectionCoefficientsEnd();
   std::vector<DisUint8>::const_iterator GetScalarDetectionCoefficientsEnd() const;

   // Return iterators to PaintSchemes vector.
   std::vector<DisEnum8>::iterator       GetPaintSchemesBegin();
   std::vector<DisEnum8>::const_iterator GetPaintSchemesBegin() const;
   std::vector<DisEnum8>::iterator       GetPaintSchemesEnd();
   std::vector<DisEnum8>::const_iterator GetPaintSchemesEnd() const;

   // Return iterators to TripDetonationWireCounts vector.
   std::vector<DisUint8>::iterator       GetTripDetonationWireCountsBegin();
   std::vector<DisUint8>::const_iterator GetTripDetonationWireCountsBegin() const;
   std::vector<DisUint8>::iterator       GetTripDetonationWireCountsEnd();
   std::vector<DisUint8>::const_iterator GetTripDetonationWireCountsEnd() const;

   // Return iterators to VertexCounts vector.
   std::vector<DisUint8>::iterator       GetVertexCountsBegin();
   std::vector<DisUint8>::const_iterator GetVertexCountsBegin() const;
   std::vector<DisUint8>::iterator       GetVertexCountsEnd();
   std::vector<DisUint8>::const_iterator GetVertexCountsEnd() const;

   // Return iterators to Vertices vector.
   std::vector<UtVec3<DisFloat32>>::iterator       GetVerticesBegin();
   std::vector<UtVec3<DisFloat32>>::const_iterator GetVerticesBegin() const;
   std::vector<UtVec3<DisFloat32>>::iterator       GetVerticesEnd();
   std::vector<UtVec3<DisFloat32>>::const_iterator GetVerticesEnd() const;

private:
   //! Returns true if the bit in mDataFilter at position aVal is set
   bool DataFilterBitValue(DisUint32 aVal);

   void GetMemberData(GenI& aGenI);

   //! Specifies how many bits of the 32-bit data filter (mDataFilter) are not unused
   static const int cDATAFILTER_FIELDS_USED = 11;

   //! the minefield to which the mines in this PDU belong
   DisEntityId mMinefieldId = DisEntityId::ENTITY_ID_UNKNOWN;

   /**!
    * the simulation that requested the information
    * from the minefield simulation in QRP mode. In heartbeat mode, the Requesting Simulation ID shall
    * contain the value NO_SITE: NO_APPLIC: NO_ENTITY.
    */
   DisEntityId mRequestingSimId = DisEntityId::ENTITY_ID_UNKNOWN;

   /**!
    * the matching minefield sequence number from the associated
    * Minefield State PDU
    */
   DisUint16 mMinefieldSequenceNumber = 0;

   /**!
    * the matching response to a request for mine information from the
    * minefield simulation made by means of a Minefield Query PDU in QRP mode. In heartbeat mode, this
    * field shall contain the value zero.
    */
   DisUint8 mMinefieldRequestID = 0;

   /**!
    * the number of the current Minefield Data PDU in a
    * sequence of Minefield Data PDUs sent in response to a
    * Minefield Query PDU when operating in QRP mode.
    */
   DisUint8 mPduSequenceNumber = 0;

   /**!
    * the total number of Minefield Data PDUs being sent in
    * response to a Minefield Query PDU when operating in QRP mode
    */
   DisUint8 mPduCount = 0;

   /**!
    * the number of mines of the same type contained in this
    * Minefield Data PDU
    */
   DisUint8 mMineCount = 0;

   /**!
    * In QRP mode, the number of sensor types employed
    * by the requesting simulation as specified in the Minefield Query PDU.
    * In heartbeat mode, the number of sensor types employed in the exercise.
    */
   DisUint8 mSensorTypesCount = 0;
   //! Data Filter
   /*!
    *  Identifies those optional data fields that are being transmitted with
    *  the current PDU.
    *  ----------------------------------------------------
    *  |      |             |                             |
    *  | Bit  |  Data Type  |  Field Name                 |
    *  |      |             |                             |
    *  |--------------------------------------------------|
    *  | 11-31| Boolean     | Padding                     |
    *  |--------------------------------------------------|
    *  |  10  | Boolean     | Paint Scheme                |
    *  |--------------------------------------------------|
    *  |   9  | Boolean     | Scalar Detection Coefficient|
    *  |--------------------------------------------------|
    *  |   8  | Boolean     | Fusing                      |
    *  |--------------------------------------------------|
    *  |   7  | Boolean     | Trip/Detonation Wire        |
    *  |--------------------------------------------------|
    *  |   6  | Boolean     | Mine Emplacement Time       |
    *  |--------------------------------------------------|
    *  |   5  | Boolean     | Reflectance                 |
    *  |--------------------------------------------------|
    *  |   4  | Boolean     | Thermal Contrast            |
    *  |--------------------------------------------------|
    *  |   3  | Boolean     | Mine Orientation            |
    *  |--------------------------------------------------|
    *  |   2  | Boolean     | Snow Burial Depth Offset    |
    *  |--------------------------------------------------|
    *  |   1  | Boolean     | Water Burial Depth Offset   |
    *  |--------------------------------------------------|
    *  |   0  | Boolean     | Ground Burial Depth Offset  |
    *  |--------------------------------------------------|
    */
   DisUint32 mDataFilter = 0;

   //! the type of mine contained in this Minefield Data PDU
   DisEntityType mMineType;

   /**!
    * In QRP mode, this field shall specify the requesting sensor type that was specified in
    * the Minefield Query PDU. In heartbeat mode, this field shall specify the sensor type that is being
    * served by the minefield.
    */
   std::vector<DisEnum16> mSensorTypeRecords;

   /**!
    * the location of the mine, relative to the Minefield Location
    * field, given in the corresponding Minefield State PDU field
    */
   std::vector<UtVec3<DisFloat32>> mMineLocations;

   /**!
    * the offset of the origin of the mine coordinate
    * system with respect to the ground surface.
    */
   std::vector<DisFloat32> mGroundBurialDepthOffsets;

   /**!
    * the offset of the origin of the mine coordinate
    * system with respect to the water surface
    */
   std::vector<DisFloat32> mWaterBurialDepthOffsets;

   /**!
    * the offset of the origin of the mine coordinate
    * system with respect to the snow surface.
    */
   std::vector<DisFloat32> mSnowBurialDepthOffsets;

   /**!
    * the orientation of the center axis direction of fire of the
    * mine, relative to the minefield Entity Coordinate System
    */
   std::vector<UtVec3<DisFloat32>> mMineOrientations;

   /**!
    * the temperature difference between the mine and the
    * surrounding soil in degrees Centigrade
    */
   std::vector<DisFloat32> mThermalContrasts;

   /**!
    * the local dielectric difference between the mine and the
    * surrounding soil.
    */
   std::vector<DisFloat32> mReflectances;

   //! the simulation time of emplacement of the mine.
   std::vector<MineEmplacementTime> mMineEmplacementTimes;

   //! the mine entity identification
   std::vector<DisUint16> mMineEntityNumbers;

   //! the primary and secondary fuse and antihandling device for each mine
   std::vector<DisUint16> mFusings;

   /**!
    * the coefficient to be utilized for proper correlation between detectors
    * located on different simulation platforms
    */
   std::vector<DisUint8> mScalarDetectionCoefficients;

   //! the camouflage scheme/color of the mine
   std::vector<DisEnum8> mPaintSchemes;

   //! the number of trip/detonation wires of the mine
   std::vector<DisUint8> mTripDetonationWireCounts;

   /**!
    * the number of vertices for a specific trip/detonation wire
    * of the mine.
    */
   std::vector<DisUint8> mVertexCounts;

   /**!
    * the location of the trip/detonation wire vertex relative to the
    * Minefield Location field given in the corresponding Minefield State PDU field.
    */
   std::vector<UtVec3<DisFloat32>> mVertices;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 32;
};

inline const DisEntityId& DisMinefieldData::GetOriginatingEntity() const
{
   return mMinefieldId;
}

// Accessors
inline const DisEntityId& DisMinefieldData::GetMineFieldId() const
{
   return mMinefieldId;
}

inline const DisEntityId& DisMinefieldData::GetRequestingSimId() const
{
   return mRequestingSimId;
}

inline DisUint16 DisMinefieldData::GetMinefieldSequenceNumber() const
{
   return mMinefieldSequenceNumber;
}

inline DisUint8 DisMinefieldData::GetMinefieldRequestID() const
{
   return mMinefieldRequestID;
}

inline DisUint8 DisMinefieldData::GetPduSequenceNumber() const
{
   return mPduSequenceNumber;
}

inline DisUint8 DisMinefieldData::GetPduCount() const
{
   return mPduCount;
}

inline DisUint8 DisMinefieldData::GetMineCount() const
{
   return mMineCount;
}

inline DisUint8 DisMinefieldData::GetSensorTypesCount() const
{
   return mSensorTypesCount;
}

inline DisUint32 DisMinefieldData::GetDataFilter() const
{
   return mDataFilter;
}

inline const DisEntityType& DisMinefieldData::GetMineType() const
{
   return mMineType;
}

// Mutators
inline void DisMinefieldData::SetMinefieldId(const DisEntityId& aMinefieldId)
{
   mMinefieldId = aMinefieldId;
}

inline void DisMinefieldData::SetRequestingSimId(const DisEntityId& aRequestingSimId)
{
   mRequestingSimId = aRequestingSimId;
}

inline void DisMinefieldData::SetMinefieldRequestID(const DisUint8 aMinefieldRequestID)
{
   mMinefieldRequestID = aMinefieldRequestID;
}

inline void DisMinefieldData::SetMinefieldSequenceNumber(DisUint16 aMinefieldSequenceNumber)
{
   mMinefieldSequenceNumber = aMinefieldSequenceNumber;
}

inline void DisMinefieldData::SetPduSequenceNumber(DisUint8 aPduSequenceNumber)
{
   mPduSequenceNumber = aPduSequenceNumber;
}

inline void DisMinefieldData::SetPduCount(DisUint8 aPduCount)
{
   mPduCount = aPduCount;
}

inline void DisMinefieldData::SetDataFilter(DisUint32 aDataFilter)
{
   mDataFilter = aDataFilter;
}

inline void DisMinefieldData::SetMineType(const DisEntityType& aMineType)
{
   mMineType = aMineType;
}

inline std::vector<DisEnum16>::iterator DisMinefieldData::GetSensorTypeRecordsBegin()
{
   return mSensorTypeRecords.begin();
}

inline std::vector<DisEnum16>::const_iterator DisMinefieldData::GetSensorTypeRecordsBegin() const
{
   return mSensorTypeRecords.begin();
}

inline std::vector<DisEnum16>::iterator DisMinefieldData::GetSensorTypeRecordsEnd()
{
   return mSensorTypeRecords.end();
}

inline std::vector<DisEnum16>::const_iterator DisMinefieldData::GetSensorTypeRecordsEnd() const
{
   return mSensorTypeRecords.end();
}

inline std::vector<UtVec3<DisFloat32>>::iterator DisMinefieldData::GetMineLocationsBegin()
{
   return mMineLocations.begin();
}

inline std::vector<UtVec3<DisFloat32>>::const_iterator DisMinefieldData::GetMineLocationsBegin() const
{
   return mMineLocations.cbegin();
}

inline std::vector<UtVec3<DisFloat32>>::iterator DisMinefieldData::GetMineLocationsEnd()
{
   return mMineLocations.end();
}

inline std::vector<UtVec3<DisFloat32>>::const_iterator DisMinefieldData::GetMineLocationsEnd() const
{
   return mMineLocations.cend();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetGroundBurialDepthOffsetsBegin()
{
   return mGroundBurialDepthOffsets.begin();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetGroundBurialDepthOffsetsBegin() const
{
   return mGroundBurialDepthOffsets.cbegin();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetGroundBurialDepthOffsetsEnd()
{
   return mGroundBurialDepthOffsets.end();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetGroundBurialDepthOffsetsEnd() const
{
   return mGroundBurialDepthOffsets.cend();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetWaterBurialDepthOffsetsBegin()
{
   return mWaterBurialDepthOffsets.begin();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetWaterBurialDepthOffsetsBegin() const
{
   return mWaterBurialDepthOffsets.cbegin();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetWaterBurialDepthOffsetsEnd()
{
   return mWaterBurialDepthOffsets.end();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetWaterBurialDepthOffsetsEnd() const
{
   return mWaterBurialDepthOffsets.cend();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetSnowBurialDepthOffsetsBegin()
{
   return mSnowBurialDepthOffsets.begin();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetSnowBurialDepthOffsetsBegin() const
{
   return mSnowBurialDepthOffsets.cbegin();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetSnowBurialDepthOffsetsEnd()
{
   return mSnowBurialDepthOffsets.end();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetSnowBurialDepthOffsetsEnd() const
{
   return mSnowBurialDepthOffsets.cend();
}

inline std::vector<UtVec3<DisFloat32>>::iterator DisMinefieldData::GetMineOrientationsBegin()
{
   return mMineOrientations.begin();
}

inline std::vector<UtVec3<DisFloat32>>::const_iterator DisMinefieldData::GetMineOrientationsBegin() const
{
   return mMineOrientations.cbegin();
}

inline std::vector<UtVec3<DisFloat32>>::iterator DisMinefieldData::GetMineOrientationsEnd()
{
   return mMineOrientations.end();
}

inline std::vector<UtVec3<DisFloat32>>::const_iterator DisMinefieldData::GetMineOrientationsEnd() const
{
   return mMineOrientations.cend();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetThermalContrastsBegin()
{
   return mThermalContrasts.begin();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetThermalContrastsBegin() const
{
   return mThermalContrasts.cbegin();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetThermalContrastsEnd()
{
   return mThermalContrasts.end();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetThermalContrastsEnd() const
{
   return mThermalContrasts.cend();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetReflectancesBegin()
{
   return mReflectances.begin();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetReflectancesBegin() const
{
   return mReflectances.cbegin();
}

inline std::vector<DisFloat32>::iterator DisMinefieldData::GetReflectancesEnd()
{
   return mReflectances.end();
}

inline std::vector<DisFloat32>::const_iterator DisMinefieldData::GetReflectancesEnd() const
{
   return mReflectances.cend();
}

inline std::vector<DisMinefieldData::MineEmplacementTime>::iterator DisMinefieldData::GetMineEmplacementTimesBegin()
{
   return mMineEmplacementTimes.begin();
}

inline std::vector<DisMinefieldData::MineEmplacementTime>::const_iterator DisMinefieldData::GetMineEmplacementTimesBegin() const
{
   return mMineEmplacementTimes.cbegin();
}

inline std::vector<DisMinefieldData::MineEmplacementTime>::iterator DisMinefieldData::GetMineEmplacementTimesEnd()
{
   return mMineEmplacementTimes.end();
}

inline std::vector<DisMinefieldData::MineEmplacementTime>::const_iterator DisMinefieldData::GetMineEmplacementTimesEnd() const
{
   return mMineEmplacementTimes.cend();
}

inline std::vector<DisUint16>::iterator DisMinefieldData::GetMineEntityNumbersBegin()
{
   return mMineEntityNumbers.begin();
}

inline std::vector<DisUint16>::const_iterator DisMinefieldData::GetMineEntityNumbersBegin() const
{
   return mMineEntityNumbers.cbegin();
}

inline std::vector<DisUint16>::iterator DisMinefieldData::GetMineEntityNumbersEnd()
{
   return mMineEntityNumbers.end();
}

inline std::vector<DisUint16>::const_iterator DisMinefieldData::GetMineEntityNumbersEnd() const
{
   return mMineEntityNumbers.cend();
}

inline std::vector<DisUint16>::iterator DisMinefieldData::GetFusingsBegin()
{
   return mFusings.begin();
}

inline std::vector<DisUint16>::const_iterator DisMinefieldData::GetFusingsBegin() const
{
   return mFusings.cbegin();
}

inline std::vector<DisUint16>::iterator DisMinefieldData::GetFusingsEnd()
{
   return mFusings.end();
}

inline std::vector<DisUint16>::const_iterator DisMinefieldData::GetFusingsEnd() const
{
   return mFusings.cend();
}

inline std::vector<DisUint8>::iterator DisMinefieldData::GetScalarDetectionCoefficientsBegin()
{
   return mScalarDetectionCoefficients.begin();
}

inline std::vector<DisUint8>::const_iterator DisMinefieldData::GetScalarDetectionCoefficientsBegin() const
{
   return mScalarDetectionCoefficients.cbegin();
}

inline std::vector<DisUint8>::iterator DisMinefieldData::GetScalarDetectionCoefficientsEnd()
{
   return mScalarDetectionCoefficients.end();
}

inline std::vector<DisUint8>::const_iterator DisMinefieldData::GetScalarDetectionCoefficientsEnd() const
{
   return mScalarDetectionCoefficients.cend();
}

inline std::vector<DisEnum8>::iterator DisMinefieldData::GetPaintSchemesBegin()
{
   return mPaintSchemes.begin();
}

inline std::vector<DisEnum8>::const_iterator DisMinefieldData::GetPaintSchemesBegin() const
{
   return mPaintSchemes.cbegin();
}

inline std::vector<DisEnum8>::iterator DisMinefieldData::GetPaintSchemesEnd()
{
   return mPaintSchemes.end();
}

inline std::vector<DisEnum8>::const_iterator DisMinefieldData::GetPaintSchemesEnd() const
{
   return mPaintSchemes.cend();
}

inline std::vector<DisUint8>::iterator DisMinefieldData::GetTripDetonationWireCountsBegin()
{
   return mTripDetonationWireCounts.begin();
}

inline std::vector<DisUint8>::const_iterator DisMinefieldData::GetTripDetonationWireCountsBegin() const
{
   return mTripDetonationWireCounts.cbegin();
}

inline std::vector<DisUint8>::iterator DisMinefieldData::GetTripDetonationWireCountsEnd()
{
   return mTripDetonationWireCounts.end();
}

inline std::vector<DisUint8>::const_iterator DisMinefieldData::GetTripDetonationWireCountsEnd() const
{
   return mTripDetonationWireCounts.cend();
}

inline std::vector<DisUint8>::iterator DisMinefieldData::GetVertexCountsBegin()
{
   return mVertexCounts.begin();
}

inline std::vector<DisUint8>::const_iterator DisMinefieldData::GetVertexCountsBegin() const
{
   return mVertexCounts.cbegin();
}

inline std::vector<DisUint8>::iterator DisMinefieldData::GetVertexCountsEnd()
{
   return mVertexCounts.begin();
}

inline std::vector<DisUint8>::const_iterator DisMinefieldData::GetVertexCountsEnd() const
{
   return mVertexCounts.cend();
}

inline std::vector<UtVec3<DisFloat32>>::iterator DisMinefieldData::GetVerticesBegin()
{
   return mVertices.begin();
}

inline std::vector<UtVec3<DisFloat32>>::const_iterator DisMinefieldData::GetVerticesBegin() const
{
   return mVertices.cbegin();
}

inline std::vector<UtVec3<DisFloat32>>::iterator DisMinefieldData::GetVerticesEnd()
{
   return mVertices.end();
}

inline std::vector<UtVec3<DisFloat32>>::const_iterator DisMinefieldData::GetVerticesEnd() const
{
   return mVertices.cend();
}
#endif
