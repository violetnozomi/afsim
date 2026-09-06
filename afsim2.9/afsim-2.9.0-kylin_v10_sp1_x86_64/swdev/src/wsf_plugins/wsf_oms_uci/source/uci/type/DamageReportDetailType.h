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
#ifndef Uci__Type__DamageReportDetailType_h
#define Uci__Type__DamageReportDetailType_h 1

#if !defined(Uci__Type__DamageAssessmentDetailType_h)
# include "uci/type/DamageAssessmentDetailType.h"
#endif

#if !defined(Uci__Type__DamageCategoryEnum_h)
# include "uci/type/DamageCategoryEnum.h"
#endif

#if !defined(Uci__Type__BehavioralChangeEnum_h)
# include "uci/type/BehavioralChangeEnum.h"
#endif

#if !defined(Uci__Type__DamageWeaponEmploymentType_h)
# include "uci/type/DamageWeaponEmploymentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageReportDetailType sequence accessor class */
      class DamageReportDetailType : public virtual uci::type::DamageAssessmentDetailType {
      public:

         /** The destructor */
         virtual ~DamageReportDetailType()
         { }

         /** Returns this accessor's type constant, i.e. DamageReportDetailType
           *
           * @return This accessor's type constant, i.e. DamageReportDetailType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageReportDetailType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageReportDetailType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DamageType.
           *
           * @return The value of the enumeration identified by DamageType.
           */
         virtual const uci::type::DamageCategoryEnum& getDamageType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DamageType.
           *
           * @return The value of the enumeration identified by DamageType.
           */
         virtual uci::type::DamageCategoryEnum& getDamageType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DamageType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDamageType(const uci::type::DamageCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DamageType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDamageType(uci::type::DamageCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield DamageTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasDamageType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getDamageType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::damageCategoryEnum This Accessor's accessor type
           */
         virtual void enableDamageType(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageCategoryEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearDamageType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the BehavioralChange.
           *
           * @return The value of the enumeration identified by BehavioralChange.
           */
         virtual const uci::type::BehavioralChangeEnum& getBehavioralChange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the BehavioralChange.
           *
           * @return The value of the enumeration identified by BehavioralChange.
           */
         virtual uci::type::BehavioralChangeEnum& getBehavioralChange()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the BehavioralChange.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBehavioralChange(const uci::type::BehavioralChangeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the BehavioralChange.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBehavioralChange(uci::type::BehavioralChangeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield BehavioralChangeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasBehavioralChange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getBehavioralChange will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::behavioralChangeEnum This Accessor's accessor type
           */
         virtual void enableBehavioralChange(uci::base::accessorType::AccessorType type = uci::type::accessorType::behavioralChangeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearBehavioralChange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponEmployment.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponEmployment.
           */
         virtual const uci::type::DamageWeaponEmploymentType& getWeaponEmployment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponEmployment.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponEmployment.
           */
         virtual uci::type::DamageWeaponEmploymentType& getWeaponEmployment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeaponEmployment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeaponEmployment
           */
         virtual void setWeaponEmployment(const uci::type::DamageWeaponEmploymentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeaponEmployment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeaponEmployment is emabled or not
           */
         virtual bool hasWeaponEmployment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeaponEmployment
           *
           * @param type = uci::type::accessorType::damageWeaponEmploymentType This Accessor's accessor type
           */
         virtual void enableWeaponEmployment(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageWeaponEmploymentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeaponEmployment */
         virtual void clearWeaponEmployment()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageReportDetailType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageReportDetailType to copy from
           */
         DamageReportDetailType(const DamageReportDetailType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageReportDetailType to the contents of the
           * DamageReportDetailType on the right hand side (rhs) of the assignment operator.DamageReportDetailType [only available
           * to derived classes]
           *
           * @param rhs The DamageReportDetailType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DamageReportDetailType
           * @return A constant reference to this DamageReportDetailType.
           */
         const DamageReportDetailType& operator=(const DamageReportDetailType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageReportDetailType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageReportDetailType_h

