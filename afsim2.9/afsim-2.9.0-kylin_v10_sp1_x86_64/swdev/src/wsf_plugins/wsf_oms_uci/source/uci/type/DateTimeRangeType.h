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
#ifndef Uci__Type__DateTimeRangeType_h
#define Uci__Type__DateTimeRangeType_h 1

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
        * continue indefinitely or until context implies an end. A range with an End and no Begin is understood to start "now".
        * A range with no Begin or End is understood to start "now" and continue indefinitely or until context implies an end.
        */
      class DateTimeRangeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DateTimeRangeType()
         { }

         /** Returns this accessor's type constant, i.e. DateTimeRangeType
           *
           * @return This accessor's type constant, i.e. DateTimeRangeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dateTimeRangeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DateTimeRangeType& accessor)
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


         /** Returns whether the Element that is identified by Begin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Begin is emabled or not
           */
         virtual bool hasBegin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Begin
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableBegin(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Begin */
         virtual void clearBegin()
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
         DateTimeRangeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DateTimeRangeType to copy from
           */
         DateTimeRangeType(const DateTimeRangeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DateTimeRangeType to the contents of the DateTimeRangeType on the
           * right hand side (rhs) of the assignment operator.DateTimeRangeType [only available to derived classes]
           *
           * @param rhs The DateTimeRangeType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DateTimeRangeType
           * @return A constant reference to this DateTimeRangeType.
           */
         const DateTimeRangeType& operator=(const DateTimeRangeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DateTimeRangeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DateTimeRangeType_h

