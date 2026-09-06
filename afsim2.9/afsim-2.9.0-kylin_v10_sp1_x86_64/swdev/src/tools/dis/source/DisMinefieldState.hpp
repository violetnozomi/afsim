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

#ifndef DISMINEFIELDSTATE_HPP
#define DISMINEFIELDSTATE_HPP

#include "dis_export.h"

#include <utility>
#include <vector>

#include "DisEntityAppearance.hpp"
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisPdu.hpp"
#include "DisTypes.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * The Minefield State PDU shall provide information about the complete minefield.
 * The minefield presence, perimeter, protocol mode, and types of mines contained within
 * the minefield shall be communicated through the Minefield State PDU.
 */
class DIS_EXPORT DisMinefieldState : public DisPdu
{
public:
   DisMinefieldState();
   DisMinefieldState(const DisMinefieldState& aSrc);
   DisMinefieldState(const DisPdu& aPdu, GenI& aGenI);
   explicit DisMinefieldState(GenI& aGenI);

   DisMinefieldState* Clone() const override;

   int GetClass() const override;

   // Accessors
   const DisEntityId&   GetOriginatingEntity() const override;
   const DisEntityId&   GetMineFieldId() const;
   DisUint32            GetRequestId() = delete;
   DisUint16            GetSequenceNumber() const;
   DisUint8             GetForceId() const;
   DisUint8             GetPerimeterPointsNumber() const;
   const DisEntityType& GetMineFieldType() const;
   DisUint16            GetMineTypesNumber() const;
   void                 GetMinefieldLocation(DisFloat64& aX, DisFloat64& aY, DisFloat64& aZ) const;
   void                 GetMinefieldOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisUint16            GetAppearance() const;
   DisUint16            GetProtocolMode() const;

   // Mutators
   void SetMinefieldId(const DisEntityId& aMinefieldId);
   void SetMinefieldSequenceNumber(DisUint16 aMinefieldSequenceNumber);
   void SetForceId(DisUint8 aForceId);
   void SetMinefieldType(const DisEntityType& aMinefieldType);
   void SetMinefieldLocation(DisFloat64 aX, DisFloat64 aY, DisFloat64 aZ);
   void SetMinefieldOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetAppearance(DisUint16 aAppearance);
   void SetProtocolMode(DisUint16 aProtocolMode);
   void AddPerimeterPointCoordinate(DisFloat32 aX, DisFloat32 aY);
   void AddMineType(DisEntityType& aType);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Return iterators to PerimeterPointCoordinate vector.
   std::vector<UtVec2<DisFloat32>>::iterator       GetPerimeterPointCoordinateBegin();
   std::vector<UtVec2<DisFloat32>>::const_iterator GetPerimeterPointCoordinateBegin() const;
   std::vector<UtVec2<DisFloat32>>::iterator       GetPerimeterPointCoordinateEnd();
   std::vector<UtVec2<DisFloat32>>::const_iterator GetPerimeterPointCoordinateEnd() const;

   // Return iterators to MineType vector.
   std::vector<DisEntityType>::iterator       GetMineTypeBegin();
   std::vector<DisEntityType>::const_iterator GetMineTypeBegin() const;
   std::vector<DisEntityType>::iterator       GetMineTypeEnd();
   std::vector<DisEntityType>::const_iterator GetMineTypeEnd() const;

private:
   void GetMemberData(GenI& aGenI);

   //! The minefield issuing the Minefield State PDU
   DisEntityId mMinefieldId = DisEntityId::ENTITY_ID_UNKNOWN;

   /**!
    * Specifies a change in state of a minefield as a result of a
    * change in minefield information or a change in the state,
    * of any of the mines contained therein
    */
   DisUint16 mMinefieldSequenceNumber = 0;

   //! The force to which the issuing minefield belongs
   DisUint8 mForceId = 0;

   //! The number of points in the perimeter of the minefield
   DisUint8 mPerimeterPointCount = 0;

   //! The minefield type of the issuing minefield
   DisEntityType mMinefieldType;

   //! The number of different mine types employed in the minefield
   DisUint16 mMineTypeCount = 0;

   //! The location of the center of the minefield
   UtVec3<DisFloat64> mMinefieldLocation;

   //! The orientation of the minefield
   UtVec3<DisFloat32> mMinefieldOrientation;

   /**!
    * The appearance information needed for displaying the
    * symbology of the minefield as a doctrinal minefield graphic
    */
   DisUint16 mAppearance = 0;

   /**!
    * Which protocol mode is being used to communicate the
    * minefield data
    */
   DisEnum16 mProtocolMode = 0;

   /**!
    * The (X,Y) location of each perimeter point, relative to
    * the Minefield Location field
    */
   std::vector<UtVec2<DisFloat32>> mPerimeterPointCoordinates;

   //! The type of each mine contained within the minefield
   std::vector<DisEntityType> mMineTypes;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 60;
};

// Accessors
inline const DisEntityId& DisMinefieldState::GetMineFieldId() const
{
   return mMinefieldId;
}

inline DisUint16 DisMinefieldState::GetSequenceNumber() const
{
   return mMinefieldSequenceNumber;
}

inline const DisEntityType& DisMinefieldState::GetMineFieldType() const
{
   return mMinefieldType;
}

inline DisUint16 DisMinefieldState::GetMineTypesNumber() const
{
   return mMineTypeCount;
}

inline void DisMinefieldState::GetMinefieldLocation(DisFloat64& aX, DisFloat64& aY, DisFloat64& aZ) const
{
   aX = mMinefieldLocation[0];
   aY = mMinefieldLocation[1];
   aZ = mMinefieldLocation[2];
}

inline void DisMinefieldState::GetMinefieldOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mMinefieldOrientation[0];
   aTheta = mMinefieldOrientation[1];
   aPhi   = mMinefieldOrientation[2];
}

inline DisUint16 DisMinefieldState::GetAppearance() const
{
   return mAppearance;
}

inline DisUint16 DisMinefieldState::GetProtocolMode() const
{
   return mProtocolMode;
}

inline DisUint8 DisMinefieldState::GetForceId() const
{
   return mForceId;
}

inline DisUint8 DisMinefieldState::GetPerimeterPointsNumber() const
{
   return mPerimeterPointCount;
}

// Mutators
inline void DisMinefieldState::SetMinefieldId(const DisEntityId& aMinefieldId)
{
   mMinefieldId = aMinefieldId;
}

inline void DisMinefieldState::SetMinefieldSequenceNumber(DisUint16 aMinefieldSequenceNumber)
{
   mMinefieldSequenceNumber = aMinefieldSequenceNumber;
}

inline void DisMinefieldState::SetForceId(DisUint8 aForceId)
{
   mForceId = aForceId;
}

inline void DisMinefieldState::SetMinefieldType(const DisEntityType& aMinefieldType)
{
   mMinefieldType = aMinefieldType;
}

inline void DisMinefieldState::SetMinefieldLocation(DisFloat64 aX, DisFloat64 aY, DisFloat64 aZ)
{
   mMinefieldLocation.Set(aX, aY, aZ);
}

inline void DisMinefieldState::SetMinefieldOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mMinefieldOrientation.Set(aPsi, aTheta, aPhi);
}

inline void DisMinefieldState::SetAppearance(DisUint16 aAppearance)
{
   mAppearance = aAppearance;
}

inline void DisMinefieldState::SetProtocolMode(DisUint16 aProtocolMode)
{
   mProtocolMode = aProtocolMode;
}

inline void DisMinefieldState::AddPerimeterPointCoordinate(DisFloat32 aX, DisFloat32 aY)
{
   mPerimeterPointCoordinates.emplace_back(aX, aY);
   mPerimeterPointCount++;
}

inline void DisMinefieldState::AddMineType(DisEntityType& aType)
{
   mMineTypes.push_back(aType);
   mMineTypeCount++;
}

inline std::string DisMinefieldState::GetStringId() const
{
   return "DisMinefieldState";
}

inline std::vector<UtVec2<DisFloat32>>::iterator DisMinefieldState::GetPerimeterPointCoordinateBegin()
{
   return mPerimeterPointCoordinates.begin();
}

inline std::vector<UtVec2<DisFloat32>>::const_iterator DisMinefieldState::GetPerimeterPointCoordinateBegin() const
{
   return mPerimeterPointCoordinates.begin();
}

inline std::vector<UtVec2<DisFloat32>>::iterator DisMinefieldState::GetPerimeterPointCoordinateEnd()
{
   return mPerimeterPointCoordinates.end();
}

inline std::vector<UtVec2<DisFloat32>>::const_iterator DisMinefieldState::GetPerimeterPointCoordinateEnd() const
{
   return mPerimeterPointCoordinates.end();
}

inline std::vector<DisEntityType>::iterator DisMinefieldState::GetMineTypeBegin()
{
   return mMineTypes.begin();
}

inline std::vector<DisEntityType>::const_iterator DisMinefieldState::GetMineTypeBegin() const
{
   return mMineTypes.begin();
}

inline std::vector<DisEntityType>::iterator DisMinefieldState::GetMineTypeEnd()
{
   return mMineTypes.end();
}

inline std::vector<DisEntityType>::const_iterator DisMinefieldState::GetMineTypeEnd() const
{
   return mMineTypes.end();
}
#endif
