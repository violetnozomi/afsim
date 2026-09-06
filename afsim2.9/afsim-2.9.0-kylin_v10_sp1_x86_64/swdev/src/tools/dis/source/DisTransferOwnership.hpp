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

#ifndef DISTRANSFEROWNERSHIP_HPP
#define DISTRANSFEROWNERSHIP_HPP

#include "dis_export.h"

#include <cassert>
#include <ostream>
#include <vector>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSpecRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisTransferOwnership : public DisPdu
{
public:
   enum TransferType
   {
      PushTransferEntity                        = 1,
      AutomaticPullTransferEntity               = 2,
      PushTransferEnvironmentalProcess          = 4,
      AutomaticPullTransferEnvironmentalProcess = 5,
      CancelTransfer                            = 7,
      ManualPullTransferEntity                  = 8,
      ManualPullTransferEnvironmentalProcess    = 9,
      RemoveEntity                              = 10
   };

   DisTransferOwnership();
   DisTransferOwnership(DisPdu& aPdu, GenI& aGenI);
   DisTransferOwnership(GenI& aGenI);
   DisTransferOwnership(const DisTransferOwnership& aSrc) = default;
   DisTransferOwnership& operator=(const DisTransferOwnership& aRhs) = default;
   ~DisTransferOwnership() override                                  = default;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId&   GetReceivingEntity() const override;
   DisUint32            GetRequestId() const override;
   DisEnum8             GetReliabilityService() const override;
   DisEnum8             GetTransferType() const;
   const DisEntityId&   GetTransferEntity() const;
   DisUint32            GetNumberOfRecordSets() const;
   const DisSpecRecord* GetRecordSet(unsigned int aIndex) const;

   // Mutators
   void SetOriginatingEntity(const DisEntityId& aOriginatingEntity);
   void SetReceivingEntity(const DisEntityId& aReceivingEntity);
   void SetRequestId(DisUint32 aRequestId);
   void SetReliabilityService(DisEnum8 aReliabilityService);
   void SetTransferType(DisEnum8 aTransferType);
   void SetTransferEntity(const DisEntityId& aTransferEntity);
   void SetNumberOfRecordSets(DisUint32 aNumberOfRecordSets);
   void AddRecordSet(const DisSpecRecord& aSpecRecord);
   void AddRecordSetAndTakeOwnership(DisSpecRecord* aSpecRecord);

   // Input/Output
   DisUint16 GetLength() override;
   void      Get(GenI& aGenI) override;
   void      Put(GenO& aGenO) override;
   void      Stream(std::ostream& aStream) const override;
   bool      IsValid() const override;

   // WARNING: RemoveRecordSet will delete aRecord
   void RemoveRecordSet(DisSpecRecord* aRecord);
   void RemoveAllRecordSets();

   std::vector<DisSpecRecord*>::iterator       GetSpecRecordBegin();
   std::vector<DisSpecRecord*>::const_iterator GetSpecRecordBegin() const;
   std::vector<DisSpecRecord*>::iterator       GetSpecRecordEnd();
   std::vector<DisSpecRecord*>::const_iterator GetSpecRecordEnd() const;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
   DisEnum8    mReliabilityService;
   DisEnum8    mTransferType;
   DisEntityId mTransferEntity;
   DisUint32   mNumberOfRecordSets;
   // The Dis Standard refers to these as Record Sets, Why does AFSIM call this DisSpecRecord?
   DisPtrContainer<DisSpecRecord> mSpecRecordList;
};

// Accessors
inline const DisEntityId& DisTransferOwnership::GetReceivingEntity() const
{
   return mReceivingEntity;
}

inline DisEnum8 DisTransferOwnership::GetTransferType() const
{
   return mTransferType;
}

inline const DisEntityId& DisTransferOwnership::GetTransferEntity() const
{
   return mTransferEntity;
}

inline void DisTransferOwnership::SetOriginatingEntity(const DisEntityId& aOriginatingEntity)
{
   mOriginatingEntity = aOriginatingEntity;
}

inline void DisTransferOwnership::SetReceivingEntity(const DisEntityId& aReceivingEntity)
{
   mReceivingEntity = aReceivingEntity;
}

inline DisUint32 DisTransferOwnership::GetNumberOfRecordSets() const
{
   return mNumberOfRecordSets;
}

inline void DisTransferOwnership::SetTransferType(DisEnum8 aTransferType)
{
   mTransferType = aTransferType;
}

inline void DisTransferOwnership::SetTransferEntity(const DisEntityId& aTransferEntity)
{
   mTransferEntity = aTransferEntity;
}

inline void DisTransferOwnership::SetNumberOfRecordSets(DisUint32 aNumberOfRecordSets)
{
   mNumberOfRecordSets = aNumberOfRecordSets;
}

inline void DisTransferOwnership::AddRecordSet(const DisSpecRecord& aSpecRecord)
{
   // Go ahead and make a copy
   DisSpecRecord* disSpecRecord = new DisSpecRecord(aSpecRecord);

   // Let the list own the pointer
   mSpecRecordList.Add(disSpecRecord);
   mNumberOfRecordSets = mSpecRecordList.GetSize();
}

inline void DisTransferOwnership::AddRecordSetAndTakeOwnership(DisSpecRecord* aSpecRecord)
{
   // Let the list own the pointer
   mSpecRecordList.Add(aSpecRecord);
   mNumberOfRecordSets = mSpecRecordList.GetSize();
}

inline void DisTransferOwnership::RemoveAllRecordSets()
{
   mSpecRecordList.RemoveAll();
   mNumberOfRecordSets = 0;
}

inline void DisTransferOwnership::RemoveRecordSet(DisSpecRecord* aRecord)
{
   mSpecRecordList.Remove(aRecord);
   mNumberOfRecordSets = mSpecRecordList.GetSize();
}

inline std::vector<DisSpecRecord*>::iterator DisTransferOwnership::GetSpecRecordBegin()
{
   return mSpecRecordList.GetBegin();
}

inline std::vector<DisSpecRecord*>::const_iterator DisTransferOwnership::GetSpecRecordBegin() const
{
   return mSpecRecordList.GetBegin();
}

inline std::vector<DisSpecRecord*>::iterator DisTransferOwnership::GetSpecRecordEnd()
{
   return mSpecRecordList.GetEnd();
}

inline std::vector<DisSpecRecord*>::const_iterator DisTransferOwnership::GetSpecRecordEnd() const
{
   return mSpecRecordList.GetEnd();
}

#endif
