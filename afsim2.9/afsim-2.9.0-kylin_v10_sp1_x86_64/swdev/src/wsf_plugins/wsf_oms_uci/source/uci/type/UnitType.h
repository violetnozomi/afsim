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
#ifndef Uci__Type__UnitType_h
#define Uci__Type__UnitType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__UnitIdentityType_h)
# include "uci/type/UnitIdentityType.h"
#endif

#if !defined(Uci__Type__StandardIdentityConfidenceType_h)
# include "uci/type/StandardIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__CountryCodeType_h)
# include "uci/type/CountryCodeType.h"
#endif

#if !defined(Uci__Type__EOB_FixedPositionType_h)
# include "uci/type/EOB_FixedPositionType.h"
#endif

#if !defined(Uci__Type__OrderOfBattleTimestampsType_h)
# include "uci/type/OrderOfBattleTimestampsType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__EOB_OperationalStatusEnum_h)
# include "uci/type/EOB_OperationalStatusEnum.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__String20Type_h)
# include "uci/type/String20Type.h"
#endif

#if !defined(Uci__Type__String4Type_h)
# include "uci/type/String4Type.h"
#endif

#if !defined(Uci__Type__AirDefenseAreaType_h)
# include "uci/type/AirDefenseAreaType.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__BaseEOB_RecordType_h)
# include "uci/type/BaseEOB_RecordType.h"
#endif

#if !defined(Uci__Type__EOB_EquipmentRecordType_h)
# include "uci/type/EOB_EquipmentRecordType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Represents a Unit in an OB. */
      class UnitType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~UnitType()
         { }

         /** Returns this accessor's type constant, i.e. UnitType
           *
           * @return This accessor's type constant, i.e. UnitType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::unitType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const UnitType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 9] */
         typedef uci::base::IntAccessor Evaluation;
         //  The type of primitive data accessed by Evaluation
         typedef xs::Int EvaluationValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 99] */
         typedef uci::base::IntAccessor EchelonTier;
         //  The type of primitive data accessed by EchelonTier
         typedef xs::Int EchelonTierValue;

         /** Specifies any sites contained within the unit. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::BaseEOB_RecordType, uci::type::accessorType::baseEOB_RecordType> Site;

         /** Specifies equipment not identified with a site and contained within the unit. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EOB_EquipmentRecordType, uci::type::accessorType::eOB_EquipmentRecordType> Equipment;

         /** Returns the accessor that provides access to the complex content that is identified by the SecurityAttributes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityAttributes.
           */
         virtual const uci::type::SecurityInformationType& getSecurityAttributes() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityAttributes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityAttributes.
           */
         virtual uci::type::SecurityInformationType& getSecurityAttributes()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityAttributes to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityAttributes
           */
         virtual void setSecurityAttributes(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::UnitIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::UnitIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::UnitIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StandardIdentity.
           *
           * @return The acecssor that provides access to the complex content that is identified by StandardIdentity.
           */
         virtual const uci::type::StandardIdentityConfidenceType& getStandardIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StandardIdentity.
           *
           * @return The acecssor that provides access to the complex content that is identified by StandardIdentity.
           */
         virtual uci::type::StandardIdentityConfidenceType& getStandardIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StandardIdentity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StandardIdentity
           */
         virtual void setStandardIdentity(const uci::type::StandardIdentityConfidenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StandardIdentity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StandardIdentity is emabled or not
           */
         virtual bool hasStandardIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StandardIdentity
           *
           * @param type = uci::type::accessorType::standardIdentityConfidenceType This Accessor's accessor type
           */
         virtual void enableStandardIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::standardIdentityConfidenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StandardIdentity */
         virtual void clearStandardIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CountryCode.
           *
           * @return The value of the string data type identified by CountryCode.
           */
         virtual const uci::type::CountryCodeType& getCountryCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CountryCode.
           *
           * @return The value of the string data type identified by CountryCode.
           */
         virtual uci::type::CountryCodeType& getCountryCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CountryCode to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCountryCode(const uci::type::CountryCodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CountryCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCountryCode(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CountryCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCountryCode(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CountryCode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CountryCode is emabled or not
           */
         virtual bool hasCountryCode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CountryCode
           *
           * @param type = uci::type::accessorType::countryCodeType This Accessor's accessor type
           */
         virtual void enableCountryCode(uci::base::accessorType::AccessorType type = uci::type::accessorType::countryCodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CountryCode */
         virtual void clearCountryCode()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::EOB_FixedPositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::EOB_FixedPositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::EOB_FixedPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Position exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Position is emabled or not
           */
         virtual bool hasPosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Position
           *
           * @param type = uci::type::accessorType::eOB_FixedPositionType This Accessor's accessor type
           */
         virtual void enablePosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_FixedPositionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Position */
         virtual void clearPosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Timestamps.
           *
           * @return The acecssor that provides access to the complex content that is identified by Timestamps.
           */
         virtual const uci::type::OrderOfBattleTimestampsType& getTimestamps() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Timestamps.
           *
           * @return The acecssor that provides access to the complex content that is identified by Timestamps.
           */
         virtual uci::type::OrderOfBattleTimestampsType& getTimestamps()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Timestamps to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Timestamps
           */
         virtual void setTimestamps(const uci::type::OrderOfBattleTimestampsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Timestamps exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Timestamps is emabled or not
           */
         virtual bool hasTimestamps() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Timestamps
           *
           * @param type = uci::type::accessorType::orderOfBattleTimestampsType This Accessor's accessor type
           */
         virtual void enableTimestamps(uci::base::accessorType::AccessorType type = uci::type::accessorType::orderOfBattleTimestampsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Timestamps */
         virtual void clearTimestamps()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Evaluation.
           *
           * @return The value of the simple primitive data type identified by Evaluation.
           */
         virtual uci::type::UnitType::EvaluationValue getEvaluation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Evaluation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEvaluation(uci::type::UnitType::EvaluationValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Evaluation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Evaluation is emabled or not
           */
         virtual bool hasEvaluation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Evaluation
           *
           * @param type = uci::type::unitType::accessorType::evaluation This Accessor's accessor type
           */
         virtual void enableEvaluation(uci::base::accessorType::AccessorType type = uci::type::unitType::accessorType::evaluation)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Evaluation */
         virtual void clearEvaluation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalStatus.
           *
           * @return The value of the enumeration identified by OperationalStatus.
           */
         virtual const uci::type::EOB_OperationalStatusEnum& getOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalStatus.
           *
           * @return The value of the enumeration identified by OperationalStatus.
           */
         virtual uci::type::EOB_OperationalStatusEnum& getOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalStatus(const uci::type::EOB_OperationalStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalStatus(uci::type::EOB_OperationalStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield OperationalStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getOperationalStatus will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::eOB_OperationalStatusEnum This Accessor's accessor type
           */
         virtual void enableOperationalStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_OperationalStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PoliticalSubdivision.
           *
           * @return The value of the string data type identified by PoliticalSubdivision.
           */
         virtual const uci::type::VisibleString256Type& getPoliticalSubdivision() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PoliticalSubdivision.
           *
           * @return The value of the string data type identified by PoliticalSubdivision.
           */
         virtual uci::type::VisibleString256Type& getPoliticalSubdivision()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PoliticalSubdivision to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPoliticalSubdivision(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PoliticalSubdivision to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPoliticalSubdivision(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PoliticalSubdivision to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPoliticalSubdivision(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PoliticalSubdivision exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PoliticalSubdivision is emabled or not
           */
         virtual bool hasPoliticalSubdivision() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PoliticalSubdivision
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enablePoliticalSubdivision(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PoliticalSubdivision */
         virtual void clearPoliticalSubdivision()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AllegianceAlternate.
           *
           * @return The value of the string data type identified by AllegianceAlternate.
           */
         virtual const uci::type::CountryCodeType& getAllegianceAlternate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AllegianceAlternate.
           *
           * @return The value of the string data type identified by AllegianceAlternate.
           */
         virtual uci::type::CountryCodeType& getAllegianceAlternate()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AllegianceAlternate to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAllegianceAlternate(const uci::type::CountryCodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AllegianceAlternate to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAllegianceAlternate(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AllegianceAlternate to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAllegianceAlternate(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AllegianceAlternate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AllegianceAlternate is emabled or not
           */
         virtual bool hasAllegianceAlternate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AllegianceAlternate
           *
           * @param type = uci::type::accessorType::countryCodeType This Accessor's accessor type
           */
         virtual void enableAllegianceAlternate(uci::base::accessorType::AccessorType type = uci::type::accessorType::countryCodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AllegianceAlternate */
         virtual void clearAllegianceAlternate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DeploymentStatus.
           *
           * @return The value of the string data type identified by DeploymentStatus.
           */
         virtual const uci::type::String20Type& getDeploymentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DeploymentStatus.
           *
           * @return The value of the string data type identified by DeploymentStatus.
           */
         virtual uci::type::String20Type& getDeploymentStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DeploymentStatus to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDeploymentStatus(const uci::type::String20Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DeploymentStatus to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDeploymentStatus(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DeploymentStatus to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDeploymentStatus(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DeploymentStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DeploymentStatus is emabled or not
           */
         virtual bool hasDeploymentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DeploymentStatus
           *
           * @param type = uci::type::accessorType::string20Type This Accessor's accessor type
           */
         virtual void enableDeploymentStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::string20Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DeploymentStatus */
         virtual void clearDeploymentStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DivisionCategory.
           *
           * @return The value of the string data type identified by DivisionCategory.
           */
         virtual const uci::type::VisibleString256Type& getDivisionCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DivisionCategory.
           *
           * @return The value of the string data type identified by DivisionCategory.
           */
         virtual uci::type::VisibleString256Type& getDivisionCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DivisionCategory to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDivisionCategory(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DivisionCategory to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDivisionCategory(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DivisionCategory to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDivisionCategory(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DivisionCategory exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DivisionCategory is emabled or not
           */
         virtual bool hasDivisionCategory() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DivisionCategory
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableDivisionCategory(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DivisionCategory */
         virtual void clearDivisionCategory()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Echelon.
           *
           * @return The value of the string data type identified by Echelon.
           */
         virtual const uci::type::String20Type& getEchelon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Echelon.
           *
           * @return The value of the string data type identified by Echelon.
           */
         virtual uci::type::String20Type& getEchelon()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Echelon to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setEchelon(const uci::type::String20Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Echelon to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEchelon(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Echelon to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEchelon(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Echelon exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Echelon is emabled or not
           */
         virtual bool hasEchelon() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Echelon
           *
           * @param type = uci::type::accessorType::string20Type This Accessor's accessor type
           */
         virtual void enableEchelon(uci::base::accessorType::AccessorType type = uci::type::accessorType::string20Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Echelon */
         virtual void clearEchelon()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EchelonTier.
           *
           * @return The value of the simple primitive data type identified by EchelonTier.
           */
         virtual uci::type::UnitType::EchelonTierValue getEchelonTier() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EchelonTier.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEchelonTier(uci::type::UnitType::EchelonTierValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EchelonTier exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EchelonTier is emabled or not
           */
         virtual bool hasEchelonTier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EchelonTier
           *
           * @param type = uci::type::unitType::accessorType::echelonTier This Accessor's accessor type
           */
         virtual void enableEchelonTier(uci::base::accessorType::AccessorType type = uci::type::unitType::accessorType::echelonTier)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EchelonTier */
         virtual void clearEchelonTier()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FlagFlown.
           *
           * @return The value of the string data type identified by FlagFlown.
           */
         virtual const uci::type::CountryCodeType& getFlagFlown() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FlagFlown.
           *
           * @return The value of the string data type identified by FlagFlown.
           */
         virtual uci::type::CountryCodeType& getFlagFlown()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FlagFlown to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFlagFlown(const uci::type::CountryCodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FlagFlown to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFlagFlown(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FlagFlown to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFlagFlown(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FlagFlown exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FlagFlown is emabled or not
           */
         virtual bool hasFlagFlown() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FlagFlown
           *
           * @param type = uci::type::accessorType::countryCodeType This Accessor's accessor type
           */
         virtual void enableFlagFlown(uci::base::accessorType::AccessorType type = uci::type::accessorType::countryCodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FlagFlown */
         virtual void clearFlagFlown()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FleetAssignment.
           *
           * @return The value of the string data type identified by FleetAssignment.
           */
         virtual const uci::type::String4Type& getFleetAssignment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FleetAssignment.
           *
           * @return The value of the string data type identified by FleetAssignment.
           */
         virtual uci::type::String4Type& getFleetAssignment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FleetAssignment to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFleetAssignment(const uci::type::String4Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FleetAssignment to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFleetAssignment(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FleetAssignment to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFleetAssignment(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FleetAssignment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FleetAssignment is emabled or not
           */
         virtual bool hasFleetAssignment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FleetAssignment
           *
           * @param type = uci::type::accessorType::string4Type This Accessor's accessor type
           */
         virtual void enableFleetAssignment(uci::base::accessorType::AccessorType type = uci::type::accessorType::string4Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FleetAssignment */
         virtual void clearFleetAssignment()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AirDefenseArea.
           *
           * @return The value of the string data type identified by AirDefenseArea.
           */
         virtual const uci::type::AirDefenseAreaType& getAirDefenseArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AirDefenseArea.
           *
           * @return The value of the string data type identified by AirDefenseArea.
           */
         virtual uci::type::AirDefenseAreaType& getAirDefenseArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AirDefenseArea to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAirDefenseArea(const uci::type::AirDefenseAreaType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AirDefenseArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAirDefenseArea(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AirDefenseArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAirDefenseArea(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AirDefenseArea exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AirDefenseArea is emabled or not
           */
         virtual bool hasAirDefenseArea() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AirDefenseArea
           *
           * @param type = uci::type::accessorType::airDefenseAreaType This Accessor's accessor type
           */
         virtual void enableAirDefenseArea(uci::base::accessorType::AccessorType type = uci::type::accessorType::airDefenseAreaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AirDefenseArea */
         virtual void clearAirDefenseArea()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ForceAssignment.
           *
           * @return The value of the string data type identified by ForceAssignment.
           */
         virtual const uci::type::VisibleString256Type& getForceAssignment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ForceAssignment.
           *
           * @return The value of the string data type identified by ForceAssignment.
           */
         virtual uci::type::VisibleString256Type& getForceAssignment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ForceAssignment to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setForceAssignment(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ForceAssignment to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setForceAssignment(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ForceAssignment to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setForceAssignment(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ForceAssignment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ForceAssignment is emabled or not
           */
         virtual bool hasForceAssignment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ForceAssignment
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableForceAssignment(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ForceAssignment */
         virtual void clearForceAssignment()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ForceName.
           *
           * @return The value of the string data type identified by ForceName.
           */
         virtual const uci::type::VisibleString256Type& getForceName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ForceName.
           *
           * @return The value of the string data type identified by ForceName.
           */
         virtual uci::type::VisibleString256Type& getForceName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ForceName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setForceName(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ForceName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setForceName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ForceName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setForceName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ForceName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ForceName is emabled or not
           */
         virtual bool hasForceName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ForceName
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableForceName(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ForceName */
         virtual void clearForceName()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FunctionRole.
           *
           * @return The value of the string data type identified by FunctionRole.
           */
         virtual const uci::type::String20Type& getFunctionRole() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FunctionRole.
           *
           * @return The value of the string data type identified by FunctionRole.
           */
         virtual uci::type::String20Type& getFunctionRole()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FunctionRole to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFunctionRole(const uci::type::String20Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FunctionRole to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFunctionRole(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FunctionRole to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFunctionRole(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FunctionRole exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FunctionRole is emabled or not
           */
         virtual bool hasFunctionRole() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FunctionRole
           *
           * @param type = uci::type::accessorType::string20Type This Accessor's accessor type
           */
         virtual void enableFunctionRole(uci::base::accessorType::AccessorType type = uci::type::accessorType::string20Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FunctionRole */
         virtual void clearFunctionRole()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionPrimary.
           *
           * @return The value of the string data type identified by MissionPrimary.
           */
         virtual const uci::type::String4Type& getMissionPrimary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionPrimary.
           *
           * @return The value of the string data type identified by MissionPrimary.
           */
         virtual uci::type::String4Type& getMissionPrimary()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPrimary to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionPrimary(const uci::type::String4Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPrimary to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionPrimary(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPrimary to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionPrimary(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPrimary exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPrimary is emabled or not
           */
         virtual bool hasMissionPrimary() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPrimary
           *
           * @param type = uci::type::accessorType::string4Type This Accessor's accessor type
           */
         virtual void enableMissionPrimary(uci::base::accessorType::AccessorType type = uci::type::accessorType::string4Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPrimary */
         virtual void clearMissionPrimary()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionPrimarySpecialty.
           *
           * @return The value of the string data type identified by MissionPrimarySpecialty.
           */
         virtual const uci::type::String4Type& getMissionPrimarySpecialty() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionPrimarySpecialty.
           *
           * @return The value of the string data type identified by MissionPrimarySpecialty.
           */
         virtual uci::type::String4Type& getMissionPrimarySpecialty()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPrimarySpecialty to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionPrimarySpecialty(const uci::type::String4Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPrimarySpecialty to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionPrimarySpecialty(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPrimarySpecialty to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionPrimarySpecialty(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPrimarySpecialty exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPrimarySpecialty is emabled or not
           */
         virtual bool hasMissionPrimarySpecialty() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPrimarySpecialty
           *
           * @param type = uci::type::accessorType::string4Type This Accessor's accessor type
           */
         virtual void enableMissionPrimarySpecialty(uci::base::accessorType::AccessorType type = uci::type::accessorType::string4Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPrimarySpecialty */
         virtual void clearMissionPrimarySpecialty()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionSecondary.
           *
           * @return The value of the string data type identified by MissionSecondary.
           */
         virtual const uci::type::String4Type& getMissionSecondary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionSecondary.
           *
           * @return The value of the string data type identified by MissionSecondary.
           */
         virtual uci::type::String4Type& getMissionSecondary()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionSecondary to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionSecondary(const uci::type::String4Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionSecondary to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionSecondary(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionSecondary to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionSecondary(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionSecondary exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionSecondary is emabled or not
           */
         virtual bool hasMissionSecondary() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionSecondary
           *
           * @param type = uci::type::accessorType::string4Type This Accessor's accessor type
           */
         virtual void enableMissionSecondary(uci::base::accessorType::AccessorType type = uci::type::accessorType::string4Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionSecondary */
         virtual void clearMissionSecondary()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionSecondarySpecialty.
           *
           * @return The value of the string data type identified by MissionSecondarySpecialty.
           */
         virtual const uci::type::String4Type& getMissionSecondarySpecialty() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionSecondarySpecialty.
           *
           * @return The value of the string data type identified by MissionSecondarySpecialty.
           */
         virtual uci::type::String4Type& getMissionSecondarySpecialty()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionSecondarySpecialty to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionSecondarySpecialty(const uci::type::String4Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionSecondarySpecialty to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionSecondarySpecialty(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionSecondarySpecialty to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionSecondarySpecialty(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionSecondarySpecialty exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionSecondarySpecialty is emabled or not
           */
         virtual bool hasMissionSecondarySpecialty() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionSecondarySpecialty
           *
           * @param type = uci::type::accessorType::string4Type This Accessor's accessor type
           */
         virtual void enableMissionSecondarySpecialty(uci::base::accessorType::AccessorType type = uci::type::accessorType::string4Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionSecondarySpecialty */
         virtual void clearMissionSecondarySpecialty()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NuclearCapacity.
           *
           * @return The value of the string data type identified by NuclearCapacity.
           */
         virtual const uci::type::String20Type& getNuclearCapacity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NuclearCapacity.
           *
           * @return The value of the string data type identified by NuclearCapacity.
           */
         virtual uci::type::String20Type& getNuclearCapacity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NuclearCapacity to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setNuclearCapacity(const uci::type::String20Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NuclearCapacity to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNuclearCapacity(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NuclearCapacity to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNuclearCapacity(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NuclearCapacity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NuclearCapacity is emabled or not
           */
         virtual bool hasNuclearCapacity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NuclearCapacity
           *
           * @param type = uci::type::accessorType::string20Type This Accessor's accessor type
           */
         virtual void enableNuclearCapacity(uci::base::accessorType::AccessorType type = uci::type::accessorType::string20Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NuclearCapacity */
         virtual void clearNuclearCapacity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ParentUnitID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ParentUnitID.
           */
         virtual const uci::type::ID_Type& getParentUnitID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ParentUnitID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ParentUnitID.
           */
         virtual uci::type::ID_Type& getParentUnitID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ParentUnitID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ParentUnitID
           */
         virtual void setParentUnitID(const uci::type::ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ParentUnitID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ParentUnitID is emabled or not
           */
         virtual bool hasParentUnitID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ParentUnitID
           *
           * @param type = uci::type::accessorType::iD_Type This Accessor's accessor type
           */
         virtual void enableParentUnitID(uci::base::accessorType::AccessorType type = uci::type::accessorType::iD_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ParentUnitID */
         virtual void clearParentUnitID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Site.
           *
           * @return The bounded list identified by Site.
           */
         virtual const uci::type::UnitType::Site& getSite() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Site.
           *
           * @return The bounded list identified by Site.
           */
         virtual uci::type::UnitType::Site& getSite()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Site.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSite(const uci::type::UnitType::Site& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Equipment.
           *
           * @return The bounded list identified by Equipment.
           */
         virtual const uci::type::UnitType::Equipment& getEquipment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Equipment.
           *
           * @return The bounded list identified by Equipment.
           */
         virtual uci::type::UnitType::Equipment& getEquipment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Equipment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEquipment(const uci::type::UnitType::Equipment& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         UnitType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The UnitType to copy from
           */
         UnitType(const UnitType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this UnitType to the contents of the UnitType on the right hand side
           * (rhs) of the assignment operator.UnitType [only available to derived classes]
           *
           * @param rhs The UnitType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::UnitType
           * @return A constant reference to this UnitType.
           */
         const UnitType& operator=(const UnitType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // UnitType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__UnitType_h

