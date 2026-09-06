// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AMTI_ActivityType_h
#define Uci__Type__AMTI_ActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AMTI_SubCapabilityEnum_h)
# include "uci/type/AMTI_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__AirTargetVolumeCommandType_h)
# include "uci/type/AirTargetVolumeCommandType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Type__MTI_ActivityStatusDetailType_h)
# include "uci/type/MTI_ActivityStatusDetailType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_ActivityType sequence accessor class */
      class AMTI_ActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~AMTI_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_ActivityType
           *
           * @return This accessor's type constant, i.e. AMTI_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific SubCapability associated with the Activity. If omitted, all SubCapabilities of the associated
           * Capability are applicable for the Activity. This element is required whenever a subset of SubCapabilities of a
           * Capability are used by the Activity. [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::AMTI_SubCapabilityEnum, uci::type::accessorType::aMTI_SubCapabilityEnum> SubCapability;

         /** Indicates the volume or specific Entity that defines the extents of the spatial region covered by the Activity. For
           * non-CUED AMTI Capabilities, one air volume instance of this element is expected. For CUED AMTI Capabilities, two
           * instances of this element are allowed: one with an Entity specified and a second with an air volume that defines the
           * extents of the cue volume around the Entity. For CUED AMTI Capabilities, a single instance is allowed if it is an
           * Entity in which case the Entity location uncertainty defines the extents of the cue volume. [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::AirTargetVolumeCommandType, uci::type::accessorType::airTargetVolumeCommandType> TargetVolume;

         /** Indicates performance metrics that provide the system insight regarding the progress of an activity, description of
           * the results of an activity and related stats. This is a list to provide reporting for all revisits between activity
           * messages. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MTI_ActivityStatusDetailType, uci::type::accessorType::mTI_ActivityStatusDetailType> Metrics;

         /** Indicates which electronic protection IDs are in use for this activity [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> ElectronicProtectionOptionsEmployed;

         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::AMTI_ActivityType::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::AMTI_ActivityType::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::AMTI_ActivityType::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TargetVolume.
           *
           * @return The bounded list identified by TargetVolume.
           */
         virtual const uci::type::AMTI_ActivityType::TargetVolume& getTargetVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TargetVolume.
           *
           * @return The bounded list identified by TargetVolume.
           */
         virtual uci::type::AMTI_ActivityType::TargetVolume& getTargetVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TargetVolume.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTargetVolume(const uci::type::AMTI_ActivityType::TargetVolume& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual const uci::type::CommandRepetitionType& getRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual uci::type::CommandRepetitionType& getRepetition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Repetition to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Repetition
           */
         virtual void setRepetition(const uci::type::CommandRepetitionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Repetition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Repetition is emabled or not
           */
         virtual bool hasRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Repetition
           *
           * @param type = uci::type::accessorType::commandRepetitionType This Accessor's accessor type
           */
         virtual void enableRepetition(uci::base::accessorType::AccessorType type = uci::type::accessorType::commandRepetitionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Repetition */
         virtual void clearRepetition()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Metrics.
           *
           * @return The bounded list identified by Metrics.
           */
         virtual const uci::type::AMTI_ActivityType::Metrics& getMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Metrics.
           *
           * @return The bounded list identified by Metrics.
           */
         virtual uci::type::AMTI_ActivityType::Metrics& getMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Metrics.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMetrics(const uci::type::AMTI_ActivityType::Metrics& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PolicyApplied.
           *
           * @return The value of the simple primitive data type identified by PolicyApplied.
           */
         virtual xs::Boolean getPolicyApplied() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PolicyApplied.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPolicyApplied(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PolicyApplied exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PolicyApplied is emabled or not
           */
         virtual bool hasPolicyApplied() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PolicyApplied
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePolicyApplied(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PolicyApplied */
         virtual void clearPolicyApplied()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AllProductsAndMessagesProduced.
           *
           * @return The value of the simple primitive data type identified by AllProductsAndMessagesProduced.
           */
         virtual xs::Boolean getAllProductsAndMessagesProduced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AllProductsAndMessagesProduced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAllProductsAndMessagesProduced(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual const uci::type::TraceabilityType& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual uci::type::TraceabilityType& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Traceability
           */
         virtual void setTraceability(const uci::type::TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Traceability is emabled or not
           */
         virtual bool hasTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Traceability
           *
           * @param type = uci::type::accessorType::traceabilityType This Accessor's accessor type
           */
         virtual void enableTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::traceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Traceability */
         virtual void clearTraceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEmployed.
           */
         virtual const uci::type::AMTI_ActivityType::ElectronicProtectionOptionsEmployed& getElectronicProtectionOptionsEmployed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEmployed.
           */
         virtual uci::type::AMTI_ActivityType::ElectronicProtectionOptionsEmployed& getElectronicProtectionOptionsEmployed()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptionsEmployed(const uci::type::AMTI_ActivityType::ElectronicProtectionOptionsEmployed& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_ActivityType to copy from
           */
         AMTI_ActivityType(const AMTI_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_ActivityType to the contents of the AMTI_ActivityType on the
           * right hand side (rhs) of the assignment operator.AMTI_ActivityType [only available to derived classes]
           *
           * @param rhs The AMTI_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AMTI_ActivityType
           * @return A constant reference to this AMTI_ActivityType.
           */
         const AMTI_ActivityType& operator=(const AMTI_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_ActivityType_h

