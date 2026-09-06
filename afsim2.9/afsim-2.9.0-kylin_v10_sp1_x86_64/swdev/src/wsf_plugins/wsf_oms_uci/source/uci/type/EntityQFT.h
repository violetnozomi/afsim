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
#ifndef Uci__Type__EntityQFT_h
#define Uci__Type__EntityQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__EntitySourceEnum_h)
# include "uci/type/EntitySourceEnum.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Type__EntityExternalType_h)
# include "uci/type/EntityExternalType.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

#if !defined(Uci__Type__SourceFiltersType_h)
# include "uci/type/SourceFiltersType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__StandardIdentityEnum_h)
# include "uci/type/StandardIdentityEnum.h"
#endif

#if !defined(Uci__Type__CountryCodeType_h)
# include "uci/type/CountryCodeType.h"
#endif

#if !defined(Uci__Type__PlatformIdentityType_h)
# include "uci/type/PlatformIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificIdentityType_h)
# include "uci/type/SpecificIdentityType.h"
#endif

#if !defined(Uci__Type__EmitterIdentityType_h)
# include "uci/type/EmitterIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificEmitterIdentityType_h)
# include "uci/type/SpecificEmitterIdentityType.h"
#endif

#if !defined(Uci__Type__MobilityEnum_h)
# include "uci/type/MobilityEnum.h"
#endif

#if !defined(Uci__Type__EntityStatusEnum_h)
# include "uci/type/EntityStatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityQFT sequence accessor class */
      class EntityQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~EntityQFT()
         { }

         /** Returns this accessor's type constant, i.e. EntityQFT
           *
           * @return This accessor's type constant, i.e. EntityQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The Entity ID to query [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> EntityID;

         /** Indicates the environment or "domain" in which the Entity is operating. Derived from MIL-STD-6016; see MIL-STD-6016
           * DFI/DUI 275/004 for additional details. Each environment has a corresponding set of specific types. [Minimum
           * occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::EnvironmentEnum, uci::type::accessorType::environmentEnum> Environment;

         /** The originator of the entity to query. [Minimum occurrences: 0] [Maximum occurrences: 25] */
         typedef uci::base::BoundedList<uci::type::EntitySourceEnum, uci::type::accessorType::entitySourceEnum> Originator;

         /** The associated ID of the entity to query [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ID_Type, uci::type::accessorType::iD_Type> AssociatedID;

         /** This element provides a mapping to IDs in another system. (i.e., non-UCI) [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityExternalType, uci::type::accessorType::entityExternalType> SourceMappingID;

         /** This element gives a geographical filter to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GeoFiltersType, uci::type::accessorType::geoFiltersType> GeoFilter;

         /** This element gives a temporal filter to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> TemporalFilter;

         /** Indicates the "standard identity" of the Entity. Derived from MIL-STD-6016; see MIL-STD-6016 DFI/DUI 376/007 for
           * additional details. [Minimum occurrences: 0] [Maximum occurrences: 8]
           */
         typedef uci::base::BoundedList<uci::type::StandardIdentityEnum, uci::type::accessorType::standardIdentityEnum> StandardIdentity;

         /** DOD standard country code designator to which an entity owes its allegiance. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CountryCodeType, uci::type::accessorType::countryCodeType> Allegiance;

         /** Indicates a possible "platform identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PlatformIdentityType, uci::type::accessorType::platformIdentityType> Platform;

         /** Indicates a possible "specific identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificIdentityType, uci::type::accessorType::specificIdentityType> Specific;

         /** Indicates the emitter typed that has been identified in association with the Entity. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EmitterIdentityType, uci::type::accessorType::emitterIdentityType> Emitter;

         /** Indicates the specific emitter type ***instance*** that has been identified in association with the Entity. Specific
           * Emitter Identification (SEI) technology passively measures emitters, and characterizes unique emitter signatures that
           * can be used for friendly or enemy emitter identification and fingerprinting. A SEI and this SpecificEmitter element
           * refer to a specific physical instance of a type of emitter as given in the sibling EmitterType element. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificEmitterIdentityType, uci::type::accessorType::specificEmitterIdentityType> SpecificEmitter;

         /** [Minimum occurrences: 0] [Maximum occurrences: 3] */
         typedef uci::base::BoundedList<uci::type::MobilityEnum, uci::type::accessorType::mobilityEnum> Mobility;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9] */
         typedef uci::base::BoundedList<uci::type::EntityStatusEnum, uci::type::accessorType::entityStatusEnum> Status;

         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual const uci::type::EntityQFT::EntityID& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual uci::type::EntityQFT::EntityID& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityID(const uci::type::EntityQFT::EntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual const uci::type::EntityQFT::Environment& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual uci::type::EntityQFT::Environment& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Environment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnvironment(const uci::type::EntityQFT::Environment& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Originator.
           *
           * @return The bounded list identified by Originator.
           */
         virtual const uci::type::EntityQFT::Originator& getOriginator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Originator.
           *
           * @return The bounded list identified by Originator.
           */
         virtual uci::type::EntityQFT::Originator& getOriginator()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Originator.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOriginator(const uci::type::EntityQFT::Originator& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedID.
           *
           * @return The bounded list identified by AssociatedID.
           */
         virtual const uci::type::EntityQFT::AssociatedID& getAssociatedID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedID.
           *
           * @return The bounded list identified by AssociatedID.
           */
         virtual uci::type::EntityQFT::AssociatedID& getAssociatedID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedID(const uci::type::EntityQFT::AssociatedID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SourceMappingID.
           *
           * @return The bounded list identified by SourceMappingID.
           */
         virtual const uci::type::EntityQFT::SourceMappingID& getSourceMappingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SourceMappingID.
           *
           * @return The bounded list identified by SourceMappingID.
           */
         virtual uci::type::EntityQFT::SourceMappingID& getSourceMappingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SourceMappingID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSourceMappingID(const uci::type::EntityQFT::SourceMappingID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual const uci::type::EntityQFT::GeoFilter& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual uci::type::EntityQFT::GeoFilter& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeoFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeoFilter(const uci::type::EntityQFT::GeoFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilter.
           */
         virtual const uci::type::SourceFiltersType& getSourceFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilter.
           */
         virtual uci::type::SourceFiltersType& getSourceFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceFilter to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceFilter
           */
         virtual void setSourceFilter(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceFilter is emabled or not
           */
         virtual bool hasSourceFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceFilter
           *
           * @param type = uci::type::accessorType::sourceFiltersType This Accessor's accessor type
           */
         virtual void enableSourceFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::sourceFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceFilter */
         virtual void clearSourceFilter()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual const uci::type::EntityQFT::TemporalFilter& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual uci::type::EntityQFT::TemporalFilter& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TemporalFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTemporalFilter(const uci::type::EntityQFT::TemporalFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StandardIdentity.
           *
           * @return The bounded list identified by StandardIdentity.
           */
         virtual const uci::type::EntityQFT::StandardIdentity& getStandardIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StandardIdentity.
           *
           * @return The bounded list identified by StandardIdentity.
           */
         virtual uci::type::EntityQFT::StandardIdentity& getStandardIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the StandardIdentity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStandardIdentity(const uci::type::EntityQFT::StandardIdentity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual const uci::type::EntityQFT::Allegiance& getAllegiance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual uci::type::EntityQFT::Allegiance& getAllegiance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Allegiance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllegiance(const uci::type::EntityQFT::Allegiance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual const uci::type::EntityQFT::Platform& getPlatform() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual uci::type::EntityQFT::Platform& getPlatform()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Platform.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlatform(const uci::type::EntityQFT::Platform& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual const uci::type::EntityQFT::Specific& getSpecific() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual uci::type::EntityQFT::Specific& getSpecific()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Specific.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecific(const uci::type::EntityQFT::Specific& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::EntityQFT::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::EntityQFT::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::EntityQFT::Emitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual const uci::type::EntityQFT::SpecificEmitter& getSpecificEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual uci::type::EntityQFT::SpecificEmitter& getSpecificEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificEmitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificEmitter(const uci::type::EntityQFT::SpecificEmitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Mobility.
           *
           * @return The bounded list identified by Mobility.
           */
         virtual const uci::type::EntityQFT::Mobility& getMobility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Mobility.
           *
           * @return The bounded list identified by Mobility.
           */
         virtual uci::type::EntityQFT::Mobility& getMobility()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Mobility.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMobility(const uci::type::EntityQFT::Mobility& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Status.
           *
           * @return The bounded list identified by Status.
           */
         virtual const uci::type::EntityQFT::Status& getStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Status.
           *
           * @return The bounded list identified by Status.
           */
         virtual uci::type::EntityQFT::Status& getStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Status.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStatus(const uci::type::EntityQFT::Status& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityQFT to copy from
           */
         EntityQFT(const EntityQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityQFT to the contents of the EntityQFT on the right hand side
           * (rhs) of the assignment operator.EntityQFT [only available to derived classes]
           *
           * @param rhs The EntityQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::EntityQFT
           * @return A constant reference to this EntityQFT.
           */
         const EntityQFT& operator=(const EntityQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityQFT_h

