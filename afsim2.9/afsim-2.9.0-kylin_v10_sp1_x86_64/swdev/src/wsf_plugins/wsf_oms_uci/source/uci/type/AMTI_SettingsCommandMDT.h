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
#ifndef Uci__Type__AMTI_SettingsCommandMDT_h
#define Uci__Type__AMTI_SettingsCommandMDT_h 1

#if !defined(Uci__Type__CapabilitySettingsCommandBaseType_h)
# include "uci/type/CapabilitySettingsCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ElectronicProtectionOptionsDefaultType_h)
# include "uci/type/ElectronicProtectionOptionsDefaultType.h"
#endif

#if !defined(Uci__Type__EmissionBalanceEnum_h)
# include "uci/type/EmissionBalanceEnum.h"
#endif

#if !defined(Uci__Type__AMTI_MessageOutputsEnum_h)
# include "uci/type/AMTI_MessageOutputsEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EmconSettingType_h)
# include "uci/type/EmconSettingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_SettingsCommandMDT sequence accessor class */
      class AMTI_SettingsCommandMDT : public virtual uci::type::CapabilitySettingsCommandBaseType {
      public:

         /** The destructor */
         virtual ~AMTI_SettingsCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_SettingsCommandMDT
           *
           * @return This accessor's type constant, i.e. AMTI_SettingsCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_SettingsCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_SettingsCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates which advertised EP options should be applied to all Activities associated with this Capability. This list
           * will completely replace any previously commanded list of EP options. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ElectronicProtectionOptionsDefaultType, uci::type::accessorType::electronicProtectionOptionsDefaultType> ElectronicProtectionOptions;

         /** Specifies which messages are to be output by the Capability as the result of capability commands. See enumerated type
           * annotations for details. [Minimum occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::AMTI_MessageOutputsEnum, uci::type::accessorType::aMTI_MessageOutputsEnum> MessageOutput;

         /** Returns the bounded list that is identified by the ElectronicProtectionOptions.
           *
           * @return The bounded list identified by ElectronicProtectionOptions.
           */
         virtual const uci::type::AMTI_SettingsCommandMDT::ElectronicProtectionOptions& getElectronicProtectionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptions.
           *
           * @return The bounded list identified by ElectronicProtectionOptions.
           */
         virtual uci::type::AMTI_SettingsCommandMDT::ElectronicProtectionOptions& getElectronicProtectionOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptions(const uci::type::AMTI_SettingsCommandMDT::ElectronicProtectionOptions& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EmissionBalanceState.
           *
           * @return The value of the enumeration identified by EmissionBalanceState.
           */
         virtual const uci::type::EmissionBalanceEnum& getEmissionBalanceState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EmissionBalanceState.
           *
           * @return The value of the enumeration identified by EmissionBalanceState.
           */
         virtual uci::type::EmissionBalanceEnum& getEmissionBalanceState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EmissionBalanceState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEmissionBalanceState(const uci::type::EmissionBalanceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EmissionBalanceState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEmissionBalanceState(uci::type::EmissionBalanceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EmissionBalanceStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEmissionBalanceState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEmissionBalanceState will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::emissionBalanceEnum This Accessor's accessor type
           */
         virtual void enableEmissionBalanceState(uci::base::accessorType::AccessorType type = uci::type::accessorType::emissionBalanceEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEmissionBalanceState()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual const uci::type::AMTI_SettingsCommandMDT::MessageOutput& getMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual uci::type::AMTI_SettingsCommandMDT::MessageOutput& getMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageOutput(const uci::type::AMTI_SettingsCommandMDT::MessageOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UnassignAll.
           *
           * @return The value of the simple primitive data type identified by UnassignAll.
           */
         virtual xs::Boolean getUnassignAll() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UnassignAll.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUnassignAll(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UnassignAll exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UnassignAll is emabled or not
           */
         virtual bool hasUnassignAll() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UnassignAll
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableUnassignAll(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UnassignAll */
         virtual void clearUnassignAll()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultEmcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultEmcon.
           */
         virtual const uci::type::EmconSettingType& getDefaultEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultEmcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultEmcon.
           */
         virtual uci::type::EmconSettingType& getDefaultEmcon()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultEmcon to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultEmcon
           */
         virtual void setDefaultEmcon(const uci::type::EmconSettingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultEmcon exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultEmcon is emabled or not
           */
         virtual bool hasDefaultEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultEmcon
           *
           * @param type = uci::type::accessorType::emconSettingType This Accessor's accessor type
           */
         virtual void enableDefaultEmcon(uci::base::accessorType::AccessorType type = uci::type::accessorType::emconSettingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultEmcon */
         virtual void clearDefaultEmcon()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_SettingsCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_SettingsCommandMDT to copy from
           */
         AMTI_SettingsCommandMDT(const AMTI_SettingsCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_SettingsCommandMDT to the contents of the
           * AMTI_SettingsCommandMDT on the right hand side (rhs) of the assignment operator.AMTI_SettingsCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The AMTI_SettingsCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::AMTI_SettingsCommandMDT
           * @return A constant reference to this AMTI_SettingsCommandMDT.
           */
         const AMTI_SettingsCommandMDT& operator=(const AMTI_SettingsCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_SettingsCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_SettingsCommandMDT_h

