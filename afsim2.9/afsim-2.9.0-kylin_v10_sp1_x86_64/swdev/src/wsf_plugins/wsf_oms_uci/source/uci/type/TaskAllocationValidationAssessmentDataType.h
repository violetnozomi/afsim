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
#ifndef Uci__Type__TaskAllocationValidationAssessmentDataType_h
#define Uci__Type__TaskAllocationValidationAssessmentDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The results of an assessment that determines if a set of tasks is accomplishable by a system. */
      class TaskAllocationValidationAssessmentDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TaskAllocationValidationAssessmentDataType()
         { }

         /** Returns this accessor's type constant, i.e. TaskAllocationValidationAssessmentDataType
           *
           * @return This accessor's type constant, i.e. TaskAllocationValidationAssessmentDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskAllocationValidationAssessmentDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskAllocationValidationAssessmentDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element defines any tasks that have invalid allocations. If there are none listed then the route was fully
           * valid. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> InvalidTaskID;

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


         /** Returns the bounded list that is identified by the InvalidTaskID.
           *
           * @return The bounded list identified by InvalidTaskID.
           */
         virtual const uci::type::TaskAllocationValidationAssessmentDataType::InvalidTaskID& getInvalidTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InvalidTaskID.
           *
           * @return The bounded list identified by InvalidTaskID.
           */
         virtual uci::type::TaskAllocationValidationAssessmentDataType::InvalidTaskID& getInvalidTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InvalidTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInvalidTaskID(const uci::type::TaskAllocationValidationAssessmentDataType::InvalidTaskID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskAllocationValidationAssessmentDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskAllocationValidationAssessmentDataType to copy from
           */
         TaskAllocationValidationAssessmentDataType(const TaskAllocationValidationAssessmentDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskAllocationValidationAssessmentDataType to the contents of the
           * TaskAllocationValidationAssessmentDataType on the right hand side (rhs) of the assignment
           * operator.TaskAllocationValidationAssessmentDataType [only available to derived classes]
           *
           * @param rhs The TaskAllocationValidationAssessmentDataType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::TaskAllocationValidationAssessmentDataType
           * @return A constant reference to this TaskAllocationValidationAssessmentDataType.
           */
         const TaskAllocationValidationAssessmentDataType& operator=(const TaskAllocationValidationAssessmentDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskAllocationValidationAssessmentDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskAllocationValidationAssessmentDataType_h

