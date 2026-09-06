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
#ifndef Uci__Type__OpLineMDT_h
#define Uci__Type__OpLineMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OpLineID_Type_h)
# include "uci/type/OpLineID_Type.h"
#endif

#if !defined(Uci__Type__OpLineCategoryEnum_h)
# include "uci/type/OpLineCategoryEnum.h"
#endif

#if !defined(Uci__Type__OpLineType_h)
# include "uci/type/OpLineType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ScheduleType_h)
# include "uci/type/ScheduleType.h"
#endif

#if !defined(Uci__Type__OpDescriptionType_h)
# include "uci/type/OpDescriptionType.h"
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

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpLineMDT sequence accessor class */
      class OpLineMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpLineMDT()
         { }

         /** Returns this accessor's type constant, i.e. OpLineMDT
           *
           * @return This accessor's type constant, i.e. OpLineMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opLineMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpLineMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** One or more systems to which this point applies. If omitted, line applies to all systems. (i.e., Link-16 J3.0 receipt
           * is globally assigned) [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Indicates time related events that are associated to the OpLine such as when activated or when operational. The
           * multiplicity is limited to N-1 options where NONE is an event that has no value, thus removed from list of available
           * options. [Minimum occurrences: 0] [Maximum occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::TimeFunctionType, uci::type::accessorType::timeFunctionType> AssociatedTime;

         /** List of datalink ID. Multiple datalink IDs can be reported for the same network type. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataLinkIdentifierPET, uci::type::accessorType::dataLinkIdentifierPET> DataLinkIdentifier;

         /** Returns the accessor that provides access to the complex content that is identified by the OpLineID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpLineID.
           */
         virtual const uci::type::OpLineID_Type& getOpLineID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpLineID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpLineID.
           */
         virtual uci::type::OpLineID_Type& getOpLineID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpLineID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpLineID
           */
         virtual void setOpLineID(const uci::type::OpLineID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Category.
           *
           * @return The value of the enumeration identified by Category.
           */
         virtual const uci::type::OpLineCategoryEnum& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Category.
           *
           * @return The value of the enumeration identified by Category.
           */
         virtual uci::type::OpLineCategoryEnum& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Category.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCategory(const uci::type::OpLineCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Category.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCategory(uci::type::OpLineCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Line.
           *
           * @return The acecssor that provides access to the complex content that is identified by Line.
           */
         virtual const uci::type::OpLineType& getLine() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Line.
           *
           * @return The acecssor that provides access to the complex content that is identified by Line.
           */
         virtual uci::type::OpLineType& getLine()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Line to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Line
           */
         virtual void setLine(const uci::type::OpLineType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::OpLineMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::OpLineMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::OpLineMDT::SystemID& value)
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


         /** Returns the bounded list that is identified by the AssociatedTime.
           *
           * @return The bounded list identified by AssociatedTime.
           */
         virtual const uci::type::OpLineMDT::AssociatedTime& getAssociatedTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedTime.
           *
           * @return The bounded list identified by AssociatedTime.
           */
         virtual uci::type::OpLineMDT::AssociatedTime& getAssociatedTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedTime.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedTime(const uci::type::OpLineMDT::AssociatedTime& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkIdentifier.
           *
           * @return The bounded list identified by DataLinkIdentifier.
           */
         virtual const uci::type::OpLineMDT::DataLinkIdentifier& getDataLinkIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkIdentifier.
           *
           * @return The bounded list identified by DataLinkIdentifier.
           */
         virtual uci::type::OpLineMDT::DataLinkIdentifier& getDataLinkIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataLinkIdentifier.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataLinkIdentifier(const uci::type::OpLineMDT::DataLinkIdentifier& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpLineMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpLineMDT to copy from
           */
         OpLineMDT(const OpLineMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpLineMDT to the contents of the OpLineMDT on the right hand side
           * (rhs) of the assignment operator.OpLineMDT [only available to derived classes]
           *
           * @param rhs The OpLineMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::OpLineMDT
           * @return A constant reference to this OpLineMDT.
           */
         const OpLineMDT& operator=(const OpLineMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpLineMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpLineMDT_h

