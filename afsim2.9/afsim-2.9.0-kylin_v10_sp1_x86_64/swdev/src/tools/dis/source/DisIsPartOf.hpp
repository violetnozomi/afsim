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

#ifndef DISISPARTOF_HPP
#define DISISPARTOF_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisNamedLocationIdentificationRecord.hpp"
#include "DisNatureEnum.hpp"
#include "DisPdu.hpp"
#include "DisStationNameEnum.hpp"

/**!
 * Used to request hierarchical linkage of separately hosted simulation entities where the
 * originating simulation application requests that the receiving simulation application honor a
 * respective host / part relationship.An entity that becomes a part of another entity is referred
 * to as a part entity from the simulation time it joins the host entity until it leaves the host
 * entity.When it leaves the host entity, it is again referred to as an entity.The originating
 * entity is also referred to as the host entity.
 */
class DIS_EXPORT DisIsPartOf : public DisPdu
{
public:
   DisIsPartOf();
   DisIsPartOf(const DisPdu& aPdu, GenI& aGenI);
   explicit DisIsPartOf(GenI& aGenI);
   DisIsPartOf(const DisIsPartOf& aSrc);

   DisIsPartOf* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisEntityId&  GetOriginatingSimulation() const;
   const DisEntityId&  GetReceivingEntity() const;
   const DisNatureEnum GetRelationshipNature() const;
   DisEnum16           GetRelationshipPosition() const;
   void GetPartLocation(DisFloat32& aPartLocationX, DisFloat32& aPartLocationY, DisFloat32& aPartLocationZ) const;
   const DisStationNameEnum                   GetStationName() const;
   DisUint16                                  GetStationNumber() const;
   const DisNamedLocationIdentificationRecord GetNamedLocationIdentificationRecord() const;
   const DisEntityType&                       GetPartType() const;

   // Setters
   void SetOriginatingSimulation(const DisEntityId& aOriginatingSimulation);
   void SetReceivingEntity(const DisEntityId& aReceivingEntity);
   void SetRelationshipNature(DisNatureEnum aRelationshipNature);
   void SetRelationshipPosition(DisEnum16 aRelationshipPosition);
   void SetPartLocation(DisFloat32 aPartLocationX, DisFloat32 aPartLocationY, DisFloat32 aPartLocationZ);
   void SetStationName(DisStationNameEnum aStationName);
   void SetStationNumber(DisUint16 aStationNumber);
   void SetNamedLocationIdentificationRecord(const DisNamedLocationIdentificationRecord& aNamedLocationIdentificationRecord);
   void SetNamedLocationIdentificationRecord(const DisStationNameEnum aStationName, const DisEnum16 aStationNumber);
   void SetPartType(const DisEntityType& aPartType);

   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   /**!
    * Originating Entity is the entity(host entity) that requests the Receiving Entity to
    * become a part of the host entity
    */
   DisEntityId mOriginatingSimulation = DisEntityId::ENTITY_ID_UNKNOWN;

   //! Receiving Entity is the entity requested to become a part of the Originating Entity
   DisEntityId mReceivedEntityId = DisEntityId::ENTITY_ID_UNKNOWN;

   // Relationship between the host entity and part entity
   /**!
    * The nature or purpose for joining of the part entity to the host entity
    * This is represented as a DisNature enumeration but passed as a DisEnum16
    */
   DisEnum16 mRelationshipNature = DisNatureEnum::cNATURE_OTHER;

   //! The position of the part entity with respect to the host entity
   DisEnum16 mRelationshipPosition = 0;

   //! Location of the part in the host entity’s coordinate system
   DisFloat32 mPartLocation[3];

   /**!
    * Information about the discrete positional relationship of the part entity with respect
    * to its host entity
    */
   DisNamedLocationIdentificationRecord mNamedLocationIdentificationRecord;

   //! The part type
   DisEntityType mPartType;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 40;
};
#endif
