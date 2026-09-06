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
#ifndef Uci__Type__OpPointPositionType_h
#define Uci__Type__OpPointPositionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__KinematicsFixedPositionType_h)
# include "uci/type/KinematicsFixedPositionType.h"
#endif

#if !defined(Uci__Type__Velocity2D_UncertaintyType_h)
# include "uci/type/Velocity2D_UncertaintyType.h"
#endif

#if !defined(Uci__Type__AltitudeAdjustmentType_h)
# include "uci/type/AltitudeAdjustmentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpPointPositionType sequence accessor class */
      class OpPointPositionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpPointPositionType()
         { }

         /** Returns this accessor's type constant, i.e. OpPointPositionType
           *
           * @return This accessor's type constant, i.e. OpPointPositionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opPointPositionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpPointPositionType& accessor)
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
         virtual const uci::type::Velocity2D_UncertaintyType& getVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual uci::type::Velocity2D_UncertaintyType& getVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Velocity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Velocity
           */
         virtual void setVelocity(const uci::type::Velocity2D_UncertaintyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Velocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Velocity is emabled or not
           */
         virtual bool hasVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Velocity
           *
           * @param type = uci::type::accessorType::velocity2D_UncertaintyType This Accessor's accessor type
           */
         virtual void enableVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_UncertaintyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Velocity */
         virtual void clearVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HAE_Adjusment.
           *
           * @return The value of the simple primitive data type identified by HAE_Adjusment.
           */
         virtual uci::type::AltitudeAdjustmentTypeValue getHAE_Adjusment() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HAE_Adjusment.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHAE_Adjusment(uci::type::AltitudeAdjustmentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HAE_Adjusment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HAE_Adjusment is emabled or not
           */
         virtual bool hasHAE_Adjusment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HAE_Adjusment
           *
           * @param type = uci::type::accessorType::altitudeAdjustmentType This Accessor's accessor type
           */
         virtual void enableHAE_Adjusment(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeAdjustmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HAE_Adjusment */
         virtual void clearHAE_Adjusment()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpPointPositionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpPointPositionType to copy from
           */
         OpPointPositionType(const OpPointPositionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpPointPositionType to the contents of the OpPointPositionType on
           * the right hand side (rhs) of the assignment operator.OpPointPositionType [only available to derived classes]
           *
           * @param rhs The OpPointPositionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OpPointPositionType
           * @return A constant reference to this OpPointPositionType.
           */
         const OpPointPositionType& operator=(const OpPointPositionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpPointPositionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpPointPositionType_h

