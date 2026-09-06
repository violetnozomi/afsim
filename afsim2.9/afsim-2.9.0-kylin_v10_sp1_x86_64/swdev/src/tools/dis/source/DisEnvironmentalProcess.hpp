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

#ifndef DISENVIRONMENTALPROCESS_HPP
#define DISENVIRONMENTALPROCESS_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisEnvironmentalRecord.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"

/**!
 * Communicates information about the environment, including simple environment variables,
 * small - scale environmental updates, and embedded processes.An embedded process is an
 * environmental effect instigated by the action of an entity that may continue after that
 * entity has left the battlespace. Examples include contrails, smoke, obscurants, chaff,
 * dust clouds, and toxic chemicals. The environmental information communicated shall be sufficient for receiving
 * applications to represent the issued environment in the application’s own simulation
 */
class DIS_EXPORT DisEnvironmentalProcess : public DisPdu
{
public:
   DisEnvironmentalProcess();
   DisEnvironmentalProcess(const DisPdu& aPdu, GenI& aGenI);
   explicit DisEnvironmentalProcess(GenI& aGenI);
   DisEnvironmentalProcess(const DisEnvironmentalProcess& aSrc);
   ~DisEnvironmentalProcess() override = default;

   DisEnvironmentalProcess* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisEntityId&            GetEnvironmentalProcessId() const;
   const DisEntityType&          GetEnvironmentalType() const;
   DisEnum8                      GetModelType() const;
   DisEnum8                      GetEnvironmentalStatus() const;
   DisUint16                     GetNumberOfEnvironmentalRecords() const;
   DisUint16                     GetSequenceNumber() const;
   const DisEnvironmentalRecord* GetEntityPartAtIndex(int aIndex) const;

   // Setters
   void SetEnvironmentalProcessId(const DisEntityId& aEnvironmentalProcessId);
   void SetEnvironmentalType(const DisEntityType& aEnvironmentalType);
   void SetModelType(DisEnum8 aModelType);
   void SetEnvironmentalStatus(DisEnum8 aEnvironmentalStatus);
   void SetNumberOfEnvironmentalRecords(DisUint16 aNumberOfEnvironmentalRecords);
   void SetSequenceNumber(DisUint16 aSequenceNumber);

   // WARNING: AddEntityPart takes ownership of the pointed to object.
   void AddEntityPart(DisEnvironmentalRecord* aParameter);

   // WARNING: ReomoveEntityPart will delete the object pointer to.
   void RemoveEntityPart(DisEnvironmentalRecord* aParameter);
   void RemoveEntityPartNoDelete(DisEnvironmentalRecord* aParameter);
   void RemoveAllEntityParts();
   void RemoveAllEntityPartsNoDelete();

   // Return iterators to DisEntityPart vector.
   std::vector<DisEnvironmentalRecord*>::iterator       GetEnvironmentalRecordBegin();
   std::vector<DisEnvironmentalRecord*>::const_iterator GetEnvironmentalRecordBegin() const;
   std::vector<DisEnvironmentalRecord*>::iterator       GetEnvironmentalRecordEnd();
   std::vector<DisEnvironmentalRecord*>::const_iterator GetEnvironmentalRecordEnd() const;

   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The environmental process issuing the Environmental Process PDU
   DisEntityId mEnvironmentalProcessId;

   //! Identifies the type of environmental effect being described
   DisEntityType mEnvironmentalType;

   /**!
    * Specifies the particular model being used to generate this environmental condition or entity
    * Supposed to follow UID 248 from the SISO document  but the only value for this enumeration is 0
    */
   DisEnum8 mModelType = 0;

   /**!
    * This field shall identify the status of the environmental process
    * See UID 249 - two values:
    * IsLast = 0: Indicates that the current update shall be the last update for the specified process
    * IsActive = 1: Describes whether the environmental process is active or not
    */
   DisEnum8 mEnvironmentalStatus = 0;

   //! The number of Environment records contained in the variable portion of this PDU
   DisUint16 mNumberOfEnvironmentalRecords = 0;

   //! This field shall indicate the PDU sequence number for the environmental process
   DisUint16 mSequenceNumber = 0;

   //! List of associated Geometric or Environmental State records
   DisPtrContainer<DisEnvironmentalRecord> mEnvironmentalRecords;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 20;
};
#endif
