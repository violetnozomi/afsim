// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMADDRESS_HPP
#define WSFCOMMADDRESS_HPP

#include "wsf_export.h"

#include <array>
#include <string>
#include <vector>

#include "UtScriptAccessible.hpp"

class UtInput;

namespace wsf
{
namespace comm
{

//! Constant values for IPV4 based addressing. If future expansion to IPV6 is
//! required, then we can make this a base class with implementations of either
//! address type. The methods should be identical with the provision of the correct
//! constants provided below.
struct IPv4
{
   static constexpr size_t        cBIT_SIZE       = 32;
   static constexpr size_t        cNUM_FIELDS     = 4;
   static constexpr const char*   cDELIMITER      = ".";
   static constexpr std::uint32_t cMASK_MAX       = 0xFFFFFFFF;
   static constexpr std::uint32_t cMASK_FIELD     = 0xFF;
   static constexpr const char*   cNULL_ADDRESS   = "0.0.0.0"; // Uninitialized address indicator
   static constexpr size_t        cDEFAULT_CIDR   = 24;
   static constexpr const char*   cCIDR_DELIMITER = "/";
   using ByteArray                                = std::array<unsigned char, 4>;
};

//! WsfCommAddress represents a typical IP address, for usage within the context
//! of addressing within the Comm domain. Even though some networks
//! are not formal Internet Protocol networks and would not be subject to this
//! standard of addressing, this method is a standard that provides Comm with
//! a internally consistent means of handling addressing of all comm-capable objects
//! within AFSIM. The addressing here is consistent with the CIDR variable subnet
//! bit masking instead of the deprecated class A,B, and C domain usages. Note that
//! this class now differs from the original instantiation in that platforms are not
//! directly associated with an address within the scope of this class.
class WSF_EXPORT Address : public UtScriptAccessible
{
public:
   Address() = default;
   Address(const std::string& aAddress, size_t aSubnetMaskBitLength = IPv4::cDEFAULT_CIDR);

   //! Note: This constructor supplies an uninitialized address, but with a specified
   //! bit mask length. This allows the conveyance of a required bit length mask for
   //! an address that is to be generated at a time beyond construction, typically using
   //! GenerateAddress().
   explicit Address(size_t aSubnetMaskBitLength);

   virtual ~Address() = default;

   const char* GetScriptClassName() const override;

   void SetAddress(const std::string& aAddress, size_t aSubnetBitLength = IPv4::cDEFAULT_CIDR);

   //! Allows parsing of addresses from the input stream using a
   //! defined standard.
   //! @note Entry into this method may use various commands. It is
   //! assumed this is called only after confirming an address is
   //! next in the stream. This will handle any possible CIDR subnet
   //! usage subsequent in the stream.
   static Address ProcessInput(UtInput& aInput);

   //! Same as the ProcessInput command, expect acts on the raw string input
   //! already retrieved from a source such as a input stream.
   static Address ProcessInput(std::string aInput);

   //! Allows the checking of a string to determine if it is a valid IPV4 format
   //! address.
   static bool IsValidAddress(const std::string& aPossibleAddress);

   //! Given a possible address string, find the CIDR notation value and return it.
   //! @note CIDR notation is denoted by a valid string address format followed by
   //! a "/" and an unsigned integer value between 0 and 32. e.g 192.168.1.1/24
   //! @note This takes a reference to the possible string address. It is modified
   //! during this call to consume the CIDR mask reference value to leave the bare address.
   //! @note If no CIDR value is found, the default CIDR value of 24 will be returned.
   static size_t GetCIDR_ValueFromString(std::string& aPossibleAddress);

   //! @name GenerateAddress Method
   //! This method populates the address object with an IP address generated
   //! from a provided string token. This token can be any string, of which
   //! a procedurally generated IP address will be generated that will consistently
   //! provide the same address given a particular string. Useful for assigning
   //! initial address values, especially for named networks when no address is provided.
   void GenerateAddress(const std::string& aGenerationToken, size_t aSubnetBitLength = IPv4::cDEFAULT_CIDR);

   const std::string& GetAddress() const { return mAddress; }
   size_t             GetSubnetMaskBitLength() const { return mSubnetMaskBitLength; }
   const std::string& GetRoutingPrefix() const { return mRoutingPrefix; }
   const std::string& GetSubnet() const { return mSubnet; }
   size_t             GetNumHostAddresses() const;
   bool               IsNull() const { return (mAddress == IPv4::cNULL_ADDRESS); }
   size_t             GetBinaryInteger() const { return mBinaryIntegerValue; }

   bool operator==(const Address& aRhs) const;
   bool operator!=(const Address& aRhs) const;
   bool operator<(const Address& aRhs) const;

   //! @name Address++ and ++Address Overloads
   //! Increments the address by one. Useful when assigning new addresses, in that the highest
   //! existing address in a subnet can be copied and then simply incremented to provide a new
   //! address.
   //! @note These do NOT check if the increment will result in a new address that changes the
   //! routing prefix of the address (essentially moving the address out of a current subnet).
   //! Any calling object should ensure that the resulting address is still viable by comparing
   //! the routing prefix.
   Address& operator++();    //++Address
   Address  operator++(int); // Address++

   friend WSF_EXPORT std::ostream& operator<<(std::ostream& aOut, const Address& aAddress);

   //! For XIO usage
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mAddress& mRoutingPrefix& mSubnet& mSubnetMaskBitLength;
   }

   //! A static method that provides an unsigned integer value from a string. Uses the same
   //! value assignment methodology as UtDictionary, and produces a consistent and repeatable
   //! value given a specific string.
   static size_t GetStringValue(const std::string& aString);

   //! A static method for checking whether or not a provided address falls within the
   //! range of addresses of a network address using CIDR notation. Be mindful of the order
   //! of addresses provided, as the CIDR value of the network address and its subnet mask
   //! is used without checking for consistency with the comparing address.
   static bool CheckInAddressRange(const Address& aNetworkAddress, const Address& aCheckAddress);

   //! If the address was generated using a user supplied string token, then that value
   //! is saved within the address for later retrieval, as the user may identify an
   //! address based on this value.
   std::string GetGenerationToken() const { return mAddressToken; }

   //! Returns the broadcast address of this address.
   std::string GetBroadcastAddress() const;

private:
   //! Helper method to convert a integer representing a binary IP address into
   //! its constituent fields.
   std::vector<size_t> TranslateIntegerIP(size_t aIP_Value) const;

   static bool IsNumber(const std::string& aString);

   //! Helper method for converting byte array to other member variables. Assumes
   //! "mBytes" is already set to correct value;
   void PopulateAddressFromBytes();

   std::string     mAddress{IPv4::cNULL_ADDRESS};
   std::string     mRoutingPrefix{""};
   std::string     mSubnet{""};
   uint32_t        mSubnetIntegerValue{0};
   size_t          mSubnetMaskBitLength{IPv4::cDEFAULT_CIDR};
   uint32_t        mBinaryIntegerValue{0};
   IPv4::ByteArray mBytes{{0, 0, 0, 0}};
   std::string     mAddressToken{""};
};

} // namespace comm
} // namespace wsf

UT_MAP_CLASS_TO_SCRIPT_NAME(wsf::comm::Address, "WsfAddress")

//! Template specialization for wsf::comm::Address. Allows usage of Address in std::unordered_map/set.
namespace std
{
template<>
struct hash<wsf::comm::Address>
{
   size_t operator()(const wsf::comm::Address& aRhs) const
   {
      return hash<size_t>()(wsf::comm::Address::GetStringValue(aRhs.GetAddress()));
   }
};

} // namespace std

#endif
