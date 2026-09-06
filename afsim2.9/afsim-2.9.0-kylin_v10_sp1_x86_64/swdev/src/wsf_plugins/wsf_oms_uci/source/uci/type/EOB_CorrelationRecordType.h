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
#ifndef Uci__Type__EOB_CorrelationRecordType_h
#define Uci__Type__EOB_CorrelationRecordType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__EOB_CorrelationRecordID_Type_h)
# include "uci/type/EOB_CorrelationRecordID_Type.h"
#endif

#if !defined(Uci__Type__EOB_CorrelationTypeEnum_h)
# include "uci/type/EOB_CorrelationTypeEnum.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__EOB_CorrelatedToType_h)
# include "uci/type/EOB_CorrelatedToType.h"
#endif

#if !defined(Uci__Type__EOB_CorrelationInputType_h)
# include "uci/type/EOB_CorrelationInputType.h"
#endif

#if !defined(Uci__Type__EOB_SignalType_h)
# include "uci/type/EOB_SignalType.h"
#endif

#if !defined(Uci__Type__IdentityConfidenceType_h)
# include "uci/type/IdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__NotationType_h)
# include "uci/type/NotationType.h"
#endif

#if !defined(Uci__Type__FixedPositionType_h)
# include "uci/type/FixedPositionType.h"
#endif

#if !defined(Uci__Type__KinematicsFixedType_h)
# include "uci/type/KinematicsFixedType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This represents results of previous attempts to correlate SignalReports, Entities or other inputs to the EOB. */
      class EOB_CorrelationRecordType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EOB_CorrelationRecordType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_CorrelationRecordType
           *
           * @return This accessor's type constant, i.e. EOB_CorrelationRecordType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_CorrelationRecordType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_CorrelationRecordType& accessor)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_CorrelationRecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_CorrelationRecordID.
           */
         virtual const uci::type::EOB_CorrelationRecordID_Type& getEOB_CorrelationRecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_CorrelationRecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_CorrelationRecordID.
           */
         virtual uci::type::EOB_CorrelationRecordID_Type& getEOB_CorrelationRecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB_CorrelationRecordID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB_CorrelationRecordID
           */
         virtual void setEOB_CorrelationRecordID(const uci::type::EOB_CorrelationRecordID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CorrelationType.
           *
           * @return The value of the enumeration identified by CorrelationType.
           */
         virtual const uci::type::EOB_CorrelationTypeEnum& getCorrelationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CorrelationType.
           *
           * @return The value of the enumeration identified by CorrelationType.
           */
         virtual uci::type::EOB_CorrelationTypeEnum& getCorrelationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CorrelationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCorrelationType(const uci::type::EOB_CorrelationTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CorrelationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCorrelationType(uci::type::EOB_CorrelationTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CorrelationTimestamp.
           *
           * @return The value of the simple primitive data type identified by CorrelationTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getCorrelationTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CorrelationTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCorrelationTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CorrelatedTo.
           *
           * @return The acecssor that provides access to the complex content that is identified by CorrelatedTo.
           */
         virtual const uci::type::EOB_CorrelatedToType& getCorrelatedTo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CorrelatedTo.
           *
           * @return The acecssor that provides access to the complex content that is identified by CorrelatedTo.
           */
         virtual uci::type::EOB_CorrelatedToType& getCorrelatedTo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CorrelatedTo to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CorrelatedTo
           */
         virtual void setCorrelatedTo(const uci::type::EOB_CorrelatedToType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CorrelatedTo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CorrelatedTo is emabled or not
           */
         virtual bool hasCorrelatedTo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CorrelatedTo
           *
           * @param type = uci::type::accessorType::eOB_CorrelatedToType This Accessor's accessor type
           */
         virtual void enableCorrelatedTo(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_CorrelatedToType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CorrelatedTo */
         virtual void clearCorrelatedTo()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CorrelationInput.
           *
           * @return The acecssor that provides access to the complex content that is identified by CorrelationInput.
           */
         virtual const uci::type::EOB_CorrelationInputType& getCorrelationInput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CorrelationInput.
           *
           * @return The acecssor that provides access to the complex content that is identified by CorrelationInput.
           */
         virtual uci::type::EOB_CorrelationInputType& getCorrelationInput()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CorrelationInput to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CorrelationInput
           */
         virtual void setCorrelationInput(const uci::type::EOB_CorrelationInputType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CorrelationInput exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CorrelationInput is emabled or not
           */
         virtual bool hasCorrelationInput() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CorrelationInput
           *
           * @param type = uci::type::accessorType::eOB_CorrelationInputType This Accessor's accessor type
           */
         virtual void enableCorrelationInput(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_CorrelationInputType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CorrelationInput */
         virtual void clearCorrelationInput()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Signal.
           *
           * @return The acecssor that provides access to the complex content that is identified by Signal.
           */
         virtual const uci::type::EOB_SignalType& getSignal() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Signal.
           *
           * @return The acecssor that provides access to the complex content that is identified by Signal.
           */
         virtual uci::type::EOB_SignalType& getSignal()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Signal to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Signal
           */
         virtual void setSignal(const uci::type::EOB_SignalType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Signal exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Signal is emabled or not
           */
         virtual bool hasSignal() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Signal
           *
           * @param type = uci::type::accessorType::eOB_SignalType This Accessor's accessor type
           */
         virtual void enableSignal(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_SignalType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Signal */
         virtual void clearSignal()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::IdentityConfidenceType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::IdentityConfidenceType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::IdentityConfidenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::identityConfidenceType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::identityConfidenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReportedNotation2.
           *
           * @return The value of the string data type identified by ReportedNotation2.
           */
         virtual const uci::type::NotationType& getReportedNotation2() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReportedNotation2.
           *
           * @return The value of the string data type identified by ReportedNotation2.
           */
         virtual uci::type::NotationType& getReportedNotation2()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportedNotation2 to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReportedNotation2(const uci::type::NotationType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportedNotation2 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReportedNotation2(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportedNotation2 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReportedNotation2(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReportedNotation2 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReportedNotation2 is emabled or not
           */
         virtual bool hasReportedNotation2() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReportedNotation2
           *
           * @param type = uci::type::accessorType::notationType This Accessor's accessor type
           */
         virtual void enableReportedNotation2(uci::base::accessorType::AccessorType type = uci::type::accessorType::notationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReportedNotation2 */
         virtual void clearReportedNotation2()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReportedNotation1.
           *
           * @return The value of the string data type identified by ReportedNotation1.
           */
         virtual const uci::type::NotationType& getReportedNotation1() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReportedNotation1.
           *
           * @return The value of the string data type identified by ReportedNotation1.
           */
         virtual uci::type::NotationType& getReportedNotation1()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportedNotation1 to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReportedNotation1(const uci::type::NotationType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportedNotation1 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReportedNotation1(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportedNotation1 to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReportedNotation1(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReportedNotation1 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReportedNotation1 is emabled or not
           */
         virtual bool hasReportedNotation1() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReportedNotation1
           *
           * @param type = uci::type::accessorType::notationType This Accessor's accessor type
           */
         virtual void enableReportedNotation1(uci::base::accessorType::AccessorType type = uci::type::accessorType::notationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReportedNotation1 */
         virtual void clearReportedNotation1()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::FixedPositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::FixedPositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::FixedPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Position exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Position is emabled or not
           */
         virtual bool hasPosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Position
           *
           * @param type = uci::type::accessorType::fixedPositionType This Accessor's accessor type
           */
         virtual void enablePosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::fixedPositionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Position */
         virtual void clearPosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformKinematics.
           */
         virtual const uci::type::KinematicsFixedType& getPlatformKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformKinematics.
           */
         virtual uci::type::KinematicsFixedType& getPlatformKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlatformKinematics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlatformKinematics
           */
         virtual void setPlatformKinematics(const uci::type::KinematicsFixedType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlatformKinematics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlatformKinematics is emabled or not
           */
         virtual bool hasPlatformKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlatformKinematics
           *
           * @param type = uci::type::accessorType::kinematicsFixedType This Accessor's accessor type
           */
         virtual void enablePlatformKinematics(uci::base::accessorType::AccessorType type = uci::type::accessorType::kinematicsFixedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlatformKinematics */
         virtual void clearPlatformKinematics()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_CorrelationRecordType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_CorrelationRecordType to copy from
           */
         EOB_CorrelationRecordType(const EOB_CorrelationRecordType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_CorrelationRecordType to the contents of the
           * EOB_CorrelationRecordType on the right hand side (rhs) of the assignment operator.EOB_CorrelationRecordType [only
           * available to derived classes]
           *
           * @param rhs The EOB_CorrelationRecordType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EOB_CorrelationRecordType
           * @return A constant reference to this EOB_CorrelationRecordType.
           */
         const EOB_CorrelationRecordType& operator=(const EOB_CorrelationRecordType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_CorrelationRecordType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_CorrelationRecordType_h

