// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__VelocityVelocityCovarianceCartesianType_h
#define Uci__Type__VelocityVelocityCovarianceCartesianType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the velocity-velocity covariance matrix for a Cartesian coordinate system. Velocity covariance is symmetric
        * and therefore the covariance values expressed here are the minimum set.
        */
      class VelocityVelocityCovarianceCartesianType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VelocityVelocityCovarianceCartesianType()
         { }

         /** Returns this accessor's type constant, i.e. VelocityVelocityCovarianceCartesianType
           *
           * @return This accessor's type constant, i.e. VelocityVelocityCovarianceCartesianType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::velocityVelocityCovarianceCartesianType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VelocityVelocityCovarianceCartesianType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VxVx.
           *
           * @return The value of the simple primitive data type identified by VxVx.
           */
         virtual xs::Double getVxVx() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VxVx.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVxVx(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VxVy.
           *
           * @return The value of the simple primitive data type identified by VxVy.
           */
         virtual xs::Double getVxVy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VxVy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVxVy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VxVz.
           *
           * @return The value of the simple primitive data type identified by VxVz.
           */
         virtual xs::Double getVxVz() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VxVz.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVxVz(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VyVy.
           *
           * @return The value of the simple primitive data type identified by VyVy.
           */
         virtual xs::Double getVyVy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VyVy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVyVy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VyVz.
           *
           * @return The value of the simple primitive data type identified by VyVz.
           */
         virtual xs::Double getVyVz() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VyVz.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVyVz(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VzVz.
           *
           * @return The value of the simple primitive data type identified by VzVz.
           */
         virtual xs::Double getVzVz() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VzVz.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVzVz(xs::Double value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VelocityVelocityCovarianceCartesianType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VelocityVelocityCovarianceCartesianType to copy from
           */
         VelocityVelocityCovarianceCartesianType(const VelocityVelocityCovarianceCartesianType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VelocityVelocityCovarianceCartesianType to the contents of the
           * VelocityVelocityCovarianceCartesianType on the right hand side (rhs) of the assignment
           * operator.VelocityVelocityCovarianceCartesianType [only available to derived classes]
           *
           * @param rhs The VelocityVelocityCovarianceCartesianType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::VelocityVelocityCovarianceCartesianType
           * @return A constant reference to this VelocityVelocityCovarianceCartesianType.
           */
         const VelocityVelocityCovarianceCartesianType& operator=(const VelocityVelocityCovarianceCartesianType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VelocityVelocityCovarianceCartesianType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VelocityVelocityCovarianceCartesianType_h

