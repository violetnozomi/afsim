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
#ifndef Uci__Type__InertialStateType_h
#define Uci__Type__InertialStateType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point4D_Type_h)
# include "uci/type/Point4D_Type.h"
#endif

#if !defined(Uci__Type__UncertaintyType_h)
# include "uci/type/UncertaintyType.h"
#endif

#if !defined(Uci__Type__Velocity3D_Type_h)
# include "uci/type/Velocity3D_Type.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

#if !defined(Uci__Type__Acceleration3D_Type_h)
# include "uci/type/Acceleration3D_Type.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

#if !defined(Uci__Type__OrientationRateType_h)
# include "uci/type/OrientationRateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the InertialStateType sequence accessor class */
      class InertialStateType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~InertialStateType()
         { }

         /** Returns this accessor's type constant, i.e. InertialStateType
           *
           * @return This accessor's type constant, i.e. InertialStateType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::inertialStateType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const InertialStateType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::Point4D_Type& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::Point4D_Type& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::Point4D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PositionUncertainty.
           *
           * @return The acecssor that provides access to the complex content that is identified by PositionUncertainty.
           */
         virtual const uci::type::UncertaintyType& getPositionUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PositionUncertainty.
           *
           * @return The acecssor that provides access to the complex content that is identified by PositionUncertainty.
           */
         virtual uci::type::UncertaintyType& getPositionUncertainty()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PositionUncertainty to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PositionUncertainty
           */
         virtual void setPositionUncertainty(const uci::type::UncertaintyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PositionUncertainty exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PositionUncertainty is emabled or not
           */
         virtual bool hasPositionUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PositionUncertainty
           *
           * @param type = uci::type::accessorType::uncertaintyType This Accessor's accessor type
           */
         virtual void enablePositionUncertainty(uci::base::accessorType::AccessorType type = uci::type::accessorType::uncertaintyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PositionUncertainty */
         virtual void clearPositionUncertainty()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DomainVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by DomainVelocity.
           */
         virtual const uci::type::Velocity3D_Type& getDomainVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DomainVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by DomainVelocity.
           */
         virtual uci::type::Velocity3D_Type& getDomainVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DomainVelocity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DomainVelocity
           */
         virtual void setDomainVelocity(const uci::type::Velocity3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DomainVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DomainVelocity is emabled or not
           */
         virtual bool hasDomainVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DomainVelocity
           *
           * @param type = uci::type::accessorType::velocity3D_Type This Accessor's accessor type
           */
         virtual void enableDomainVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DomainVelocity */
         virtual void clearDomainVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GroundVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by GroundVelocity.
           */
         virtual const uci::type::Velocity2D_Type& getGroundVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GroundVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by GroundVelocity.
           */
         virtual uci::type::Velocity2D_Type& getGroundVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GroundVelocity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GroundVelocity
           */
         virtual void setGroundVelocity(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GroundVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GroundVelocity is emabled or not
           */
         virtual bool hasGroundVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GroundVelocity
           *
           * @param type = uci::type::accessorType::velocity2D_Type This Accessor's accessor type
           */
         virtual void enableGroundVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GroundVelocity */
         virtual void clearGroundVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DomainAcceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by DomainAcceleration.
           */
         virtual const uci::type::Acceleration3D_Type& getDomainAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DomainAcceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by DomainAcceleration.
           */
         virtual uci::type::Acceleration3D_Type& getDomainAcceleration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DomainAcceleration to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DomainAcceleration
           */
         virtual void setDomainAcceleration(const uci::type::Acceleration3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DomainAcceleration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DomainAcceleration is emabled or not
           */
         virtual bool hasDomainAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DomainAcceleration
           *
           * @param type = uci::type::accessorType::acceleration3D_Type This Accessor's accessor type
           */
         virtual void enableDomainAcceleration(uci::base::accessorType::AccessorType type = uci::type::accessorType::acceleration3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DomainAcceleration */
         virtual void clearDomainAcceleration()
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


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationRate.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationRate.
           */
         virtual const uci::type::OrientationRateType& getOrientationRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationRate.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationRate.
           */
         virtual uci::type::OrientationRateType& getOrientationRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OrientationRate to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OrientationRate
           */
         virtual void setOrientationRate(const uci::type::OrientationRateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OrientationRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OrientationRate is emabled or not
           */
         virtual bool hasOrientationRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OrientationRate
           *
           * @param type = uci::type::accessorType::orientationRateType This Accessor's accessor type
           */
         virtual void enableOrientationRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OrientationRate */
         virtual void clearOrientationRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         InertialStateType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The InertialStateType to copy from
           */
         InertialStateType(const InertialStateType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this InertialStateType to the contents of the InertialStateType on the
           * right hand side (rhs) of the assignment operator.InertialStateType [only available to derived classes]
           *
           * @param rhs The InertialStateType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::InertialStateType
           * @return A constant reference to this InertialStateType.
           */
         const InertialStateType& operator=(const InertialStateType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // InertialStateType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__InertialStateType_h

