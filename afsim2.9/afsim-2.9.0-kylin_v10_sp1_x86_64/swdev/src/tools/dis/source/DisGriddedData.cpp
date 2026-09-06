// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisGriddedData.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGriddedData::DisGriddedData()
{
   SetPduType(DisEnum::Pdu::Type::GriddedData);
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisGriddedData::DisGriddedData(const DisGriddedData& aSrc)
   : DisPdu(aSrc)
   , mEnvironmentalSimulationId(aSrc.mEnvironmentalSimulationId)
   , mFieldNumber(aSrc.mFieldNumber)
   , mPduNumber(aSrc.mPduNumber)
   , mPduTotal(aSrc.mPduTotal)
   , mCoordinateSystem(aSrc.mCoordinateSystem)
   , mNumberOfGridAxis(aSrc.mNumberOfGridAxis)
   , mConstantGrid(aSrc.mConstantGrid)
   , mEnvironmentType(aSrc.mEnvironmentType)
   , mSampleTime(aSrc.mSampleTime)
   , mTotalValue(aSrc.mTotalValue)
   , mVectorDimension(aSrc.mVectorDimension)
   , mPadding1(aSrc.mPadding1)
   , mPadding2(aSrc.mPadding2)
   , mGridAccessDescriptors(aSrc.mGridAccessDescriptors)
   , mGridData(aSrc.mGridData)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = aSrc.mOrientation[i];
   }

   SetPduType(DisEnum::Pdu::Type::GriddedData);
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   GetLength();
}

DisGriddedData::DisGriddedData(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisGriddedData* DisGriddedData::Clone() const
{
   return new DisGriddedData(*this);
}

int DisGriddedData::GetClass() const
{
   return DisEnum::Pdu::Type::GriddedData;
}

int DisGriddedData::GetFamily() const
{
   return DisEnum::Pdu::Family::SyntheticEnvironment;
}

void DisGriddedData::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisGriddedData::GetLength()
{
   DisUint16                                               length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   std::vector<DisGridAxisDescriptorBaseRecord*>::iterator descriptorsIter(mGridAccessDescriptors.GetBegin());
   while (descriptorsIter != mGridAccessDescriptors.GetEnd())
   {
      length += (*descriptorsIter++)->GetLength();
   }

   std::vector<DisGridDataRecord*>::iterator iterator(mGridData.GetBegin());
   while (iterator != mGridData.GetEnd())
   {
      length += (*iterator++)->GetLength();
   }

   SetLength(length);

   return length;
}

void DisGriddedData::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mEnvironmentalSimulationId.Put(aGenO);
   aGenO << mFieldNumber;
   aGenO << mPduNumber;
   aGenO << mPduTotal;
   aGenO << mCoordinateSystem;
   aGenO << mNumberOfGridAxis;
   aGenO << mConstantGrid;
   mEnvironmentType.Put(aGenO);
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   mSampleTime.Put(aGenO);
   aGenO << mTotalValue;
   aGenO << mVectorDimension;
   aGenO << mPadding1;
   aGenO << mPadding2;

   std::vector<DisGridAxisDescriptorBaseRecord*>::iterator descriptorsIter(mGridAccessDescriptors.GetBegin());
   while (descriptorsIter != mGridAccessDescriptors.GetEnd())
   {
      (*descriptorsIter++)->Put(aGenO);
   }

   if (mGridData.GetSize() > 0)
   {
      std::vector<DisGridDataRecord*>::iterator dataRecordIterator(mGridData.GetBegin());
      while (dataRecordIterator != mGridData.GetEnd())
      {
         (*dataRecordIterator++)->Put(aGenO);
      }
   }
}

void DisGriddedData::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

const DisSimulationId DisGriddedData::GetEnvironmentalSimulationId() const
{
   return mEnvironmentalSimulationId;
}

DisUint16 DisGriddedData::GetFieldNumber() const
{
   return mFieldNumber;
}

DisUint16 DisGriddedData::GetPduNumber() const
{
   return mPduNumber;
}

DisUint16 DisGriddedData::GetPduTotal() const
{
   return mPduTotal;
}

const DisCoordinateSystemEnum DisGriddedData::GetCoordinateSystem() const
{
   switch (mCoordinateSystem)
   {
   case 0:
   {
      return DisCoordinateSystemEnum::cRIGHT_HANDED_CARTESIAN;
      break;
   }
   case 1:
   {
      return DisCoordinateSystemEnum::cLEFT_HANDED_CARTESIAN;
      break;
   }
   case 2:
   {
      return DisCoordinateSystemEnum::cLATITUDE_LONGITUDE_HEIGHT;
      break;
   }
   case 3:
   {
      return DisCoordinateSystemEnum::cLATITUDE_LONGITUDE_DEPTH;
      break;
   }
   default:
   {
      // This may need to be changed later
      return DisCoordinateSystemEnum::cRIGHT_HANDED_CARTESIAN;
      break;
   }
   }
}

DisUint8 DisGriddedData::GetNumberOfGridAxis() const
{
   return mNumberOfGridAxis;
}

DisEnum8 DisGriddedData::GetConstantGrid() const
{
   return mConstantGrid;
}

const DisEntityType DisGriddedData::GetEnvironmentType() const
{
   return mEnvironmentType;
}

void DisGriddedData::GetOrientation(DisFloat32& aOrientationX, DisFloat32& aOrientationY, DisFloat32& aOrientationZ) const
{
   aOrientationX = mOrientation[0];
   aOrientationY = mOrientation[1];
   aOrientationZ = mOrientation[2];
}

const DisClockTime DisGriddedData::GetSampleTime() const
{
   return mSampleTime;
}

DisUint32 DisGriddedData::GetTotalValue() const
{
   return mTotalValue;
}

DisUint8 DisGriddedData::GetVectorDimension() const
{
   return mVectorDimension;
}

const DisGridAxisDescriptorBaseRecord& DisGriddedData::GetGridAccessDescriptorAtIndex(int aIndex) const
{
   if (aIndex >= 0 && aIndex < static_cast<int>(mGridAccessDescriptors.GetSize()))
   {
      return *mGridAccessDescriptors[aIndex];
   }

   return *ut::make_unique<DisGridAxisDescriptorBaseRecord>();
}

DisPtrContainer<DisGridDataRecord>& DisGriddedData::GetGridData()
{
   return mGridData;
}

void DisGriddedData::SetEnvironmentalSimulationId(const DisSimulationId& aEnvironmentalSimulationId)
{
   mEnvironmentalSimulationId = aEnvironmentalSimulationId;
}

void DisGriddedData::SetFieldNumber(DisUint16 aFieldNumber)
{
   mFieldNumber = aFieldNumber;
}

void DisGriddedData::SetPduNumber(DisUint16 aPduNumber)
{
   mPduNumber = aPduNumber;
}

void DisGriddedData::SetPduTotal(DisUint16 aPduTotal)
{
   mPduTotal = aPduTotal;
}

void DisGriddedData::SetCoordinateSystem(const DisCoordinateSystemEnum aCoordinateSystem)
{
   mCoordinateSystem = static_cast<DisUint16>(aCoordinateSystem);
}

void DisGriddedData::SetNumberOfGridAxis(DisUint8 aNumberOfGridAxis)
{
   mNumberOfGridAxis = aNumberOfGridAxis;
}

void DisGriddedData::SetConstantGrid(DisEnum8 aConstantGrid)
{
   mConstantGrid = aConstantGrid;
}

void DisGriddedData::SetEnvironmentType(const DisEntityType& aEnvironmentType)
{
   mEnvironmentType = aEnvironmentType;
}

void DisGriddedData::SetOrientation(DisFloat32 aOrientationX, DisFloat32 aOrientationY, DisFloat32 aOrientationZ)
{
   mOrientation[0] = aOrientationX;
   mOrientation[1] = aOrientationY;
   mOrientation[2] = aOrientationZ;
}

void DisGriddedData::SetSampleTime(DisClockTime aSampleTime)
{
   mSampleTime = aSampleTime;
}

void DisGriddedData::SetTotalValue(DisUint32 aTotalValue)
{
   mTotalValue = aTotalValue;
}

void DisGriddedData::SetVectorDimension(DisUint8 aVectorDimension)
{
   mVectorDimension = aVectorDimension;
}

void DisGriddedData::SetGridAccessDescriptor(const DisPtrContainer<DisGridAxisDescriptorBaseRecord>& aGridAccessDescriptors)
{
   mGridAccessDescriptors = aGridAccessDescriptors;
}

void DisGriddedData::SetGridData(const DisPtrContainer<DisGridDataRecord>& aGridData)
{
   mGridData = aGridData;
}

bool DisGriddedData::IsValid() const
{
   bool isValid = mEnvironmentalSimulationId.IsValid() && mEnvironmentType.IsValid() &&
                  ValidateScalarArray(mOrientation, 3) && mSampleTime.IsValid();

   std::vector<DisGridAxisDescriptorBaseRecord*>::const_iterator descriptorIterator(mGridAccessDescriptors.GetBegin());
   while (descriptorIterator != mGridAccessDescriptors.GetEnd())
   {
      isValid &= (*descriptorIterator)->IsValid();
   }

   std::vector<DisGridDataRecord*>::const_iterator dataRecordIterator(mGridData.GetBegin());
   while (dataRecordIterator != mGridData.GetEnd())
   {
      isValid &= (*dataRecordIterator)->IsValid();
   }
   return isValid;
}

std::string DisGriddedData::GetStringId() const
{
   return "DisGriddedData";
}

std::string DisGriddedData::ToString() const
{
   std::string returnString = "NOT IMPLEMENTED";
   return returnString;
}

void DisGriddedData::GetMemberData(GenI& aGenI)
{
   mEnvironmentalSimulationId.Get(aGenI);
   aGenI >> mFieldNumber;
   aGenI >> mPduNumber;
   aGenI >> mPduTotal;
   aGenI >> mCoordinateSystem;
   aGenI >> mNumberOfGridAxis;
   aGenI >> mConstantGrid;
   mEnvironmentType.Get(aGenI);
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   mSampleTime.Get(aGenI);
   aGenI >> mTotalValue;
   aGenI >> mVectorDimension;
   aGenI >> mPadding1;
   aGenI >> mPadding2;

   for (int i = 0; i < mNumberOfGridAxis; i++)
   {
      auto newDescriptor = ut::make_unique<DisGridAxisDescriptorBaseRecord>(DisGridAxisDescriptorBaseRecord(aGenI));
      mGridAccessDescriptors.Add(newDescriptor.get());
   }

   for (int i = 0; i < mNumberOfGridAxis; i++)
   {
      auto newRecord = ut::make_unique<DisGridDataRecord>(DisGridDataRecord(aGenI));
      mGridData.Add(newRecord.get());
   }
   GetLength();
}
