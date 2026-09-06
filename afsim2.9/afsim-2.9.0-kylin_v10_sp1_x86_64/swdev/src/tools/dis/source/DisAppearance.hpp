// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISAPPEARANCE_HPP
#define DISAPPEARANCE_HPP

#include "dis_export.h"

#include <array>

#include "DisCapabilities.hpp"
#include "DisEntityAppearance.hpp"
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisLiveEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

/**!
 * DisAppearance - This PDU communicates information about the appearance of an Live Entity. This includes state
 * information that is necessary for the receiving simulation applications to represent the issuing entity’s
 * appearance in the simulation application’s own simulation.
 */
class DIS_EXPORT DisAppearance : public DisPdu
{
public:
   DisAppearance();
   DisAppearance(const DisAppearance& aSrc);
   DisAppearance(const DisPdu& aPdu, GenI& aGenI);
   explicit DisAppearance(GenI& aGenI);

   DisAppearance* Clone() const override;

   virtual std::string ToString() const override;

   // Get PDU Type and Family enums
   int GetClass() const override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Accessors
   const DisLiveEntityId&     GetLiveEntityId() const;
   DisUint8                   GetAppearanceFlag1() const;
   DisUint8                   GetAppearanceFlag2() const;
   DisEnum8                   GetForceId() const;
   const DisEntityType&       GetEntityType() const;
   const DisEntityType&       GetAlternateEntityType() const;
   DisUint8                   GetMarkingCharSet() const;
   void                       GetMarkingChars(DisUint8 aMarkingChars[11]) const;
   std::string                GetMarkingString() const; // Returns empty string if marking field is null
   DisUint32                  GetCapabilities() const;
   const DisCapabilities&     GetCapabilitiesRecord();
   const DisEntityAppearance& GetAppearanceVisual() const;
   const DisEntityAppearance& GetAppearanceIR() const;
   const DisEntityAppearance& GetAppearanceEM() const;
   const DisEntityAppearance& GetAppearanceAudio() const;

   // Mutators
   void SetLiveEntityId(const DisLiveEntityId& aId);
   void SetAppearanceFlag1(DisUint8 aFlag);
   void SetAppearanceFlag2(DisUint8 aFlag);
   void SetForceId(DisEnum8 aForceId);
   void SetEntityType(const DisEntityType& aType);
   void SetAlternateEntityType(const DisEntityType& aType);
   void SetMarkingCharSet(DisUint8 aMarkingCharSet);
   void SetMarkingChars(DisUint8 aMarkingChars[11]);
   void SetCapabilities(DisUint32 aCapabilities);
   void SetCapabilitiesRecord(const DisCapabilities& aCapabilitiesRecord);
   void SetAppearanceVisual(const DisEntityAppearance& aRecord);
   void SetAppearanceIR(const DisEntityAppearance& aRecord);
   void SetAppearanceEM(const DisEntityAppearance& aRecord);
   void SetAppearanceAudio(const DisEntityAppearance& aRecord);

private:
   void GetMemberData(GenI& aGenI) override;

   /**!
    * This field shall identify those optional data fields that are being transmitted with
    * the current PDU.
    */
   DisLiveEntityId mEntityID;

   //! Appearance Flag 1
   /*!
    *  This field shall identify those optional data fields that are being transmitted with
    *  the current PDU.
    *
    *  ---------------------------------------------------------------
    *  |      |          |                                             |
    *  | Bit  |  State   |Flag Octet 1 Meaning                         |
    *  |      |          |                                             |
    *  |---------------------------------------------------------------|
    *  |   7  | 0        | Flag Octet 2 not included                   |
    *  |      | 1        | Flag Octet 2 is included                    |
    *  |---------------------------------------------------------------
    *  |   6  | 0        | Appearance-IR field not included            |
    *  |      | 1        | Appearance-IR field is included             |
    *  |---------------------------------------------------------------
    *  |   5  | 0        | Appearance-Visual field not included        |
    *  |      | 1        | Appearance-Visual field is included         |
    *  |---------------------------------------------------------------
    *  |   4  | 0        | Capabilities field not included             |
    *  |      | 1        | Capabilities field is included              |
    *  |---------------------------------------------------------------
    *  |   3  | 0        | Entity Marking field not included           |
    *  |      | 1        | Entity Marking field is included            |
    *  |---------------------------------------------------------------
    *  |   2  | 0        | Alternate Entity Type field not included    |
    *  |      | 1        | Alternate Entity Type field is included     |
    *  |---------------------------------------------------------------
    *  |   1  | 0        | Entity Type field not included              |
    *  |      | 1        | Entity Type field is included               |
    *  |---------------------------------------------------------------
    *  |   0  | 0        | Force ID field not included                 |
    *  |      | 1        | Force ID field is included                  |
    *  |---------------------------------------------------------------
    */
   DisUint8 mAppearanceFlag1 = 0;

   //! Appearance Flag 2
   /*!
    *  This field shall identify those optional data fields that are being transmitted with
    *  the current PDU.
    *
    *  ---------------------------------------------------------------
    *  |      |          |                                             |
    *  | Bit  |  State   |Flag Octet 2 Meaning                         |
    *  |      |          |                                             |
    *  |---------------------------------------------------------------|
    *  |   7  | 0        | Reserved for flag continuation flag         |
    *  |---------------------------------------------------------------
    *  |   6  | 0        | Unused                                      |
    *  |---------------------------------------------------------------
    *  |   5  | 0        | Unused                                      |
    *  |---------------------------------------------------------------
    *  |   4  | 0        | Unused                                      |
    *  |---------------------------------------------------------------
    *  |   3  | 0        | Unused                                      |
    *  |---------------------------------------------------------------
    *  |   2  | 0        | Unused                                      |
    *  |---------------------------------------------------------------
    *  |   1  | 0        | Appearance-Audio field not included         |
    *  |      | 1        | Appearance-Audio field is included          |
    *  |---------------------------------------------------------------
    *  |   0  | 0        | Appearance-EM field not included            |
    *  |      | 1        | Appearance-EM field is included             |
    *  |---------------------------------------------------------------
    */
   DisUint8 mAppearanceFlag2 = 0;

   //! This optional field shall identify the force to which the issuing entity belongs
   DisEnum8 mForceID = 0;

   /**!
    * This optional field shall identify the entity type to be displayed by members of the same
    * force as the issuing entity.
    */
   DisEntityType mEntityType;

   /**!
    * This optional field shall identify the entity type to be displayed by members
    * of forces other than that of the issuing entity
    */
   DisEntityType mAlternateEntityType;

   //! Identifies any unique markings on the issuing entity (Marking record - 8 bit enumeration)
   DisEnum8 mMarkingCharSet;

   DisUint8 mMarkingChars[11];

   //! Specifies the entity's capabilities
   DisCapabilities mCapabilities;

   //! 32 bit Entity Appearance Records
   struct
   {
      /**!
       * Field shall specify the dynamic changes to the entity’s visual
       * appearance attributes.
       */
      DisEntityAppearance Visual;

      /**!
       * Field shall specify the dynamic changes to the entity’s infrared
       * appearance attributes.
       */
      DisEntityAppearance IR;

      /**!
       * Field shall specify the dynamic changes to the entity’s
       * electromagnetic appearance attributes.
       */
      DisEntityAppearance EM;

      /**!
       * Field shall specify the dynamic changes to the entity’s acoustic
       * appearance attributes.
       */
      DisEntityAppearance Audio;
   } mAppearance;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 5;
};
inline int DisAppearance::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::Appearance);
}

inline const DisLiveEntityId& DisAppearance::GetLiveEntityId() const
{
   return mEntityID;
}
inline DisUint8 DisAppearance::GetAppearanceFlag1() const
{
   return mAppearanceFlag1;
}
inline DisUint8 DisAppearance::GetAppearanceFlag2() const
{
   return mAppearanceFlag2;
}
inline DisEnum8 DisAppearance::GetForceId() const
{
   return mForceID;
}
inline const DisEntityType& DisAppearance::GetEntityType() const
{
   return mEntityType;
}
inline const DisEntityType& DisAppearance::GetAlternateEntityType() const
{
   return mAlternateEntityType;
}
/**
 * Gets the capabilities
 */
inline DisUint32 DisAppearance::GetCapabilities() const
{
   return mCapabilities.Value();
}

inline const DisCapabilities& DisAppearance::GetCapabilitiesRecord()
{
   return mCapabilities;
}

inline const DisEntityAppearance& DisAppearance::GetAppearanceVisual() const
{
   return mAppearance.Visual;
}

inline const DisEntityAppearance& DisAppearance::GetAppearanceIR() const
{
   return mAppearance.IR;
}

inline const DisEntityAppearance& DisAppearance::GetAppearanceEM() const
{
   return mAppearance.EM;
}

inline const DisEntityAppearance& DisAppearance::GetAppearanceAudio() const
{
   return mAppearance.Audio;
}

inline DisUint8 DisAppearance::GetMarkingCharSet() const
{
   return mMarkingCharSet;
}

inline void DisAppearance::GetMarkingChars(DisUint8 aMarkingChars[11]) const
{
   for (unsigned int i = 0; i < 11; ++i)
   {
      aMarkingChars[i] = mMarkingChars[i];
   }
}

inline std::string DisAppearance::GetMarkingString() const
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

inline std::string DisAppearance::GetStringId() const
{
   return "APPEARANCE";
}

inline void DisAppearance::SetLiveEntityId(const DisLiveEntityId& aId)
{
   mEntityID = aId;
}

inline void DisAppearance::SetAppearanceFlag1(DisUint8 aFlag)
{
   mAppearanceFlag1 = aFlag;
}

inline void DisAppearance::SetAppearanceFlag2(DisUint8 aFlag)
{
   mAppearanceFlag2 = aFlag;
}

inline void DisAppearance::SetForceId(DisEnum8 aForceId)
{
   mForceID = aForceId;
}

inline void DisAppearance::SetEntityType(const DisEntityType& aType)
{
   mEntityType = aType;
}

inline void DisAppearance::SetAlternateEntityType(const DisEntityType& aType)
{
   mAlternateEntityType = aType;
}

inline void DisAppearance::SetMarkingCharSet(DisUint8 aMarkingCharSet)
{
   mMarkingCharSet = aMarkingCharSet;
}

inline void DisAppearance::SetMarkingChars(DisUint8 aMarkingChars[11])
{
   for (unsigned int i = 0; i < 11; ++i)
   {
      mMarkingChars[i] = aMarkingChars[i];
   }
}
inline void DisAppearance::SetCapabilities(DisUint32 aCapabilities)
{
   mCapabilities.Set(aCapabilities);
}
inline void DisAppearance::SetCapabilitiesRecord(const DisCapabilities& aCapabilities)
{
   mCapabilities.Set(aCapabilities.Value());
}

inline void DisAppearance::SetAppearanceVisual(const DisEntityAppearance& aRecord)
{
   mAppearance.Visual = aRecord;
}
inline void DisAppearance::SetAppearanceIR(const DisEntityAppearance& aRecord)
{
   mAppearance.IR = aRecord;
}
inline void DisAppearance::SetAppearanceEM(const DisEntityAppearance& aRecord)
{
   mAppearance.EM = aRecord;
}
inline void DisAppearance::SetAppearanceAudio(const DisEntityAppearance& aRecord)
{
   mAppearance.Audio = aRecord;
}
#endif
