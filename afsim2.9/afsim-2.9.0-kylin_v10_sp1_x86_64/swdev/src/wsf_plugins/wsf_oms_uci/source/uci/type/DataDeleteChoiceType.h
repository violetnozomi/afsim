// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DataDeleteChoiceType_h
#define Uci__Type__DataDeleteChoiceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__ProductLocationID_Type_h)
# include "uci/type/ProductLocationID_Type.h"
#endif

#if !defined(Uci__Type__FileMetadataID_Type_h)
# include "uci/type/FileMetadataID_Type.h"
#endif

#if !defined(Uci__Type__FileLocationID_Type_h)
# include "uci/type/FileLocationID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DataDeleteChoiceType choice accessor class */
      class DataDeleteChoiceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataDeleteChoiceType()
         { }

         /** Returns this accessor's type constant, i.e. DataDeleteChoiceType
           *
           * @return This accessor's type constant, i.e. DataDeleteChoiceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataDeleteChoiceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataDeleteChoiceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific Capability whose associated data is to be deleted. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates a specific Component whose associated data is to be deleted. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum DataDeleteChoiceTypeChoice {
            DATADELETECHOICETYPE_CHOICE_NONE,
            DATADELETECHOICETYPE_CHOICE_PRODUCTMETADATAID,
            DATADELETECHOICETYPE_CHOICE_PRODUCTLOCATIONID,
            DATADELETECHOICETYPE_CHOICE_FILEMETADATAID,
            DATADELETECHOICETYPE_CHOICE_FILELOCATIONID,
            DATADELETECHOICETYPE_CHOICE_CAPABILITYID,
            DATADELETECHOICETYPE_CHOICE_COMPONENTID
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual DataDeleteChoiceTypeChoice getDataDeleteChoiceTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getDataDeleteChoiceTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
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
         virtual void setDataDeleteChoiceTypeChoiceOrdinal(DataDeleteChoiceTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
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


         /** Returns the accessor that provides access to the complex content that is identified by the ProductLocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductLocationID.
           */
         virtual const uci::type::ProductLocationID_Type& getProductLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductLocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductLocationID.
           */
         virtual uci::type::ProductLocationID_Type& getProductLocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductLocationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductLocationID
           */
         virtual void setProductLocationID(const uci::type::ProductLocationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileMetadataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileMetadataID.
           */
         virtual const uci::type::FileMetadataID_Type& getFileMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileMetadataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileMetadataID.
           */
         virtual uci::type::FileMetadataID_Type& getFileMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileMetadataID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileMetadataID
           */
         virtual void setFileMetadataID(const uci::type::FileMetadataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileLocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileLocationID.
           */
         virtual const uci::type::FileLocationID_Type& getFileLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileLocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileLocationID.
           */
         virtual uci::type::FileLocationID_Type& getFileLocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileLocationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileLocationID
           */
         virtual void setFileLocationID(const uci::type::FileLocationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::DataDeleteChoiceType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::DataDeleteChoiceType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::DataDeleteChoiceType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::DataDeleteChoiceType::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::DataDeleteChoiceType::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::DataDeleteChoiceType::ComponentID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataDeleteChoiceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataDeleteChoiceType to copy from
           */
         DataDeleteChoiceType(const DataDeleteChoiceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataDeleteChoiceType to the contents of the DataDeleteChoiceType
           * on the right hand side (rhs) of the assignment operator.DataDeleteChoiceType [only available to derived classes]
           *
           * @param rhs The DataDeleteChoiceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DataDeleteChoiceType
           * @return A constant reference to this DataDeleteChoiceType.
           */
         const DataDeleteChoiceType& operator=(const DataDeleteChoiceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataDeleteChoiceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataDeleteChoiceType_h

