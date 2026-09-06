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
#ifndef Uci__Type__DamageAssessmentRequestMDT_h
#define Uci__Type__DamageAssessmentRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamageAssessmentRequestID_Type_h)
# include "uci/type/DamageAssessmentRequestID_Type.h"
#endif

#if !defined(Uci__Type__DamageCategoryEnum_h)
# include "uci/type/DamageCategoryEnum.h"
#endif

#if !defined(Uci__Type__DamageSubjectType_h)
# include "uci/type/DamageSubjectType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__DamageSummaryID_Type_h)
# include "uci/type/DamageSummaryID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageAssessmentRequestMDT sequence accessor class */
      class DamageAssessmentRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageAssessmentRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. DamageAssessmentRequestMDT
           *
           * @return This accessor's type constant, i.e. DamageAssessmentRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageAssessmentRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageAssessmentRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the ID of a data product which should be used to perform the damage assessment. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** Indicates summarized damage information that should be used to perform the damage assessment. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DamageSummaryID_Type, uci::type::accessorType::damageSummaryID_Type> DamageSummaryID;

         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentRequestID.
           */
         virtual const uci::type::DamageAssessmentRequestID_Type& getDamageAssessmentRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageAssessmentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageAssessmentRequestID.
           */
         virtual uci::type::DamageAssessmentRequestID_Type& getDamageAssessmentRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageAssessmentRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageAssessmentRequestID
           */
         virtual void setDamageAssessmentRequestID(const uci::type::DamageAssessmentRequestID_Type& value)
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


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::DamageAssessmentRequestMDT::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::DamageAssessmentRequestMDT::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::DamageAssessmentRequestMDT::ProductMetadataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageSummaryID.
           *
           * @return The bounded list identified by DamageSummaryID.
           */
         virtual const uci::type::DamageAssessmentRequestMDT::DamageSummaryID& getDamageSummaryID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageSummaryID.
           *
           * @return The bounded list identified by DamageSummaryID.
           */
         virtual uci::type::DamageAssessmentRequestMDT::DamageSummaryID& getDamageSummaryID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DamageSummaryID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDamageSummaryID(const uci::type::DamageAssessmentRequestMDT::DamageSummaryID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RespondBy.
           *
           * @return The value of the simple primitive data type identified by RespondBy.
           */
         virtual uci::type::DateTimeTypeValue getRespondBy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RespondBy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRespondBy(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RespondBy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RespondBy is emabled or not
           */
         virtual bool hasRespondBy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RespondBy
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableRespondBy(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RespondBy */
         virtual void clearRespondBy()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageAssessmentRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageAssessmentRequestMDT to copy from
           */
         DamageAssessmentRequestMDT(const DamageAssessmentRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageAssessmentRequestMDT to the contents of the
           * DamageAssessmentRequestMDT on the right hand side (rhs) of the assignment operator.DamageAssessmentRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The DamageAssessmentRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageAssessmentRequestMDT
           * @return A constant reference to this DamageAssessmentRequestMDT.
           */
         const DamageAssessmentRequestMDT& operator=(const DamageAssessmentRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageAssessmentRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageAssessmentRequestMDT_h

