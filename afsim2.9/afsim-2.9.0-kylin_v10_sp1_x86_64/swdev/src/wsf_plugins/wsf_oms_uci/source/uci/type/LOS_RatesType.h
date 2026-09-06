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
#ifndef Uci__Type__LOS_RatesType_h
#define Uci__Type__LOS_RatesType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AngleRateType_h)
# include "uci/type/AngleRateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LOS_RatesType sequence accessor class */
      class LOS_RatesType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LOS_RatesType()
         { }

         /** Returns this accessor's type constant, i.e. LOS_RatesType
           *
           * @return This accessor's type constant, i.e. LOS_RatesType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lOS_RatesType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LOS_RatesType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AzimuthRate.
           *
           * @return The value of the simple primitive data type identified by AzimuthRate.
           */
         virtual uci::type::AngleRateTypeValue getAzimuthRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AzimuthRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAzimuthRate(uci::type::AngleRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ElevationRate.
           *
           * @return The value of the simple primitive data type identified by ElevationRate.
           */
         virtual uci::type::AngleRateTypeValue getElevationRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ElevationRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setElevationRate(uci::type::AngleRateTypeValue value)
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


         /** Returns whether the Element that is identified by RollRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RollRate is emabled or not
           */
         virtual bool hasRollRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RollRate
           *
           * @param type = uci::type::accessorType::angleRateType This Accessor's accessor type
           */
         virtual void enableRollRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RollRate */
         virtual void clearRollRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LOS_RatesType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LOS_RatesType to copy from
           */
         LOS_RatesType(const LOS_RatesType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LOS_RatesType to the contents of the LOS_RatesType on the right
           * hand side (rhs) of the assignment operator.LOS_RatesType [only available to derived classes]
           *
           * @param rhs The LOS_RatesType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LOS_RatesType
           * @return A constant reference to this LOS_RatesType.
           */
         const LOS_RatesType& operator=(const LOS_RatesType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LOS_RatesType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LOS_RatesType_h

