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
#ifndef Uci__Type__SubsystemStatusMDT_h
#define Uci__Type__SubsystemStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemStateEnum_h)
# include "uci/type/SubsystemStateEnum.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemEnabledSettingType_h)
# include "uci/type/SubsystemEnabledSettingType.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Type__StateTransitionStatusEnum_h)
# include "uci/type/StateTransitionStatusEnum.h"
#endif

#if !defined(Uci__Type__SubsystemConfigurationType_h)
# include "uci/type/SubsystemConfigurationType.h"
#endif

#if !defined(Uci__Type__SubsystemComponentType_h)
# include "uci/type/SubsystemComponentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemStatusMDT sequence accessor class */
      class SubsystemStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStatusMDT
           *
           * @return This accessor's type constant, i.e. SubsystemStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a Subsystem setting that is currently enabled. Settings are enabled/disabled via the
           * SubsystemSettingsCommand. See SubsystemSettingEnum for further details. If multiple instances are given, each should
           * be of a different setting type as indicated by the child element. [Minimum occurrences: 0] [Maximum occurrences: 8]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemEnabledSettingType, uci::type::accessorType::subsystemEnabledSettingType> EnabledSetting;

         /** Indicates an auxiliary or subordinate Component of this Subsystem. Components can be physical or functional. Any
           * Component that is relevant to the state, health and/or C2 of the Subsystem can be reported. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemComponentType, uci::type::accessorType::subsystemComponentType> SubsystemComponent;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubsystemState.
           *
           * @return The value of the enumeration identified by SubsystemState.
           */
         virtual const uci::type::SubsystemStateEnum& getSubsystemState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubsystemState.
           *
           * @return The value of the enumeration identified by SubsystemState.
           */
         virtual uci::type::SubsystemStateEnum& getSubsystemState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubsystemState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubsystemState(const uci::type::SubsystemStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubsystemState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubsystemState(uci::type::SubsystemStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SubsystemStateDescription.
           *
           * @return The value of the string data type identified by SubsystemStateDescription.
           */
         virtual const uci::type::VisibleString256Type& getSubsystemStateDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SubsystemStateDescription.
           *
           * @return The value of the string data type identified by SubsystemStateDescription.
           */
         virtual uci::type::VisibleString256Type& getSubsystemStateDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SubsystemStateDescription to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSubsystemStateDescription(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SubsystemStateDescription to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSubsystemStateDescription(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SubsystemStateDescription to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSubsystemStateDescription(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubsystemStateDescription exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubsystemStateDescription is emabled or not
           */
         virtual bool hasSubsystemStateDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubsystemStateDescription
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableSubsystemStateDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubsystemStateDescription */
         virtual void clearSubsystemStateDescription()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EnabledSetting.
           *
           * @return The bounded list identified by EnabledSetting.
           */
         virtual const uci::type::SubsystemStatusMDT::EnabledSetting& getEnabledSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EnabledSetting.
           *
           * @return The bounded list identified by EnabledSetting.
           */
         virtual uci::type::SubsystemStatusMDT::EnabledSetting& getEnabledSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EnabledSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnabledSetting(const uci::type::SubsystemStatusMDT::EnabledSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EraseStatus.
           *
           * @return The value of the enumeration identified by EraseStatus.
           */
         virtual const uci::type::ProcessingStatusEnum& getEraseStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EraseStatus.
           *
           * @return The value of the enumeration identified by EraseStatus.
           */
         virtual uci::type::ProcessingStatusEnum& getEraseStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EraseStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEraseStatus(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EraseStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEraseStatus(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EraseStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEraseStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEraseStatus will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::processingStatusEnum This Accessor's accessor type
           */
         virtual void enableEraseStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::processingStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEraseStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the StateTransitionStatus.
           *
           * @return The value of the enumeration identified by StateTransitionStatus.
           */
         virtual const uci::type::StateTransitionStatusEnum& getStateTransitionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the StateTransitionStatus.
           *
           * @return The value of the enumeration identified by StateTransitionStatus.
           */
         virtual uci::type::StateTransitionStatusEnum& getStateTransitionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the StateTransitionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStateTransitionStatus(const uci::type::StateTransitionStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the StateTransitionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStateTransitionStatus(uci::type::StateTransitionStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield StateTransitionStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasStateTransitionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getStateTransitionStatus will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::stateTransitionStatusEnum This Accessor's accessor type
           */
         virtual void enableStateTransitionStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::stateTransitionStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearStateTransitionStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Configuration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Configuration.
           */
         virtual const uci::type::SubsystemConfigurationType& getConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Configuration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Configuration.
           */
         virtual uci::type::SubsystemConfigurationType& getConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Configuration to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Configuration
           */
         virtual void setConfiguration(const uci::type::SubsystemConfigurationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemComponent.
           *
           * @return The bounded list identified by SubsystemComponent.
           */
         virtual const uci::type::SubsystemStatusMDT::SubsystemComponent& getSubsystemComponent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemComponent.
           *
           * @return The bounded list identified by SubsystemComponent.
           */
         virtual uci::type::SubsystemStatusMDT::SubsystemComponent& getSubsystemComponent()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemComponent.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemComponent(const uci::type::SubsystemStatusMDT::SubsystemComponent& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStatusMDT to copy from
           */
         SubsystemStatusMDT(const SubsystemStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStatusMDT to the contents of the SubsystemStatusMDT on
           * the right hand side (rhs) of the assignment operator.SubsystemStatusMDT [only available to derived classes]
           *
           * @param rhs The SubsystemStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SubsystemStatusMDT
           * @return A constant reference to this SubsystemStatusMDT.
           */
         const SubsystemStatusMDT& operator=(const SubsystemStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStatusMDT_h

