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
#ifndef Uci__Type__ESM_ActivityType_h
#define Uci__Type__ESM_ActivityType_h 1

#if !defined(Uci__Type__SharedRF_ApertureActivityBaseType_h)
# include "uci/type/SharedRF_ApertureActivityBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ESM_SubCapabilityEnum_h)
# include "uci/type/ESM_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__SubCapabilityDetailsType_h)
# include "uci/type/SubCapabilityDetailsType.h"
#endif

#if !defined(Uci__Type__ESM_ReceiveDwellType_h)
# include "uci/type/ESM_ReceiveDwellType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ConsentEnum_h)
# include "uci/type/ConsentEnum.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_ActivityType sequence accessor class */
      class ESM_ActivityType : public virtual uci::type::SharedRF_ApertureActivityBaseType {
      public:

         /** The destructor */
         virtual ~ESM_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. ESM_ActivityType
           *
           * @return This accessor's type constant, i.e. ESM_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific SubCapability that is included in the Activity. If omitted, all SubCapabilities of the
           * associated Capability(ies) (as indicated in the ESM_Capability message) will be included in the Activity. This
           * element is required whenever a subset of SubCapabilities of the associated Capability(ies) are included in the
           * Activity. [Minimum occurrences: 0] [Maximum occurrences: 12]
           */
         typedef uci::base::BoundedList<uci::type::ESM_SubCapabilityEnum, uci::type::accessorType::eSM_SubCapabilityEnum> SubCapability;

         /** Indicates select SubCapability-specific details regarding the originating command/source of the Activity (as
           * indicated by the sibling ActivitySource element). In other words, this element is an echo of select elements from the
           * ESM_Command, MDF, etc. that originated the Activity. Additionally, in the case of ESM_Commands whose results are
           * independent of typical ESM output messages such as Entity or SignalReport, discrete command results are given here
           * instead of in those messages. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubCapabilityDetailsType, uci::type::accessorType::subCapabilityDetailsType> SubCapabilityDetails;

         /** Indicates an ESM receiver Dwell that is applicable to the Activity. This element is optional in support of ESM
           * Capabilities whose detailed dwell configuration and scheduling is internally isolated. Though optional here, dwell
           * information is required for successful, higher level ESM Capability management across heterogeneous families Systems,
           * Services and Subsystems. Dwell information is potentially useful even if the associated Activity and/or Capability
           * aren't manual and/or interactive. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ESM_ReceiveDwellType, uci::type::accessorType::eSM_ReceiveDwellType> ReceiveDwell;

         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::ESM_ActivityType::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::ESM_ActivityType::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::ESM_ActivityType::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityDetails.
           *
           * @return The bounded list identified by SubCapabilityDetails.
           */
         virtual const uci::type::ESM_ActivityType::SubCapabilityDetails& getSubCapabilityDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityDetails.
           *
           * @return The bounded list identified by SubCapabilityDetails.
           */
         virtual uci::type::ESM_ActivityType::SubCapabilityDetails& getSubCapabilityDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityDetails.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityDetails(const uci::type::ESM_ActivityType::SubCapabilityDetails& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveDwell.
           *
           * @return The bounded list identified by ReceiveDwell.
           */
         virtual const uci::type::ESM_ActivityType::ReceiveDwell& getReceiveDwell() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveDwell.
           *
           * @return The bounded list identified by ReceiveDwell.
           */
         virtual uci::type::ESM_ActivityType::ReceiveDwell& getReceiveDwell()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ReceiveDwell.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReceiveDwell(const uci::type::ESM_ActivityType::ReceiveDwell& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ConsentRequired.
           *
           * @return The value of the simple primitive data type identified by ConsentRequired.
           */
         virtual xs::Boolean getConsentRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConsentRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConsentRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConsentRequired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConsentRequired is emabled or not
           */
         virtual bool hasConsentRequired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConsentRequired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableConsentRequired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConsentRequired */
         virtual void clearConsentRequired()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual const uci::type::ConsentEnum& getConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual uci::type::ConsentEnum& getConsentState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(const uci::type::ConsentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(uci::type::ConsentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ConsentStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getConsentState will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::consentEnum This Accessor's accessor type
           */
         virtual void enableConsentState(uci::base::accessorType::AccessorType type = uci::type::accessorType::consentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConsentState()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @return The value of the simple primitive data type identified by ActualStartTime.
           */
         virtual uci::type::DateTimeTypeValue getActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualStartTime is emabled or not
           */
         virtual bool hasActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualStartTime */
         virtual void clearActualStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualEndTime.
           *
           * @return The value of the simple primitive data type identified by ActualEndTime.
           */
         virtual uci::type::DateTimeTypeValue getActualEndTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualEndTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualEndTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualEndTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualEndTime is emabled or not
           */
         virtual bool hasActualEndTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualEndTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualEndTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualEndTime */
         virtual void clearActualEndTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @return The value of the simple primitive data type identified by SpectralDensityReporting.
           */
         virtual xs::Boolean getSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpectralDensityReporting(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpectralDensityReporting exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpectralDensityReporting is emabled or not
           */
         virtual bool hasSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpectralDensityReporting
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSpectralDensityReporting(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpectralDensityReporting */
         virtual void clearSpectralDensityReporting()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_ActivityType to copy from
           */
         ESM_ActivityType(const ESM_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_ActivityType to the contents of the ESM_ActivityType on the
           * right hand side (rhs) of the assignment operator.ESM_ActivityType [only available to derived classes]
           *
           * @param rhs The ESM_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ESM_ActivityType
           * @return A constant reference to this ESM_ActivityType.
           */
         const ESM_ActivityType& operator=(const ESM_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_ActivityType_h

