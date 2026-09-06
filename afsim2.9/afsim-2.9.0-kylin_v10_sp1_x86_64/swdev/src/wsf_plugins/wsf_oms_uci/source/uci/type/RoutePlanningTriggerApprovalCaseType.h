// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RoutePlanningTriggerApprovalCaseType_h
#define Uci__Type__RoutePlanningTriggerApprovalCaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanningTriggerType_h)
# include "uci/type/MissionPlanningTriggerType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RoutePlanningApprovalPolicyType_h)
# include "uci/type/RoutePlanningApprovalPolicyType.h"
#endif

#if !defined(Uci__Type__RoutePlanningResultCaseType_h)
# include "uci/type/RoutePlanningResultCaseType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RoutePlanningTriggerApprovalCaseType sequence accessor class */
      class RoutePlanningTriggerApprovalCaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RoutePlanningTriggerApprovalCaseType()
         { }

         /** Returns this accessor's type constant, i.e. RoutePlanningTriggerApprovalCaseType
           *
           * @return This accessor's type constant, i.e. RoutePlanningTriggerApprovalCaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routePlanningTriggerApprovalCaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RoutePlanningTriggerApprovalCaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element specifies the default approval policy for the trigger. This element can be further overridden based on
           * the outcome of the reroute utilizing the TriggerApprovalPolicyByResultCase element. [Minimum occurrences: 0] [Maximum
           * occurrences: 4]
           */
         typedef uci::base::BoundedList<uci::type::RoutePlanningApprovalPolicyType, uci::type::accessorType::routePlanningApprovalPolicyType> DefaultPolicy;

         /** This element allows the default trigger approval policy to be overridden based on the route planning result. If
           * multiple instances are given, each should be of a different result type as indicated by the child element. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RoutePlanningResultCaseType, uci::type::accessorType::routePlanningResultCaseType> ByResultCase;

         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual const uci::type::MissionPlanningTriggerType& getTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual uci::type::MissionPlanningTriggerType& getTrigger()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Trigger to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Trigger
           */
         virtual void setTrigger(const uci::type::MissionPlanningTriggerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DefaultPolicy.
           *
           * @return The bounded list identified by DefaultPolicy.
           */
         virtual const uci::type::RoutePlanningTriggerApprovalCaseType::DefaultPolicy& getDefaultPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DefaultPolicy.
           *
           * @return The bounded list identified by DefaultPolicy.
           */
         virtual uci::type::RoutePlanningTriggerApprovalCaseType::DefaultPolicy& getDefaultPolicy()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DefaultPolicy.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDefaultPolicy(const uci::type::RoutePlanningTriggerApprovalCaseType::DefaultPolicy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByResultCase.
           *
           * @return The bounded list identified by ByResultCase.
           */
         virtual const uci::type::RoutePlanningTriggerApprovalCaseType::ByResultCase& getByResultCase() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByResultCase.
           *
           * @return The bounded list identified by ByResultCase.
           */
         virtual uci::type::RoutePlanningTriggerApprovalCaseType::ByResultCase& getByResultCase()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ByResultCase.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setByResultCase(const uci::type::RoutePlanningTriggerApprovalCaseType::ByResultCase& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RoutePlanningTriggerApprovalCaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RoutePlanningTriggerApprovalCaseType to copy from
           */
         RoutePlanningTriggerApprovalCaseType(const RoutePlanningTriggerApprovalCaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RoutePlanningTriggerApprovalCaseType to the contents of the
           * RoutePlanningTriggerApprovalCaseType on the right hand side (rhs) of the assignment
           * operator.RoutePlanningTriggerApprovalCaseType [only available to derived classes]
           *
           * @param rhs The RoutePlanningTriggerApprovalCaseType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::RoutePlanningTriggerApprovalCaseType
           * @return A constant reference to this RoutePlanningTriggerApprovalCaseType.
           */
         const RoutePlanningTriggerApprovalCaseType& operator=(const RoutePlanningTriggerApprovalCaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RoutePlanningTriggerApprovalCaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RoutePlanningTriggerApprovalCaseType_h

