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
#ifndef Uci__Type__RefuelCapabilityDrogueStatusType_h
#define Uci__Type__RefuelCapabilityDrogueStatusType_h 1

#if !defined(Uci__Type__RefuelCapabilityDrogueCommonType_h)
# include "uci/type/RefuelCapabilityDrogueCommonType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

#if !defined(Uci__Type__HoseCutStatusEnum_h)
# include "uci/type/HoseCutStatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelCapabilityDrogueStatusType sequence accessor class */
      class RefuelCapabilityDrogueStatusType : public virtual uci::type::RefuelCapabilityDrogueCommonType {
      public:

         /** The destructor */
         virtual ~RefuelCapabilityDrogueStatusType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelCapabilityDrogueStatusType
           *
           * @return This accessor's type constant, i.e. RefuelCapabilityDrogueStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelCapabilityDrogueStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelCapabilityDrogueStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DrogueStowed.
           *
           * @return The value of the simple primitive data type identified by DrogueStowed.
           */
         virtual xs::Boolean getDrogueStowed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DrogueStowed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDrogueStowed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DrogueStowed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DrogueStowed is emabled or not
           */
         virtual bool hasDrogueStowed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DrogueStowed
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableDrogueStowed(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DrogueStowed */
         virtual void clearDrogueStowed()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ScheduledMissionFuelToBeDelivered.
           *
           * @return The value of the simple primitive data type identified by ScheduledMissionFuelToBeDelivered.
           */
         virtual uci::type::MassTypeValue getScheduledMissionFuelToBeDelivered() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ScheduledMissionFuelToBeDelivered.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setScheduledMissionFuelToBeDelivered(uci::type::MassTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ScheduledMissionFuelToBeDelivered exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ScheduledMissionFuelToBeDelivered is emabled or not
           */
         virtual bool hasScheduledMissionFuelToBeDelivered() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ScheduledMissionFuelToBeDelivered
           *
           * @param type = uci::type::accessorType::massType This Accessor's accessor type
           */
         virtual void enableScheduledMissionFuelToBeDelivered(uci::base::accessorType::AccessorType type = uci::type::accessorType::massType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ScheduledMissionFuelToBeDelivered */
         virtual void clearScheduledMissionFuelToBeDelivered()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the HoseCutStatus.
           *
           * @return The value of the enumeration identified by HoseCutStatus.
           */
         virtual const uci::type::HoseCutStatusEnum& getHoseCutStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the HoseCutStatus.
           *
           * @return The value of the enumeration identified by HoseCutStatus.
           */
         virtual uci::type::HoseCutStatusEnum& getHoseCutStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the HoseCutStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setHoseCutStatus(const uci::type::HoseCutStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the HoseCutStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setHoseCutStatus(uci::type::HoseCutStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield HoseCutStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasHoseCutStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getHoseCutStatus will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::hoseCutStatusEnum This Accessor's accessor type
           */
         virtual void enableHoseCutStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::hoseCutStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearHoseCutStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelCapabilityDrogueStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityDrogueStatusType to copy from
           */
         RefuelCapabilityDrogueStatusType(const RefuelCapabilityDrogueStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelCapabilityDrogueStatusType to the contents of the
           * RefuelCapabilityDrogueStatusType on the right hand side (rhs) of the assignment
           * operator.RefuelCapabilityDrogueStatusType [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityDrogueStatusType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::RefuelCapabilityDrogueStatusType
           * @return A constant reference to this RefuelCapabilityDrogueStatusType.
           */
         const RefuelCapabilityDrogueStatusType& operator=(const RefuelCapabilityDrogueStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelCapabilityDrogueStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelCapabilityDrogueStatusType_h

