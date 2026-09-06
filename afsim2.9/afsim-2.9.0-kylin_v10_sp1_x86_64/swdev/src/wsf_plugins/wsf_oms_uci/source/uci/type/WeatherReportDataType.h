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
#ifndef Uci__Type__WeatherReportDataType_h
#define Uci__Type__WeatherReportDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__WeatherReportTypeEnum_h)
# include "uci/type/WeatherReportTypeEnum.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__WeatherReportType_h)
# include "uci/type/WeatherReportType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherReportDataType sequence accessor class */
      class WeatherReportDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~WeatherReportDataType()
         { }

         /** Returns this accessor's type constant, i.e. WeatherReportDataType
           *
           * @return This accessor's type constant, i.e. WeatherReportDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherReportDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherReportDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the time interval over which the report applies. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> Schedule;

         /** Returns the value of the enumeration that is identified by the WeatherReportType.
           *
           * @return The value of the enumeration identified by WeatherReportType.
           */
         virtual const uci::type::WeatherReportTypeEnum& getWeatherReportType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the WeatherReportType.
           *
           * @return The value of the enumeration identified by WeatherReportType.
           */
         virtual uci::type::WeatherReportTypeEnum& getWeatherReportType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the WeatherReportType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeatherReportType(const uci::type::WeatherReportTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the WeatherReportType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeatherReportType(uci::type::WeatherReportTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Created.
           *
           * @return The value of the simple primitive data type identified by Created.
           */
         virtual uci::type::DateTimeTypeValue getCreated() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Created.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCreated(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Schedule.
           *
           * @return The bounded list identified by Schedule.
           */
         virtual const uci::type::WeatherReportDataType::Schedule& getSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Schedule.
           *
           * @return The bounded list identified by Schedule.
           */
         virtual uci::type::WeatherReportDataType::Schedule& getSchedule()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Schedule.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSchedule(const uci::type::WeatherReportDataType::Schedule& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherArea.
           */
         virtual const uci::type::ZoneType& getWeatherArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherArea.
           */
         virtual uci::type::ZoneType& getWeatherArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherArea to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherArea
           */
         virtual void setWeatherArea(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the QNH.
           *
           * @return The value of the simple primitive data type identified by QNH.
           */
         virtual xs::Float getQNH() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the QNH.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setQNH(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by QNH exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by QNH is emabled or not
           */
         virtual bool hasQNH() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by QNH
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableQNH(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by QNH */
         virtual void clearQNH()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherData.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherData.
           */
         virtual const uci::type::WeatherReportType& getWeatherData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherData.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherData.
           */
         virtual uci::type::WeatherReportType& getWeatherData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherData
           */
         virtual void setWeatherData(const uci::type::WeatherReportType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherReportDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherReportDataType to copy from
           */
         WeatherReportDataType(const WeatherReportDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherReportDataType to the contents of the WeatherReportDataType
           * on the right hand side (rhs) of the assignment operator.WeatherReportDataType [only available to derived classes]
           *
           * @param rhs The WeatherReportDataType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::WeatherReportDataType
           * @return A constant reference to this WeatherReportDataType.
           */
         const WeatherReportDataType& operator=(const WeatherReportDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherReportDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherReportDataType_h

