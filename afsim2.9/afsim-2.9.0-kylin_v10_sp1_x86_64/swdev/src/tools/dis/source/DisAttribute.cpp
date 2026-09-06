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

#include "DisAttribute.hpp"

#include "DisAttributeRecordSet.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

// Static
const DisUint16 DisAttribute::sSizeOfFixedPortion = 32; // 256 bits - size of Attribute PDU with no records

DisAttribute::DisAttribute()
   : DisPdu()
   , mOriginatingSimulationSite(0)
   , mOriginatingSimulationApp(0)
   , mPadding1(0)
   , mPadding2(0)
   , mAttributeRecordPduType(0)
   , mAttributeRecordProtocolVersion(0)
   , mMasterAttributeRecordType(0)
   , mActionCode(0)
   , mPadding3(0)
   , mNumberOfAttributeRecordSets(0)
{
   SetPduType(DisEnum::Pdu::Type::Attribute);

   // Attribute PDU is NOT limited to extending Emissions PDUs, this is just an
   // initial value, since the Attribute PDU is being used initially in our Sim
   // for 3 emission related Attribute Records.
   SetProtocolFamily(FamilyDistributedEmissionRegeneration);

   SetLength(sSizeOfFixedPortion);
}

DisAttribute::DisAttribute(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisAttribute::DisAttribute(DisPdu& aPdu)
   : DisPdu(aPdu)
{
}

DisAttribute::DisAttribute(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// Virtual
int DisAttribute::GetClass() const
{
   return DisEnum::Pdu::Type::Attribute;
}

// Virtual
const DisEntityId& DisAttribute::GetOriginatingEntity() const
{
   mOriginatingEntity.SetSite(mOriginatingSimulationSite);
   mOriginatingEntity.SetApplication(mOriginatingSimulationApp);

   return mOriginatingEntity;
}

void DisAttribute::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisAttribute::GetLength()
{
   UpdateAttributeRecordSetCount();

   DisUint16 octetCount = sSizeOfFixedPortion;

   std::vector<DisAttributeRecordSet*>::const_iterator iter(mAttributeRecordSetList.GetBegin());
   while (iter != mAttributeRecordSetList.GetEnd())
   {
      octetCount += (*iter++)->GetLength();
   }

   SetLength(octetCount);
   return octetCount;
}

void DisAttribute::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   RemoveAllAttributeRecordSets();

   aGenI >> mOriginatingSimulationSite;
   aGenI >> mOriginatingSimulationApp;
   aGenI >> mPadding1;
   aGenI >> mPadding2;
   aGenI >> mAttributeRecordPduType;
   aGenI >> mAttributeRecordProtocolVersion;
   aGenI >> mMasterAttributeRecordType;
   aGenI >> mActionCode;
   aGenI >> mPadding3;
   aGenI >> mNumberOfAttributeRecordSets;

   lengthToRead -= sSizeOfFixedPortion;

   if (mNumberOfAttributeRecordSets != 0)
   {
      mAttributeRecordSetList.Reserve(mNumberOfAttributeRecordSets);
      for (int i = 0; i < mNumberOfAttributeRecordSets; ++i)
      {
         DisAttributeRecordSet* attributeRecordSet = new DisAttributeRecordSet();
         aGenI >> *attributeRecordSet;
         mAttributeRecordSetList.Add(attributeRecordSet); // List takes ownership

         lengthToRead -= attributeRecordSet->GetLengthRead();
      }
   }

   // Skip any 'extra' data
   ReadExtraData(aGenI, lengthToRead);
}

void DisAttribute::Put(GenO& aGenO)
{
   GetLength(); // Ensure the length is current
   DisPdu::Put(aGenO);

   aGenO << mOriginatingSimulationSite;
   aGenO << mOriginatingSimulationApp;
   aGenO << mPadding1;
   aGenO << mPadding2;
   aGenO << mAttributeRecordPduType;
   aGenO << mAttributeRecordProtocolVersion;
   aGenO << mMasterAttributeRecordType;
   aGenO << mActionCode;
   aGenO << mPadding3;
   aGenO << mNumberOfAttributeRecordSets;

   for (auto iter = mAttributeRecordSetList.GetBegin(); iter != mAttributeRecordSetList.GetEnd(); iter++)
   {
      aGenO << **iter;
   }
}

void DisAttribute::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisAttribute::ToString() const
{
   using namespace std;
   using namespace DisEnum;

   std::ostringstream ss("");
   ss << "***** Attribute PDU *****\n";
   ss << DisPdu::ToString() << '\n'
      << "***** Attribute Data *****\n"
      << "Originating Simulation Site:       " << GetOriginatingSimulationSite() << '\n'
      << "Originating Simulation App:        " << GetOriginatingSimulationApp() << '\n'
      << "Attribute Record PDU Type:         " << static_cast<unsigned int>(GetAttributeRecordPduType()) << '\n'
      << "Attribute Record Protocol Version: " << static_cast<unsigned int>(GetProtocolVersion()) << '\n'
      << "Master Attribute Record Type:      " << GetMasterAttributeRecordType() << '\n'
      << "Action Code:                       " << static_cast<unsigned int>(GetActionCode()) << '\n'
      << "Number of Attribute Record Sets:   " << GetAttributeRecordSetCount() << '\n';
   // Check to ensure that the above is representative of the actual
   // size of the structure.
   // Can't use DisAttribute::GetAttributeRecordSetCount, because that
   // actually modifies mNumberOfAttributeRecordSets, the thing
   // returned above.
   size_t attrRecordSetSize = 0;
   for (vector<DisAttributeRecordSet*>::const_iterator it = GetAttributeRecordSetBegin(); it != GetAttributeRecordSetEnd();
        ++it)
   {
      ++attrRecordSetSize;
   }
   ss << "Attribute Record Set List Size:   " << attrRecordSetSize << '\n';
   size_t display_counter = 0;
   for (std::vector<DisAttributeRecordSet*>::const_iterator it = GetAttributeRecordSetBegin();
        it != GetAttributeRecordSetEnd();
        ++it)
   {
      ss << "***** Attribute Record Set " << display_counter << " Data   *****\n";
      if (*it)
      {
         ss << (**it).ToString();
      }
      else
      {
         ss << " null data pointer (this is bad)\n";
      }
      ss << "***** End of Attribute Record Set " << display_counter << " *****\n";
      ++display_counter;
   }
   ss << "***** Attribute PDU End *****";

   return ss.str();
}

bool DisAttribute::IsValid() const
{
   // Perform data validation.
   if (!DisPdu::IsValid())
   {
      return false;
   }

   auto iter = mAttributeRecordSetList.GetBegin();
   while (iter != mAttributeRecordSetList.GetEnd())
   {
      if (!(*iter++)->IsValid())
      {
         return false;
      }
   }

   return true;
}
