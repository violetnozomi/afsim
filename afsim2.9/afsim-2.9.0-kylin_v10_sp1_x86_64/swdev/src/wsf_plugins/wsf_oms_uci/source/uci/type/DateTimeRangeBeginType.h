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
#ifndef Uci__Type__DateTimeRangeBeginType_h
#define Uci__Type__DateTimeRangeBeginType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a time range with beginning and ending dates and times. A range with a Begin and no End is understood to
        * continue indefinitely or until context implies an end.
        */
      class DateTimeRangeBeginType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DateTimeRangeBeginType()
         { }

         /** Returns this accessor's type constant, i.e. DateTimeRangeBeginType
           *
           * @return This accessor's type constant, i.e. DateTimeRangeBeginType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dateTimeRangeBeginType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DateTimeRangeBeginType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Begin.
           *
           * @return The value of the simple primitive data type identified by Begin.
           */
         virtual uci::type::DateTimeTypeValue getBegin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Begin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBegin(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the End.
           *
           * @return The value of the simple primitive data type identified by End.
           */
         virtual uci::type::DateTimeTypeValue getEnd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the End.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEnd(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by End exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by End is emabled or not
           */
         virtual bool hasEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by End
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEnd(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by End */
         virtual void clearEnd()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DateTimeRangeBeginType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DateTimeRangeBeginType to copy from
           */
         DateTimeRangeBeginType(const DateTimeRangeBeginType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DateTimeRangeBeginType to the contents of the
           * DateTimeRangeBeginType on the right hand side (rhs) of the assignment operator.DateTimeRangeBeginType [only available
           * to derived classes]
           *
           * @param rhs The DateTimeRangeBeginType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DateTimeRangeBeginType
           * @return A constant reference to this DateTimeRangeBeginType.
           */
         const DateTimeRangeBeginType& operator=(const DateTimeRangeBeginType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DateTimeRangeBeginType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DateTimeRangeBeginType_h

