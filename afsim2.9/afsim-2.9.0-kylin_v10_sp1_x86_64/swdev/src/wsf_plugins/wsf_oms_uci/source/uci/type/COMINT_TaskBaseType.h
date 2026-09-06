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
#ifndef Uci__Type__COMINT_TaskBaseType_h
#define Uci__Type__COMINT_TaskBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__COMINT_SubCapabilityEnum_h)
# include "uci/type/COMINT_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_TaskBaseType sequence accessor class */
      class COMINT_TaskBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_TaskBaseType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_TaskBaseType
           *
           * @return This accessor's type constant, i.e. COMINT_TaskBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_TaskBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_TaskBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a required output Product of the Task. See ProductTypeEnum for further details. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandBasicType, uci::type::accessorType::productOutputCommandBasicType> Output;

         /** Returns the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @return The value of the enumeration identified by SubCapabilityType.
           */
         virtual const uci::type::COMINT_SubCapabilityEnum& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @return The value of the enumeration identified by SubCapabilityType.
           */
         virtual uci::type::COMINT_SubCapabilityEnum& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubCapabilityType(const uci::type::COMINT_SubCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubCapabilityType(uci::type::COMINT_SubCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual const uci::type::CommandRepetitionType& getRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual uci::type::CommandRepetitionType& getRepetition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Repetition to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Repetition
           */
         virtual void setRepetition(const uci::type::CommandRepetitionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Repetition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Repetition is emabled or not
           */
         virtual bool hasRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Repetition
           *
           * @param type = uci::type::accessorType::commandRepetitionType This Accessor's accessor type
           */
         virtual void enableRepetition(uci::base::accessorType::AccessorType type = uci::type::accessorType::commandRepetitionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Repetition */
         virtual void clearRepetition()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Output.
           *
           * @return The bounded list identified by Output.
           */
         virtual const uci::type::COMINT_TaskBaseType::Output& getOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Output.
           *
           * @return The bounded list identified by Output.
           */
         virtual uci::type::COMINT_TaskBaseType::Output& getOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Output.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOutput(const uci::type::COMINT_TaskBaseType::Output& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_TaskBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_TaskBaseType to copy from
           */
         COMINT_TaskBaseType(const COMINT_TaskBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_TaskBaseType to the contents of the COMINT_TaskBaseType on
           * the right hand side (rhs) of the assignment operator.COMINT_TaskBaseType [only available to derived classes]
           *
           * @param rhs The COMINT_TaskBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::COMINT_TaskBaseType
           * @return A constant reference to this COMINT_TaskBaseType.
           */
         const COMINT_TaskBaseType& operator=(const COMINT_TaskBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_TaskBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_TaskBaseType_h

