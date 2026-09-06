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
#ifndef Uci__Type__OrbitalKinematicsAccelerationType_h
#define Uci__Type__OrbitalKinematicsAccelerationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AccelerationType_h)
# include "uci/type/AccelerationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates an acceleration vector in the specified reference system. */
      class OrbitalKinematicsAccelerationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrbitalKinematicsAccelerationType()
         { }

         /** Returns this accessor's type constant, i.e. OrbitalKinematicsAccelerationType
           *
           * @return This accessor's type constant, i.e. OrbitalKinematicsAccelerationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orbitalKinematicsAccelerationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrbitalKinematicsAccelerationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the X.
           *
           * @return The value of the simple primitive data type identified by X.
           */
         virtual uci::type::AccelerationTypeValue getX() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the X.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setX(uci::type::AccelerationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Y.
           *
           * @return The value of the simple primitive data type identified by Y.
           */
         virtual uci::type::AccelerationTypeValue getY() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Y.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setY(uci::type::AccelerationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Z.
           *
           * @return The value of the simple primitive data type identified by Z.
           */
         virtual uci::type::AccelerationTypeValue getZ() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Z.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setZ(uci::type::AccelerationTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrbitalKinematicsAccelerationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrbitalKinematicsAccelerationType to copy from
           */
         OrbitalKinematicsAccelerationType(const OrbitalKinematicsAccelerationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrbitalKinematicsAccelerationType to the contents of the
           * OrbitalKinematicsAccelerationType on the right hand side (rhs) of the assignment
           * operator.OrbitalKinematicsAccelerationType [only available to derived classes]
           *
           * @param rhs The OrbitalKinematicsAccelerationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::OrbitalKinematicsAccelerationType
           * @return A constant reference to this OrbitalKinematicsAccelerationType.
           */
         const OrbitalKinematicsAccelerationType& operator=(const OrbitalKinematicsAccelerationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrbitalKinematicsAccelerationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrbitalKinematicsAccelerationType_h

