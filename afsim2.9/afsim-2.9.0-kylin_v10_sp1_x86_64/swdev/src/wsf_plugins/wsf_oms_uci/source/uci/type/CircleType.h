// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CircleType_h
#define Uci__Type__CircleType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CircleType sequence accessor class */
      class CircleType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CircleType()
         { }

         /** Returns this accessor's type constant, i.e. CircleType
           *
           * @return This accessor's type constant, i.e. CircleType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::circleType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CircleType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Point2D.
           *
           * @return The acecssor that provides access to the complex content that is identified by Point2D.
           */
         virtual const uci::type::Point2D_Type& getPoint2D() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Point2D.
           *
           * @return The acecssor that provides access to the complex content that is identified by Point2D.
           */
         virtual uci::type::Point2D_Type& getPoint2D()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Point2D to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Point2D
           */
         virtual void setPoint2D(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Radius.
           *
           * @return The value of the simple primitive data type identified by Radius.
           */
         virtual uci::type::DistanceTypeValue getRadius() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Radius.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRadius(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CircleType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CircleType to copy from
           */
         CircleType(const CircleType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CircleType to the contents of the CircleType on the right hand
           * side (rhs) of the assignment operator.CircleType [only available to derived classes]
           *
           * @param rhs The CircleType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::CircleType
           * @return A constant reference to this CircleType.
           */
         const CircleType& operator=(const CircleType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CircleType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CircleType_h

