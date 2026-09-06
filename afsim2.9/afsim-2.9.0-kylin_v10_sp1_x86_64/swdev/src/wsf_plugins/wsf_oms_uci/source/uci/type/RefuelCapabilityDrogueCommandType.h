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
#ifndef Uci__Type__RefuelCapabilityDrogueCommandType_h
#define Uci__Type__RefuelCapabilityDrogueCommandType_h 1

#if !defined(Uci__Type__RefuelCapabilityDrogueCommonType_h)
# include "uci/type/RefuelCapabilityDrogueCommonType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__HoseCutCommandEnum_h)
# include "uci/type/HoseCutCommandEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelCapabilityDrogueCommandType sequence accessor class */
      class RefuelCapabilityDrogueCommandType : public virtual uci::type::RefuelCapabilityDrogueCommonType {
      public:

         /** The destructor */
         virtual ~RefuelCapabilityDrogueCommandType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelCapabilityDrogueCommandType
           *
           * @return This accessor's type constant, i.e. RefuelCapabilityDrogueCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelCapabilityDrogueCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelCapabilityDrogueCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResetReferencePressure.
           *
           * @return The value of the simple primitive data type identified by ResetReferencePressure.
           */
         virtual xs::Boolean getResetReferencePressure() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResetReferencePressure.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResetReferencePressure(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ResetReferencePressure exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ResetReferencePressure is emabled or not
           */
         virtual bool hasResetReferencePressure() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ResetReferencePressure
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableResetReferencePressure(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ResetReferencePressure */
         virtual void clearResetReferencePressure()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResetDeliveredFuel.
           *
           * @return The value of the simple primitive data type identified by ResetDeliveredFuel.
           */
         virtual xs::Boolean getResetDeliveredFuel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResetDeliveredFuel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResetDeliveredFuel(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ResetDeliveredFuel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ResetDeliveredFuel is emabled or not
           */
         virtual bool hasResetDeliveredFuel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ResetDeliveredFuel
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableResetDeliveredFuel(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ResetDeliveredFuel */
         virtual void clearResetDeliveredFuel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResetMissionFuel.
           *
           * @return The value of the simple primitive data type identified by ResetMissionFuel.
           */
         virtual xs::Boolean getResetMissionFuel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResetMissionFuel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResetMissionFuel(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ResetMissionFuel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ResetMissionFuel is emabled or not
           */
         virtual bool hasResetMissionFuel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ResetMissionFuel
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableResetMissionFuel(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ResetMissionFuel */
         virtual void clearResetMissionFuel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the HoseCut.
           *
           * @return The value of the enumeration identified by HoseCut.
           */
         virtual const uci::type::HoseCutCommandEnum& getHoseCut() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the HoseCut.
           *
           * @return The value of the enumeration identified by HoseCut.
           */
         virtual uci::type::HoseCutCommandEnum& getHoseCut()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the HoseCut.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setHoseCut(const uci::type::HoseCutCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the HoseCut.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setHoseCut(uci::type::HoseCutCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield HoseCutis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasHoseCut() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getHoseCut will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::hoseCutCommandEnum This Accessor's accessor type
           */
         virtual void enableHoseCut(uci::base::accessorType::AccessorType type = uci::type::accessorType::hoseCutCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearHoseCut()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelCapabilityDrogueCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityDrogueCommandType to copy from
           */
         RefuelCapabilityDrogueCommandType(const RefuelCapabilityDrogueCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelCapabilityDrogueCommandType to the contents of the
           * RefuelCapabilityDrogueCommandType on the right hand side (rhs) of the assignment
           * operator.RefuelCapabilityDrogueCommandType [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityDrogueCommandType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::RefuelCapabilityDrogueCommandType
           * @return A constant reference to this RefuelCapabilityDrogueCommandType.
           */
         const RefuelCapabilityDrogueCommandType& operator=(const RefuelCapabilityDrogueCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelCapabilityDrogueCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelCapabilityDrogueCommandType_h

