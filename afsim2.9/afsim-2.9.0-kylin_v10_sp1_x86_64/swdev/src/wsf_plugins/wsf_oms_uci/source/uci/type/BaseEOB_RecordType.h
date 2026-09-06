// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__BaseEOB_RecordType_h
#define Uci__Type__BaseEOB_RecordType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__EOB_RecordID_Type_h)
# include "uci/type/EOB_RecordID_Type.h"
#endif

#if !defined(Uci__Type__OrderOfBattleTimestampsType_h)
# include "uci/type/OrderOfBattleTimestampsType.h"
#endif

#if !defined(Uci__Type__IdentityType_h)
# include "uci/type/IdentityType.h"
#endif

#if !defined(Uci__Type__EOB_FixedPositionType_h)
# include "uci/type/EOB_FixedPositionType.h"
#endif

#if !defined(Uci__Type__MobilityEnum_h)
# include "uci/type/MobilityEnum.h"
#endif

#if !defined(Uci__Type__EOB_CodesType_h)
# include "uci/type/EOB_CodesType.h"
#endif

#if !defined(Uci__Type__EOB_OperationalStatusEnum_h)
# include "uci/type/EOB_OperationalStatusEnum.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Type__VisibleString512Type_h)
# include "uci/type/VisibleString512Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the BaseEOB_RecordType sequence accessor class */
      class BaseEOB_RecordType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~BaseEOB_RecordType()
         { }

         /** Returns this accessor's type constant, i.e. BaseEOB_RecordType
           *
           * @return This accessor's type constant, i.e. BaseEOB_RecordType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::baseEOB_RecordType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const BaseEOB_RecordType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [White space: preserve] [Minimum length: 0] [Maximum length: 3] [Pattern: "[A-Za-z0-9 \-_]{0,3}"] */
         typedef xs::String RecordOwner;

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


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual const uci::type::EOB_RecordID_Type& getEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual uci::type::EOB_RecordID_Type& getEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB_RecordID
           */
         virtual void setEOB_RecordID(const uci::type::EOB_RecordID_Type& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::IdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::IdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::IdentityType& value)
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


         /** Returns the value of the enumeration that is identified by the Mobility.
           *
           * @return The value of the enumeration identified by Mobility.
           */
         virtual const uci::type::MobilityEnum& getMobility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mobility.
           *
           * @return The value of the enumeration identified by Mobility.
           */
         virtual uci::type::MobilityEnum& getMobility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mobility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMobility(const uci::type::MobilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mobility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMobility(uci::type::MobilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Mobilityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMobility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMobility will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::mobilityEnum This Accessor's accessor type
           */
         virtual void enableMobility(uci::base::accessorType::AccessorType type = uci::type::accessorType::mobilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMobility()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_Codes.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_Codes.
           */
         virtual const uci::type::EOB_CodesType& getEOB_Codes() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_Codes.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_Codes.
           */
         virtual uci::type::EOB_CodesType& getEOB_Codes()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB_Codes to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB_Codes
           */
         virtual void setEOB_Codes(const uci::type::EOB_CodesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EOB_Codes exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EOB_Codes is emabled or not
           */
         virtual bool hasEOB_Codes() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EOB_Codes
           *
           * @param type = uci::type::accessorType::eOB_CodesType This Accessor's accessor type
           */
         virtual void enableEOB_Codes(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_CodesType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EOB_Codes */
         virtual void clearEOB_Codes()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SiteOperationalStatus.
           *
           * @return The value of the enumeration identified by SiteOperationalStatus.
           */
         virtual const uci::type::EOB_OperationalStatusEnum& getSiteOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SiteOperationalStatus.
           *
           * @return The value of the enumeration identified by SiteOperationalStatus.
           */
         virtual uci::type::EOB_OperationalStatusEnum& getSiteOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SiteOperationalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSiteOperationalStatus(const uci::type::EOB_OperationalStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SiteOperationalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSiteOperationalStatus(uci::type::EOB_OperationalStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SiteOperationalStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSiteOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSiteOperationalStatus will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::eOB_OperationalStatusEnum This Accessor's accessor type
           */
         virtual void enableSiteOperationalStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_OperationalStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSiteOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RecordOwner.
           *
           * @return The value of the string data type identified by RecordOwner.
           */
         virtual const uci::type::BaseEOB_RecordType::RecordOwner& getRecordOwner() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RecordOwner.
           *
           * @return The value of the string data type identified by RecordOwner.
           */
         virtual uci::type::BaseEOB_RecordType::RecordOwner& getRecordOwner()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RecordOwner to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRecordOwner(const uci::type::BaseEOB_RecordType::RecordOwner& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RecordOwner to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRecordOwner(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RecordOwner to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRecordOwner(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RecordOwner exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RecordOwner is emabled or not
           */
         virtual bool hasRecordOwner() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RecordOwner
           *
           * @param type = uci::type::baseEOB_RecordType::accessorType::recordOwner This Accessor's accessor type
           */
         virtual void enableRecordOwner(uci::base::accessorType::AccessorType type = uci::type::baseEOB_RecordType::accessorType::recordOwner)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RecordOwner */
         virtual void clearRecordOwner()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RemarksText.
           *
           * @return The value of the string data type identified by RemarksText.
           */
         virtual const uci::type::VisibleString512Type& getRemarksText() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RemarksText.
           *
           * @return The value of the string data type identified by RemarksText.
           */
         virtual uci::type::VisibleString512Type& getRemarksText()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RemarksText to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRemarksText(const uci::type::VisibleString512Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RemarksText to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarksText(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RemarksText to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarksText(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemarksText exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemarksText is emabled or not
           */
         virtual bool hasRemarksText() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemarksText
           *
           * @param type = uci::type::accessorType::visibleString512Type This Accessor's accessor type
           */
         virtual void enableRemarksText(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString512Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemarksText */
         virtual void clearRemarksText()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         BaseEOB_RecordType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The BaseEOB_RecordType to copy from
           */
         BaseEOB_RecordType(const BaseEOB_RecordType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this BaseEOB_RecordType to the contents of the BaseEOB_RecordType on
           * the right hand side (rhs) of the assignment operator.BaseEOB_RecordType [only available to derived classes]
           *
           * @param rhs The BaseEOB_RecordType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::BaseEOB_RecordType
           * @return A constant reference to this BaseEOB_RecordType.
           */
         const BaseEOB_RecordType& operator=(const BaseEOB_RecordType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // BaseEOB_RecordType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__BaseEOB_RecordType_h

