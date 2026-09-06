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
#ifndef Uci__Type__CommTerminalPlanEffectivityStateType_h
#define Uci__Type__CommTerminalPlanEffectivityStateType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommEffectivityID_Type_h)
# include "uci/type/CommEffectivityID_Type.h"
#endif

#if !defined(Uci__Type__CommTerminalPlanConfigStateEnum_h)
# include "uci/type/CommTerminalPlanConfigStateEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommTerminalPlanConfigurationStateType_h)
# include "uci/type/CommTerminalPlanConfigurationStateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class CommTerminalPlanEffectivityStateType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommTerminalPlanEffectivityStateType()
         { }

         /** Returns this accessor's type constant, i.e. CommTerminalPlanEffectivityStateType
           *
           * @return This accessor's type constant, i.e. CommTerminalPlanEffectivityStateType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commTerminalPlanEffectivityStateType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommTerminalPlanEffectivityStateType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The Configuration states owned by this CommTerminalPlan Effectivity [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommTerminalPlanConfigurationStateType, uci::type::accessorType::commTerminalPlanConfigurationStateType> EffectivityConfiguration;

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


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual const uci::type::CommTerminalPlanConfigStateEnum& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual uci::type::CommTerminalPlanConfigStateEnum& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(const uci::type::CommTerminalPlanConfigStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(uci::type::CommTerminalPlanConfigStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectivityConfiguration.
           *
           * @return The bounded list identified by EffectivityConfiguration.
           */
         virtual const uci::type::CommTerminalPlanEffectivityStateType::EffectivityConfiguration& getEffectivityConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectivityConfiguration.
           *
           * @return The bounded list identified by EffectivityConfiguration.
           */
         virtual uci::type::CommTerminalPlanEffectivityStateType::EffectivityConfiguration& getEffectivityConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EffectivityConfiguration.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEffectivityConfiguration(const uci::type::CommTerminalPlanEffectivityStateType::EffectivityConfiguration& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommTerminalPlanEffectivityStateType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommTerminalPlanEffectivityStateType to copy from
           */
         CommTerminalPlanEffectivityStateType(const CommTerminalPlanEffectivityStateType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommTerminalPlanEffectivityStateType to the contents of the
           * CommTerminalPlanEffectivityStateType on the right hand side (rhs) of the assignment
           * operator.CommTerminalPlanEffectivityStateType [only available to derived classes]
           *
           * @param rhs The CommTerminalPlanEffectivityStateType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommTerminalPlanEffectivityStateType
           * @return A constant reference to this CommTerminalPlanEffectivityStateType.
           */
         const CommTerminalPlanEffectivityStateType& operator=(const CommTerminalPlanEffectivityStateType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommTerminalPlanEffectivityStateType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommTerminalPlanEffectivityStateType_h

