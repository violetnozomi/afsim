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
#ifndef Uci__Type__ObjectTypeDamageEstimateType_h
#define Uci__Type__ObjectTypeDamageEstimateType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DamageObjectClassType_h)
# include "uci/type/DamageObjectClassType.h"
#endif

#if !defined(Uci__Type__DamageType_h)
# include "uci/type/DamageType.h"
#endif

#if !defined(Uci__Type__DamageProbabilityZoneType_h)
# include "uci/type/DamageProbabilityZoneType.h"
#endif

#if !defined(Uci__Type__DamageCauseEnum_h)
# include "uci/type/DamageCauseEnum.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ObjectTypeDamageEstimateType sequence accessor class */
      class ObjectTypeDamageEstimateType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ObjectTypeDamageEstimateType()
         { }

         /** Returns this accessor's type constant, i.e. ObjectTypeDamageEstimateType
           *
           * @return This accessor's type constant, i.e. ObjectTypeDamageEstimateType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::objectTypeDamageEstimateType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ObjectTypeDamageEstimateType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates information for identifying the object as a platform, entity, or human. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DamageObjectClassType, uci::type::accessorType::damageObjectClassType> ObjectType;

         /** Indicates the type of damage being summarized. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DamageType, uci::type::accessorType::damageType> DamageType;

         /** Indicates a geo-spatial zone and a range of probability that the damage given in the sibling DamageType element will
           * occur. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DamageProbabilityZoneType, uci::type::accessorType::damageProbabilityZoneType> ProbabilityZone;

         /** Returns the bounded list that is identified by the ObjectType.
           *
           * @return The bounded list identified by ObjectType.
           */
         virtual const uci::type::ObjectTypeDamageEstimateType::ObjectType& getObjectType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ObjectType.
           *
           * @return The bounded list identified by ObjectType.
           */
         virtual uci::type::ObjectTypeDamageEstimateType::ObjectType& getObjectType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ObjectType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setObjectType(const uci::type::ObjectTypeDamageEstimateType::ObjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageType.
           *
           * @return The bounded list identified by DamageType.
           */
         virtual const uci::type::ObjectTypeDamageEstimateType::DamageType& getDamageType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageType.
           *
           * @return The bounded list identified by DamageType.
           */
         virtual uci::type::ObjectTypeDamageEstimateType::DamageType& getDamageType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DamageType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDamageType(const uci::type::ObjectTypeDamageEstimateType::DamageType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProbabilityZone.
           *
           * @return The bounded list identified by ProbabilityZone.
           */
         virtual const uci::type::ObjectTypeDamageEstimateType::ProbabilityZone& getProbabilityZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProbabilityZone.
           *
           * @return The bounded list identified by ProbabilityZone.
           */
         virtual uci::type::ObjectTypeDamageEstimateType::ProbabilityZone& getProbabilityZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProbabilityZone.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProbabilityZone(const uci::type::ObjectTypeDamageEstimateType::ProbabilityZone& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DamageCause.
           *
           * @return The value of the enumeration identified by DamageCause.
           */
         virtual const uci::type::DamageCauseEnum& getDamageCause() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DamageCause.
           *
           * @return The value of the enumeration identified by DamageCause.
           */
         virtual uci::type::DamageCauseEnum& getDamageCause()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DamageCause.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDamageCause(const uci::type::DamageCauseEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DamageCause.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDamageCause(uci::type::DamageCauseEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield DamageCauseis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasDamageCause() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getDamageCause will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::damageCauseEnum This Accessor's accessor type
           */
         virtual void enableDamageCause(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageCauseEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearDamageCause()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DamageNarrative.
           *
           * @return The value of the string data type identified by DamageNarrative.
           */
         virtual const uci::type::VisibleString1024Type& getDamageNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DamageNarrative.
           *
           * @return The value of the string data type identified by DamageNarrative.
           */
         virtual uci::type::VisibleString1024Type& getDamageNarrative()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DamageNarrative to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDamageNarrative(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DamageNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDamageNarrative(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DamageNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDamageNarrative(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DamageNarrative exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DamageNarrative is emabled or not
           */
         virtual bool hasDamageNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DamageNarrative
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableDamageNarrative(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DamageNarrative */
         virtual void clearDamageNarrative()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ObjectTypeDamageEstimateType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ObjectTypeDamageEstimateType to copy from
           */
         ObjectTypeDamageEstimateType(const ObjectTypeDamageEstimateType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ObjectTypeDamageEstimateType to the contents of the
           * ObjectTypeDamageEstimateType on the right hand side (rhs) of the assignment operator.ObjectTypeDamageEstimateType
           * [only available to derived classes]
           *
           * @param rhs The ObjectTypeDamageEstimateType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ObjectTypeDamageEstimateType
           * @return A constant reference to this ObjectTypeDamageEstimateType.
           */
         const ObjectTypeDamageEstimateType& operator=(const ObjectTypeDamageEstimateType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ObjectTypeDamageEstimateType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ObjectTypeDamageEstimateType_h

