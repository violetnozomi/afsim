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
#ifndef Uci__Type__SupportedMissionPlanCommandsTaskAllocationType_h
#define Uci__Type__SupportedMissionPlanCommandsTaskAllocationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SupportedMissionPlanCommandsTaskAllocationMissionPlanProcessType_h)
# include "uci/type/SupportedMissionPlanCommandsTaskAllocationMissionPlanProcessType.h"
#endif

#if !defined(Uci__Type__SupportedMissionPlanCommandsTaskAllocationFunctionType_h)
# include "uci/type/SupportedMissionPlanCommandsTaskAllocationFunctionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SupportedMissionPlanCommandsTaskAllocationType sequence accessor class */
      class SupportedMissionPlanCommandsTaskAllocationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SupportedMissionPlanCommandsTaskAllocationType()
         { }

         /** Returns this accessor's type constant, i.e. SupportedMissionPlanCommandsTaskAllocationType
           *
           * @return This accessor's type constant, i.e. SupportedMissionPlanCommandsTaskAllocationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::supportedMissionPlanCommandsTaskAllocationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SupportedMissionPlanCommandsTaskAllocationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the mission plan processes which can be invoked via a MissionPlanCommand...TaskAllocation. A task
           * allocation mission plan process invocation of a given type continues with the corresponding route planning mission
           * plan process type. In other words, a MissionPlanCommand...TaskAllocation invocation of the type given here will
           * result in the route planning steps for the same MissionPlanProcess type to be performed also. If multiple instances
           * are given, each should be of a different process type as indicated by the child element. [Minimum occurrences: 0]
           * [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::SupportedMissionPlanCommandsTaskAllocationMissionPlanProcessType, uci::type::accessorType::supportedMissionPlanCommandsTaskAllocationMissionPlanProcessType> MissionPlanProcess;

         /** Provides a list of task allocation functions which can be invoked via the MissionPlanCommand...TaskAllocation
           * command. An invocation of a task allocation function will not result in continuation with additional steps in a
           * mission plan process. If multiple instances are given, each should be of a different function type as indicated by
           * the child element. [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::SupportedMissionPlanCommandsTaskAllocationFunctionType, uci::type::accessorType::supportedMissionPlanCommandsTaskAllocationFunctionType> Function;

         /** Returns the bounded list that is identified by the MissionPlanProcess.
           *
           * @return The bounded list identified by MissionPlanProcess.
           */
         virtual const uci::type::SupportedMissionPlanCommandsTaskAllocationType::MissionPlanProcess& getMissionPlanProcess() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanProcess.
           *
           * @return The bounded list identified by MissionPlanProcess.
           */
         virtual uci::type::SupportedMissionPlanCommandsTaskAllocationType::MissionPlanProcess& getMissionPlanProcess()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanProcess.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanProcess(const uci::type::SupportedMissionPlanCommandsTaskAllocationType::MissionPlanProcess& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Function.
           *
           * @return The bounded list identified by Function.
           */
         virtual const uci::type::SupportedMissionPlanCommandsTaskAllocationType::Function& getFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Function.
           *
           * @return The bounded list identified by Function.
           */
         virtual uci::type::SupportedMissionPlanCommandsTaskAllocationType::Function& getFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Function.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFunction(const uci::type::SupportedMissionPlanCommandsTaskAllocationType::Function& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SupportedMissionPlanCommandsTaskAllocationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SupportedMissionPlanCommandsTaskAllocationType to copy from
           */
         SupportedMissionPlanCommandsTaskAllocationType(const SupportedMissionPlanCommandsTaskAllocationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SupportedMissionPlanCommandsTaskAllocationType to the contents of
           * the SupportedMissionPlanCommandsTaskAllocationType on the right hand side (rhs) of the assignment
           * operator.SupportedMissionPlanCommandsTaskAllocationType [only available to derived classes]
           *
           * @param rhs The SupportedMissionPlanCommandsTaskAllocationType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::SupportedMissionPlanCommandsTaskAllocationType
           * @return A constant reference to this SupportedMissionPlanCommandsTaskAllocationType.
           */
         const SupportedMissionPlanCommandsTaskAllocationType& operator=(const SupportedMissionPlanCommandsTaskAllocationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SupportedMissionPlanCommandsTaskAllocationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SupportedMissionPlanCommandsTaskAllocationType_h

