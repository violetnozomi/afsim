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
#ifndef Uci__Type__GatewayCapabilityType_h
#define Uci__Type__GatewayCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CommDataFormatEnum_h)
# include "uci/type/CommDataFormatEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__GatewayCapabilityOptionsType_h)
# include "uci/type/GatewayCapabilityOptionsType.h"
#endif

#if !defined(Uci__Type__CommProfileID_Type_h)
# include "uci/type/CommProfileID_Type.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to describe a Gateway Capability which is synonymous with a Message Processor and-or a product disseminator. */
      class GatewayCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayCapabilityType
           *
           * @return This accessor's type constant, i.e. GatewayCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an accepted control interface for the Capability. See enumeration annotations for further details. [Maximum
           * occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Indicates a Profile (preset) that is applicable to this Capability. Only the profiles currently available and
           * applicable to this Capability should be listed. A Profile (or preset) is a logical collection of data, whose
           * implementation is not specified, that is used to tailor, configure or otherwise affect the behavior of a Capability.
           * Profiles can vary from mission to mission or even by phases of a single mission. They are generally developed in
           * pre-mission planning and are unique for each Subsystem model. Their content is not standardized by UCI, but the
           * mechanism for advertising their availability and activating them is. Their content is abstracted by the UCI concept
           * of Capability. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommProfileID_Type, uci::type::accessorType::commProfileID_Type> AvailableProfileID;

         /** If specified in the capability that the type is Product then, SupportedProduct should be filled. The product type
           * that the ProductProcessingPlan applies to [Minimum occurrences: 0] [Maximum occurrences: 78]
           */
         typedef uci::base::BoundedList<uci::type::ProductTypeEnum, uci::type::accessorType::productTypeEnum> SupportedProduct;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::CommDataFormatEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::CommDataFormatEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::CommDataFormatEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::CommDataFormatEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapabilityType.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapabilityType.
           */
         virtual const uci::type::ForeignKeyType& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapabilityType.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapabilityType.
           */
         virtual uci::type::ForeignKeyType& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubCapabilityType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubCapabilityType
           */
         virtual void setSubCapabilityType(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubCapabilityType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubCapabilityType is emabled or not
           */
         virtual bool hasSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubCapabilityType
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableSubCapabilityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubCapabilityType */
         virtual void clearSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::GatewayCapabilityType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::GatewayCapabilityType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::GatewayCapabilityType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual const uci::type::GatewayCapabilityOptionsType& getCapabilityOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual uci::type::GatewayCapabilityOptionsType& getCapabilityOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityOptions
           */
         virtual void setCapabilityOptions(const uci::type::GatewayCapabilityOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableProfileID.
           *
           * @return The bounded list identified by AvailableProfileID.
           */
         virtual const uci::type::GatewayCapabilityType::AvailableProfileID& getAvailableProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableProfileID.
           *
           * @return The bounded list identified by AvailableProfileID.
           */
         virtual uci::type::GatewayCapabilityType::AvailableProfileID& getAvailableProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AvailableProfileID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAvailableProfileID(const uci::type::GatewayCapabilityType::AvailableProfileID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedProduct.
           *
           * @return The bounded list identified by SupportedProduct.
           */
         virtual const uci::type::GatewayCapabilityType::SupportedProduct& getSupportedProduct() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedProduct.
           *
           * @return The bounded list identified by SupportedProduct.
           */
         virtual uci::type::GatewayCapabilityType::SupportedProduct& getSupportedProduct()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedProduct.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedProduct(const uci::type::GatewayCapabilityType::SupportedProduct& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayCapabilityType to copy from
           */
         GatewayCapabilityType(const GatewayCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayCapabilityType to the contents of the GatewayCapabilityType
           * on the right hand side (rhs) of the assignment operator.GatewayCapabilityType [only available to derived classes]
           *
           * @param rhs The GatewayCapabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::GatewayCapabilityType
           * @return A constant reference to this GatewayCapabilityType.
           */
         const GatewayCapabilityType& operator=(const GatewayCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayCapabilityType_h

