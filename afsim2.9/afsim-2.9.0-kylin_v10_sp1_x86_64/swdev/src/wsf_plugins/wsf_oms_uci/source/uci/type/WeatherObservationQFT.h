// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__WeatherObservationQFT_h
#define Uci__Type__WeatherObservationQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
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

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherObservationQFT sequence accessor class */
      class WeatherObservationQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~WeatherObservationQFT()
         { }

         /** Returns this accessor's type constant, i.e. WeatherObservationQFT
           *
           * @return This accessor's type constant, i.e. WeatherObservationQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherObservationQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherObservationQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilter.
           */
         virtual const uci::type::GeoFiltersType& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilter.
           */
         virtual uci::type::GeoFiltersType& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeoFilter to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeoFilter
           */
         virtual void setGeoFilter(const uci::type::GeoFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GeoFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GeoFilter is emabled or not
           */
         virtual bool hasGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GeoFilter
           *
           * @param type = uci::type::accessorType::geoFiltersType This Accessor's accessor type
           */
         virtual void enableGeoFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::geoFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GeoFilter */
         virtual void clearGeoFilter()
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


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilter.
           */
         virtual const uci::type::SourceFiltersType& getSourceFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilter.
           */
         virtual uci::type::SourceFiltersType& getSourceFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceFilter to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceFilter
           */
         virtual void setSourceFilter(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceFilter is emabled or not
           */
         virtual bool hasSourceFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceFilter
           *
           * @param type = uci::type::accessorType::sourceFiltersType This Accessor's accessor type
           */
         virtual void enableSourceFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::sourceFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceFilter */
         virtual void clearSourceFilter()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherObservationQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherObservationQFT to copy from
           */
         WeatherObservationQFT(const WeatherObservationQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherObservationQFT to the contents of the WeatherObservationQFT
           * on the right hand side (rhs) of the assignment operator.WeatherObservationQFT [only available to derived classes]
           *
           * @param rhs The WeatherObservationQFT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::WeatherObservationQFT
           * @return A constant reference to this WeatherObservationQFT.
           */
         const WeatherObservationQFT& operator=(const WeatherObservationQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherObservationQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherObservationQFT_h

