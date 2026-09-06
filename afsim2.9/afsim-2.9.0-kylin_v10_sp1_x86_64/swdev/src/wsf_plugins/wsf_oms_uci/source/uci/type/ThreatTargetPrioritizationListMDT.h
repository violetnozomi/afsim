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
#ifndef Uci__Type__ThreatTargetPrioritizationListMDT_h
#define Uci__Type__ThreatTargetPrioritizationListMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationListID_Type_h)
# include "uci/type/ThreatTargetPrioritizationListID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationType_h)
# include "uci/type/ThreatTargetPrioritizationType.h"
#endif

#if !defined(Uci__Type__OpDescriptionType_h)
# include "uci/type/OpDescriptionType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__ScheduleType_h)
# include "uci/type/ScheduleType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatTargetPrioritizationListMDT sequence accessor class */
      class ThreatTargetPrioritizationListMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatTargetPrioritizationListMDT()
         { }

         /** Returns this accessor's type constant, i.e. ThreatTargetPrioritizationListMDT
           *
           * @return This accessor's type constant, i.e. ThreatTargetPrioritizationListMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatTargetPrioritizationListMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatTargetPrioritizationListMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an item in the TTPL; a threat/target and associated priority. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ThreatTargetPrioritizationType, uci::type::accessorType::threatTargetPrioritizationType> Item;

         /** One or more Systems to which the prioritization applies. If omitted, the prioritization applies to all Systems.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApplicableSystemID;

         /** One or more Services to which the mission response list applies. If omitted, the settings apply to all Services of
           * the Systems given in the sibling ApplicableSystemID. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ApplicableServiceID;

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


         /** Returns the bounded list that is identified by the Item.
           *
           * @return The bounded list identified by Item.
           */
         virtual const uci::type::ThreatTargetPrioritizationListMDT::Item& getItem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Item.
           *
           * @return The bounded list identified by Item.
           */
         virtual uci::type::ThreatTargetPrioritizationListMDT::Item& getItem()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Item.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setItem(const uci::type::ThreatTargetPrioritizationListMDT::Item& value)
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


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual const uci::type::ThreatTargetPrioritizationListMDT::ApplicableSystemID& getApplicableSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual uci::type::ThreatTargetPrioritizationListMDT::ApplicableSystemID& getApplicableSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableSystemID(const uci::type::ThreatTargetPrioritizationListMDT::ApplicableSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableServiceID.
           *
           * @return The bounded list identified by ApplicableServiceID.
           */
         virtual const uci::type::ThreatTargetPrioritizationListMDT::ApplicableServiceID& getApplicableServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableServiceID.
           *
           * @return The bounded list identified by ApplicableServiceID.
           */
         virtual uci::type::ThreatTargetPrioritizationListMDT::ApplicableServiceID& getApplicableServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableServiceID(const uci::type::ThreatTargetPrioritizationListMDT::ApplicableServiceID& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the Schedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by Schedule.
           */
         virtual const uci::type::ScheduleType& getSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Schedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by Schedule.
           */
         virtual uci::type::ScheduleType& getSchedule()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Schedule to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Schedule
           */
         virtual void setSchedule(const uci::type::ScheduleType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Schedule exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Schedule is emabled or not
           */
         virtual bool hasSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Schedule
           *
           * @param type = uci::type::accessorType::scheduleType This Accessor's accessor type
           */
         virtual void enableSchedule(uci::base::accessorType::AccessorType type = uci::type::accessorType::scheduleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Schedule */
         virtual void clearSchedule()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatTargetPrioritizationListMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationListMDT to copy from
           */
         ThreatTargetPrioritizationListMDT(const ThreatTargetPrioritizationListMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatTargetPrioritizationListMDT to the contents of the
           * ThreatTargetPrioritizationListMDT on the right hand side (rhs) of the assignment
           * operator.ThreatTargetPrioritizationListMDT [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationListMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ThreatTargetPrioritizationListMDT
           * @return A constant reference to this ThreatTargetPrioritizationListMDT.
           */
         const ThreatTargetPrioritizationListMDT& operator=(const ThreatTargetPrioritizationListMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatTargetPrioritizationListMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatTargetPrioritizationListMDT_h

