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
#ifndef Uci__Type__ThreadConfigurationType_h
#define Uci__Type__ThreadConfigurationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__AntennaResourceInstanceIdentifierType_h)
# include "uci/type/AntennaResourceInstanceIdentifierType.h"
#endif

#if !defined(Uci__Type__RF_TypeEnum_h)
# include "uci/type/RF_TypeEnum.h"
#endif

#if !defined(Uci__Type__BeamPowerModeEnum_h)
# include "uci/type/BeamPowerModeEnum.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__BeamPointingReferenceType_h)
# include "uci/type/BeamPointingReferenceType.h"
#endif

#if !defined(Uci__Type__PolarizationType_h)
# include "uci/type/PolarizationType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__BeamShapingType_h)
# include "uci/type/BeamShapingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreadConfigurationType sequence accessor class */
      class ThreadConfigurationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreadConfigurationType()
         { }

         /** Returns this accessor's type constant, i.e. ThreadConfigurationType
           *
           * @return This accessor's type constant, i.e. ThreadConfigurationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threadConfigurationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreadConfigurationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BeamConfigurationIndex.
           *
           * @return The value of the simple primitive data type identified by BeamConfigurationIndex.
           */
         virtual xs::UnsignedInt getBeamConfigurationIndex() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BeamConfigurationIndex.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeamConfigurationIndex(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaID.
           */
         virtual const uci::type::SupportCapabilityID_Type& getAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaID.
           */
         virtual uci::type::SupportCapabilityID_Type& getAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaID
           */
         virtual void setAntennaID(const uci::type::SupportCapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AntennaResourceInstanceIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaResourceInstanceIndex.
           */
         virtual const uci::type::AntennaResourceInstanceIdentifierType& getAntennaResourceInstanceIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AntennaResourceInstanceIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaResourceInstanceIndex.
           */
         virtual uci::type::AntennaResourceInstanceIdentifierType& getAntennaResourceInstanceIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaResourceInstanceIndex to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaResourceInstanceIndex
           */
         virtual void setAntennaResourceInstanceIndex(const uci::type::AntennaResourceInstanceIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RF_Mode.
           *
           * @return The value of the enumeration identified by RF_Mode.
           */
         virtual const uci::type::RF_TypeEnum& getRF_Mode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RF_Mode.
           *
           * @return The value of the enumeration identified by RF_Mode.
           */
         virtual uci::type::RF_TypeEnum& getRF_Mode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RF_Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRF_Mode(const uci::type::RF_TypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RF_Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRF_Mode(uci::type::RF_TypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the BeamPowerMode.
           *
           * @return The value of the enumeration identified by BeamPowerMode.
           */
         virtual const uci::type::BeamPowerModeEnum& getBeamPowerMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the BeamPowerMode.
           *
           * @return The value of the enumeration identified by BeamPowerMode.
           */
         virtual uci::type::BeamPowerModeEnum& getBeamPowerMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the BeamPowerMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBeamPowerMode(const uci::type::BeamPowerModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the BeamPowerMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBeamPowerMode(uci::type::BeamPowerModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield BeamPowerModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasBeamPowerMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getBeamPowerMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::beamPowerModeEnum This Accessor's accessor type
           */
         virtual void enableBeamPowerMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::beamPowerModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearBeamPowerMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CenterFrequency.
           *
           * @return The value of the simple primitive data type identified by CenterFrequency.
           */
         virtual uci::type::FrequencyTypeValue getCenterFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CenterFrequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCenterFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CenterFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CenterFrequency is emabled or not
           */
         virtual bool hasCenterFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CenterFrequency
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableCenterFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CenterFrequency */
         virtual void clearCenterFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamPointingReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamPointingReference.
           */
         virtual const uci::type::BeamPointingReferenceType& getBeamPointingReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamPointingReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamPointingReference.
           */
         virtual uci::type::BeamPointingReferenceType& getBeamPointingReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeamPointingReference to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeamPointingReference
           */
         virtual void setBeamPointingReference(const uci::type::BeamPointingReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BeamPointingReference exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BeamPointingReference is emabled or not
           */
         virtual bool hasBeamPointingReference() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BeamPointingReference
           *
           * @param type = uci::type::accessorType::beamPointingReferenceType This Accessor's accessor type
           */
         virtual void enableBeamPointingReference(uci::base::accessorType::AccessorType type = uci::type::accessorType::beamPointingReferenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BeamPointingReference */
         virtual void clearBeamPointingReference()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Polarization.
           *
           * @return The acecssor that provides access to the complex content that is identified by Polarization.
           */
         virtual const uci::type::PolarizationType& getPolarization() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Polarization.
           *
           * @return The acecssor that provides access to the complex content that is identified by Polarization.
           */
         virtual uci::type::PolarizationType& getPolarization()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Polarization to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Polarization
           */
         virtual void setPolarization(const uci::type::PolarizationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Polarization exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Polarization is emabled or not
           */
         virtual bool hasPolarization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Polarization
           *
           * @param type = uci::type::accessorType::polarizationType This Accessor's accessor type
           */
         virtual void enablePolarization(uci::base::accessorType::AccessorType type = uci::type::accessorType::polarizationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Polarization */
         virtual void clearPolarization()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AntennaAttenuation.
           *
           * @return The value of the simple primitive data type identified by AntennaAttenuation.
           */
         virtual uci::type::DecibelTypeValue getAntennaAttenuation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AntennaAttenuation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAntennaAttenuation(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaAttenuation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaAttenuation is emabled or not
           */
         virtual bool hasAntennaAttenuation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaAttenuation
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableAntennaAttenuation(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaAttenuation */
         virtual void clearAntennaAttenuation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AntennaPriority.
           *
           * @return The value of the simple primitive data type identified by AntennaPriority.
           */
         virtual xs::UnsignedInt getAntennaPriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AntennaPriority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAntennaPriority(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaPriority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaPriority is emabled or not
           */
         virtual bool hasAntennaPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaPriority
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableAntennaPriority(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaPriority */
         virtual void clearAntennaPriority()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamShaping.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamShaping.
           */
         virtual const uci::type::BeamShapingType& getBeamShaping() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamShaping.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamShaping.
           */
         virtual uci::type::BeamShapingType& getBeamShaping()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeamShaping to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeamShaping
           */
         virtual void setBeamShaping(const uci::type::BeamShapingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BeamShaping exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BeamShaping is emabled or not
           */
         virtual bool hasBeamShaping() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BeamShaping
           *
           * @param type = uci::type::accessorType::beamShapingType This Accessor's accessor type
           */
         virtual void enableBeamShaping(uci::base::accessorType::AccessorType type = uci::type::accessorType::beamShapingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BeamShaping */
         virtual void clearBeamShaping()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreadConfigurationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreadConfigurationType to copy from
           */
         ThreadConfigurationType(const ThreadConfigurationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreadConfigurationType to the contents of the
           * ThreadConfigurationType on the right hand side (rhs) of the assignment operator.ThreadConfigurationType [only
           * available to derived classes]
           *
           * @param rhs The ThreadConfigurationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ThreadConfigurationType
           * @return A constant reference to this ThreadConfigurationType.
           */
         const ThreadConfigurationType& operator=(const ThreadConfigurationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreadConfigurationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreadConfigurationType_h

