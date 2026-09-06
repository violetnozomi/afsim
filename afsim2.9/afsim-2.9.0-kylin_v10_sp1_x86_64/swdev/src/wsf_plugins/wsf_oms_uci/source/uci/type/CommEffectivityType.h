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
#ifndef Uci__Type__CommEffectivityType_h
#define Uci__Type__CommEffectivityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommEffectivityID_Type_h)
# include "uci/type/CommEffectivityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PlannedCommActionExtendedType_h)
# include "uci/type/PlannedCommActionExtendedType.h"
#endif

#if !defined(Uci__Type__CommPlanTriggerType_h)
# include "uci/type/CommPlanTriggerType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** For a communication plan, this is the collection of Triggers and the communication plan settings to be applied upon
        * these triggers
        */
      class CommEffectivityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommEffectivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommEffectivityType
           *
           * @return This accessor's type constant, i.e. CommEffectivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commEffectivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommEffectivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a list of communication actions/settings to be applied upon the supplied triggers to support a
           * communication plan. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PlannedCommActionExtendedType, uci::type::accessorType::plannedCommActionExtendedType> PlannedCommAction;

         /** Returns the accessor that provides access to the complex content that is identified by the EffectivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EffectivityID.
           */
         virtual const uci::type::CommEffectivityID_Type& getEffectivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EffectivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EffectivityID.
           */
         virtual uci::type::CommEffectivityID_Type& getEffectivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EffectivityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EffectivityID
           */
         virtual void setEffectivityID(const uci::type::CommEffectivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PlannedCommAction.
           *
           * @return The bounded list identified by PlannedCommAction.
           */
         virtual const uci::type::CommEffectivityType::PlannedCommAction& getPlannedCommAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PlannedCommAction.
           *
           * @return The bounded list identified by PlannedCommAction.
           */
         virtual uci::type::CommEffectivityType::PlannedCommAction& getPlannedCommAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PlannedCommAction.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlannedCommAction(const uci::type::CommEffectivityType::PlannedCommAction& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual const uci::type::CommPlanTriggerType& getTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Trigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by Trigger.
           */
         virtual uci::type::CommPlanTriggerType& getTrigger()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Trigger to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Trigger
           */
         virtual void setTrigger(const uci::type::CommPlanTriggerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Trigger exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Trigger is emabled or not
           */
         virtual bool hasTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Trigger
           *
           * @param type = uci::type::accessorType::commPlanTriggerType This Accessor's accessor type
           */
         virtual void enableTrigger(uci::base::accessorType::AccessorType type = uci::type::accessorType::commPlanTriggerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Trigger */
         virtual void clearTrigger()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommEffectivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommEffectivityType to copy from
           */
         CommEffectivityType(const CommEffectivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommEffectivityType to the contents of the CommEffectivityType on
           * the right hand side (rhs) of the assignment operator.CommEffectivityType [only available to derived classes]
           *
           * @param rhs The CommEffectivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommEffectivityType
           * @return A constant reference to this CommEffectivityType.
           */
         const CommEffectivityType& operator=(const CommEffectivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommEffectivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommEffectivityType_h

