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
#ifndef Uci__Type__COE_NonEquatorialOrientationType_h
#define Uci__Type__COE_NonEquatorialOrientationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AnglePositiveType_h)
# include "uci/type/AnglePositiveType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Elements to describe orbits that go from south to north. */
      class COE_NonEquatorialOrientationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COE_NonEquatorialOrientationType()
         { }

         /** Returns this accessor's type constant, i.e. COE_NonEquatorialOrientationType
           *
           * @return This accessor's type constant, i.e. COE_NonEquatorialOrientationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOE_NonEquatorialOrientationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COE_NonEquatorialOrientationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RightAscensionOfAscendingNode.
           *
           * @return The value of the simple primitive data type identified by RightAscensionOfAscendingNode.
           */
         virtual uci::type::AnglePositiveTypeValue getRightAscensionOfAscendingNode() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RightAscensionOfAscendingNode.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRightAscensionOfAscendingNode(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ArgumentOfPerigee.
           *
           * @return The value of the simple primitive data type identified by ArgumentOfPerigee.
           */
         virtual uci::type::AnglePositiveTypeValue getArgumentOfPerigee() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ArgumentOfPerigee.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setArgumentOfPerigee(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COE_NonEquatorialOrientationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COE_NonEquatorialOrientationType to copy from
           */
         COE_NonEquatorialOrientationType(const COE_NonEquatorialOrientationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COE_NonEquatorialOrientationType to the contents of the
           * COE_NonEquatorialOrientationType on the right hand side (rhs) of the assignment
           * operator.COE_NonEquatorialOrientationType [only available to derived classes]
           *
           * @param rhs The COE_NonEquatorialOrientationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::COE_NonEquatorialOrientationType
           * @return A constant reference to this COE_NonEquatorialOrientationType.
           */
         const COE_NonEquatorialOrientationType& operator=(const COE_NonEquatorialOrientationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COE_NonEquatorialOrientationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COE_NonEquatorialOrientationType_h

