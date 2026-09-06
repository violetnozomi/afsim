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
#ifndef Uci__Type__RoutePlanningApprovalPolicyType_h
#define Uci__Type__RoutePlanningApprovalPolicyType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RoutePlanningAutonomousRerouteType_h)
# include "uci/type/RoutePlanningAutonomousRerouteType.h"
#endif

#if !defined(Uci__Type__DefaultResponseType_h)
# include "uci/type/DefaultResponseType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Types of Route Planning with associated approval data. */
      class RoutePlanningApprovalPolicyType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RoutePlanningApprovalPolicyType()
         { }

         /** Returns this accessor's type constant, i.e. RoutePlanningApprovalPolicyType
           *
           * @return This accessor's type constant, i.e. RoutePlanningApprovalPolicyType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routePlanningApprovalPolicyType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RoutePlanningApprovalPolicyType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlanningType.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlanningType.
           */
         virtual const uci::type::RoutePlanningAutonomousRerouteType& getPlanningType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlanningType.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlanningType.
           */
         virtual uci::type::RoutePlanningAutonomousRerouteType& getPlanningType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlanningType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlanningType
           */
         virtual void setPlanningType(const uci::type::RoutePlanningAutonomousRerouteType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultResponse.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultResponse.
           */
         virtual const uci::type::DefaultResponseType& getDefaultResponse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultResponse.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultResponse.
           */
         virtual uci::type::DefaultResponseType& getDefaultResponse()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultResponse to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultResponse
           */
         virtual void setDefaultResponse(const uci::type::DefaultResponseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultResponse exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultResponse is emabled or not
           */
         virtual bool hasDefaultResponse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultResponse
           *
           * @param type = uci::type::accessorType::defaultResponseType This Accessor's accessor type
           */
         virtual void enableDefaultResponse(uci::base::accessorType::AccessorType type = uci::type::accessorType::defaultResponseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultResponse */
         virtual void clearDefaultResponse()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RoutePlanningApprovalPolicyType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RoutePlanningApprovalPolicyType to copy from
           */
         RoutePlanningApprovalPolicyType(const RoutePlanningApprovalPolicyType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RoutePlanningApprovalPolicyType to the contents of the
           * RoutePlanningApprovalPolicyType on the right hand side (rhs) of the assignment
           * operator.RoutePlanningApprovalPolicyType [only available to derived classes]
           *
           * @param rhs The RoutePlanningApprovalPolicyType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::RoutePlanningApprovalPolicyType
           * @return A constant reference to this RoutePlanningApprovalPolicyType.
           */
         const RoutePlanningApprovalPolicyType& operator=(const RoutePlanningApprovalPolicyType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RoutePlanningApprovalPolicyType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RoutePlanningApprovalPolicyType_h

