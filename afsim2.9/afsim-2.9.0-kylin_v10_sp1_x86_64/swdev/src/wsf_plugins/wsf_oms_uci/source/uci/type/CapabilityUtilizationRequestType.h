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
#ifndef Uci__Type__CapabilityUtilizationRequestType_h
#define Uci__Type__CapabilityUtilizationRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__TaskID_InclusionType_h)
# include "uci/type/TaskID_InclusionType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Type__SystemMissionPlanSpecificationType_h)
# include "uci/type/SystemMissionPlanSpecificationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The inputs used in generating and/or requesting a Capability Utilization Assessment. */
      class CapabilityUtilizationRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityUtilizationRequestType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityUtilizationRequestType
           *
           * @return This accessor's type constant, i.e. CapabilityUtilizationRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityUtilizationRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityUtilizationRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element is used if the assessment should be generated in regards to only certain threats. If this element is not
           * specified then all entities are considered. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> EntityID;

         /** Represents a list of tasks (beyond those allocated to the constraint MissionPlan if defined) that should also be
           * assessed as ad hoc relative to the baseline Mission Plan. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_InclusionType, uci::type::accessorType::taskID_InclusionType> TaskID;

         /** Represents a list of capability commands (beyond those allocated to the constraint MissionPlan if defined) that
           * should also be assessed as ad hoc relative to the baseline Mission Plan. This is depicted by the
           * CapabilityCommandBaseType with the expectation that the user will populate with more specific {Capability}Command
           * types. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityCommandBaseType, uci::type::accessorType::capabilityCommandBaseType> CapabilityCommand;

         /** Represents a list of activity commands(beyond those allocated to the constraint MissionPlan if defined) that should
           * also be assessed as ad hoc relative to the baseline Mission Plan. This is depicted by the ActivityCommandBaseType
           * with the expectation that the user will populate with more specific {Capability}Command types. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivityCommandBaseType, uci::type::accessorType::activityCommandBaseType> ActivityCommand;

         /** Indicate which system or systems to assess. Optionally provide addional information about the position of each system
           * and any capabilities to consider in conjunction with a specified system. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemMissionPlanSpecificationType, uci::type::accessorType::systemMissionPlanSpecificationType> SystemToAssess;

         /** Indicate additional systems for the assessment, that are not to be assessed. Optionally provide addional information
           * about the position of each system and any capabilities to consider in conjunction with a specified system. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemMissionPlanSpecificationType, uci::type::accessorType::systemMissionPlanSpecificationType> SupportingSystem;

         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual const uci::type::CapabilityUtilizationRequestType::EntityID& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual uci::type::CapabilityUtilizationRequestType::EntityID& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityID(const uci::type::CapabilityUtilizationRequestType::EntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::CapabilityUtilizationRequestType::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::CapabilityUtilizationRequestType::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::CapabilityUtilizationRequestType::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommand.
           *
           * @return The bounded list identified by CapabilityCommand.
           */
         virtual const uci::type::CapabilityUtilizationRequestType::CapabilityCommand& getCapabilityCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommand.
           *
           * @return The bounded list identified by CapabilityCommand.
           */
         virtual uci::type::CapabilityUtilizationRequestType::CapabilityCommand& getCapabilityCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityCommand.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityCommand(const uci::type::CapabilityUtilizationRequestType::CapabilityCommand& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityCommand.
           *
           * @return The bounded list identified by ActivityCommand.
           */
         virtual const uci::type::CapabilityUtilizationRequestType::ActivityCommand& getActivityCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityCommand.
           *
           * @return The bounded list identified by ActivityCommand.
           */
         virtual uci::type::CapabilityUtilizationRequestType::ActivityCommand& getActivityCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityCommand.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityCommand(const uci::type::CapabilityUtilizationRequestType::ActivityCommand& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemToAssess.
           *
           * @return The bounded list identified by SystemToAssess.
           */
         virtual const uci::type::CapabilityUtilizationRequestType::SystemToAssess& getSystemToAssess() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemToAssess.
           *
           * @return The bounded list identified by SystemToAssess.
           */
         virtual uci::type::CapabilityUtilizationRequestType::SystemToAssess& getSystemToAssess()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemToAssess.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemToAssess(const uci::type::CapabilityUtilizationRequestType::SystemToAssess& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportingSystem.
           *
           * @return The bounded list identified by SupportingSystem.
           */
         virtual const uci::type::CapabilityUtilizationRequestType::SupportingSystem& getSupportingSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportingSystem.
           *
           * @return The bounded list identified by SupportingSystem.
           */
         virtual uci::type::CapabilityUtilizationRequestType::SupportingSystem& getSupportingSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportingSystem.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportingSystem(const uci::type::CapabilityUtilizationRequestType::SupportingSystem& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityUtilizationRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityUtilizationRequestType to copy from
           */
         CapabilityUtilizationRequestType(const CapabilityUtilizationRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityUtilizationRequestType to the contents of the
           * CapabilityUtilizationRequestType on the right hand side (rhs) of the assignment
           * operator.CapabilityUtilizationRequestType [only available to derived classes]
           *
           * @param rhs The CapabilityUtilizationRequestType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CapabilityUtilizationRequestType
           * @return A constant reference to this CapabilityUtilizationRequestType.
           */
         const CapabilityUtilizationRequestType& operator=(const CapabilityUtilizationRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityUtilizationRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityUtilizationRequestType_h

