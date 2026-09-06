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

#ifndef DISIOREPORT_HPP
#define DISIOREPORT_HPP

#include "dis_export.h"

#include <utility>
#include <vector>

#include "DisEntityId.hpp"
#include "DisIOBaseRecord.hpp"
#include "DisIOCommNodeRecord.hpp"
#include "DisIOCommunicationNodeIdRecord.hpp"
#include "DisIOEffectRecord.hpp"
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
 * The IO Report PDU shall be used to communicate the effects of an IO attack on one or more target entities.
 * Targets include, but are not limited to, operational and communications centers, communication nodes such
 * as microwave towers and relay facilities, and other associated equipment and links of communications
 * networks. The information contained in the IO Report PDU is used by a receiving simulation to determine
 * whether to continue an IO attack and to change attack parameters.
 */
class DIS_EXPORT DisIOReport : public DisPdu
{
public:
   DisIOReport();
   DisIOReport(const DisIOReport& aSrc);
   DisIOReport(const DisPdu& aPdu, GenI& aGenI);
   explicit DisIOReport(GenI& aGenI);
   ~DisIOReport() override = default;

   DisIOReport* Clone() const override;
   void         Stream(std::ostream& aStream) const override;
   std::string  ToString() const override;
   std::string  PrintRecords() const;

   const char* GetScriptClassName() const override;

   // Get PDU Type and Family enums
   int GetClass() const override;
   int GetFamily() const override;

   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors
   const DisSimulationId& GetOriginSimulationId() const;
   DisEnum16              GetIOSimulationSource() const;
   DisEnum8               GetIOReportType() const;
   const DisEntityId&     GetIOAttackerEntityId() const;
   const DisEntityId&     GetIOPrimaryTargetEntityId() const;
   DisUint16              GetRecordsCount() const;

   // Mutators
   void SetOriginSimulationId(const DisSimulationId& aSimId);
   void SetIOSimulationSource(DisEnum16 aSource);
   void SetIOReportType(DisEnum8 aType);
   void SetIOAttackerEntityId(const DisEntityId& aEntityId);
   void SetIOPrimaryTargetEntityId(const DisEntityId& aEntityId);

   bool                   AddRecord(DisIOBaseRecord* aRecord);
   const DisIOBaseRecord* GetRecordAtIndex(int aIndex);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Return iterators to Records vector.
   std::vector<DisIOBaseRecord*>::const_iterator GetRecordsBegin() const;
   std::vector<DisIOBaseRecord*>::iterator       GetRecordsBegin();
   std::vector<DisIOBaseRecord*>::const_iterator GetRecordsEnd() const;
   std::vector<DisIOBaseRecord*>::iterator       GetRecordsEnd();

private:
   void PutRecords(GenO& aGenO);
   void GetRecords(GenI& aGenI);
   void GetMemberData(GenI& aGenI) override;

   //! The simulation that is issuing the PDU
   DisSimulationId mOriginSimulationId;

   //! The name of the simulation model issuing this PDU
   DisEnum16 mIOSimulationSource = 0;

   //! The type of IO report
   DisEnum8 mIOReportType = 0;

   //! The IO attacker entity, if known
   DisEntityId mIOAttackerEntityID;

   //! The IO primary target entity
   DisEntityId mIOPrimaryTargetEntityID;

   //! Number of records
   DisUint16 mRecordCount = 0;

   //! Records associated with this pdu
   DisPtrContainer<DisIOBaseRecord> mRecords;

   //! Base pdu length in octets (no IO Records)
   static const DisUint16 cBASE_LENGTH_OCTETS = 28;
};
#endif
