//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISISGROUPOF_HPP
#define DISISGROUPOF_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisGroupEntityDescriptionRecord.hpp"
#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"

/**!
 * Communicates information about the individual states of a group of entities including
 * state information that is necessary for the receiving simulation applications to represent the issuing
 * group of entities in the simulation applications’ own simulation.
 *
 * Only entities originated by the issuing simulation are eligible to be contained in
 * this PDU due to the dependency on a master Site Number and Application Number for all group entities.
 *
 * This also means that a local entity with a different Site Number and Application Number from that of the Group
 * Entity Identifier Site Number and Application Number is not eligible to be included in this PDU.
 *
 * This condition is normally the result of an ownership transfer.
 */
class DIS_EXPORT DisIsGroupOf : public DisPdu
{
public:
   DisIsGroupOf();
   DisIsGroupOf(const DisPdu& aPdu, GenI& aGenI);
   explicit DisIsGroupOf(GenI& aGenI);
   DisIsGroupOf(const DisIsGroupOf& aSrc);

   DisIsGroupOf* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisEntityId&                       GetGroupId() const;
   DisEnum8                                 GetGroupedEntityCategory() const;
   DisUint8                                 GetNumberOfGroupedEntities() const;
   void                                     GetGroupLocation(DisFloat64& aLatitude, DisFloat64& aLongitude) const;
   const DisGroupedEntityDescriptionRecord* GetGroupEntityDescriptionAtIndex(DisUint16 aIndex) const;

   // Setters
   void SetGroupId(const DisEntityId& aGroupId);
   void SetGroupedEntityCategory(DisGroupEntityCategoryEnum aCategory);
   void SetNumberOfGroupedEntities(DisUint8 aNumberofGroupedEntities);
   void SetGroupLocation(DisFloat64 aLatitude, DisFloat64 aLongitude);
   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

   // WARNING: AddEntityPart takes ownership of the pointed to object.
   void AddGroupEntityDescription(DisGroupedEntityDescriptionRecord* aParameter);

   // WARNING: RemoveEntityPart will delete the object pointer too.
   void RemoveGroupEntityDescription(DisGroupedEntityDescriptionRecord* aParameter);
   void RemoveAllGroupEntityDescriptions();
   void RemoveGroupEntityDescriptionNoDelete(DisGroupedEntityDescriptionRecord* aParameter);
   void RemoveAllGroupEntityDescriptionsNoDelete();

   // Return iterators to DisEntityPart vector.
   std::vector<DisGroupedEntityDescriptionRecord*>::iterator       GetGroupEntityDescriptionRecordsBegin();
   std::vector<DisGroupedEntityDescriptionRecord*>::const_iterator GetGroupEntityDescriptionRecordsBegin() const;
   std::vector<DisGroupedEntityDescriptionRecord*>::iterator       GetGroupEntityDescriptionRecordsEnd();
   std::vector<DisGroupedEntityDescriptionRecord*>::const_iterator GetGroupEntityDescriptionRecordsEnd() const;

private:
   void        GetMemberData(GenI& aGenI) override;
   void        PutSpecificRecord(GenO& aGenO);
   void        GetSpecificRecord(GenI& aGenI);
   DisUint16   GetSpecificRecordLength();
   std::string SpecificRecordToString() const;
   bool        ValidateSpecificRecord() const;

   //! Identifies the entity that represents the group of entities
   DisEntityId mGroupId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! Specifies the entity that represents the group of entities
   DisEnum8 mGroupedEntityCategory = DisGroupEntityCategoryEnum::cUNDEFINED;

   //! Describes the number of individual entities constituting the group
   DisUint8 mNumberOfGroupedEntities = 0;

   //! PDU padding
   DisFloat32 mPadding = 0;

   /*!
    * Specifies the location of the group in latitude,longitude (radians) that is used as a reference point
    * from which the locations of all other grouped entities are based. The third coordinate of the
    * Reference Point, which will not be transmitted in this PDU, is defined to be 100 m below
    * Adjusted Mean Sea Level to compensate for the lowest surface point on the Earth.
    */
   DisFloat64 mGroupLocation[2];

   //! GED records that specify information about each entity within the group.
   DisPtrContainer<DisGroupedEntityDescriptionRecord> mGroupEntityDescription;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 28;
};
#endif
