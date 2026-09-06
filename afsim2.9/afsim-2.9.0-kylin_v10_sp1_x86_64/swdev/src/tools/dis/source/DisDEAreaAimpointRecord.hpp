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

// DIS DE Area Aimpoint Record

#ifndef DIS_DE_AREA_AIMPOINT_RECORD_HPP
#define DIS_DE_AREA_AIMPOINT_RECORD_HPP

#include "dis_export.h"

#include "DisDEBeamAntennaPatternRecord.hpp"
#include "DisDERecord.hpp"
#include "DisDETargetEnergyDepositionRecord.hpp"
#include "DisPtrContainer.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDEAreaAimpointRecord : public DisDERecord
{
public:
   DisDEAreaAimpointRecord();
   DisDEAreaAimpointRecord(DisDERecord& aRecord, GenI& aGenI);
   ~DisDEAreaAimpointRecord() override;

   // Input/output

   DisUint16 GetDerivedClassLength() override;
   void      GetDerivedClass(GenI& aGenI) override;
   void      PutDerivedClass(GenO& aGenO) const override;

   // Data Validation
   bool IsValid() const override;

   // Accessors

   DisUint16 GetBeamAntennaPatternRecordCount();
   DisUint16 GetTargetEnergyDepositionRecordCount();

   // WARNING: AddBeamAntennaPatternRecord takes ownership of the passed object.
   void AddBeamAntennaPatternRecord(DisDEBeamAntennaPatternRecord* aBeamAntennaPatternRecord);

   // WARNING: AddTargetEnergyDepositionRecord takes ownership of the passed object.
   void AddTargetEnergyDepositionRecord(DisDETargetEnergyDepositionRecord* aTargetEnergyDepositionRecord);

   // Return iterators to the DisDEBeamAntennaPatternRecord vector.
   std::vector<DisDEBeamAntennaPatternRecord*>::iterator       GetDEBeamAntennaPatternBegin();
   std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator GetDEBeamAntennaPatternBegin() const;
   std::vector<DisDEBeamAntennaPatternRecord*>::iterator       GetDEBeamAntennaPatternEnd();
   std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator GetDEBeamAntennaPatternEnd() const;

   // Return iterators to the DisDETargetEnergyDepositionRecord vector.
   std::vector<DisDETargetEnergyDepositionRecord*>::iterator       GetDETargetEnergyDepositionBegin();
   std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator GetDETargetEnergyDepositionBegin() const;
   std::vector<DisDETargetEnergyDepositionRecord*>::iterator       GetDETargetEnergyDepositionEnd();
   std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator GetDETargetEnergyDepositionEnd() const;

   // Remove and delete a DisDEBeamAntennaPatternRecord object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first BeamAntennaPatternRecord on the list.
   unsigned int RemoveBeamAntennaPatternRecord(DisDEBeamAntennaPatternRecord* aBeamAntennaPatternRecord);

   // Remove and delete a DisDETargetEnergyDepositionRecord object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first TargetEnergyDepositionRecord on the list.
   unsigned int RemoveTargetEnergyDepositionRecord(DisDETargetEnergyDepositionRecord* aTargetEnergyDepositionRecord);

   // Remove and delete all DisDEBeamAntennaPatternRecord objects from the list.
   void RemoveAllBeamAntennaPatternRecords();

   // Remove and delete all DisDETargetEnergyDepositionRecord objects from the list.
   void RemoveAllTargetEnergyDepositionRecords();

   // Remove DisDEBeamAntennaPatternRecord object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first BeamAntennaPatternRecord on the list.
   unsigned int RemoveBeamAntennaPatternRecordWithNoDelete(DisDEBeamAntennaPatternRecord* aBeamAntennaPatternRecord);

   // Remove DisDETargetEnergyDepositionRecord object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first TargetEnergyDepositionRecord on the list.
   unsigned int RemoveTargetEnergyDepositionRecordWithNoDelete(DisDETargetEnergyDepositionRecord* aTargetEnergyDepositionRecord);

   // Remove all DisDEBeamAntennaPatternRecord objects from the list, but DO NOT DELETE them.
   void RemoveAllBeamAntennaPatternRecordsWithNoDelete();

   // Remove all DisDETargetEnergyDepositionRecord objects from the list, but DO NOT DELETE them.
   void RemoveAllTargetEnergyDepositionRecordsWithNoDelete();

private:
   void UpdateBeamAntennaPatternRecordCount() const;
   void UpdateTargetEnergyDepositionRecordCount() const;

   mutable DisUint16 mBeamAntennaPatternRecordCount;
   mutable DisUint16 mTargetEnergyDepositionRecordCount;

   DisPtrContainer<DisDEBeamAntennaPatternRecord>     mBeamAntennaPatternRecordList;
   DisPtrContainer<DisDETargetEnergyDepositionRecord> mTargetEnergyDepositionRecordList;
};

inline void DisDEAreaAimpointRecord::UpdateBeamAntennaPatternRecordCount() const
{
   mBeamAntennaPatternRecordCount = static_cast<DisUint16>(mBeamAntennaPatternRecordList.GetSize());
}

inline void DisDEAreaAimpointRecord::UpdateTargetEnergyDepositionRecordCount() const
{
   mTargetEnergyDepositionRecordCount = static_cast<DisUint16>(mTargetEnergyDepositionRecordList.GetSize());
}

// Accessors

inline DisUint16 DisDEAreaAimpointRecord::GetBeamAntennaPatternRecordCount()
{
   UpdateBeamAntennaPatternRecordCount();
   return mBeamAntennaPatternRecordCount;
}

inline DisUint16 DisDEAreaAimpointRecord::GetTargetEnergyDepositionRecordCount()
{
   UpdateTargetEnergyDepositionRecordCount();
   return mTargetEnergyDepositionRecordCount;
}

// BeamAntennaPatternRecord list methods

inline void DisDEAreaAimpointRecord::AddBeamAntennaPatternRecord(DisDEBeamAntennaPatternRecord* aBeamAntennaPatternRecord)
{
   mBeamAntennaPatternRecordList.Add(aBeamAntennaPatternRecord);
}

inline std::vector<DisDEBeamAntennaPatternRecord*>::iterator DisDEAreaAimpointRecord::GetDEBeamAntennaPatternBegin()
{
   return mBeamAntennaPatternRecordList.GetBegin();
}

inline std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator DisDEAreaAimpointRecord::GetDEBeamAntennaPatternBegin() const
{
   return mBeamAntennaPatternRecordList.GetBegin();
}

inline std::vector<DisDEBeamAntennaPatternRecord*>::iterator DisDEAreaAimpointRecord::GetDEBeamAntennaPatternEnd()
{
   return mBeamAntennaPatternRecordList.GetEnd();
}

inline std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator DisDEAreaAimpointRecord::GetDEBeamAntennaPatternEnd() const
{
   return mBeamAntennaPatternRecordList.GetEnd();
}

inline unsigned int DisDEAreaAimpointRecord::RemoveBeamAntennaPatternRecord(DisDEBeamAntennaPatternRecord* aBeamAntennaPatternRecord)
{
   return mBeamAntennaPatternRecordList.Remove(aBeamAntennaPatternRecord);
}

inline void DisDEAreaAimpointRecord::RemoveAllBeamAntennaPatternRecords()
{
   mBeamAntennaPatternRecordList.RemoveAll();
}

inline unsigned int DisDEAreaAimpointRecord::RemoveBeamAntennaPatternRecordWithNoDelete(
   DisDEBeamAntennaPatternRecord* aBeamAntennaPatternRecord)
{
   return mBeamAntennaPatternRecordList.RemoveWithNoDelete(aBeamAntennaPatternRecord);
}

inline void DisDEAreaAimpointRecord::RemoveAllBeamAntennaPatternRecordsWithNoDelete()
{
   mBeamAntennaPatternRecordList.RemoveAllWithNoDelete();
}

// TargetEnergyDepositionRecord list methods

inline void DisDEAreaAimpointRecord::AddTargetEnergyDepositionRecord(DisDETargetEnergyDepositionRecord* aTargetEnergyDepositionRecord)
{
   mTargetEnergyDepositionRecordList.Add(aTargetEnergyDepositionRecord);
}

inline std::vector<DisDETargetEnergyDepositionRecord*>::iterator DisDEAreaAimpointRecord::GetDETargetEnergyDepositionBegin()
{
   return mTargetEnergyDepositionRecordList.GetBegin();
}

inline std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator
DisDEAreaAimpointRecord::GetDETargetEnergyDepositionBegin() const
{
   return mTargetEnergyDepositionRecordList.GetBegin();
}

inline std::vector<DisDETargetEnergyDepositionRecord*>::iterator DisDEAreaAimpointRecord::GetDETargetEnergyDepositionEnd()
{
   return mTargetEnergyDepositionRecordList.GetEnd();
}

inline std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator
DisDEAreaAimpointRecord::GetDETargetEnergyDepositionEnd() const
{
   return mTargetEnergyDepositionRecordList.GetEnd();
}

inline unsigned int DisDEAreaAimpointRecord::RemoveTargetEnergyDepositionRecord(
   DisDETargetEnergyDepositionRecord* aTargetEnergyDepositionRecord)
{
   return mTargetEnergyDepositionRecordList.Remove(aTargetEnergyDepositionRecord);
}

inline void DisDEAreaAimpointRecord::RemoveAllTargetEnergyDepositionRecords()
{
   mTargetEnergyDepositionRecordList.RemoveAll();
}

inline unsigned int DisDEAreaAimpointRecord::RemoveTargetEnergyDepositionRecordWithNoDelete(
   DisDETargetEnergyDepositionRecord* aTargetEnergyDepositionRecord)
{
   return mTargetEnergyDepositionRecordList.RemoveWithNoDelete(aTargetEnergyDepositionRecord);
}

inline void DisDEAreaAimpointRecord::RemoveAllTargetEnergyDepositionRecordsWithNoDelete()
{
   mTargetEnergyDepositionRecordList.RemoveAllWithNoDelete();
}

#endif
