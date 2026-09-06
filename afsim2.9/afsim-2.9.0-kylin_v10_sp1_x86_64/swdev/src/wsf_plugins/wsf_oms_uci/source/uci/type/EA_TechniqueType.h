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
#ifndef Uci__Type__EA_TechniqueType_h
#define Uci__Type__EA_TechniqueType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EA_TechniqueIdentifierType_h)
# include "uci/type/EA_TechniqueIdentifierType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EA_ActivationEnum_h)
# include "uci/type/EA_ActivationEnum.h"
#endif

#if !defined(Uci__Type__EA_PointingEnum_h)
# include "uci/type/EA_PointingEnum.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__EmitterIdentityType_h)
# include "uci/type/EmitterIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificEmitterIdentityType_h)
# include "uci/type/SpecificEmitterIdentityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_TechniqueType sequence accessor class */
      class EA_TechniqueType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TechniqueType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TechniqueType
           *
           * @return This accessor's type constant, i.e. EA_TechniqueType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TechniqueType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TechniqueType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates how the technique is activated. See annotated type enumerations for further details. [Maximum occurrences:
           * 2]
           */
         typedef uci::base::BoundedList<uci::type::EA_ActivationEnum, uci::type::accessorType::eA_ActivationEnum> Activation;

         /** Indicates the bandwidth of the Technique. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> Band;

         /** Indicates an EA Component the Technique can be assigned to. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** Indicates an emitter type which the Technique might be effective against. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EmitterIdentityType, uci::type::accessorType::emitterIdentityType> Emitter;

         /** Indicates the specific emitter type ***instance*** which the Technique might be effective against. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificEmitterIdentityType, uci::type::accessorType::specificEmitterIdentityType> SpecificEmitter;

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


         /** Returns the bounded list that is identified by the Activation.
           *
           * @return The bounded list identified by Activation.
           */
         virtual const uci::type::EA_TechniqueType::Activation& getActivation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Activation.
           *
           * @return The bounded list identified by Activation.
           */
         virtual uci::type::EA_TechniqueType::Activation& getActivation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Activation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivation(const uci::type::EA_TechniqueType::Activation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Pointing.
           *
           * @return The value of the enumeration identified by Pointing.
           */
         virtual const uci::type::EA_PointingEnum& getPointing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Pointing.
           *
           * @return The value of the enumeration identified by Pointing.
           */
         virtual uci::type::EA_PointingEnum& getPointing()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Pointing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointing(const uci::type::EA_PointingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Pointing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointing(uci::type::EA_PointingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Band.
           *
           * @return The bounded list identified by Band.
           */
         virtual const uci::type::EA_TechniqueType::Band& getBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Band.
           *
           * @return The bounded list identified by Band.
           */
         virtual uci::type::EA_TechniqueType::Band& getBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Band.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBand(const uci::type::EA_TechniqueType::Band& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::EA_TechniqueType::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::EA_TechniqueType::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::EA_TechniqueType::ComponentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::EA_TechniqueType::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::EA_TechniqueType::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::EA_TechniqueType::Emitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual const uci::type::EA_TechniqueType::SpecificEmitter& getSpecificEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual uci::type::EA_TechniqueType::SpecificEmitter& getSpecificEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificEmitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificEmitter(const uci::type::EA_TechniqueType::SpecificEmitter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TechniqueType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TechniqueType to copy from
           */
         EA_TechniqueType(const EA_TechniqueType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TechniqueType to the contents of the EA_TechniqueType on the
           * right hand side (rhs) of the assignment operator.EA_TechniqueType [only available to derived classes]
           *
           * @param rhs The EA_TechniqueType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EA_TechniqueType
           * @return A constant reference to this EA_TechniqueType.
           */
         const EA_TechniqueType& operator=(const EA_TechniqueType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TechniqueType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TechniqueType_h

