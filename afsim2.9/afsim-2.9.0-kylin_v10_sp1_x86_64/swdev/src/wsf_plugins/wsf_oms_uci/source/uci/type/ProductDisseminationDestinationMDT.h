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
#ifndef Uci__Type__ProductDisseminationDestinationMDT_h
#define Uci__Type__ProductDisseminationDestinationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DestinationID_Type_h)
# include "uci/type/DestinationID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__ProductDisseminationGatewayType_h)
# include "uci/type/ProductDisseminationGatewayType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__DestinationCategoryEnum_h)
# include "uci/type/DestinationCategoryEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

#if !defined(Uci__Type__MIME_Type_h)
# include "uci/type/MIME_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type defines all the fields required to represent a destination for purposes of dissemination. */
      class ProductDisseminationDestinationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductDisseminationDestinationMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationDestinationMDT
           *
           * @return This accessor's type constant, i.e. ProductDisseminationDestinationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationDestinationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationDestinationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This field allows for logical names to be used in place of an ID to reference a configured Destination. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::VisibleString256Type, uci::type::accessorType::visibleString256Type> Handle;

         /** This field is used to list the product types that this destination supports for dissemination. [Minimum occurrences:
           * 0] [Maximum occurrences: 78]
           */
         typedef uci::base::BoundedList<uci::type::ProductTypeEnum, uci::type::accessorType::productTypeEnum> SupportedProductType;

         /** This field is used to list the product formats that this destination supports for dissemination. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MIME_Type, uci::type::accessorType::mIME_Type> SupportedFormat;

         /** Returns the accessor that provides access to the complex content that is identified by the DestinationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DestinationID.
           */
         virtual const uci::type::DestinationID_Type& getDestinationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DestinationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DestinationID.
           */
         virtual uci::type::DestinationID_Type& getDestinationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DestinationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DestinationID
           */
         virtual void setDestinationID(const uci::type::DestinationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Handle.
           *
           * @return The bounded list identified by Handle.
           */
         virtual const uci::type::ProductDisseminationDestinationMDT::Handle& getHandle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Handle.
           *
           * @return The bounded list identified by Handle.
           */
         virtual uci::type::ProductDisseminationDestinationMDT::Handle& getHandle()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Handle.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setHandle(const uci::type::ProductDisseminationDestinationMDT::Handle& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endpoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endpoint.
           */
         virtual const uci::type::ProductDisseminationGatewayType& getEndpoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endpoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endpoint.
           */
         virtual uci::type::ProductDisseminationGatewayType& getEndpoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Endpoint to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Endpoint
           */
         virtual void setEndpoint(const uci::type::ProductDisseminationGatewayType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityInformation.
           */
         virtual const uci::type::SecurityInformationType& getSecurityInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityInformation.
           */
         virtual uci::type::SecurityInformationType& getSecurityInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityInformation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityInformation
           */
         virtual void setSecurityInformation(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DestinationType.
           *
           * @return The value of the enumeration identified by DestinationType.
           */
         virtual const uci::type::DestinationCategoryEnum& getDestinationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DestinationType.
           *
           * @return The value of the enumeration identified by DestinationType.
           */
         virtual uci::type::DestinationCategoryEnum& getDestinationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DestinationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDestinationType(const uci::type::DestinationCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DestinationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDestinationType(uci::type::DestinationCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Enabled.
           *
           * @return The value of the simple primitive data type identified by Enabled.
           */
         virtual xs::Boolean getEnabled() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Enabled.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEnabled(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedProductType.
           *
           * @return The bounded list identified by SupportedProductType.
           */
         virtual const uci::type::ProductDisseminationDestinationMDT::SupportedProductType& getSupportedProductType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedProductType.
           *
           * @return The bounded list identified by SupportedProductType.
           */
         virtual uci::type::ProductDisseminationDestinationMDT::SupportedProductType& getSupportedProductType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedProductType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedProductType(const uci::type::ProductDisseminationDestinationMDT::SupportedProductType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedFormat.
           *
           * @return The bounded list identified by SupportedFormat.
           */
         virtual const uci::type::ProductDisseminationDestinationMDT::SupportedFormat& getSupportedFormat() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedFormat.
           *
           * @return The bounded list identified by SupportedFormat.
           */
         virtual uci::type::ProductDisseminationDestinationMDT::SupportedFormat& getSupportedFormat()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedFormat.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedFormat(const uci::type::ProductDisseminationDestinationMDT::SupportedFormat& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationDestinationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationDestinationMDT to copy from
           */
         ProductDisseminationDestinationMDT(const ProductDisseminationDestinationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationDestinationMDT to the contents of the
           * ProductDisseminationDestinationMDT on the right hand side (rhs) of the assignment
           * operator.ProductDisseminationDestinationMDT [only available to derived classes]
           *
           * @param rhs The ProductDisseminationDestinationMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductDisseminationDestinationMDT
           * @return A constant reference to this ProductDisseminationDestinationMDT.
           */
         const ProductDisseminationDestinationMDT& operator=(const ProductDisseminationDestinationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationDestinationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationDestinationMDT_h

