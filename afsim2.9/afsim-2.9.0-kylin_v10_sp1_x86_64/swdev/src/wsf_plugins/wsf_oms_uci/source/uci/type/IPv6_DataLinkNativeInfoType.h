// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__IPv6_DataLinkNativeInfoType_h
#define Uci__Type__IPv6_DataLinkNativeInfoType_h 1

#if !defined(Uci__Type__DataLinkNativeInfoPET_h)
# include "uci/type/DataLinkNativeInfoPET.h"
#endif

#if !defined(Uci__Type__IPv6_AddressType_h)
# include "uci/type/IPv6_AddressType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base DataLinkNativeInfoPET for IPV6. */
      class IPv6_DataLinkNativeInfoType : public virtual uci::type::DataLinkNativeInfoPET {
      public:

         /** The destructor */
         virtual ~IPv6_DataLinkNativeInfoType()
         { }

         /** Returns this accessor's type constant, i.e. IPv6_DataLinkNativeInfoType
           *
           * @return This accessor's type constant, i.e. IPv6_DataLinkNativeInfoType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::iPv6_DataLinkNativeInfoType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IPv6_DataLinkNativeInfoType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IPv6_SourceAddress.
           *
           * @return The value of the string data type identified by IPv6_SourceAddress.
           */
         virtual const uci::type::IPv6_AddressType& getIPv6_SourceAddress() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IPv6_SourceAddress.
           *
           * @return The value of the string data type identified by IPv6_SourceAddress.
           */
         virtual uci::type::IPv6_AddressType& getIPv6_SourceAddress()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IPv6_SourceAddress to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setIPv6_SourceAddress(const uci::type::IPv6_AddressType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IPv6_SourceAddress to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIPv6_SourceAddress(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IPv6_SourceAddress to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIPv6_SourceAddress(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_SourceAddress exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_SourceAddress is emabled or not
           */
         virtual bool hasIPv6_SourceAddress() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_SourceAddress
           *
           * @param type = uci::type::accessorType::iPv6_AddressType This Accessor's accessor type
           */
         virtual void enableIPv6_SourceAddress(uci::base::accessorType::AccessorType type = uci::type::accessorType::iPv6_AddressType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_SourceAddress */
         virtual void clearIPv6_SourceAddress()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_SourceCIDR_Mask.
           *
           * @return The value of the simple primitive data type identified by IPv6_SourceCIDR_Mask.
           */
         virtual xs::Int getIPv6_SourceCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_SourceCIDR_Mask.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_SourceCIDR_Mask(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_SourceCIDR_Mask exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_SourceCIDR_Mask is emabled or not
           */
         virtual bool hasIPv6_SourceCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_SourceCIDR_Mask
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_SourceCIDR_Mask(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_SourceCIDR_Mask */
         virtual void clearIPv6_SourceCIDR_Mask()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IPv6_DestinationAddress.
           *
           * @return The value of the string data type identified by IPv6_DestinationAddress.
           */
         virtual const uci::type::IPv6_AddressType& getIPv6_DestinationAddress() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the IPv6_DestinationAddress.
           *
           * @return The value of the string data type identified by IPv6_DestinationAddress.
           */
         virtual uci::type::IPv6_AddressType& getIPv6_DestinationAddress()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IPv6_DestinationAddress to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setIPv6_DestinationAddress(const uci::type::IPv6_AddressType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IPv6_DestinationAddress to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIPv6_DestinationAddress(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the IPv6_DestinationAddress to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setIPv6_DestinationAddress(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_DestinationAddress exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_DestinationAddress is emabled or not
           */
         virtual bool hasIPv6_DestinationAddress() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_DestinationAddress
           *
           * @param type = uci::type::accessorType::iPv6_AddressType This Accessor's accessor type
           */
         virtual void enableIPv6_DestinationAddress(uci::base::accessorType::AccessorType type = uci::type::accessorType::iPv6_AddressType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_DestinationAddress */
         virtual void clearIPv6_DestinationAddress()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_DestinationCIDR_Mask.
           *
           * @return The value of the simple primitive data type identified by IPv6_DestinationCIDR_Mask.
           */
         virtual xs::Int getIPv6_DestinationCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_DestinationCIDR_Mask.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_DestinationCIDR_Mask(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_DestinationCIDR_Mask exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_DestinationCIDR_Mask is emabled or not
           */
         virtual bool hasIPv6_DestinationCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_DestinationCIDR_Mask
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_DestinationCIDR_Mask(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_DestinationCIDR_Mask */
         virtual void clearIPv6_DestinationCIDR_Mask()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_ProtocolHeader.
           *
           * @return The value of the simple primitive data type identified by IPv6_ProtocolHeader.
           */
         virtual xs::Int getIPv6_ProtocolHeader() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_ProtocolHeader.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_ProtocolHeader(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_ProtocolHeader exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_ProtocolHeader is emabled or not
           */
         virtual bool hasIPv6_ProtocolHeader() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_ProtocolHeader
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_ProtocolHeader(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_ProtocolHeader */
         virtual void clearIPv6_ProtocolHeader()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_SourcePortRangeStart.
           *
           * @return The value of the simple primitive data type identified by IPv6_SourcePortRangeStart.
           */
         virtual xs::Int getIPv6_SourcePortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_SourcePortRangeStart.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_SourcePortRangeStart(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_SourcePortRangeStart exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_SourcePortRangeStart is emabled or not
           */
         virtual bool hasIPv6_SourcePortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_SourcePortRangeStart
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_SourcePortRangeStart(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_SourcePortRangeStart */
         virtual void clearIPv6_SourcePortRangeStart()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_SourcePortRangeEnd.
           *
           * @return The value of the simple primitive data type identified by IPv6_SourcePortRangeEnd.
           */
         virtual xs::Int getIPv6_SourcePortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_SourcePortRangeEnd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_SourcePortRangeEnd(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_SourcePortRangeEnd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_SourcePortRangeEnd is emabled or not
           */
         virtual bool hasIPv6_SourcePortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_SourcePortRangeEnd
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_SourcePortRangeEnd(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_SourcePortRangeEnd */
         virtual void clearIPv6_SourcePortRangeEnd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_DestinationPortRangeStart.
           *
           * @return The value of the simple primitive data type identified by IPv6_DestinationPortRangeStart.
           */
         virtual xs::Int getIPv6_DestinationPortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_DestinationPortRangeStart.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_DestinationPortRangeStart(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_DestinationPortRangeStart exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_DestinationPortRangeStart is emabled or not
           */
         virtual bool hasIPv6_DestinationPortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_DestinationPortRangeStart
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_DestinationPortRangeStart(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_DestinationPortRangeStart */
         virtual void clearIPv6_DestinationPortRangeStart()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_DestinationPortRangeEnd.
           *
           * @return The value of the simple primitive data type identified by IPv6_DestinationPortRangeEnd.
           */
         virtual xs::Int getIPv6_DestinationPortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_DestinationPortRangeEnd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_DestinationPortRangeEnd(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_DestinationPortRangeEnd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_DestinationPortRangeEnd is emabled or not
           */
         virtual bool hasIPv6_DestinationPortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_DestinationPortRangeEnd
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_DestinationPortRangeEnd(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_DestinationPortRangeEnd */
         virtual void clearIPv6_DestinationPortRangeEnd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_DS_Field.
           *
           * @return The value of the simple primitive data type identified by IPv6_DS_Field.
           */
         virtual xs::Int getIPv6_DS_Field() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_DS_Field.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_DS_Field(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_DS_Field exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_DS_Field is emabled or not
           */
         virtual bool hasIPv6_DS_Field() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_DS_Field
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_DS_Field(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_DS_Field */
         virtual void clearIPv6_DS_Field()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv6_MTU_Size.
           *
           * @return The value of the simple primitive data type identified by IPv6_MTU_Size.
           */
         virtual xs::Int getIPv6_MTU_Size() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv6_MTU_Size.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv6_MTU_Size(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv6_MTU_Size exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv6_MTU_Size is emabled or not
           */
         virtual bool hasIPv6_MTU_Size() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv6_MTU_Size
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv6_MTU_Size(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv6_MTU_Size */
         virtual void clearIPv6_MTU_Size()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IPv6_DataLinkNativeInfoType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IPv6_DataLinkNativeInfoType to copy from
           */
         IPv6_DataLinkNativeInfoType(const IPv6_DataLinkNativeInfoType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IPv6_DataLinkNativeInfoType to the contents of the
           * IPv6_DataLinkNativeInfoType on the right hand side (rhs) of the assignment operator.IPv6_DataLinkNativeInfoType [only
           * available to derived classes]
           *
           * @param rhs The IPv6_DataLinkNativeInfoType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::IPv6_DataLinkNativeInfoType
           * @return A constant reference to this IPv6_DataLinkNativeInfoType.
           */
         const IPv6_DataLinkNativeInfoType& operator=(const IPv6_DataLinkNativeInfoType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IPv6_DataLinkNativeInfoType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IPv6_DataLinkNativeInfoType_h

