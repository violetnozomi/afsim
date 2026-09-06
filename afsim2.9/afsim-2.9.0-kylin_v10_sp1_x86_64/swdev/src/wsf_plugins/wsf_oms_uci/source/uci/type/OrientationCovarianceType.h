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
#ifndef Uci__Type__OrientationCovarianceType_h
#define Uci__Type__OrientationCovarianceType_h 1

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

      /** This is the OrientationCovarianceType sequence accessor class */
      class OrientationCovarianceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrientationCovarianceType()
         { }

         /** Returns this accessor's type constant, i.e. OrientationCovarianceType
           *
           * @return This accessor's type constant, i.e. OrientationCovarianceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orientationCovarianceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrientationCovarianceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RrRr.
           *
           * @return The value of the simple primitive data type identified by RrRr.
           */
         virtual xs::Double getRrRr() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RrRr.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRrRr(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RrRp.
           *
           * @return The value of the simple primitive data type identified by RrRp.
           */
         virtual xs::Double getRrRp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RrRp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRrRp(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RrRy.
           *
           * @return The value of the simple primitive data type identified by RrRy.
           */
         virtual xs::Double getRrRy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RrRy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRrRy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RpRp.
           *
           * @return The value of the simple primitive data type identified by RpRp.
           */
         virtual xs::Double getRpRp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RpRp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRpRp(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RpRy.
           *
           * @return The value of the simple primitive data type identified by RpRy.
           */
         virtual xs::Double getRpRy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RpRy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRpRy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RyRy.
           *
           * @return The value of the simple primitive data type identified by RyRy.
           */
         virtual xs::Double getRyRy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RyRy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRyRy(xs::Double value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrientationCovarianceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrientationCovarianceType to copy from
           */
         OrientationCovarianceType(const OrientationCovarianceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrientationCovarianceType to the contents of the
           * OrientationCovarianceType on the right hand side (rhs) of the assignment operator.OrientationCovarianceType [only
           * available to derived classes]
           *
           * @param rhs The OrientationCovarianceType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OrientationCovarianceType
           * @return A constant reference to this OrientationCovarianceType.
           */
         const OrientationCovarianceType& operator=(const OrientationCovarianceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrientationCovarianceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrientationCovarianceType_h

