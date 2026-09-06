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
#ifndef Uci__Type__ApprovalPolicyMDT_h
#define Uci__Type__ApprovalPolicyMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ApprovalPolicyID_Type_h)
# include "uci/type/ApprovalPolicyID_Type.h"
#endif

#if !defined(Uci__Type__TaskAllocationPolicyType_h)
# include "uci/type/TaskAllocationPolicyType.h"
#endif

#if !defined(Uci__Type__RoutePlanningPolicyType_h)
# include "uci/type/RoutePlanningPolicyType.h"
#endif

#if !defined(Uci__Type__TaskExecutionPolicyType_h)
# include "uci/type/TaskExecutionPolicyType.h"
#endif

#if !defined(Uci__Type__MissionPlanActivationPolicyType_h)
# include "uci/type/MissionPlanActivationPolicyType.h"
#endif

#if !defined(Uci__Type__TimedZoneType_h)
# include "uci/type/TimedZoneType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__MissionTraceabilityType_h)
# include "uci/type/MissionTraceabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ApprovalPolicyMDT sequence accessor class */
      class ApprovalPolicyMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ApprovalPolicyMDT()
         { }

         /** Returns this accessor's type constant, i.e. ApprovalPolicyMDT
           *
           * @return This accessor's type constant, i.e. ApprovalPolicyMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::approvalPolicyMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ApprovalPolicyMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApprovalPolicyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalPolicyID.
           */
         virtual const uci::type::ApprovalPolicyID_Type& getApprovalPolicyID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApprovalPolicyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalPolicyID.
           */
         virtual uci::type::ApprovalPolicyID_Type& getApprovalPolicyID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApprovalPolicyID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApprovalPolicyID
           */
         virtual void setApprovalPolicyID(const uci::type::ApprovalPolicyID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAllocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAllocation.
           */
         virtual const uci::type::TaskAllocationPolicyType& getTaskAllocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAllocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAllocation.
           */
         virtual uci::type::TaskAllocationPolicyType& getTaskAllocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskAllocation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskAllocation
           */
         virtual void setTaskAllocation(const uci::type::TaskAllocationPolicyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TaskAllocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TaskAllocation is emabled or not
           */
         virtual bool hasTaskAllocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TaskAllocation
           *
           * @param type = uci::type::accessorType::taskAllocationPolicyType This Accessor's accessor type
           */
         virtual void enableTaskAllocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::taskAllocationPolicyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TaskAllocation */
         virtual void clearTaskAllocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RoutePlanning.
           *
           * @return The acecssor that provides access to the complex content that is identified by RoutePlanning.
           */
         virtual const uci::type::RoutePlanningPolicyType& getRoutePlanning() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RoutePlanning.
           *
           * @return The acecssor that provides access to the complex content that is identified by RoutePlanning.
           */
         virtual uci::type::RoutePlanningPolicyType& getRoutePlanning()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RoutePlanning to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RoutePlanning
           */
         virtual void setRoutePlanning(const uci::type::RoutePlanningPolicyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RoutePlanning exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RoutePlanning is emabled or not
           */
         virtual bool hasRoutePlanning() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RoutePlanning
           *
           * @param type = uci::type::accessorType::routePlanningPolicyType This Accessor's accessor type
           */
         virtual void enableRoutePlanning(uci::base::accessorType::AccessorType type = uci::type::accessorType::routePlanningPolicyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RoutePlanning */
         virtual void clearRoutePlanning()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskExecution.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskExecution.
           */
         virtual const uci::type::TaskExecutionPolicyType& getTaskExecution() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskExecution.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskExecution.
           */
         virtual uci::type::TaskExecutionPolicyType& getTaskExecution()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskExecution to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskExecution
           */
         virtual void setTaskExecution(const uci::type::TaskExecutionPolicyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TaskExecution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TaskExecution is emabled or not
           */
         virtual bool hasTaskExecution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TaskExecution
           *
           * @param type = uci::type::accessorType::taskExecutionPolicyType This Accessor's accessor type
           */
         virtual void enableTaskExecution(uci::base::accessorType::AccessorType type = uci::type::accessorType::taskExecutionPolicyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TaskExecution */
         virtual void clearTaskExecution()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanActivation.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanActivation.
           */
         virtual const uci::type::MissionPlanActivationPolicyType& getMissionPlanActivation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanActivation.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanActivation.
           */
         virtual uci::type::MissionPlanActivationPolicyType& getMissionPlanActivation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanActivation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanActivation
           */
         virtual void setMissionPlanActivation(const uci::type::MissionPlanActivationPolicyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlanActivation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanActivation is emabled or not
           */
         virtual bool hasMissionPlanActivation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanActivation
           *
           * @param type = uci::type::accessorType::missionPlanActivationPolicyType This Accessor's accessor type
           */
         virtual void enableMissionPlanActivation(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanActivationPolicyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanActivation */
         virtual void clearMissionPlanActivation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimedZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimedZone.
           */
         virtual const uci::type::TimedZoneType& getTimedZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimedZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimedZone.
           */
         virtual uci::type::TimedZoneType& getTimedZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimedZone to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimedZone
           */
         virtual void setTimedZone(const uci::type::TimedZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimedZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimedZone is emabled or not
           */
         virtual bool hasTimedZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimedZone
           *
           * @param type = uci::type::accessorType::timedZoneType This Accessor's accessor type
           */
         virtual void enableTimedZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::timedZoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimedZone */
         virtual void clearTimedZone()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Expires.
           *
           * @return The value of the simple primitive data type identified by Expires.
           */
         virtual uci::type::DateTimeTypeValue getExpires() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Expires.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExpires(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Expires exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Expires is emabled or not
           */
         virtual bool hasExpires() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Expires
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableExpires(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Expires */
         virtual void clearExpires()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionTraceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionTraceability.
           */
         virtual const uci::type::MissionTraceabilityType& getMissionTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionTraceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionTraceability.
           */
         virtual uci::type::MissionTraceabilityType& getMissionTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionTraceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionTraceability
           */
         virtual void setMissionTraceability(const uci::type::MissionTraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionTraceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionTraceability is emabled or not
           */
         virtual bool hasMissionTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionTraceability
           *
           * @param type = uci::type::accessorType::missionTraceabilityType This Accessor's accessor type
           */
         virtual void enableMissionTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionTraceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionTraceability */
         virtual void clearMissionTraceability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ApprovalPolicyMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ApprovalPolicyMDT to copy from
           */
         ApprovalPolicyMDT(const ApprovalPolicyMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ApprovalPolicyMDT to the contents of the ApprovalPolicyMDT on the
           * right hand side (rhs) of the assignment operator.ApprovalPolicyMDT [only available to derived classes]
           *
           * @param rhs The ApprovalPolicyMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ApprovalPolicyMDT
           * @return A constant reference to this ApprovalPolicyMDT.
           */
         const ApprovalPolicyMDT& operator=(const ApprovalPolicyMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ApprovalPolicyMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ApprovalPolicyMDT_h

