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
#ifndef Uci__Type__ProductMetadataMDT_h
#define Uci__Type__ProductMetadataMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__ProductOutputType_h)
# include "uci/type/ProductOutputType.h"
#endif

#if !defined(Uci__Type__FileNameType_h)
# include "uci/type/FileNameType.h"
#endif

#if !defined(Uci__Type__BasicEncyclopediaNumberType_h)
# include "uci/type/BasicEncyclopediaNumberType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductGeospatialLocationType_h)
# include "uci/type/ProductGeospatialLocationType.h"
#endif

#if !defined(Uci__Type__ProductCollectionLocationType_h)
# include "uci/type/ProductCollectionLocationType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__SourceFiltersType_h)
# include "uci/type/SourceFiltersType.h"
#endif

#if !defined(Uci__Type__CreationSourceEnum_h)
# include "uci/type/CreationSourceEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__ThumbnailType_h)
# include "uci/type/ThumbnailType.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Type__TaskMetadataType_h)
# include "uci/type/TaskMetadataType.h"
#endif

#if !defined(Uci__Type__EntityMetadataType_h)
# include "uci/type/EntityMetadataType.h"
#endif

#if !defined(Uci__Type__CapabilityMetadataType_h)
# include "uci/type/CapabilityMetadataType.h"
#endif

#if !defined(Uci__Type__SHA_2_256_HashType_h)
# include "uci/type/SHA_2_256_HashType.h"
#endif

#if !defined(Uci__Type__DemodulationParameterType_h)
# include "uci/type/DemodulationParameterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductMetadataMDT sequence accessor class */
      class ProductMetadataMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductMetadataMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductMetadataMDT
           *
           * @return This accessor's type constant, i.e. ProductMetadataMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productMetadataMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductMetadataMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Describes the geospatial location(s) which the product associated with this ProductMetadata is relevant. If more than
           * one is provided, they may be disjoint. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductGeospatialLocationType, uci::type::accessorType::productGeospatialLocationType> GeospatialLocation;

         /** Indicates the unique ID of a ProductMetadata message corresponding to the original product that this product was
           * derived from. It is possible for the original products to "expire" despite this reference. This differs from
           * "DerivedFrom" in SecurityInformation in that it refers to a specific ProductMetadataID for a product that might not
           * persist vs. a less specific name, description or identification that remains meaningful over time. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> DerivedFromID;

         /** Tasking sources work at different levels of detail and abstraction, progressing from what to do to how specifically
           * to do it. Inputs from abstract tasking sources and Tasks can be decomposed into discrete, actionable Tasks. This
           * element provides traceability to the external tasking or Task associated with the Product described by this
           * ProductMetadata. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TraceabilityType, uci::type::accessorType::traceabilityType> Traceability;

         /** Indicates Task-related metadata for Products created in response to Tasks. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskMetadataType, uci::type::accessorType::taskMetadataType> TaskMetadata;

         /** Indicates Entity-related metadata for Products which are associated with or contain Entities. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityMetadataType, uci::type::accessorType::entityMetadataType> EntityMetadata;

         /** Indicates Capability-related metadata for Products created in response to Capability commands. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityMetadataType, uci::type::accessorType::capabilityMetadataType> CapabilityMetadata;

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


         /** Returns the accessor that provides access to the complex content that is identified by the ProductDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductDescription.
           */
         virtual const uci::type::ProductOutputType& getProductDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductDescription.
           */
         virtual uci::type::ProductOutputType& getProductDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductDescription to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductDescription
           */
         virtual void setProductDescription(const uci::type::ProductOutputType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the BE_Number.
           *
           * @return The acecssor that provides access to the complex content that is identified by BE_Number.
           */
         virtual const uci::type::BasicEncyclopediaNumberType& getBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BE_Number.
           *
           * @return The acecssor that provides access to the complex content that is identified by BE_Number.
           */
         virtual uci::type::BasicEncyclopediaNumberType& getBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BE_Number to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BE_Number
           */
         virtual void setBE_Number(const uci::type::BasicEncyclopediaNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BE_Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BE_Number is emabled or not
           */
         virtual bool hasBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BE_Number
           *
           * @param type = uci::type::accessorType::basicEncyclopediaNumberType This Accessor's accessor type
           */
         virtual void enableBE_Number(uci::base::accessorType::AccessorType type = uci::type::accessorType::basicEncyclopediaNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BE_Number */
         virtual void clearBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SceneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SceneID.
           */
         virtual const uci::type::ForeignKeyType& getSceneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SceneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SceneID.
           */
         virtual uci::type::ForeignKeyType& getSceneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SceneID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SceneID
           */
         virtual void setSceneID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SceneID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SceneID is emabled or not
           */
         virtual bool hasSceneID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SceneID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableSceneID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SceneID */
         virtual void clearSceneID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeospatialLocation.
           *
           * @return The bounded list identified by GeospatialLocation.
           */
         virtual const uci::type::ProductMetadataMDT::GeospatialLocation& getGeospatialLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeospatialLocation.
           *
           * @return The bounded list identified by GeospatialLocation.
           */
         virtual uci::type::ProductMetadataMDT::GeospatialLocation& getGeospatialLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeospatialLocation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeospatialLocation(const uci::type::ProductMetadataMDT::GeospatialLocation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionLocation.
           */
         virtual const uci::type::ProductCollectionLocationType& getCollectionLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionLocation.
           */
         virtual uci::type::ProductCollectionLocationType& getCollectionLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionLocation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionLocation
           */
         virtual void setCollectionLocation(const uci::type::ProductCollectionLocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionLocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionLocation is emabled or not
           */
         virtual bool hasCollectionLocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionLocation
           *
           * @param type = uci::type::accessorType::productCollectionLocationType This Accessor's accessor type
           */
         virtual void enableCollectionLocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::productCollectionLocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionLocation */
         virtual void clearCollectionLocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductSource.
           */
         virtual const uci::type::SourceFiltersType& getProductSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductSource.
           */
         virtual uci::type::SourceFiltersType& getProductSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductSource to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductSource
           */
         virtual void setProductSource(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CreationSource.
           *
           * @return The value of the enumeration identified by CreationSource.
           */
         virtual const uci::type::CreationSourceEnum& getCreationSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CreationSource.
           *
           * @return The value of the enumeration identified by CreationSource.
           */
         virtual uci::type::CreationSourceEnum& getCreationSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CreationSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCreationSource(const uci::type::CreationSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CreationSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCreationSource(uci::type::CreationSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UntrustedModification.
           *
           * @return The value of the simple primitive data type identified by UntrustedModification.
           */
         virtual xs::Boolean getUntrustedModification() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UntrustedModification.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUntrustedModification(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the Thumbnail.
           *
           * @return The acecssor that provides access to the complex content that is identified by Thumbnail.
           */
         virtual const uci::type::ThumbnailType& getThumbnail() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Thumbnail.
           *
           * @return The acecssor that provides access to the complex content that is identified by Thumbnail.
           */
         virtual uci::type::ThumbnailType& getThumbnail()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Thumbnail to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Thumbnail
           */
         virtual void setThumbnail(const uci::type::ThumbnailType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Thumbnail exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Thumbnail is emabled or not
           */
         virtual bool hasThumbnail() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Thumbnail
           *
           * @param type = uci::type::accessorType::thumbnailType This Accessor's accessor type
           */
         virtual void enableThumbnail(uci::base::accessorType::AccessorType type = uci::type::accessorType::thumbnailType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Thumbnail */
         virtual void clearThumbnail()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DerivedFromID.
           *
           * @return The bounded list identified by DerivedFromID.
           */
         virtual const uci::type::ProductMetadataMDT::DerivedFromID& getDerivedFromID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DerivedFromID.
           *
           * @return The bounded list identified by DerivedFromID.
           */
         virtual uci::type::ProductMetadataMDT::DerivedFromID& getDerivedFromID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DerivedFromID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDerivedFromID(const uci::type::ProductMetadataMDT::DerivedFromID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Traceability.
           *
           * @return The bounded list identified by Traceability.
           */
         virtual const uci::type::ProductMetadataMDT::Traceability& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Traceability.
           *
           * @return The bounded list identified by Traceability.
           */
         virtual uci::type::ProductMetadataMDT::Traceability& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Traceability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTraceability(const uci::type::ProductMetadataMDT::Traceability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskMetadata.
           *
           * @return The bounded list identified by TaskMetadata.
           */
         virtual const uci::type::ProductMetadataMDT::TaskMetadata& getTaskMetadata() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskMetadata.
           *
           * @return The bounded list identified by TaskMetadata.
           */
         virtual uci::type::ProductMetadataMDT::TaskMetadata& getTaskMetadata()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskMetadata.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskMetadata(const uci::type::ProductMetadataMDT::TaskMetadata& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityMetadata.
           *
           * @return The bounded list identified by EntityMetadata.
           */
         virtual const uci::type::ProductMetadataMDT::EntityMetadata& getEntityMetadata() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityMetadata.
           *
           * @return The bounded list identified by EntityMetadata.
           */
         virtual uci::type::ProductMetadataMDT::EntityMetadata& getEntityMetadata()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityMetadata.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityMetadata(const uci::type::ProductMetadataMDT::EntityMetadata& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityMetadata.
           *
           * @return The bounded list identified by CapabilityMetadata.
           */
         virtual const uci::type::ProductMetadataMDT::CapabilityMetadata& getCapabilityMetadata() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityMetadata.
           *
           * @return The bounded list identified by CapabilityMetadata.
           */
         virtual uci::type::ProductMetadataMDT::CapabilityMetadata& getCapabilityMetadata()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityMetadata.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityMetadata(const uci::type::ProductMetadataMDT::CapabilityMetadata& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SHA_2_Hash field contained in this message fragment
           *
           * @return The reference to the accessor to the SHA_2_Hash field.
           */
         virtual const uci::type::SHA_2_256_HashType& getSHA_2_Hash() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SHA_2_Hash field contained in this message fragment
           *
           * @return The reference to the accessor to the SHA_2_Hash field.
           */
         virtual uci::type::SHA_2_256_HashType& getSHA_2_Hash()
            throw(uci::base::UCIException) = 0;


         /** Set the contents of the message fragment that this accesssor provides access to to the contents the specified
           * accessor provides access to. A deep copy will be used by this method in setting the contents
           *
           * @param value The accessor to use to set the contents of the message fragment that this accessor provides access to.
           */
         virtual void setSHA_2_Hash(const uci::type::SHA_2_256_HashType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SHA_2_Hashis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSHA_2_Hash() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSHA_2_Hash will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::sHA_2_256_HashType This Accessor's accessor type
           */
         virtual void enableSHA_2_Hash(uci::base::accessorType::AccessorType type = uci::type::accessorType::sHA_2_256_HashType)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSHA_2_Hash()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DemodulationParameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by DemodulationParameters.
           */
         virtual const uci::type::DemodulationParameterType& getDemodulationParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DemodulationParameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by DemodulationParameters.
           */
         virtual uci::type::DemodulationParameterType& getDemodulationParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DemodulationParameters to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DemodulationParameters
           */
         virtual void setDemodulationParameters(const uci::type::DemodulationParameterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DemodulationParameters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DemodulationParameters is emabled or not
           */
         virtual bool hasDemodulationParameters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DemodulationParameters
           *
           * @param type = uci::type::accessorType::demodulationParameterType This Accessor's accessor type
           */
         virtual void enableDemodulationParameters(uci::base::accessorType::AccessorType type = uci::type::accessorType::demodulationParameterType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DemodulationParameters */
         virtual void clearDemodulationParameters()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductMetadataMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductMetadataMDT to copy from
           */
         ProductMetadataMDT(const ProductMetadataMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductMetadataMDT to the contents of the ProductMetadataMDT on
           * the right hand side (rhs) of the assignment operator.ProductMetadataMDT [only available to derived classes]
           *
           * @param rhs The ProductMetadataMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ProductMetadataMDT
           * @return A constant reference to this ProductMetadataMDT.
           */
         const ProductMetadataMDT& operator=(const ProductMetadataMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductMetadataMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductMetadataMDT_h

