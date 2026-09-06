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

#include "DisDirectedEnergyFire.hpp"

#include <cassert>

#include "DisDEAreaAimpointRecord.hpp"
#include "DisDEPrecisionAimpointRecord.hpp"
#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDirectedEnergyFire::DisDirectedEnergyFire()
   : DisPdu()
   , mFiringEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEvent()
   , mMunitionType()
   , mShotStartTime()
   , mCumulativeShotTime(0.0)
   , mApertureDiameter(0.0)
   , mWavelength(0.0)
   , mPadding4(0)
   , mPulseRepetitionFrequency(0.0)
   , mPulseWidth(0.0)
   , mFlags(0)
   , mPulseShape(0)
   , mPadding1(0)
   , mPadding2(0)
   , mPadding3(0)
   , mNumberOfDERecords(0)
{
   SetPduType(DisEnum::Pdu::Type::DirectedEnergyFire);
   SetProtocolFamily(DisEnum::Pdu::Family::Warfare);
   SetLength(88); // 704 bits -> size with no DE Records
   mMunitionType.SetEntityKind(DisEntityType::EKT_MUNITION_KIND);
   mMunitionType.SetDomain(DisEntityType::MKT_DIRECTED_ENERGY_WEAPON);
   mMunitionType.SetCategory(DisEntityType::MCT_ELECTRO_MAGNETIC_RADIATION);


   mApertureLocation[0] = 0.0;
   mApertureLocation[1] = 0.0;
   mApertureLocation[2] = 0.0;
}

DisDirectedEnergyFire::DisDirectedEnergyFire(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mFiringEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEvent()
   , mMunitionType()
   , mShotStartTime()
   , mCumulativeShotTime(0.0)
   , mApertureDiameter(0.0)
   , mWavelength(0.0)
   , mPadding4(0)
   , mPulseRepetitionFrequency(0.0)
   , mPulseWidth(0.0)
   , mFlags(0)
   , mPulseShape(0)
   , mPadding1(0)
   , mPadding2(0)
   , mPadding3(0)
   , mNumberOfDERecords(0)
{
   GetMemberData(aGenI);
}

DisDirectedEnergyFire::DisDirectedEnergyFire(GenI& aGenI)
   : DisPdu(aGenI)
   , mFiringEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEvent()
   , mMunitionType()
   , mShotStartTime()
   , mCumulativeShotTime(0.0)
   , mApertureDiameter(0.0)
   , mWavelength(0.0)
   , mPadding4(0)
   , mPulseRepetitionFrequency(0.0)
   , mPulseWidth(0.0)
   , mFlags(0)
   , mPulseShape(0)
   , mPadding1(0)
   , mPadding2(0)
   , mPadding3(0)
   , mNumberOfDERecords(0)
{
   GetMemberData(aGenI);
}

DisDirectedEnergyFire::~DisDirectedEnergyFire() {}

DisPdu* DisDirectedEnergyFire::Clone() const
{
   return new DisDirectedEnergyFire(*this);
}

// Virtual
int DisDirectedEnergyFire::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::DirectedEnergyFire);
}

void DisDirectedEnergyFire::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisDirectedEnergyFire::GetMemberData(aGenI);
}

DisUint16 DisDirectedEnergyFire::GetLength()
{
   UpdateDERecordCount();

   DisUint16 length = DisPdu::GetBaseLength() + 76; // length of fixed portion

   std::vector<DisDERecord*>::const_iterator iter(mDERecordList.GetBegin());
   while (iter != mDERecordList.GetEnd())
   {
      length += (*iter++)->GetLength();
   }

   SetLength(length);
   return length;
}

void DisDirectedEnergyFire::GetMemberData(GenI& aGenI)
{
   DisInt32 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mFiringEntity;
   aGenI >> mEvent;
   aGenI >> mMunitionType;
   aGenI >> mShotStartTime;
   aGenI >> mCumulativeShotTime;
   aGenI >> mApertureLocation[0];
   aGenI >> mApertureLocation[1];
   aGenI >> mApertureLocation[2];
   aGenI >> mApertureDiameter;
   aGenI >> mWavelength;
   aGenI >> mPadding4;
   aGenI >> mPulseRepetitionFrequency;
   aGenI >> mPulseWidth;
   aGenI >> mFlags;
   aGenI >> mPulseShape;
   aGenI >> mPadding1;
   aGenI >> mPadding2;
   aGenI >> mPadding3;
   aGenI >> mNumberOfDERecords;

   lengthToRead -= DisPdu::GetBaseLength();
   lengthToRead -= 76; // length of fixed portion (608 bits)

   RemoveAllDERecords();

   int recordType = 0;
   mDERecordList.Reserve(mNumberOfDERecords);
   for (int i = 0; i < mNumberOfDERecords; ++i)
   {
      DisDERecord  recordHeader(aGenI);
      DisDERecord* recordPtr;
      recordType = recordHeader.GetRecordType();
      switch (recordType)
      {
      case DisDERecord::AREA_AIMPOINT_DE_RECORD_TYPE:
         recordPtr = new DisDEAreaAimpointRecord(recordHeader, aGenI);
         break;
      case DisDERecord::PRECISION_AIMPOINT_DE_RECORD_TYPE:
         recordPtr = new DisDEPrecisionAimpointRecord(recordHeader, aGenI);
         break;
      default:
         recordPtr = nullptr;
         break;
      }
      if (recordPtr != nullptr)
      {
         //  recordPtr->Get(aGenI);
         mDERecordList.Add(recordPtr); // List takes ownership
         lengthToRead -= recordPtr->GetLength();
      }
      else
      {
         // Read past the unimplemented or invalid attribute record
         DisUint16 givenRecordLength  = recordHeader.GetRecordLength();
         DisUint16 sizeOfRecordHeader = recordHeader.GetLength();
         int       sizeOfBytesNotRead = givenRecordLength - sizeOfRecordHeader;

         aGenI.SetGetOffset(sizeOfBytesNotRead, GenBuf::FromCur);
         lengthToRead -= givenRecordLength;
      }
   }
   assert(lengthToRead >= 0);
   ReadExtraData(aGenI, static_cast<DisUint16>(lengthToRead));
}

void DisDirectedEnergyFire::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mFiringEntity;
   aGenO << mEvent;
   aGenO << mMunitionType;
   aGenO << mShotStartTime;
   aGenO << mCumulativeShotTime;
   aGenO << mApertureLocation[0] << mApertureLocation[1] << mApertureLocation[2];
   aGenO << mApertureDiameter;
   aGenO << mWavelength;
   aGenO << mPadding4;
   aGenO << mPulseRepetitionFrequency;
   aGenO << mPulseWidth;
   aGenO << mFlags;
   aGenO << mPulseShape;
   aGenO << mPadding1;
   aGenO << mPadding2;
   aGenO << mPadding3;
   aGenO << mNumberOfDERecords;

   std::vector<DisDERecord*>::const_iterator iter(mDERecordList.GetBegin());
   while (iter != mDERecordList.GetEnd())
   {
      (*iter++)->Put(aGenO);
   }
}

bool DisDirectedEnergyFire::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mFiringEntity.IsValid())
      isValid = false;
   if (!mEvent.IsValid())
      isValid = false;
   if (!mMunitionType.IsValid())
      isValid = false;
   if (!mShotStartTime.IsValid())
      isValid = false;
   if (!ValidateScalar(mCumulativeShotTime))
      isValid = false;
   if (!ValidateScalar(mApertureLocation[0]))
      isValid = false;
   if (!ValidateScalar(mApertureLocation[1]))
      isValid = false;
   if (!ValidateScalar(mApertureLocation[2]))
      isValid = false;
   if (!ValidateScalar(mApertureDiameter))
      isValid = false;
   if (!ValidateScalar(mWavelength))
      isValid = false;
   if (!ValidateScalar(mPulseRepetitionFrequency))
      isValid = false;
   if (!ValidateScalar(mPulseWidth))
      isValid = false;

   std::vector<DisDERecord*>::const_iterator iter(mDERecordList.GetBegin());
   while (iter != mDERecordList.GetEnd())
   {
      if (!(*iter++)->IsValid())
         isValid = false;
   }

   return isValid;
}
