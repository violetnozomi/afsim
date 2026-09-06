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
#ifndef Uci__Type__OrientationRateType_h
#define Uci__Type__OrientationRateType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AngleRateType_h)
# include "uci/type/AngleRateType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the rate of change of the orientation. */
      class OrientationRateType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrientationRateType()
         { }

         /** Returns this accessor's type constant, i.e. OrientationRateType
           *
           * @return This accessor's type constant, i.e. OrientationRateType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orientationRateType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrientationRateType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the YawRate.
           *
           * @return The value of the simple primitive data type identified by YawRate.
           */
         virtual uci::type::AngleRateTypeValue getYawRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the YawRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setYawRate(uci::type::AngleRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PitchRate.
           *
           * @return The value of the simple primitive data type identified by PitchRate.
           */
         virtual uci::type::AngleRateTypeValue getPitchRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PitchRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPitchRate(uci::type::AngleRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RollRate.
           *
           * @return The value of the simple primitive data type identified by RollRate.
           */
         virtual uci::type::AngleRateTypeValue getRollRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RollRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRollRate(uci::type::AngleRateTypeValue value)
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
         OrientationRateType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrientationRateType to copy from
           */
         OrientationRateType(const OrientationRateType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrientationRateType to the contents of the OrientationRateType on
           * the right hand side (rhs) of the assignment operator.OrientationRateType [only available to derived classes]
           *
           * @param rhs The OrientationRateType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OrientationRateType
           * @return A constant reference to this OrientationRateType.
           */
         const OrientationRateType& operator=(const OrientationRateType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrientationRateType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrientationRateType_h

