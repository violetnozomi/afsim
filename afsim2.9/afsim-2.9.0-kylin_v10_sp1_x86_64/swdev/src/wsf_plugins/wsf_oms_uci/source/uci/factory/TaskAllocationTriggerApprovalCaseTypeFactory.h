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
#ifndef Uci__Factory__TaskAllocationTriggerApprovalCaseTypeFactory_h
#define Uci__Factory__TaskAllocationTriggerApprovalCaseTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__TaskAllocationTriggerApprovalCaseType_h)
# include "uci/type/TaskAllocationTriggerApprovalCaseType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class TaskAllocationTriggerApprovalCaseTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new TaskAllocationTriggerApprovalCaseTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::TaskAllocationTriggerApprovalCaseTypeFactory* uci_getTaskAllocationTriggerApprovalCaseTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a TaskAllocationTriggerApprovalCaseTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyTaskAllocationTriggerApprovalCaseTypeFactory(uci::factory::TaskAllocationTriggerApprovalCaseTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class TaskAllocationTriggerApprovalCaseTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~TaskAllocationTriggerApprovalCaseTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::TaskAllocationTriggerApprovalCaseTypeFactory FactoryType;
         typedef  uci::type::TaskAllocationTriggerApprovalCaseType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::TaskAllocationTriggerApprovalCaseTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getTaskAllocationTriggerApprovalCaseTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::TaskAllocationTriggerApprovalCaseTypeFactory* factory )
         {
            uci_destroyTaskAllocationTriggerApprovalCaseTypeFactory( factory );
         }

         /** This method constructs a new TaskAllocationTriggerApprovalCaseType message, returning an accessor to that message
           * that can be used to access that message.
           *
           * @return A message accessor that can be used to access the newly created TaskAllocationTriggerApprovalCaseType message
           */
         virtual uci::type::TaskAllocationTriggerApprovalCaseType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new TaskAllocationTriggerApprovalCaseType message, returning an accessor to that message
           * that can be used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created TaskAllocationTriggerApprovalCaseType message
           */
         virtual uci::type::TaskAllocationTriggerApprovalCaseType& create(const uci::type::TaskAllocationTriggerApprovalCaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a TaskAllocationTriggerApprovalCaseType message accessor, along with the message that the
           * accessor was providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::TaskAllocationTriggerApprovalCaseType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskAllocationTriggerApprovalCaseTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskAllocationTriggerApprovalCaseTypeFactory to copy from
           */
         TaskAllocationTriggerApprovalCaseTypeFactory(const TaskAllocationTriggerApprovalCaseTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskAllocationTriggerApprovalCaseTypeFactory to the contents of
           * the TaskAllocationTriggerApprovalCaseTypeFactory on the right hand side (rhs) of the assignment
           * operator.TaskAllocationTriggerApprovalCaseTypeFactory [only available to derived classes]
           *
           * @param rhs The TaskAllocationTriggerApprovalCaseTypeFactory on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::factory::TaskAllocationTriggerApprovalCaseTypeFactory
           * @return A constant reference to this TaskAllocationTriggerApprovalCaseTypeFactory.
           */
         const TaskAllocationTriggerApprovalCaseTypeFactory& operator=(const TaskAllocationTriggerApprovalCaseTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskAllocationTriggerApprovalCaseTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__TaskAllocationTriggerApprovalCaseTypeFactory_h

