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
#ifndef Uci__Type__AirDataType_h
#define Uci__Type__AirDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__BaroAltitudeType_h)
# include "uci/type/BaroAltitudeType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

#if !defined(Uci__Type__AngleHalfType_h)
# include "uci/type/AngleHalfType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AirDataType sequence accessor class */
      class AirDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AirDataType()
         { }

         /** Returns this accessor's type constant, i.e. AirDataType
           *
           * @return This accessor's type constant, i.e. AirDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::airDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AirDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IndicatedBaroAltitude.
           *
           * @return The value of the simple primitive data type identified by IndicatedBaroAltitude.
           */
         virtual uci::type::BaroAltitudeTypeValue getIndicatedBaroAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IndicatedBaroAltitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIndicatedBaroAltitude(uci::type::BaroAltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Kollsman.
           *
           * @return The value of the simple primitive data type identified by Kollsman.
           */
         virtual xs::Double getKollsman() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Kollsman.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setKollsman(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TrueAirspeed.
           *
           * @return The value of the simple primitive data type identified by TrueAirspeed.
           */
         virtual uci::type::SpeedTypeValue getTrueAirspeed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TrueAirspeed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrueAirspeed(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Alpha.
           *
           * @return The value of the simple primitive data type identified by Alpha.
           */
         virtual uci::type::AngleHalfTypeValue getAlpha() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Alpha.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAlpha(uci::type::AngleHalfTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Alpha exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Alpha is emabled or not
           */
         virtual bool hasAlpha() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Alpha
           *
           * @param type = uci::type::accessorType::angleHalfType This Accessor's accessor type
           */
         virtual void enableAlpha(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleHalfType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Alpha */
         virtual void clearAlpha()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Beta.
           *
           * @return The value of the simple primitive data type identified by Beta.
           */
         virtual uci::type::AngleHalfTypeValue getBeta() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Beta.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeta(uci::type::AngleHalfTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Beta exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Beta is emabled or not
           */
         virtual bool hasBeta() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Beta
           *
           * @param type = uci::type::accessorType::angleHalfType This Accessor's accessor type
           */
         virtual void enableBeta(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleHalfType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Beta */
         virtual void clearBeta()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Timestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Timestamp is emabled or not
           */
         virtual bool hasTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Timestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Timestamp */
         virtual void clearTimestamp()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AirDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AirDataType to copy from
           */
         AirDataType(const AirDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AirDataType to the contents of the AirDataType on the right hand
           * side (rhs) of the assignment operator.AirDataType [only available to derived classes]
           *
           * @param rhs The AirDataType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::AirDataType
           * @return A constant reference to this AirDataType.
           */
         const AirDataType& operator=(const AirDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AirDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AirDataType_h

