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
#ifndef Uci__Type__OperatorActionControlledType_h
#define Uci__Type__OperatorActionControlledType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OperatorActionType_h)
# include "uci/type/OperatorActionType.h"
#endif

#if !defined(Uci__Type__OperatorRoleType_h)
# include "uci/type/OperatorRoleType.h"
#endif

#if !defined(Uci__Type__OperatorDefaultActionType_h)
# include "uci/type/OperatorDefaultActionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorActionControlledType sequence accessor class */
      class OperatorActionControlledType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorActionControlledType()
         { }

         /** Returns this accessor's type constant, i.e. OperatorActionControlledType
           *
           * @return This accessor's type constant, i.e. OperatorActionControlledType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorActionControlledType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorActionControlledType& accessor)
            throw(uci::base::UCIException) = 0;


         /** List of actions the originator of the notification may take. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::OperatorActionType, uci::type::accessorType::operatorActionType> ActionOptions;

         /** Returns the bounded list that is identified by the ActionOptions.
           *
           * @return The bounded list identified by ActionOptions.
           */
         virtual const uci::type::OperatorActionControlledType::ActionOptions& getActionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActionOptions.
           *
           * @return The bounded list identified by ActionOptions.
           */
         virtual uci::type::OperatorActionControlledType::ActionOptions& getActionOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActionOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActionOptions(const uci::type::OperatorActionControlledType::ActionOptions& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Destinations.
           *
           * @return The acecssor that provides access to the complex content that is identified by Destinations.
           */
         virtual const uci::type::OperatorRoleType& getDestinations() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Destinations.
           *
           * @return The acecssor that provides access to the complex content that is identified by Destinations.
           */
         virtual uci::type::OperatorRoleType& getDestinations()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Destinations to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Destinations
           */
         virtual void setDestinations(const uci::type::OperatorRoleType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Destinations exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Destinations is emabled or not
           */
         virtual bool hasDestinations() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Destinations
           *
           * @param type = uci::type::accessorType::operatorRoleType This Accessor's accessor type
           */
         virtual void enableDestinations(uci::base::accessorType::AccessorType type = uci::type::accessorType::operatorRoleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Destinations */
         virtual void clearDestinations()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultAction.
           */
         virtual const uci::type::OperatorDefaultActionType& getDefaultAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultAction.
           */
         virtual uci::type::OperatorDefaultActionType& getDefaultAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultAction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultAction
           */
         virtual void setDefaultAction(const uci::type::OperatorDefaultActionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultAction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultAction is emabled or not
           */
         virtual bool hasDefaultAction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultAction
           *
           * @param type = uci::type::accessorType::operatorDefaultActionType This Accessor's accessor type
           */
         virtual void enableDefaultAction(uci::base::accessorType::AccessorType type = uci::type::accessorType::operatorDefaultActionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultAction */
         virtual void clearDefaultAction()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorActionControlledType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorActionControlledType to copy from
           */
         OperatorActionControlledType(const OperatorActionControlledType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorActionControlledType to the contents of the
           * OperatorActionControlledType on the right hand side (rhs) of the assignment operator.OperatorActionControlledType
           * [only available to derived classes]
           *
           * @param rhs The OperatorActionControlledType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::OperatorActionControlledType
           * @return A constant reference to this OperatorActionControlledType.
           */
         const OperatorActionControlledType& operator=(const OperatorActionControlledType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorActionControlledType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorActionControlledType_h

