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
#ifndef Uci__Type__TimeWindowType_h
#define Uci__Type__TimeWindowType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TimeWindowType sequence accessor class */
      class TimeWindowType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TimeWindowType()
         { }

         /** Returns this accessor's type constant, i.e. TimeWindowType
           *
           * @return This accessor's type constant, i.e. TimeWindowType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::timeWindowType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TimeWindowType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Range.
           *
           * @return The acecssor that provides access to the complex content that is identified by Range.
           */
         virtual const uci::type::DateTimeRangeType& getRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Range.
           *
           * @return The acecssor that provides access to the complex content that is identified by Range.
           */
         virtual uci::type::DateTimeRangeType& getRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Range to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Range
           */
         virtual void setRange(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Range exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Range is emabled or not
           */
         virtual bool hasRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Range
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Range */
         virtual void clearRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Duration.
           *
           * @return The value of the simple primitive data type identified by Duration.
           */
         virtual uci::type::DurationTypeValue getDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Duration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Duration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Duration is emabled or not
           */
         virtual bool hasDuration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Duration
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableDuration(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Duration */
         virtual void clearDuration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TimeWindowType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TimeWindowType to copy from
           */
         TimeWindowType(const TimeWindowType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TimeWindowType to the contents of the TimeWindowType on the right
           * hand side (rhs) of the assignment operator.TimeWindowType [only available to derived classes]
           *
           * @param rhs The TimeWindowType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::TimeWindowType
           * @return A constant reference to this TimeWindowType.
           */
         const TimeWindowType& operator=(const TimeWindowType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TimeWindowType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TimeWindowType_h

