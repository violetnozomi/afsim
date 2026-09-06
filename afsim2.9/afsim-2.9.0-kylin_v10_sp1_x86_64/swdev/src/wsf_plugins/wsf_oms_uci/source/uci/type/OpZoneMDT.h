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
#ifndef Uci__Type__OpZoneMDT_h
#define Uci__Type__OpZoneMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

#if !defined(Uci__Type__OpZoneCategoryType_h)
# include "uci/type/OpZoneCategoryType.h"
#endif

#if !defined(Uci__Type__OpZoneType_h)
# include "uci/type/OpZoneType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__OpDescriptionType_h)
# include "uci/type/OpDescriptionType.h"
#endif

#if !defined(Uci__Type__MissionTraceabilityType_h)
# include "uci/type/MissionTraceabilityType.h"
#endif

#if !defined(Uci__Type__DataLinkIdentifierPET_h)
# include "uci/type/DataLinkIdentifierPET.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpZoneMDT sequence accessor class */
      class OpZoneMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpZoneMDT()
         { }

         /** Returns this accessor's type constant, i.e. OpZoneMDT
           *
           * @return This accessor's type constant, i.e. OpZoneMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opZoneMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpZoneMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** One or more systems to which this zone applies. If omitted, zone applies to all systems. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** List of datalink ID. Multiple datalink IDs can be reported for the same network type. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataLinkIdentifierPET, uci::type::accessorType::dataLinkIdentifierPET> DataLinkIdentifier;

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


         /** Returns the accessor that provides access to the complex content that is identified by the Category.
           *
           * @return The acecssor that provides access to the complex content that is identified by Category.
           */
         virtual const uci::type::OpZoneCategoryType& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Category.
           *
           * @return The acecssor that provides access to the complex content that is identified by Category.
           */
         virtual uci::type::OpZoneCategoryType& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Category to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Category
           */
         virtual void setCategory(const uci::type::OpZoneCategoryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Zone.
           *
           * @return The acecssor that provides access to the complex content that is identified by Zone.
           */
         virtual const uci::type::OpZoneType& getZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Zone.
           *
           * @return The acecssor that provides access to the complex content that is identified by Zone.
           */
         virtual uci::type::OpZoneType& getZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Zone to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Zone
           */
         virtual void setZone(const uci::type::OpZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SituationalAwareness.
           *
           * @return The value of the simple primitive data type identified by SituationalAwareness.
           */
         virtual xs::Boolean getSituationalAwareness() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SituationalAwareness.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSituationalAwareness(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::OpZoneMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::OpZoneMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::OpZoneMDT::SystemID& value)
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


         /** Returns the bounded list that is identified by the DataLinkIdentifier.
           *
           * @return The bounded list identified by DataLinkIdentifier.
           */
         virtual const uci::type::OpZoneMDT::DataLinkIdentifier& getDataLinkIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkIdentifier.
           *
           * @return The bounded list identified by DataLinkIdentifier.
           */
         virtual uci::type::OpZoneMDT::DataLinkIdentifier& getDataLinkIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataLinkIdentifier.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataLinkIdentifier(const uci::type::OpZoneMDT::DataLinkIdentifier& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpZoneMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpZoneMDT to copy from
           */
         OpZoneMDT(const OpZoneMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpZoneMDT to the contents of the OpZoneMDT on the right hand side
           * (rhs) of the assignment operator.OpZoneMDT [only available to derived classes]
           *
           * @param rhs The OpZoneMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::OpZoneMDT
           * @return A constant reference to this OpZoneMDT.
           */
         const OpZoneMDT& operator=(const OpZoneMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpZoneMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpZoneMDT_h

