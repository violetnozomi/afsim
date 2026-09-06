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

#include "DisGeneralAppearanceRecord.hpp"

#include <iostream>

#include "DisGeneralAppearanceRecordEnums.hpp"
#include "DisValidationUtils.hpp"
namespace
{
const DisUint32 cPERCENT_COMPLETE_MASK = 0x000000ff; // bit 0 - 7
const DisUint32 cDAMAGE_MASK           = 0x00000300; // bit 8&9
const DisUint32 cPREDISTRIBUTED_MASK   = 0x00000400; // bit 10
const DisUint32 cSTATE_MASK            = 0x00000800; // bit 11
const DisUint32 cIS_SMOKING_MASK       = 0x00001000; // bit 12
const DisUint32 cIS_FLAMING_MASK       = 0x00002000; // bit 13
const DisUint32 cIED_PRESENT_MASK      = 0x0000C000; // bit 14&15
} // namespace

DisGeneralAppearanceRecord::DisGeneralAppearanceRecord(DisUint16 aValue)
   : mGeneralAppearanceRecord(aValue){};

std::string DisGeneralAppearanceRecord::ToString() const
{
   std::string returnString = "General Appearance Record:\n";
   returnString += "   Percent Complete:           " + std::to_string(GetPercentComplete()) + '\n';
   returnString += "   Damage:                     " +
                   DisEnum::GeneralAppearanceRecord::ObjectDamage::ToString(
                      (DisEnum::GeneralAppearanceRecord::ObjectDamage::Enum)GetDamage()) +
                   '\n';
   returnString += "   Predistributed:             " +
                   DisEnum::GeneralAppearanceRecord::Predistributed::ToString(
                      (DisEnum::GeneralAppearanceRecord::Predistributed::Enum)Predistributed()) +
                   '\n';
   returnString += "   State:                      " +
                   DisEnum::GeneralAppearanceRecord::ObjectState::ToString(
                      (DisEnum::GeneralAppearanceRecord::ObjectState::Enum)GetState()) +
                   '\n';
   returnString += "   Smoking:                    ";
   if (IsSmoking())
   {
      returnString += "True";
   }
   else
   {
      returnString += "False";
   }
   returnString += '\n';
   returnString += "   Flaming:                    ";
   if (IsFlaming())
   {
      returnString += "True";
   }
   else
   {
      returnString += "False";
   }
   returnString += '\n';
   returnString += "   IED Present:                " +
                   DisEnum::GeneralAppearanceRecord::IEDPresent::ToString(
                      (DisEnum::GeneralAppearanceRecord::IEDPresent::Enum)IEDPresent()) +
                   '\n';
   return returnString;
}

void DisGeneralAppearanceRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << '\n';
}

DisGeneralAppearanceRecord* DisGeneralAppearanceRecord::Clone() const
{
   return new DisGeneralAppearanceRecord(*this);
}

void DisGeneralAppearanceRecord::Get(GenI& aGenI)
{
   aGenI >> mGeneralAppearanceRecord;
}

void DisGeneralAppearanceRecord::Put(GenO& aGenO) const
{
   aGenO << mGeneralAppearanceRecord;
}

bool DisGeneralAppearanceRecord::IsValid() const
{
   return true;
}

bool DisGeneralAppearanceRecord::operator==(const DisGeneralAppearanceRecord& aRhs) const
{
   return mGeneralAppearanceRecord == aRhs.mGeneralAppearanceRecord;
}

// The right-most integer in the expressions below are the number of bit shifts.
unsigned int DisGeneralAppearanceRecord::GetPercentComplete() const
{
   return ((mGeneralAppearanceRecord & cPERCENT_COMPLETE_MASK) >> 0);
}
unsigned int DisGeneralAppearanceRecord::GetDamage() const
{
   return ((mGeneralAppearanceRecord & cDAMAGE_MASK) >> 8);
}
unsigned int DisGeneralAppearanceRecord::Predistributed() const
{
   return ((mGeneralAppearanceRecord & cPREDISTRIBUTED_MASK) >> 10);
}
unsigned int DisGeneralAppearanceRecord::GetState() const
{
   return ((mGeneralAppearanceRecord & cSTATE_MASK) >> 11);
}
unsigned int DisGeneralAppearanceRecord::IsSmoking() const
{
   return ((mGeneralAppearanceRecord & cIS_SMOKING_MASK) >> 12);
}
unsigned int DisGeneralAppearanceRecord::IsFlaming() const
{
   return ((mGeneralAppearanceRecord & cIS_FLAMING_MASK) >> 13);
}
unsigned int DisGeneralAppearanceRecord::IEDPresent() const
{
   return ((mGeneralAppearanceRecord & cIED_PRESENT_MASK) >> 14);
}

void DisGeneralAppearanceRecord::SetPercentComplete(unsigned int aValue)
{
   mGeneralAppearanceRecord =
      ((aValue << 0) & cPERCENT_COMPLETE_MASK) | (mGeneralAppearanceRecord & ~cPERCENT_COMPLETE_MASK);
}
void DisGeneralAppearanceRecord::SetDamage(unsigned int aValue)
{
   mGeneralAppearanceRecord = ((aValue << 8) & cDAMAGE_MASK) | (mGeneralAppearanceRecord & ~cDAMAGE_MASK);
};
void DisGeneralAppearanceRecord::SetPredistributed(unsigned int aValue)
{
   mGeneralAppearanceRecord = ((aValue << 10) & cPREDISTRIBUTED_MASK) | (mGeneralAppearanceRecord & ~cPREDISTRIBUTED_MASK);
};
void DisGeneralAppearanceRecord::SetState(unsigned int aValue)
{
   mGeneralAppearanceRecord = ((aValue << 11) & cSTATE_MASK) | (mGeneralAppearanceRecord & ~cSTATE_MASK);
};
void DisGeneralAppearanceRecord::SetSmoking(unsigned int aValue)
{
   mGeneralAppearanceRecord = ((aValue << 12) & cIS_SMOKING_MASK) | (mGeneralAppearanceRecord & ~cIS_SMOKING_MASK);
};
void DisGeneralAppearanceRecord::SetFlaming(unsigned int aValue)
{
   mGeneralAppearanceRecord = ((aValue << 13) & cIS_FLAMING_MASK) | (mGeneralAppearanceRecord & ~cIS_FLAMING_MASK);
};
void DisGeneralAppearanceRecord::SetIEDPresent(unsigned int aValue)
{
   mGeneralAppearanceRecord = ((aValue << 14) & cIED_PRESENT_MASK) | (mGeneralAppearanceRecord & ~cIED_PRESENT_MASK);
};
