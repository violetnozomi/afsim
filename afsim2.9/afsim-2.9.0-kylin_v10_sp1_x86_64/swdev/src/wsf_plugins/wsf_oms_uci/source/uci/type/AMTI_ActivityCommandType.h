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
#ifndef Uci__Type__AMTI_ActivityCommandType_h
#define Uci__Type__AMTI_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AirTargetVolumeCommandType_h)
# include "uci/type/AirTargetVolumeCommandType.h"
#endif

#if !defined(Uci__Type__AirTargetVolumeType_h)
# include "uci/type/AirTargetVolumeType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandFrequencyType_h)
# include "uci/type/CapabilityCommandFrequencyType.h"
#endif

#if !defined(Uci__Type__RadarCollectionOptionsType_h)
# include "uci/type/RadarCollectionOptionsType.h"
#endif

#if !defined(Uci__Type__AMTI_CollectionConstraintsType_h)
# include "uci/type/AMTI_CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_ActivityCommandType sequence accessor class */
      class AMTI_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~AMTI_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. AMTI_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the volume or specific Entity that defines the extents of the spatial region covered by the command. For
           * non-CUED AMTI Capabilities, one air volume instance of this element is expected. For CUED AMTI Capabilities, two
           * instances of this element are allowed: one with an Entity specified and a second with an air volume that define the
           * extents of the cue volume around the Entity. For CUED AMTI Capabilities, a single instance is allowed if it is an
           * Entity in which case the Entity location uncertainty defines the extents of the cue volume. [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::AirTargetVolumeCommandType, uci::type::accessorType::airTargetVolumeCommandType> TargetVolume;

         /** Indicates an air volume that should be "blanked"; the Capability should not generate outputs associated with the
           * volume because it is a known source of clutter or other interference for the Capability. For example, a blanking
           * volume could be created around a wind farm that interferes with AMTI. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AirTargetVolumeType, uci::type::accessorType::airTargetVolumeType> BlankingVolume;

         /** Indicates a required output Product of the Command. Only Products consistent with AMTI and supported by the commanded
           * AMTI Capability should be requested. If omitted, no output Product should be produced. See ProductTypeEnum for
           * further details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandBasicType, uci::type::accessorType::productOutputCommandBasicType> ProductOutput;

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


         /** Returns the bounded list that is identified by the TargetVolume.
           *
           * @return The bounded list identified by TargetVolume.
           */
         virtual const uci::type::AMTI_ActivityCommandType::TargetVolume& getTargetVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TargetVolume.
           *
           * @return The bounded list identified by TargetVolume.
           */
         virtual uci::type::AMTI_ActivityCommandType::TargetVolume& getTargetVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TargetVolume.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTargetVolume(const uci::type::AMTI_ActivityCommandType::TargetVolume& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BlankingVolume.
           *
           * @return The bounded list identified by BlankingVolume.
           */
         virtual const uci::type::AMTI_ActivityCommandType::BlankingVolume& getBlankingVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BlankingVolume.
           *
           * @return The bounded list identified by BlankingVolume.
           */
         virtual uci::type::AMTI_ActivityCommandType::BlankingVolume& getBlankingVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BlankingVolume.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBlankingVolume(const uci::type::AMTI_ActivityCommandType::BlankingVolume& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual const uci::type::CapabilityCommandFrequencyType& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual uci::type::CapabilityCommandFrequencyType& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Frequency to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Frequency
           */
         virtual void setFrequency(const uci::type::CapabilityCommandFrequencyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Frequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Frequency is emabled or not
           */
         virtual bool hasFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Frequency
           *
           * @param type = uci::type::accessorType::capabilityCommandFrequencyType This Accessor's accessor type
           */
         virtual void enableFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandFrequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Frequency */
         virtual void clearFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionOptions.
           */
         virtual const uci::type::RadarCollectionOptionsType& getCollectionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionOptions.
           */
         virtual uci::type::RadarCollectionOptionsType& getCollectionOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionOptions
           */
         virtual void setCollectionOptions(const uci::type::RadarCollectionOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual const uci::type::AMTI_CollectionConstraintsType& getCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual uci::type::AMTI_CollectionConstraintsType& getCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraints
           */
         virtual void setCollectionConstraints(const uci::type::AMTI_CollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraints is emabled or not
           */
         virtual bool hasCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraints
           *
           * @param type = uci::type::accessorType::aMTI_CollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::aMTI_CollectionConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraints */
         virtual void clearCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TrackingRange.
           *
           * @return The value of the simple primitive data type identified by TrackingRange.
           */
         virtual uci::type::DistanceTypeValue getTrackingRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TrackingRange.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrackingRange(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackingRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackingRange is emabled or not
           */
         virtual bool hasTrackingRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackingRange
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableTrackingRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackingRange */
         virtual void clearTrackingRange()
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


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::AMTI_ActivityCommandType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::AMTI_ActivityCommandType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::AMTI_ActivityCommandType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_ActivityCommandType to copy from
           */
         AMTI_ActivityCommandType(const AMTI_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_ActivityCommandType to the contents of the
           * AMTI_ActivityCommandType on the right hand side (rhs) of the assignment operator.AMTI_ActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The AMTI_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::AMTI_ActivityCommandType
           * @return A constant reference to this AMTI_ActivityCommandType.
           */
         const AMTI_ActivityCommandType& operator=(const AMTI_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_ActivityCommandType_h

