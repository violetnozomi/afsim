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
#ifndef Uci__Type__ProductMetadataQFT_h
#define Uci__Type__ProductMetadataQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__MetadataFiltersType_h)
# include "uci/type/MetadataFiltersType.h"
#endif

#if !defined(Uci__Type__FileFiltersType_h)
# include "uci/type/FileFiltersType.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__SourceFiltersType_h)
# include "uci/type/SourceFiltersType.h"
#endif

#if !defined(Uci__Type__WeatherFiltersType_h)
# include "uci/type/WeatherFiltersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductMetadataQFT sequence accessor class */
      class ProductMetadataQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ProductMetadataQFT()
         { }

         /** Returns this accessor's type constant, i.e. ProductMetadataQFT
           *
           * @return This accessor's type constant, i.e. ProductMetadataQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productMetadataQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductMetadataQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IDsOnly.
           *
           * @return The value of the simple primitive data type identified by IDsOnly.
           */
         virtual xs::Boolean getIDsOnly() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IDsOnly.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIDsOnly(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetadataFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetadataFilters.
           */
         virtual const uci::type::MetadataFiltersType& getMetadataFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetadataFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetadataFilters.
           */
         virtual uci::type::MetadataFiltersType& getMetadataFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MetadataFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MetadataFilters
           */
         virtual void setMetadataFilters(const uci::type::MetadataFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MetadataFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MetadataFilters is emabled or not
           */
         virtual bool hasMetadataFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MetadataFilters
           *
           * @param type = uci::type::accessorType::metadataFiltersType This Accessor's accessor type
           */
         virtual void enableMetadataFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::metadataFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MetadataFilters */
         virtual void clearMetadataFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFilters.
           */
         virtual const uci::type::FileFiltersType& getFileFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFilters.
           */
         virtual uci::type::FileFiltersType& getFileFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileFilters
           */
         virtual void setFileFilters(const uci::type::FileFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileFilters is emabled or not
           */
         virtual bool hasFileFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileFilters
           *
           * @param type = uci::type::accessorType::fileFiltersType This Accessor's accessor type
           */
         virtual void enableFileFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::fileFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileFilters */
         virtual void clearFileFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilters.
           */
         virtual const uci::type::GeoFiltersType& getGeoFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilters.
           */
         virtual uci::type::GeoFiltersType& getGeoFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeoFilters to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeoFilters
           */
         virtual void setGeoFilters(const uci::type::GeoFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GeoFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GeoFilters is emabled or not
           */
         virtual bool hasGeoFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GeoFilters
           *
           * @param type = uci::type::accessorType::geoFiltersType This Accessor's accessor type
           */
         virtual void enableGeoFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::geoFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GeoFilters */
         virtual void clearGeoFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalFilter.
           */
         virtual const uci::type::DateTimeRangeType& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalFilter.
           */
         virtual uci::type::DateTimeRangeType& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TemporalFilter to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TemporalFilter
           */
         virtual void setTemporalFilter(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TemporalFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TemporalFilter is emabled or not
           */
         virtual bool hasTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TemporalFilter
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableTemporalFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TemporalFilter */
         virtual void clearTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilters.
           */
         virtual const uci::type::SourceFiltersType& getSourceFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilters.
           */
         virtual uci::type::SourceFiltersType& getSourceFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceFilters
           */
         virtual void setSourceFilters(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceFilters is emabled or not
           */
         virtual bool hasSourceFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceFilters
           *
           * @param type = uci::type::accessorType::sourceFiltersType This Accessor's accessor type
           */
         virtual void enableSourceFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::sourceFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceFilters */
         virtual void clearSourceFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherFilters.
           */
         virtual const uci::type::WeatherFiltersType& getWeatherFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherFilters.
           */
         virtual uci::type::WeatherFiltersType& getWeatherFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherFilters
           */
         virtual void setWeatherFilters(const uci::type::WeatherFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeatherFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeatherFilters is emabled or not
           */
         virtual bool hasWeatherFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeatherFilters
           *
           * @param type = uci::type::accessorType::weatherFiltersType This Accessor's accessor type
           */
         virtual void enableWeatherFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeatherFilters */
         virtual void clearWeatherFilters()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductMetadataQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductMetadataQFT to copy from
           */
         ProductMetadataQFT(const ProductMetadataQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductMetadataQFT to the contents of the ProductMetadataQFT on
           * the right hand side (rhs) of the assignment operator.ProductMetadataQFT [only available to derived classes]
           *
           * @param rhs The ProductMetadataQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ProductMetadataQFT
           * @return A constant reference to this ProductMetadataQFT.
           */
         const ProductMetadataQFT& operator=(const ProductMetadataQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductMetadataQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductMetadataQFT_h

