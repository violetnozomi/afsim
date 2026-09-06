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

#ifndef DISENTITYSTATEUPDATE_HPP
#define DISENTITYSTATEUPDATE_HPP

#include "dis_export.h"

#include <memory>
#include <string>
#include <vector>

#include "DisEntityAppearance.hpp"
#include "DisEntityId.hpp"
#include "DisEntityPart.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisPtrContainer.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * Entity State Update PDU is a network bandwidth-reducing extension of the basic Entity State PDU and
 * may be used to communicate specific nonstatic information about an entity’s state
 */
class DIS_EXPORT DisEntityStateUpdate : public DisPdu
{
public:
   DisEntityStateUpdate();
   DisEntityStateUpdate(const DisEntityStateUpdate& aSrc);
   DisEntityStateUpdate(const DisPdu& aPdu, GenI& aGenI);
   explicit DisEntityStateUpdate(GenI& aGenI);

   DisEntityStateUpdate& operator=(const DisEntityStateUpdate&) = delete;
   DisEntityStateUpdate* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Accessors
   const DisEntityId&   GetOriginatingEntity() const override;
   const DisEntityId&   GetEntityId() const;
   DisUint8             GetVariableParameterRecordsCount() const;
   void                 GetLinearVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void                 GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const;
   void                 GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisUint32            GetAppearance() const;
   DisEntityAppearance& GetAppearanceRecord() const;

   // Mutators
   void SetEntityId(const DisEntityId& aEntityId);
   void SetLinearVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetAppearance(DisUint32 aAppearance);

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   // Articulated part list maintenance

   // WARNING: AddEntityPart takes ownership of the pointed to object.
   void AddVariableParameterRecord(DisEntityPart* aParameter);

   // WARNING: ReomoveEntityPart will delete the object pointer to.
   void RemoveVariableParameterRecord(DisEntityPart* aParameter);
   void RemoveAllVariableParameterRecords();

   const DisVariableParameterRecord* GetPartAtIndex(int aIndex);

   // Return iterators to DisEntityPart vector.
   std::vector<DisVariableParameterRecord*>::const_iterator GetVariableParameterRecordsBegin() const;
   std::vector<DisVariableParameterRecord*>::const_iterator GetVariableParameterRecordsEnd() const;

private:
   void GetMemberData(GenI& aGenI) override;
   void GetRecords(GenI& aGenI);
   void PutRecords(GenO& aGenO);

   //! Identifies the entity issuing the PDU
   DisEntityId mEntityId;

   DisUint8 mPadding = 0;

   //! Specifies the number of variable parameter records associated with the entity
   DisUint8 mVariableParameterRecordsCount = 0;

   //! Specifies an entity's linear velocity. Coordinate system depends on the dead reckoning algorithm used
   UtVec3<DisFloat32> mLinearVelocity;

   //! Specifies an entity's physical location in the simulated world is represented by a World Coordinates record (float array)
   UtVec3<DisFloat64> mLocation;

   //! Specifies an entity's orientation and is represented by a Euler Angles record (float array)
   UtVec3<DisFloat32> mOrientation;

   //! Specifies the dynamic changes to the entity's appearance attributes. Represented by an Entity Apperance record
   DisEntityAppearance mAppearance;

   //! List of entity parts associated with this PDU
   DisPtrContainer<DisVariableParameterRecord> mVariableParameterRecords;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 60;
};

inline int DisEntityStateUpdate::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::EntityStateUpdate);
}

inline int DisEntityStateUpdate::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::EntityInformationInteraction);
}

inline const DisEntityId& DisEntityStateUpdate::GetOriginatingEntity() const
{
   return mEntityId;
}

// Accessors
/**
 * Gets the Entity State
 */
inline const DisEntityId& DisEntityStateUpdate::GetEntityId() const
{
   return mEntityId;
}

/**
 * Gets the Variable Parameter Records Count
 */
inline DisUint8 DisEntityStateUpdate::GetVariableParameterRecordsCount() const
{
   return mVariableParameterRecordsCount;
}

/**
 * Gets the linear velocity
 */
inline void DisEntityStateUpdate::GetLinearVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mLinearVelocity[0];
   aVelocityY = mLinearVelocity[1];
   aVelocityZ = mLinearVelocity[2];
}

/**
 * Gets the location
 */
inline void DisEntityStateUpdate::GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}

/**
 * Gets the orientation
 */
inline void DisEntityStateUpdate::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

/**
 * Gets the appearance
 */
inline DisUint32 DisEntityStateUpdate::GetAppearance() const
{
   return mAppearance.Value();
}

/**
 * Gets the appearance record
 */
inline DisEntityAppearance& DisEntityStateUpdate::GetAppearanceRecord() const
{
   return const_cast<DisEntityAppearance&>(mAppearance);
}

// Mutators
inline void DisEntityStateUpdate::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}

inline void DisEntityStateUpdate::SetLinearVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mLinearVelocity[0] = aVelocityX;
   mLinearVelocity[1] = aVelocityY;
   mLinearVelocity[2] = aVelocityZ;
}

inline void DisEntityStateUpdate::SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

inline void DisEntityStateUpdate::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

inline void DisEntityStateUpdate::SetAppearance(DisUint32 aAppearance)
{
   mAppearance.Set(aAppearance);
}

inline std::string DisEntityStateUpdate::GetStringId() const
{
   return "DisEntityStateUpdate";
}

// EntityPartList Functions
inline void DisEntityStateUpdate::AddVariableParameterRecord(DisEntityPart* aPartPtr)
{
   mVariableParameterRecords.Add(aPartPtr);
   mVariableParameterRecordsCount = static_cast<DisUint8>(mVariableParameterRecords.GetSize());
   GetLength();
}

inline void DisEntityStateUpdate::RemoveAllVariableParameterRecords()
{
   mVariableParameterRecords.RemoveAll();
   mVariableParameterRecordsCount = 0;
   GetLength();
}

inline void DisEntityStateUpdate::RemoveVariableParameterRecord(DisEntityPart* aPartPtr)
{
   mVariableParameterRecords.Remove(aPartPtr);
   mVariableParameterRecordsCount = static_cast<DisUint8>(mVariableParameterRecords.GetSize());
   GetLength();
}

inline const DisVariableParameterRecord* DisEntityStateUpdate::GetPartAtIndex(int aIndex)
{
   const DisVariableParameterRecord* recordPtr{nullptr};
   if (aIndex >= 0 && aIndex < static_cast<int>(mVariableParameterRecords.GetSize()))
   {
      return mVariableParameterRecords[aIndex];
   }

   return recordPtr;
}

inline std::vector<DisVariableParameterRecord*>::const_iterator DisEntityStateUpdate::GetVariableParameterRecordsBegin() const
{
   return mVariableParameterRecords.GetBegin();
}

inline std::vector<DisVariableParameterRecord*>::const_iterator DisEntityStateUpdate::GetVariableParameterRecordsEnd() const
{
   return mVariableParameterRecords.GetEnd();
}

#endif
