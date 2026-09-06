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
#ifndef Uci__Base__DateTimeAccessor_h
#define Uci__Base__DateTimeAccessor_h 1

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

      /** The DateTimeAccessor accessor class is used to provide access to DateTime data types. */
      class DateTimeAccessor : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DateTimeAccessor()
         { }

         /** Returns this accessor's type constant, i.e. dateTimeAccessor
           *
           * @return This accessor's type constant, i.e. dateTimeAccessor
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::base::accessorType::dateTimeAccessor;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DateTimeAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the DateTime accessed by this DateTimeAccessor
           *
           * @return The DateTime accessed by this DateTimeAccessor
           */
         virtual xs::DateTime getDateTimeValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the DateTime accessed by this DateTimeAccessor to the specified Double
           *
           * @param value The DateTime whose value will be used to set the value of the DateTime accessed by this accessor.
           */
         virtual void setDateTimeValue(xs::DateTime value)
            throw(uci::base::UCIException) = 0;


         /** Assigns the value of the DateTime accessed by this DateTimeAccessor to the value of the DateTime accessed by the
           * DateTimeAccessor on the right hand side (rhs) of the assignment operator.
           *
           * @param rhs The DateTimeAccessor on the right hand side (rhs) of the assignment operator which provides access to the
           *      DateTime whose value is to be used to set the DateTime that is accessed by this DateTimeAccessor.
           * @return A constant reference to this DateTimeAccessor.
           */
         virtual const DateTimeAccessor& operator=(const DateTimeAccessor& rhs)
         {
            setDateTimeValue(rhs.getDateTimeValue());
            return *this;
         }


         /** Conversion operator to convert this DateTimeAccessor to the native xs::DateTime type.
           *
           * @return No return specified, this conversion operator returns a xs::DateTime
           */
         virtual  operator xs::DateTime() const
            throw(uci::base::UCIException)
         {
            return getDateTimeValue();
         }



      protected:

         /** The constructor */
         DateTimeAccessor()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DateTimeAccessor to copy from
           */
         DateTimeAccessor(const DateTimeAccessor& rhs)
         {
            (void)rhs;
         }

      }; // DateTimeAccessor


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__DateTimeAccessor_h

