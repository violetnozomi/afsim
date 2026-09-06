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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DisEntityDamageStatus_HPP
#define DisEntityDamageStatus_HPP

#include "dis_export.h"

#include "DisDDRecord.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"

class DIS_EXPORT DisEntityDamageStatus : public DisPdu
{
public:
   DisEntityDamageStatus();
   DisEntityDamageStatus(DisPdu& aPdu, GenI& aGenI);
   DisEntityDamageStatus(GenI& aGenI);
   ~DisEntityDamageStatus() override;

   DisPdu* Clone() const override;
   // Allow compiler to build copy constructor and operator=
   // DisEntityDamageStatus(const DisEntityDamageStatus& aSrc);
   // DisEntityDamageStatus& operator=(const DisEntityDamageStatus& aRhs);

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetDamagedEntity() const;
   DisUint16          GetNumberOfDDRecords();

   // Mutators

   void SetDamagedEntity(const DisEntityId& aDamagedEntity);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   void AddDDRecord(DisDDRecord* aDDRecord);

   // Return iterators to DDRecord vector.
   std::vector<DisDDRecord*>::iterator       GetDDRecordBegin();
   std::vector<DisDDRecord*>::const_iterator GetDDRecordBegin() const;
   std::vector<DisDDRecord*>::iterator       GetDDRecordEnd();
   std::vector<DisDDRecord*>::const_iterator GetDDRecordEnd() const;

   unsigned int RemoveDDRecord(DisDDRecord* aDDRecord);

   void RemoveAllDDRecords();

   unsigned int RemoveDDRecordWithNoDelete(DisDDRecord* aDDRecord);

   void RemoveAllDDRecordsWithNoDelete();

private:
   void GetMemberData(GenI& aGenI);

   void UpdateDDRecordCount() const;

   DisEntityId       mDamagedEntity;
   mutable DisUint16 mNumberOfDDRecords;

   DisPtrContainer<DisDDRecord> mDDRecordList;
};

// Accessors

inline void DisEntityDamageStatus::UpdateDDRecordCount() const
{
   mNumberOfDDRecords = static_cast<DisUint16>(mDDRecordList.GetSize());
}
inline const DisEntityId& DisEntityDamageStatus::GetDamagedEntity() const
{
   return mDamagedEntity;
}
inline DisUint16 DisEntityDamageStatus::GetNumberOfDDRecords()
{
   UpdateDDRecordCount();
   return mNumberOfDDRecords;
}

// Mutators

inline void DisEntityDamageStatus::SetDamagedEntity(const DisEntityId& aDamagedEntity)
{
   mDamagedEntity = aDamagedEntity;
}

// DD Record list methods
inline void DisEntityDamageStatus::AddDDRecord(DisDDRecord* aDDRecord)
{
   mDDRecordList.Add(aDDRecord);
   UpdateDDRecordCount();
}
inline std::vector<DisDDRecord*>::iterator DisEntityDamageStatus::GetDDRecordBegin()
{
   return mDDRecordList.GetBegin();
}
inline std::vector<DisDDRecord*>::const_iterator DisEntityDamageStatus::GetDDRecordBegin() const
{
   return mDDRecordList.GetBegin();
}
inline std::vector<DisDDRecord*>::iterator DisEntityDamageStatus::GetDDRecordEnd()
{
   return mDDRecordList.GetEnd();
}
inline std::vector<DisDDRecord*>::const_iterator DisEntityDamageStatus::GetDDRecordEnd() const
{
   return mDDRecordList.GetEnd();
}
inline unsigned int DisEntityDamageStatus::RemoveDDRecord(DisDDRecord* aDDRecord)
{
   return mDDRecordList.Remove(aDDRecord);
}
inline void DisEntityDamageStatus::RemoveAllDDRecords()
{
   mDDRecordList.RemoveAll();
}
inline unsigned int DisEntityDamageStatus::RemoveDDRecordWithNoDelete(DisDDRecord* aDDRecord)
{
   return mDDRecordList.RemoveWithNoDelete(aDDRecord);
}
inline void DisEntityDamageStatus::RemoveAllDDRecordsWithNoDelete()
{
   mDDRecordList.RemoveAllWithNoDelete();
}

#endif
