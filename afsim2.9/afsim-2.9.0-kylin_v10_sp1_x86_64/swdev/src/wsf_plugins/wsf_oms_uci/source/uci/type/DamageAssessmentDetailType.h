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
#ifndef Uci__Type__DamageAssessmentDetailType_h
#define Uci__Type__DamageAssessmentDetailType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamagedObjectType_h)
# include "uci/type/DamagedObjectType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FunctionalDamageConfidenceType_h)
# include "uci/type/FunctionalDamageConfidenceType.h"
#endif

#if !defined(Uci__Type__DamagedHumanType_h)
# include "uci/type/DamagedHumanType.h"
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

      /** This is the DamageAssessmentDetailType sequence accessor class */
      class DamageAssessmentDetailType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageAssessmentDetailType()
         { }

         /** Returns this accessor's type constant, i.e. DamageAssessmentDetailType
           *
           * @return This accessor's type constant, i.e. DamageAssessmentDetailType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageAssessmentDetailType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageAssessmentDetailType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates functional damage for the object. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FunctionalDamageConfidenceType, uci::type::accessorType::functionalDamageConfidenceType> FunctionalDamage;

         /** Returns the accessor that provides access to the complex content that is identified by the DamagedObject.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamagedObject.
           */
         virtual const uci::type::DamagedObjectType& getDamagedObject() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamagedObject.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamagedObject.
           */
         virtual uci::type::DamagedObjectType& getDamagedObject()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamagedObject to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamagedObject
           */
         virtual void setDamagedObject(const uci::type::DamagedObjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FunctionalDamage.
           *
           * @return The bounded list identified by FunctionalDamage.
           */
         virtual const uci::type::DamageAssessmentDetailType::FunctionalDamage& getFunctionalDamage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FunctionalDamage.
           *
           * @return The bounded list identified by FunctionalDamage.
           */
         virtual uci::type::DamageAssessmentDetailType::FunctionalDamage& getFunctionalDamage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FunctionalDamage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFunctionalDamage(const uci::type::DamageAssessmentDetailType::FunctionalDamage& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the HumanDamage.
           *
           * @return The acecssor that provides access to the complex content that is identified by HumanDamage.
           */
         virtual const uci::type::DamagedHumanType& getHumanDamage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the HumanDamage.
           *
           * @return The acecssor that provides access to the complex content that is identified by HumanDamage.
           */
         virtual uci::type::DamagedHumanType& getHumanDamage()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the HumanDamage to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by HumanDamage
           */
         virtual void setHumanDamage(const uci::type::DamagedHumanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HumanDamage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HumanDamage is emabled or not
           */
         virtual bool hasHumanDamage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HumanDamage
           *
           * @param type = uci::type::accessorType::damagedHumanType This Accessor's accessor type
           */
         virtual void enableHumanDamage(uci::base::accessorType::AccessorType type = uci::type::accessorType::damagedHumanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HumanDamage */
         virtual void clearHumanDamage()
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
         DamageAssessmentDetailType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageAssessmentDetailType to copy from
           */
         DamageAssessmentDetailType(const DamageAssessmentDetailType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageAssessmentDetailType to the contents of the
           * DamageAssessmentDetailType on the right hand side (rhs) of the assignment operator.DamageAssessmentDetailType [only
           * available to derived classes]
           *
           * @param rhs The DamageAssessmentDetailType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageAssessmentDetailType
           * @return A constant reference to this DamageAssessmentDetailType.
           */
         const DamageAssessmentDetailType& operator=(const DamageAssessmentDetailType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageAssessmentDetailType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageAssessmentDetailType_h

