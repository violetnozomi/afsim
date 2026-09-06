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
#ifndef Uci__Type__SAR_ActivityType_h
#define Uci__Type__SAR_ActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SAR_SubCapabilityEnum_h)
# include "uci/type/SAR_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__RadarPointingTargetType_h)
# include "uci/type/RadarPointingTargetType.h"
#endif

#if !defined(Uci__Type__SAR_ActivityStatusDetailType_h)
# include "uci/type/SAR_ActivityStatusDetailType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SAR_PredictedCollectionsType_h)
# include "uci/type/SAR_PredictedCollectionsType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_ActivityType sequence accessor class */
      class SAR_ActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~SAR_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_ActivityType
           *
           * @return This accessor's type constant, i.e. SAR_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific SubCapability associated with the Activity. If omitted, all SubCapabilities of the associated
           * Capability are applicable for the Activity. This element is required whenever a subset of SubCapabilities of a
           * Capability are used by the Activity. [Minimum occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::SAR_SubCapabilityEnum, uci::type::accessorType::sAR_SubCapabilityEnum> SubCapability;

         /** Indicates actual performance metrics that provide the system insight regarding the progress of an activity,
           * description of the results of an activity and related stats. This is a list to provide reporting for all revisits
           * between activity messages. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SAR_ActivityStatusDetailType, uci::type::accessorType::sAR_ActivityStatusDetailType> Metrics;

         /** Supports interaction with a capability assessment function. This allows a subsystem or a model of a subsystem to
           * estimate future performance. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SAR_PredictedCollectionsType, uci::type::accessorType::sAR_PredictedCollectionsType> PredictedCollections;

         /** Indicates which electronic protection IDs are in use for this activity. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> ElectronicProtectionOptionsEmployed;

         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::SAR_ActivityType::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::SAR_ActivityType::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::SAR_ActivityType::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Attempt.
           *
           * @return The value of the simple primitive data type identified by Attempt.
           */
         virtual xs::UnsignedInt getAttempt() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Attempt.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAttempt(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Attempt exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Attempt is emabled or not
           */
         virtual bool hasAttempt() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Attempt
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableAttempt(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Attempt */
         virtual void clearAttempt()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RadarPointingTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by RadarPointingTarget.
           */
         virtual const uci::type::RadarPointingTargetType& getRadarPointingTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RadarPointingTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by RadarPointingTarget.
           */
         virtual uci::type::RadarPointingTargetType& getRadarPointingTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RadarPointingTarget to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RadarPointingTarget
           */
         virtual void setRadarPointingTarget(const uci::type::RadarPointingTargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Metrics.
           *
           * @return The bounded list identified by Metrics.
           */
         virtual const uci::type::SAR_ActivityType::Metrics& getMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Metrics.
           *
           * @return The bounded list identified by Metrics.
           */
         virtual uci::type::SAR_ActivityType::Metrics& getMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Metrics.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMetrics(const uci::type::SAR_ActivityType::Metrics& value)
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


         /** Returns the bounded list that is identified by the PredictedCollections.
           *
           * @return The bounded list identified by PredictedCollections.
           */
         virtual const uci::type::SAR_ActivityType::PredictedCollections& getPredictedCollections() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PredictedCollections.
           *
           * @return The bounded list identified by PredictedCollections.
           */
         virtual uci::type::SAR_ActivityType::PredictedCollections& getPredictedCollections()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PredictedCollections.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPredictedCollections(const uci::type::SAR_ActivityType::PredictedCollections& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEmployed.
           */
         virtual const uci::type::SAR_ActivityType::ElectronicProtectionOptionsEmployed& getElectronicProtectionOptionsEmployed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEmployed.
           */
         virtual uci::type::SAR_ActivityType::ElectronicProtectionOptionsEmployed& getElectronicProtectionOptionsEmployed()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptionsEmployed(const uci::type::SAR_ActivityType::ElectronicProtectionOptionsEmployed& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_ActivityType to copy from
           */
         SAR_ActivityType(const SAR_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_ActivityType to the contents of the SAR_ActivityType on the
           * right hand side (rhs) of the assignment operator.SAR_ActivityType [only available to derived classes]
           *
           * @param rhs The SAR_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SAR_ActivityType
           * @return A constant reference to this SAR_ActivityType.
           */
         const SAR_ActivityType& operator=(const SAR_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_ActivityType_h

