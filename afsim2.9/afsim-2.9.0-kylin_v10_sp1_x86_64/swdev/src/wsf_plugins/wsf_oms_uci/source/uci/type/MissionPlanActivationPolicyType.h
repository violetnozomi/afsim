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
#ifndef Uci__Type__MissionPlanActivationPolicyType_h
#define Uci__Type__MissionPlanActivationPolicyType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DefaultResponseType_h)
# include "uci/type/DefaultResponseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionPlanCommandApprovalType_h)
# include "uci/type/MissionPlanCommandApprovalType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanActivationPolicyType sequence accessor class */
      class MissionPlanActivationPolicyType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanActivationPolicyType()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanActivationPolicyType
           *
           * @return This accessor's type constant, i.e. MissionPlanActivationPolicyType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanActivationPolicyType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanActivationPolicyType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Specifies mission plan activation command type and associated approval policy. [Minimum occurrences: 0] [Maximum
           * occurrences: 5]
           */
         typedef uci::base::BoundedList<uci::type::MissionPlanCommandApprovalType, uci::type::accessorType::missionPlanCommandApprovalType> ByCommand;

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


         /** Returns the bounded list that is identified by the ByCommand.
           *
           * @return The bounded list identified by ByCommand.
           */
         virtual const uci::type::MissionPlanActivationPolicyType::ByCommand& getByCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByCommand.
           *
           * @return The bounded list identified by ByCommand.
           */
         virtual uci::type::MissionPlanActivationPolicyType::ByCommand& getByCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ByCommand.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setByCommand(const uci::type::MissionPlanActivationPolicyType::ByCommand& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanActivationPolicyType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationPolicyType to copy from
           */
         MissionPlanActivationPolicyType(const MissionPlanActivationPolicyType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanActivationPolicyType to the contents of the
           * MissionPlanActivationPolicyType on the right hand side (rhs) of the assignment
           * operator.MissionPlanActivationPolicyType [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationPolicyType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MissionPlanActivationPolicyType
           * @return A constant reference to this MissionPlanActivationPolicyType.
           */
         const MissionPlanActivationPolicyType& operator=(const MissionPlanActivationPolicyType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanActivationPolicyType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanActivationPolicyType_h

