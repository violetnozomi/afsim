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
#ifndef Uci__Type__SAR_ActivityCommandType_h
#define Uci__Type__SAR_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__SAR_CapabilityCommandSubCapabilityType_h)
# include "uci/type/SAR_CapabilityCommandSubCapabilityType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandTemporalConstraintsType_h)
# include "uci/type/CapabilityCommandTemporalConstraintsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CapabilityCommandFrequencyType_h)
# include "uci/type/CapabilityCommandFrequencyType.h"
#endif

#if !defined(Uci__Type__SAR_CollectionOptionsType_h)
# include "uci/type/SAR_CollectionOptionsType.h"
#endif

#if !defined(Uci__Type__SAR_CollectionConstraintsType_h)
# include "uci/type/SAR_CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__ECEF_UnitVectorType_h)
# include "uci/type/ECEF_UnitVectorType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AtmosphericPropagationLossType_h)
# include "uci/type/AtmosphericPropagationLossType.h"
#endif

#if !defined(Uci__Type__SAR_DesiredWaveformType_h)
# include "uci/type/SAR_DesiredWaveformType.h"
#endif

#if !defined(Uci__Type__PairIdentifierEnum_h)
# include "uci/type/PairIdentifierEnum.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandImageryType_h)
# include "uci/type/ProductOutputCommandImageryType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_ActivityCommandType sequence accessor class */
      class SAR_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~SAR_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. SAR_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Atmospheric propagation information to be used when determining CNR or range/power levels. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AtmosphericPropagationLossType, uci::type::accessorType::atmosphericPropagationLossType> AtmosphericPropagation;

         /** Indicates a required output Product of the Command. Only Products consistent with SAR and supported by the commanded
           * SAR Capability should be requested. If omitted, no output Product should be produced. See ProductTypeEnum for further
           * details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandImageryType, uci::type::accessorType::productOutputCommandImageryType> ProductOutput;

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


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapability.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapability.
           */
         virtual const uci::type::SAR_CapabilityCommandSubCapabilityType& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapability.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapability.
           */
         virtual uci::type::SAR_CapabilityCommandSubCapabilityType& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubCapability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubCapability
           */
         virtual void setSubCapability(const uci::type::SAR_CapabilityCommandSubCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubCapability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubCapability is emabled or not
           */
         virtual bool hasSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubCapability
           *
           * @param type = uci::type::accessorType::sAR_CapabilityCommandSubCapabilityType This Accessor's accessor type
           */
         virtual void enableSubCapability(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_CapabilityCommandSubCapabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubCapability */
         virtual void clearSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalConstraints.
           */
         virtual const uci::type::CapabilityCommandTemporalConstraintsType& getTemporalConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalConstraints.
           */
         virtual uci::type::CapabilityCommandTemporalConstraintsType& getTemporalConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TemporalConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TemporalConstraints
           */
         virtual void setTemporalConstraints(const uci::type::CapabilityCommandTemporalConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TemporalConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TemporalConstraints is emabled or not
           */
         virtual bool hasTemporalConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TemporalConstraints
           *
           * @param type = uci::type::accessorType::capabilityCommandTemporalConstraintsType This Accessor's accessor type
           */
         virtual void enableTemporalConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandTemporalConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TemporalConstraints */
         virtual void clearTemporalConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OverrideRejection.
           *
           * @return The value of the simple primitive data type identified by OverrideRejection.
           */
         virtual xs::Boolean getOverrideRejection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OverrideRejection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOverrideRejection(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OverrideRejection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OverrideRejection is emabled or not
           */
         virtual bool hasOverrideRejection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OverrideRejection
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableOverrideRejection(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OverrideRejection */
         virtual void clearOverrideRejection()
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
         virtual const uci::type::SAR_CollectionOptionsType& getCollectionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionOptions.
           */
         virtual uci::type::SAR_CollectionOptionsType& getCollectionOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionOptions
           */
         virtual void setCollectionOptions(const uci::type::SAR_CollectionOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionOptions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionOptions is emabled or not
           */
         virtual bool hasCollectionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionOptions
           *
           * @param type = uci::type::accessorType::sAR_CollectionOptionsType This Accessor's accessor type
           */
         virtual void enableCollectionOptions(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_CollectionOptionsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionOptions */
         virtual void clearCollectionOptions()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual const uci::type::SAR_CollectionConstraintsType& getCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual uci::type::SAR_CollectionConstraintsType& getCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraints
           */
         virtual void setCollectionConstraints(const uci::type::SAR_CollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraints is emabled or not
           */
         virtual bool hasCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraints
           *
           * @param type = uci::type::accessorType::sAR_CollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_CollectionConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraints */
         virtual void clearCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlaneNormalUnitVector.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlaneNormalUnitVector.
           */
         virtual const uci::type::ECEF_UnitVectorType& getPlaneNormalUnitVector() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlaneNormalUnitVector.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlaneNormalUnitVector.
           */
         virtual uci::type::ECEF_UnitVectorType& getPlaneNormalUnitVector()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlaneNormalUnitVector to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlaneNormalUnitVector
           */
         virtual void setPlaneNormalUnitVector(const uci::type::ECEF_UnitVectorType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlaneNormalUnitVector exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlaneNormalUnitVector is emabled or not
           */
         virtual bool hasPlaneNormalUnitVector() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlaneNormalUnitVector
           *
           * @param type = uci::type::accessorType::eCEF_UnitVectorType This Accessor's accessor type
           */
         virtual void enablePlaneNormalUnitVector(uci::base::accessorType::AccessorType type = uci::type::accessorType::eCEF_UnitVectorType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlaneNormalUnitVector */
         virtual void clearPlaneNormalUnitVector()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CoefficientOfTroposphericRefraction.
           *
           * @return The value of the simple primitive data type identified by CoefficientOfTroposphericRefraction.
           */
         virtual xs::Double getCoefficientOfTroposphericRefraction() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CoefficientOfTroposphericRefraction.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCoefficientOfTroposphericRefraction(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CoefficientOfTroposphericRefraction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CoefficientOfTroposphericRefraction is emabled or not
           */
         virtual bool hasCoefficientOfTroposphericRefraction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CoefficientOfTroposphericRefraction
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableCoefficientOfTroposphericRefraction(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CoefficientOfTroposphericRefraction */
         virtual void clearCoefficientOfTroposphericRefraction()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AtmosphericPropagation.
           *
           * @return The bounded list identified by AtmosphericPropagation.
           */
         virtual const uci::type::SAR_ActivityCommandType::AtmosphericPropagation& getAtmosphericPropagation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AtmosphericPropagation.
           *
           * @return The bounded list identified by AtmosphericPropagation.
           */
         virtual uci::type::SAR_ActivityCommandType::AtmosphericPropagation& getAtmosphericPropagation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AtmosphericPropagation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAtmosphericPropagation(const uci::type::SAR_ActivityCommandType::AtmosphericPropagation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DesiredWaveform.
           *
           * @return The acecssor that provides access to the complex content that is identified by DesiredWaveform.
           */
         virtual const uci::type::SAR_DesiredWaveformType& getDesiredWaveform() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DesiredWaveform.
           *
           * @return The acecssor that provides access to the complex content that is identified by DesiredWaveform.
           */
         virtual uci::type::SAR_DesiredWaveformType& getDesiredWaveform()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DesiredWaveform to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DesiredWaveform
           */
         virtual void setDesiredWaveform(const uci::type::SAR_DesiredWaveformType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DesiredWaveform exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DesiredWaveform is emabled or not
           */
         virtual bool hasDesiredWaveform() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DesiredWaveform
           *
           * @param type = uci::type::accessorType::sAR_DesiredWaveformType This Accessor's accessor type
           */
         virtual void enableDesiredWaveform(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_DesiredWaveformType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DesiredWaveform */
         virtual void clearDesiredWaveform()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PairIdentifier.
           *
           * @return The value of the enumeration identified by PairIdentifier.
           */
         virtual const uci::type::PairIdentifierEnum& getPairIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PairIdentifier.
           *
           * @return The value of the enumeration identified by PairIdentifier.
           */
         virtual uci::type::PairIdentifierEnum& getPairIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PairIdentifier.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPairIdentifier(const uci::type::PairIdentifierEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PairIdentifier.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPairIdentifier(uci::type::PairIdentifierEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PairIdentifieris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPairIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPairIdentifier will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::pairIdentifierEnum This Accessor's accessor type
           */
         virtual void enablePairIdentifier(uci::base::accessorType::AccessorType type = uci::type::accessorType::pairIdentifierEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPairIdentifier()
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
         virtual const uci::type::SAR_ActivityCommandType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::SAR_ActivityCommandType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::SAR_ActivityCommandType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_ActivityCommandType to copy from
           */
         SAR_ActivityCommandType(const SAR_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_ActivityCommandType to the contents of the
           * SAR_ActivityCommandType on the right hand side (rhs) of the assignment operator.SAR_ActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The SAR_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SAR_ActivityCommandType
           * @return A constant reference to this SAR_ActivityCommandType.
           */
         const SAR_ActivityCommandType& operator=(const SAR_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_ActivityCommandType_h

