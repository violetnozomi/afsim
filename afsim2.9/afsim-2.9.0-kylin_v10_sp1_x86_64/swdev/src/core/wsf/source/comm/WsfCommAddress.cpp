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

#include "WsfCommAddress.hpp"

#include <cctype>
#include <cmath>
#include <sstream>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtLexicalCast.hpp"
#include "UtLog.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
Address::Address(const std::string& aAddress, size_t aSubnetMaskBitLength)
{
   SetAddress(aAddress, aSubnetMaskBitLength);
}

// =================================================================================================
Address::Address(size_t aSubnetMaskBitLength)
   : mSubnetMaskBitLength(aSubnetMaskBitLength)
{
}

// =================================================================================================
// This method will set the address, routing prefix, subnet, and subnet bit length of the address.
// The values for those variables not provided in the parameter list are derived from the provided
// values.
void Address::SetAddress(const std::string& aAddress, size_t aSubnetMaskBitLength /*= IPv4::cDEFAULT_CIDR */)
{
   //! Ensure the bit mask length is valid
   if (aSubnetMaskBitLength < 1 || aSubnetMaskBitLength > IPv4::cBIT_SIZE)
   {
      throw UtException("Invalid CIDR subnet bit mask size in WsfComm::SetAddress");
   }

   mAddress             = aAddress;
   mSubnetMaskBitLength = aSubnetMaskBitLength;
   std::vector<size_t> fieldVectorNum;
   auto                tokenizeString = aAddress;

   //! Parse out the strings associated with each subfield in the address
   //! e.g. 123.2.1.33 -> 123, 2, 1, 33
   for (size_t i = 0; i < IPv4::cNUM_FIELDS; ++i)
   {
      if (i != IPv4::cNUM_FIELDS - 1)
      {
         auto position = tokenizeString.find(IPv4::cDELIMITER);
         if (position == std::string::npos)
         {
            throw UtException("Malformed address provided to WsfCommAddress::SetAddress.");
         }
         auto token = tokenizeString.substr(0, position);
         fieldVectorNum.push_back(ut::lexical_cast<size_t>(token));
         tokenizeString.erase(0, position + 1);
      }
      else
      {
         if (tokenizeString.find(IPv4::cDELIMITER) != std::string::npos)
         {
            throw UtException("Malformed address provided to WsfCommAddress::SetAddress.");
         }
         fieldVectorNum.push_back(ut::lexical_cast<size_t>(tokenizeString));
      }
   }

   //! Convert each field value to a integer representing the binary value
   //! for each field
   uint32_t addressValue = 0;
   for (size_t i = 0; i < fieldVectorNum.size(); ++i)
   {
      addressValue += static_cast<uint32_t>(fieldVectorNum[i] << (IPv4::cBIT_SIZE - (8 * (i + 1))));
      //! Store each field as a byte representation
      mBytes[i] = static_cast<unsigned char>(fieldVectorNum[i]);
   }

   //! Store the integer value of the address for possible later comparisons
   mBinaryIntegerValue = addressValue;

   //! Get the CIDR mask
   auto inverseMaskLength = IPv4::cBIT_SIZE - aSubnetMaskBitLength;
   auto maskValue         = addressValue & (IPv4::cMASK_MAX << inverseMaskLength);

   //! Get the vector of integer values for the routing/network address
   fieldVectorNum = TranslateIntegerIP(maskValue);
   //! Get the vector of integer value of the subnet mask
   auto subnetVector = TranslateIntegerIP(IPv4::cMASK_MAX << inverseMaskLength);

   //! Translate the subnet into an integer value for easy comparisons between addresses
   mSubnetIntegerValue = 0;
   for (size_t i = 0; i < subnetVector.size(); ++i)
   {
      mSubnetIntegerValue += static_cast<uint32_t>(subnetVector[i] << (IPv4::cBIT_SIZE - (8 * (i + 1))));
   }

   //! Convert the two values above to strings with delimiters,  and save them.
   std::string networkAddress = "";
   std::string subnet         = "";
   for (size_t i = 0; i < fieldVectorNum.size(); ++i)
   {
      if (i != 0)
      {
         networkAddress += ".";
      }
      networkAddress += std::to_string(fieldVectorNum[i]);
   }
   mRoutingPrefix = networkAddress;

   for (size_t i = 0; i < subnetVector.size(); ++i)
   {
      if (i != 0)
      {
         subnet += ".";
      }
      subnet += std::to_string(subnetVector[i]);
   }
   mSubnet = subnet;
}

// =================================================================================================
std::vector<size_t> Address::TranslateIntegerIP(size_t aIP_Value) const
{
   std::vector<size_t> fieldVector;
   fieldVector.resize(IPv4::cNUM_FIELDS);

   for (size_t i = 0; i < IPv4::cNUM_FIELDS; ++i)
   {
      fieldVector[i] = (aIP_Value >> (IPv4::cBIT_SIZE - (8 * (i + 1)))) & IPv4::cMASK_FIELD;
   }

   return fieldVector;
}

// =================================================================================================
bool Address::operator==(const Address& aRhs) const
{
   return mBytes == aRhs.mBytes;
}

// =================================================================================================
bool Address::operator!=(const Address& aRhs) const
{
   return !(*this == aRhs);
}

// =================================================================================================
bool Address::operator<(const Address& aRhs) const
{
   // Immediately return false is the addresses are equivalent. This avoids
   // an additional conditional check for the least significant byte every iteration.
   if ((*this) == aRhs)
   {
      return false;
   }

   //! Iterate from most significant to least significant bytes. Values may be equal for
   //! some higher order bytes, but we're guaranteed to differ
   //! somewhere due to the previous equivalence check.
   for (size_t i = 0; i < mBytes.size(); ++i)
   {
      if (mBytes[i] < aRhs.mBytes[i])
      {
         return true;
      }
      if (mBytes[i] > aRhs.mBytes[i])
      {
         return false;
      }
   }
   //! We should never reach this point.
   return true;
}

// =================================================================================================
size_t Address::GetNumHostAddresses() const
{
   //! Note: Typically, we would subtract two available hosts from an IPV4
   //! host range, as the highest and lowest available host addresses are used
   //! for broadcasting. We don't make the distinction in this implementation.
   //! Therefore, if checking with external references, this value is likely
   //! two hosts higher.
   auto numHosts = std::pow(2u, (IPv4::cBIT_SIZE - mSubnetMaskBitLength));
   return static_cast<size_t>(numHosts);
}

// =================================================================================================
std::ostream& operator<<(std::ostream& aOut, const Address& aAddress)
{
   aOut << aAddress.mAddress;
   return aOut;
}

// =================================================================================================
Address& Address::operator++()
{
   //! Increment the lowest order byte, carrying the value if we
   //! are at the max value for this byte (255).
   for (size_t i = IPv4::cNUM_FIELDS; i-- > 0;)
   {
      if (mBytes[i] == IPv4::cMASK_FIELD)
      {
         mBytes[i] = 0;
      }
      else
      {
         ++mBytes[i];
         break;
      }
   }

   //! Reset the other member variables to account for the address change.
   PopulateAddressFromBytes();
   return *this;
}

// =================================================================================================
Address Address::operator++(int)
{
   Address result(*this);
   ++(*this);
   return result;
}

// =================================================================================================
void Address::PopulateAddressFromBytes()
{
   std::string newAddress = "";
   for (size_t i = 0; i < IPv4::cNUM_FIELDS; ++i)
   {
      newAddress += std::to_string(mBytes[i]);
      if (i != (IPv4::cNUM_FIELDS - 1))
      {
         newAddress += ".";
      }
   }
   SetAddress(newAddress, mSubnetMaskBitLength);
}

// =================================================================================================
void Address::GenerateAddress(const std::string& aGenerationToken, size_t aSubnetBitLength)
{
   mAddressToken        = aGenerationToken;
   mSubnetMaskBitLength = aSubnetBitLength;

   //! Generate an integer value for the token string.
   uint32_t ongoingValue = 0;
   ongoingValue += static_cast<uint32_t>(GetStringValue(aGenerationToken));

   //! We always add the above integer value to the integer representation of the
   //! existing address. Typically, this will be 0.0.0.0 for previously unused address
   //! objects, but this call may have been made previously and resulted in an unsuitable
   //! address (due to a collision with an existing address, or range limitations of usage).
   //! Thus, we have a predictable and reproducible way of getting new addresses given the same
   //! token and subsequent calls to this method, yet get different results from each iteration
   //! of the call.
   ongoingValue += static_cast<uint32_t>(GetStringValue(mAddress));

   //! Convert the integer value into an IPV4 format address.
   auto fields = TranslateIntegerIP(ongoingValue);
   for (size_t i = 0; i < IPv4::cNUM_FIELDS; ++i)
   {
      mBytes[i] = static_cast<unsigned char>(fields[i]);
   }
   PopulateAddressFromBytes();
}

// =================================================================================================
size_t Address::GetStringValue(const std::string& aString)
{
   size_t value = 0;
   for (size_t i = 0; i < aString.size(); ++i)
   {
      value += value * 5 + static_cast<size_t>(aString[i]);
   }
   return value;
}

// =================================================================================================
bool Address::CheckInAddressRange(const Address& aNetworkAddress, const Address& aCheckAddress)
{
   if ((aNetworkAddress.mBinaryIntegerValue & aNetworkAddress.mSubnetIntegerValue) ==
       (aCheckAddress.mBinaryIntegerValue & aNetworkAddress.mSubnetIntegerValue))
   {
      return true;
   }
   return false;
}

// =================================================================================================
Address Address::ProcessInput(UtInput& aInput)
{
   std::string addressInput;

   aInput.ReadValue(addressInput);
   auto address = ProcessInput(addressInput);
   if (address.IsNull())
   {
      throw UtInput::BadValue(aInput, "Address provided is not in a valid format");
   }
   return address;
}

// =================================================================================================
Address Address::ProcessInput(std::string aInput)
{
   auto isValidAddress = IsValidAddress(aInput);
   auto cidrSubnet     = GetCIDR_ValueFromString(aInput);

   if (!isValidAddress)
   {
      return Address();
   }
   return Address(aInput, cidrSubnet);
}

// =================================================================================================
bool Address::IsValidAddress(const std::string& aPossibleAddress)
{
   auto    tokenizeString = aPossibleAddress;
   Address tempAddress;

   for (size_t i = 0; i < IPv4::cNUM_FIELDS; ++i)
   {
      if (i != IPv4::cNUM_FIELDS - 1)
      {
         auto position = tokenizeString.find(IPv4::cDELIMITER);
         if (position == std::string::npos)
         {
            return false;
         }

         auto token = tokenizeString.substr(0, position);
         tokenizeString.erase(0, position + 1);

         if (!IsNumber(token))
         {
            auto out = ut::log::warning() << "Address contains a field that is not a number.";
            out.AddNote() << "Field: " << token;
            return false;
         }

         auto fieldValue = std::stoi(token);
         if ((fieldValue < 0) || (fieldValue > 255))
         {
            auto out = ut::log::warning() << "Invalid address field value.";
            out.AddNote() << "Address: " << fieldValue;
            out.AddNote() << "Address must be between 0 and 255.";
            return false;
         }
      }
      else if (tokenizeString.find(IPv4::cDELIMITER) != std::string::npos)
      {
         return false;
      }
      else
      {
         auto fieldValue = std::stoi(tokenizeString);
         if ((fieldValue < 0) || (fieldValue > 255))
         {
            return false;
         }
      }
   }
   return true;
}

// =================================================================================================
size_t Address::GetCIDR_ValueFromString(std::string& aPossibleAddress)
{
   Address tempAddress;

   if (!IsValidAddress(aPossibleAddress))
   {
      return IPv4::cDEFAULT_CIDR;
   }

   auto position = aPossibleAddress.find(IPv4::cCIDR_DELIMITER);
   if (position == std::string::npos)
   {
      return IPv4::cDEFAULT_CIDR;
   }

   auto cidrString = aPossibleAddress.substr(position + 1, aPossibleAddress.length());
   aPossibleAddress.erase(position, aPossibleAddress.length());

   auto cidrValue = std::stoi(cidrString);
   if ((cidrValue >= 0) && (cidrValue <= static_cast<int>(IPv4::cBIT_SIZE)))
   {
      return static_cast<size_t>(cidrValue);
   }
   else
   {
      return IPv4::cDEFAULT_CIDR;
   }
}

// =================================================================================================
bool Address::IsNumber(const std::string& aString)
{
   bool isNumber = true;

   if (aString.length() == 0)
   {
      return false;
   }

   for (size_t i = 0; i < aString.length(); ++i)
   {
      isNumber &= (0 != std::isdigit(aString[i]));
   }

   return isNumber;
}

// =================================================================================================
std::string Address::GetBroadcastAddress() const
{
   auto              broadcastInteger = (mBinaryIntegerValue | (~mSubnetIntegerValue));
   auto              ipVec            = TranslateIntegerIP(static_cast<size_t>(broadcastInteger));
   std::stringstream result;

   for (size_t i = 0; i < IPv4::cNUM_FIELDS; ++i)
   {
      result << std::to_string(ipVec[i]);
      if (i < (IPv4::cNUM_FIELDS - 1))
      {
         result << IPv4::cDELIMITER;
      }
   }

   return result.str();
}

} // namespace comm
} // namespace wsf
