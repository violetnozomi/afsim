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
#ifndef Uci__Type__ObservationCountsByHourType_h
#define Uci__Type__ObservationCountsByHourType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the hour of day (00-23) and the number of observations during that hour. */
      class ObservationCountsByHourType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ObservationCountsByHourType()
         { }

         /** Returns this accessor's type constant, i.e. ObservationCountsByHourType
           *
           * @return This accessor's type constant, i.e. ObservationCountsByHourType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::observationCountsByHourType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ObservationCountsByHourType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour00Count.
           *
           * @return The value of the simple primitive data type identified by Hour00Count.
           */
         virtual xs::UnsignedInt getHour00Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour00Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour00Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour01Count.
           *
           * @return The value of the simple primitive data type identified by Hour01Count.
           */
         virtual xs::UnsignedInt getHour01Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour01Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour01Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour02Count.
           *
           * @return The value of the simple primitive data type identified by Hour02Count.
           */
         virtual xs::UnsignedInt getHour02Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour02Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour02Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour03Count.
           *
           * @return The value of the simple primitive data type identified by Hour03Count.
           */
         virtual xs::UnsignedInt getHour03Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour03Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour03Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour04Count.
           *
           * @return The value of the simple primitive data type identified by Hour04Count.
           */
         virtual xs::UnsignedInt getHour04Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour04Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour04Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour05Count.
           *
           * @return The value of the simple primitive data type identified by Hour05Count.
           */
         virtual xs::UnsignedInt getHour05Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour05Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour05Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour06Count.
           *
           * @return The value of the simple primitive data type identified by Hour06Count.
           */
         virtual xs::UnsignedInt getHour06Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour06Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour06Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour07Count.
           *
           * @return The value of the simple primitive data type identified by Hour07Count.
           */
         virtual xs::UnsignedInt getHour07Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour07Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour07Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour08Count.
           *
           * @return The value of the simple primitive data type identified by Hour08Count.
           */
         virtual xs::UnsignedInt getHour08Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour08Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour08Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour09Count.
           *
           * @return The value of the simple primitive data type identified by Hour09Count.
           */
         virtual xs::UnsignedInt getHour09Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour09Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour09Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour10Count.
           *
           * @return The value of the simple primitive data type identified by Hour10Count.
           */
         virtual xs::UnsignedInt getHour10Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour10Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour10Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour11Count.
           *
           * @return The value of the simple primitive data type identified by Hour11Count.
           */
         virtual xs::UnsignedInt getHour11Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour11Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour11Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour12Count.
           *
           * @return The value of the simple primitive data type identified by Hour12Count.
           */
         virtual xs::UnsignedInt getHour12Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour12Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour12Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour13Count.
           *
           * @return The value of the simple primitive data type identified by Hour13Count.
           */
         virtual xs::UnsignedInt getHour13Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour13Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour13Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour14Count.
           *
           * @return The value of the simple primitive data type identified by Hour14Count.
           */
         virtual xs::UnsignedInt getHour14Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour14Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour14Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour15Count.
           *
           * @return The value of the simple primitive data type identified by Hour15Count.
           */
         virtual xs::UnsignedInt getHour15Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour15Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour15Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour16Count.
           *
           * @return The value of the simple primitive data type identified by Hour16Count.
           */
         virtual xs::UnsignedInt getHour16Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour16Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour16Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour17Count.
           *
           * @return The value of the simple primitive data type identified by Hour17Count.
           */
         virtual xs::UnsignedInt getHour17Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour17Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour17Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour18Count.
           *
           * @return The value of the simple primitive data type identified by Hour18Count.
           */
         virtual xs::UnsignedInt getHour18Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour18Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour18Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour19Count.
           *
           * @return The value of the simple primitive data type identified by Hour19Count.
           */
         virtual xs::UnsignedInt getHour19Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour19Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour19Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour20Count.
           *
           * @return The value of the simple primitive data type identified by Hour20Count.
           */
         virtual xs::UnsignedInt getHour20Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour20Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour20Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour21Count.
           *
           * @return The value of the simple primitive data type identified by Hour21Count.
           */
         virtual xs::UnsignedInt getHour21Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour21Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour21Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour22Count.
           *
           * @return The value of the simple primitive data type identified by Hour22Count.
           */
         virtual xs::UnsignedInt getHour22Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour22Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour22Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hour23Count.
           *
           * @return The value of the simple primitive data type identified by Hour23Count.
           */
         virtual xs::UnsignedInt getHour23Count() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hour23Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHour23Count(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ObservationCountsByHourType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ObservationCountsByHourType to copy from
           */
         ObservationCountsByHourType(const ObservationCountsByHourType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ObservationCountsByHourType to the contents of the
           * ObservationCountsByHourType on the right hand side (rhs) of the assignment operator.ObservationCountsByHourType [only
           * available to derived classes]
           *
           * @param rhs The ObservationCountsByHourType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ObservationCountsByHourType
           * @return A constant reference to this ObservationCountsByHourType.
           */
         const ObservationCountsByHourType& operator=(const ObservationCountsByHourType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ObservationCountsByHourType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ObservationCountsByHourType_h

