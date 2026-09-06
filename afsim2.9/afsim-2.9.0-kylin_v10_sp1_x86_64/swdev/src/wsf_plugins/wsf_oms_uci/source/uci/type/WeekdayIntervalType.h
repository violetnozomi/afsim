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
#ifndef Uci__Type__WeekdayIntervalType_h
#define Uci__Type__WeekdayIntervalType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__WeekdayEnum_h)
# include "uci/type/WeekdayEnum.h"
#endif

#if !defined(Uci__Type__TimeType_h)
# include "uci/type/TimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeekdayIntervalType sequence accessor class */
      class WeekdayIntervalType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~WeekdayIntervalType()
         { }

         /** Returns this accessor's type constant, i.e. WeekdayIntervalType
           *
           * @return This accessor's type constant, i.e. WeekdayIntervalType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weekdayIntervalType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeekdayIntervalType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the days of the week in which the interval is valid. [Maximum occurrences: 7] */
         typedef uci::base::BoundedList<uci::type::WeekdayEnum, uci::type::accessorType::weekdayEnum> Weekday;

         /** Returns the accessor that provides access to the complex content that is identified by the TimeSpan.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeSpan.
           */
         virtual const uci::type::DateTimeRangeType& getTimeSpan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeSpan.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeSpan.
           */
         virtual uci::type::DateTimeRangeType& getTimeSpan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimeSpan to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimeSpan
           */
         virtual void setTimeSpan(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Weekday.
           *
           * @return The bounded list identified by Weekday.
           */
         virtual const uci::type::WeekdayIntervalType::Weekday& getWeekday() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Weekday.
           *
           * @return The bounded list identified by Weekday.
           */
         virtual uci::type::WeekdayIntervalType::Weekday& getWeekday()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Weekday.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWeekday(const uci::type::WeekdayIntervalType::Weekday& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @return The value of the simple primitive data type identified by StartTime.
           */
         virtual uci::type::TimeTypeValue getStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartTime(uci::type::TimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StartTime is emabled or not
           */
         virtual bool hasStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StartTime
           *
           * @param type = uci::type::accessorType::timeType This Accessor's accessor type
           */
         virtual void enableStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::timeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StartTime */
         virtual void clearStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EndTime.
           *
           * @return The value of the simple primitive data type identified by EndTime.
           */
         virtual uci::type::TimeTypeValue getEndTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EndTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEndTime(uci::type::TimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EndTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EndTime is emabled or not
           */
         virtual bool hasEndTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EndTime
           *
           * @param type = uci::type::accessorType::timeType This Accessor's accessor type
           */
         virtual void enableEndTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::timeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EndTime */
         virtual void clearEndTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeekdayIntervalType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeekdayIntervalType to copy from
           */
         WeekdayIntervalType(const WeekdayIntervalType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeekdayIntervalType to the contents of the WeekdayIntervalType on
           * the right hand side (rhs) of the assignment operator.WeekdayIntervalType [only available to derived classes]
           *
           * @param rhs The WeekdayIntervalType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::WeekdayIntervalType
           * @return A constant reference to this WeekdayIntervalType.
           */
         const WeekdayIntervalType& operator=(const WeekdayIntervalType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeekdayIntervalType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeekdayIntervalType_h

