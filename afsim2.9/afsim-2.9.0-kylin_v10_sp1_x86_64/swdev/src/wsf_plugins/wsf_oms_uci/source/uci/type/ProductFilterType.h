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
#ifndef Uci__Type__ProductFilterType_h
#define Uci__Type__ProductFilterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SourceFilterType_h)
# include "uci/type/SourceFilterType.h"
#endif

#if !defined(Uci__Type__LocationFilterType_h)
# include "uci/type/LocationFilterType.h"
#endif

#if !defined(Uci__Type__SourceLocationFilterType_h)
# include "uci/type/SourceLocationFilterType.h"
#endif

#if !defined(Uci__Type__FileFormatType_h)
# include "uci/type/FileFormatType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This element defines a filter which can be applied to any product regardless of type */
      class ProductFilterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductFilterType()
         { }

         /** Returns this accessor's type constant, i.e. ProductFilterType
           *
           * @return This accessor's type constant, i.e. ProductFilterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productFilterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductFilterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum ProductFilterTypeChoice {
            PRODUCTFILTERTYPE_CHOICE_NONE,
            PRODUCTFILTERTYPE_CHOICE_SOURCE,
            PRODUCTFILTERTYPE_CHOICE_GEOSPATIAL,
            PRODUCTFILTERTYPE_CHOICE_SOURCEGEOSPATIAL,
            PRODUCTFILTERTYPE_CHOICE_FILEFORMAT,
            PRODUCTFILTERTYPE_CHOICE_SECURITYINFORMATION,
            PRODUCTFILTERTYPE_CHOICE_DATETIMERANGE,
            PRODUCTFILTERTYPE_CHOICE_XPATH,
            PRODUCTFILTERTYPE_CHOICE_PRODUCTTYPE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual ProductFilterTypeChoice getProductFilterTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getProductFilterTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
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
         virtual void setProductFilterTypeChoiceOrdinal(ProductFilterTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::SourceFilterType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::SourceFilterType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::SourceFilterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Geospatial.
           *
           * @return The acecssor that provides access to the complex content that is identified by Geospatial.
           */
         virtual const uci::type::LocationFilterType& getGeospatial() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Geospatial.
           *
           * @return The acecssor that provides access to the complex content that is identified by Geospatial.
           */
         virtual uci::type::LocationFilterType& getGeospatial()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Geospatial to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Geospatial
           */
         virtual void setGeospatial(const uci::type::LocationFilterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceGeospatial.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceGeospatial.
           */
         virtual const uci::type::SourceLocationFilterType& getSourceGeospatial() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceGeospatial.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceGeospatial.
           */
         virtual uci::type::SourceLocationFilterType& getSourceGeospatial()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceGeospatial to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceGeospatial
           */
         virtual void setSourceGeospatial(const uci::type::SourceLocationFilterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFormat.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFormat.
           */
         virtual const uci::type::FileFormatType& getFileFormat() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFormat.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFormat.
           */
         virtual uci::type::FileFormatType& getFileFormat()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileFormat to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileFormat
           */
         virtual void setFileFormat(const uci::type::FileFormatType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the DateTimeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DateTimeRange.
           */
         virtual const uci::type::DateTimeRangeType& getDateTimeRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DateTimeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DateTimeRange.
           */
         virtual uci::type::DateTimeRangeType& getDateTimeRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DateTimeRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DateTimeRange
           */
         virtual void setDateTimeRange(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the XPath.
           *
           * @return The value of the string data type identified by XPath.
           */
         virtual const uci::type::VisibleString1024Type& getXPath() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the XPath.
           *
           * @return The value of the string data type identified by XPath.
           */
         virtual uci::type::VisibleString1024Type& getXPath()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the XPath to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setXPath(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the XPath to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setXPath(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the XPath to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setXPath(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProductType.
           *
           * @return The value of the enumeration identified by ProductType.
           */
         virtual const uci::type::ProductTypeEnum& getProductType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProductType.
           *
           * @return The value of the enumeration identified by ProductType.
           */
         virtual uci::type::ProductTypeEnum& getProductType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductType(const uci::type::ProductTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductType(uci::type::ProductTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductFilterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductFilterType to copy from
           */
         ProductFilterType(const ProductFilterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductFilterType to the contents of the ProductFilterType on the
           * right hand side (rhs) of the assignment operator.ProductFilterType [only available to derived classes]
           *
           * @param rhs The ProductFilterType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ProductFilterType
           * @return A constant reference to this ProductFilterType.
           */
         const ProductFilterType& operator=(const ProductFilterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductFilterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductFilterType_h

