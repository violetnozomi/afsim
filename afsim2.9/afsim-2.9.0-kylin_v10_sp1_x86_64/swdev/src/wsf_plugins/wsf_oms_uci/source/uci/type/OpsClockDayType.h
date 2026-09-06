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
#ifndef Uci__Type__OpsClockDayType_h
#define Uci__Type__OpsClockDayType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__WeekdayEnum_h)
# include "uci/type/WeekdayEnum.h"
#endif

#if !defined(Uci__Type__ObservationCountsByHourType_h)
# include "uci/type/ObservationCountsByHourType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The consolidated operational schedule AKA Ops Clock: the seven days of the week with the corresponding 24 hour of
        * counts of observations.
        */
      class OpsClockDayType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpsClockDayType()
         { }

         /** Returns this accessor's type constant, i.e. OpsClockDayType
           *
           * @return This accessor's type constant, i.e. OpsClockDayType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opsClockDayType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpsClockDayType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DayOfWeek.
           *
           * @return The value of the enumeration identified by DayOfWeek.
           */
         virtual const uci::type::WeekdayEnum& getDayOfWeek() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DayOfWeek.
           *
           * @return The value of the enumeration identified by DayOfWeek.
           */
         virtual uci::type::WeekdayEnum& getDayOfWeek()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DayOfWeek.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDayOfWeek(const uci::type::WeekdayEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DayOfWeek.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDayOfWeek(uci::type::WeekdayEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ObservationCountsByHour.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObservationCountsByHour.
           */
         virtual const uci::type::ObservationCountsByHourType& getObservationCountsByHour() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ObservationCountsByHour.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObservationCountsByHour.
           */
         virtual uci::type::ObservationCountsByHourType& getObservationCountsByHour()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ObservationCountsByHour to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ObservationCountsByHour
           */
         virtual void setObservationCountsByHour(const uci::type::ObservationCountsByHourType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpsClockDayType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpsClockDayType to copy from
           */
         OpsClockDayType(const OpsClockDayType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpsClockDayType to the contents of the OpsClockDayType on the
           * right hand side (rhs) of the assignment operator.OpsClockDayType [only available to derived classes]
           *
           * @param rhs The OpsClockDayType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::OpsClockDayType
           * @return A constant reference to this OpsClockDayType.
           */
         const OpsClockDayType& operator=(const OpsClockDayType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpsClockDayType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpsClockDayType_h

