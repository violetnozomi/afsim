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
#ifndef Uci__Type__VehicleConfigurationType_h
#define Uci__Type__VehicleConfigurationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__InertialStateType_h)
# include "uci/type/InertialStateType.h"
#endif

#if !defined(Uci__Type__EnduranceType_h)
# include "uci/type/EnduranceType.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the VehicleConfigurationType sequence accessor class */
      class VehicleConfigurationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleConfigurationType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleConfigurationType
           *
           * @return This accessor's type constant, i.e. VehicleConfigurationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleConfigurationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleConfigurationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the State.
           *
           * @return The acecssor that provides access to the complex content that is identified by State.
           */
         virtual const uci::type::InertialStateType& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the State.
           *
           * @return The acecssor that provides access to the complex content that is identified by State.
           */
         virtual uci::type::InertialStateType& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the State to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by State
           */
         virtual void setState(const uci::type::InertialStateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual const uci::type::EnduranceType& getEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual uci::type::EnduranceType& getEndurance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Endurance to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Endurance
           */
         virtual void setEndurance(const uci::type::EnduranceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Endurance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Endurance is emabled or not
           */
         virtual bool hasEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Endurance
           *
           * @param type = uci::type::accessorType::enduranceType This Accessor's accessor type
           */
         virtual void enableEndurance(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Endurance */
         virtual void clearEndurance()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VehicleGrossWeight.
           *
           * @return The value of the simple primitive data type identified by VehicleGrossWeight.
           */
         virtual uci::type::MassTypeValue getVehicleGrossWeight() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VehicleGrossWeight.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVehicleGrossWeight(uci::type::MassTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VehicleGrossWeight exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VehicleGrossWeight is emabled or not
           */
         virtual bool hasVehicleGrossWeight() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VehicleGrossWeight
           *
           * @param type = uci::type::accessorType::massType This Accessor's accessor type
           */
         virtual void enableVehicleGrossWeight(uci::base::accessorType::AccessorType type = uci::type::accessorType::massType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VehicleGrossWeight */
         virtual void clearVehicleGrossWeight()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleConfigurationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleConfigurationType to copy from
           */
         VehicleConfigurationType(const VehicleConfigurationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleConfigurationType to the contents of the
           * VehicleConfigurationType on the right hand side (rhs) of the assignment operator.VehicleConfigurationType [only
           * available to derived classes]
           *
           * @param rhs The VehicleConfigurationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::VehicleConfigurationType
           * @return A constant reference to this VehicleConfigurationType.
           */
         const VehicleConfigurationType& operator=(const VehicleConfigurationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleConfigurationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleConfigurationType_h

