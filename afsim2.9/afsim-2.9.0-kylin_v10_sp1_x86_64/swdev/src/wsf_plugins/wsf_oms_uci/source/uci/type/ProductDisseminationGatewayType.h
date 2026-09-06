// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ProductDisseminationGatewayType_h
#define Uci__Type__ProductDisseminationGatewayType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AttributedURI_Type_h)
# include "uci/type/AttributedURI_Type.h"
#endif

#if !defined(Uci__Type__CommConfigurationID_Type_h)
# include "uci/type/CommConfigurationID_Type.h"
#endif

#if !defined(Uci__Type__DataLinkNativeInfoPET_h)
# include "uci/type/DataLinkNativeInfoPET.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes the gateway method for this product dissemination destination. Describes how a product Dissemination
        * Destination performs data transmission via a gateway to a remote network node.
        */
      class ProductDisseminationGatewayType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductDisseminationGatewayType()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationGatewayType
           *
           * @return This accessor's type constant, i.e. ProductDisseminationGatewayType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationGatewayType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationGatewayType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Address.
           *
           * @return The value of the string data type identified by Address.
           */
         virtual const uci::type::AttributedURI_Type& getAddress() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Address.
           *
           * @return The value of the string data type identified by Address.
           */
         virtual uci::type::AttributedURI_Type& getAddress()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Address to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAddress(const uci::type::AttributedURI_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Address to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAddress(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Address to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAddress(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Address exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Address is emabled or not
           */
         virtual bool hasAddress() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Address
           *
           * @param type = uci::type::accessorType::attributedURI_Type This Accessor's accessor type
           */
         virtual void enableAddress(uci::base::accessorType::AccessorType type = uci::type::accessorType::attributedURI_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Address */
         virtual void clearAddress()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConfigurationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConfigurationID.
           */
         virtual const uci::type::CommConfigurationID_Type& getConfigurationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConfigurationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConfigurationID.
           */
         virtual uci::type::CommConfigurationID_Type& getConfigurationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConfigurationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConfigurationID
           */
         virtual void setConfigurationID(const uci::type::CommConfigurationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConfigurationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConfigurationID is emabled or not
           */
         virtual bool hasConfigurationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConfigurationID
           *
           * @param type = uci::type::accessorType::commConfigurationID_Type This Accessor's accessor type
           */
         virtual void enableConfigurationID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commConfigurationID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConfigurationID */
         virtual void clearConfigurationID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataLinkNativeInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataLinkNativeInfo.
           */
         virtual const uci::type::DataLinkNativeInfoPET& getDataLinkNativeInfo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataLinkNativeInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataLinkNativeInfo.
           */
         virtual uci::type::DataLinkNativeInfoPET& getDataLinkNativeInfo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataLinkNativeInfo to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataLinkNativeInfo
           */
         virtual void setDataLinkNativeInfo(const uci::type::DataLinkNativeInfoPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataLinkNativeInfo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataLinkNativeInfo is emabled or not
           */
         virtual bool hasDataLinkNativeInfo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataLinkNativeInfo
           *
           * @param type = uci::type::accessorType::dataLinkNativeInfoPET This Accessor's accessor type
           */
         virtual void enableDataLinkNativeInfo(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataLinkNativeInfoPET)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataLinkNativeInfo */
         virtual void clearDataLinkNativeInfo()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationGatewayType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationGatewayType to copy from
           */
         ProductDisseminationGatewayType(const ProductDisseminationGatewayType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationGatewayType to the contents of the
           * ProductDisseminationGatewayType on the right hand side (rhs) of the assignment
           * operator.ProductDisseminationGatewayType [only available to derived classes]
           *
           * @param rhs The ProductDisseminationGatewayType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductDisseminationGatewayType
           * @return A constant reference to this ProductDisseminationGatewayType.
           */
         const ProductDisseminationGatewayType& operator=(const ProductDisseminationGatewayType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationGatewayType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationGatewayType_h

