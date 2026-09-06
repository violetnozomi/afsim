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
#ifndef Uci__Type__ProductReferenceType_h
#define Uci__Type__ProductReferenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FileNameType_h)
# include "uci/type/FileNameType.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__ProductReferenceByParentType_h)
# include "uci/type/ProductReferenceByParentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductReferenceType choice accessor class */
      class ProductReferenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductReferenceType()
         { }

         /** Returns this accessor's type constant, i.e. ProductReferenceType
           *
           * @return This accessor's type constant, i.e. ProductReferenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productReferenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductReferenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum ProductReferenceTypeChoice {
            PRODUCTREFERENCETYPE_CHOICE_NONE,
            PRODUCTREFERENCETYPE_CHOICE_FILENAME,
            PRODUCTREFERENCETYPE_CHOICE_PRODUCTMETADATAID,
            PRODUCTREFERENCETYPE_CHOICE_PRODUCTREFERENCEBYPARENT
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual ProductReferenceTypeChoice getProductReferenceTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getProductReferenceTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
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
         virtual void setProductReferenceTypeChoiceOrdinal(ProductReferenceTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileName.
           *
           * @return The value of the string data type identified by FileName.
           */
         virtual const uci::type::FileNameType& getFileName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileName.
           *
           * @return The value of the string data type identified by FileName.
           */
         virtual uci::type::FileNameType& getFileName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileName(const uci::type::FileNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductMetadataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductMetadataID.
           */
         virtual const uci::type::ProductMetadataID_Type& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductMetadataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductMetadataID.
           */
         virtual uci::type::ProductMetadataID_Type& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductMetadataID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductMetadataID
           */
         virtual void setProductMetadataID(const uci::type::ProductMetadataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductReferenceByParent.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductReferenceByParent.
           */
         virtual const uci::type::ProductReferenceByParentType& getProductReferenceByParent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductReferenceByParent.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductReferenceByParent.
           */
         virtual uci::type::ProductReferenceByParentType& getProductReferenceByParent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductReferenceByParent to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductReferenceByParent
           */
         virtual void setProductReferenceByParent(const uci::type::ProductReferenceByParentType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductReferenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductReferenceType to copy from
           */
         ProductReferenceType(const ProductReferenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductReferenceType to the contents of the ProductReferenceType
           * on the right hand side (rhs) of the assignment operator.ProductReferenceType [only available to derived classes]
           *
           * @param rhs The ProductReferenceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ProductReferenceType
           * @return A constant reference to this ProductReferenceType.
           */
         const ProductReferenceType& operator=(const ProductReferenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductReferenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductReferenceType_h

