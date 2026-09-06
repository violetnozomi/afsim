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
#ifndef Uci__Type__StoreVerificationStatusType_h
#define Uci__Type__StoreVerificationStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MessageDataVerificationEnum_h)
# include "uci/type/MessageDataVerificationEnum.h"
#endif

#if !defined(Uci__Type__CannotComplyType_h)
# include "uci/type/CannotComplyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the verification state for this Capability */
      class StoreVerificationStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StoreVerificationStatusType()
         { }

         /** Returns this accessor's type constant, i.e. StoreVerificationStatusType
           *
           * @return This accessor's type constant, i.e. StoreVerificationStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeVerificationStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreVerificationStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the VerificationState.
           *
           * @return The value of the enumeration identified by VerificationState.
           */
         virtual const uci::type::MessageDataVerificationEnum& getVerificationState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the VerificationState.
           *
           * @return The value of the enumeration identified by VerificationState.
           */
         virtual uci::type::MessageDataVerificationEnum& getVerificationState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the VerificationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setVerificationState(const uci::type::MessageDataVerificationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the VerificationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setVerificationState(uci::type::MessageDataVerificationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VerificationStateReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by VerificationStateReason.
           */
         virtual const uci::type::CannotComplyType& getVerificationStateReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VerificationStateReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by VerificationStateReason.
           */
         virtual uci::type::CannotComplyType& getVerificationStateReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VerificationStateReason to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VerificationStateReason
           */
         virtual void setVerificationStateReason(const uci::type::CannotComplyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VerificationStateReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VerificationStateReason is emabled or not
           */
         virtual bool hasVerificationStateReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VerificationStateReason
           *
           * @param type = uci::type::accessorType::cannotComplyType This Accessor's accessor type
           */
         virtual void enableVerificationStateReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::cannotComplyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VerificationStateReason */
         virtual void clearVerificationStateReason()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreVerificationStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreVerificationStatusType to copy from
           */
         StoreVerificationStatusType(const StoreVerificationStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreVerificationStatusType to the contents of the
           * StoreVerificationStatusType on the right hand side (rhs) of the assignment operator.StoreVerificationStatusType [only
           * available to derived classes]
           *
           * @param rhs The StoreVerificationStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StoreVerificationStatusType
           * @return A constant reference to this StoreVerificationStatusType.
           */
         const StoreVerificationStatusType& operator=(const StoreVerificationStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreVerificationStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreVerificationStatusType_h

