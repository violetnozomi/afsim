// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SystemTimeAtReferenceMDT_h
#define Uci__Type__SystemTimeAtReferenceMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__DateTimeAccessor_h)
# include "uci/base/DateTimeAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SystemTimeAtReferenceMDT sequence accessor class */
      class SystemTimeAtReferenceMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemTimeAtReferenceMDT()
         { }

         /** Returns this accessor's type constant, i.e. SystemTimeAtReferenceMDT
           *
           * @return This accessor's type constant, i.e. SystemTimeAtReferenceMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemTimeAtReferenceMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemTimeAtReferenceMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SystemTimeAtLastReference.
           *
           * @return The value of the simple primitive data type identified by SystemTimeAtLastReference.
           */
         virtual xs::DateTime getSystemTimeAtLastReference() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SystemTimeAtLastReference.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSystemTimeAtLastReference(xs::DateTime value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CurrentSystemTime.
           *
           * @return The value of the simple primitive data type identified by CurrentSystemTime.
           */
         virtual xs::DateTime getCurrentSystemTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CurrentSystemTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCurrentSystemTime(xs::DateTime value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentSystemTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentSystemTime is emabled or not
           */
         virtual bool hasCurrentSystemTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentSystemTime
           *
           * @param type = uci::base::accessorType::dateTimeAccessor This Accessor's accessor type
           */
         virtual void enableCurrentSystemTime(uci::base::accessorType::AccessorType type = uci::base::accessorType::dateTimeAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentSystemTime */
         virtual void clearCurrentSystemTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CurrentZuluTime.
           *
           * @return The value of the simple primitive data type identified by CurrentZuluTime.
           */
         virtual uci::type::DateTimeTypeValue getCurrentZuluTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CurrentZuluTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCurrentZuluTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentZuluTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentZuluTime is emabled or not
           */
         virtual bool hasCurrentZuluTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentZuluTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableCurrentZuluTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentZuluTime */
         virtual void clearCurrentZuluTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemTimeAtReferenceMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemTimeAtReferenceMDT to copy from
           */
         SystemTimeAtReferenceMDT(const SystemTimeAtReferenceMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemTimeAtReferenceMDT to the contents of the
           * SystemTimeAtReferenceMDT on the right hand side (rhs) of the assignment operator.SystemTimeAtReferenceMDT [only
           * available to derived classes]
           *
           * @param rhs The SystemTimeAtReferenceMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SystemTimeAtReferenceMDT
           * @return A constant reference to this SystemTimeAtReferenceMDT.
           */
         const SystemTimeAtReferenceMDT& operator=(const SystemTimeAtReferenceMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemTimeAtReferenceMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemTimeAtReferenceMDT_h

