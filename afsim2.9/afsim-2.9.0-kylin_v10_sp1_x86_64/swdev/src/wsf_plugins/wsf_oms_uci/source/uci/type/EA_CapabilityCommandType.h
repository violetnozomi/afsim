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
#ifndef Uci__Type__EA_CapabilityCommandType_h
#define Uci__Type__EA_CapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__EA_EmissionType_h)
# include "uci/type/EA_EmissionType.h"
#endif

#if !defined(Uci__Type__EA_ActivationEnum_h)
# include "uci/type/EA_ActivationEnum.h"
#endif

#if !defined(Uci__Type__CapabilityCommandFrequencyType_h)
# include "uci/type/CapabilityCommandFrequencyType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProtectedAssetReturnPowerType_h)
# include "uci/type/ProtectedAssetReturnPowerType.h"
#endif

#if !defined(Uci__Type__ResourceSelectionType_h)
# include "uci/type/ResourceSelectionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_CapabilityCommandType sequence accessor class */
      class EA_CapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~EA_CapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. EA_CapabilityCommandType
           *
           * @return This accessor's type constant, i.e. EA_CapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_CapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_CapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The ProtectedAsset child element indicates which of the protected assets of the activity are the subject of the child
           * attribute ChangeProtectedAssetPredictedReturnPower. (In order to add or remove protected entities, a new
           * CapabilityCommandType must be utilized.) [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProtectedAssetReturnPowerType, uci::type::accessorType::protectedAssetReturnPowerType> ProtectedEntityPredictedReturnPower;

         /** Indicates the unique ID of a Shared Antenna and/or AntennaResources to be used for the resulting Activity. This
           * allows Mission Processing to direct an specific antenna, antenna resource type, or antenna resource instance to be
           * used by the resulting Activity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ResourceSelectionType, uci::type::accessorType::resourceSelectionType> ResourceSelection;

         /** Returns the accessor that provides access to the complex content that is identified by the Emission.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emission.
           */
         virtual const uci::type::EA_EmissionType& getEmission() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emission.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emission.
           */
         virtual uci::type::EA_EmissionType& getEmission()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Emission to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Emission
           */
         virtual void setEmission(const uci::type::EA_EmissionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Emission exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Emission is emabled or not
           */
         virtual bool hasEmission() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Emission
           *
           * @param type = uci::type::accessorType::eA_EmissionType This Accessor's accessor type
           */
         virtual void enableEmission(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_EmissionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Emission */
         virtual void clearEmission()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Activation.
           *
           * @return The value of the enumeration identified by Activation.
           */
         virtual const uci::type::EA_ActivationEnum& getActivation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Activation.
           *
           * @return The value of the enumeration identified by Activation.
           */
         virtual uci::type::EA_ActivationEnum& getActivation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Activation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActivation(const uci::type::EA_ActivationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Activation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActivation(uci::type::EA_ActivationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Activationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasActivation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getActivation will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::eA_ActivationEnum This Accessor's accessor type
           */
         virtual void enableActivation(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_ActivationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearActivation()
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


         /** Returns the bounded list that is identified by the ProtectedEntityPredictedReturnPower.
           *
           * @return The bounded list identified by ProtectedEntityPredictedReturnPower.
           */
         virtual const uci::type::EA_CapabilityCommandType::ProtectedEntityPredictedReturnPower& getProtectedEntityPredictedReturnPower() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProtectedEntityPredictedReturnPower.
           *
           * @return The bounded list identified by ProtectedEntityPredictedReturnPower.
           */
         virtual uci::type::EA_CapabilityCommandType::ProtectedEntityPredictedReturnPower& getProtectedEntityPredictedReturnPower()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProtectedEntityPredictedReturnPower.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProtectedEntityPredictedReturnPower(const uci::type::EA_CapabilityCommandType::ProtectedEntityPredictedReturnPower& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceSelection.
           *
           * @return The bounded list identified by ResourceSelection.
           */
         virtual const uci::type::EA_CapabilityCommandType::ResourceSelection& getResourceSelection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceSelection.
           *
           * @return The bounded list identified by ResourceSelection.
           */
         virtual uci::type::EA_CapabilityCommandType::ResourceSelection& getResourceSelection()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceSelection.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceSelection(const uci::type::EA_CapabilityCommandType::ResourceSelection& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_CapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_CapabilityCommandType to copy from
           */
         EA_CapabilityCommandType(const EA_CapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_CapabilityCommandType to the contents of the
           * EA_CapabilityCommandType on the right hand side (rhs) of the assignment operator.EA_CapabilityCommandType [only
           * available to derived classes]
           *
           * @param rhs The EA_CapabilityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EA_CapabilityCommandType
           * @return A constant reference to this EA_CapabilityCommandType.
           */
         const EA_CapabilityCommandType& operator=(const EA_CapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_CapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_CapabilityCommandType_h

