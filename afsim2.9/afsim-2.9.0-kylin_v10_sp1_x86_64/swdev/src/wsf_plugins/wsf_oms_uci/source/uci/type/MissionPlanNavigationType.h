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
#ifndef Uci__Type__MissionPlanNavigationType_h
#define Uci__Type__MissionPlanNavigationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanNavigationType sequence accessor class */
      class MissionPlanNavigationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanNavigationType()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanNavigationType
           *
           * @return This accessor's type constant, i.e. MissionPlanNavigationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanNavigationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanNavigationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpeedOverride.
           *
           * @return The value of the simple primitive data type identified by SpeedOverride.
           */
         virtual uci::type::SpeedTypeValue getSpeedOverride() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpeedOverride.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpeedOverride(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpeedOverride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpeedOverride is emabled or not
           */
         virtual bool hasSpeedOverride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpeedOverride
           *
           * @param type = uci::type::accessorType::speedType This Accessor's accessor type
           */
         virtual void enableSpeedOverride(uci::base::accessorType::AccessorType type = uci::type::accessorType::speedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpeedOverride */
         virtual void clearSpeedOverride()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AltitudeOverride.
           *
           * @return The value of the simple primitive data type identified by AltitudeOverride.
           */
         virtual uci::type::AltitudeTypeValue getAltitudeOverride() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AltitudeOverride.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitudeOverride(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeOverride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeOverride is emabled or not
           */
         virtual bool hasAltitudeOverride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeOverride
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableAltitudeOverride(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeOverride */
         virtual void clearAltitudeOverride()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanNavigationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanNavigationType to copy from
           */
         MissionPlanNavigationType(const MissionPlanNavigationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanNavigationType to the contents of the
           * MissionPlanNavigationType on the right hand side (rhs) of the assignment operator.MissionPlanNavigationType [only
           * available to derived classes]
           *
           * @param rhs The MissionPlanNavigationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MissionPlanNavigationType
           * @return A constant reference to this MissionPlanNavigationType.
           */
         const MissionPlanNavigationType& operator=(const MissionPlanNavigationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanNavigationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanNavigationType_h

