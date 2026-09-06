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
#ifndef Uci__Type__ResourceRequestPeriodType_h
#define Uci__Type__ResourceRequestPeriodType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__PercentRangeType_h)
# include "uci/type/PercentRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A description of the requested duration and duty cycle of an access period. */
      class ResourceRequestPeriodType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ResourceRequestPeriodType()
         { }

         /** Returns this accessor's type constant, i.e. ResourceRequestPeriodType
           *
           * @return This accessor's type constant, i.e. ResourceRequestPeriodType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::resourceRequestPeriodType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ResourceRequestPeriodType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AccessPeriodIndex.
           *
           * @return The value of the simple primitive data type identified by AccessPeriodIndex.
           */
         virtual xs::UnsignedInt getAccessPeriodIndex() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AccessPeriodIndex.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAccessPeriodIndex(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TypicalAccessPeriodDuration.
           *
           * @return The value of the simple primitive data type identified by TypicalAccessPeriodDuration.
           */
         virtual uci::type::DurationTypeValue getTypicalAccessPeriodDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TypicalAccessPeriodDuration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTypicalAccessPeriodDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TypicalAccessPeriodDuration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TypicalAccessPeriodDuration is emabled or not
           */
         virtual bool hasTypicalAccessPeriodDuration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TypicalAccessPeriodDuration
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTypicalAccessPeriodDuration(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TypicalAccessPeriodDuration */
         virtual void clearTypicalAccessPeriodDuration()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxAccessWidthRequested.
           *
           * @return The value of the simple primitive data type identified by MaxAccessWidthRequested.
           */
         virtual uci::type::DurationTypeValue getMaxAccessWidthRequested() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxAccessWidthRequested.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxAccessWidthRequested(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaxAccessWidthRequested exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaxAccessWidthRequested is emabled or not
           */
         virtual bool hasMaxAccessWidthRequested() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaxAccessWidthRequested
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableMaxAccessWidthRequested(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaxAccessWidthRequested */
         virtual void clearMaxAccessWidthRequested()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TypicalAccessRequestToReleaseDuration.
           *
           * @return The value of the simple primitive data type identified by TypicalAccessRequestToReleaseDuration.
           */
         virtual uci::type::DurationTypeValue getTypicalAccessRequestToReleaseDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TypicalAccessRequestToReleaseDuration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTypicalAccessRequestToReleaseDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TypicalAccessReleaseToRequestDuration.
           *
           * @return The value of the simple primitive data type identified by TypicalAccessReleaseToRequestDuration.
           */
         virtual uci::type::DurationTypeValue getTypicalAccessReleaseToRequestDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TypicalAccessReleaseToRequestDuration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTypicalAccessReleaseToRequestDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * PercentAccessDuringThePeriod.
           *
           * @return The acecssor that provides access to the complex content that is identified by PercentAccessDuringThePeriod.
           */
         virtual const uci::type::PercentRangeType& getPercentAccessDuringThePeriod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * PercentAccessDuringThePeriod.
           *
           * @return The acecssor that provides access to the complex content that is identified by PercentAccessDuringThePeriod.
           */
         virtual uci::type::PercentRangeType& getPercentAccessDuringThePeriod()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PercentAccessDuringThePeriod to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PercentAccessDuringThePeriod
           */
         virtual void setPercentAccessDuringThePeriod(const uci::type::PercentRangeType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ResourceRequestPeriodType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ResourceRequestPeriodType to copy from
           */
         ResourceRequestPeriodType(const ResourceRequestPeriodType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ResourceRequestPeriodType to the contents of the
           * ResourceRequestPeriodType on the right hand side (rhs) of the assignment operator.ResourceRequestPeriodType [only
           * available to derived classes]
           *
           * @param rhs The ResourceRequestPeriodType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ResourceRequestPeriodType
           * @return A constant reference to this ResourceRequestPeriodType.
           */
         const ResourceRequestPeriodType& operator=(const ResourceRequestPeriodType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ResourceRequestPeriodType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ResourceRequestPeriodType_h

