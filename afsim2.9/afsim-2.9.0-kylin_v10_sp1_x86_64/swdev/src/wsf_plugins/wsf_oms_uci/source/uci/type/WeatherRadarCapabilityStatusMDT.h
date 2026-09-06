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
#ifndef Uci__Type__WeatherRadarCapabilityStatusMDT_h
#define Uci__Type__WeatherRadarCapabilityStatusMDT_h 1

#if !defined(Uci__Type__CapabilityStatusBaseType_h)
# include "uci/type/CapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityStatusType_h)
# include "uci/type/CapabilityStatusType.h"
#endif

#if !defined(Uci__Type__RF_ProfileType_h)
# include "uci/type/RF_ProfileType.h"
#endif

#if !defined(Uci__Type__EmissionBalanceEnum_h)
# include "uci/type/EmissionBalanceEnum.h"
#endif

#if !defined(Uci__Type__WeatherRadarMessageOutputsEnum_h)
# include "uci/type/WeatherRadarMessageOutputsEnum.h"
#endif

#if !defined(Uci__Type__ElectronicProtectionOptionsEnabledType_h)
# include "uci/type/ElectronicProtectionOptionsEnabledType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherRadarCapabilityStatusMDT sequence accessor class */
      class WeatherRadarCapabilityStatusMDT : public virtual uci::type::CapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~WeatherRadarCapabilityStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. WeatherRadarCapabilityStatusMDT
           *
           * @return This accessor's type constant, i.e. WeatherRadarCapabilityStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherRadarCapabilityStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherRadarCapabilityStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the high level availability/state of a Capability instance. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityStatusType, uci::type::accessorType::capabilityStatusType> CapabilityStatus;

         /** Indicates a RF Emission Profile currently applied to the Capability. If omitted, there are no current profiles and
           * the Capability is operating per its defaults and other constraints. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_ProfileType, uci::type::accessorType::rF_ProfileType> RF_Profile;

         /** Indicates messages that are currently an output of the Capability. See enumerated type annotations for details.
           * [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::WeatherRadarMessageOutputsEnum, uci::type::accessorType::weatherRadarMessageOutputsEnum> CurrentMessageOutput;

         /** Indicates which electronic protection options are currently enabled and will be used by this Capability. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ElectronicProtectionOptionsEnabledType, uci::type::accessorType::electronicProtectionOptionsEnabledType> ElectronicProtectionOptionsEnabled;

         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual const uci::type::WeatherRadarCapabilityStatusMDT::CapabilityStatus& getCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual uci::type::WeatherRadarCapabilityStatusMDT::CapabilityStatus& getCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityStatus(const uci::type::WeatherRadarCapabilityStatusMDT::CapabilityStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Profile.
           *
           * @return The bounded list identified by RF_Profile.
           */
         virtual const uci::type::WeatherRadarCapabilityStatusMDT::RF_Profile& getRF_Profile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Profile.
           *
           * @return The bounded list identified by RF_Profile.
           */
         virtual uci::type::WeatherRadarCapabilityStatusMDT::RF_Profile& getRF_Profile()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_Profile.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_Profile(const uci::type::WeatherRadarCapabilityStatusMDT::RF_Profile& value)
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


         /** Returns the bounded list that is identified by the CurrentMessageOutput.
           *
           * @return The bounded list identified by CurrentMessageOutput.
           */
         virtual const uci::type::WeatherRadarCapabilityStatusMDT::CurrentMessageOutput& getCurrentMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CurrentMessageOutput.
           *
           * @return The bounded list identified by CurrentMessageOutput.
           */
         virtual uci::type::WeatherRadarCapabilityStatusMDT::CurrentMessageOutput& getCurrentMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CurrentMessageOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCurrentMessageOutput(const uci::type::WeatherRadarCapabilityStatusMDT::CurrentMessageOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEnabled.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEnabled.
           */
         virtual const uci::type::WeatherRadarCapabilityStatusMDT::ElectronicProtectionOptionsEnabled& getElectronicProtectionOptionsEnabled() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEnabled.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEnabled.
           */
         virtual uci::type::WeatherRadarCapabilityStatusMDT::ElectronicProtectionOptionsEnabled& getElectronicProtectionOptionsEnabled()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptionsEnabled.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptionsEnabled(const uci::type::WeatherRadarCapabilityStatusMDT::ElectronicProtectionOptionsEnabled& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherRadarCapabilityStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherRadarCapabilityStatusMDT to copy from
           */
         WeatherRadarCapabilityStatusMDT(const WeatherRadarCapabilityStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherRadarCapabilityStatusMDT to the contents of the
           * WeatherRadarCapabilityStatusMDT on the right hand side (rhs) of the assignment
           * operator.WeatherRadarCapabilityStatusMDT [only available to derived classes]
           *
           * @param rhs The WeatherRadarCapabilityStatusMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::WeatherRadarCapabilityStatusMDT
           * @return A constant reference to this WeatherRadarCapabilityStatusMDT.
           */
         const WeatherRadarCapabilityStatusMDT& operator=(const WeatherRadarCapabilityStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherRadarCapabilityStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherRadarCapabilityStatusMDT_h

