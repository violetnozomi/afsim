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
#ifndef Uci__Type__TaskDataBaseType_h
#define Uci__Type__TaskDataBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DependentTaskType_h)
# include "uci/type/DependentTaskType.h"
#endif

#if !defined(Uci__Type__ConcurrentTaskReferenceType_h)
# include "uci/type/ConcurrentTaskReferenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class TaskDataBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TaskDataBaseType()
         { }

         /** Returns this accessor's type constant, i.e. TaskDataBaseType
           *
           * @return This accessor's type constant, i.e. TaskDataBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskDataBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskDataBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides a way to link tasks. This can be used to represent a strategy such as look-shoot-look. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DependentTaskType, uci::type::accessorType::dependentTaskType> DependentTask;

         /** Provides a way to link tasks with a concurrent temporal relationship. This can be used to represent coordinated
           * attacks [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConcurrentTaskReferenceType, uci::type::accessorType::concurrentTaskReferenceType> ConcurrentTask;

         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual const uci::type::TaskID_Type& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual uci::type::TaskID_Type& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskID
           */
         virtual void setTaskID(const uci::type::TaskID_Type& value)
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


         /** Returns the bounded list that is identified by the DependentTask.
           *
           * @return The bounded list identified by DependentTask.
           */
         virtual const uci::type::TaskDataBaseType::DependentTask& getDependentTask() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTask.
           *
           * @return The bounded list identified by DependentTask.
           */
         virtual uci::type::TaskDataBaseType::DependentTask& getDependentTask()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DependentTask.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDependentTask(const uci::type::TaskDataBaseType::DependentTask& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentTask.
           *
           * @return The bounded list identified by ConcurrentTask.
           */
         virtual const uci::type::TaskDataBaseType::ConcurrentTask& getConcurrentTask() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentTask.
           *
           * @return The bounded list identified by ConcurrentTask.
           */
         virtual uci::type::TaskDataBaseType::ConcurrentTask& getConcurrentTask()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConcurrentTask.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConcurrentTask(const uci::type::TaskDataBaseType::ConcurrentTask& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskDataBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskDataBaseType to copy from
           */
         TaskDataBaseType(const TaskDataBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskDataBaseType to the contents of the TaskDataBaseType on the
           * right hand side (rhs) of the assignment operator.TaskDataBaseType [only available to derived classes]
           *
           * @param rhs The TaskDataBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::TaskDataBaseType
           * @return A constant reference to this TaskDataBaseType.
           */
         const TaskDataBaseType& operator=(const TaskDataBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskDataBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskDataBaseType_h

