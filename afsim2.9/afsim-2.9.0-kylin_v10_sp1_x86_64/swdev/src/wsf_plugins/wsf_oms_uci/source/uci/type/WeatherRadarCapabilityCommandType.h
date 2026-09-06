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
#ifndef Uci__Type__WeatherRadarCapabilityCommandType_h
#define Uci__Type__WeatherRadarCapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__AirVolumeSensorReferencedType_h)
# include "uci/type/AirVolumeSensorReferencedType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandFrequencyType_h)
# include "uci/type/CapabilityCommandFrequencyType.h"
#endif

#if !defined(Uci__Type__CollectionPolicyEnum_h)
# include "uci/type/CollectionPolicyEnum.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

#if !defined(Uci__Type__EmconConstraintType_h)
# include "uci/type/EmconConstraintType.h"
#endif

#if !defined(Uci__Type__ElectronicProtectionOptionsEnableType_h)
# include "uci/type/ElectronicProtectionOptionsEnableType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherRadarCapabilityCommandType sequence accessor class */
      class WeatherRadarCapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~WeatherRadarCapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. WeatherRadarCapabilityCommandType
           *
           * @return This accessor's type constant, i.e. WeatherRadarCapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherRadarCapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherRadarCapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a required output Product of the Command. Only Products consistent with WeatherRadar and supported by the
           * commanded WeatherRadar Capability should be requested. If omitted, no output Product should be produced. See
           * ProductTypeEnum for further details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandBasicType, uci::type::accessorType::productOutputCommandBasicType> ProductOutput;

         /** Indicates the list of externally controllable EP options that should be applied to this command instead of the EP
           * options currently enabled for this Capability. If this list is a subset of the EP options that are available for this
           * capability, then no other EP options should be employed for this command. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ElectronicProtectionOptionsEnableType, uci::type::accessorType::electronicProtectionOptionsEnableType> ElectronicProtectionOptionsOverride;

         /** Returns the accessor that provides access to the complex content that is identified by the AirVolume.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirVolume.
           */
         virtual const uci::type::AirVolumeSensorReferencedType& getAirVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirVolume.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirVolume.
           */
         virtual uci::type::AirVolumeSensorReferencedType& getAirVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AirVolume to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AirVolume
           */
         virtual void setAirVolume(const uci::type::AirVolumeSensorReferencedType& value)
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


         /** Returns the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @return The value of the enumeration identified by CollectionPolicy.
           */
         virtual const uci::type::CollectionPolicyEnum& getCollectionPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @return The value of the enumeration identified by CollectionPolicy.
           */
         virtual uci::type::CollectionPolicyEnum& getCollectionPolicy()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCollectionPolicy(const uci::type::CollectionPolicyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCollectionPolicy(uci::type::CollectionPolicyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CollectionPolicyis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCollectionPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCollectionPolicy will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::collectionPolicyEnum This Accessor's accessor type
           */
         virtual void enableCollectionPolicy(uci::base::accessorType::AccessorType type = uci::type::accessorType::collectionPolicyEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCollectionPolicy()
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
         virtual const uci::type::WeatherRadarCapabilityCommandType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::WeatherRadarCapabilityCommandType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::WeatherRadarCapabilityCommandType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emcon.
           */
         virtual const uci::type::EmconConstraintType& getEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emcon.
           */
         virtual uci::type::EmconConstraintType& getEmcon()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Emcon to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Emcon
           */
         virtual void setEmcon(const uci::type::EmconConstraintType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Emcon exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Emcon is emabled or not
           */
         virtual bool hasEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Emcon
           *
           * @param type = uci::type::accessorType::emconConstraintType This Accessor's accessor type
           */
         virtual void enableEmcon(uci::base::accessorType::AccessorType type = uci::type::accessorType::emconConstraintType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Emcon */
         virtual void clearEmcon()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsOverride.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsOverride.
           */
         virtual const uci::type::WeatherRadarCapabilityCommandType::ElectronicProtectionOptionsOverride& getElectronicProtectionOptionsOverride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsOverride.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsOverride.
           */
         virtual uci::type::WeatherRadarCapabilityCommandType::ElectronicProtectionOptionsOverride& getElectronicProtectionOptionsOverride()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptionsOverride.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptionsOverride(const uci::type::WeatherRadarCapabilityCommandType::ElectronicProtectionOptionsOverride& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherRadarCapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherRadarCapabilityCommandType to copy from
           */
         WeatherRadarCapabilityCommandType(const WeatherRadarCapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherRadarCapabilityCommandType to the contents of the
           * WeatherRadarCapabilityCommandType on the right hand side (rhs) of the assignment
           * operator.WeatherRadarCapabilityCommandType [only available to derived classes]
           *
           * @param rhs The WeatherRadarCapabilityCommandType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::WeatherRadarCapabilityCommandType
           * @return A constant reference to this WeatherRadarCapabilityCommandType.
           */
         const WeatherRadarCapabilityCommandType& operator=(const WeatherRadarCapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherRadarCapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherRadarCapabilityCommandType_h

