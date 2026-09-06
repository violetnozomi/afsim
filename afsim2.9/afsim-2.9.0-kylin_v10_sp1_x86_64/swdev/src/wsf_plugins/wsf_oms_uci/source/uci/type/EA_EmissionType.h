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
#ifndef Uci__Type__EA_EmissionType_h
#define Uci__Type__EA_EmissionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EA_TechniqueIdentifierType_h)
# include "uci/type/EA_TechniqueIdentifierType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EA_TargetType_h)
# include "uci/type/EA_TargetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_EmissionType sequence accessor class */
      class EA_EmissionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_EmissionType()
         { }

         /** Returns this accessor's type constant, i.e. EA_EmissionType
           *
           * @return This accessor's type constant, i.e. EA_EmissionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_EmissionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_EmissionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the target of the EA emission. This element indicates the characteristics of the target which are the focus
           * of the EA. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EA_TargetType, uci::type::accessorType::eA_TargetType> Target;

         /** Returns the accessor that provides access to the complex content that is identified by the TechniqueID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TechniqueID.
           */
         virtual const uci::type::EA_TechniqueIdentifierType& getTechniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TechniqueID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TechniqueID.
           */
         virtual uci::type::EA_TechniqueIdentifierType& getTechniqueID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TechniqueID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TechniqueID
           */
         virtual void setTechniqueID(const uci::type::EA_TechniqueIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TechniqueID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TechniqueID is emabled or not
           */
         virtual bool hasTechniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TechniqueID
           *
           * @param type = uci::type::accessorType::eA_TechniqueIdentifierType This Accessor's accessor type
           */
         virtual void enableTechniqueID(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_TechniqueIdentifierType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TechniqueID */
         virtual void clearTechniqueID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual const uci::type::FrequencyRangeType& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual uci::type::FrequencyRangeType& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Frequency to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Frequency
           */
         virtual void setFrequency(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Frequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Frequency is emabled or not
           */
         virtual bool hasFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Frequency
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Frequency */
         virtual void clearFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Target.
           *
           * @return The bounded list identified by Target.
           */
         virtual const uci::type::EA_EmissionType::Target& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Target.
           *
           * @return The bounded list identified by Target.
           */
         virtual uci::type::EA_EmissionType::Target& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Target.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTarget(const uci::type::EA_EmissionType::Target& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_EmissionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_EmissionType to copy from
           */
         EA_EmissionType(const EA_EmissionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_EmissionType to the contents of the EA_EmissionType on the
           * right hand side (rhs) of the assignment operator.EA_EmissionType [only available to derived classes]
           *
           * @param rhs The EA_EmissionType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EA_EmissionType
           * @return A constant reference to this EA_EmissionType.
           */
         const EA_EmissionType& operator=(const EA_EmissionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_EmissionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_EmissionType_h

