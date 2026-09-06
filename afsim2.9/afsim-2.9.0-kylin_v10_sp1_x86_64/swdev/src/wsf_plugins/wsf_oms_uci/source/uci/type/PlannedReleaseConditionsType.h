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
#ifndef Uci__Type__PlannedReleaseConditionsType_h
#define Uci__Type__PlannedReleaseConditionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

#if !defined(Uci__Type__MissionPlanReferenceType_h)
# include "uci/type/MissionPlanReferenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PlannedReleaseConditionsType sequence accessor class */
      class PlannedReleaseConditionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PlannedReleaseConditionsType()
         { }

         /** Returns this accessor's type constant, i.e. PlannedReleaseConditionsType
           *
           * @return This accessor's type constant, i.e. PlannedReleaseConditionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::plannedReleaseConditionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PlannedReleaseConditionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::Point3D_Type& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::Point3D_Type& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual const uci::type::Velocity2D_Type& getVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual uci::type::Velocity2D_Type& getVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Velocity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Velocity
           */
         virtual void setVelocity(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Orientation.
           *
           * @return The acecssor that provides access to the complex content that is identified by Orientation.
           */
         virtual const uci::type::OrientationType& getOrientation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Orientation.
           *
           * @return The acecssor that provides access to the complex content that is identified by Orientation.
           */
         virtual uci::type::OrientationType& getOrientation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Orientation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Orientation
           */
         virtual void setOrientation(const uci::type::OrientationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Orientation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Orientation is emabled or not
           */
         virtual bool hasOrientation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Orientation
           *
           * @param type = uci::type::accessorType::orientationType This Accessor's accessor type
           */
         virtual void enableOrientation(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Orientation */
         virtual void clearOrientation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WindVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by WindVelocity.
           */
         virtual const uci::type::Velocity2D_Type& getWindVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WindVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by WindVelocity.
           */
         virtual uci::type::Velocity2D_Type& getWindVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WindVelocity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WindVelocity
           */
         virtual void setWindVelocity(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WindVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WindVelocity is emabled or not
           */
         virtual bool hasWindVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WindVelocity
           *
           * @param type = uci::type::accessorType::velocity2D_Type This Accessor's accessor type
           */
         virtual void enableWindVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WindVelocity */
         virtual void clearWindVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssociatedMissionPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssociatedMissionPlan.
           */
         virtual const uci::type::MissionPlanReferenceType& getAssociatedMissionPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssociatedMissionPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssociatedMissionPlan.
           */
         virtual uci::type::MissionPlanReferenceType& getAssociatedMissionPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AssociatedMissionPlan to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AssociatedMissionPlan
           */
         virtual void setAssociatedMissionPlan(const uci::type::MissionPlanReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AssociatedMissionPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssociatedMissionPlan is emabled or not
           */
         virtual bool hasAssociatedMissionPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssociatedMissionPlan
           *
           * @param type = uci::type::accessorType::missionPlanReferenceType This Accessor's accessor type
           */
         virtual void enableAssociatedMissionPlan(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanReferenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssociatedMissionPlan */
         virtual void clearAssociatedMissionPlan()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PlannedReleaseConditionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PlannedReleaseConditionsType to copy from
           */
         PlannedReleaseConditionsType(const PlannedReleaseConditionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PlannedReleaseConditionsType to the contents of the
           * PlannedReleaseConditionsType on the right hand side (rhs) of the assignment operator.PlannedReleaseConditionsType
           * [only available to derived classes]
           *
           * @param rhs The PlannedReleaseConditionsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::PlannedReleaseConditionsType
           * @return A constant reference to this PlannedReleaseConditionsType.
           */
         const PlannedReleaseConditionsType& operator=(const PlannedReleaseConditionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PlannedReleaseConditionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PlannedReleaseConditionsType_h

