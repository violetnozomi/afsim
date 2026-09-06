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
#ifndef Uci__Type__GCRS_KinematicsType_h
#define Uci__Type__GCRS_KinematicsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__OrbitalKinematicsPositionType_h)
# include "uci/type/OrbitalKinematicsPositionType.h"
#endif

#if !defined(Uci__Type__OrbitalKinematicsVelocityType_h)
# include "uci/type/OrbitalKinematicsVelocityType.h"
#endif

#if !defined(Uci__Type__OrbitalKinematicsAccelerationType_h)
# include "uci/type/OrbitalKinematicsAccelerationType.h"
#endif

#if !defined(Uci__Type__QuaternionType_h)
# include "uci/type/QuaternionType.h"
#endif

#if !defined(Uci__Type__OrbitalPositionVelocityCovarianceType_h)
# include "uci/type/OrbitalPositionVelocityCovarianceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates kinematics vectors in the Geocentric Celestial Reference System (GCRS). */
      class GCRS_KinematicsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GCRS_KinematicsType()
         { }

         /** Returns this accessor's type constant, i.e. GCRS_KinematicsType
           *
           * @return This accessor's type constant, i.e. GCRS_KinematicsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gCRS_KinematicsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GCRS_KinematicsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Epoch.
           *
           * @return The value of the simple primitive data type identified by Epoch.
           */
         virtual uci::type::DateTimeTypeValue getEpoch() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Epoch.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEpoch(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::OrbitalKinematicsPositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::OrbitalKinematicsPositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::OrbitalKinematicsPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual const uci::type::OrbitalKinematicsVelocityType& getVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual uci::type::OrbitalKinematicsVelocityType& getVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Velocity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Velocity
           */
         virtual void setVelocity(const uci::type::OrbitalKinematicsVelocityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Acceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Acceleration.
           */
         virtual const uci::type::OrbitalKinematicsAccelerationType& getAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Acceleration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Acceleration.
           */
         virtual uci::type::OrbitalKinematicsAccelerationType& getAcceleration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Acceleration to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Acceleration
           */
         virtual void setAcceleration(const uci::type::OrbitalKinematicsAccelerationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Acceleration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Acceleration is emabled or not
           */
         virtual bool hasAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Acceleration
           *
           * @param type = uci::type::accessorType::orbitalKinematicsAccelerationType This Accessor's accessor type
           */
         virtual void enableAcceleration(uci::base::accessorType::AccessorType type = uci::type::accessorType::orbitalKinematicsAccelerationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Acceleration */
         virtual void clearAcceleration()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Attitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by Attitude.
           */
         virtual const uci::type::QuaternionType& getAttitude() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Attitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by Attitude.
           */
         virtual uci::type::QuaternionType& getAttitude()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Attitude to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Attitude
           */
         virtual void setAttitude(const uci::type::QuaternionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Attitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Attitude is emabled or not
           */
         virtual bool hasAttitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Attitude
           *
           * @param type = uci::type::accessorType::quaternionType This Accessor's accessor type
           */
         virtual void enableAttitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::quaternionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Attitude */
         virtual void clearAttitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Covariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Covariance.
           */
         virtual const uci::type::OrbitalPositionVelocityCovarianceType& getCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Covariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Covariance.
           */
         virtual uci::type::OrbitalPositionVelocityCovarianceType& getCovariance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Covariance to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Covariance
           */
         virtual void setCovariance(const uci::type::OrbitalPositionVelocityCovarianceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Covariance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Covariance is emabled or not
           */
         virtual bool hasCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Covariance
           *
           * @param type = uci::type::accessorType::orbitalPositionVelocityCovarianceType This Accessor's accessor type
           */
         virtual void enableCovariance(uci::base::accessorType::AccessorType type = uci::type::accessorType::orbitalPositionVelocityCovarianceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Covariance */
         virtual void clearCovariance()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GCRS_KinematicsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GCRS_KinematicsType to copy from
           */
         GCRS_KinematicsType(const GCRS_KinematicsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GCRS_KinematicsType to the contents of the GCRS_KinematicsType on
           * the right hand side (rhs) of the assignment operator.GCRS_KinematicsType [only available to derived classes]
           *
           * @param rhs The GCRS_KinematicsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::GCRS_KinematicsType
           * @return A constant reference to this GCRS_KinematicsType.
           */
         const GCRS_KinematicsType& operator=(const GCRS_KinematicsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GCRS_KinematicsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GCRS_KinematicsType_h

