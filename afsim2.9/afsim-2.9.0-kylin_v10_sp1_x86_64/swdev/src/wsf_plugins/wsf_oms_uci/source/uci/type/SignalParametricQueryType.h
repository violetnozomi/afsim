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
#ifndef Uci__Type__SignalParametricQueryType_h
#define Uci__Type__SignalParametricQueryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__DurationRangeType_h)
# include "uci/type/DurationRangeType.h"
#endif

#if !defined(Uci__Type__PRI_TypeEnum_h)
# include "uci/type/PRI_TypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is intended to support a queries for messages that contain signal parametrics. */
      class SignalParametricQueryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalParametricQueryType()
         { }

         /** Returns this accessor's type constant, i.e. SignalParametricQueryType
           *
           * @return This accessor's type constant, i.e. SignalParametricQueryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalParametricQueryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalParametricQueryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The range of radio frequencies of a signal to query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> RadioFrequencyRange;

         /** The range of Pulse Repetition Intervals of a signal to query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DurationRangeType, uci::type::accessorType::durationRangeType> PulseRepetitionIntervalRange;

         /** The range of Pulse Width of a signal to query. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DurationRangeType, uci::type::accessorType::durationRangeType> PulseWidthRange;

         /** The Scan Period range of a signal to query. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DurationRangeType, uci::type::accessorType::durationRangeType> ScanPeriodRange;

         /** The PRI type enum of a signal to query. [Minimum occurrences: 0] [Maximum occurrences: 7] */
         typedef uci::base::BoundedList<uci::type::PRI_TypeEnum, uci::type::accessorType::pRI_TypeEnum> PulseRepetitionIntervalType;

         /** Returns the bounded list that is identified by the RadioFrequencyRange.
           *
           * @return The bounded list identified by RadioFrequencyRange.
           */
         virtual const uci::type::SignalParametricQueryType::RadioFrequencyRange& getRadioFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RadioFrequencyRange.
           *
           * @return The bounded list identified by RadioFrequencyRange.
           */
         virtual uci::type::SignalParametricQueryType::RadioFrequencyRange& getRadioFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RadioFrequencyRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRadioFrequencyRange(const uci::type::SignalParametricQueryType::RadioFrequencyRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseRepetitionIntervalRange.
           *
           * @return The bounded list identified by PulseRepetitionIntervalRange.
           */
         virtual const uci::type::SignalParametricQueryType::PulseRepetitionIntervalRange& getPulseRepetitionIntervalRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseRepetitionIntervalRange.
           *
           * @return The bounded list identified by PulseRepetitionIntervalRange.
           */
         virtual uci::type::SignalParametricQueryType::PulseRepetitionIntervalRange& getPulseRepetitionIntervalRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PulseRepetitionIntervalRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPulseRepetitionIntervalRange(const uci::type::SignalParametricQueryType::PulseRepetitionIntervalRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseWidthRange.
           *
           * @return The bounded list identified by PulseWidthRange.
           */
         virtual const uci::type::SignalParametricQueryType::PulseWidthRange& getPulseWidthRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseWidthRange.
           *
           * @return The bounded list identified by PulseWidthRange.
           */
         virtual uci::type::SignalParametricQueryType::PulseWidthRange& getPulseWidthRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PulseWidthRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPulseWidthRange(const uci::type::SignalParametricQueryType::PulseWidthRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanPeriodRange.
           *
           * @return The bounded list identified by ScanPeriodRange.
           */
         virtual const uci::type::SignalParametricQueryType::ScanPeriodRange& getScanPeriodRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanPeriodRange.
           *
           * @return The bounded list identified by ScanPeriodRange.
           */
         virtual uci::type::SignalParametricQueryType::ScanPeriodRange& getScanPeriodRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScanPeriodRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScanPeriodRange(const uci::type::SignalParametricQueryType::ScanPeriodRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseRepetitionIntervalType.
           *
           * @return The bounded list identified by PulseRepetitionIntervalType.
           */
         virtual const uci::type::SignalParametricQueryType::PulseRepetitionIntervalType& getPulseRepetitionIntervalType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseRepetitionIntervalType.
           *
           * @return The bounded list identified by PulseRepetitionIntervalType.
           */
         virtual uci::type::SignalParametricQueryType::PulseRepetitionIntervalType& getPulseRepetitionIntervalType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PulseRepetitionIntervalType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPulseRepetitionIntervalType(const uci::type::SignalParametricQueryType::PulseRepetitionIntervalType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalParametricQueryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalParametricQueryType to copy from
           */
         SignalParametricQueryType(const SignalParametricQueryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalParametricQueryType to the contents of the
           * SignalParametricQueryType on the right hand side (rhs) of the assignment operator.SignalParametricQueryType [only
           * available to derived classes]
           *
           * @param rhs The SignalParametricQueryType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SignalParametricQueryType
           * @return A constant reference to this SignalParametricQueryType.
           */
         const SignalParametricQueryType& operator=(const SignalParametricQueryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalParametricQueryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalParametricQueryType_h

