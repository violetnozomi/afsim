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

#ifndef DISAGGREGATESTATE_HPP
#define DISAGGREGATESTATE_HPP

#include "DisAggregateTypeRecord.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSilentAggregateSystem.hpp"
#include "DisSilentEntitySystemRecord.hpp"
#include "DisVariableDatum.hpp"

/**!
 * Provides a capability for aggregating entities and communicating information about these aggregates of entities.It
 * also provides a mechanism for participants in a DIS exercise to request the level at which these entities are
 * represented. This PDU works in conjunction with the following PDUs:
 *   1. Action Request PDU
 *   2. Action Response PDU
 *   3. Event Report PDU
 */
class DIS_EXPORT DisAggregateState : public DisPdu
{
public:
   DisAggregateState();
   DisAggregateState(const DisPdu& aPdu, GenI& aGenI);
   DisAggregateState(GenI& aGenI);
   explicit DisAggregateState(const DisAggregateState& aSrc);
   ~DisAggregateState() override;

   DisAggregateState* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

   // Accessors
   const DisEntityId&            GetAggregateId() const;
   DisEnum8                      GetForceId() const;
   DisEnum8                      GetAggregateState() const;
   const DisAggregateTypeRecord& GetAggregateTypeRecord() const;
   DisEnum32                     GetFormation() const;
   const DisEnum8*               GetAggregateMarking() const;
   void      GetDimensions(DisFloat32& aDimensionX, DisFloat32& aDimensionY, DisFloat32& aDimensionZ) const;
   void      GetOrientation(DisFloat32& aOrientationX, DisFloat32& aOrientationY, DisFloat32& aOrientationZ) const;
   void      GetCenterOfMass(DisFloat64& aCenterOfMassX, DisFloat64& aCenterOfMassY, DisFloat64& aCenterOfMassZ) const;
   void      GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   DisUint16 GetNumberOfAggregateIds() const;
   DisUint16 GetNumberOfEntityIds() const;
   DisUint16 GetNumberOfSilentAggregateSystems() const;
   DisUint16 GetNumberOfSilentEntitySystems() const;
   const std::vector<DisEntityId>     GetAggregateIds() const;
   const std::vector<DisEntityId>     GetEntityIds() const;
   const DisSilentAggregateSystem&    GetSilentAggregateSystemAtIndex(DisUint16 aIndex) const;
   const DisSilentEntitySystemRecord& GetSilentEntitySystemAtIndex(DisUint16 aIndex) const;
   const DisVariableDatum&            GetVariableDatumAtIndex(DisUint16 aIndex) const;
   DisUint32                          GetNumberOfVariableDatumRecords() const;

   // Mutators
   void SetAggregateId(const DisEntityId& aAggregateId);
   void SetForceId(const DisEnum8 aForceId);
   void SetAggregateState(const DisEnum8 aAggregateState);
   void SetAggregateTypeRecord(const DisAggregateTypeRecord& aAggregateTypeRecord);
   void SetFormation(const DisEnum32 aFormation);
   void SetAggregateMarking(const DisEnum8* aAggregateMarking);
   void SetDimensions(DisFloat32 aDimensionX, DisFloat32 aDimensionY, DisFloat32 aDimensionZ);
   void SetOrientation(DisFloat32 aOrientationX, DisFloat32 aOrientationY, DisFloat32 aOrientationZ);
   void SetCenterOfMass(DisFloat64 aCenterOfMassX, DisFloat64 aCenterOfMassY, DisFloat64 aCenterOfMassZ);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetNumberOfAggregateIds(const DisUint16 aNumberOfAggregateIds);
   void SetNumberOfEntityIds(const DisUint16 aNumberOfEntityIds);
   void SetNumberOfSilentAggregateSystems(const DisUint16 aNumberOfSilentAggregateSystems);
   void SetNumberOfSilentEntitySystems(const DisUint16 aNumberfSilentEntitySystems);
   void SetAggregateIds(const std::vector<DisEntityId>& aAggregateIds);
   void SetEntityIds(const std::vector<DisEntityId>& aEntityIds);

   //! Add subaggregates that are transmitting Aggregate State PDUs
   void AddAggregateId(DisEntityId aAggregateID);

   //! Add constituent entities that are transmitting Entity State PDUs
   void AddEntityId(DisEntityId aEntityID);

   //! Add information about the subaggregates not producing Aggregate State PDUs
   void AddSilentAggregateSystem(DisSilentAggregateSystem* aSilentAggregateSystem);

   //! Add information about entities not producing Entity State PDUs
   void AddSilentEntitySystem(DisSilentEntitySystemRecord* aSilentEntitySystem);

   //! Add extra data that is used by the entry level and aggregate level simulations
   void AddVariableDatum(DisVariableDatum* aVariableDatum);

   // Return iterators to the DisSilentAggregateSystem vector.
   std::vector<DisSilentAggregateSystem*>::iterator       GetSilentAggregateSystemBegin();
   std::vector<DisSilentAggregateSystem*>::const_iterator GetSilentAggregateSystemBegin() const;
   std::vector<DisSilentAggregateSystem*>::iterator       GetSilentAggregateSystemEnd();
   std::vector<DisSilentAggregateSystem*>::const_iterator GetSilentAggregateSystemEnd() const;

   // Return iterators to the DisDETargetEnergyDepositionRecord vector.
   std::vector<DisSilentEntitySystemRecord*>::iterator       GetSilentEntitySystemBegin();
   std::vector<DisSilentEntitySystemRecord*>::const_iterator GetSilentEntitySystemBegin() const;
   std::vector<DisSilentEntitySystemRecord*>::iterator       GetSilentEntitySystemEnd();
   std::vector<DisSilentEntitySystemRecord*>::const_iterator GetSilentEntitySystemEnd() const;

private:
   void GetRecordData(GenI& aGenI);
   void PutRecordData(GenO& aGenO);
   void GetMemberData(GenI& aGenI) override;

   //! The aggregate issuing PDU
   DisEntityId mAggregateId;

   //! The common force to which the aggregate belongs
   DisEnum8 mForceId = 0;

   //! The state of the aggregate
   DisEnum8 mAggregateState = 0;

   //! The aggregate type
   DisAggregateTypeRecord mAggregateType;

   //! The formation of the aggregate
   DisEnum32 mFormation = 0;

   /**!
    * In the spec, this is a whole record on its own, but for implementation purposes,
    * the actual data being stored has been put here instead.
    */
   DisEnum8 mAggregateMarking[32];

   //! Specifies an entity's Dimensions and is represented by float array
   DisFloat32 mDimensions[3];

   //! Specifies an entity's orientation and is represented by a Euler Angles record (float array)
   DisFloat32 mOrientation[3];

   /**!
    * In the spec, this is a whole record on its own, but for implementation purposes,
    * the actual data being stored has been put here instead.
    */
   DisFloat64 mCenterOfMass[3];

   //! Specifies an entity's linear velocity. Coordinate system depends on the dead reckoning algorithm used
   DisFloat32 mVelocity[3];

   //! Specifies the number of subaggregates that are transmitting Aggregate State PDUs
   DisUint16 mNumberOfAggregateIds = 0;

   //! Specifies the number of constituent entities that are transmitting Entity State PDUs
   DisUint16 mNumberOfEntityIds = 0;

   //! Specifies the number of subaggregates that are not transmitting Aggregate State PDUs
   DisUint16 mNumberofSilentAggregateSystems = 0;

   //! Specifies the number of constutuent entity systems that are not transmitting Entity State PDUs
   DisUint16 mNumberOfSilentEntitySystems = 0;

   //! Specifies the subaggregates that are transmitting Aggregate State PDUs
   std::vector<DisEntityId> mAggregateIds;

   //! Specifies the constituent entities that are transmitting Entity State PDUs
   std::vector<DisEntityId> mEntityIds;

   //! Information about the subaggregates not producing Aggregate State PDUs
   DisPtrContainer<DisSilentAggregateSystem> mSilentAggregateSystems;

   //! Information about entities not producing Entity State PDUs
   DisPtrContainer<DisSilentEntitySystemRecord> mSilentEntitySystems;

   //! Specifies the number of variable datum records to follow
   DisUint32 mNumberOfVariableDatumRecords = 0;

   //! Specifies extra data that is used by the entry level and aggregate level simulations to transfer control and
   //! correlate the simulation of entities in an aggregate.
   DisPtrContainer<DisVariableDatum> mVariableDatumRecords;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 124;
};
#endif
