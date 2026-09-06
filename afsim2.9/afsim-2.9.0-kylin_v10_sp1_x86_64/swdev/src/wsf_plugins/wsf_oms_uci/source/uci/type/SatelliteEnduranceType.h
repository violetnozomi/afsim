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
#ifndef Uci__Type__SatelliteEnduranceType_h
#define Uci__Type__SatelliteEnduranceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EnduranceBaseType_h)
# include "uci/type/EnduranceBaseType.h"
#endif

#if !defined(Uci__Type__OrbitalManeuverDetailsBaseType_h)
# include "uci/type/OrbitalManeuverDetailsBaseType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the satellite's endurance specified by the indicated elements. */
      class SatelliteEnduranceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SatelliteEnduranceType()
         { }

         /** Returns this accessor's type constant, i.e. SatelliteEnduranceType
           *
           * @return This accessor's type constant, i.e. SatelliteEnduranceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::satelliteEnduranceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SatelliteEnduranceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Propulsion.
           *
           * @return The acecssor that provides access to the complex content that is identified by Propulsion.
           */
         virtual const uci::type::EnduranceBaseType& getPropulsion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Propulsion.
           *
           * @return The acecssor that provides access to the complex content that is identified by Propulsion.
           */
         virtual uci::type::EnduranceBaseType& getPropulsion()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Propulsion to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Propulsion
           */
         virtual void setPropulsion(const uci::type::EnduranceBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Propulsion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Propulsion is emabled or not
           */
         virtual bool hasPropulsion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Propulsion
           *
           * @param type = uci::type::accessorType::enduranceBaseType This Accessor's accessor type
           */
         virtual void enablePropulsion(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Propulsion */
         virtual void clearPropulsion()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Maneuver.
           *
           * @return The acecssor that provides access to the complex content that is identified by Maneuver.
           */
         virtual const uci::type::OrbitalManeuverDetailsBaseType& getManeuver() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Maneuver.
           *
           * @return The acecssor that provides access to the complex content that is identified by Maneuver.
           */
         virtual uci::type::OrbitalManeuverDetailsBaseType& getManeuver()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Maneuver to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Maneuver
           */
         virtual void setManeuver(const uci::type::OrbitalManeuverDetailsBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Maneuver exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Maneuver is emabled or not
           */
         virtual bool hasManeuver() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Maneuver
           *
           * @param type = uci::type::accessorType::orbitalManeuverDetailsBaseType This Accessor's accessor type
           */
         virtual void enableManeuver(uci::base::accessorType::AccessorType type = uci::type::accessorType::orbitalManeuverDetailsBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Maneuver */
         virtual void clearManeuver()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Operational.
           *
           * @return The acecssor that provides access to the complex content that is identified by Operational.
           */
         virtual const uci::type::EnduranceBaseType& getOperational() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Operational.
           *
           * @return The acecssor that provides access to the complex content that is identified by Operational.
           */
         virtual uci::type::EnduranceBaseType& getOperational()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Operational to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Operational
           */
         virtual void setOperational(const uci::type::EnduranceBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Operational exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Operational is emabled or not
           */
         virtual bool hasOperational() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Operational
           *
           * @param type = uci::type::accessorType::enduranceBaseType This Accessor's accessor type
           */
         virtual void enableOperational(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Operational */
         virtual void clearOperational()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SatelliteEnduranceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SatelliteEnduranceType to copy from
           */
         SatelliteEnduranceType(const SatelliteEnduranceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SatelliteEnduranceType to the contents of the
           * SatelliteEnduranceType on the right hand side (rhs) of the assignment operator.SatelliteEnduranceType [only available
           * to derived classes]
           *
           * @param rhs The SatelliteEnduranceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SatelliteEnduranceType
           * @return A constant reference to this SatelliteEnduranceType.
           */
         const SatelliteEnduranceType& operator=(const SatelliteEnduranceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SatelliteEnduranceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SatelliteEnduranceType_h

