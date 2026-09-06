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
#ifndef Uci__Type__ECEF_UnitVectorType_h
#define Uci__Type__ECEF_UnitVectorType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ECEF_UnitVectorType sequence accessor class */
      class ECEF_UnitVectorType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ECEF_UnitVectorType()
         { }

         /** Returns this accessor's type constant, i.e. ECEF_UnitVectorType
           *
           * @return This accessor's type constant, i.e. ECEF_UnitVectorType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eCEF_UnitVectorType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ECEF_UnitVectorType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the X axis unit vector component in the WGS-84 Earth-Center Earth-Fixed coordinate frame.; [Minimum
           * inclusive: -1.0] [Maximum inclusive: 1.0]
           */
         typedef uci::base::FloatAccessor X;
         //  The type of primitive data accessed by X
         typedef xs::Float XValue;

         /** Indicates the Y axis unit vector component in the WGS-84 Earth-Center Earth-Fixed coordinate frame.; [Minimum
           * inclusive: -1.0] [Maximum inclusive: 1.0]
           */
         typedef uci::base::FloatAccessor Y;
         //  The type of primitive data accessed by Y
         typedef xs::Float YValue;

         /** Indicates the Z axis unit vector component in the WGS-84 Earth-Center Earth-Fixed coordinate frame.; [Minimum
           * inclusive: -1.0] [Maximum inclusive: 1.0]
           */
         typedef uci::base::FloatAccessor Z;
         //  The type of primitive data accessed by Z
         typedef xs::Float ZValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the X.
           *
           * @return The value of the simple primitive data type identified by X.
           */
         virtual uci::type::ECEF_UnitVectorType::XValue getX() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the X.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setX(uci::type::ECEF_UnitVectorType::XValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Y.
           *
           * @return The value of the simple primitive data type identified by Y.
           */
         virtual uci::type::ECEF_UnitVectorType::YValue getY() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Y.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setY(uci::type::ECEF_UnitVectorType::YValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Z.
           *
           * @return The value of the simple primitive data type identified by Z.
           */
         virtual uci::type::ECEF_UnitVectorType::ZValue getZ() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Z.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setZ(uci::type::ECEF_UnitVectorType::ZValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ECEF_UnitVectorType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ECEF_UnitVectorType to copy from
           */
         ECEF_UnitVectorType(const ECEF_UnitVectorType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ECEF_UnitVectorType to the contents of the ECEF_UnitVectorType on
           * the right hand side (rhs) of the assignment operator.ECEF_UnitVectorType [only available to derived classes]
           *
           * @param rhs The ECEF_UnitVectorType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ECEF_UnitVectorType
           * @return A constant reference to this ECEF_UnitVectorType.
           */
         const ECEF_UnitVectorType& operator=(const ECEF_UnitVectorType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ECEF_UnitVectorType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ECEF_UnitVectorType_h

