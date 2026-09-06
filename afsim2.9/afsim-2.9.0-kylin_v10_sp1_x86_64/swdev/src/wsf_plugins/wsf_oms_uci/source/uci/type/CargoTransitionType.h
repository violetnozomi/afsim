// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CargoTransitionType_h
#define Uci__Type__CargoTransitionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CargoID_Type_h)
# include "uci/type/CargoID_Type.h"
#endif

#if !defined(Uci__Type__VehicleControlStrategyEnum_h)
# include "uci/type/VehicleControlStrategyEnum.h"
#endif

#if !defined(Uci__Type__CargoDeliverySubCapabilityEnum_h)
# include "uci/type/CargoDeliverySubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__SectorType_h)
# include "uci/type/SectorType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CargoTransitionType sequence accessor class */
      class CargoTransitionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CargoTransitionType()
         { }

         /** Returns this accessor's type constant, i.e. CargoTransitionType
           *
           * @return This accessor's type constant, i.e. CargoTransitionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cargoTransitionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CargoTransitionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A list of the unique IDs assigned to the cargo items involved in this cargo relocation. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CargoID_Type, uci::type::accessorType::cargoID_Type> CargoID;

         /** Point of control and/or cargo transition (release, drop, pick-up, launch, handover, etc.). For the case of
           * independent handoff and handback points, two are given with the handoff point being first. [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::Point3D_Type, uci::type::accessorType::point3D_Type> TransitionLocation;

         /** Returns the bounded list that is identified by the CargoID.
           *
           * @return The bounded list identified by CargoID.
           */
         virtual const uci::type::CargoTransitionType::CargoID& getCargoID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CargoID.
           *
           * @return The bounded list identified by CargoID.
           */
         virtual uci::type::CargoTransitionType::CargoID& getCargoID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CargoID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCargoID(const uci::type::CargoTransitionType::CargoID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the VehicleControlStrategy.
           *
           * @return The value of the enumeration identified by VehicleControlStrategy.
           */
         virtual const uci::type::VehicleControlStrategyEnum& getVehicleControlStrategy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the VehicleControlStrategy.
           *
           * @return The value of the enumeration identified by VehicleControlStrategy.
           */
         virtual uci::type::VehicleControlStrategyEnum& getVehicleControlStrategy()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the VehicleControlStrategy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setVehicleControlStrategy(const uci::type::VehicleControlStrategyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the VehicleControlStrategy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setVehicleControlStrategy(uci::type::VehicleControlStrategyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LoadUnloadStrategy.
           *
           * @return The value of the enumeration identified by LoadUnloadStrategy.
           */
         virtual const uci::type::CargoDeliverySubCapabilityEnum& getLoadUnloadStrategy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LoadUnloadStrategy.
           *
           * @return The value of the enumeration identified by LoadUnloadStrategy.
           */
         virtual uci::type::CargoDeliverySubCapabilityEnum& getLoadUnloadStrategy()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LoadUnloadStrategy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLoadUnloadStrategy(const uci::type::CargoDeliverySubCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LoadUnloadStrategy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLoadUnloadStrategy(uci::type::CargoDeliverySubCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransitionLocation.
           *
           * @return The bounded list identified by TransitionLocation.
           */
         virtual const uci::type::CargoTransitionType::TransitionLocation& getTransitionLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransitionLocation.
           *
           * @return The bounded list identified by TransitionLocation.
           */
         virtual uci::type::CargoTransitionType::TransitionLocation& getTransitionLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TransitionLocation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTransitionLocation(const uci::type::CargoTransitionType::TransitionLocation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EgressConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by EgressConstraint.
           */
         virtual const uci::type::SectorType& getEgressConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EgressConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by EgressConstraint.
           */
         virtual uci::type::SectorType& getEgressConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EgressConstraint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EgressConstraint
           */
         virtual void setEgressConstraint(const uci::type::SectorType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EgressConstraint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EgressConstraint is emabled or not
           */
         virtual bool hasEgressConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EgressConstraint
           *
           * @param type = uci::type::accessorType::sectorType This Accessor's accessor type
           */
         virtual void enableEgressConstraint(uci::base::accessorType::AccessorType type = uci::type::accessorType::sectorType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EgressConstraint */
         virtual void clearEgressConstraint()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IngressConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by IngressConstraint.
           */
         virtual const uci::type::SectorType& getIngressConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IngressConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by IngressConstraint.
           */
         virtual uci::type::SectorType& getIngressConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the IngressConstraint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by IngressConstraint
           */
         virtual void setIngressConstraint(const uci::type::SectorType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IngressConstraint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IngressConstraint is emabled or not
           */
         virtual bool hasIngressConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IngressConstraint
           *
           * @param type = uci::type::accessorType::sectorType This Accessor's accessor type
           */
         virtual void enableIngressConstraint(uci::base::accessorType::AccessorType type = uci::type::accessorType::sectorType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IngressConstraint */
         virtual void clearIngressConstraint()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CargoTransitionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CargoTransitionType to copy from
           */
         CargoTransitionType(const CargoTransitionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CargoTransitionType to the contents of the CargoTransitionType on
           * the right hand side (rhs) of the assignment operator.CargoTransitionType [only available to derived classes]
           *
           * @param rhs The CargoTransitionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CargoTransitionType
           * @return A constant reference to this CargoTransitionType.
           */
         const CargoTransitionType& operator=(const CargoTransitionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CargoTransitionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CargoTransitionType_h

