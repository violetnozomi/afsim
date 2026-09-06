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
#ifndef Uci__Base__TimeAccessor_h
#define Uci__Base__TimeAccessor_h 1

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

      /** The TimeAccessor accessor class is used to provide access to Time data types. */
      class TimeAccessor : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TimeAccessor()
         { }

         /** Returns this accessor's type constant, i.e. timeAccessor
           *
           * @return This accessor's type constant, i.e. timeAccessor
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::base::accessorType::timeAccessor;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TimeAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the Time accessed by this TimeAccessor
           *
           * @return The Time accessed by this TimeAccessor
           */
         virtual xs::Time getTimeValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the Time accessed by this TimeAccessor to the specified Double
           *
           * @param value The Time whose value will be used to set the value of the Time accessed by this accessor.
           */
         virtual void setTimeValue(xs::Time value)
            throw(uci::base::UCIException) = 0;


         /** Assigns the value of the Time accessed by this TimeAccessor to the value of the Time accessed by the TimeAccessor on
           * the right hand side (rhs) of the assignment operator.
           *
           * @param rhs The TimeAccessor on the right hand side (rhs) of the assignment operator which provides access to the Time
           *      whose value is to be used to set the Time that is accessed by this TimeAccessor.
           * @return A constant reference to this TimeAccessor.
           */
         virtual const TimeAccessor& operator=(const TimeAccessor& rhs)
         {
            setTimeValue(rhs.getTimeValue());
            return *this;
         }


         /** Conversion operator to convert this TimeAccessor to the native xs::Time type.
           *
           * @return No return specified, this conversion operator returns a xs::Time
           */
         virtual  operator xs::Time() const
            throw(uci::base::UCIException)
         {
            return getTimeValue();
         }



      protected:

         /** The constructor */
         TimeAccessor()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TimeAccessor to copy from
           */
         TimeAccessor(const TimeAccessor& rhs)
         {
            (void)rhs;
         }

      }; // TimeAccessor


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__TimeAccessor_h

