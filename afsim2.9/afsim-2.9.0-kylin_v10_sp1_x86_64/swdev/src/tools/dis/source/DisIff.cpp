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

#include "DisIff.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtCast.hpp"

// NOTE:
//
// IEEE 1278.1a-1998 says the PDU with Layer 2 present is 736 + 192n bits
// long.  Per Steve Monson this is WRONG.  It should be 704 + 192n bits.  The
// problem is that the 'Beam Data' record is documented as 192 bits long but
// is really 160 bits long.
//
// The DISengine and Mak Technologies both use the 704 + 192n length.

namespace
{
const DisUint16 minLayer2Length = 28;
const DisUint16 minLayer3Length = 28;
const DisUint16 minLayer5Length = 16;

// The Information Layers field has bit n set if the nth layer is present.
DisUint8 layerToMask(int aLayer)
{
   return ut::safe_cast<DisUint8, int>(1 << aLayer);
}
} // namespace

DisIff::DisIff()
   : DisPdu()
   , mEmitterId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventId()
   , mLocationX(0.0F)
   , mLocationY(0.0F)
   , mLocationZ(0.0F)
   , mSystemId()
   , mSystemStatus(0)
   , mAlternateParameter4(0)
   , mInformationLayers(layerToMask(1))
   , mModifier(0)
   , mParameter1(0)
   , mParameter2(0)
   , mParameter3(0)
   , mParameter4(0)
   , mParameter5(0)
   , mParameter6(0)
   , mBeamDataRecord()
   , mLayer2Parameter1(0)
   , mLayer2Parameter2(0)
   , mParametersCount(0)
   , mParametersList()
   , mReportingSite(0)
   , mReportingApplication(0)
   , mMode5Status(0)
   , mPIN(0)
   , mMessageFormat(0)
   , mEnhancedMode1(0)
   , mNationOfOrigin(0)
   , mMissionCode(0)
   , mSquitterStatus(0)
   , mNavigationSource(0)
   , mFigureOfMerit(0)
   , mNumMode5Records(0)
   , mAisPositionData(nullptr)
   , mAisStaticData(nullptr)
{
   SetPduType(DisEnum::Pdu::Type::IFF);
   SetProtocolFamily(DisEnum::Pdu::Family::DistributedEmissionRegeneration);
   SetLength(GetLayer1Length());
}

DisIff::DisIff(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mAisPositionData(nullptr)
   , mAisStaticData(nullptr)
{
   GetMemberData(aGenI);
}

DisIff::DisIff(GenI& aGenI)
   : DisPdu(aGenI)
   , mAisPositionData(nullptr)
   , mAisStaticData(nullptr)
{
   GetMemberData(aGenI);
}
DisIff::DisIff(const DisIff& aSrc)
   : DisPdu(aSrc)
   , mEmitterId(aSrc.mEmitterId)
   , mEventId(aSrc.mEventId)
   , mLocationX(aSrc.mLocationX)
   , mLocationY(aSrc.mLocationY)
   , mLocationZ(aSrc.mLocationZ)
   , mSystemId(aSrc.mSystemId)
   , mSystemStatus(aSrc.mSystemStatus)
   , mAlternateParameter4(aSrc.mAlternateParameter4)
   , mInformationLayers(aSrc.mInformationLayers)
   , mModifier(aSrc.mModifier)
   , mParameter1(aSrc.mParameter1)
   , mParameter2(aSrc.mParameter2)
   , mParameter3(aSrc.mParameter3)
   , mParameter4(aSrc.mParameter4)
   , mParameter5(aSrc.mParameter5)
   , mParameter6(aSrc.mParameter6)
   , mBeamDataRecord(aSrc.mBeamDataRecord)
   , mLayer2Parameter1(aSrc.mLayer2Parameter1)
   , mLayer2Parameter2(aSrc.mLayer2Parameter2)
   , mParametersCount(aSrc.mParametersCount)
   , mParametersList(aSrc.mParametersList)
   , mReportingSite(aSrc.mReportingSite)
   , mReportingApplication(aSrc.mReportingApplication)
   , mMode5Status(aSrc.mMode5Status)
   , mPIN(aSrc.mPIN)
   , mMessageFormat(aSrc.mMessageFormat)
   , mEnhancedMode1(aSrc.mEnhancedMode1)
   , mNationOfOrigin(aSrc.mNationOfOrigin)
   , mMissionCode(aSrc.mMissionCode)
   , mSquitterStatus(aSrc.mSquitterStatus)
   , mNavigationSource(aSrc.mNavigationSource)
   , mFigureOfMerit(aSrc.mFigureOfMerit)
   , mNumMode5Records(aSrc.mNumMode5Records)
   , mAisPositionData(nullptr)
   , mAisStaticData(nullptr)
{
   if (aSrc.mAisPositionData)
   {
      mAisPositionData = new DisAisPositionData(*aSrc.mAisPositionData);
   }
   if (aSrc.mAisStaticData)
   {
      mAisStaticData = new DisAisStaticData(*aSrc.mAisStaticData);
   }
}

const DisIff& DisIff::operator=(const DisIff& aRhs)
{
   if (&aRhs != this)
   {
      DisPdu::operator      =(aRhs);
      mEmitterId            = aRhs.mEmitterId;
      mEventId              = aRhs.mEventId;
      mLocationX            = aRhs.mLocationX;
      mLocationY            = aRhs.mLocationY;
      mLocationZ            = aRhs.mLocationZ;
      mSystemId             = aRhs.mSystemId;
      mSystemStatus         = aRhs.mSystemStatus;
      mAlternateParameter4  = aRhs.mAlternateParameter4;
      mInformationLayers    = aRhs.mInformationLayers;
      mModifier             = aRhs.mModifier;
      mParameter1           = aRhs.mParameter1;
      mParameter2           = aRhs.mParameter2;
      mParameter3           = aRhs.mParameter3;
      mParameter4           = aRhs.mParameter4;
      mParameter5           = aRhs.mParameter5;
      mParameter6           = aRhs.mParameter6;
      mBeamDataRecord       = aRhs.mBeamDataRecord;
      mLayer2Parameter1     = aRhs.mLayer2Parameter1;
      mLayer2Parameter2     = aRhs.mLayer2Parameter2;
      mParametersCount      = aRhs.mParametersCount;
      mParametersList       = aRhs.mParametersList;
      mReportingSite        = aRhs.mReportingSite;
      mReportingApplication = aRhs.mReportingApplication;
      mMode5Status          = aRhs.mMode5Status;
      mPIN                  = aRhs.mPIN;
      mMessageFormat        = aRhs.mMessageFormat;
      mEnhancedMode1        = aRhs.mEnhancedMode1;
      mNationOfOrigin       = aRhs.mNationOfOrigin;
      mMissionCode          = aRhs.mMissionCode;
      mSquitterStatus       = aRhs.mSquitterStatus;
      mNavigationSource     = aRhs.mNavigationSource;
      mFigureOfMerit        = aRhs.mFigureOfMerit;
      mNumMode5Records      = aRhs.mNumMode5Records;

      if (aRhs.mAisPositionData)
      {
         DisAisPositionData* posOrig = mAisPositionData;
         mAisPositionData            = new DisAisPositionData(*aRhs.mAisPositionData);
         delete posOrig;
      }
      else
      {
         delete mAisPositionData;
         mAisPositionData = nullptr;
      }
      if (aRhs.mAisStaticData)
      {
         DisAisStaticData* staticOrig = mAisStaticData;
         mAisStaticData               = new DisAisStaticData(*aRhs.mAisStaticData);
         delete staticOrig;
      }
      else
      {
         delete mAisStaticData;
         mAisStaticData = nullptr;
      }
   }

   return *this;
}

// virtual
DisIff::~DisIff()
{
   delete mAisPositionData;
   delete mAisStaticData;
}

// virtual
DisPdu* DisIff::Clone() const
{
   return new DisIff(*this);
}

// virtual
int DisIff::GetClass() const
{
   return (int)DisEnum::Pdu::Type::IFF;
}

// virtual
const DisEntityId& DisIff::GetOriginatingEntity() const
{
   return mEmitterId;
}

void DisIff::SetAisPositionData(DisAisPositionData* aData)
{
   delete mAisPositionData;
   mAisPositionData = aData;
}

void DisIff::SetAisStaticData(DisAisStaticData* aData)
{
   delete mAisStaticData;
   mAisStaticData = aData;
}

// virtual
void DisIff::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisIff::GetMemberData(aGenI);
}

// virtual
DisUint16 DisIff::GetLength()
{
   DisUint16 octetCount = GetLayer1Length();

   if (IsLayerPresent(2))
   {
      octetCount += GetLayer2Length();
   }
   if (IsLayerPresent(3))
   {
      octetCount += GetLayer3Length();
   }
   if (IsLayerPresent(5))
   {
      octetCount += GetLayer5Length();
   }
   SetLength(octetCount);
   return octetCount;
}

// private
void DisIff::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   ResetLayer2Data();
   ResetLayer3Data();
   ResetLayer5Data();

   bool isGood = ReadLayer1(aGenI, lengthToRead);
   if (isGood && IsLayerPresent(2))
   {
      isGood = ReadLayer2(aGenI, lengthToRead);
   }
   if (isGood && IsLayerPresent(3))
   {
      isGood = ReadLayer3(aGenI, lengthToRead);
   }
   if (isGood && IsLayerPresent(5))
   {
      isGood = ReadLayer5(aGenI, lengthToRead);
   }

   // Skip 'extra' data
   ReadExtraData(aGenI, lengthToRead);
}

// virtual
void DisIff::Put(GenO& aGenO)
{
   DisUint16 pad16(0);

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);

   // Write layer 1 data

   aGenO << mEmitterId;
   aGenO << mEventId;
   aGenO << mLocationX << mLocationY << mLocationZ;
   aGenO << mSystemId;
   aGenO << pad16;
   aGenO << mSystemStatus;
   aGenO << mAlternateParameter4;
   aGenO << mInformationLayers;
   aGenO << mModifier;
   aGenO << mParameter1;
   aGenO << mParameter2;
   aGenO << mParameter3;
   aGenO << mParameter4;
   aGenO << mParameter5;
   aGenO << mParameter6;

   // Write layer 2 data

   if (IsLayerPresent(2))
   {
      DisUint8  layerNumber(2);
      DisUint8  layerInfo(0);
      DisUint16 layerLength(GetLayer2Length());

      aGenO << layerNumber;
      aGenO << layerInfo;
      aGenO << layerLength;
      aGenO << mBeamDataRecord;
      aGenO << mLayer2Parameter1;
      aGenO << mLayer2Parameter2;
      aGenO << mParametersCount;

      std::vector<DisIffAtcNavaidsParams*>::const_iterator iter(mParametersList.GetBegin());
      while (iter != mParametersList.GetEnd())
      {
         aGenO << **iter++;
      }
   }

   // Write layer 3 data

   if (IsLayerPresent(3))
   {
      DisUint8  layerNumber(3);
      DisUint8  layerInfo(0);
      DisUint16 layerLength(28); // TBD on implementing the Mode 5 records
      DisUint8  padding = 0;

      aGenO << layerNumber;
      aGenO << layerInfo;
      aGenO << layerLength;
      aGenO << mReportingSite;
      aGenO << mReportingApplication;
      aGenO << mMode5Status;
      aGenO << mPIN;
      aGenO << mMessageFormat;
      aGenO << mEnhancedMode1;
      aGenO << mNationOfOrigin;
      aGenO << mMissionCode;
      aGenO << mSquitterStatus;
      aGenO << mNavigationSource;
      aGenO << mFigureOfMerit;
      aGenO << padding;

      aGenO << mNumMode5Records;

      // DisIffAtcNavaidsParams* parametersPtr = mParametersList.GetFirst();
      while (mNumMode5Records != 0)
      {
         // TBD Mode 5 Records
         // aGenO << *parametersPtr;
         // parametersPtr = mParametersList.GetNext();
      }
   }

   // Write layer 5 data

   if (IsLayerPresent(5))
   {
      DisUint8  layerNumber(5);
      DisUint8  layerInfo(0);
      DisUint16 layerLength(GetLayer5Length());
      DisUint8  applicableLayers(layerToMask(1) & layerToMask(2));
      DisUint8  dataCategory(0);

      aGenO << layerNumber;
      aGenO << layerInfo;
      aGenO << layerLength;
      aGenO << mEmitterId.GetSite();
      aGenO << mEmitterId.GetApplication();
      aGenO << pad16;
      aGenO << applicableLayers;
      aGenO << dataCategory;
      aGenO << pad16;

      DisUint16 numRecords(0);
      if (mAisPositionData)
         ++numRecords;
      if (mAisStaticData)
         ++numRecords;
      aGenO << numRecords;

      if (mAisPositionData)
      {
         mAisPositionData->Put(aGenO);
      }
      if (mAisStaticData)
      {
         mAisStaticData->Put(aGenO);
      }
   }
}

bool DisIff::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mEmitterId.IsValid())
      isValid = false;
   if (!mEventId.IsValid())
      isValid = false;
   if (!ValidateScalar(mLocationX))
      isValid = false;
   if (!ValidateScalar(mLocationY))
      isValid = false;
   if (!ValidateScalar(mLocationZ))
      isValid = false;
   if (!mSystemId.IsValid())
      isValid = false;

   // Layer 2 data

   if (!mBeamDataRecord.IsValid())
      isValid = false;

   std::vector<DisIffAtcNavaidsParams*>::const_iterator iter(mParametersList.GetBegin());
   while (iter != mParametersList.GetEnd())
   {
      if (!(*iter++)->IsValid())
         isValid = false;
   }

   // Layer 5 data

   if (mAisPositionData && !mAisPositionData->IsValid())
      isValid = false;
   if (mAisStaticData && !mAisStaticData->IsValid())
      isValid = false;

   return isValid;
}

bool DisIff::IsLayerPresent(int aLayer) const
{
   return (mInformationLayers & layerToMask(aLayer)) != 0;
}

void DisIff::UnsetLayerPresent(int aLayer)
{
   mInformationLayers &= (~layerToMask(aLayer));
}

DisUint16 DisIff::GetLayer1Length() const
{
   return 60; // 480 bits
}

DisUint16 DisIff::GetLayer2Length() const
{
   return ut::safe_cast<DisUint16, unsigned int>(minLayer2Length + 24U * mParametersList.GetSize());
}

DisUint16 DisIff::GetLayer3Length() const
{
   return 28; // have not yet implemented Mode 5 Transponder
              // Data Records
}


DisUint16 DisIff::GetLayer5Length() const
{
   DisUint16 length = minLayer5Length;

   if (mAisPositionData)
   {
      length += DisAisPositionData::GetRecordLength();
   }
   if (mAisStaticData)
   {
      length += DisAisStaticData::GetRecordLength();
   }

   return length;
}

bool DisIff::ReadLayer1(GenI& aGenI, DisUint16& aLengthToRead)
{
   bool layer1Read = false;

   if (aLengthToRead >= GetLayer1Length())
   {
      DisUint16 pad16(0);
      aGenI >> mEmitterId;
      aGenI >> mEventId;
      aGenI >> mLocationX >> mLocationY >> mLocationZ;
      aGenI >> mSystemId;
      aGenI >> pad16;
      aGenI >> mSystemStatus;
      aGenI >> mAlternateParameter4;
      aGenI >> mInformationLayers;
      aGenI >> mModifier;
      aGenI >> mParameter1;
      aGenI >> mParameter2;
      aGenI >> mParameter3;
      aGenI >> mParameter4;
      aGenI >> mParameter5;
      aGenI >> mParameter6;
      aLengthToRead -= GetLayer1Length();
      layer1Read = true;
   }

   return layer1Read;
}

bool DisIff::ReadLayer2(GenI& aGenI, DisUint16& aLengthToRead)
{
   bool layer2Read = false;

   if (aLengthToRead >= minLayer2Length)
   {
      DisUint8  layerNumber = 0;
      DisUint8  layerInfo   = 0;
      DisUint16 layerLength = 0;

      aGenI >> layerNumber;
      aGenI >> layerInfo;
      aGenI >> layerLength;
      aLengthToRead -= 4;

      if (layerNumber == 2 && layerLength >= minLayer2Length)
      {
         layer2Read = ReadLayer2Params(aGenI, aLengthToRead, layerLength);
      }
   }

   if (!layer2Read)
   {
      UnsetLayerPresent(2);
      ResetLayer2Data();
   }

   return layer2Read;
}

bool DisIff::ReadLayer2Params(GenI& aGenI, DisUint16& aLengthToRead, DisUint16 aLayerLength)
{
   aGenI >> mBeamDataRecord;
   aGenI >> mLayer2Parameter1;
   aGenI >> mLayer2Parameter2;
   aGenI >> mParametersCount;
   aLengthToRead -= 24;

   if (aLengthToRead >= mParametersCount * 24 && aLayerLength == minLayer2Length + mParametersCount * 24)
   {
      mParametersList.Reserve(mParametersCount);
      for (int unsigned i = 0; i < mParametersCount; ++i)
      {
         DisIffAtcNavaidsParams* parametersPtr = new DisIffAtcNavaidsParams();
         aGenI >> *parametersPtr;
         mParametersList.Add(parametersPtr); // List takes ownership
         aLengthToRead -= 24;
      }

      return true;
   }
   return false;
}

bool DisIff::ReadLayer3(GenI& aGenI, DisUint16& aLengthToRead)
{
   bool layer3Read = false;

   if (aLengthToRead >= minLayer3Length)
   {
      DisUint8  layerNumber = 0;
      DisUint8  layerInfo   = 0;
      DisUint16 layerLength = 0;

      aGenI >> layerNumber;
      aGenI >> layerInfo;
      aGenI >> layerLength;
      aLengthToRead -= 4;

      if (layerNumber == 3 && layerLength >= minLayer3Length)
      {
         aGenI >> mReportingSite;
         aGenI >> mReportingApplication;
         aGenI >> mMode5Status;
         aGenI >> mPIN;
         aGenI >> mMessageFormat;
         aGenI >> mEnhancedMode1;
         aGenI >> mNationOfOrigin;
         aGenI >> mMissionCode;
         aGenI >> mSquitterStatus;
         aGenI >> mNavigationSource;
         aGenI >> mFigureOfMerit;
         DisUint8 pad8;
         aGenI >> pad8;

         aGenI >> mNumMode5Records;

         aLengthToRead -= 24;
         // mMode5RecordList.SetCapacity(mNumMode5Records);
         for (int unsigned i = 0; i < mNumMode5Records; ++i)
         {
            // TBD will have to set up to read the records!!!
            // DisIffAtcNavaidsParams* parametersPtr =
            // new DisIffAtcNavaidsParams();
            // if ((lengthToRead > lengthRead) &&
            //((lengthToRead - lengthRead) >= 24))
            //{
            // lengthRead += 24;
            // aGenI >> *parametersPtr;
            // }
            //  List takes ownership
            // mParametersList.AddToEnd(parametersPtr);
         }
         layer3Read = true;
      }
   }

   if (!layer3Read)
   {
      UnsetLayerPresent(3);
      ResetLayer3Data();
   }

   return layer3Read;
}

bool DisIff::ReadLayer5(GenI& aGenI, DisUint16& aLengthToRead)
{
   bool layer5Read = false;

   if (aLengthToRead >= minLayer5Length)
   {
      DisUint8  layerNumber = 0;
      DisUint8  layerInfo   = 0;
      DisUint16 layerLength = 0;

      aGenI >> layerNumber;
      aGenI >> layerInfo;
      aGenI >> layerLength;
      aLengthToRead -= 4;

      if (layerNumber == 5 && layerLength >= minLayer5Length)
      {
         DisUint16 reportingSite;
         DisUint16 reportingApp;
         DisUint16 pad16;
         DisUint8  applicableLayers;
         DisEnum8  dataCategory;
         DisUint16 numRecords;

         aGenI >> reportingSite;
         aGenI >> reportingApp;
         aGenI >> pad16;
         aGenI >> applicableLayers;
         aGenI >> dataCategory;
         aGenI >> pad16;
         aGenI >> numRecords;
         aLengthToRead -= 12;

         layer5Read = ReadLayer5Records(aGenI, aLengthToRead, numRecords);
      }
   }

   if (!layer5Read)
   {
      UnsetLayerPresent(5);
      ResetLayer5Data();
   }

   return layer5Read;
}

bool DisIff::ReadLayer5Records(GenI& aGenI, DisUint16& aLengthToRead, DisUint16 aNumRecords)
{
   for (DisUint16 i = 0; i < aNumRecords; ++i)
   {
      DisEnum32 recordType;
      DisUint16 recordLength;
      aGenI >> recordType;
      aGenI >> recordLength;

      if (recordType == DisAisPositionData::GetRecordType())
      {
         if (recordLength == DisAisPositionData::GetRecordLength() && aLengthToRead >= recordLength)
         {
            DisAisPositionData* tmp = mAisPositionData;
            mAisPositionData        = new DisAisPositionData;
            delete tmp;
            mAisPositionData->Get(aGenI);
            aLengthToRead -= recordLength;
         }
         else
         {
            aLengthToRead -= 3;
            return false;
         }
      }
      else if (recordType == DisAisStaticData::GetRecordType())
      {
         if (recordLength == DisAisStaticData::GetRecordLength() && aLengthToRead >= recordLength)
         {
            DisAisStaticData* tmp = mAisStaticData;
            mAisStaticData        = new DisAisStaticData;
            delete tmp;
            mAisStaticData->Get(aGenI);
            aLengthToRead -= recordLength;
         }
         else
         {
            aLengthToRead -= 3;
            return false;
         }
      }
      else
      {
         aLengthToRead -= 3;
         return false;
      }
   }

   return true;
}

void DisIff::ResetLayer2Data()
{
   mBeamDataRecord   = DisBeamDataRecord();
   mLayer2Parameter1 = 0;
   mLayer2Parameter2 = 0;
   RemoveAllParameters();
   mParametersCount = 0;
}

void DisIff::ResetLayer3Data()
{
   mReportingSite        = 0;
   mReportingApplication = 0;
   mMode5Status          = 0;
   mPIN                  = 0;
   mMessageFormat        = 0;
   mEnhancedMode1        = 0;
   mNationOfOrigin       = 0;
   mMissionCode          = 0;
   mSquitterStatus       = 0;
   mNavigationSource     = 0;
   mFigureOfMerit        = 0;
   mNumMode5Records      = 0;
}

void DisIff::ResetLayer5Data()
{
   delete mAisPositionData;
   mAisPositionData = nullptr;
   delete mAisStaticData;
   mAisStaticData = nullptr;
}
