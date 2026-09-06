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

#ifndef DISENTITYSTATE_HPP
#define DISENTITYSTATE_HPP

#include "dis_export.h"

#include <iostream>
#include <stdio.h>
#include <string>

#include "DisCapabilities.hpp"
#include "DisEntityAppearance.hpp"
#include "DisEntityId.hpp"
#include "DisEntityPart.hpp"
#include "DisEntityStateEnums.hpp"
#include "DisEntityType.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"

class GenI;
class GenO;

/**!
 * The Entity State PDU shall communicate information about an entity’s state. This includes state information
 * that is necessary for the receiving simulation applications to represent the issuing entity in the simulation
 * applications’ own simulation
 */
class DIS_EXPORT DisEntityState : public DisPdu
{
public:
   enum
   {
      Unused       = 0,
      ASCII        = 1,
      ArmyMarking  = 2,
      DigitChevron = 3
   };

   DisEntityState();
   DisEntityState(const DisEntityState& aSrc);
   DisEntityState(const DisPdu& aPdu, GenI& aGenI);
   explicit DisEntityState(GenI& aGenI);
   ~DisEntityState() override;

   DisEntityState* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;


   // Accessors
   const DisEntityId&         GetOriginatingEntity() const override;
   const DisEntityId&         GetEntityId() const;
   DisUint8                   GetForceId() const;
   DisUint8                   GetEntityPartCount() const;
   const DisEntityType&       GetEntityType() const;
   const DisEntityType&       GetAlternativeEntityType() const;
   void                       GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void                       GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const;
   void                       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisUint32                  GetAppearance() const;
   const DisEntityAppearance& GetAppearanceRecord() const;
   DisEntityAppearance&       GetAppearanceRecord();
   DisUint8                   GetDeadreckoningAlgorithm() const;
   void GetAcceleration(DisFloat32& aAccelerationX, DisFloat32& aAccelerationY, DisFloat32& aAccelerationZ) const;
   void GetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ) const;
   DisUint8               GetMarkingCharSet() const;
   void                   GetMarkingChars(DisUint8 aMarkingChars[11]) const;
   std::string            GetMarkingString() const; // Returns empty string if marking field is null
   DisUint32              GetCapabilities() const;
   const DisCapabilities& GetCapabilitiesRecord() const;

   // Mutators

   void SetEntityId(const DisEntityId& aEntityId);
   void SetForceId(DisUint8 aForceId);
   void SetEntityType(const DisEntityType& aEntityType);
   void SetAlternativeEntityType(const DisEntityType& aAlternativeEntityType);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetAppearance(DisUint32 aAppearance);
   void SetDeadreckoningAlgorithm(DisUint8 aDeadreckoningAlgorithm);
   void SetAcceleration(DisFloat32 aAccelerationX, DisFloat32 aAccelerationY, DisFloat32 aAccelerationZ);
   void SetAngularVelocity(DisFloat32 aAngularVelocityX, DisFloat32 aAngularVelocityY, DisFloat32 aAngularVelocityZ);
   void SetMarkingCharSet(DisUint8 aMarkingCharSet);
   void SetMarkingChars(DisUint8 aMarkingChars[11]);
   void SetCapabilities(DisUint32 aCapabilities);
   void SetCapabilitiesRecord(const DisCapabilities& aCapabilitiesRecord);

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;
   std::string         GetEntityPartCountStringValue(int aEnumValue) const;

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   // Articulated part list maintenance

   // WARNING: AddEntityPart takes ownership of the pointed to object.
   void AddEntityPart(DisEntityPart* aParameter);

   // WARNING: ReomoveEntityPart will delete the object pointer to.
   void RemoveEntityPart(DisEntityPart* aParameter);
   void RemoveAllEntityParts();

   const DisEntityPart* GetPartAtIndex(int aIndex);

   // Return iterators to DisEntityPart vector.
   std::vector<DisEntityPart*>::iterator       GetEntityPartBegin();
   std::vector<DisEntityPart*>::const_iterator GetEntityPartBegin() const;
   std::vector<DisEntityPart*>::iterator       GetEntityPartEnd();
   std::vector<DisEntityPart*>::const_iterator GetEntityPartEnd() const;

private:
   void GetMemberData(GenI& aGenI) override;
   /*!
    * Identifies the entity issuing the PDU
    */
   DisEntityId mEntityId;
   /*!
    * Specifies the ID of the force that the issuing entity belongs
    */
   DisUint8 mForceId;
   /*!
    * Specifies the number of entity parts, this is used as the number of variable
    * parameter records associated with this PDU.
    */
   DisUint8 mEntityPartCount;
   /*!
    * Identifies the type of entity to display by members of the same force as the issuing entity.
    */
   DisEntityType mEntityType;
   /*!
    * Identifies the entity type to be displayed by the members of forces other than
    * the issuing entity's force.
    */
   DisEntityType mAlternativeEntityType;
   /*!
    * Specifies the linear velocity of the entity. The coordinate system for an entity
    * depends on the dead reckoning algorithm used.
    */
   DisFloat32 mVelocity[3];
   /*!
    * Specifies the entity's physical location in the simulated world.
    */
   DisFloat64 mLocation[3];
   /*!
    * Specifies the entity's orientation.
    */
   DisFloat32 mOrientation[3];

   /*!
    * Specifies the dynamic changes to the entity's appearance attributes
    */
   DisEntityAppearance mAppearance;
   /*!
    * Specifies the dead reckoning algorithm used by the issuing entity
    */
   DisUint8 mDeadreckoningAlgorithm;
   // The following two variables (acceleration and angular velocity) are requirements of the dead reckoning algorithm
   /*!
    * Specifies the linear acceleration of the issuing entity
    */
   DisFloat32 mAcceleration[3];
   /*!
    * Specifies the angular velocity of the issuing entity
    */
   DisFloat32 mAngularVelocity[3];
   /*!
    * Identifies any unique markings on an entity
    */
   DisEnum8 mMarkingCharSet;
   /*!
    * The actual marking data for the entity
    */
   DisUint8 mMarkingChars[11];
   /*!
    * Specifies the entity's capabilities
    */
   DisCapabilities mCapabilities;
   /*!
    * Specifies the entity parts associated with the entity
    */
   DisPtrContainer<DisEntityPart> mEntityPartList;
};

// Accessors
/**
 * Gets the Entity State
 */
inline const DisEntityId& DisEntityState::GetEntityId() const
{
   return mEntityId;
}

/**
 * Gets the Force Id
 */
inline DisUint8 DisEntityState::GetForceId() const
{
   return mForceId;
}

/**
 * Gets the entity part count
 */
inline DisUint8 DisEntityState::GetEntityPartCount() const
{
   return mEntityPartCount;
}

/**
 * Gets the entity type
 */
inline const DisEntityType& DisEntityState::GetEntityType() const
{
   return mEntityType;
}

/**
 * Gets the alternate entity type
 */
inline const DisEntityType& DisEntityState::GetAlternativeEntityType() const
{
   return mAlternativeEntityType;
}

/**
 * Gets the velocity
 */
inline void DisEntityState::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

/**
 * Gets the location
 */
inline void DisEntityState::GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}

/**
 * Gets the orientation
 */
inline void DisEntityState::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

/**
 * Gets the appearance
 */
inline DisUint32 DisEntityState::GetAppearance() const
{
   return mAppearance.Value();
}

/**
 * Gets the appearance record
 */
inline const DisEntityAppearance& DisEntityState::GetAppearanceRecord() const
{
   return mAppearance;
}

inline DisEntityAppearance& DisEntityState::GetAppearanceRecord()
{
   return mAppearance;
}

/**
 * Gets the dead reckoning algorithm
 */
inline DisUint8 DisEntityState::GetDeadreckoningAlgorithm() const
{
   return mDeadreckoningAlgorithm;
}

/**
 * Gets the acceleration
 */
inline void DisEntityState::GetAcceleration(DisFloat32& aAccelerationX,
                                            DisFloat32& aAccelerationY,
                                            DisFloat32& aAccelerationZ) const
{
   aAccelerationX = mAcceleration[0];
   aAccelerationY = mAcceleration[1];
   aAccelerationZ = mAcceleration[2];
}

/**
 * Gets the angular velocity
 */
inline void DisEntityState::GetAngularVelocity(DisFloat32& aAngularVelocityX,
                                               DisFloat32& aAngularVelocityY,
                                               DisFloat32& aAngularVelocityZ) const
{
   aAngularVelocityX = mAngularVelocity[0];
   aAngularVelocityY = mAngularVelocity[1];
   aAngularVelocityZ = mAngularVelocity[2];
}

/**
 * Gets the char marking set
 */
inline DisUint8 DisEntityState::GetMarkingCharSet() const
{
   return mMarkingCharSet;
}

/**
 * Gets the marking chars
 */
inline void DisEntityState::GetMarkingChars(DisUint8 aMarkingChars[11]) const
{
   for (unsigned int i = 0; i < 11; ++i)
   {
      aMarkingChars[i] = mMarkingChars[i];
   }
}

/**
 * Gets the marking string
 */
inline std::string DisEntityState::GetMarkingString() const
{
   std::string markingStr;
   for (auto markingChar : mMarkingChars)
   {
      if (markingChar != '\0')
      {
         markingStr.push_back(markingChar);
      }
      else
      {
         break;
      }
   }
   return markingStr;
}

/**
 * Gets the capabilities
 */
inline DisUint32 DisEntityState::GetCapabilities() const
{
   return mCapabilities.Value();
}

inline const DisCapabilities& DisEntityState::GetCapabilitiesRecord() const
{
   return mCapabilities;
}

// Mutators

inline void DisEntityState::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}

inline void DisEntityState::SetForceId(DisUint8 aForceId)
{
   mForceId = aForceId;
}

inline void DisEntityState::SetEntityType(const DisEntityType& aEntityType)
{
   mEntityType = aEntityType;
}

inline void DisEntityState::SetAlternativeEntityType(const DisEntityType& aAlternativeEntityType)
{
   mAlternativeEntityType = aAlternativeEntityType;
}

inline void DisEntityState::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

inline void DisEntityState::SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

inline void DisEntityState::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

inline void DisEntityState::SetAppearance(DisUint32 aAppearance)
{
   mAppearance.Set(aAppearance);
}

inline void DisEntityState::SetDeadreckoningAlgorithm(DisUint8 aDeadreckoningAlgorithm)
{
   mDeadreckoningAlgorithm = aDeadreckoningAlgorithm;
}

inline void DisEntityState::SetAcceleration(DisFloat32 aAccelerationX, DisFloat32 aAccelerationY, DisFloat32 aAccelerationZ)
{
   mAcceleration[0] = aAccelerationX;
   mAcceleration[1] = aAccelerationY;
   mAcceleration[2] = aAccelerationZ;
}

inline void DisEntityState::SetAngularVelocity(DisFloat32 aAngularVelocityX,
                                               DisFloat32 aAngularVelocityY,
                                               DisFloat32 aAngularVelocityZ)
{
   mAngularVelocity[0] = aAngularVelocityX;
   mAngularVelocity[1] = aAngularVelocityY;
   mAngularVelocity[2] = aAngularVelocityZ;
}

inline void DisEntityState::SetMarkingCharSet(DisUint8 aMarkingCharSet)
{
   mMarkingCharSet = aMarkingCharSet;
}

inline void DisEntityState::SetMarkingChars(DisUint8 aMarkingChars[11])
{
   for (unsigned int i = 0; i < 11; ++i)
   {
      mMarkingChars[i] = aMarkingChars[i];
   }
}

inline void DisEntityState::SetCapabilities(DisUint32 aCapabilities)
{
   mCapabilities.Set(aCapabilities);
}

inline void DisEntityState::SetCapabilitiesRecord(const DisCapabilities& aCapabilities)
{
   mCapabilities.Set(aCapabilities.Value());
}

inline std::string DisEntityState::GetStringId() const
{
   return "ENTITY STATE";
}

// EntityPartList Functions

inline void DisEntityState::AddEntityPart(DisEntityPart* aPartPtr)
{
   mEntityPartList.Add(aPartPtr);
   mEntityPartCount = static_cast<DisUint8>(mEntityPartList.GetSize());
}

inline void DisEntityState::RemoveAllEntityParts()
{
   mEntityPartList.RemoveAll();
   mEntityPartCount = 0;
}

inline void DisEntityState::RemoveEntityPart(DisEntityPart* aPartPtr)
{
   mEntityPartList.Remove(aPartPtr);
   mEntityPartCount = static_cast<DisUint8>(mEntityPartList.GetSize());
}

inline const DisEntityPart* DisEntityState::GetPartAtIndex(int aIndex)
{
   return mEntityPartList[aIndex];
}

inline std::vector<DisEntityPart*>::iterator DisEntityState::GetEntityPartBegin()
{
   return mEntityPartList.GetBegin();
}

inline std::vector<DisEntityPart*>::const_iterator DisEntityState::GetEntityPartBegin() const
{
   return mEntityPartList.GetBegin();
}

inline std::vector<DisEntityPart*>::iterator DisEntityState::GetEntityPartEnd()
{
   return mEntityPartList.GetEnd();
}

inline std::vector<DisEntityPart*>::const_iterator DisEntityState::GetEntityPartEnd() const
{
   return mEntityPartList.GetEnd();
}

#endif
