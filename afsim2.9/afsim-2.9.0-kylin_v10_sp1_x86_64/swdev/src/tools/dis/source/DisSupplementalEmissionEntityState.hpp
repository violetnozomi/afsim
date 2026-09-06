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

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPropulsionSystemDataRecord.hpp"
#include "DisPtrContainer.hpp"
#include "DisVectoringNozzleSystemDataRecord.hpp"

#ifndef DISSUPPLEMENTALEMISSIONENTITYSTATE_HPP
#define DISSUPPLEMENTALEMISSIONENTITYSTATE_HPP

//! The SEES PDU shall be used to communicate supplemental information concerning passive emissions
//! (infrared and radar cross section) and active emissions(acoustic), along with additional information
//! pertaining to propulsion systems contained within the entity.
class DIS_EXPORT DisSupplementalEmissionEntityState : public DisPdu
{
public:
   DisSupplementalEmissionEntityState();
   DisSupplementalEmissionEntityState(const DisSupplementalEmissionEntityState& aSrc);
   DisSupplementalEmissionEntityState(const DisPdu& aPdu, GenI& aGenI);
   explicit DisSupplementalEmissionEntityState(GenI& aGenI);

   DisSupplementalEmissionEntityState* Clone() const override;

   int                GetClass() const override;
   int                GetFamily() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors
   const DisEntityId& GetEntityId() const;
   DisUint16          GetInfraredSignatureRepresentationIndex() const;
   DisUint16          GetAcousticSignatureRepresentationIndex() const;
   DisUint16          GetRadarCrossSectionRepresentationIndex() const;
   DisUint16          GetNumberOfPropulsionSystems() const;
   DisUint16          GetNumberOfVectoringNozzleSystems() const;

   const DisVectoringNozzleSystemDataRecord* GetVectoringNozzleSystemDataRecordAtIndex(int aIndex) const;
   const DisPropulsionSystemDataRecord*      GetPropulsionSystemDataRecordAtIndex(int aIndex) const;

   // return iterators to the propulsion systems pointer container and vector nozzle container
   std::vector<DisPropulsionSystemDataRecord*>::iterator            GetPropulsionSystemBegin();
   std::vector<DisPropulsionSystemDataRecord*>::const_iterator      GetPropulsionSystemBegin() const;
   std::vector<DisVectoringNozzleSystemDataRecord*>::iterator       GetVectoringNozzleSystemBegin();
   std::vector<DisVectoringNozzleSystemDataRecord*>::const_iterator GetVectoringNozzleSystemBegin() const;

   // Mutators
   void SetEntityId(const DisEntityId& aEntityId);
   void SetInfraredSignatureRepresentationIndex(DisUint16 aInfraredSignatureRepresentationIndex);
   void SetAcousticSignatureRepresentationIndex(DisUint16 aAcousticSignatureRepresentationIndex);
   void SetRadarCrossSectionRepresentationIndex(DisUint16 aRadarCrossSectionRepresentationIndex);
   void SetNumberOfPropulsionSystems(DisUint16 aNumberOfPropulsionSystems);
   void SetNumberOfVectoringNozzleSystems(DisUint16 aNumberOfVectoringNozzleSystems);

   // Propulsion system maintenance
   void         AddPropulsionSystemRecord(DisPropulsionSystemDataRecord* aPropulsionSystemRecord);
   unsigned int RemovePropulsionSystemRecord(DisPropulsionSystemDataRecord* aPropulsionSystemRecord);
   void         RemoveAllPropulsionSystemRecords();
   unsigned int RemovePropulsionSystemRecordWithNoDelete(DisPropulsionSystemDataRecord* aPropulsionSystemRecord);
   void         RemoveAllPropulsionSystemRecordsWithNoDelete();


   // Vector nozzle maintenance
   void         AddVectoringNozzleSystemRecord(DisVectoringNozzleSystemDataRecord* aVectorNozzleSystemRecord);
   unsigned int RemoveVectoringNozzleSystemRecord(DisVectoringNozzleSystemDataRecord* aVectorNozzleSystemRecord);
   void         RemoveAllVectoringNozzleSystemRecords();
   unsigned int RemoveVectoringNozzleSystemRecordWithNoDelete(DisVectoringNozzleSystemDataRecord* aVectorNozzleSystemRecord);
   void         RemoveAllVectoringNozzleSystemRecordsWithNoDelete();


   // LOGGING
   std::string         GetStringId() const override;
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const override;
   // END LOGGING

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   void UpdatePropulsionRecordCount() const;
   void UpdateVectorNozzleRecordCount() const;

private:
   void CopyRecords(const DisSupplementalEmissionEntityState& aSrc);
   void GetMemberData(GenI& aGenI) override;

   //! The entity that is the source of information contained in this PDU
   DisEntityId mOriginatingEntity = DisEntityId::ENTITY_ID_UNKNOWN;

   //! An index to a specific value or an index pointer to a data table accessed with other information to obtain
   //! a specific value for infrared signature in which the entity is currently.
   DisUint16 mInfraredSignatureRepresentationIndex = 0;

   //! An index to a specific value or an index pointer to a data table accessed with other information to obtain
   //! a specific value for the acoustic signaturestate in which the entity is currently.
   DisUint16 mAcousticSignatureRepresentationIndex = 0;

   //! An index to a specific value or an index pointer to a data table accessed with other information to obtain
   //! a specific value for the radar cross-section signature state in which the entity is currently.
   DisUint16 mRadarCrossSectionRepresentationIndex = 0;

   //! The number of operational propulsion systems aboard the entity that are described in the current SEES PDU.
   //! One, several, or all of the propulsion systems on a particular entity may be described in a single SEES PDU.
   mutable DisUint16 mNumberOfPropulsionSystemRecords = 0;

   //! The number of operational vectoring nozzle systems aboard the entity. One, several, or all of the nozzle systems
   //! on a particular entity may be described in the SEES PDU.
   mutable DisUint16 mNumberOfVectoringNozzleSystemRecords = 0;

   //! Identifies basic operation data for the propulsion systes aboard the entity.
   DisPtrContainer<DisPropulsionSystemDataRecord> mPropulsionSystemsRecordList;

   //! Identifies the basic operation data for the vectoring nozzle systems aboard the entity.
   DisPtrContainer<DisVectoringNozzleSystemDataRecord> mVectoringNozzleSystemRecordList;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   const DisUint16 cPDU_MIN_LENGTH_OCTETS = 16;
};
#endif
