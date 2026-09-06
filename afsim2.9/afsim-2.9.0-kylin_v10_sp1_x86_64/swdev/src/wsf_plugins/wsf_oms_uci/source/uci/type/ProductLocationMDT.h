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
#ifndef Uci__Type__ProductLocationMDT_h
#define Uci__Type__ProductLocationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductLocationID_Type_h)
# include "uci/type/ProductLocationID_Type.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__MediaLocationStatusEnum_h)
# include "uci/type/MediaLocationStatusEnum.h"
#endif

#if !defined(Uci__Type__ProductLocationType_h)
# include "uci/type/ProductLocationType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EndpointReferenceType_h)
# include "uci/type/EndpointReferenceType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ArchiveRequestType_h)
# include "uci/type/ArchiveRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductLocationMDT sequence accessor class */
      class ProductLocationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductLocationMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductLocationMDT
           *
           * @return This accessor's type constant, i.e. ProductLocationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productLocationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductLocationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the network location at which the product thumbnail, whose data format is not defined within the UCI
           * schema, associated with this ProductMetadata can be found. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EndpointReferenceType, uci::type::accessorType::endpointReferenceType> ThumbnailNetworkLocation;

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


         /** Returns the value of the enumeration that is identified by the ProductLocationStatus.
           *
           * @return The value of the enumeration identified by ProductLocationStatus.
           */
         virtual const uci::type::MediaLocationStatusEnum& getProductLocationStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProductLocationStatus.
           *
           * @return The value of the enumeration identified by ProductLocationStatus.
           */
         virtual uci::type::MediaLocationStatusEnum& getProductLocationStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductLocationStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductLocationStatus(const uci::type::MediaLocationStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductLocationStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductLocationStatus(uci::type::MediaLocationStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual const uci::type::ProductLocationType& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual uci::type::ProductLocationType& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Location to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Location
           */
         virtual void setLocation(const uci::type::ProductLocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThumbnailNetworkLocation.
           *
           * @return The bounded list identified by ThumbnailNetworkLocation.
           */
         virtual const uci::type::ProductLocationMDT::ThumbnailNetworkLocation& getThumbnailNetworkLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThumbnailNetworkLocation.
           *
           * @return The bounded list identified by ThumbnailNetworkLocation.
           */
         virtual uci::type::ProductLocationMDT::ThumbnailNetworkLocation& getThumbnailNetworkLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ThumbnailNetworkLocation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setThumbnailNetworkLocation(const uci::type::ProductLocationMDT::ThumbnailNetworkLocation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Expires.
           *
           * @return The value of the simple primitive data type identified by Expires.
           */
         virtual uci::type::DateTimeTypeValue getExpires() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Expires.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExpires(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Expires exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Expires is emabled or not
           */
         virtual bool hasExpires() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Expires
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableExpires(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Expires */
         virtual void clearExpires()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ArchiveConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by ArchiveConfiguration.
           */
         virtual const uci::type::ArchiveRequestType& getArchiveConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ArchiveConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by ArchiveConfiguration.
           */
         virtual uci::type::ArchiveRequestType& getArchiveConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ArchiveConfiguration to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ArchiveConfiguration
           */
         virtual void setArchiveConfiguration(const uci::type::ArchiveRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ArchiveConfiguration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ArchiveConfiguration is emabled or not
           */
         virtual bool hasArchiveConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ArchiveConfiguration
           *
           * @param type = uci::type::accessorType::archiveRequestType This Accessor's accessor type
           */
         virtual void enableArchiveConfiguration(uci::base::accessorType::AccessorType type = uci::type::accessorType::archiveRequestType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ArchiveConfiguration */
         virtual void clearArchiveConfiguration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductLocationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductLocationMDT to copy from
           */
         ProductLocationMDT(const ProductLocationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductLocationMDT to the contents of the ProductLocationMDT on
           * the right hand side (rhs) of the assignment operator.ProductLocationMDT [only available to derived classes]
           *
           * @param rhs The ProductLocationMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ProductLocationMDT
           * @return A constant reference to this ProductLocationMDT.
           */
         const ProductLocationMDT& operator=(const ProductLocationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductLocationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductLocationMDT_h

