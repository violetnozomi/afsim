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
#ifndef Uci__Type__InputProductType_h
#define Uci__Type__InputProductType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductReferenceType_h)
# include "uci/type/ProductReferenceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductFilterType_h)
# include "uci/type/ProductFilterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the InputProductType choice accessor class */
      class InputProductType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~InputProductType()
         { }

         /** Returns this accessor's type constant, i.e. InputProductType
           *
           * @return This accessor's type constant, i.e. InputProductType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::inputProductType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const InputProductType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Specifies a product type and source. If there is more than one filter type defined, treat the filters as a logical
           * AND, where all criteria must be met to satisfy the filters. This element can be used in 2 ways: 1.) Streaming
           * products: Streaming products stream continuously and product management functions are applied to them continuously.
           * For example, continuous dissemination, classification, and processing of an EOIR video stream. Streams may or may not
           * have associated product metadata; therefore, type and source can be used to identify a specific stream as an input
           * product to a product management function. 2.) Discrete products: For discrete products, this element can be used to
           * make reference to all discrete products of a specified type. This use is typically used to reference products that
           * have already been created and is not intended to override a product management plan (download, dissemination,
           * classification, etc.) over some indefinite time period in the future. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductFilterType, uci::type::accessorType::productFilterType> ProductTypeReference;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum InputProductTypeChoice {
            INPUTPRODUCTTYPE_CHOICE_NONE,
            INPUTPRODUCTTYPE_CHOICE_PRODUCTREFERENCE,
            INPUTPRODUCTTYPE_CHOICE_PRODUCTTYPEREFERENCE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual InputProductTypeChoice getInputProductTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getInputProductTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setInputProductTypeChoiceOrdinal(InputProductTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductReference.
           */
         virtual const uci::type::ProductReferenceType& getProductReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductReference.
           */
         virtual uci::type::ProductReferenceType& getProductReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductReference to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductReference
           */
         virtual void setProductReference(const uci::type::ProductReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductTypeReference.
           *
           * @return The bounded list identified by ProductTypeReference.
           */
         virtual const uci::type::InputProductType::ProductTypeReference& getProductTypeReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductTypeReference.
           *
           * @return The bounded list identified by ProductTypeReference.
           */
         virtual uci::type::InputProductType::ProductTypeReference& getProductTypeReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductTypeReference.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductTypeReference(const uci::type::InputProductType::ProductTypeReference& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         InputProductType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The InputProductType to copy from
           */
         InputProductType(const InputProductType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this InputProductType to the contents of the InputProductType on the
           * right hand side (rhs) of the assignment operator.InputProductType [only available to derived classes]
           *
           * @param rhs The InputProductType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::InputProductType
           * @return A constant reference to this InputProductType.
           */
         const InputProductType& operator=(const InputProductType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // InputProductType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__InputProductType_h

