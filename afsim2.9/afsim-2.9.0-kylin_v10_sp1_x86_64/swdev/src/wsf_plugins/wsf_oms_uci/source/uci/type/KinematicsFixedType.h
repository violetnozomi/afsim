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
#ifndef Uci__Type__KinematicsFixedType_h
#define Uci__Type__KinematicsFixedType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__KinematicsFixedPositionType_h)
# include "uci/type/KinematicsFixedPositionType.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

#if !defined(Uci__Type__PositionAndVelocityCovarianceType_h)
# include "uci/type/PositionAndVelocityCovarianceType.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

#if !defined(Uci__Type__OrientationCovarianceType_h)
# include "uci/type/OrientationCovarianceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the KinematicsFixedType sequence accessor class */
      class KinematicsFixedType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~KinematicsFixedType()
         { }

         /** Returns this accessor's type constant, i.e. KinematicsFixedType
           *
           * @return This accessor's type constant, i.e. KinematicsFixedType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::kinematicsFixedType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const KinematicsFixedType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::KinematicsFixedPositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::KinematicsFixedPositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::KinematicsFixedPositionType& value)
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


         /** Returns whether the Element that is identified by Velocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Velocity is emabled or not
           */
         virtual bool hasVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Velocity
           *
           * @param type = uci::type::accessorType::velocity2D_Type This Accessor's accessor type
           */
         virtual void enableVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Velocity */
         virtual void clearVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * PositionAndVelocityCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by PositionAndVelocityCovariance.
           */
         virtual const uci::type::PositionAndVelocityCovarianceType& getPositionAndVelocityCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * PositionAndVelocityCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by PositionAndVelocityCovariance.
           */
         virtual uci::type::PositionAndVelocityCovarianceType& getPositionAndVelocityCovariance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PositionAndVelocityCovariance to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PositionAndVelocityCovariance
           */
         virtual void setPositionAndVelocityCovariance(const uci::type::PositionAndVelocityCovarianceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PositionAndVelocityCovariance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PositionAndVelocityCovariance is emabled or not
           */
         virtual bool hasPositionAndVelocityCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PositionAndVelocityCovariance
           *
           * @param type = uci::type::accessorType::positionAndVelocityCovarianceType This Accessor's accessor type
           */
         virtual void enablePositionAndVelocityCovariance(uci::base::accessorType::AccessorType type = uci::type::accessorType::positionAndVelocityCovarianceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PositionAndVelocityCovariance */
         virtual void clearPositionAndVelocityCovariance()
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


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationCovariance.
           */
         virtual const uci::type::OrientationCovarianceType& getOrientationCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationCovariance.
           */
         virtual uci::type::OrientationCovarianceType& getOrientationCovariance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OrientationCovariance to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OrientationCovariance
           */
         virtual void setOrientationCovariance(const uci::type::OrientationCovarianceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OrientationCovariance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OrientationCovariance is emabled or not
           */
         virtual bool hasOrientationCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OrientationCovariance
           *
           * @param type = uci::type::accessorType::orientationCovarianceType This Accessor's accessor type
           */
         virtual void enableOrientationCovariance(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationCovarianceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OrientationCovariance */
         virtual void clearOrientationCovariance()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         KinematicsFixedType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The KinematicsFixedType to copy from
           */
         KinematicsFixedType(const KinematicsFixedType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this KinematicsFixedType to the contents of the KinematicsFixedType on
           * the right hand side (rhs) of the assignment operator.KinematicsFixedType [only available to derived classes]
           *
           * @param rhs The KinematicsFixedType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::KinematicsFixedType
           * @return A constant reference to this KinematicsFixedType.
           */
         const KinematicsFixedType& operator=(const KinematicsFixedType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // KinematicsFixedType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__KinematicsFixedType_h

