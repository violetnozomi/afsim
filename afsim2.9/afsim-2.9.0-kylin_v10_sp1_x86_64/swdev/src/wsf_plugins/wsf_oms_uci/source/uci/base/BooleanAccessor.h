// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Base__BooleanAccessor_h
#define Uci__Base__BooleanAccessor_h 1

#if !defined(Xs__Type__SimpleXmlSchemaPrimitives_h)
# include "xs/type/simpleXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** The BooleanAccessor accessor class is used to provide access to Boolean data types. */
      class BooleanAccessor : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~BooleanAccessor()
         { }

         /** Returns this accessor's type constant, i.e. booleanAccessor
           *
           * @return This accessor's type constant, i.e. booleanAccessor
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::base::accessorType::booleanAccessor;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const BooleanAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the Boolean accessed by this BooleanAccessor
           *
           * @return The Boolean accessed by this BooleanAccessor
           */
         virtual xs::Boolean getBooleanValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the Boolean accessed by this BooleanAccessor to the specified Double
           *
           * @param value The Boolean whose value will be used to set the value of the Boolean accessed by this accessor.
           */
         virtual void setBooleanValue(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Assigns the value of the Boolean accessed by this BooleanAccessor to the value of the Boolean accessed by the
           * BooleanAccessor on the right hand side (rhs) of the assignment operator.
           *
           * @param rhs The BooleanAccessor on the right hand side (rhs) of the assignment operator which provides access to the
           *      Boolean whose value is to be used to set the Boolean that is accessed by this BooleanAccessor.
           * @return A constant reference to this BooleanAccessor.
           */
         virtual const BooleanAccessor& operator=(const BooleanAccessor& rhs)
         {
            setBooleanValue(rhs.getBooleanValue());
            return *this;
         }


         /** Conversion operator to convert this BooleanAccessor to the native xs::Boolean type.
           *
           * @return No return specified, this conversion operator returns a xs::Boolean
           */
         virtual  operator xs::Boolean() const
            throw(uci::base::UCIException)
         {
            return getBooleanValue();
         }



      protected:

         /** The constructor */
         BooleanAccessor()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The BooleanAccessor to copy from
           */
         BooleanAccessor(const BooleanAccessor& rhs)
         {
            (void)rhs;
         }

      }; // BooleanAccessor


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__BooleanAccessor_h

