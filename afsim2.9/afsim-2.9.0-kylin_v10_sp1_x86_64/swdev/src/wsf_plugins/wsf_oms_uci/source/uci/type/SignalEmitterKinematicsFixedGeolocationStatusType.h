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
#ifndef Uci__Type__SignalEmitterKinematicsFixedGeolocationStatusType_h
#define Uci__Type__SignalEmitterKinematicsFixedGeolocationStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SignalGeolocationMethodEnum_h)
# include "uci/type/SignalGeolocationMethodEnum.h"
#endif

#if !defined(Uci__Type__SignalGeolocationControlEnum_h)
# include "uci/type/SignalGeolocationControlEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalEmitterKinematicsFixedGeolocationStatusType sequence accessor class */
      class SignalEmitterKinematicsFixedGeolocationStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalEmitterKinematicsFixedGeolocationStatusType()
         { }

         /** Returns this accessor's type constant, i.e. SignalEmitterKinematicsFixedGeolocationStatusType
           *
           * @return This accessor's type constant, i.e. SignalEmitterKinematicsFixedGeolocationStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalEmitterKinematicsFixedGeolocationStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalEmitterKinematicsFixedGeolocationStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationMethod.
           *
           * @return The value of the enumeration identified by GeolocationMethod.
           */
         virtual const uci::type::SignalGeolocationMethodEnum& getGeolocationMethod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationMethod.
           *
           * @return The value of the enumeration identified by GeolocationMethod.
           */
         virtual uci::type::SignalGeolocationMethodEnum& getGeolocationMethod()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationMethod.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationMethod(const uci::type::SignalGeolocationMethodEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationMethod.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationMethod(uci::type::SignalGeolocationMethodEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield GeolocationMethodis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasGeolocationMethod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getGeolocationMethod will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::signalGeolocationMethodEnum This Accessor's accessor type
           */
         virtual void enableGeolocationMethod(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalGeolocationMethodEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearGeolocationMethod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @return The value of the enumeration identified by GeolocationControlState.
           */
         virtual const uci::type::SignalGeolocationControlEnum& getGeolocationControlState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @return The value of the enumeration identified by GeolocationControlState.
           */
         virtual uci::type::SignalGeolocationControlEnum& getGeolocationControlState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationControlState(const uci::type::SignalGeolocationControlEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GeolocationControlState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGeolocationControlState(uci::type::SignalGeolocationControlEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield GeolocationControlStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasGeolocationControlState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getGeolocationControlState will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::signalGeolocationControlEnum This Accessor's accessor type
           */
         virtual void enableGeolocationControlState(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalGeolocationControlEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearGeolocationControlState()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalEmitterKinematicsFixedGeolocationStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalEmitterKinematicsFixedGeolocationStatusType to copy from
           */
         SignalEmitterKinematicsFixedGeolocationStatusType(const SignalEmitterKinematicsFixedGeolocationStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalEmitterKinematicsFixedGeolocationStatusType to the contents
           * of the SignalEmitterKinematicsFixedGeolocationStatusType on the right hand side (rhs) of the assignment
           * operator.SignalEmitterKinematicsFixedGeolocationStatusType [only available to derived classes]
           *
           * @param rhs The SignalEmitterKinematicsFixedGeolocationStatusType on the right hand side (rhs) of the assignment
           *      operator whose contents are used to set the contents of this
           *      uci::type::SignalEmitterKinematicsFixedGeolocationStatusType
           * @return A constant reference to this SignalEmitterKinematicsFixedGeolocationStatusType.
           */
         const SignalEmitterKinematicsFixedGeolocationStatusType& operator=(const SignalEmitterKinematicsFixedGeolocationStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalEmitterKinematicsFixedGeolocationStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalEmitterKinematicsFixedGeolocationStatusType_h

