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
#ifndef Uci__Type__RoutePlanningPolicyType_h
#define Uci__Type__RoutePlanningPolicyType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
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

#if !defined(Uci__Type__RoutePlanningTriggerApprovalCaseType_h)
# include "uci/type/RoutePlanningTriggerApprovalCaseType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RoutePlanningPolicyType sequence accessor class */
      class RoutePlanningPolicyType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RoutePlanningPolicyType()
         { }

         /** Returns this accessor's type constant, i.e. RoutePlanningPolicyType
           *
           * @return This accessor's type constant, i.e. RoutePlanningPolicyType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routePlanningPolicyType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RoutePlanningPolicyType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Route plans can be categorized into those that affect "all" Systems, however slightly, and those which affect only
           * those Systems directly related to the cause/trigger of the route plan. This element indicates whether or not approval
           * is required for route plans in each of those categories. Since all allocations fall into one category, this element
           * is considered the "default" route planning approval policy. The default can be overridden in specific conditions as
           * indicated by the ApprovalPolicyBy* siblings of this element. For example, this element could indicate that approval
           * is required for "affected only" route plans and then use ApprovalPolicyBy* siblings to specify conditions when route
           * plan approval is not required. Alternately, this element could indicate that approval is not required by default and
           * then use ApprovalPolicyBy* siblings to specify conditions when route plan approval is required. Creators of this
           * message are expected to carefully avoid creating logically inconsistent ApprovalPolicyBy* instances and therefore no
           * guidance is given on deconflicting illogical instances. For example, an ApprovalPolicy that indicates approval is not
           * required by default with conflicting ApprovalPolicyBy* overrides of 1) AUTO_REJECT for DROPPED_TASKS and 2)
           * AUTO_APPROVE for operator triggered DROPPED_TASKS would be considered illogical. [Minimum occurrences: 0] [Maximum
           * occurrences: 4]
           */
         typedef uci::base::BoundedList<uci::type::RoutePlanningApprovalPolicyType, uci::type::accessorType::routePlanningApprovalPolicyType> DefaultPolicy;

         /** Indicates whether or not approval is required when route planning results match certain conditions. Conditions
           * specified here are considered to be overrides to the default route planning approval policy given in the sibling
           * Default element. For example, if the default policy indicates route planning approval is never required, this element
           * could override the default and require approval whenever route planning results in previously allocated Task(s) being
           * dropped. See the annotations in the sibling Default element for an overview of how the siblings relate to each other.
           * If multiple instances are given, each should be of a different result type as indicated by the child element.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RoutePlanningResultCaseType, uci::type::accessorType::routePlanningResultCaseType> ByResultCase;

         /** Indicates whether or not approval is required for a given route planning trigger type, overriding the default route
           * planning approval policy given in the sibling Default element. Additionally, this element allows trigger-specific
           * "reroute all" and "reroute affected only" defaults, further overridden "by result". For example, if the default
           * policy indicates route planning approval is never required, this element could override, requiring approval for route
           * plans affecting all Systems triggered by new threats if the route re-plan results in dropped tasks. See the
           * annotations in the sibling Default element for an overview of how the siblings relate to each other. If multiple
           * instances are given, each should be of a different trigger type as indicated by the child element. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RoutePlanningTriggerApprovalCaseType, uci::type::accessorType::routePlanningTriggerApprovalCaseType> ByTriggerCase;

         /** Returns the bounded list that is identified by the DefaultPolicy.
           *
           * @return The bounded list identified by DefaultPolicy.
           */
         virtual const uci::type::RoutePlanningPolicyType::DefaultPolicy& getDefaultPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DefaultPolicy.
           *
           * @return The bounded list identified by DefaultPolicy.
           */
         virtual uci::type::RoutePlanningPolicyType::DefaultPolicy& getDefaultPolicy()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DefaultPolicy.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDefaultPolicy(const uci::type::RoutePlanningPolicyType::DefaultPolicy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByResultCase.
           *
           * @return The bounded list identified by ByResultCase.
           */
         virtual const uci::type::RoutePlanningPolicyType::ByResultCase& getByResultCase() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByResultCase.
           *
           * @return The bounded list identified by ByResultCase.
           */
         virtual uci::type::RoutePlanningPolicyType::ByResultCase& getByResultCase()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ByResultCase.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setByResultCase(const uci::type::RoutePlanningPolicyType::ByResultCase& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByTriggerCase.
           *
           * @return The bounded list identified by ByTriggerCase.
           */
         virtual const uci::type::RoutePlanningPolicyType::ByTriggerCase& getByTriggerCase() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByTriggerCase.
           *
           * @return The bounded list identified by ByTriggerCase.
           */
         virtual uci::type::RoutePlanningPolicyType::ByTriggerCase& getByTriggerCase()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ByTriggerCase.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setByTriggerCase(const uci::type::RoutePlanningPolicyType::ByTriggerCase& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RoutePlanningPolicyType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RoutePlanningPolicyType to copy from
           */
         RoutePlanningPolicyType(const RoutePlanningPolicyType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RoutePlanningPolicyType to the contents of the
           * RoutePlanningPolicyType on the right hand side (rhs) of the assignment operator.RoutePlanningPolicyType [only
           * available to derived classes]
           *
           * @param rhs The RoutePlanningPolicyType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RoutePlanningPolicyType
           * @return A constant reference to this RoutePlanningPolicyType.
           */
         const RoutePlanningPolicyType& operator=(const RoutePlanningPolicyType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RoutePlanningPolicyType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RoutePlanningPolicyType_h

