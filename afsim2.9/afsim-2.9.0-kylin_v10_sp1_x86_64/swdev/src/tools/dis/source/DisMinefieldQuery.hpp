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

#ifndef DISMINEFIELDQUERY_HPP
#define DISMINEFIELDQUERY_HPP

#include "dis_export.h"

#include <utility>
#include <vector>

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * The Minefield Query PDU shall provide the means by which a simulation shall query
 * a minefield simulation for information on the individual mines contained within a
 * minefield when operating in QRP mode.
 */
class DIS_EXPORT DisMinefieldQuery : public DisPdu
{
public:
   DisMinefieldQuery();
   DisMinefieldQuery(const DisMinefieldQuery& aSrc);
   DisMinefieldQuery(const DisPdu& aPdu, GenI& aGenI);
   explicit DisMinefieldQuery(GenI& aGenI);

   DisMinefieldQuery* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Accessors
   const DisEntityId&   GetOriginatingEntity() const override;
   const DisEntityId&   GetMineFieldId() const;
   DisUint32            GetRequestId() = delete;
   const DisEntityId&   GetRequestingSimulationId() const;
   DisUint8             GetMinefieldRequestID() const;
   DisUint8             GetPerimeterPointsNumber() const;
   DisUint8             GetSensorTypesNumber() const;
   DisUint32            GetDataFilter() const;
   const DisEntityType& GetRequestedMineType() const;

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aOriginatingEntity);
   void SetMinefieldId(const DisEntityId& aMinefieldId);
   void SetRequestingSimulationId(const DisEntityId& aSimId);
   void SetMinefieldRequestID(DisUint8 aMinefieldRequestID);
   void SetPerimeterPointsNumber(DisUint8 aPerimeterPointsNumber);
   void SetSensorTypesNumber(DisUint8 aSensorTypesNumber);
   void SetDataFilter(DisUint32 aDataFilter);
   void SetRequestedMineType(const DisEntityType& aMineType);

   void AddPerimeterPointCoordinate(std::pair<DisFloat32, DisFloat32>& aPoint);
   void AddSensorType(DisUint16 aType);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Return iterators to PerimeterPointCoordinate vector.
   std::vector<std::pair<DisFloat32, DisFloat32>>::iterator       GetPerimeterPointCoordinateBegin();
   std::vector<std::pair<DisFloat32, DisFloat32>>::const_iterator GetPerimeterPointCoordinateBegin() const;
   std::vector<std::pair<DisFloat32, DisFloat32>>::iterator       GetPerimeterPointCoordinateEnd();
   std::vector<std::pair<DisFloat32, DisFloat32>>::const_iterator GetPerimeterPointCoordinateEnd() const;

   // Return iterators to MineType vector.
   std::vector<DisUint16>::iterator       GetSensorTypeBegin();
   std::vector<DisUint16>::const_iterator GetSensorTypeBegin() const;
   std::vector<DisUint16>::iterator       GetSensorTypeEnd();
   std::vector<DisUint16>::const_iterator GetSensorTypeEnd() const;


private:
   void GetMemberData(GenI& aGenI);

   //! The minefield to which this query is addressed
   DisEntityId mMinefieldId = DisEntityId::ENTITY_ID_UNKNOWN;

   /**!
    * The simulation that requested the information from the
    * minefield simulation.
    */
   DisEntityId mRequestingSimulationId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! The minefield query request
   DisUint8 mMinefieldRequestID = 0;

   /**!
    * The number of points in the perimeter of the area of interest
    * defined by the requesting entity.
    */
   DisUint8 mPerimeterPointCount = 0;

   //! The number of sensor types employed by the requesting simulation
   DisUint8 mSensorTypeCount = 0;

   /**!
    * Identify which of the optional fields in the Minefield Data PDU are
    * being requested.
    */
   DisUint32 mDataFilter = 0;

   //! The type of mine being queried by the requesting simulation
   DisEntityType mRequestedMineType;

   /**!
    * The (X,Y) location of each perimeter point in the requested area, relative to
    * the Minefield Location field from the Minefield State PDU
    */
   std::vector<std::pair<DisFloat32, DisFloat32>> mPerimeterPointCoordinates;

   //! The type of sensor that is requesting the data.
   std::vector<DisUint16> mSensorTypes;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 28;
};

inline int DisMinefieldQuery::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::MinefieldQuery);
}

inline int DisMinefieldQuery::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::Minefield);
}

inline const DisEntityId& DisMinefieldQuery::GetOriginatingEntity() const
{
   return mRequestingSimulationId;
}

// Accessors
inline const DisEntityId& DisMinefieldQuery::GetMineFieldId() const
{
   return mMinefieldId;
}

inline const DisEntityId& DisMinefieldQuery::GetRequestingSimulationId() const
{
   return mRequestingSimulationId;
}

inline DisUint8 DisMinefieldQuery::GetMinefieldRequestID() const
{
   return mMinefieldRequestID;
}

inline DisUint8 DisMinefieldQuery::GetPerimeterPointsNumber() const
{
   return mPerimeterPointCount;
}

inline DisUint8 DisMinefieldQuery::GetSensorTypesNumber() const
{
   return mSensorTypeCount;
}

inline DisUint32 DisMinefieldQuery::GetDataFilter() const
{
   return mDataFilter;
}

inline const DisEntityType& DisMinefieldQuery::GetRequestedMineType() const
{
   return mRequestedMineType;
}

inline void DisMinefieldQuery::SetOriginatingEntity(const DisEntityId& aOriginatingEntity)
{
   mMinefieldId = aOriginatingEntity;
}

// Mutators
inline void DisMinefieldQuery::SetMinefieldId(const DisEntityId& aMinefieldId)
{
   mMinefieldId = aMinefieldId;
}

inline void DisMinefieldQuery::SetRequestingSimulationId(const DisEntityId& aSimId)
{
   mRequestingSimulationId = aSimId;
}

inline void DisMinefieldQuery::SetMinefieldRequestID(DisUint8 aMinefieldRequestID)
{
   mMinefieldRequestID = aMinefieldRequestID;
}

inline void DisMinefieldQuery::SetPerimeterPointsNumber(DisUint8 aPerimeterPointsNumber)
{
   mPerimeterPointCount = aPerimeterPointsNumber;
}

inline void DisMinefieldQuery::SetSensorTypesNumber(DisUint8 aSensorTypesNumber)
{
   mSensorTypeCount = aSensorTypesNumber;
}

inline void DisMinefieldQuery::SetDataFilter(DisUint32 aDataFilter)
{
   mDataFilter = aDataFilter;
}

inline void DisMinefieldQuery::SetRequestedMineType(const DisEntityType& aMineType)
{
   mRequestedMineType = aMineType;
}

inline void DisMinefieldQuery::AddPerimeterPointCoordinate(std::pair<DisFloat32, DisFloat32>& aPoint)
{
   mPerimeterPointCoordinates.push_back(aPoint);
   mPerimeterPointCount++;
}

inline void DisMinefieldQuery::AddSensorType(DisUint16 aType)
{
   mSensorTypes.push_back(aType);
   mSensorTypeCount++;
}

inline std::string DisMinefieldQuery::GetStringId() const
{
   return "DisMinefieldQuery";
}

inline std::vector<std::pair<DisFloat32, DisFloat32>>::iterator DisMinefieldQuery::GetPerimeterPointCoordinateBegin()
{
   return mPerimeterPointCoordinates.begin();
}

inline std::vector<std::pair<DisFloat32, DisFloat32>>::const_iterator DisMinefieldQuery::GetPerimeterPointCoordinateBegin() const
{
   return mPerimeterPointCoordinates.begin();
}

inline std::vector<std::pair<DisFloat32, DisFloat32>>::iterator DisMinefieldQuery::GetPerimeterPointCoordinateEnd()
{
   return mPerimeterPointCoordinates.end();
}

inline std::vector<std::pair<DisFloat32, DisFloat32>>::const_iterator DisMinefieldQuery::GetPerimeterPointCoordinateEnd() const
{
   return mPerimeterPointCoordinates.end();
}

inline std::vector<DisEnum16>::iterator DisMinefieldQuery::GetSensorTypeBegin()
{
   return mSensorTypes.begin();
}

inline std::vector<DisEnum16>::const_iterator DisMinefieldQuery::GetSensorTypeBegin() const
{
   return mSensorTypes.begin();
}

inline std::vector<DisEnum16>::iterator DisMinefieldQuery::GetSensorTypeEnd()
{
   return mSensorTypes.end();
}

inline std::vector<DisEnum16>::const_iterator DisMinefieldQuery::GetSensorTypeEnd() const
{
   return mSensorTypes.end();
}
#endif
