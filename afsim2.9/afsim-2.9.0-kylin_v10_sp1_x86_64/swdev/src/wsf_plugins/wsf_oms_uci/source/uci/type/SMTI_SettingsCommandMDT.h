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
#ifndef Uci__Type__SMTI_SettingsCommandMDT_h
#define Uci__Type__SMTI_SettingsCommandMDT_h 1

#if !defined(Uci__Type__CapabilitySettingsCommandBaseType_h)
# include "uci/type/CapabilitySettingsCommandBaseType.h"
#endif

#if !defined(Uci__Type__EmissionBalanceEnum_h)
# include "uci/type/EmissionBalanceEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SMTI_MessageOutputsEnum_h)
# include "uci/type/SMTI_MessageOutputsEnum.h"
#endif

#if !defined(Uci__Type__STANAG_4607_PackingPlanType_h)
# include "uci/type/STANAG_4607_PackingPlanType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EmconSettingType_h)
# include "uci/type/EmconSettingType.h"
#endif

#if !defined(Uci__Type__ElectronicProtectionOptionsDefaultType_h)
# include "uci/type/ElectronicProtectionOptionsDefaultType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SMTI_SettingsCommandMDT sequence accessor class */
      class SMTI_SettingsCommandMDT : public virtual uci::type::CapabilitySettingsCommandBaseType {
      public:

         /** The destructor */
         virtual ~SMTI_SettingsCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. SMTI_SettingsCommandMDT
           *
           * @return This accessor's type constant, i.e. SMTI_SettingsCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sMTI_SettingsCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SMTI_SettingsCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates messages that are an output by the Capability as the result of capability commands. See enumerated type
           * annotations for details. [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::SMTI_MessageOutputsEnum, uci::type::accessorType::sMTI_MessageOutputsEnum> MessageOutput;

         /** Defines a list of per capability default electronic protection options. These default options will be used if no
           * electronic protection options are provided in the capability command. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ElectronicProtectionOptionsDefaultType, uci::type::accessorType::electronicProtectionOptionsDefaultType> DefaultElectronicProtectionOptions;

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
         virtual const uci::type::SMTI_SettingsCommandMDT::MessageOutput& getMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual uci::type::SMTI_SettingsCommandMDT::MessageOutput& getMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageOutput(const uci::type::SMTI_SettingsCommandMDT::MessageOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultPackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultPackingPlan.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType& getDefaultPackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultPackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultPackingPlan.
           */
         virtual uci::type::STANAG_4607_PackingPlanType& getDefaultPackingPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultPackingPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultPackingPlan
           */
         virtual void setDefaultPackingPlan(const uci::type::STANAG_4607_PackingPlanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultPackingPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultPackingPlan is emabled or not
           */
         virtual bool hasDefaultPackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultPackingPlan
           *
           * @param type = uci::type::accessorType::sTANAG_4607_PackingPlanType This Accessor's accessor type
           */
         virtual void enableDefaultPackingPlan(uci::base::accessorType::AccessorType type = uci::type::accessorType::sTANAG_4607_PackingPlanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultPackingPlan */
         virtual void clearDefaultPackingPlan()
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


         /** Returns the bounded list that is identified by the DefaultElectronicProtectionOptions.
           *
           * @return The bounded list identified by DefaultElectronicProtectionOptions.
           */
         virtual const uci::type::SMTI_SettingsCommandMDT::DefaultElectronicProtectionOptions& getDefaultElectronicProtectionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DefaultElectronicProtectionOptions.
           *
           * @return The bounded list identified by DefaultElectronicProtectionOptions.
           */
         virtual uci::type::SMTI_SettingsCommandMDT::DefaultElectronicProtectionOptions& getDefaultElectronicProtectionOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DefaultElectronicProtectionOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDefaultElectronicProtectionOptions(const uci::type::SMTI_SettingsCommandMDT::DefaultElectronicProtectionOptions& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SMTI_SettingsCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SMTI_SettingsCommandMDT to copy from
           */
         SMTI_SettingsCommandMDT(const SMTI_SettingsCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SMTI_SettingsCommandMDT to the contents of the
           * SMTI_SettingsCommandMDT on the right hand side (rhs) of the assignment operator.SMTI_SettingsCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The SMTI_SettingsCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SMTI_SettingsCommandMDT
           * @return A constant reference to this SMTI_SettingsCommandMDT.
           */
         const SMTI_SettingsCommandMDT& operator=(const SMTI_SettingsCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SMTI_SettingsCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SMTI_SettingsCommandMDT_h

