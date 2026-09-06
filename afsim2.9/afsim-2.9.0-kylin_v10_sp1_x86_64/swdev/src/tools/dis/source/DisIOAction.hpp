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

#ifndef DISIOACTION_HPP
#define DISIOACTION_HPP

#include "dis_export.h"

#include <utility>
#include <vector>

#include "DisEntityId.hpp"
#include "DisIOBaseRecord.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisPtrContainer.hpp"
#include "DisRecordHeader.hpp"
#include "DisSimulationId.hpp"
#include "DisTypes.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * The IO Action PDU shall be used to communicate an IO attack or the effects of an IO attack on one or more
 * target entities. Targets include, but are not limited to, operational and communications centers,
 * communication nodes such as microwave towers and relay facilities, and other associated equipment and
 * links of communications networks. The information contained in the IO Action PDU is used by a receiving
 * simulation to model the effects of the IO attack on its entities.
 */
class DIS_EXPORT DisIOAction : public DisPdu
{
public:
   DisIOAction();
   DisIOAction(const DisIOAction& aSrc);
   DisIOAction(const DisPdu& aPdu, GenI& aGenI);
   explicit DisIOAction(GenI& aGenI);
   ~DisIOAction() override = default;

   DisIOAction* Clone() const override;
   void         Stream(std::ostream& aStream) const override;
   std::string  ToString() const override;
   std::string  PrintRecords() const;

   const char* GetScriptClassName() const override;

   // Get PDU Type and Family enums
   int GetClass() const override;
   int GetFamily() const override;

   // Accessors
   const DisEntityId&     GetOriginatingEntity() const override;
   const DisSimulationId& GetOriginSimulationId() const;
   const DisSimulationId& GetReceivingSimulationId() const;
   DisUint32              GetRequestId() const override;
   DisEnum16              GetIOWarfareType() const;
   DisEnum16              GetIOSimulationSource() const;
   DisEnum16              GetIOActionType() const;
   DisEnum16              GetIOActionPhase() const;
   const DisEntityId&     GetIOAttackerEntityId() const;
   const DisEntityId&     GetIOPrimaryTargetEntityId() const;
   DisUint16              GetRecordsCount() const;
   const DisIOBaseRecord* GetRecordAtIndex(int aIndex) const;

   // Mutators
   void SetOriginSimulationId(const DisSimulationId& aSimId);
   void SetReceivingSimulationId(const DisSimulationId& aSimId);
   void SetRequestId(DisUint32 aRequestId);
   void SetIOWarfareType(DisEnum16 aType);
   void SetIOSimulationSource(DisEnum16 aSource);
   void SetIOActionType(DisEnum16 aType);
   void SetIOActionPhase(DisEnum16 aPhase);
   void SetIOAttackerEntityId(const DisEntityId& aEntityId);
   void SetIOPrimaryTargetEntityId(const DisEntityId& aEntityId);

   bool AddRecord(DisIOBaseRecord* aRecord);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Return iterators to Records vector.
   std::vector<DisIOBaseRecord*>::iterator       GetRecordsBegin();
   std::vector<DisIOBaseRecord*>::const_iterator GetRecordsBegin() const;
   std::vector<DisIOBaseRecord*>::iterator       GetRecordsEnd();
   std::vector<DisIOBaseRecord*>::const_iterator GetRecordsEnd() const;

private:
   void PutRecords(GenO& aGenO);
   void GetRecords(GenI& aGenI);
   void GetMemberData(GenI& aGenI) override;

   //! The simulation that is issuing the PDU
   DisSimulationId mOriginSimulationId;
   //! The simulation to which this PDU is addressed, if applicable
   DisSimulationId mReceivingSimulationId;
   //! The request number for this IO Action PDU
   DisUint32 mRequestId = 0;
   //! The type of IO warfare
   DisEnum16 mIOWarfareType = 0;
   //! The name of the simulation model issuing this PDU
   DisEnum16 mIOSimulationSource = 0;
   //! The type of IO action
   DisEnum16 mIOActionType = 0;
   //! The phase of the IO action
   DisEnum16 mIOActionPhase = 0;
   //! The IO attacker entity
   DisEntityId mIOAttackerEntityID;
   //! The IO primary target entity
   DisEntityId mIOPrimaryTargetEntityID;
   //! Number of Records
   DisUint16 mRecordCount = 0;
   //! Records associated with this particular pdu
   DisPtrContainer<DisIOBaseRecord> mRecords;
   //! Base pdu length in octets (no IO Records)
   static constexpr DisUint16 cBASE_LENGTH_OCTETS = 44;
};
#endif
