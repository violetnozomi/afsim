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

#ifndef DISLINEAROBJECTSTATE_HPP
#define DISLINEAROBJECTSTATE_HPP

#include "dis_export.h"

#include "DisAppearance.hpp"
#include "DisEntityType.hpp"
#include "DisLinearSegmentParameterRecord.hpp"
#include "DisObjectType.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSimulationAddressRecord.hpp"

/*!
 * The Linear Object State PDU shall communicate the addition/modification of a synthetic environment
 * object that is geometrically anchored to the terrain with one point and has a segment size and orientation.
 */
class DIS_EXPORT DisLinearObjectState : public DisPdu
{
public:
   DisLinearObjectState();
   DisLinearObjectState(const DisLinearObjectState& aSrc);
   DisLinearObjectState(const DisPdu& aPdu, GenI& aGenI);
   ~DisLinearObjectState() = default;

   DisLinearObjectState* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisEntityId                                 GetObjectId() const;
   const DisEntityId                                 GetReferencedObjectId() const;
   DisUint16                                         GetUpdateNumber() const;
   DisEnum8                                          GetForceId() const;
   DisUint8                                          GetNumberOfLinearSegments() const;
   const DisSimulationAddressRecord                  GetRequesterSimulationId() const;
   const DisSimulationAddressRecord                  GetReceivingSimulationId() const;
   const DisObjectType                               GetObjectType() const;
   DisPtrContainer<DisLinearSegmentParameterRecord>* GetLinearSegmentParameterRecords();
   const DisLinearSegmentParameterRecord*            GetLinearSegmentParameterRecordAtIndex(int aIndex);

   // Setters
   void SetObjectId(const DisEntityId& aObjectId);
   void SetReferenceObjectId(const DisEntityId& aObjectReferenceId);
   void SetUpdateNumber(DisUint16 aUpdateNumber);
   void SetForceId(DisEnum8 aForceId);
   void SetNumberOfLinearSegments(DisEnum8 aNumberOfLineSegments);
   void SetObjectType(const DisObjectType& aObjectType);
   void SetRequesterSimulationId(const DisSimulationAddressRecord& aRequesterSimulationId);
   void SetReceivingSimulationId(const DisSimulationAddressRecord& aRecievingSimulationId);

   void AddLinearSegmentParameterRecord(DisLinearSegmentParameterRecord* aLinearSegmentParameterRecord);

   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! This field shall specify the unique identification of the object in the synthetic environment.
   DisEntityId mObjectId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! This field shall identify the synthetic environment object with which this point object is associated.
   DisEntityId mReferencedObjectId = DisEntityId::ENTITY_ID_UNKNOWN;

   /*!
    * This field shall represent the unique update number, starting with 1,
    * of each state transition of an individual object.
    */
   DisUint16 mUpdateNumber = 0;

   //! This field shall identify the force that created or modified the object.
   DisEnum8 mForceId = 0;

   /*!
    * This field shall specify the number of Linear Segment Parameter
    * records required for the specification of the linear object
    */
   DisUint8 mNumberOfLinearSegments = 0;

   /*!
    * This field shall specify the simulation application that is sending or has sent
    * the Point Object State PDU to the Environment Manager
    */
   DisSimulationAddressRecord mRequesterSimulationId;

   /*!
    * This field shall specify the simulation application that is to receive the
    * Point Object State PDU
    */
   DisSimulationAddressRecord mReceivingSimulationId;

   /*!
    * This field shall identify the type of synthetic environment object.
    */
   DisObjectType mObjectType;

   /*!
    * This field shall specify the parameter values for representing each
    * linear segment.
    */
   DisPtrContainer<DisLinearSegmentParameterRecord> mLinearSegmentParameters;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 28;
};
#endif
