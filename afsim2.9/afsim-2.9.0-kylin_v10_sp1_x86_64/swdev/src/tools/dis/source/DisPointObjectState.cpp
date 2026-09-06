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

#include "DisPointObjectState.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisPointObjectState::DisPointObjectState()
   : DisPdu()
   , mObjectId()
{
   SetPduType(DisEnum::Pdu::Type::PointObjectState);
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   for (int i = 0; i < 3; i++)
   {
      mObjectLocation[i] = 0;
      mOrientation[i]    = 0;
   }
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisPointObjectState::DisPointObjectState(const DisPointObjectState& aSrc)
   : DisPdu(aSrc)
   , mObjectId(aSrc.mObjectId)
   , mReferencedObjectId(aSrc.mReferencedObjectId)
   , mUpdateNumber(aSrc.mUpdateNumber)
   , mForceId(aSrc.mForceId)
   , mModifications(aSrc.mModifications)
   , mObjectType(aSrc.mObjectType)
   , mSpecificObjectAppearance(aSrc.mSpecificObjectAppearance)
   , mGeneralObjectAppearance(aSrc.mGeneralObjectAppearance)
   , mRequesterSimulationId(aSrc.mRequesterSimulationId)
   , mReceivingSimulationId(aSrc.mReceivingSimulationId)
{
   SetPduType(DisEnum::Pdu::Type::PointObjectState);
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   for (int i = 0; i < 3; i++)
   {
      mObjectLocation[i] = aSrc.mObjectLocation[i];
      mOrientation[i]    = aSrc.mOrientation[i];
   }
   GetLength();
}

DisPointObjectState::DisPointObjectState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisPointObjectState* DisPointObjectState::Clone() const
{
   return new DisPointObjectState(*this);
}

int DisPointObjectState::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::PointObjectState);
}

int DisPointObjectState::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::SyntheticEnvironment);
}

void DisPointObjectState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisPointObjectState::GetLength()
{
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;

   SetLength(length);

   return length;
}

void DisPointObjectState::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mObjectId.Put(aGenO);
   mReferencedObjectId.Put(aGenO);
   aGenO << mUpdateNumber;
   aGenO << mForceId;
   aGenO << mModifications;
   mObjectType.Put(aGenO);
   aGenO << mObjectLocation[0] << mObjectLocation[1] << mObjectLocation[2];
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   mSpecificObjectAppearance.Put(aGenO);
   mGeneralObjectAppearance.Put(aGenO);
   aGenO << mPadding;
   mRequesterSimulationId.Put(aGenO);
   mReceivingSimulationId.Put(aGenO);
   aGenO << mPadding2;
}

const DisEntityId DisPointObjectState::GetObjectId() const
{
   return mObjectId;
}

const DisEntityId DisPointObjectState::GetReferencedObjectId() const
{
   return mReferencedObjectId;
}

DisUint16 DisPointObjectState::GetUpdateNumber() const
{
   return mUpdateNumber;
}

DisEnum8 DisPointObjectState::GetForceId() const
{
   return mForceId;
}

DisEnum8 DisPointObjectState::GetModifications() const
{
   return mModifications;
}

const DisObjectType DisPointObjectState::GetObjectType() const
{
   return mObjectType;
}

void DisPointObjectState::GetObjectLocation(DisFloat64& aObjectLocationX,
                                            DisFloat64& aObjectLocationY,
                                            DisFloat64& aObjectLocationZ) const
{
   aObjectLocationX = mObjectLocation[0];
   aObjectLocationY = mObjectLocation[1];
   aObjectLocationZ = mObjectLocation[2];
}

void DisPointObjectState::GetObjectOrientation(DisFloat32& aObjectOrientationX,
                                               DisFloat32& aObjectOrientationY,
                                               DisFloat32& aObjectOrientationZ) const
{
   aObjectOrientationX = mOrientation[0];
   aObjectOrientationY = mOrientation[1];
   aObjectOrientationZ = mOrientation[2];
}

const DisEntityAppearance DisPointObjectState::GetSpecificObjectAppearance() const
{
   return mSpecificObjectAppearance;
}

const DisGeneralAppearanceRecord DisPointObjectState::GetGeneralObjectAppearance() const
{
   return mGeneralObjectAppearance;
}

const DisSimulationAddressRecord DisPointObjectState::GetRequesterSimulationId() const
{
   return mRequesterSimulationId;
}

const DisSimulationAddressRecord DisPointObjectState::GetReceivingSimulationId() const
{
   return mReceivingSimulationId;
}

void DisPointObjectState::SetObjectId(const DisEntityId& aObjectId)
{
   mObjectId = aObjectId;
}

void DisPointObjectState::SetReferenceObjectId(const DisEntityId& aObjectReferenceId)
{
   mReferencedObjectId = aObjectReferenceId;
}

void DisPointObjectState::SetUpdateNumber(DisUint16 aUpdateNumber)
{
   mUpdateNumber = aUpdateNumber;
}

void DisPointObjectState::SetForceId(DisEnum8 aForceId)
{
   mForceId = aForceId;
}

void DisPointObjectState::SetModifications(DisEnum8 aModifications)
{
   mModifications = aModifications;
}

void DisPointObjectState::SetObjectType(const DisObjectType& aObjectType)
{
   mObjectType = aObjectType;
}

void DisPointObjectState::SetObjectLocation(DisFloat64 aObjectLocationX, DisFloat64 aObjectLocationY, DisFloat64 aObjectLocationZ)
{
   mObjectLocation[0] = aObjectLocationX;
   mObjectLocation[1] = aObjectLocationY;
   mObjectLocation[2] = aObjectLocationZ;
}

void DisPointObjectState::SetObjectOrientation(DisFloat32 aObjectOrientationX,
                                               DisFloat32 aObjectOrientationY,
                                               DisFloat32 aObjectOrientationZ)
{
   mOrientation[0] = aObjectOrientationX;
   mOrientation[1] = aObjectOrientationY;
   mOrientation[2] = aObjectOrientationZ;
}

void DisPointObjectState::SetSpecificObjectAppearance(const DisEntityAppearance& aSpecificObjectAppearance)
{
   mSpecificObjectAppearance = aSpecificObjectAppearance;
}

void DisPointObjectState::SetGeneralObjectAppearance(const DisGeneralAppearanceRecord& aGeneralObjectAppearance)
{
   mGeneralObjectAppearance = aGeneralObjectAppearance;
}

void DisPointObjectState::SetRequesterSimulationId(const DisSimulationAddressRecord& aRequesterSimulationId)
{
   mRequesterSimulationId = aRequesterSimulationId;
}

void DisPointObjectState::SetReceivingSimulationId(const DisSimulationAddressRecord& aRecievingSimulationId)
{
   mReceivingSimulationId = aRecievingSimulationId;
}

bool DisPointObjectState::IsValid() const
{
   bool isValid = DisPdu::IsValid() && mObjectId.IsValid() && mReferencedObjectId.IsValid() && mObjectType.IsValid() &&
                  mSpecificObjectAppearance.IsValid() && mRequesterSimulationId.IsValid() &&
                  mReceivingSimulationId.IsValid();

   for (int i = 0; i < 3; i++)
   {
      if (!ValidateScalar(mObjectLocation[i]))
      {
         isValid = false;
      }
      if (!ValidateScalar(mOrientation[i]))
      {
         isValid = false;
      }
   }
   return isValid;
}

std::string DisPointObjectState::GetStringId() const
{
   return "DisPointObjectState";
}

std::string DisPointObjectState::ToString() const
{
   std::string spacing      = "   ";
   std::string returnString = "-------Point Object State-------\n";
   returnString += "Object Id: \n";
   returnString += mObjectId.ToString();
   returnString += "Object Reference Id: \n";
   returnString += mReferencedObjectId.ToString();
   returnString += "Update Number: " + std::to_string(mUpdateNumber) + "\n";
   returnString += "Force Id: " + std::to_string(mForceId) + "\n";
   returnString += "Modifications: " + std::to_string(mModifications) + "\n";
   returnString += mObjectType.ToString();
   returnString += "-------Object Location-------\n";
   returnString += "   X: " + std::to_string(mObjectLocation[0]) + "\n";
   returnString += "   Y: " + std::to_string(mObjectLocation[1]) + "\n";
   returnString += "   Z: " + std::to_string(mObjectLocation[2]) + "\n";
   returnString += "-----End Object Location-----\n";
   returnString += "-------Object Orientation-------\n";
   returnString += "   X: " + std::to_string(mOrientation[0]) + "\n";
   returnString += "   Y: " + std::to_string(mOrientation[1]) + "\n";
   returnString += "   Z: " + std::to_string(mOrientation[2]) + "\n";
   returnString += "-----End Object Orientation-----\n";
   returnString += "   Specific Object Appearance: \n";
   returnString += mSpecificObjectAppearance.ToString(GetObjectType().GetObjectKind(), GetObjectType().GetDomain());
   returnString += "   General Object Appearance: \n";
   returnString += spacing + mGeneralObjectAppearance.ToString();
   returnString += "   Requesting Simulation Id: \n";
   returnString += mRequesterSimulationId.ToString();
   returnString += "   Receiving Simulation Id: \n";
   returnString += mReceivingSimulationId.ToString();
   returnString += "-----End Point Object State-----\n";
   return returnString;
}

void DisPointObjectState::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisPointObjectState::GetMemberData(GenI& aGenI)
{
   mObjectId.Get(aGenI);
   mReferencedObjectId.Get(aGenI);
   aGenI >> mUpdateNumber;
   aGenI >> mForceId;
   aGenI >> mModifications;
   mObjectType.Get(aGenI);
   aGenI >> mObjectLocation[0] >> mObjectLocation[1] >> mObjectLocation[2];
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   mSpecificObjectAppearance.Get(aGenI);
   mGeneralObjectAppearance.Get(aGenI);
   aGenI >> mPadding;
   mRequesterSimulationId.Get(aGenI);
   mReceivingSimulationId.Get(aGenI);
   aGenI >> mPadding2;
   GetLength();
}
