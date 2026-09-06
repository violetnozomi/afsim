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
#ifndef Uci__Type__EntityClauseType_h
#define Uci__Type__EntityClauseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityComparativeType_h)
# include "uci/type/EntityComparativeType.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__EntitySourceEnum_h)
# include "uci/type/EntitySourceEnum.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityClauseType sequence accessor class */
      class EntityClauseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityClauseType()
         { }

         /** Returns this accessor's type constant, i.e. EntityClauseType
           *
           * @return This accessor's type constant, i.e. EntityClauseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityClauseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityClauseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element indicates an Entity related clause (requiring a comparative test between a value in the Entity
           * associated with the Trigger and/or a related message and a value given here) that must be met before the response for
           * the Rule is initiated. Multiple instances are logically ANDed. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityComparativeType, uci::type::accessorType::entityComparativeType> Comparative;

         /** Indicates a unique ID that must match the triggering Entity or one of its fusion contributors. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> EntityID;

         /** Indicates a clause requiring the triggering Entity to be from a specific source(s). If omitted, any Entity sources
           * satisfies the clause. Multiple instances are logically ORed; any of the multiple instances will satisfy the clause.
           * [Minimum occurrences: 0] [Maximum occurrences: 25]
           */
         typedef uci::base::BoundedList<uci::type::EntitySourceEnum, uci::type::accessorType::entitySourceEnum> Source;

         /** Returns the value of the SimplePrimitive data type that is identified by the DesignatedTarget.
           *
           * @return The value of the simple primitive data type identified by DesignatedTarget.
           */
         virtual xs::Boolean getDesignatedTarget() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DesignatedTarget.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDesignatedTarget(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Comparative.
           *
           * @return The bounded list identified by Comparative.
           */
         virtual const uci::type::EntityClauseType::Comparative& getComparative() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Comparative.
           *
           * @return The bounded list identified by Comparative.
           */
         virtual uci::type::EntityClauseType::Comparative& getComparative()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Comparative.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComparative(const uci::type::EntityClauseType::Comparative& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual const uci::type::EntityClauseType::EntityID& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual uci::type::EntityClauseType::EntityID& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityID(const uci::type::EntityClauseType::EntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Source.
           *
           * @return The bounded list identified by Source.
           */
         virtual const uci::type::EntityClauseType::Source& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Source.
           *
           * @return The bounded list identified by Source.
           */
         virtual uci::type::EntityClauseType::Source& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Source.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSource(const uci::type::EntityClauseType::Source& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableZone.
           */
         virtual const uci::type::ZoneType& getApplicableZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableZone.
           */
         virtual uci::type::ZoneType& getApplicableZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApplicableZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApplicableZone
           */
         virtual void setApplicableZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ApplicableZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ApplicableZone is emabled or not
           */
         virtual bool hasApplicableZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ApplicableZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableApplicableZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ApplicableZone */
         virtual void clearApplicableZone()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityClauseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityClauseType to copy from
           */
         EntityClauseType(const EntityClauseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityClauseType to the contents of the EntityClauseType on the
           * right hand side (rhs) of the assignment operator.EntityClauseType [only available to derived classes]
           *
           * @param rhs The EntityClauseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EntityClauseType
           * @return A constant reference to this EntityClauseType.
           */
         const EntityClauseType& operator=(const EntityClauseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityClauseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityClauseType_h

