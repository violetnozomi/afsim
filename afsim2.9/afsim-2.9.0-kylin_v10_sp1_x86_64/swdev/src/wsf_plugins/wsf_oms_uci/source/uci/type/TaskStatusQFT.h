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
#ifndef Uci__Type__TaskStatusQFT_h
#define Uci__Type__TaskStatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__TaskStateEnum_h)
# include "uci/type/TaskStateEnum.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TaskStatusQFT sequence accessor class */
      class TaskStatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~TaskStatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. TaskStatusQFT
           *
           * @return This accessor's type constant, i.e. TaskStatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskStatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter by the Task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Filter by the state of the task. [Minimum occurrences: 0] [Maximum occurrences: 13] */
         typedef uci::base::BoundedList<uci::type::TaskStateEnum, uci::type::accessorType::taskStateEnum> TaskState;

         /** Filter by the associated MissionPlanID. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeTask.
           *
           * @return The value of the simple primitive data type identified by IncludeTask.
           */
         virtual xs::Boolean getIncludeTask() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeTask.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeTask(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::TaskStatusQFT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::TaskStatusQFT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::TaskStatusQFT::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskState.
           *
           * @return The bounded list identified by TaskState.
           */
         virtual const uci::type::TaskStatusQFT::TaskState& getTaskState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskState.
           *
           * @return The bounded list identified by TaskState.
           */
         virtual uci::type::TaskStatusQFT::TaskState& getTaskState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskState(const uci::type::TaskStatusQFT::TaskState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::TaskStatusQFT::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::TaskStatusQFT::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::TaskStatusQFT::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskStatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskStatusQFT to copy from
           */
         TaskStatusQFT(const TaskStatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskStatusQFT to the contents of the TaskStatusQFT on the right
           * hand side (rhs) of the assignment operator.TaskStatusQFT [only available to derived classes]
           *
           * @param rhs The TaskStatusQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::TaskStatusQFT
           * @return A constant reference to this TaskStatusQFT.
           */
         const TaskStatusQFT& operator=(const TaskStatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskStatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskStatusQFT_h

