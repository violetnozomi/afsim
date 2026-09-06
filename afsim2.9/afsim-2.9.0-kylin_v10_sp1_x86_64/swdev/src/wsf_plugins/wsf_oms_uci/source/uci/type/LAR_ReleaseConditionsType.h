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
#ifndef Uci__Type__LAR_ReleaseConditionsType_h
#define Uci__Type__LAR_ReleaseConditionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LAR_ReleaseConditionsType sequence accessor class */
      class LAR_ReleaseConditionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LAR_ReleaseConditionsType()
         { }

         /** Returns this accessor's type constant, i.e. LAR_ReleaseConditionsType
           *
           * @return This accessor's type constant, i.e. LAR_ReleaseConditionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lAR_ReleaseConditionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LAR_ReleaseConditionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ValidAltitude.
           *
           * @return The value of the simple primitive data type identified by ValidAltitude.
           */
         virtual xs::Boolean getValidAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ValidAltitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setValidAltitude(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ValidVelocity.
           *
           * @return The value of the simple primitive data type identified by ValidVelocity.
           */
         virtual xs::Boolean getValidVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ValidVelocity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setValidVelocity(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ValidHeading.
           *
           * @return The value of the simple primitive data type identified by ValidHeading.
           */
         virtual xs::Boolean getValidHeading() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ValidHeading.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setValidHeading(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AltitudeDelta.
           *
           * @return The value of the simple primitive data type identified by AltitudeDelta.
           */
         virtual uci::type::AltitudeTypeValue getAltitudeDelta() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AltitudeDelta.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitudeDelta(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeDelta exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeDelta is emabled or not
           */
         virtual bool hasAltitudeDelta() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeDelta
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableAltitudeDelta(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeDelta */
         virtual void clearAltitudeDelta()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VelocityDelta.
           *
           * @return The acecssor that provides access to the complex content that is identified by VelocityDelta.
           */
         virtual const uci::type::Velocity2D_Type& getVelocityDelta() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VelocityDelta.
           *
           * @return The acecssor that provides access to the complex content that is identified by VelocityDelta.
           */
         virtual uci::type::Velocity2D_Type& getVelocityDelta()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VelocityDelta to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VelocityDelta
           */
         virtual void setVelocityDelta(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VelocityDelta exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VelocityDelta is emabled or not
           */
         virtual bool hasVelocityDelta() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VelocityDelta
           *
           * @param type = uci::type::accessorType::velocity2D_Type This Accessor's accessor type
           */
         virtual void enableVelocityDelta(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VelocityDelta */
         virtual void clearVelocityDelta()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HeadingDelta.
           *
           * @return The value of the simple primitive data type identified by HeadingDelta.
           */
         virtual uci::type::AngleTypeValue getHeadingDelta() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HeadingDelta.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHeadingDelta(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HeadingDelta exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HeadingDelta is emabled or not
           */
         virtual bool hasHeadingDelta() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HeadingDelta
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableHeadingDelta(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HeadingDelta */
         virtual void clearHeadingDelta()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LAR_ReleaseConditionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_ReleaseConditionsType to copy from
           */
         LAR_ReleaseConditionsType(const LAR_ReleaseConditionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_ReleaseConditionsType to the contents of the
           * LAR_ReleaseConditionsType on the right hand side (rhs) of the assignment operator.LAR_ReleaseConditionsType [only
           * available to derived classes]
           *
           * @param rhs The LAR_ReleaseConditionsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::LAR_ReleaseConditionsType
           * @return A constant reference to this LAR_ReleaseConditionsType.
           */
         const LAR_ReleaseConditionsType& operator=(const LAR_ReleaseConditionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_ReleaseConditionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LAR_ReleaseConditionsType_h

