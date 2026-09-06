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
#ifndef Uci__Type__SignalEmitterKinematicsFixedType_h
#define Uci__Type__SignalEmitterKinematicsFixedType_h 1

#if !defined(Uci__Type__FixedPositionType_h)
# include "uci/type/FixedPositionType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SignalEmitterKinematicsFixedGeolocationStatusType_h)
# include "uci/type/SignalEmitterKinematicsFixedGeolocationStatusType.h"
#endif

#if !defined(Uci__Type__EW_FixEnum_h)
# include "uci/type/EW_FixEnum.h"
#endif

#if !defined(Uci__Type__SquareCircleSwitchEnum_h)
# include "uci/type/SquareCircleSwitchEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalEmitterKinematicsFixedType sequence accessor class */
      class SignalEmitterKinematicsFixedType : public virtual uci::type::FixedPositionType {
      public:

         /** The destructor */
         virtual ~SignalEmitterKinematicsFixedType()
         { }

         /** Returns this accessor's type constant, i.e. SignalEmitterKinematicsFixedType
           *
           * @return This accessor's type constant, i.e. SignalEmitterKinematicsFixedType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalEmitterKinematicsFixedType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalEmitterKinematicsFixedType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the status of a method of geolocation used to determine the emitter position. Multiple instances allows the
           * last state of multiple methods to be reported. If multiple instances are given, each should be of a different
           * geolocation method as indicated by the child element. [Minimum occurrences: 0] [Maximum occurrences: 10]
           */
         typedef uci::base::BoundedList<uci::type::SignalEmitterKinematicsFixedGeolocationStatusType, uci::type::accessorType::signalEmitterKinematicsFixedGeolocationStatusType> GeolocationStatus;

         /** Returns the bounded list that is identified by the GeolocationStatus.
           *
           * @return The bounded list identified by GeolocationStatus.
           */
         virtual const uci::type::SignalEmitterKinematicsFixedType::GeolocationStatus& getGeolocationStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeolocationStatus.
           *
           * @return The bounded list identified by GeolocationStatus.
           */
         virtual uci::type::SignalEmitterKinematicsFixedType::GeolocationStatus& getGeolocationStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeolocationStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeolocationStatus(const uci::type::SignalEmitterKinematicsFixedType::GeolocationStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EW_FixType.
           *
           * @return The value of the enumeration identified by EW_FixType.
           */
         virtual const uci::type::EW_FixEnum& getEW_FixType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EW_FixType.
           *
           * @return The value of the enumeration identified by EW_FixType.
           */
         virtual uci::type::EW_FixEnum& getEW_FixType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EW_FixType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEW_FixType(const uci::type::EW_FixEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EW_FixType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEW_FixType(uci::type::EW_FixEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EW_FixTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEW_FixType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEW_FixType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::eW_FixEnum This Accessor's accessor type
           */
         virtual void enableEW_FixType(uci::base::accessorType::AccessorType type = uci::type::accessorType::eW_FixEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEW_FixType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SquareCircleSwitch.
           *
           * @return The value of the enumeration identified by SquareCircleSwitch.
           */
         virtual const uci::type::SquareCircleSwitchEnum& getSquareCircleSwitch() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SquareCircleSwitch.
           *
           * @return The value of the enumeration identified by SquareCircleSwitch.
           */
         virtual uci::type::SquareCircleSwitchEnum& getSquareCircleSwitch()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SquareCircleSwitch.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSquareCircleSwitch(const uci::type::SquareCircleSwitchEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SquareCircleSwitch.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSquareCircleSwitch(uci::type::SquareCircleSwitchEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SquareCircleSwitchis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSquareCircleSwitch() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSquareCircleSwitch will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::squareCircleSwitchEnum This Accessor's accessor type
           */
         virtual void enableSquareCircleSwitch(uci::base::accessorType::AccessorType type = uci::type::accessorType::squareCircleSwitchEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSquareCircleSwitch()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalEmitterKinematicsFixedType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalEmitterKinematicsFixedType to copy from
           */
         SignalEmitterKinematicsFixedType(const SignalEmitterKinematicsFixedType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalEmitterKinematicsFixedType to the contents of the
           * SignalEmitterKinematicsFixedType on the right hand side (rhs) of the assignment
           * operator.SignalEmitterKinematicsFixedType [only available to derived classes]
           *
           * @param rhs The SignalEmitterKinematicsFixedType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SignalEmitterKinematicsFixedType
           * @return A constant reference to this SignalEmitterKinematicsFixedType.
           */
         const SignalEmitterKinematicsFixedType& operator=(const SignalEmitterKinematicsFixedType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalEmitterKinematicsFixedType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalEmitterKinematicsFixedType_h

