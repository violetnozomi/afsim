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
#ifndef Uci__Type__DamageAssessmentMDT_h
#define Uci__Type__DamageAssessmentMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamageAssessmentID_Type_h)
# include "uci/type/DamageAssessmentID_Type.h"
#endif

#if !defined(Uci__Type__DamageCategoryEnum_h)
# include "uci/type/DamageCategoryEnum.h"
#endif

#if !defined(Uci__Type__DamageSubjectType_h)
# include "uci/type/DamageSubjectType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DamageAssessmentDetailType_h)
# include "uci/type/DamageAssessmentDetailType.h"
#endif

#if !defined(Uci__Type__DamageSummaryType_h)
# include "uci/type/DamageSummaryType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DamageSummaryID_Type_h)
# include "uci/type/DamageSummaryID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageAssessmentMDT sequence accessor class */
      class DamageAssessmentMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageAssessmentMDT()
         { }

         /** Returns this accessor's type constant, i.e. DamageAssessmentMDT
           *
           * @return This accessor's type constant, i.e. DamageAssessmentMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageAssessmentMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageAssessmentMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates damage assessment details for a specific object. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DamageAssessmentDetailType, uci::type::accessorType::damageAssessmentDetailType> AssessedObject;

         /** Indicates summarized damage information that was used for this assessment. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DamageSummaryID_Type, uci::type::accessorType::damageSummaryID_Type> DamageSummaryID;

         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentID.
           */
         virtual const uci::type::DamageAssessmentID_Type& getDamageAssessmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentID.
           */
         virtual uci::type::DamageAssessmentID_Type& getDamageAssessmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageAssessmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageAssessmentID
           */
         virtual void setDamageAssessmentID(const uci::type::DamageAssessmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AssessmentType.
           *
           * @return The value of the enumeration identified by AssessmentType.
           */
         virtual const uci::type::DamageCategoryEnum& getAssessmentType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AssessmentType.
           *
           * @return The value of the enumeration identified by AssessmentType.
           */
         virtual uci::type::DamageCategoryEnum& getAssessmentType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AssessmentType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAssessmentType(const uci::type::DamageCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AssessmentType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAssessmentType(uci::type::DamageCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Subject.
           *
           * @return The acecssor that provides access to the complex content that is identified by Subject.
           */
         virtual const uci::type::DamageSubjectType& getSubject() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Subject.
           *
           * @return The acecssor that provides access to the complex content that is identified by Subject.
           */
         virtual uci::type::DamageSubjectType& getSubject()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Subject to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Subject
           */
         virtual void setSubject(const uci::type::DamageSubjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CompletionTime.
           *
           * @return The value of the simple primitive data type identified by CompletionTime.
           */
         virtual uci::type::DateTimeTypeValue getCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CompletionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCompletionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentZone.
           */
         virtual const uci::type::ZoneType& getAssessmentZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentZone.
           */
         virtual uci::type::ZoneType& getAssessmentZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AssessmentZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AssessmentZone
           */
         virtual void setAssessmentZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AssessmentZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssessmentZone is emabled or not
           */
         virtual bool hasAssessmentZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssessmentZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableAssessmentZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssessmentZone */
         virtual void clearAssessmentZone()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssessedObject.
           *
           * @return The bounded list identified by AssessedObject.
           */
         virtual const uci::type::DamageAssessmentMDT::AssessedObject& getAssessedObject() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssessedObject.
           *
           * @return The bounded list identified by AssessedObject.
           */
         virtual uci::type::DamageAssessmentMDT::AssessedObject& getAssessedObject()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssessedObject.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssessedObject(const uci::type::DamageAssessmentMDT::AssessedObject& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Summary.
           *
           * @return The acecssor that provides access to the complex content that is identified by Summary.
           */
         virtual const uci::type::DamageSummaryType& getSummary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Summary.
           *
           * @return The acecssor that provides access to the complex content that is identified by Summary.
           */
         virtual uci::type::DamageSummaryType& getSummary()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Summary to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Summary
           */
         virtual void setSummary(const uci::type::DamageSummaryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Summary exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Summary is emabled or not
           */
         virtual bool hasSummary() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Summary
           *
           * @param type = uci::type::accessorType::damageSummaryType This Accessor's accessor type
           */
         virtual void enableSummary(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageSummaryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Summary */
         virtual void clearSummary()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AssessmentNarrative.
           *
           * @return The value of the string data type identified by AssessmentNarrative.
           */
         virtual const uci::type::VisibleString1024Type& getAssessmentNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AssessmentNarrative.
           *
           * @return The value of the string data type identified by AssessmentNarrative.
           */
         virtual uci::type::VisibleString1024Type& getAssessmentNarrative()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AssessmentNarrative to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAssessmentNarrative(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AssessmentNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAssessmentNarrative(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AssessmentNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAssessmentNarrative(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AssessmentNarrative exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssessmentNarrative is emabled or not
           */
         virtual bool hasAssessmentNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssessmentNarrative
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableAssessmentNarrative(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssessmentNarrative */
         virtual void clearAssessmentNarrative()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReAttackRequired.
           *
           * @return The value of the simple primitive data type identified by ReAttackRequired.
           */
         virtual xs::Boolean getReAttackRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReAttackRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReAttackRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReAttackNarrative.
           *
           * @return The value of the string data type identified by ReAttackNarrative.
           */
         virtual const uci::type::VisibleString1024Type& getReAttackNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReAttackNarrative.
           *
           * @return The value of the string data type identified by ReAttackNarrative.
           */
         virtual uci::type::VisibleString1024Type& getReAttackNarrative()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReAttackNarrative to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReAttackNarrative(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReAttackNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReAttackNarrative(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReAttackNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReAttackNarrative(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReAttackNarrative exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReAttackNarrative is emabled or not
           */
         virtual bool hasReAttackNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReAttackNarrative
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableReAttackNarrative(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReAttackNarrative */
         virtual void clearReAttackNarrative()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageSummaryID.
           *
           * @return The bounded list identified by DamageSummaryID.
           */
         virtual const uci::type::DamageAssessmentMDT::DamageSummaryID& getDamageSummaryID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageSummaryID.
           *
           * @return The bounded list identified by DamageSummaryID.
           */
         virtual uci::type::DamageAssessmentMDT::DamageSummaryID& getDamageSummaryID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DamageSummaryID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDamageSummaryID(const uci::type::DamageAssessmentMDT::DamageSummaryID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageAssessmentMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageAssessmentMDT to copy from
           */
         DamageAssessmentMDT(const DamageAssessmentMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageAssessmentMDT to the contents of the DamageAssessmentMDT on
           * the right hand side (rhs) of the assignment operator.DamageAssessmentMDT [only available to derived classes]
           *
           * @param rhs The DamageAssessmentMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DamageAssessmentMDT
           * @return A constant reference to this DamageAssessmentMDT.
           */
         const DamageAssessmentMDT& operator=(const DamageAssessmentMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageAssessmentMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageAssessmentMDT_h

