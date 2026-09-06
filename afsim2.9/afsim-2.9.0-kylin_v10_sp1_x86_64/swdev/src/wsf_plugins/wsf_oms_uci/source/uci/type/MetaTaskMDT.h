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
#ifndef Uci__Type__MetaTaskMDT_h
#define Uci__Type__MetaTaskMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MetaTaskID_Type_h)
# include "uci/type/MetaTaskID_Type.h"
#endif

#if !defined(Uci__Type__MetaTaskTypeEnum_h)
# include "uci/type/MetaTaskTypeEnum.h"
#endif

#if !defined(Uci__Type__MetaTaskTargetType_h)
# include "uci/type/MetaTaskTargetType.h"
#endif

#if !defined(Uci__Type__MetaTaskCommandedResponseType_h)
# include "uci/type/MetaTaskCommandedResponseType.h"
#endif

#if !defined(Uci__Type__MetaTaskTriggerType_h)
# include "uci/type/MetaTaskTriggerType.h"
#endif

#if !defined(Uci__Type__TimingConstraintsType_h)
# include "uci/type/TimingConstraintsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DependentTaskType_h)
# include "uci/type/DependentTaskType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetaTaskMDT sequence accessor class */
      class MetaTaskMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MetaTaskMDT()
         { }

         /** Returns this accessor's type constant, i.e. MetaTaskMDT
           *
           * @return This accessor's type constant, i.e. MetaTaskMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metaTaskMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetaTaskMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides a way to link tasks. This can be used to represent a strategy such as look-shoot-look. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DependentTaskType, uci::type::accessorType::dependentTaskType> DependentTask;

         /** Indicates the unique ID of a System which the mission response list is applicable to. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApplicableSystemID;

         /** One or more Services to which the mission response list applies. If omitted, the settings apply to all Services of
           * the Systems given in the sibling ApplicableSystemID. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ApplicableServiceID;

         /** Returns the accessor that provides access to the complex content that is identified by the MetaTaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetaTaskID.
           */
         virtual const uci::type::MetaTaskID_Type& getMetaTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetaTaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetaTaskID.
           */
         virtual uci::type::MetaTaskID_Type& getMetaTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MetaTaskID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MetaTaskID
           */
         virtual void setMetaTaskID(const uci::type::MetaTaskID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MetaTaskType.
           *
           * @return The value of the enumeration identified by MetaTaskType.
           */
         virtual const uci::type::MetaTaskTypeEnum& getMetaTaskType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MetaTaskType.
           *
           * @return The value of the enumeration identified by MetaTaskType.
           */
         virtual uci::type::MetaTaskTypeEnum& getMetaTaskType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MetaTaskType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMetaTaskType(const uci::type::MetaTaskTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MetaTaskType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMetaTaskType(uci::type::MetaTaskTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::MetaTaskTargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::MetaTaskTargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::MetaTaskTargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandedResponse.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandedResponse.
           */
         virtual const uci::type::MetaTaskCommandedResponseType& getCommandedResponse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandedResponse.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandedResponse.
           */
         virtual uci::type::MetaTaskCommandedResponseType& getCommandedResponse()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandedResponse to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandedResponse
           */
         virtual void setCommandedResponse(const uci::type::MetaTaskCommandedResponseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommandedResponse exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommandedResponse is emabled or not
           */
         virtual bool hasCommandedResponse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommandedResponse
           *
           * @param type = uci::type::accessorType::metaTaskCommandedResponseType This Accessor's accessor type
           */
         virtual void enableCommandedResponse(uci::base::accessorType::AccessorType type = uci::type::accessorType::metaTaskCommandedResponseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommandedResponse */
         virtual void clearCommandedResponse()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual const uci::type::MetaTaskTriggerType& getTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual uci::type::MetaTaskTriggerType& getTrigger()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Trigger to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Trigger
           */
         virtual void setTrigger(const uci::type::MetaTaskTriggerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Trigger exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Trigger is emabled or not
           */
         virtual bool hasTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Trigger
           *
           * @param type = uci::type::accessorType::metaTaskTriggerType This Accessor's accessor type
           */
         virtual void enableTrigger(uci::base::accessorType::AccessorType type = uci::type::accessorType::metaTaskTriggerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Trigger */
         virtual void clearTrigger()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual const uci::type::TimingConstraintsType& getTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual uci::type::TimingConstraintsType& getTimingConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimingConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimingConstraints
           */
         virtual void setTimingConstraints(const uci::type::TimingConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimingConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimingConstraints is emabled or not
           */
         virtual bool hasTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimingConstraints
           *
           * @param type = uci::type::accessorType::timingConstraintsType This Accessor's accessor type
           */
         virtual void enableTimingConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::timingConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimingConstraints */
         virtual void clearTimingConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTask.
           *
           * @return The bounded list identified by DependentTask.
           */
         virtual const uci::type::MetaTaskMDT::DependentTask& getDependentTask() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentTask.
           *
           * @return The bounded list identified by DependentTask.
           */
         virtual uci::type::MetaTaskMDT::DependentTask& getDependentTask()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DependentTask.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDependentTask(const uci::type::MetaTaskMDT::DependentTask& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual const uci::type::MetaTaskMDT::ApplicableSystemID& getApplicableSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual uci::type::MetaTaskMDT::ApplicableSystemID& getApplicableSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableSystemID(const uci::type::MetaTaskMDT::ApplicableSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableServiceID.
           *
           * @return The bounded list identified by ApplicableServiceID.
           */
         virtual const uci::type::MetaTaskMDT::ApplicableServiceID& getApplicableServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableServiceID.
           *
           * @return The bounded list identified by ApplicableServiceID.
           */
         virtual uci::type::MetaTaskMDT::ApplicableServiceID& getApplicableServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableServiceID(const uci::type::MetaTaskMDT::ApplicableServiceID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetaTaskMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetaTaskMDT to copy from
           */
         MetaTaskMDT(const MetaTaskMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetaTaskMDT to the contents of the MetaTaskMDT on the right hand
           * side (rhs) of the assignment operator.MetaTaskMDT [only available to derived classes]
           *
           * @param rhs The MetaTaskMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::MetaTaskMDT
           * @return A constant reference to this MetaTaskMDT.
           */
         const MetaTaskMDT& operator=(const MetaTaskMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetaTaskMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetaTaskMDT_h

