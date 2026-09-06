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
#ifndef Uci__Type__SystemStatusMDT_h
#define Uci__Type__SystemStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__SystemStateEnum_h)
# include "uci/type/SystemStateEnum.h"
#endif

#if !defined(Uci__Type__SystemSourceEnum_h)
# include "uci/type/SystemSourceEnum.h"
#endif

#if !defined(Uci__Type__EntityFusionEligibilityEnum_h)
# include "uci/type/EntityFusionEligibilityEnum.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

#if !defined(Uci__Type__SystemIdentityType_h)
# include "uci/type/SystemIdentityType.h"
#endif

#if !defined(Uci__Type__SystemCommunicationsType_h)
# include "uci/type/SystemCommunicationsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OperatorID_Type_h)
# include "uci/type/OperatorID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__PlatformStatusType_h)
# include "uci/type/PlatformStatusType.h"
#endif

#if !defined(Uci__Type__VoiceControlType_h)
# include "uci/type/VoiceControlType.h"
#endif

#if !defined(Uci__Type__ActivityByType_h)
# include "uci/type/ActivityByType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SystemStatusMDT sequence accessor class */
      class SystemStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. SystemStatusMDT
           *
           * @return This accessor's type constant, i.e. SystemStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The UUID of the operator associated with this system. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::OperatorID_Type, uci::type::accessorType::operatorID_Type> OperatorID;

         /** Indicates a C2 relevant Subsystem of the System. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SubsystemID_Type, uci::type::accessorType::subsystemID_Type> SubsystemID;

         /** Indicates a C2 relevant Capability of the System. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates a Service that is implemented by the System. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ServiceID;

         /** This element indicates an activity, that isn't otherwise represented by UCI Tasks, Capability commands, etc. being
           * performed by the Entity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivityByType, uci::type::accessorType::activityByType> ActivityBy;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SystemState.
           *
           * @return The value of the enumeration identified by SystemState.
           */
         virtual const uci::type::SystemStateEnum& getSystemState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SystemState.
           *
           * @return The value of the enumeration identified by SystemState.
           */
         virtual uci::type::SystemStateEnum& getSystemState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SystemState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSystemState(const uci::type::SystemStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SystemState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSystemState(uci::type::SystemStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual const uci::type::SystemSourceEnum& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual uci::type::SystemSourceEnum& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(const uci::type::SystemSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(uci::type::SystemSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FusionEligibility.
           *
           * @return The value of the enumeration identified by FusionEligibility.
           */
         virtual const uci::type::EntityFusionEligibilityEnum& getFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FusionEligibility.
           *
           * @return The value of the enumeration identified by FusionEligibility.
           */
         virtual uci::type::EntityFusionEligibilityEnum& getFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFusionEligibility(const uci::type::EntityFusionEligibilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFusionEligibility(uci::type::EntityFusionEligibilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FusionEligibilityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFusionEligibility will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::entityFusionEligibilityEnum This Accessor's accessor type
           */
         virtual void enableFusionEligibility(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityFusionEligibilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Model.
           *
           * @return The value of the string data type identified by Model.
           */
         virtual const uci::type::VisibleString32Type& getModel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Model.
           *
           * @return The value of the string data type identified by Model.
           */
         virtual uci::type::VisibleString32Type& getModel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Model to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setModel(const uci::type::VisibleString32Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Model to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModel(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Model to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModel(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Model exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Model is emabled or not
           */
         virtual bool hasModel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Model
           *
           * @param type = uci::type::accessorType::visibleString32Type This Accessor's accessor type
           */
         virtual void enableModel(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString32Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Model */
         virtual void clearModel()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::SystemIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::SystemIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::SystemIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::systemIdentityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Communications.
           *
           * @return The acecssor that provides access to the complex content that is identified by Communications.
           */
         virtual const uci::type::SystemCommunicationsType& getCommunications() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Communications.
           *
           * @return The acecssor that provides access to the complex content that is identified by Communications.
           */
         virtual uci::type::SystemCommunicationsType& getCommunications()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Communications to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Communications
           */
         virtual void setCommunications(const uci::type::SystemCommunicationsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorID.
           *
           * @return The bounded list identified by OperatorID.
           */
         virtual const uci::type::SystemStatusMDT::OperatorID& getOperatorID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorID.
           *
           * @return The bounded list identified by OperatorID.
           */
         virtual uci::type::SystemStatusMDT::OperatorID& getOperatorID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorID(const uci::type::SystemStatusMDT::OperatorID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual const uci::type::SystemStatusMDT::SubsystemID& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual uci::type::SystemStatusMDT::SubsystemID& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemID(const uci::type::SystemStatusMDT::SubsystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::SystemStatusMDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::SystemStatusMDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::SystemStatusMDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual const uci::type::SystemStatusMDT::ServiceID& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual uci::type::SystemStatusMDT::ServiceID& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceID(const uci::type::SystemStatusMDT::ServiceID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformStatus.
           */
         virtual const uci::type::PlatformStatusType& getPlatformStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformStatus.
           */
         virtual uci::type::PlatformStatusType& getPlatformStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlatformStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlatformStatus
           */
         virtual void setPlatformStatus(const uci::type::PlatformStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlatformStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlatformStatus is emabled or not
           */
         virtual bool hasPlatformStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlatformStatus
           *
           * @param type = uci::type::accessorType::platformStatusType This Accessor's accessor type
           */
         virtual void enablePlatformStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::platformStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlatformStatus */
         virtual void clearPlatformStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VoiceControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by VoiceControl.
           */
         virtual const uci::type::VoiceControlType& getVoiceControl() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VoiceControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by VoiceControl.
           */
         virtual uci::type::VoiceControlType& getVoiceControl()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VoiceControl to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VoiceControl
           */
         virtual void setVoiceControl(const uci::type::VoiceControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceControl exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceControl is emabled or not
           */
         virtual bool hasVoiceControl() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceControl
           *
           * @param type = uci::type::accessorType::voiceControlType This Accessor's accessor type
           */
         virtual void enableVoiceControl(uci::base::accessorType::AccessorType type = uci::type::accessorType::voiceControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceControl */
         virtual void clearVoiceControl()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityBy.
           *
           * @return The bounded list identified by ActivityBy.
           */
         virtual const uci::type::SystemStatusMDT::ActivityBy& getActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityBy.
           *
           * @return The bounded list identified by ActivityBy.
           */
         virtual uci::type::SystemStatusMDT::ActivityBy& getActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityBy.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityBy(const uci::type::SystemStatusMDT::ActivityBy& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemStatusMDT to copy from
           */
         SystemStatusMDT(const SystemStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemStatusMDT to the contents of the SystemStatusMDT on the
           * right hand side (rhs) of the assignment operator.SystemStatusMDT [only available to derived classes]
           *
           * @param rhs The SystemStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::SystemStatusMDT
           * @return A constant reference to this SystemStatusMDT.
           */
         const SystemStatusMDT& operator=(const SystemStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemStatusMDT_h

