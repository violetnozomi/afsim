// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CommPointingPlanRequestType_h
#define Uci__Type__CommPointingPlanRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__CommPointingAssessmentEnum_h)
# include "uci/type/CommPointingAssessmentEnum.h"
#endif

#if !defined(Uci__Type__CommCoverageUserDataType_h)
# include "uci/type/CommCoverageUserDataType.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequirementID_Type_h)
# include "uci/type/CommSchedulingRequirementID_Type.h"
#endif

#if !defined(Uci__Type__CommScheduleAllocationID_Type_h)
# include "uci/type/CommScheduleAllocationID_Type.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The inputs used in generating and/or requesting a Comm Pointing Assessment. */
      class CommPointingPlanRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPointingPlanRequestType()
         { }

         /** Returns this accessor's type constant, i.e. CommPointingPlanRequestType
           *
           * @return This accessor's type constant, i.e. CommPointingPlanRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPointingPlanRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPointingPlanRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** List of MissionPlan IDs that need to have communication supported. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** This element constrains the assessment to the use of all unscheduled, unpurchased, available comm. When Assessment
           * Constraint Level is set to COMM_SCHEDULE_AVAILABILITY, this element is populated concurrently with communication
           * scheduling Requirements Constraints and communication schedule allocation Constraints. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommCoverageUserDataType, uci::type::accessorType::commCoverageUserDataType> CommScheduleAvailabilityConstraints;

         /** This element constrains the assessment to the use of communication requirements that may or may not have been
           * submitted in a CommSchedulingRequest. This assessment type allows communication to be analyzed based on any pending
           * communication purchases or prior to purchase. When Assessment Constraint Level is set to COMM_SCHEDULE_REQUIREMENTS,
           * this element may be populated concurrently with communicationSchedule Allocation Constraints [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequirementID_Type, uci::type::accessorType::commSchedulingRequirementID_Type> CommSchedulingRequirementConstraintsID;

         /** List of external communications resources that have been allocated to Missions. When Assessment Constraint Level is
           * set to COMM_SCHEDULE_ALLOCATIONS, this element is populated by itself in absence of the other constraint types.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommScheduleAllocationID_Type, uci::type::accessorType::commScheduleAllocationID_Type> CommScheduleAllocationConstraintsID;

         /** Identifies the foreign key(s) associated with the assessment resources. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> AssessmentResourceList;

         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::CommPointingPlanRequestType::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::CommPointingPlanRequestType::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::CommPointingPlanRequestType::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AssessmentConstraintLevel.
           *
           * @return The value of the enumeration identified by AssessmentConstraintLevel.
           */
         virtual const uci::type::CommPointingAssessmentEnum& getAssessmentConstraintLevel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AssessmentConstraintLevel.
           *
           * @return The value of the enumeration identified by AssessmentConstraintLevel.
           */
         virtual uci::type::CommPointingAssessmentEnum& getAssessmentConstraintLevel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AssessmentConstraintLevel.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAssessmentConstraintLevel(const uci::type::CommPointingAssessmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AssessmentConstraintLevel.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAssessmentConstraintLevel(uci::type::CommPointingAssessmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommScheduleAvailabilityConstraints.
           *
           * @return The bounded list identified by CommScheduleAvailabilityConstraints.
           */
         virtual const uci::type::CommPointingPlanRequestType::CommScheduleAvailabilityConstraints& getCommScheduleAvailabilityConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommScheduleAvailabilityConstraints.
           *
           * @return The bounded list identified by CommScheduleAvailabilityConstraints.
           */
         virtual uci::type::CommPointingPlanRequestType::CommScheduleAvailabilityConstraints& getCommScheduleAvailabilityConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommScheduleAvailabilityConstraints.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommScheduleAvailabilityConstraints(const uci::type::CommPointingPlanRequestType::CommScheduleAvailabilityConstraints& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementConstraintsID.
           *
           * @return The bounded list identified by CommSchedulingRequirementConstraintsID.
           */
         virtual const uci::type::CommPointingPlanRequestType::CommSchedulingRequirementConstraintsID& getCommSchedulingRequirementConstraintsID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementConstraintsID.
           *
           * @return The bounded list identified by CommSchedulingRequirementConstraintsID.
           */
         virtual uci::type::CommPointingPlanRequestType::CommSchedulingRequirementConstraintsID& getCommSchedulingRequirementConstraintsID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSchedulingRequirementConstraintsID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSchedulingRequirementConstraintsID(const uci::type::CommPointingPlanRequestType::CommSchedulingRequirementConstraintsID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommScheduleAllocationConstraintsID.
           *
           * @return The bounded list identified by CommScheduleAllocationConstraintsID.
           */
         virtual const uci::type::CommPointingPlanRequestType::CommScheduleAllocationConstraintsID& getCommScheduleAllocationConstraintsID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommScheduleAllocationConstraintsID.
           *
           * @return The bounded list identified by CommScheduleAllocationConstraintsID.
           */
         virtual uci::type::CommPointingPlanRequestType::CommScheduleAllocationConstraintsID& getCommScheduleAllocationConstraintsID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommScheduleAllocationConstraintsID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommScheduleAllocationConstraintsID(const uci::type::CommPointingPlanRequestType::CommScheduleAllocationConstraintsID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssessmentResourceList.
           *
           * @return The bounded list identified by AssessmentResourceList.
           */
         virtual const uci::type::CommPointingPlanRequestType::AssessmentResourceList& getAssessmentResourceList() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssessmentResourceList.
           *
           * @return The bounded list identified by AssessmentResourceList.
           */
         virtual uci::type::CommPointingPlanRequestType::AssessmentResourceList& getAssessmentResourceList()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssessmentResourceList.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssessmentResourceList(const uci::type::CommPointingPlanRequestType::AssessmentResourceList& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPointingPlanRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPointingPlanRequestType to copy from
           */
         CommPointingPlanRequestType(const CommPointingPlanRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPointingPlanRequestType to the contents of the
           * CommPointingPlanRequestType on the right hand side (rhs) of the assignment operator.CommPointingPlanRequestType [only
           * available to derived classes]
           *
           * @param rhs The CommPointingPlanRequestType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommPointingPlanRequestType
           * @return A constant reference to this CommPointingPlanRequestType.
           */
         const CommPointingPlanRequestType& operator=(const CommPointingPlanRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPointingPlanRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPointingPlanRequestType_h

