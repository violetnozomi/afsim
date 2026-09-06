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
#ifndef Uci__Type__DetailedKinematicsType_h
#define Uci__Type__DetailedKinematicsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point4D_Type_h)
# include "uci/type/Point4D_Type.h"
#endif

#if !defined(Uci__Type__Velocity3D_Type_h)
# include "uci/type/Velocity3D_Type.h"
#endif

#if !defined(Uci__Type__AirDataType_h)
# include "uci/type/AirDataType.h"
#endif

#if !defined(Uci__Type__Acceleration3D_Type_h)
# include "uci/type/Acceleration3D_Type.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__OrientationRateType_h)
# include "uci/type/OrientationRateType.h"
#endif

#if !defined(Uci__Type__OrientationAccelerationType_h)
# include "uci/type/OrientationAccelerationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DetailedKinematicsType sequence accessor class */
      class DetailedKinematicsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DetailedKinematicsType()
         { }

         /** Returns this accessor's type constant, i.e. DetailedKinematicsType
           *
           * @return This accessor's type constant, i.e. DetailedKinematicsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::detailedKinematicsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DetailedKinematicsType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual const uci::type::Velocity3D_Type& getVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual uci::type::Velocity3D_Type& getVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Velocity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Velocity
           */
         virtual void setVelocity(const uci::type::Velocity3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirData.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirData.
           */
         virtual const uci::type::AirDataType& getAirData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirData.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirData.
           */
         virtual uci::type::AirDataType& getAirData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AirData to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AirData
           */
         virtual void setAirData(const uci::type::AirDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AirData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AirData is emabled or not
           */
         virtual bool hasAirData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AirData
           *
           * @param type = uci::type::accessorType::airDataType This Accessor's accessor type
           */
         virtual void enableAirData(uci::base::accessorType::AccessorType type = uci::type::accessorType::airDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AirData */
         virtual void clearAirData()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Acceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Acceleration.
           */
         virtual const uci::type::Acceleration3D_Type& getAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Acceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Acceleration.
           */
         virtual uci::type::Acceleration3D_Type& getAcceleration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Acceleration to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Acceleration
           */
         virtual void setAcceleration(const uci::type::Acceleration3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Acceleration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Acceleration is emabled or not
           */
         virtual bool hasAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Acceleration
           *
           * @param type = uci::type::accessorType::acceleration3D_Type This Accessor's accessor type
           */
         virtual void enableAcceleration(uci::base::accessorType::AccessorType type = uci::type::accessorType::acceleration3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Acceleration */
         virtual void clearAcceleration()
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


         /** Returns the value of the SimplePrimitive data type that is identified by the WanderAngle.
           *
           * @return The value of the simple primitive data type identified by WanderAngle.
           */
         virtual uci::type::AngleTypeValue getWanderAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WanderAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWanderAngle(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WanderAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WanderAngle is emabled or not
           */
         virtual bool hasWanderAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WanderAngle
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableWanderAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WanderAngle */
         virtual void clearWanderAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MagneticHeading.
           *
           * @return The value of the simple primitive data type identified by MagneticHeading.
           */
         virtual uci::type::AngleTypeValue getMagneticHeading() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MagneticHeading.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMagneticHeading(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MagneticHeading exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MagneticHeading is emabled or not
           */
         virtual bool hasMagneticHeading() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MagneticHeading
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableMagneticHeading(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MagneticHeading */
         virtual void clearMagneticHeading()
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


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationAcceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationAcceleration.
           */
         virtual const uci::type::OrientationAccelerationType& getOrientationAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationAcceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationAcceleration.
           */
         virtual uci::type::OrientationAccelerationType& getOrientationAcceleration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OrientationAcceleration to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OrientationAcceleration
           */
         virtual void setOrientationAcceleration(const uci::type::OrientationAccelerationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OrientationAcceleration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OrientationAcceleration is emabled or not
           */
         virtual bool hasOrientationAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OrientationAcceleration
           *
           * @param type = uci::type::accessorType::orientationAccelerationType This Accessor's accessor type
           */
         virtual void enableOrientationAcceleration(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationAccelerationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OrientationAcceleration */
         virtual void clearOrientationAcceleration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DetailedKinematicsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DetailedKinematicsType to copy from
           */
         DetailedKinematicsType(const DetailedKinematicsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DetailedKinematicsType to the contents of the
           * DetailedKinematicsType on the right hand side (rhs) of the assignment operator.DetailedKinematicsType [only available
           * to derived classes]
           *
           * @param rhs The DetailedKinematicsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DetailedKinematicsType
           * @return A constant reference to this DetailedKinematicsType.
           */
         const DetailedKinematicsType& operator=(const DetailedKinematicsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DetailedKinematicsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DetailedKinematicsType_h

