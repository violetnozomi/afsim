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
#ifndef Uci__Type__DamageAssessmentQFT_h
#define Uci__Type__DamageAssessmentQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
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

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageAssessmentQFT sequence accessor class */
      class DamageAssessmentQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~DamageAssessmentQFT()
         { }

         /** Returns this accessor's type constant, i.e. DamageAssessmentQFT
           *
           * @return This accessor's type constant, i.e. DamageAssessmentQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageAssessmentQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageAssessmentQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DamageAssessmentID_Type, uci::type::accessorType::damageAssessmentID_Type> DamageAssessmentID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::DamageCategoryEnum, uci::type::accessorType::damageCategoryEnum> AssessmentType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DamageSubjectType, uci::type::accessorType::damageSubjectType> Subject;

         /** Returns the bounded list that is identified by the DamageAssessmentID.
           *
           * @return The bounded list identified by DamageAssessmentID.
           */
         virtual const uci::type::DamageAssessmentQFT::DamageAssessmentID& getDamageAssessmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageAssessmentID.
           *
           * @return The bounded list identified by DamageAssessmentID.
           */
         virtual uci::type::DamageAssessmentQFT::DamageAssessmentID& getDamageAssessmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DamageAssessmentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDamageAssessmentID(const uci::type::DamageAssessmentQFT::DamageAssessmentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssessmentType.
           *
           * @return The bounded list identified by AssessmentType.
           */
         virtual const uci::type::DamageAssessmentQFT::AssessmentType& getAssessmentType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssessmentType.
           *
           * @return The bounded list identified by AssessmentType.
           */
         virtual uci::type::DamageAssessmentQFT::AssessmentType& getAssessmentType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssessmentType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssessmentType(const uci::type::DamageAssessmentQFT::AssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Subject.
           *
           * @return The bounded list identified by Subject.
           */
         virtual const uci::type::DamageAssessmentQFT::Subject& getSubject() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Subject.
           *
           * @return The bounded list identified by Subject.
           */
         virtual uci::type::DamageAssessmentQFT::Subject& getSubject()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Subject.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubject(const uci::type::DamageAssessmentQFT::Subject& value)
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


         /** Returns whether the Element that is identified by ReAttackRequired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReAttackRequired is emabled or not
           */
         virtual bool hasReAttackRequired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReAttackRequired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableReAttackRequired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReAttackRequired */
         virtual void clearReAttackRequired()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilter.
           */
         virtual const uci::type::GeoFiltersType& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilter.
           */
         virtual uci::type::GeoFiltersType& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeoFilter to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeoFilter
           */
         virtual void setGeoFilter(const uci::type::GeoFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GeoFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GeoFilter is emabled or not
           */
         virtual bool hasGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GeoFilter
           *
           * @param type = uci::type::accessorType::geoFiltersType This Accessor's accessor type
           */
         virtual void enableGeoFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::geoFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GeoFilter */
         virtual void clearGeoFilter()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageAssessmentQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageAssessmentQFT to copy from
           */
         DamageAssessmentQFT(const DamageAssessmentQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageAssessmentQFT to the contents of the DamageAssessmentQFT on
           * the right hand side (rhs) of the assignment operator.DamageAssessmentQFT [only available to derived classes]
           *
           * @param rhs The DamageAssessmentQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DamageAssessmentQFT
           * @return A constant reference to this DamageAssessmentQFT.
           */
         const DamageAssessmentQFT& operator=(const DamageAssessmentQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageAssessmentQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageAssessmentQFT_h

