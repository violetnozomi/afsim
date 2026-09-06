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
#ifndef Uci__Type__OrbitalKinematicsVelocityType_h
#define Uci__Type__OrbitalKinematicsVelocityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a velocity vector in the specified reference system. */
      class OrbitalKinematicsVelocityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrbitalKinematicsVelocityType()
         { }

         /** Returns this accessor's type constant, i.e. OrbitalKinematicsVelocityType
           *
           * @return This accessor's type constant, i.e. OrbitalKinematicsVelocityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orbitalKinematicsVelocityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrbitalKinematicsVelocityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the X.
           *
           * @return The value of the simple primitive data type identified by X.
           */
         virtual uci::type::SpeedTypeValue getX() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the X.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setX(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Y.
           *
           * @return The value of the simple primitive data type identified by Y.
           */
         virtual uci::type::SpeedTypeValue getY() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Y.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setY(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Z.
           *
           * @return The value of the simple primitive data type identified by Z.
           */
         virtual uci::type::SpeedTypeValue getZ() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Z.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setZ(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrbitalKinematicsVelocityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrbitalKinematicsVelocityType to copy from
           */
         OrbitalKinematicsVelocityType(const OrbitalKinematicsVelocityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrbitalKinematicsVelocityType to the contents of the
           * OrbitalKinematicsVelocityType on the right hand side (rhs) of the assignment operator.OrbitalKinematicsVelocityType
           * [only available to derived classes]
           *
           * @param rhs The OrbitalKinematicsVelocityType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::OrbitalKinematicsVelocityType
           * @return A constant reference to this OrbitalKinematicsVelocityType.
           */
         const OrbitalKinematicsVelocityType& operator=(const OrbitalKinematicsVelocityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrbitalKinematicsVelocityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrbitalKinematicsVelocityType_h

