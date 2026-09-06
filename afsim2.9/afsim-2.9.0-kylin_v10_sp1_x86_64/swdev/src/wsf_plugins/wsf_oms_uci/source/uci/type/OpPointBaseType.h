// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__OpPointBaseType_h
#define Uci__Type__OpPointBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OpPointID_Type_h)
# include "uci/type/OpPointID_Type.h"
#endif

#if !defined(Uci__Type__OpPointPositionType_h)
# include "uci/type/OpPointPositionType.h"
#endif

#if !defined(Uci__Type__OpDescriptionType_h)
# include "uci/type/OpDescriptionType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemConfigurationType_h)
# include "uci/type/SystemConfigurationType.h"
#endif

#if !defined(Uci__Type__MissionTraceabilityType_h)
# include "uci/type/MissionTraceabilityType.h"
#endif

#if !defined(Uci__Type__TimeFunctionType_h)
# include "uci/type/TimeFunctionType.h"
#endif

#if !defined(Uci__Type__DataLinkIdentifierPET_h)
# include "uci/type/DataLinkIdentifierPET.h"
#endif

#if !defined(Uci__Type__QualifyingTagsType_h)
# include "uci/type/QualifyingTagsType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__RelativeDirectionEnum_h)
# include "uci/type/RelativeDirectionEnum.h"
#endif

#if !defined(Uci__Type__AnglePairType_h)
# include "uci/type/AnglePairType.h"
#endif

#if !defined(Uci__Type__SafeAltitudeType_h)
# include "uci/type/SafeAltitudeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Base Type for OpPointMDT, which does not include a Category */
      class OpPointBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpPointBaseType()
         { }

         /** Returns this accessor's type constant, i.e. OpPointBaseType
           *
           * @return This accessor's type constant, i.e. OpPointBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opPointBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpPointBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a System which the OpPoint is applicable for and its planned configuration/state upon arriving at the
           * OpPoint. If omitted, the OpPoint applies to all Systems. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemConfigurationType, uci::type::accessorType::systemConfigurationType> SystemConfiguration;

         /** Indicates time related events that are associated to the OpPoint such as when activated or when operational. The
           * multiplicity is limited to N-1 options where NONE is an event that has no value, thus removed from list of available
           * options. [Minimum occurrences: 0] [Maximum occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::TimeFunctionType, uci::type::accessorType::timeFunctionType> AssociatedTime;

         /** List of datalink ID. Multiple datalink IDs can be reported for the same network type. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataLinkIdentifierPET, uci::type::accessorType::dataLinkIdentifierPET> DataLinkIdentifier;

         /** Returns the accessor that provides access to the complex content that is identified by the OpPointID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpPointID.
           */
         virtual const uci::type::OpPointID_Type& getOpPointID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpPointID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpPointID.
           */
         virtual uci::type::OpPointID_Type& getOpPointID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpPointID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpPointID
           */
         virtual void setOpPointID(const uci::type::OpPointID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Point.
           *
           * @return The acecssor that provides access to the complex content that is identified by Point.
           */
         virtual const uci::type::OpPointPositionType& getPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Point.
           *
           * @return The acecssor that provides access to the complex content that is identified by Point.
           */
         virtual uci::type::OpPointPositionType& getPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Point to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Point
           */
         virtual void setPoint(const uci::type::OpPointPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpDescription.
           */
         virtual const uci::type::OpDescriptionType& getOpDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpDescription.
           */
         virtual uci::type::OpDescriptionType& getOpDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpDescription to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpDescription
           */
         virtual void setOpDescription(const uci::type::OpDescriptionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpDescription exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpDescription is emabled or not
           */
         virtual bool hasOpDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpDescription
           *
           * @param type = uci::type::accessorType::opDescriptionType This Accessor's accessor type
           */
         virtual void enableOpDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::opDescriptionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpDescription */
         virtual void clearOpDescription()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemConfiguration.
           *
           * @return The bounded list identified by SystemConfiguration.
           */
         virtual const uci::type::OpPointBaseType::SystemConfiguration& getSystemConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemConfiguration.
           *
           * @return The bounded list identified by SystemConfiguration.
           */
         virtual uci::type::OpPointBaseType::SystemConfiguration& getSystemConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemConfiguration.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemConfiguration(const uci::type::OpPointBaseType::SystemConfiguration& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionTraceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionTraceability.
           */
         virtual const uci::type::MissionTraceabilityType& getMissionTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionTraceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionTraceability.
           */
         virtual uci::type::MissionTraceabilityType& getMissionTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionTraceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionTraceability
           */
         virtual void setMissionTraceability(const uci::type::MissionTraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionTraceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionTraceability is emabled or not
           */
         virtual bool hasMissionTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionTraceability
           *
           * @param type = uci::type::accessorType::missionTraceabilityType This Accessor's accessor type
           */
         virtual void enableMissionTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionTraceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionTraceability */
         virtual void clearMissionTraceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedTime.
           *
           * @return The bounded list identified by AssociatedTime.
           */
         virtual const uci::type::OpPointBaseType::AssociatedTime& getAssociatedTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedTime.
           *
           * @return The bounded list identified by AssociatedTime.
           */
         virtual uci::type::OpPointBaseType::AssociatedTime& getAssociatedTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedTime.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedTime(const uci::type::OpPointBaseType::AssociatedTime& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkIdentifier.
           *
           * @return The bounded list identified by DataLinkIdentifier.
           */
         virtual const uci::type::OpPointBaseType::DataLinkIdentifier& getDataLinkIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkIdentifier.
           *
           * @return The bounded list identified by DataLinkIdentifier.
           */
         virtual uci::type::OpPointBaseType::DataLinkIdentifier& getDataLinkIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataLinkIdentifier.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataLinkIdentifier(const uci::type::OpPointBaseType::DataLinkIdentifier& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QualifyingTags.
           *
           * @return The acecssor that provides access to the complex content that is identified by QualifyingTags.
           */
         virtual const uci::type::QualifyingTagsType& getQualifyingTags() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QualifyingTags.
           *
           * @return The acecssor that provides access to the complex content that is identified by QualifyingTags.
           */
         virtual uci::type::QualifyingTagsType& getQualifyingTags()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the QualifyingTags to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by QualifyingTags
           */
         virtual void setQualifyingTags(const uci::type::QualifyingTagsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by QualifyingTags exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by QualifyingTags is emabled or not
           */
         virtual bool hasQualifyingTags() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by QualifyingTags
           *
           * @param type = uci::type::accessorType::qualifyingTagsType This Accessor's accessor type
           */
         virtual void enableQualifyingTags(uci::base::accessorType::AccessorType type = uci::type::accessorType::qualifyingTagsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by QualifyingTags */
         virtual void clearQualifyingTags()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Priority.
           *
           * @return The value of the simple primitive data type identified by Priority.
           */
         virtual xs::UnsignedInt getPriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Priority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPriority(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Priority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Priority is emabled or not
           */
         virtual bool hasPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Priority
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enablePriority(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Priority */
         virtual void clearPriority()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TurnDirection.
           *
           * @return The value of the enumeration identified by TurnDirection.
           */
         virtual const uci::type::RelativeDirectionEnum& getTurnDirection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TurnDirection.
           *
           * @return The value of the enumeration identified by TurnDirection.
           */
         virtual uci::type::RelativeDirectionEnum& getTurnDirection()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TurnDirection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurnDirection(const uci::type::RelativeDirectionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TurnDirection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurnDirection(uci::type::RelativeDirectionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TurnDirectionis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTurnDirection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTurnDirection will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::relativeDirectionEnum This Accessor's accessor type
           */
         virtual void enableTurnDirection(uci::base::accessorType::AccessorType type = uci::type::accessorType::relativeDirectionEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTurnDirection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IngressConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by IngressConstraint.
           */
         virtual const uci::type::AnglePairType& getIngressConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IngressConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by IngressConstraint.
           */
         virtual uci::type::AnglePairType& getIngressConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the IngressConstraint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by IngressConstraint
           */
         virtual void setIngressConstraint(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IngressConstraint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IngressConstraint is emabled or not
           */
         virtual bool hasIngressConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IngressConstraint
           *
           * @param type = uci::type::accessorType::anglePairType This Accessor's accessor type
           */
         virtual void enableIngressConstraint(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IngressConstraint */
         virtual void clearIngressConstraint()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SafeAltitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by SafeAltitude.
           */
         virtual const uci::type::SafeAltitudeType& getSafeAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SafeAltitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by SafeAltitude.
           */
         virtual uci::type::SafeAltitudeType& getSafeAltitude()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SafeAltitude to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SafeAltitude
           */
         virtual void setSafeAltitude(const uci::type::SafeAltitudeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SafeAltitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SafeAltitude is emabled or not
           */
         virtual bool hasSafeAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SafeAltitude
           *
           * @param type = uci::type::accessorType::safeAltitudeType This Accessor's accessor type
           */
         virtual void enableSafeAltitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::safeAltitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SafeAltitude */
         virtual void clearSafeAltitude()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpPointBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpPointBaseType to copy from
           */
         OpPointBaseType(const OpPointBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpPointBaseType to the contents of the OpPointBaseType on the
           * right hand side (rhs) of the assignment operator.OpPointBaseType [only available to derived classes]
           *
           * @param rhs The OpPointBaseType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::OpPointBaseType
           * @return A constant reference to this OpPointBaseType.
           */
         const OpPointBaseType& operator=(const OpPointBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpPointBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpPointBaseType_h

