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
#ifndef Uci__Base__FloatAccessor_h
#define Uci__Base__FloatAccessor_h 1

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

      /** The FloatAccessor accessor class is used to provide access to Float data types. */
      class FloatAccessor : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FloatAccessor()
         { }

         /** Returns this accessor's type constant, i.e. floatAccessor
           *
           * @return This accessor's type constant, i.e. floatAccessor
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::base::accessorType::floatAccessor;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FloatAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the Float accessed by this FloatAccessor
           *
           * @return The Float accessed by this FloatAccessor
           */
         virtual xs::Float getFloatValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the Float accessed by this FloatAccessor to the specified Double
           *
           * @param value The Float whose value will be used to set the value of the Float accessed by this accessor.
           */
         virtual void setFloatValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Assigns the value of the Float accessed by this FloatAccessor to the value of the Float accessed by the FloatAccessor
           * on the right hand side (rhs) of the assignment operator.
           *
           * @param rhs The FloatAccessor on the right hand side (rhs) of the assignment operator which provides access to the
           *      Float whose value is to be used to set the Float that is accessed by this FloatAccessor.
           * @return A constant reference to this FloatAccessor.
           */
         virtual const FloatAccessor& operator=(const FloatAccessor& rhs)
         {
            setFloatValue(rhs.getFloatValue());
            return *this;
         }


         /** Conversion operator to convert this FloatAccessor to the native xs::Float type.
           *
           * @return No return specified, this conversion operator returns a xs::Float
           */
         virtual  operator xs::Float() const
            throw(uci::base::UCIException)
         {
            return getFloatValue();
         }



      protected:

         /** The constructor */
         FloatAccessor()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FloatAccessor to copy from
           */
         FloatAccessor(const FloatAccessor& rhs)
         {
            (void)rhs;
         }

      }; // FloatAccessor


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__FloatAccessor_h

