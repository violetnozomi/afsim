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
#ifndef Uci__Type__RefuelActivityType_h
#define Uci__Type__RefuelActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__RefuelActivityStatusType_h)
# include "uci/type/RefuelActivityStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelActivityType sequence accessor class */
      class RefuelActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~RefuelActivityType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelActivityType
           *
           * @return This accessor's type constant, i.e. RefuelActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DispenserActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by DispenserActivity.
           */
         virtual const uci::type::RefuelActivityStatusType& getDispenserActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DispenserActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by DispenserActivity.
           */
         virtual uci::type::RefuelActivityStatusType& getDispenserActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DispenserActivity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DispenserActivity
           */
         virtual void setDispenserActivity(const uci::type::RefuelActivityStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DispenserActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DispenserActivity is emabled or not
           */
         virtual bool hasDispenserActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DispenserActivity
           *
           * @param type = uci::type::accessorType::refuelActivityStatusType This Accessor's accessor type
           */
         virtual void enableDispenserActivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::refuelActivityStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DispenserActivity */
         virtual void clearDispenserActivity()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelActivityType to copy from
           */
         RefuelActivityType(const RefuelActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelActivityType to the contents of the RefuelActivityType on
           * the right hand side (rhs) of the assignment operator.RefuelActivityType [only available to derived classes]
           *
           * @param rhs The RefuelActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RefuelActivityType
           * @return A constant reference to this RefuelActivityType.
           */
         const RefuelActivityType& operator=(const RefuelActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelActivityType_h

