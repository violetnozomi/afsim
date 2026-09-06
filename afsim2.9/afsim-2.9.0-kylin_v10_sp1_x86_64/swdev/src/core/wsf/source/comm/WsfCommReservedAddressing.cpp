// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommReservedAddressing.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
bool ReservedAddressingImplementation::IsReserved(const Address& aAddress) const
{
   auto cidr = aAddress.GetSubnetMaskBitLength();
   if (cidr == IPv4::cBIT_SIZE)
   {
      return IsReservedSimple(aAddress);
   }
   else
   {
      //! The provided address is a range. Check for overlap.
      //! Check individual addresses first.
      for (const auto& address : mReservedAddressSet)
      {
         if (Address::CheckInAddressRange(address, aAddress))
         {
            return true;
         }
      }

      //! Check the defined ranges. If the first and last discrete reserved
      //! address are not within the address range, then all inclusive
      //! addresses are also not within the range, and there is not overlap.
      for (const auto& rangePair : mReservedAddressRangeSet)
      {
         if ((aAddress.GetBinaryInteger() > rangePair.first.GetBinaryInteger()) &&
             (aAddress.GetBinaryInteger() < rangePair.second.GetBinaryInteger()))
         {
            return true;
         }
      }
   }

   return false;
}

// =================================================================================================
bool ReservedAddressingImplementation::IsReservedSimple(const Address& aAddress) const
{
   auto it = mReservedAddressSet.find(aAddress);
   if (it != std::end(mReservedAddressSet))
   {
      return true;
   }

   for (auto setIt = std::begin(mReservedAddressRangeSet); setIt != std::end(mReservedAddressRangeSet); ++setIt)
   {
      auto firstVal  = setIt->first.GetBinaryInteger();
      auto secondVal = setIt->second.GetBinaryInteger();
      auto checkVal  = aAddress.GetBinaryInteger();
      if ((checkVal >= firstVal) && (checkVal <= secondVal))
      {
         return true;
      }
   }

   return false;
}

// =================================================================================================
Address ReservedAddressingImplementation::QueryAddressByType(const std::string& aReservedType) const
{
   auto it = mReservedTypeToAddressMap.find(aReservedType);

   if (it != std::end(mReservedTypeToAddressMap))
   {
      return it->second;
   }

   return Address{};
}

// =================================================================================================
bool ReservedAddressing::IsReserved(const Address& aAddress) const
{
   for (auto& implementation : mImplementations)
   {
      if (implementation->IsReserved(aAddress))
      {
         return true;
      }
   }

   return false;
}

// =================================================================================================
Address ReservedAddressing::QueryAddressByType(const std::string& aReservedType) const
{
   Address returnAddress;
   for (auto& implementation : mImplementations)
   {
      returnAddress = implementation->QueryAddressByType(aReservedType);
      if (!returnAddress.IsNull())
      {
         return returnAddress;
      }
   }

   return returnAddress;
}

// =================================================================================================
void ReservedAddressing::Register(UniqueReserved aImplementation)
{
   mImplementations.push_back(std::move(aImplementation));
}

// =================================================================================================
void ReservedAddressing::Initialize()
{
   for (auto& implementation : mImplementations)
   {
      implementation->Initialize();
   }
}

// =================================================================================================
void ReservedAddressBase::Initialize()
{
   //! Reserve the addresses used for multicasting.
   Address multicastBegin("224.0.0.0", 32);
   Address multicastEnd("239.255.255.255", 32);
   mReservedAddressRangeSet.emplace(multicastBegin, multicastEnd);
   mReservedTypeToAddressMap[reserved::cMULTICAST_INITIAL]     = multicastBegin;
   mReservedTypeToAddressMap[reserved::cMULTICAST_LAST]        = multicastEnd;
   mReservedTypeToAddressMap[reserved::cMULTICAST_ALL_HOSTS]   = Address("224.0.0.1", 32);
   mReservedTypeToAddressMap[reserved::cMULTICAST_ALL_ROUTERS] = Address("224.0.0.2", 32);
   mReservedTypeToAddressMap[reserved::cOSPF_MULTICAST_ALL]    = Address("224.0.0.5", 32);
   mReservedTypeToAddressMap[reserved::cOSPF_MULTICAST_DRBR]   = Address("224.0.0.6", 32);
   mReservedTypeToAddressMap[reserved::cRIPv2_MULTICAST]       = Address("224.0.0.9", 32);

   //! Reserve the addresses used for the "DEFAULT" AFSIM network subnet,
   //! where all AFSIM comm objects get assigned to when not explicitly designated (legacy)
   Address defaultBegin("0.1.0.0", 32);
   Address defaultEnd("0.1.255.255", 32);
   mReservedAddressRangeSet.emplace(defaultBegin, defaultEnd);
   mReservedTypeToAddressMap[reserved::cDEFAULT_INITIAL] = defaultBegin;
   mReservedTypeToAddressMap[reserved::cDEFAULT_LAST]    = defaultEnd;

   //! Reserve the "this network" broadcast address
   Address broadcastLocal("255.255.255.255", 32);
   mReservedAddressSet.emplace(broadcastLocal);
   mReservedTypeToAddressMap[reserved::cBROADCAST_LOCAL] = broadcastLocal;
}

} // namespace comm
} // namespace wsf
