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
#ifndef Uci__Type__EA_TaskSuppressionConstraintsType_h
#define Uci__Type__EA_TaskSuppressionConstraintsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EA_TaskSuppressedEntityType_h)
# include "uci/type/EA_TaskSuppressedEntityType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__EA_TechniqueIdentifierType_h)
# include "uci/type/EA_TechniqueIdentifierType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_TaskSuppressionConstraintsType sequence accessor class */
      class EA_TaskSuppressionConstraintsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TaskSuppressionConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TaskSuppressionConstraintsType
           *
           * @return This accessor's type constant, i.e. EA_TaskSuppressionConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TaskSuppressionConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TaskSuppressionConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element defines the list of hostile entities (threats) which should be the target of jamming. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EA_TaskSuppressedEntityType, uci::type::accessorType::eA_TaskSuppressedEntityType> SuppressedEntity;

         /** This element defines the list of specific frequencies which should be the target of jamming. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> SuppressedFrequencies;

         /** Returns the bounded list that is identified by the SuppressedEntity.
           *
           * @return The bounded list identified by SuppressedEntity.
           */
         virtual const uci::type::EA_TaskSuppressionConstraintsType::SuppressedEntity& getSuppressedEntity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SuppressedEntity.
           *
           * @return The bounded list identified by SuppressedEntity.
           */
         virtual uci::type::EA_TaskSuppressionConstraintsType::SuppressedEntity& getSuppressedEntity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SuppressedEntity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSuppressedEntity(const uci::type::EA_TaskSuppressionConstraintsType::SuppressedEntity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SuppressedFrequencies.
           *
           * @return The bounded list identified by SuppressedFrequencies.
           */
         virtual const uci::type::EA_TaskSuppressionConstraintsType::SuppressedFrequencies& getSuppressedFrequencies() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SuppressedFrequencies.
           *
           * @return The bounded list identified by SuppressedFrequencies.
           */
         virtual uci::type::EA_TaskSuppressionConstraintsType::SuppressedFrequencies& getSuppressedFrequencies()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SuppressedFrequencies.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSuppressedFrequencies(const uci::type::EA_TaskSuppressionConstraintsType::SuppressedFrequencies& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TechniqueIdentifier.
           *
           * @return The acecssor that provides access to the complex content that is identified by TechniqueIdentifier.
           */
         virtual const uci::type::EA_TechniqueIdentifierType& getTechniqueIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TechniqueIdentifier.
           *
           * @return The acecssor that provides access to the complex content that is identified by TechniqueIdentifier.
           */
         virtual uci::type::EA_TechniqueIdentifierType& getTechniqueIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TechniqueIdentifier to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TechniqueIdentifier
           */
         virtual void setTechniqueIdentifier(const uci::type::EA_TechniqueIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TechniqueIdentifier exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TechniqueIdentifier is emabled or not
           */
         virtual bool hasTechniqueIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TechniqueIdentifier
           *
           * @param type = uci::type::accessorType::eA_TechniqueIdentifierType This Accessor's accessor type
           */
         virtual void enableTechniqueIdentifier(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_TechniqueIdentifierType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TechniqueIdentifier */
         virtual void clearTechniqueIdentifier()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TaskSuppressionConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TaskSuppressionConstraintsType to copy from
           */
         EA_TaskSuppressionConstraintsType(const EA_TaskSuppressionConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TaskSuppressionConstraintsType to the contents of the
           * EA_TaskSuppressionConstraintsType on the right hand side (rhs) of the assignment
           * operator.EA_TaskSuppressionConstraintsType [only available to derived classes]
           *
           * @param rhs The EA_TaskSuppressionConstraintsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::EA_TaskSuppressionConstraintsType
           * @return A constant reference to this EA_TaskSuppressionConstraintsType.
           */
         const EA_TaskSuppressionConstraintsType& operator=(const EA_TaskSuppressionConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TaskSuppressionConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TaskSuppressionConstraintsType_h

