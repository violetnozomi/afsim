// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__WorkingEOB_QFT_h
#define Uci__Type__WorkingEOB_QFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__WorkingEOB_ID_Type_h)
# include "uci/type/WorkingEOB_ID_Type.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationListID_Type_h)
# include "uci/type/ThreatTargetPrioritizationListID_Type.h"
#endif

#if !defined(Uci__Type__SignalIdentityQueryType_h)
# include "uci/type/SignalIdentityQueryType.h"
#endif

#if !defined(Uci__Type__SignalParametricQueryType_h)
# include "uci/type/SignalParametricQueryType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__CED_RestrictedNameType_h)
# include "uci/type/CED_RestrictedNameType.h"
#endif

#if !defined(Uci__Base__UnsignedShortAccessor_h)
# include "uci/base/UnsignedShortAccessor.h"
#endif

#if !defined(Uci__Type__EOB_RecordID_Type_h)
# include "uci/type/EOB_RecordID_Type.h"
#endif

#if !defined(Uci__Type__BasicEncyclopediaNumberType_h)
# include "uci/type/BasicEncyclopediaNumberType.h"
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

#if !defined(Uci__Type__StandardIdentityEnum_h)
# include "uci/type/StandardIdentityEnum.h"
#endif

#if !defined(Uci__Type__PlatformIdentityType_h)
# include "uci/type/PlatformIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificIdentityType_h)
# include "uci/type/SpecificIdentityType.h"
#endif

#if !defined(Uci__Type__CountryCodeType_h)
# include "uci/type/CountryCodeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WorkingEOB_QFT sequence accessor class */
      class WorkingEOB_QFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~WorkingEOB_QFT()
         { }

         /** Returns this accessor's type constant, i.e. WorkingEOB_QFT
           *
           * @return This accessor's type constant, i.e. WorkingEOB_QFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::workingEOB_QFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WorkingEOB_QFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a filter based on unique identifiers [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::WorkingEOB_ID_Type, uci::type::accessorType::workingEOB_ID_Type> WorkingEOB_ID;

         /** The range of Date/times that an emitter was active to query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> EmitterLastObservationRange;

         /** The CED Name of the emitter to query. See the Combined Emitter Database (CED) NSA/CSS NTIPS/DASS for more
           * information. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CED_RestrictedNameType, uci::type::accessorType::cED_RestrictedNameType> CED_Name;

         /** The EOB_Record ID to query. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::EOB_RecordID_Type, uci::type::accessorType::eOB_RecordID_Type> EOB_RecordID;

         /** The EOB_Record Basic Encyclopedia Number to query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::BasicEncyclopediaNumberType, uci::type::accessorType::basicEncyclopediaNumberType> BE_Number;

         /** Indicates the environment or "domain" in which the Entity is operating. Derived from MIL-STD-6016; see MIL-STD-6016
           * DFI/DUI 275/004 for additional details. Each environment has a corresponding set of specific types. [Minimum
           * occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::EnvironmentEnum, uci::type::accessorType::environmentEnum> Environment;

         /** The originator of the entity to query. [Minimum occurrences: 0] [Maximum occurrences: 25] */
         typedef uci::base::BoundedList<uci::type::EntitySourceEnum, uci::type::accessorType::entitySourceEnum> Originator;

         /** The associated ID of the EOB_Record to query. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
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

         /** Indicates a possible "platform identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PlatformIdentityType, uci::type::accessorType::platformIdentityType> Platform;

         /** Indicates a possible "specific identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificIdentityType, uci::type::accessorType::specificIdentityType> Specific;

         /** DOD standard country code designator to which an entity owes its allegiance. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CountryCodeType, uci::type::accessorType::countryCodeType> Allegiance;

         /** Returns the bounded list that is identified by the WorkingEOB_ID.
           *
           * @return The bounded list identified by WorkingEOB_ID.
           */
         virtual const uci::type::WorkingEOB_QFT::WorkingEOB_ID& getWorkingEOB_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WorkingEOB_ID.
           *
           * @return The bounded list identified by WorkingEOB_ID.
           */
         virtual uci::type::WorkingEOB_QFT::WorkingEOB_ID& getWorkingEOB_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the WorkingEOB_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWorkingEOB_ID(const uci::type::WorkingEOB_QFT::WorkingEOB_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual const uci::type::OpZoneID_Type& getOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual uci::type::OpZoneID_Type& getOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpZoneID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpZoneID
           */
         virtual void setOpZoneID(const uci::type::OpZoneID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpZoneID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpZoneID is emabled or not
           */
         virtual bool hasOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpZoneID
           *
           * @param type = uci::type::accessorType::opZoneID_Type This Accessor's accessor type
           */
         virtual void enableOpZoneID(uci::base::accessorType::AccessorType type = uci::type::accessorType::opZoneID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpZoneID */
         virtual void clearOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ThreatTargetPrioritizationListID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ThreatTargetPrioritizationListID.
           */
         virtual const uci::type::ThreatTargetPrioritizationListID_Type& getThreatTargetPrioritizationListID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ThreatTargetPrioritizationListID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ThreatTargetPrioritizationListID.
           */
         virtual uci::type::ThreatTargetPrioritizationListID_Type& getThreatTargetPrioritizationListID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ThreatTargetPrioritizationListID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ThreatTargetPrioritizationListID
           */
         virtual void setThreatTargetPrioritizationListID(const uci::type::ThreatTargetPrioritizationListID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ThreatTargetPrioritizationListID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ThreatTargetPrioritizationListID is emabled or not
           */
         virtual bool hasThreatTargetPrioritizationListID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ThreatTargetPrioritizationListID
           *
           * @param type = uci::type::accessorType::threatTargetPrioritizationListID_Type This Accessor's accessor type
           */
         virtual void enableThreatTargetPrioritizationListID(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatTargetPrioritizationListID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ThreatTargetPrioritizationListID */
         virtual void clearThreatTargetPrioritizationListID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalIdentities.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalIdentities.
           */
         virtual const uci::type::SignalIdentityQueryType& getSignalIdentities() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalIdentities.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalIdentities.
           */
         virtual uci::type::SignalIdentityQueryType& getSignalIdentities()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalIdentities to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalIdentities
           */
         virtual void setSignalIdentities(const uci::type::SignalIdentityQueryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalIdentities exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalIdentities is emabled or not
           */
         virtual bool hasSignalIdentities() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalIdentities
           *
           * @param type = uci::type::accessorType::signalIdentityQueryType This Accessor's accessor type
           */
         virtual void enableSignalIdentities(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalIdentityQueryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalIdentities */
         virtual void clearSignalIdentities()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalParametrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalParametrics.
           */
         virtual const uci::type::SignalParametricQueryType& getSignalParametrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalParametrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalParametrics.
           */
         virtual uci::type::SignalParametricQueryType& getSignalParametrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalParametrics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalParametrics
           */
         virtual void setSignalParametrics(const uci::type::SignalParametricQueryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalParametrics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalParametrics is emabled or not
           */
         virtual bool hasSignalParametrics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalParametrics
           *
           * @param type = uci::type::accessorType::signalParametricQueryType This Accessor's accessor type
           */
         virtual void enableSignalParametrics(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalParametricQueryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalParametrics */
         virtual void clearSignalParametrics()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EmitterLastObservationRange.
           *
           * @return The bounded list identified by EmitterLastObservationRange.
           */
         virtual const uci::type::WorkingEOB_QFT::EmitterLastObservationRange& getEmitterLastObservationRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EmitterLastObservationRange.
           *
           * @return The bounded list identified by EmitterLastObservationRange.
           */
         virtual uci::type::WorkingEOB_QFT::EmitterLastObservationRange& getEmitterLastObservationRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EmitterLastObservationRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitterLastObservationRange(const uci::type::WorkingEOB_QFT::EmitterLastObservationRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CED_Name.
           *
           * @return The bounded list identified by CED_Name.
           */
         virtual const uci::type::WorkingEOB_QFT::CED_Name& getCED_Name() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CED_Name.
           *
           * @return The bounded list identified by CED_Name.
           */
         virtual uci::type::WorkingEOB_QFT::CED_Name& getCED_Name()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CED_Name.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCED_Name(const uci::type::WorkingEOB_QFT::CED_Name& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfDaysToGenOpsClock.
           *
           * @return The value of the simple primitive data type identified by NumberOfDaysToGenOpsClock.
           */
         virtual xs::UnsignedShort getNumberOfDaysToGenOpsClock() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfDaysToGenOpsClock.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfDaysToGenOpsClock(xs::UnsignedShort value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumberOfDaysToGenOpsClock exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumberOfDaysToGenOpsClock is emabled or not
           */
         virtual bool hasNumberOfDaysToGenOpsClock() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumberOfDaysToGenOpsClock
           *
           * @param type = uci::base::accessorType::unsignedShortAccessor This Accessor's accessor type
           */
         virtual void enableNumberOfDaysToGenOpsClock(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedShortAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumberOfDaysToGenOpsClock */
         virtual void clearNumberOfDaysToGenOpsClock()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EOB_RecordID.
           *
           * @return The bounded list identified by EOB_RecordID.
           */
         virtual const uci::type::WorkingEOB_QFT::EOB_RecordID& getEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EOB_RecordID.
           *
           * @return The bounded list identified by EOB_RecordID.
           */
         virtual uci::type::WorkingEOB_QFT::EOB_RecordID& getEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EOB_RecordID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEOB_RecordID(const uci::type::WorkingEOB_QFT::EOB_RecordID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BE_Number.
           *
           * @return The bounded list identified by BE_Number.
           */
         virtual const uci::type::WorkingEOB_QFT::BE_Number& getBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BE_Number.
           *
           * @return The bounded list identified by BE_Number.
           */
         virtual uci::type::WorkingEOB_QFT::BE_Number& getBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BE_Number.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBE_Number(const uci::type::WorkingEOB_QFT::BE_Number& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual const uci::type::WorkingEOB_QFT::Environment& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual uci::type::WorkingEOB_QFT::Environment& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Environment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnvironment(const uci::type::WorkingEOB_QFT::Environment& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Originator.
           *
           * @return The bounded list identified by Originator.
           */
         virtual const uci::type::WorkingEOB_QFT::Originator& getOriginator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Originator.
           *
           * @return The bounded list identified by Originator.
           */
         virtual uci::type::WorkingEOB_QFT::Originator& getOriginator()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Originator.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOriginator(const uci::type::WorkingEOB_QFT::Originator& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedID.
           *
           * @return The bounded list identified by AssociatedID.
           */
         virtual const uci::type::WorkingEOB_QFT::AssociatedID& getAssociatedID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedID.
           *
           * @return The bounded list identified by AssociatedID.
           */
         virtual uci::type::WorkingEOB_QFT::AssociatedID& getAssociatedID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedID(const uci::type::WorkingEOB_QFT::AssociatedID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SourceMappingID.
           *
           * @return The bounded list identified by SourceMappingID.
           */
         virtual const uci::type::WorkingEOB_QFT::SourceMappingID& getSourceMappingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SourceMappingID.
           *
           * @return The bounded list identified by SourceMappingID.
           */
         virtual uci::type::WorkingEOB_QFT::SourceMappingID& getSourceMappingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SourceMappingID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSourceMappingID(const uci::type::WorkingEOB_QFT::SourceMappingID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual const uci::type::WorkingEOB_QFT::GeoFilter& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual uci::type::WorkingEOB_QFT::GeoFilter& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeoFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeoFilter(const uci::type::WorkingEOB_QFT::GeoFilter& value)
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
         virtual const uci::type::WorkingEOB_QFT::TemporalFilter& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual uci::type::WorkingEOB_QFT::TemporalFilter& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TemporalFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTemporalFilter(const uci::type::WorkingEOB_QFT::TemporalFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StandardIdentity.
           *
           * @return The bounded list identified by StandardIdentity.
           */
         virtual const uci::type::WorkingEOB_QFT::StandardIdentity& getStandardIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StandardIdentity.
           *
           * @return The bounded list identified by StandardIdentity.
           */
         virtual uci::type::WorkingEOB_QFT::StandardIdentity& getStandardIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the StandardIdentity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStandardIdentity(const uci::type::WorkingEOB_QFT::StandardIdentity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual const uci::type::WorkingEOB_QFT::Platform& getPlatform() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual uci::type::WorkingEOB_QFT::Platform& getPlatform()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Platform.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlatform(const uci::type::WorkingEOB_QFT::Platform& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual const uci::type::WorkingEOB_QFT::Specific& getSpecific() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual uci::type::WorkingEOB_QFT::Specific& getSpecific()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Specific.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecific(const uci::type::WorkingEOB_QFT::Specific& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual const uci::type::WorkingEOB_QFT::Allegiance& getAllegiance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual uci::type::WorkingEOB_QFT::Allegiance& getAllegiance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Allegiance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllegiance(const uci::type::WorkingEOB_QFT::Allegiance& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WorkingEOB_QFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WorkingEOB_QFT to copy from
           */
         WorkingEOB_QFT(const WorkingEOB_QFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WorkingEOB_QFT to the contents of the WorkingEOB_QFT on the right
           * hand side (rhs) of the assignment operator.WorkingEOB_QFT [only available to derived classes]
           *
           * @param rhs The WorkingEOB_QFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::WorkingEOB_QFT
           * @return A constant reference to this WorkingEOB_QFT.
           */
         const WorkingEOB_QFT& operator=(const WorkingEOB_QFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WorkingEOB_QFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WorkingEOB_QFT_h

