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
#ifndef Uci__Type__TaskQFT_h
#define Uci__Type__TaskQFT_h 1

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

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

#if !defined(Uci__Type__TaskTypeEnum_h)
# include "uci/type/TaskTypeEnum.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__TaskStateEnum_h)
# include "uci/type/TaskStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TaskQFT sequence accessor class */
      class TaskQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~TaskQFT()
         { }

         /** Returns this accessor's type constant, i.e. TaskQFT
           *
           * @return This accessor's type constant, i.e. TaskQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter by Task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** This element lets a geographical filter be applied to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GeoFiltersType, uci::type::accessorType::geoFiltersType> GeoFilter;

         /** Filter on the type of task [Minimum occurrences: 0] [Maximum occurrences: 22] */
         typedef uci::base::BoundedList<uci::type::TaskTypeEnum, uci::type::accessorType::taskTypeEnum> TaskType;

         /** This element lets a temporal filter be applied to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> TemporalFilter;

         /** Filter on the Parent Task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> ParentTaskID;

         /** Filter on the Dependent Task ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> DependentTaskID;

         /** Filter on the state of the task. [Minimum occurrences: 0] [Maximum occurrences: 13] */
         typedef uci::base::BoundedList<uci::type::TaskStateEnum, uci::type::accessorType::taskStateEnum> TaskState;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @return The value of the simple primitive data type identified by IncludeStatus.
           */
         virtual xs::Boolean getIncludeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::TaskQFT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::TaskQFT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::TaskQFT::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual const uci::type::TaskQFT::GeoFilter& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual uci::type::TaskQFT::GeoFilter& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeoFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeoFilter(const uci::type::TaskQFT::GeoFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskType.
           *
           * @return The bounded list identified by TaskType.
           */
         virtual const uci::type::TaskQFT::TaskType& getTaskType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskType.
           *
           * @return The bounded list identified by TaskType.
           */
         virtual uci::type::TaskQFT::TaskType& getTaskType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskType(const uci::type::TaskQFT::TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual const uci::type::TraceabilityType& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual uci::type::TraceabilityType& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Traceability
           */
         virtual void setTraceability(const uci::type::TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Traceability is emabled or not
           */
         virtual bool hasTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Traceability
           *
           * @param type = uci::type::accessorType::traceabilityType This Accessor's accessor type
           */
         virtual void enableTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::traceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Traceability */
         virtual void clearTraceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual const uci::type::TaskQFT::TemporalFilter& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual uci::type::TaskQFT::TemporalFilter& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TemporalFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTemporalFilter(const uci::type::TaskQFT::TemporalFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ParentTaskID.
           *
           * @return The bounded list identified by ParentTaskID.
           */
         virtual const uci::type::TaskQFT::ParentTaskID& getParentTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ParentTaskID.
           *
           * @return The bounded list identified by ParentTaskID.
           */
         virtual uci::type::TaskQFT::ParentTaskID& getParentTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ParentTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setParentTaskID(const uci::type::TaskQFT::ParentTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTaskID.
           *
           * @return The bounded list identified by DependentTaskID.
           */
         virtual const uci::type::TaskQFT::DependentTaskID& getDependentTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTaskID.
           *
           * @return The bounded list identified by DependentTaskID.
           */
         virtual uci::type::TaskQFT::DependentTaskID& getDependentTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DependentTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDependentTaskID(const uci::type::TaskQFT::DependentTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskState.
           *
           * @return The bounded list identified by TaskState.
           */
         virtual const uci::type::TaskQFT::TaskState& getTaskState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskState.
           *
           * @return The bounded list identified by TaskState.
           */
         virtual uci::type::TaskQFT::TaskState& getTaskState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskState(const uci::type::TaskQFT::TaskState& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskQFT to copy from
           */
         TaskQFT(const TaskQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskQFT to the contents of the TaskQFT on the right hand side
           * (rhs) of the assignment operator.TaskQFT [only available to derived classes]
           *
           * @param rhs The TaskQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::TaskQFT
           * @return A constant reference to this TaskQFT.
           */
         const TaskQFT& operator=(const TaskQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskQFT_h

