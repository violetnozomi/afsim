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
#ifndef Uci__Type__MissionContingencyConditionType_h
#define Uci__Type__MissionContingencyConditionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ConflictStateEnum_h)
# include "uci/type/ConflictStateEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ConflictType_h)
# include "uci/type/ConflictType.h"
#endif

#if !defined(Uci__Type__MissionPlanningTriggerType_h)
# include "uci/type/MissionPlanningTriggerType.h"
#endif

#if !defined(Uci__Type__AutonomousActionStatusEnum_h)
# include "uci/type/AutonomousActionStatusEnum.h"
#endif

#if !defined(Uci__Type__OperatorRecommendationType_h)
# include "uci/type/OperatorRecommendationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionContingencyConditionType sequence accessor class */
      class MissionContingencyConditionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionContingencyConditionType()
         { }

         /** Returns this accessor's type constant, i.e. MissionContingencyConditionType
           *
           * @return This accessor's type constant, i.e. MissionContingencyConditionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionContingencyConditionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionContingencyConditionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element provides details of the contingency condition. This element may contain many conflicts if and only if
           * the ActionInProgress element is identical across conflicts. For example, if multiple pop-up threats occur this
           * element could contain all threats in conflict with a system (vehicle) because the autonomy case (Threat Reroute) and
           * action (Reroute affected vehicles) is the same for both conflicts. Most elements within are optional because of the
           * many different cases that this element is intended to represent. However, the requirements for describing a conflict
           * are the following: 1. ConflictingObject is required if known. 2. AffectedTaskIDs are required if and only if it is
           * known that a task can no longer be performed. 3. ConflictLocation primarily applies to route conflicts. This data is
           * primarily for operator SA. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConflictType, uci::type::accessorType::conflictType> Conflict;

         /** This element can be provided when autonomy settings dictate that autonomous correction of a problem is not allowed or
           * when a system does not have autonomous contingency correction capability. Multiple instances allow a series of
           * actions to be specified to address the contingency condition. Order is implied by the order in which elements are
           * included in the message. For example, to resolve a contingency associated with a failed Capability, the series of
           * actions might be: 0. Drop Tasks 1. Reallocate Dropped Tasks. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::OperatorRecommendationType, uci::type::accessorType::operatorRecommendationType> OperatorRecommendation;

         /** Returns the accessor that provides access to the complex content that is identified by the ConflictedSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConflictedSystemID.
           */
         virtual const uci::type::SystemID_Type& getConflictedSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConflictedSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConflictedSystemID.
           */
         virtual uci::type::SystemID_Type& getConflictedSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConflictedSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConflictedSystemID
           */
         virtual void setConflictedSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConflictState.
           *
           * @return The value of the enumeration identified by ConflictState.
           */
         virtual const uci::type::ConflictStateEnum& getConflictState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConflictState.
           *
           * @return The value of the enumeration identified by ConflictState.
           */
         virtual uci::type::ConflictStateEnum& getConflictState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConflictState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConflictState(const uci::type::ConflictStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConflictState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConflictState(uci::type::ConflictStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Conflict.
           *
           * @return The bounded list identified by Conflict.
           */
         virtual const uci::type::MissionContingencyConditionType::Conflict& getConflict() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Conflict.
           *
           * @return The bounded list identified by Conflict.
           */
         virtual uci::type::MissionContingencyConditionType::Conflict& getConflict()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Conflict.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConflict(const uci::type::MissionContingencyConditionType::Conflict& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual const uci::type::MissionPlanningTriggerType& getTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual uci::type::MissionPlanningTriggerType& getTrigger()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Trigger to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Trigger
           */
         virtual void setTrigger(const uci::type::MissionPlanningTriggerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Trigger exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Trigger is emabled or not
           */
         virtual bool hasTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Trigger
           *
           * @param type = uci::type::accessorType::missionPlanningTriggerType This Accessor's accessor type
           */
         virtual void enableTrigger(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanningTriggerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Trigger */
         virtual void clearTrigger()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AutonomousActionStatus.
           *
           * @return The value of the enumeration identified by AutonomousActionStatus.
           */
         virtual const uci::type::AutonomousActionStatusEnum& getAutonomousActionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AutonomousActionStatus.
           *
           * @return The value of the enumeration identified by AutonomousActionStatus.
           */
         virtual uci::type::AutonomousActionStatusEnum& getAutonomousActionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AutonomousActionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAutonomousActionStatus(const uci::type::AutonomousActionStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AutonomousActionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAutonomousActionStatus(uci::type::AutonomousActionStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AutonomousActionStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAutonomousActionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAutonomousActionStatus will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::autonomousActionStatusEnum This Accessor's accessor type
           */
         virtual void enableAutonomousActionStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::autonomousActionStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAutonomousActionStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorRecommendation.
           *
           * @return The bounded list identified by OperatorRecommendation.
           */
         virtual const uci::type::MissionContingencyConditionType::OperatorRecommendation& getOperatorRecommendation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorRecommendation.
           *
           * @return The bounded list identified by OperatorRecommendation.
           */
         virtual uci::type::MissionContingencyConditionType::OperatorRecommendation& getOperatorRecommendation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorRecommendation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorRecommendation(const uci::type::MissionContingencyConditionType::OperatorRecommendation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionContingencyConditionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionContingencyConditionType to copy from
           */
         MissionContingencyConditionType(const MissionContingencyConditionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionContingencyConditionType to the contents of the
           * MissionContingencyConditionType on the right hand side (rhs) of the assignment
           * operator.MissionContingencyConditionType [only available to derived classes]
           *
           * @param rhs The MissionContingencyConditionType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MissionContingencyConditionType
           * @return A constant reference to this MissionContingencyConditionType.
           */
         const MissionContingencyConditionType& operator=(const MissionContingencyConditionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionContingencyConditionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionContingencyConditionType_h

