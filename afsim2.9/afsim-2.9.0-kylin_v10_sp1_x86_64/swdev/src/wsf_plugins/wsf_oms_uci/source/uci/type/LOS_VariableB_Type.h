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
#ifndef Uci__Type__LOS_VariableB_Type_h
#define Uci__Type__LOS_VariableB_Type_h 1

#if !defined(Uci__Type__LOS_VariableA_Type_h)
# include "uci/type/LOS_VariableA_Type.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__LOS_RatesType_h)
# include "uci/type/LOS_RatesType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Defines a line of sight ray in a world inertial or body frame or reference. */
      class LOS_VariableB_Type : public virtual uci::type::LOS_VariableA_Type {
      public:

         /** The destructor */
         virtual ~LOS_VariableB_Type()
         { }

         /** Returns this accessor's type constant, i.e. LOS_VariableB_Type
           *
           * @return This accessor's type constant, i.e. LOS_VariableB_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lOS_VariableB_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LOS_VariableB_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Roll.
           *
           * @return The value of the simple primitive data type identified by Roll.
           */
         virtual uci::type::AngleTypeValue getRoll() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Roll.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRoll(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Roll exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Roll is emabled or not
           */
         virtual bool hasRoll() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Roll
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableRoll(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Roll */
         virtual void clearRoll()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Rates.
           *
           * @return The acecssor that provides access to the complex content that is identified by Rates.
           */
         virtual const uci::type::LOS_RatesType& getRates() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Rates.
           *
           * @return The acecssor that provides access to the complex content that is identified by Rates.
           */
         virtual uci::type::LOS_RatesType& getRates()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Rates to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Rates
           */
         virtual void setRates(const uci::type::LOS_RatesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Rates exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Rates is emabled or not
           */
         virtual bool hasRates() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Rates
           *
           * @param type = uci::type::accessorType::lOS_RatesType This Accessor's accessor type
           */
         virtual void enableRates(uci::base::accessorType::AccessorType type = uci::type::accessorType::lOS_RatesType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Rates */
         virtual void clearRates()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LOS_VariableB_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LOS_VariableB_Type to copy from
           */
         LOS_VariableB_Type(const LOS_VariableB_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LOS_VariableB_Type to the contents of the LOS_VariableB_Type on
           * the right hand side (rhs) of the assignment operator.LOS_VariableB_Type [only available to derived classes]
           *
           * @param rhs The LOS_VariableB_Type on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::LOS_VariableB_Type
           * @return A constant reference to this LOS_VariableB_Type.
           */
         const LOS_VariableB_Type& operator=(const LOS_VariableB_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LOS_VariableB_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LOS_VariableB_Type_h

