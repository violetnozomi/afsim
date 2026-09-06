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
#ifndef Uci__Type__TaskStatusMDT_h
#define Uci__Type__TaskStatusMDT_h 1

#if !defined(Uci__Type__TaskStatusDataBaseType_h)
# include "uci/type/TaskStatusDataBaseType.h"
#endif

#if !defined(Uci__Type__TaskStateEnum_h)
# include "uci/type/TaskStateEnum.h"
#endif

#if !defined(Uci__Type__CannotComplyType_h)
# include "uci/type/CannotComplyType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__UnallocatedReasonType_h)
# include "uci/type/UnallocatedReasonType.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__MetricsType_h)
# include "uci/type/MetricsType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__CommandActivityTraceabilityType_h)
# include "uci/type/CommandActivityTraceabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class TaskStatusMDT : public virtual uci::type::TaskStatusDataBaseType {
      public:

         /** The destructor */
         virtual ~TaskStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. TaskStatusMDT
           *
           * @return This accessor's type constant, i.e. TaskStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates details regarding the reason why the Task is unallocated or could not be allocated. This element is
           * expected and strongly encouraged when the sibling TaskState indicates unallocated. Multiple instances are allowed
           * because a single Task can have many unallocated reasons, one or more per System that allocation was attempted
           * against. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::UnallocatedReasonType, uci::type::accessorType::unallocatedReasonType> UnallocatedReason;

         /** These are IDs of Commands and/or Activities that were created to accomplish the Task. A pair may contain: 1.
           * CommandID only if a command exists but has no related activity(s); 2. Both CommandID and ActivityID, a single command
           * could generate one or multiple activities, e.g. a SAR command could result in multiple activities to cover the area;
           * 3. ActivityID only; could occur if a system supports activities not originating from a command. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommandActivityTraceabilityType, uci::type::accessorType::commandActivityTraceabilityType> Traceability;

         /** Returns the value of the enumeration that is identified by the TaskState.
           *
           * @return The value of the enumeration identified by TaskState.
           */
         virtual const uci::type::TaskStateEnum& getTaskState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TaskState.
           *
           * @return The value of the enumeration identified by TaskState.
           */
         virtual uci::type::TaskStateEnum& getTaskState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TaskState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTaskState(const uci::type::TaskStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TaskState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTaskState(uci::type::TaskStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskStateReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskStateReason.
           */
         virtual const uci::type::CannotComplyType& getTaskStateReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskStateReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskStateReason.
           */
         virtual uci::type::CannotComplyType& getTaskStateReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskStateReason to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskStateReason
           */
         virtual void setTaskStateReason(const uci::type::CannotComplyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TaskStateReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TaskStateReason is emabled or not
           */
         virtual bool hasTaskStateReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TaskStateReason
           *
           * @param type = uci::type::accessorType::cannotComplyType This Accessor's accessor type
           */
         virtual void enableTaskStateReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::cannotComplyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TaskStateReason */
         virtual void clearTaskStateReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UnallocatedReason.
           *
           * @return The bounded list identified by UnallocatedReason.
           */
         virtual const uci::type::TaskStatusMDT::UnallocatedReason& getUnallocatedReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UnallocatedReason.
           *
           * @return The bounded list identified by UnallocatedReason.
           */
         virtual uci::type::TaskStatusMDT::UnallocatedReason& getUnallocatedReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the UnallocatedReason.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setUnallocatedReason(const uci::type::TaskStatusMDT::UnallocatedReason& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlanID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanID is emabled or not
           */
         virtual bool hasMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanID
           *
           * @param type = uci::type::accessorType::missionPlanID_Type This Accessor's accessor type
           */
         virtual void enableMissionPlanID(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanID */
         virtual void clearMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedExecutionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedExecutionTime.
           */
         virtual const uci::type::DateTimeRangeType& getPlannedExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedExecutionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedExecutionTime.
           */
         virtual uci::type::DateTimeRangeType& getPlannedExecutionTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlannedExecutionTime to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlannedExecutionTime
           */
         virtual void setPlannedExecutionTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlannedExecutionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlannedExecutionTime is emabled or not
           */
         virtual bool hasPlannedExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlannedExecutionTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enablePlannedExecutionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlannedExecutionTime */
         virtual void clearPlannedExecutionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActualExecutionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActualExecutionTime.
           */
         virtual const uci::type::DateTimeRangeType& getActualExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActualExecutionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActualExecutionTime.
           */
         virtual uci::type::DateTimeRangeType& getActualExecutionTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActualExecutionTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActualExecutionTime
           */
         virtual void setActualExecutionTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualExecutionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualExecutionTime is emabled or not
           */
         virtual bool hasActualExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualExecutionTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableActualExecutionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualExecutionTime */
         virtual void clearActualExecutionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedEffectTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedEffectTime.
           */
         virtual const uci::type::DateTimeRangeType& getPlannedEffectTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedEffectTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedEffectTime.
           */
         virtual uci::type::DateTimeRangeType& getPlannedEffectTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlannedEffectTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlannedEffectTime
           */
         virtual void setPlannedEffectTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlannedEffectTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlannedEffectTime is emabled or not
           */
         virtual bool hasPlannedEffectTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlannedEffectTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enablePlannedEffectTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlannedEffectTime */
         virtual void clearPlannedEffectTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActualEffectTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActualEffectTime.
           */
         virtual const uci::type::DateTimeRangeType& getActualEffectTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActualEffectTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActualEffectTime.
           */
         virtual uci::type::DateTimeRangeType& getActualEffectTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActualEffectTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActualEffectTime
           */
         virtual void setActualEffectTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualEffectTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualEffectTime is emabled or not
           */
         virtual bool hasActualEffectTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualEffectTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableActualEffectTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualEffectTime */
         virtual void clearActualEffectTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metrics.
           */
         virtual const uci::type::MetricsType& getMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metrics.
           */
         virtual uci::type::MetricsType& getMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Metrics to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Metrics
           */
         virtual void setMetrics(const uci::type::MetricsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Metrics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Metrics is emabled or not
           */
         virtual bool hasMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Metrics
           *
           * @param type = uci::type::accessorType::metricsType This Accessor's accessor type
           */
         virtual void enableMetrics(uci::base::accessorType::AccessorType type = uci::type::accessorType::metricsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Metrics */
         virtual void clearMetrics()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentCompleted.
           *
           * @return The value of the simple primitive data type identified by PercentCompleted.
           */
         virtual uci::type::PercentTypeValue getPercentCompleted() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentCompleted.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentCompleted(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentCompleted exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentCompleted is emabled or not
           */
         virtual bool hasPercentCompleted() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentCompleted
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentCompleted(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentCompleted */
         virtual void clearPercentCompleted()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Traceability.
           *
           * @return The bounded list identified by Traceability.
           */
         virtual const uci::type::TaskStatusMDT::Traceability& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Traceability.
           *
           * @return The bounded list identified by Traceability.
           */
         virtual uci::type::TaskStatusMDT::Traceability& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Traceability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTraceability(const uci::type::TaskStatusMDT::Traceability& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskStatusMDT to copy from
           */
         TaskStatusMDT(const TaskStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskStatusMDT to the contents of the TaskStatusMDT on the right
           * hand side (rhs) of the assignment operator.TaskStatusMDT [only available to derived classes]
           *
           * @param rhs The TaskStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::TaskStatusMDT
           * @return A constant reference to this TaskStatusMDT.
           */
         const TaskStatusMDT& operator=(const TaskStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskStatusMDT_h

