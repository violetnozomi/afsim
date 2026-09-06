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
#ifndef Uci__Type__ESM_ProfileEntryType_h
#define Uci__Type__ESM_ProfileEntryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EmitterEntryType_h)
# include "uci/type/EmitterEntryType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__ESM_ActivationEnum_h)
# include "uci/type/ESM_ActivationEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_ProfileEntryType sequence accessor class */
      class ESM_ProfileEntryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ESM_ProfileEntryType()
         { }

         /** Returns this accessor's type constant, i.e. ESM_ProfileEntryType
           *
           * @return This accessor's type constant, i.e. ESM_ProfileEntryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_ProfileEntryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_ProfileEntryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Specify an emitter by ID or by MDF_Entry number. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::EmitterEntryType, uci::type::accessorType::emitterEntryType> EmitterEntry;

         /** Returns the bounded list that is identified by the EmitterEntry.
           *
           * @return The bounded list identified by EmitterEntry.
           */
         virtual const uci::type::ESM_ProfileEntryType::EmitterEntry& getEmitterEntry() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EmitterEntry.
           *
           * @return The bounded list identified by EmitterEntry.
           */
         virtual uci::type::ESM_ProfileEntryType::EmitterEntry& getEmitterEntry()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EmitterEntry.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitterEntry(const uci::type::ESM_ProfileEntryType::EmitterEntry& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EmitterPriorityBin.
           *
           * @return The value of the simple primitive data type identified by EmitterPriorityBin.
           */
         virtual xs::UnsignedInt getEmitterPriorityBin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EmitterPriorityBin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEmitterPriorityBin(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterPriorityBin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterPriorityBin is emabled or not
           */
         virtual bool hasEmitterPriorityBin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterPriorityBin
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableEmitterPriorityBin(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterPriorityBin */
         virtual void clearEmitterPriorityBin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Activation.
           *
           * @return The value of the enumeration identified by Activation.
           */
         virtual const uci::type::ESM_ActivationEnum& getActivation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Activation.
           *
           * @return The value of the enumeration identified by Activation.
           */
         virtual uci::type::ESM_ActivationEnum& getActivation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Activation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActivation(const uci::type::ESM_ActivationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Activation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActivation(uci::type::ESM_ActivationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Activationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasActivation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getActivation will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::eSM_ActivationEnum This Accessor's accessor type
           */
         virtual void enableActivation(uci::base::accessorType::AccessorType type = uci::type::accessorType::eSM_ActivationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearActivation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_ProfileEntryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_ProfileEntryType to copy from
           */
         ESM_ProfileEntryType(const ESM_ProfileEntryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_ProfileEntryType to the contents of the ESM_ProfileEntryType
           * on the right hand side (rhs) of the assignment operator.ESM_ProfileEntryType [only available to derived classes]
           *
           * @param rhs The ESM_ProfileEntryType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ESM_ProfileEntryType
           * @return A constant reference to this ESM_ProfileEntryType.
           */
         const ESM_ProfileEntryType& operator=(const ESM_ProfileEntryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_ProfileEntryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_ProfileEntryType_h

