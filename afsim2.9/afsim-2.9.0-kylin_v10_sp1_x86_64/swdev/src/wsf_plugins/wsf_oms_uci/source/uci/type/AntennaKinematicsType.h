// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AntennaKinematicsType_h
#define Uci__Type__AntennaKinematicsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

#if !defined(Uci__Type__AntennaBoresightVelocityType_h)
# include "uci/type/AntennaBoresightVelocityType.h"
#endif

#if !defined(Uci__Type__VelocityVelocityCovarianceCartesianType_h)
# include "uci/type/VelocityVelocityCovarianceCartesianType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AntennaKinematicsType sequence accessor class */
      class AntennaKinematicsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AntennaKinematicsType()
         { }

         /** Returns this accessor's type constant, i.e. AntennaKinematicsType
           *
           * @return This accessor's type constant, i.e. AntennaKinematicsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::antennaKinematicsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AntennaKinematicsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationSigma.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationSigma.
           */
         virtual const uci::type::OrientationType& getOrientationSigma() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationSigma.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationSigma.
           */
         virtual uci::type::OrientationType& getOrientationSigma()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OrientationSigma to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OrientationSigma
           */
         virtual void setOrientationSigma(const uci::type::OrientationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OrientationSigma exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OrientationSigma is emabled or not
           */
         virtual bool hasOrientationSigma() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OrientationSigma
           *
           * @param type = uci::type::accessorType::orientationType This Accessor's accessor type
           */
         virtual void enableOrientationSigma(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OrientationSigma */
         virtual void clearOrientationSigma()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaBoresightVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaBoresightVelocity.
           */
         virtual const uci::type::AntennaBoresightVelocityType& getAntennaBoresightVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaBoresightVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaBoresightVelocity.
           */
         virtual uci::type::AntennaBoresightVelocityType& getAntennaBoresightVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaBoresightVelocity to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaBoresightVelocity
           */
         virtual void setAntennaBoresightVelocity(const uci::type::AntennaBoresightVelocityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaBoresightVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaBoresightVelocity is emabled or not
           */
         virtual bool hasAntennaBoresightVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaBoresightVelocity
           *
           * @param type = uci::type::accessorType::antennaBoresightVelocityType This Accessor's accessor type
           */
         virtual void enableAntennaBoresightVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::antennaBoresightVelocityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaBoresightVelocity */
         virtual void clearAntennaBoresightVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AntennaBoresightCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaBoresightCovariance.
           */
         virtual const uci::type::VelocityVelocityCovarianceCartesianType& getAntennaBoresightCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AntennaBoresightCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaBoresightCovariance.
           */
         virtual uci::type::VelocityVelocityCovarianceCartesianType& getAntennaBoresightCovariance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaBoresightCovariance to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaBoresightCovariance
           */
         virtual void setAntennaBoresightCovariance(const uci::type::VelocityVelocityCovarianceCartesianType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaBoresightCovariance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaBoresightCovariance is emabled or not
           */
         virtual bool hasAntennaBoresightCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaBoresightCovariance
           *
           * @param type = uci::type::accessorType::velocityVelocityCovarianceCartesianType This Accessor's accessor type
           */
         virtual void enableAntennaBoresightCovariance(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocityVelocityCovarianceCartesianType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaBoresightCovariance */
         virtual void clearAntennaBoresightCovariance()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AntennaKinematicsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AntennaKinematicsType to copy from
           */
         AntennaKinematicsType(const AntennaKinematicsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AntennaKinematicsType to the contents of the AntennaKinematicsType
           * on the right hand side (rhs) of the assignment operator.AntennaKinematicsType [only available to derived classes]
           *
           * @param rhs The AntennaKinematicsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AntennaKinematicsType
           * @return A constant reference to this AntennaKinematicsType.
           */
         const AntennaKinematicsType& operator=(const AntennaKinematicsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AntennaKinematicsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AntennaKinematicsType_h

