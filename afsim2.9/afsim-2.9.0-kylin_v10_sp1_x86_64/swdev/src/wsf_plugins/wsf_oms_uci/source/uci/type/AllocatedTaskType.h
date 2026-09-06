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
#ifndef Uci__Type__AllocatedTaskType_h
#define Uci__Type__AllocatedTaskType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__MetricsType_h)
# include "uci/type/MetricsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PlannedCapabilityType_h)
# include "uci/type/PlannedCapabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AllocatedTaskType sequence accessor class */
      class AllocatedTaskType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AllocatedTaskType()
         { }

         /** Returns this accessor's type constant, i.e. AllocatedTaskType
           *
           * @return This accessor's type constant, i.e. AllocatedTaskType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::allocatedTaskType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AllocatedTaskType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Potentially addresses the initially determined capabilities and/or DMPIs associated with a Task. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PlannedCapabilityType, uci::type::accessorType::plannedCapabilityType> PlannedCapability;

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


         /** Returns the accessor that provides access to the complex content that is identified by the RequiredExecutionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequiredExecutionTime.
           */
         virtual const uci::type::DateTimeRangeType& getRequiredExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequiredExecutionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequiredExecutionTime.
           */
         virtual uci::type::DateTimeRangeType& getRequiredExecutionTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequiredExecutionTime to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequiredExecutionTime
           */
         virtual void setRequiredExecutionTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RequiredExecutionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RequiredExecutionTime is emabled or not
           */
         virtual bool hasRequiredExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RequiredExecutionTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableRequiredExecutionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RequiredExecutionTime */
         virtual void clearRequiredExecutionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequiredEffectTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequiredEffectTime.
           */
         virtual const uci::type::DateTimeRangeType& getRequiredEffectTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequiredEffectTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequiredEffectTime.
           */
         virtual uci::type::DateTimeRangeType& getRequiredEffectTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequiredEffectTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequiredEffectTime
           */
         virtual void setRequiredEffectTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RequiredEffectTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RequiredEffectTime is emabled or not
           */
         virtual bool hasRequiredEffectTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RequiredEffectTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableRequiredEffectTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RequiredEffectTime */
         virtual void clearRequiredEffectTime()
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


         /** Returns the bounded list that is identified by the PlannedCapability.
           *
           * @return The bounded list identified by PlannedCapability.
           */
         virtual const uci::type::AllocatedTaskType::PlannedCapability& getPlannedCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PlannedCapability.
           *
           * @return The bounded list identified by PlannedCapability.
           */
         virtual uci::type::AllocatedTaskType::PlannedCapability& getPlannedCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PlannedCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlannedCapability(const uci::type::AllocatedTaskType::PlannedCapability& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AllocatedTaskType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AllocatedTaskType to copy from
           */
         AllocatedTaskType(const AllocatedTaskType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AllocatedTaskType to the contents of the AllocatedTaskType on the
           * right hand side (rhs) of the assignment operator.AllocatedTaskType [only available to derived classes]
           *
           * @param rhs The AllocatedTaskType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AllocatedTaskType
           * @return A constant reference to this AllocatedTaskType.
           */
         const AllocatedTaskType& operator=(const AllocatedTaskType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AllocatedTaskType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AllocatedTaskType_h

