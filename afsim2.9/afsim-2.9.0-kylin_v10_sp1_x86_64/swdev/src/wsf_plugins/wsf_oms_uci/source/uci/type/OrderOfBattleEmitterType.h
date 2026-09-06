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
#ifndef Uci__Type__OrderOfBattleEmitterType_h
#define Uci__Type__OrderOfBattleEmitterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EOB_EmitterID_Type_h)
# include "uci/type/EOB_EmitterID_Type.h"
#endif

#if !defined(Uci__Type__EOB_EmitterIdentityType_h)
# include "uci/type/EOB_EmitterIdentityType.h"
#endif

#if !defined(Uci__Type__OrderOfBattleTimestampsType_h)
# include "uci/type/OrderOfBattleTimestampsType.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Type__VisibleString512Type_h)
# include "uci/type/VisibleString512Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EOB_EmitterModeType_h)
# include "uci/type/EOB_EmitterModeType.h"
#endif

#if !defined(Uci__Type__OperationalScheduleType_h)
# include "uci/type/OperationalScheduleType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** An emitter’s data regarding collection information, modes/parametrics, operational schedule and other related
        * information. Modes/Parametrics are provided, in part, by the Global Command and Control System Integrated Imagery and
        * Intelligence (GCCS-I3). More detail found in the Application Program Interface Reference Manual (APIRM) for MIDB Data
        * Access Layer (MDAL). Additional mode/parametric information is developed through EOB correlation.
        */
      class OrderOfBattleEmitterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrderOfBattleEmitterType()
         { }

         /** Returns this accessor's type constant, i.e. OrderOfBattleEmitterType
           *
           * @return This accessor's type constant, i.e. OrderOfBattleEmitterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orderOfBattleEmitterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrderOfBattleEmitterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Maximum length: 3] [Pattern: "[A-Z0-9]{2}[A-Z0-9]?"] */
         typedef xs::String LastCollector;

         /** [Minimum length: 0] [Maximum length: 3] [Pattern: "[A-Za-z0-9 ]{0,3}"] */
         typedef xs::String RecordOwner;

         /** Indicates the Modes/Parametrics that this emitter have been observed operating. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EOB_EmitterModeType, uci::type::accessorType::eOB_EmitterModeType> Mode;

         /** Returns the accessor that provides access to the complex content that is identified by the EmitterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterID.
           */
         virtual const uci::type::EOB_EmitterID_Type& getEmitterID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterID.
           */
         virtual uci::type::EOB_EmitterID_Type& getEmitterID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterID
           */
         virtual void setEmitterID(const uci::type::EOB_EmitterID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::EOB_EmitterIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::EOB_EmitterIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::EOB_EmitterIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::eOB_EmitterIdentityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_EmitterIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
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


         /** Returns the value of the string data type that is identified by the LastCollector.
           *
           * @return The value of the string data type identified by LastCollector.
           */
         virtual const uci::type::OrderOfBattleEmitterType::LastCollector& getLastCollector() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the LastCollector.
           *
           * @return The value of the string data type identified by LastCollector.
           */
         virtual uci::type::OrderOfBattleEmitterType::LastCollector& getLastCollector()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LastCollector to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setLastCollector(const uci::type::OrderOfBattleEmitterType::LastCollector& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LastCollector to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLastCollector(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LastCollector to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLastCollector(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastCollector exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastCollector is emabled or not
           */
         virtual bool hasLastCollector() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastCollector
           *
           * @param type = uci::type::orderOfBattleEmitterType::accessorType::lastCollector This Accessor's accessor type
           */
         virtual void enableLastCollector(uci::base::accessorType::AccessorType type = uci::type::orderOfBattleEmitterType::accessorType::lastCollector)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastCollector */
         virtual void clearLastCollector()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RecordOwner.
           *
           * @return The value of the string data type identified by RecordOwner.
           */
         virtual const uci::type::OrderOfBattleEmitterType::RecordOwner& getRecordOwner() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RecordOwner.
           *
           * @return The value of the string data type identified by RecordOwner.
           */
         virtual uci::type::OrderOfBattleEmitterType::RecordOwner& getRecordOwner()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RecordOwner to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRecordOwner(const uci::type::OrderOfBattleEmitterType::RecordOwner& value)
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
           * @param type = uci::type::orderOfBattleEmitterType::accessorType::recordOwner This Accessor's accessor type
           */
         virtual void enableRecordOwner(uci::base::accessorType::AccessorType type = uci::type::orderOfBattleEmitterType::accessorType::recordOwner)
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


         /** Returns the bounded list that is identified by the Mode.
           *
           * @return The bounded list identified by Mode.
           */
         virtual const uci::type::OrderOfBattleEmitterType::Mode& getMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Mode.
           *
           * @return The bounded list identified by Mode.
           */
         virtual uci::type::OrderOfBattleEmitterType::Mode& getMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Mode.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMode(const uci::type::OrderOfBattleEmitterType::Mode& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperationalSchedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperationalSchedule.
           */
         virtual const uci::type::OperationalScheduleType& getOperationalSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperationalSchedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperationalSchedule.
           */
         virtual uci::type::OperationalScheduleType& getOperationalSchedule()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperationalSchedule to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperationalSchedule
           */
         virtual void setOperationalSchedule(const uci::type::OperationalScheduleType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OperationalSchedule exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OperationalSchedule is emabled or not
           */
         virtual bool hasOperationalSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OperationalSchedule
           *
           * @param type = uci::type::accessorType::operationalScheduleType This Accessor's accessor type
           */
         virtual void enableOperationalSchedule(uci::base::accessorType::AccessorType type = uci::type::accessorType::operationalScheduleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OperationalSchedule */
         virtual void clearOperationalSchedule()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrderOfBattleEmitterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrderOfBattleEmitterType to copy from
           */
         OrderOfBattleEmitterType(const OrderOfBattleEmitterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrderOfBattleEmitterType to the contents of the
           * OrderOfBattleEmitterType on the right hand side (rhs) of the assignment operator.OrderOfBattleEmitterType [only
           * available to derived classes]
           *
           * @param rhs The OrderOfBattleEmitterType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OrderOfBattleEmitterType
           * @return A constant reference to this OrderOfBattleEmitterType.
           */
         const OrderOfBattleEmitterType& operator=(const OrderOfBattleEmitterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrderOfBattleEmitterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrderOfBattleEmitterType_h

