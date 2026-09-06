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

#include "DisDEAreaAimpointRecord.hpp"

#include <iostream>

#include "DisEntityId.hpp"
#include "GenI.hpp"
#include "GenO.hpp"


DisDEAreaAimpointRecord::DisDEAreaAimpointRecord()
   : DisDERecord(DisDERecord::AREA_AIMPOINT_DE_RECORD_TYPE)
   , mBeamAntennaPatternRecordCount(0)
   , mTargetEnergyDepositionRecordCount(0)
{
   mBeamAntennaPatternRecordList.RemoveAll();
   mTargetEnergyDepositionRecordList.RemoveAll();
}

DisDEAreaAimpointRecord::DisDEAreaAimpointRecord(DisDERecord& aRecord, GenI& aGenI)
   : DisDERecord(aRecord)
   , mBeamAntennaPatternRecordCount(0)
   , mTargetEnergyDepositionRecordCount(0)
{
   GetDerivedClass(aGenI);
}

DisDEAreaAimpointRecord::~DisDEAreaAimpointRecord() {}

// virtual
DisUint16 DisDEAreaAimpointRecord::GetDerivedClassLength()
{
   DisUint16 length = 0;

   UpdateBeamAntennaPatternRecordCount();
   UpdateTargetEnergyDepositionRecordCount();

   std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator beamIter(mBeamAntennaPatternRecordList.GetBegin());
   while (beamIter != mBeamAntennaPatternRecordList.GetEnd())
   {
      length += (*beamIter++)->GetLength();
   }

   std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator targetIter(mTargetEnergyDepositionRecordList.GetBegin());
   while (targetIter != mTargetEnergyDepositionRecordList.GetEnd())
   {
      length += (*targetIter++)->GetLength();
   }

   return length + (8U - (length % 8U));
}

// virtual
void DisDEAreaAimpointRecord::PutDerivedClass(GenO& aGenO) const
{
   unsigned int length = 4;

   aGenO << mBeamAntennaPatternRecordCount;
   aGenO << mTargetEnergyDepositionRecordCount;

   std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator beamIter(mBeamAntennaPatternRecordList.GetBegin());
   while (beamIter != mBeamAntennaPatternRecordList.GetEnd())
   {
      aGenO << **beamIter;
      length += (*beamIter++)->GetLength();
   }

   std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator targetIter(mTargetEnergyDepositionRecordList.GetBegin());
   while (targetIter != mTargetEnergyDepositionRecordList.GetEnd())
   {
      aGenO << **targetIter;
      length += (*targetIter++)->GetLength();
   }

   DisUint8     padding(0);
   unsigned int numPadding = length % 8;

   for (unsigned int i = 0; i < numPadding; ++i)
   {
      aGenO << padding;
   }
}

// virtual
void DisDEAreaAimpointRecord::GetDerivedClass(GenI& aGenI)
{
   unsigned int length = 4;

   aGenI >> mBeamAntennaPatternRecordCount;
   aGenI >> mTargetEnergyDepositionRecordCount;

   RemoveAllBeamAntennaPatternRecords();
   RemoveAllTargetEnergyDepositionRecords();

   mBeamAntennaPatternRecordList.Reserve(mBeamAntennaPatternRecordCount);
   mTargetEnergyDepositionRecordList.Reserve(mTargetEnergyDepositionRecordCount);
   unsigned int i;

   for (i = 0; i < mBeamAntennaPatternRecordCount; ++i)
   {
      DisDEBeamAntennaPatternRecord* beamRecord = new DisDEBeamAntennaPatternRecord(aGenI);
      length += beamRecord->GetLength();
      mBeamAntennaPatternRecordList.Add(beamRecord);
   }

   for (i = 0; i < mTargetEnergyDepositionRecordCount; ++i)
   {
      DisDETargetEnergyDepositionRecord* targetRecord = new DisDETargetEnergyDepositionRecord(aGenI);
      length += targetRecord->GetLength();
      mTargetEnergyDepositionRecordList.Add(targetRecord);
   }

   DisUint8     extraByte;
   unsigned int numPadding = length % 8;

   for (unsigned int i = 0; i < numPadding; ++i)
   {
      aGenI >> extraByte;
   }
}

bool DisDEAreaAimpointRecord::IsValid() const
{
   bool isValid = true;

   // Perform data validation.
   std::vector<DisDEBeamAntennaPatternRecord*>::const_iterator beamIter(mBeamAntennaPatternRecordList.GetBegin());
   while (beamIter != mBeamAntennaPatternRecordList.GetEnd())
   {
      if (!(*beamIter++)->IsValid())
         isValid = false;
   }

   std::vector<DisDETargetEnergyDepositionRecord*>::const_iterator targetIter(mTargetEnergyDepositionRecordList.GetBegin());
   while (targetIter != mTargetEnergyDepositionRecordList.GetEnd())
   {
      if (!(*targetIter++)->IsValid())
         isValid = false;
   }

   return isValid;
}
