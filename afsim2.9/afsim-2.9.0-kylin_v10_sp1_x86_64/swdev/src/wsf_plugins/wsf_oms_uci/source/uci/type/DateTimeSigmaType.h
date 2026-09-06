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
#ifndef Uci__Type__DateTimeSigmaType_h
#define Uci__Type__DateTimeSigmaType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DateTimeSigmaType sequence accessor class */
      class DateTimeSigmaType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DateTimeSigmaType()
         { }

         /** Returns this accessor's type constant, i.e. DateTimeSigmaType
           *
           * @return This accessor's type constant, i.e. DateTimeSigmaType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dateTimeSigmaType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DateTimeSigmaType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DateTime.
           *
           * @return The value of the simple primitive data type identified by DateTime.
           */
         virtual uci::type::DateTimeTypeValue getDateTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DateTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDateTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DateTimeSigma.
           *
           * @return The value of the simple primitive data type identified by DateTimeSigma.
           */
         virtual uci::type::DurationTypeValue getDateTimeSigma() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DateTimeSigma.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDateTimeSigma(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DateTimeSigma exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DateTimeSigma is emabled or not
           */
         virtual bool hasDateTimeSigma() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DateTimeSigma
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableDateTimeSigma(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DateTimeSigma */
         virtual void clearDateTimeSigma()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DateTimeSigmaType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DateTimeSigmaType to copy from
           */
         DateTimeSigmaType(const DateTimeSigmaType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DateTimeSigmaType to the contents of the DateTimeSigmaType on the
           * right hand side (rhs) of the assignment operator.DateTimeSigmaType [only available to derived classes]
           *
           * @param rhs The DateTimeSigmaType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DateTimeSigmaType
           * @return A constant reference to this DateTimeSigmaType.
           */
         const DateTimeSigmaType& operator=(const DateTimeSigmaType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DateTimeSigmaType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DateTimeSigmaType_h

