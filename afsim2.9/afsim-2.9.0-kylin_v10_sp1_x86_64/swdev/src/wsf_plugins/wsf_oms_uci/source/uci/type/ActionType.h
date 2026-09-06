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
#ifndef Uci__Type__ActionType_h
#define Uci__Type__ActionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TaskTransitionActionType_h)
# include "uci/type/TaskTransitionActionType.h"
#endif

#if !defined(Uci__Type__VehicleCommandDataType_h)
# include "uci/type/VehicleCommandDataType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__SupportCapabilityCommandBaseType_h)
# include "uci/type/SupportCapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__CommDataType_h)
# include "uci/type/CommDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ActionType choice accessor class */
      class ActionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ActionType()
         { }

         /** Returns this accessor's type constant, i.e. ActionType
           *
           * @return This accessor's type constant, i.e. ActionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::actionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ActionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum ActionTypeChoice {
            ACTIONTYPE_CHOICE_NONE,
            ACTIONTYPE_CHOICE_TASKACTION,
            ACTIONTYPE_CHOICE_VEHICLEACTION,
            ACTIONTYPE_CHOICE_CAPABILITYACTION,
            ACTIONTYPE_CHOICE_SUPPORTINGCAPABILITYACTION,
            ACTIONTYPE_CHOICE_COMMACTION
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual ActionTypeChoice getActionTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getActionTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the access
           * methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setActionTypeChoiceOrdinal(ActionTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAction.
           */
         virtual const uci::type::TaskTransitionActionType& getTaskAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAction.
           */
         virtual uci::type::TaskTransitionActionType& getTaskAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskAction to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskAction
           */
         virtual void setTaskAction(const uci::type::TaskTransitionActionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleAction.
           */
         virtual const uci::type::VehicleCommandDataType& getVehicleAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleAction.
           */
         virtual uci::type::VehicleCommandDataType& getVehicleAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VehicleAction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VehicleAction
           */
         virtual void setVehicleAction(const uci::type::VehicleCommandDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityAction.
           */
         virtual const uci::type::CapabilityCommandBaseType& getCapabilityAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityAction.
           */
         virtual uci::type::CapabilityCommandBaseType& getCapabilityAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityAction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityAction
           */
         virtual void setCapabilityAction(const uci::type::CapabilityCommandBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * SupportingCapabilityAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportingCapabilityAction.
           */
         virtual const uci::type::SupportCapabilityCommandBaseType& getSupportingCapabilityAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * SupportingCapabilityAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportingCapabilityAction.
           */
         virtual uci::type::SupportCapabilityCommandBaseType& getSupportingCapabilityAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportingCapabilityAction to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportingCapabilityAction
           */
         virtual void setSupportingCapabilityAction(const uci::type::SupportCapabilityCommandBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAction.
           */
         virtual const uci::type::CommDataType& getCommAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAction.
           */
         virtual uci::type::CommDataType& getCommAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommAction to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommAction
           */
         virtual void setCommAction(const uci::type::CommDataType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ActionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ActionType to copy from
           */
         ActionType(const ActionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ActionType to the contents of the ActionType on the right hand
           * side (rhs) of the assignment operator.ActionType [only available to derived classes]
           *
           * @param rhs The ActionType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::ActionType
           * @return A constant reference to this ActionType.
           */
         const ActionType& operator=(const ActionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ActionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ActionType_h

