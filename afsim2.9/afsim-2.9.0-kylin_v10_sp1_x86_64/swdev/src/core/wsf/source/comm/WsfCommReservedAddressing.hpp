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

#ifndef WSFCOMMRESERVEDADDRESSING_HPP
#define WSFCOMMRESERVEDADDRESSING_HPP

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "WsfCommAddress.hpp"

namespace wsf
{
namespace comm
{
//! The following classes provide the framework for reserved addressing.
//! Any address specified directly on within a range will not be used for
//! network/host assignments by the NetworkManager.
//! In addition, specific addresses may be tagged with an identifier. This
//! provides a uniform way of retrieving/querying addresses associated with
//! a specific use case. All addresses are specific, as in the addresses are
//! expected with a CIDR value of 32 indicating a single address, and not
//! a range of addresses within any specific addresses.

//! This namespace is dedicated solely for definition of string constants
//! that identify specific reserved addresses that are used for protocols,
//! hardware, layer implementations, etc. These can be queried for their
//! specific address from the ReservedAddressing object, via the NetworkManager,
//! via the sim object.
namespace reserved
{
constexpr const char* const cBROADCAST_LOCAL       = "BROADCAST_LOCAL";
constexpr const char* const cMULTICAST_INITIAL     = "MULTICAST_INITIAL";
constexpr const char* const cMULTICAST_LAST        = "MULTICAST_LAST";
constexpr const char* const cDEFAULT_INITIAL       = "DEFAULT_INITIAL";
constexpr const char* const cDEFAULT_LAST          = "DEFAULT_LAST";
constexpr const char* const cMULTICAST_ALL_HOSTS   = "MULTICAST_ALL_HOSTS";   // All multicast hosts
constexpr const char* const cMULTICAST_ALL_ROUTERS = "MULTICAST_ALL_ROUTERS"; // All multicast routers
constexpr const char* const cRIPv2_MULTICAST       = "RIPv2_MULTICAST";       // Local Network Only
constexpr const char* const cOSPF_MULTICAST_ALL    = "OSPF_MULTICAST_ALL";    // Hello and Protocol Usage
constexpr const char* const cOSPF_MULTICAST_DRBR   = "OSPF_MULTICAST_DRBR";   // DR/BR Usage Only
} // namespace reserved

//! The abstract implementation object for reserved addressing. Users should derive
//! off of this object to instantiate their own set of reserved addresses, and register
//! their implementation with the ReservedAddressing collection object held by the
//! NetworkManager via the sim object.
class ReservedAddressingImplementation
{
public:
   ReservedAddressingImplementation()          = default;
   virtual ~ReservedAddressingImplementation() = default;

   //! It is expected that all reserved addresses are set by the conclusion of
   //! the Initialize call, however the user decides to get/define those reserved
   //! addresses.
   virtual void Initialize() = 0;

   //! Checks if the address/32 value is reserved.
   virtual bool    IsReserved(const Address& aAddress) const;
   virtual Address QueryAddressByType(const std::string& aReservedType) const;

protected:
   //! Checks the simple case of an address/32 collision.
   virtual bool IsReservedSimple(const Address& aAddress) const;

   //! A struct used for correct hashing of std::pair<Address, Address>, used
   //! specifically in the context for these classes. This was chosen instead of
   //! specializing the template inside namespace std to avoid generalizing this
   //! use case.
   struct AddressPairHash
   {
      std::size_t operator()(const std::pair<Address, Address>& aKey) const
      {
         auto hash1        = std::hash<Address>{}(aKey.first);
         auto hash2        = std::hash<Address>{}(aKey.second);
         auto combineHash1 = hash1 ^ (hash2 << 1);
         auto combineHash2 = hash2 ^ (hash1 << 1);

         return std::min(combineHash1, combineHash2);
      }
   };

   using ReservedSet     = std::unordered_set<Address>;
   using ReservedPairSet = std::unordered_set<std::pair<Address, Address>, AddressPairHash>;
   using ReservedTypeMap = std::unordered_map<std::string, Address>;

   ReservedSet     mReservedAddressSet;
   ReservedPairSet mReservedAddressRangeSet;
   ReservedTypeMap mReservedTypeToAddressMap;
};

//! The ReservedAddressing object is a collection of registered and defined derivations
//! of the implementation object. All registered objects are queried via standard interface
//! methods for extension of reserved addressing sets. Currently, this object does not
//! ensure individual instances do not overlap in their reserved set definitions, and may
//! need such functionality in the future.
class ReservedAddressing
{
public:
   using UniqueReserved = std::unique_ptr<ReservedAddressingImplementation>;

   ReservedAddressing()                               = default;
   ReservedAddressing(const ReservedAddressing& aSrc) = delete;
   ReservedAddressing& operator=(const ReservedAddressing& aSrc) = delete;

   virtual ~ReservedAddressing() = default;
   void Register(UniqueReserved aImplementation);
   void Initialize();

   bool    IsReserved(const Address& aAddress) const;
   Address QueryAddressByType(const std::string& aReservedType) const;

private:
   std::vector<UniqueReserved> mImplementations;
};

//! The default reserved addressing implementation used by AFSIM core, for usage with
//! pre-defined protocols and basic capabilities.
class ReservedAddressBase : public ReservedAddressingImplementation
{
public:
   void Initialize() override;
};

} // namespace comm
} // namespace wsf

#endif
