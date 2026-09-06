// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__OperationalScheduleType_h
#define Uci__Type__OperationalScheduleType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OpsClockDayType_h)
# include "uci/type/OpsClockDayType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** An operational schedule for an EOB record. The OperationalScheduleTimeSpan indicates the data range that was used to
        * generate the Operational Schedule. The HitCountByHour indicates the number of observed collects during the indicated
        * hour.
        */
      class OperationalScheduleType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperationalScheduleType()
         { }

         /** Returns this accessor's type constant, i.e. OperationalScheduleType
           *
           * @return This accessor's type constant, i.e. OperationalScheduleType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operationalScheduleType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperationalScheduleType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The consolidated operation schedule or Ops Clock. This is a sequence per 7 days of the week with the 0-23 observed
           * counts of an entity and what hour that count was observed [Minimum occurrences: 0] [Maximum occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::OpsClockDayType, uci::type::accessorType::opsClockDayType> OpsClock;

         /** Returns the accessor that provides access to the complex content that is identified by the ObservationPeriod.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObservationPeriod.
           */
         virtual const uci::type::DateTimeRangeType& getObservationPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ObservationPeriod.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObservationPeriod.
           */
         virtual uci::type::DateTimeRangeType& getObservationPeriod()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ObservationPeriod to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ObservationPeriod
           */
         virtual void setObservationPeriod(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OpsClock.
           *
           * @return The bounded list identified by OpsClock.
           */
         virtual const uci::type::OperationalScheduleType::OpsClock& getOpsClock() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OpsClock.
           *
           * @return The bounded list identified by OpsClock.
           */
         virtual uci::type::OperationalScheduleType::OpsClock& getOpsClock()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OpsClock.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOpsClock(const uci::type::OperationalScheduleType::OpsClock& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperationalScheduleType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperationalScheduleType to copy from
           */
         OperationalScheduleType(const OperationalScheduleType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperationalScheduleType to the contents of the
           * OperationalScheduleType on the right hand side (rhs) of the assignment operator.OperationalScheduleType [only
           * available to derived classes]
           *
           * @param rhs The OperationalScheduleType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OperationalScheduleType
           * @return A constant reference to this OperationalScheduleType.
           */
         const OperationalScheduleType& operator=(const OperationalScheduleType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperationalScheduleType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperationalScheduleType_h

