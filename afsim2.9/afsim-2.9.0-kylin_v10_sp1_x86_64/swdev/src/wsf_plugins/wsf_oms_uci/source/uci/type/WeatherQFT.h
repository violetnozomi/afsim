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
#ifndef Uci__Type__WeatherQFT_h
#define Uci__Type__WeatherQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__WeatherReportTypeEnum_h)
# include "uci/type/WeatherReportTypeEnum.h"
#endif

#if !defined(Uci__Type__WeatherDataTypeEnum_h)
# include "uci/type/WeatherDataTypeEnum.h"
#endif

#if !defined(Uci__Type__WeatherWarningEnum_h)
# include "uci/type/WeatherWarningEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherQFT sequence accessor class */
      class WeatherQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~WeatherQFT()
         { }

         /** Returns this accessor's type constant, i.e. WeatherQFT
           *
           * @return This accessor's type constant, i.e. WeatherQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter by type of the weather report [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::WeatherReportTypeEnum, uci::type::accessorType::weatherReportTypeEnum> ReportType;

         /** Filter by weather data type of the report. This is the WeatherDataType sequence of optionals in the form of an
           * enumeration. The presence of that optional element within the weather message would fit the filter. [Minimum
           * occurrences: 0] [Maximum occurrences: 11]
           */
         typedef uci::base::BoundedList<uci::type::WeatherDataTypeEnum, uci::type::accessorType::weatherDataTypeEnum> WeatherDataType;

         /** Filter by type of warning [Minimum occurrences: 0] [Maximum occurrences: 16] */
         typedef uci::base::BoundedList<uci::type::WeatherWarningEnum, uci::type::accessorType::weatherWarningEnum> WarningType;

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


         /** Returns the bounded list that is identified by the ReportType.
           *
           * @return The bounded list identified by ReportType.
           */
         virtual const uci::type::WeatherQFT::ReportType& getReportType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReportType.
           *
           * @return The bounded list identified by ReportType.
           */
         virtual uci::type::WeatherQFT::ReportType& getReportType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ReportType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReportType(const uci::type::WeatherQFT::ReportType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WeatherDataType.
           *
           * @return The bounded list identified by WeatherDataType.
           */
         virtual const uci::type::WeatherQFT::WeatherDataType& getWeatherDataType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WeatherDataType.
           *
           * @return The bounded list identified by WeatherDataType.
           */
         virtual uci::type::WeatherQFT::WeatherDataType& getWeatherDataType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the WeatherDataType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWeatherDataType(const uci::type::WeatherQFT::WeatherDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WarningType.
           *
           * @return The bounded list identified by WarningType.
           */
         virtual const uci::type::WeatherQFT::WarningType& getWarningType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WarningType.
           *
           * @return The bounded list identified by WarningType.
           */
         virtual uci::type::WeatherQFT::WarningType& getWarningType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the WarningType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWarningType(const uci::type::WeatherQFT::WarningType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherQFT to copy from
           */
         WeatherQFT(const WeatherQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherQFT to the contents of the WeatherQFT on the right hand
           * side (rhs) of the assignment operator.WeatherQFT [only available to derived classes]
           *
           * @param rhs The WeatherQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::WeatherQFT
           * @return A constant reference to this WeatherQFT.
           */
         const WeatherQFT& operator=(const WeatherQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherQFT_h

