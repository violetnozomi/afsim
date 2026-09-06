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
#ifndef Uci__Type__SignalAmplitudeType_h
#define Uci__Type__SignalAmplitudeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalAmplitudeType sequence accessor class */
      class SignalAmplitudeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalAmplitudeType()
         { }

         /** Returns this accessor's type constant, i.e. SignalAmplitudeType
           *
           * @return This accessor's type constant, i.e. SignalAmplitudeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalAmplitudeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalAmplitudeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AmplitudeAverage.
           *
           * @return The value of the simple primitive data type identified by AmplitudeAverage.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getAmplitudeAverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AmplitudeAverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmplitudeAverage(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AmplitudeAverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AmplitudeAverage is emabled or not
           */
         virtual bool hasAmplitudeAverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AmplitudeAverage
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableAmplitudeAverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AmplitudeAverage */
         virtual void clearAmplitudeAverage()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AmplitudeMin.
           *
           * @return The value of the simple primitive data type identified by AmplitudeMin.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getAmplitudeMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AmplitudeMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmplitudeMin(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AmplitudeMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AmplitudeMin is emabled or not
           */
         virtual bool hasAmplitudeMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AmplitudeMin
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableAmplitudeMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AmplitudeMin */
         virtual void clearAmplitudeMin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AmplitudeMax.
           *
           * @return The value of the simple primitive data type identified by AmplitudeMax.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getAmplitudeMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AmplitudeMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmplitudeMax(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AmplitudeMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AmplitudeMax is emabled or not
           */
         virtual bool hasAmplitudeMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AmplitudeMax
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableAmplitudeMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AmplitudeMax */
         virtual void clearAmplitudeMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AmplitudeLast.
           *
           * @return The value of the simple primitive data type identified by AmplitudeLast.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getAmplitudeLast() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AmplitudeLast.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmplitudeLast(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AmplitudeLast exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AmplitudeLast is emabled or not
           */
         virtual bool hasAmplitudeLast() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AmplitudeLast
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableAmplitudeLast(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AmplitudeLast */
         virtual void clearAmplitudeLast()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RelativeAmplitude.
           *
           * @return The value of the simple primitive data type identified by RelativeAmplitude.
           */
         virtual uci::type::DecibelTypeValue getRelativeAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RelativeAmplitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRelativeAmplitude(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RelativeAmplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RelativeAmplitude is emabled or not
           */
         virtual bool hasRelativeAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RelativeAmplitude
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableRelativeAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RelativeAmplitude */
         virtual void clearRelativeAmplitude()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalAmplitudeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalAmplitudeType to copy from
           */
         SignalAmplitudeType(const SignalAmplitudeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalAmplitudeType to the contents of the SignalAmplitudeType on
           * the right hand side (rhs) of the assignment operator.SignalAmplitudeType [only available to derived classes]
           *
           * @param rhs The SignalAmplitudeType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SignalAmplitudeType
           * @return A constant reference to this SignalAmplitudeType.
           */
         const SignalAmplitudeType& operator=(const SignalAmplitudeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalAmplitudeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalAmplitudeType_h

