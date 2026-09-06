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
#ifndef Uci__Type__OpticalCollectionConstraintsType_h
#define Uci__Type__OpticalCollectionConstraintsType_h 1

#if !defined(Uci__Type__CollectionConstraintsType_h)
# include "uci/type/CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__AngleHalfPairType_h)
# include "uci/type/AngleHalfPairType.h"
#endif

#if !defined(Uci__Type__AnglePairType_h)
# include "uci/type/AnglePairType.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is used to specify a set of constraints that constrain when a PO or AO sensor can start collecting the
        * requested image data.
        */
      class OpticalCollectionConstraintsType : public virtual uci::type::CollectionConstraintsType {
      public:

         /** The destructor */
         virtual ~OpticalCollectionConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. OpticalCollectionConstraintsType
           *
           * @return This accessor's type constant, i.e. OpticalCollectionConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opticalCollectionConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpticalCollectionConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunElevAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunElevAngles.
           */
         virtual const uci::type::AngleHalfPairType& getSunElevAngles() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunElevAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunElevAngles.
           */
         virtual uci::type::AngleHalfPairType& getSunElevAngles()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SunElevAngles to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SunElevAngles
           */
         virtual void setSunElevAngles(const uci::type::AngleHalfPairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SunElevAngles exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SunElevAngles is emabled or not
           */
         virtual bool hasSunElevAngles() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SunElevAngles
           *
           * @param type = uci::type::accessorType::angleHalfPairType This Accessor's accessor type
           */
         virtual void enableSunElevAngles(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleHalfPairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SunElevAngles */
         virtual void clearSunElevAngles()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunAzimuthAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunAzimuthAngles.
           */
         virtual const uci::type::AnglePairType& getSunAzimuthAngles() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunAzimuthAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunAzimuthAngles.
           */
         virtual uci::type::AnglePairType& getSunAzimuthAngles()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SunAzimuthAngles to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SunAzimuthAngles
           */
         virtual void setSunAzimuthAngles(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SunAzimuthAngles exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SunAzimuthAngles is emabled or not
           */
         virtual bool hasSunAzimuthAngles() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SunAzimuthAngles
           *
           * @param type = uci::type::accessorType::anglePairType This Accessor's accessor type
           */
         virtual void enableSunAzimuthAngles(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SunAzimuthAngles */
         virtual void clearSunAzimuthAngles()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SweepSpeed.
           *
           * @return The value of the simple primitive data type identified by SweepSpeed.
           */
         virtual uci::type::SpeedTypeValue getSweepSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SweepSpeed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSweepSpeed(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SweepSpeed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SweepSpeed is emabled or not
           */
         virtual bool hasSweepSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SweepSpeed
           *
           * @param type = uci::type::accessorType::speedType This Accessor's accessor type
           */
         virtual void enableSweepSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::speedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SweepSpeed */
         virtual void clearSweepSpeed()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpticalCollectionConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpticalCollectionConstraintsType to copy from
           */
         OpticalCollectionConstraintsType(const OpticalCollectionConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpticalCollectionConstraintsType to the contents of the
           * OpticalCollectionConstraintsType on the right hand side (rhs) of the assignment
           * operator.OpticalCollectionConstraintsType [only available to derived classes]
           *
           * @param rhs The OpticalCollectionConstraintsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::OpticalCollectionConstraintsType
           * @return A constant reference to this OpticalCollectionConstraintsType.
           */
         const OpticalCollectionConstraintsType& operator=(const OpticalCollectionConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpticalCollectionConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpticalCollectionConstraintsType_h

