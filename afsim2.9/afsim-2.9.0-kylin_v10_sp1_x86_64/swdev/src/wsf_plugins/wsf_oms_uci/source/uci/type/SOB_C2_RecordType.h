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
#ifndef Uci__Type__SOB_C2_RecordType_h
#define Uci__Type__SOB_C2_RecordType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__SOB_C2_RecordID_Type_h)
# include "uci/type/SOB_C2_RecordID_Type.h"
#endif

#if !defined(Uci__Type__EOB_RecordID_Type_h)
# include "uci/type/EOB_RecordID_Type.h"
#endif

#if !defined(Uci__Type__SatelliteC2_IdentityType_h)
# include "uci/type/SatelliteC2_IdentityType.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__OrderOfBattleTimestampsType_h)
# include "uci/type/OrderOfBattleTimestampsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SOB_SatelliteRecordID_Type_h)
# include "uci/type/SOB_SatelliteRecordID_Type.h"
#endif

#if !defined(Uci__Type__EOB_OperationalStatusEnum_h)
# include "uci/type/EOB_OperationalStatusEnum.h"
#endif

#if !defined(Uci__Type__EOB_EmitterType_h)
# include "uci/type/EOB_EmitterType.h"
#endif

#if !defined(Uci__Type__VisibleString512Type_h)
# include "uci/type/VisibleString512Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Contains information about the C2 node associated with a satellite record. */
      class SOB_C2_RecordType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SOB_C2_RecordType()
         { }

         /** Returns this accessor's type constant, i.e. SOB_C2_RecordType
           *
           * @return This accessor's type constant, i.e. SOB_C2_RecordType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sOB_C2_RecordType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SOB_C2_RecordType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of the satellite that is being controlled by this C2 node. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SOB_SatelliteRecordID_Type, uci::type::accessorType::sOB_SatelliteRecordID_Type> ControlledSatelliteID;

         /** Indicates the unique ID of the lower ranking C2 node. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SOB_C2_RecordID_Type, uci::type::accessorType::sOB_C2_RecordID_Type> SubordinateC2ID;

         /** Indicates mode and parametrics of a previously observed emitter associated with the SOB record. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EOB_EmitterType, uci::type::accessorType::eOB_EmitterType> Emitter;

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


         /** Returns the accessor that provides access to the complex content that is identified by the C2_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by C2_RecordID.
           */
         virtual const uci::type::SOB_C2_RecordID_Type& getC2_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the C2_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by C2_RecordID.
           */
         virtual uci::type::SOB_C2_RecordID_Type& getC2_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the C2_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by C2_RecordID
           */
         virtual void setC2_RecordID(const uci::type::SOB_C2_RecordID_Type& value)
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


         /** Returns whether the Element that is identified by EOB_RecordID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EOB_RecordID is emabled or not
           */
         virtual bool hasEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EOB_RecordID
           *
           * @param type = uci::type::accessorType::eOB_RecordID_Type This Accessor's accessor type
           */
         virtual void enableEOB_RecordID(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_RecordID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EOB_RecordID */
         virtual void clearEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::SatelliteC2_IdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::SatelliteC2_IdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::SatelliteC2_IdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::satelliteC2_IdentityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::satelliteC2_IdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual const uci::type::Point2D_Type& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual uci::type::Point2D_Type& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Location to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Location
           */
         virtual void setLocation(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Location exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Location is emabled or not
           */
         virtual bool hasLocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Location
           *
           * @param type = uci::type::accessorType::point2D_Type This Accessor's accessor type
           */
         virtual void enableLocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::point2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Location */
         virtual void clearLocation()
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


         /** Returns the bounded list that is identified by the ControlledSatelliteID.
           *
           * @return The bounded list identified by ControlledSatelliteID.
           */
         virtual const uci::type::SOB_C2_RecordType::ControlledSatelliteID& getControlledSatelliteID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ControlledSatelliteID.
           *
           * @return The bounded list identified by ControlledSatelliteID.
           */
         virtual uci::type::SOB_C2_RecordType::ControlledSatelliteID& getControlledSatelliteID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ControlledSatelliteID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setControlledSatelliteID(const uci::type::SOB_C2_RecordType::ControlledSatelliteID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubordinateC2ID.
           *
           * @return The bounded list identified by SubordinateC2ID.
           */
         virtual const uci::type::SOB_C2_RecordType::SubordinateC2ID& getSubordinateC2ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubordinateC2ID.
           *
           * @return The bounded list identified by SubordinateC2ID.
           */
         virtual uci::type::SOB_C2_RecordType::SubordinateC2ID& getSubordinateC2ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubordinateC2ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubordinateC2ID(const uci::type::SOB_C2_RecordType::SubordinateC2ID& value)
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


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::SOB_C2_RecordType::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::SOB_C2_RecordType::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::SOB_C2_RecordType::Emitter& value)
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
         SOB_C2_RecordType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SOB_C2_RecordType to copy from
           */
         SOB_C2_RecordType(const SOB_C2_RecordType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SOB_C2_RecordType to the contents of the SOB_C2_RecordType on the
           * right hand side (rhs) of the assignment operator.SOB_C2_RecordType [only available to derived classes]
           *
           * @param rhs The SOB_C2_RecordType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SOB_C2_RecordType
           * @return A constant reference to this SOB_C2_RecordType.
           */
         const SOB_C2_RecordType& operator=(const SOB_C2_RecordType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SOB_C2_RecordType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SOB_C2_RecordType_h

