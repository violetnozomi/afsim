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
#ifndef Uci__Type__LAR_DetailsType_h
#define Uci__Type__LAR_DetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__WeaponTargetPairingChoiceType_h)
# include "uci/type/WeaponTargetPairingChoiceType.h"
#endif

#if !defined(Uci__Type__PlannedReleaseConditionsType_h)
# include "uci/type/PlannedReleaseConditionsType.h"
#endif

#if !defined(Uci__Type__MissionPlanReferenceType_h)
# include "uci/type/MissionPlanReferenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LAR_DetailsType sequence accessor class */
      class LAR_DetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LAR_DetailsType()
         { }

         /** Returns this accessor's type constant, i.e. LAR_DetailsType
           *
           * @return This accessor's type constant, i.e. LAR_DetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lAR_DetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LAR_DetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Composite.
           *
           * @return The value of the simple primitive data type identified by Composite.
           */
         virtual xs::Boolean getComposite() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Composite.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setComposite(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponTargetPairing.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponTargetPairing.
           */
         virtual const uci::type::WeaponTargetPairingChoiceType& getWeaponTargetPairing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponTargetPairing.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponTargetPairing.
           */
         virtual uci::type::WeaponTargetPairingChoiceType& getWeaponTargetPairing()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeaponTargetPairing to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeaponTargetPairing
           */
         virtual void setWeaponTargetPairing(const uci::type::WeaponTargetPairingChoiceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedReleaseConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedReleaseConditions.
           */
         virtual const uci::type::PlannedReleaseConditionsType& getPlannedReleaseConditions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedReleaseConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedReleaseConditions.
           */
         virtual uci::type::PlannedReleaseConditionsType& getPlannedReleaseConditions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlannedReleaseConditions to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlannedReleaseConditions
           */
         virtual void setPlannedReleaseConditions(const uci::type::PlannedReleaseConditionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponRoute.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponRoute.
           */
         virtual const uci::type::MissionPlanReferenceType& getWeaponRoute() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponRoute.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponRoute.
           */
         virtual uci::type::MissionPlanReferenceType& getWeaponRoute()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeaponRoute to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeaponRoute
           */
         virtual void setWeaponRoute(const uci::type::MissionPlanReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeaponRoute exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeaponRoute is emabled or not
           */
         virtual bool hasWeaponRoute() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeaponRoute
           *
           * @param type = uci::type::accessorType::missionPlanReferenceType This Accessor's accessor type
           */
         virtual void enableWeaponRoute(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanReferenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeaponRoute */
         virtual void clearWeaponRoute()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LAR_DetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_DetailsType to copy from
           */
         LAR_DetailsType(const LAR_DetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_DetailsType to the contents of the LAR_DetailsType on the
           * right hand side (rhs) of the assignment operator.LAR_DetailsType [only available to derived classes]
           *
           * @param rhs The LAR_DetailsType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LAR_DetailsType
           * @return A constant reference to this LAR_DetailsType.
           */
         const LAR_DetailsType& operator=(const LAR_DetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_DetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LAR_DetailsType_h

