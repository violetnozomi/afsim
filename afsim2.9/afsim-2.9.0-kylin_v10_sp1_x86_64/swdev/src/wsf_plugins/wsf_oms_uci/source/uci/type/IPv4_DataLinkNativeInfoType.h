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
#ifndef Uci__Type__IPv4_DataLinkNativeInfoType_h
#define Uci__Type__IPv4_DataLinkNativeInfoType_h 1

#if !defined(Uci__Type__DataLinkNativeInfoPET_h)
# include "uci/type/DataLinkNativeInfoPET.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base DataLinkNativeInfoPET for IPV4. */
      class IPv4_DataLinkNativeInfoType : public virtual uci::type::DataLinkNativeInfoPET {
      public:

         /** The destructor */
         virtual ~IPv4_DataLinkNativeInfoType()
         { }

         /** Returns this accessor's type constant, i.e. IPv4_DataLinkNativeInfoType
           *
           * @return This accessor's type constant, i.e. IPv4_DataLinkNativeInfoType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::iPv4_DataLinkNativeInfoType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IPv4_DataLinkNativeInfoType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_SourceAddress.
           *
           * @return The value of the simple primitive data type identified by IPv4_SourceAddress.
           */
         virtual xs::Int getIPv4_SourceAddress() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_SourceAddress.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_SourceAddress(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_SourceAddress exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_SourceAddress is emabled or not
           */
         virtual bool hasIPv4_SourceAddress() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_SourceAddress
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_SourceAddress(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_SourceAddress */
         virtual void clearIPv4_SourceAddress()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_SourceCIDR_Mask.
           *
           * @return The value of the simple primitive data type identified by IPv4_SourceCIDR_Mask.
           */
         virtual xs::Int getIPv4_SourceCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_SourceCIDR_Mask.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_SourceCIDR_Mask(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_SourceCIDR_Mask exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_SourceCIDR_Mask is emabled or not
           */
         virtual bool hasIPv4_SourceCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_SourceCIDR_Mask
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_SourceCIDR_Mask(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_SourceCIDR_Mask */
         virtual void clearIPv4_SourceCIDR_Mask()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_DestinationAddress.
           *
           * @return The value of the simple primitive data type identified by IPv4_DestinationAddress.
           */
         virtual xs::Int getIPv4_DestinationAddress() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_DestinationAddress.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_DestinationAddress(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_DestinationAddress exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_DestinationAddress is emabled or not
           */
         virtual bool hasIPv4_DestinationAddress() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_DestinationAddress
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_DestinationAddress(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_DestinationAddress */
         virtual void clearIPv4_DestinationAddress()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_DestinationCIDR_Mask.
           *
           * @return The value of the simple primitive data type identified by IPv4_DestinationCIDR_Mask.
           */
         virtual xs::Int getIPv4_DestinationCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_DestinationCIDR_Mask.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_DestinationCIDR_Mask(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_DestinationCIDR_Mask exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_DestinationCIDR_Mask is emabled or not
           */
         virtual bool hasIPv4_DestinationCIDR_Mask() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_DestinationCIDR_Mask
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_DestinationCIDR_Mask(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_DestinationCIDR_Mask */
         virtual void clearIPv4_DestinationCIDR_Mask()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_ProtocolHeader.
           *
           * @return The value of the simple primitive data type identified by IPv4_ProtocolHeader.
           */
         virtual xs::Int getIPv4_ProtocolHeader() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_ProtocolHeader.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_ProtocolHeader(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_ProtocolHeader exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_ProtocolHeader is emabled or not
           */
         virtual bool hasIPv4_ProtocolHeader() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_ProtocolHeader
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_ProtocolHeader(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_ProtocolHeader */
         virtual void clearIPv4_ProtocolHeader()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_SourcePortRangeStart.
           *
           * @return The value of the simple primitive data type identified by IPv4_SourcePortRangeStart.
           */
         virtual xs::Int getIPv4_SourcePortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_SourcePortRangeStart.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_SourcePortRangeStart(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_SourcePortRangeStart exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_SourcePortRangeStart is emabled or not
           */
         virtual bool hasIPv4_SourcePortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_SourcePortRangeStart
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_SourcePortRangeStart(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_SourcePortRangeStart */
         virtual void clearIPv4_SourcePortRangeStart()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_SourcePortRangeEnd.
           *
           * @return The value of the simple primitive data type identified by IPv4_SourcePortRangeEnd.
           */
         virtual xs::Int getIPv4_SourcePortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_SourcePortRangeEnd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_SourcePortRangeEnd(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_SourcePortRangeEnd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_SourcePortRangeEnd is emabled or not
           */
         virtual bool hasIPv4_SourcePortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_SourcePortRangeEnd
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_SourcePortRangeEnd(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_SourcePortRangeEnd */
         virtual void clearIPv4_SourcePortRangeEnd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_DestinationPortRangeStart.
           *
           * @return The value of the simple primitive data type identified by IPv4_DestinationPortRangeStart.
           */
         virtual xs::Int getIPv4_DestinationPortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_DestinationPortRangeStart.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_DestinationPortRangeStart(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_DestinationPortRangeStart exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_DestinationPortRangeStart is emabled or not
           */
         virtual bool hasIPv4_DestinationPortRangeStart() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_DestinationPortRangeStart
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_DestinationPortRangeStart(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_DestinationPortRangeStart */
         virtual void clearIPv4_DestinationPortRangeStart()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_DestinationPortRangeEnd.
           *
           * @return The value of the simple primitive data type identified by IPv4_DestinationPortRangeEnd.
           */
         virtual xs::Int getIPv4_DestinationPortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_DestinationPortRangeEnd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_DestinationPortRangeEnd(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_DestinationPortRangeEnd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_DestinationPortRangeEnd is emabled or not
           */
         virtual bool hasIPv4_DestinationPortRangeEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_DestinationPortRangeEnd
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_DestinationPortRangeEnd(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_DestinationPortRangeEnd */
         virtual void clearIPv4_DestinationPortRangeEnd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_DS_Field.
           *
           * @return The value of the simple primitive data type identified by IPv4_DS_Field.
           */
         virtual xs::Int getIPv4_DS_Field() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_DS_Field.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_DS_Field(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_DS_Field exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_DS_Field is emabled or not
           */
         virtual bool hasIPv4_DS_Field() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_DS_Field
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_DS_Field(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_DS_Field */
         virtual void clearIPv4_DS_Field()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IPv4_MTU_Size.
           *
           * @return The value of the simple primitive data type identified by IPv4_MTU_Size.
           */
         virtual xs::Int getIPv4_MTU_Size() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IPv4_MTU_Size.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIPv4_MTU_Size(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IPv4_MTU_Size exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IPv4_MTU_Size is emabled or not
           */
         virtual bool hasIPv4_MTU_Size() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IPv4_MTU_Size
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableIPv4_MTU_Size(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IPv4_MTU_Size */
         virtual void clearIPv4_MTU_Size()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IPv4_DataLinkNativeInfoType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IPv4_DataLinkNativeInfoType to copy from
           */
         IPv4_DataLinkNativeInfoType(const IPv4_DataLinkNativeInfoType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IPv4_DataLinkNativeInfoType to the contents of the
           * IPv4_DataLinkNativeInfoType on the right hand side (rhs) of the assignment operator.IPv4_DataLinkNativeInfoType [only
           * available to derived classes]
           *
           * @param rhs The IPv4_DataLinkNativeInfoType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::IPv4_DataLinkNativeInfoType
           * @return A constant reference to this IPv4_DataLinkNativeInfoType.
           */
         const IPv4_DataLinkNativeInfoType& operator=(const IPv4_DataLinkNativeInfoType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IPv4_DataLinkNativeInfoType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IPv4_DataLinkNativeInfoType_h

