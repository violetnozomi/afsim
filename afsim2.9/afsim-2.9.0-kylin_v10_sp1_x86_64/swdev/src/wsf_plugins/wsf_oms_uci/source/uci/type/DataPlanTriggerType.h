// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DataPlanTriggerType_h
#define Uci__Type__DataPlanTriggerType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__CommTerminalPlanID_Type_h)
# include "uci/type/CommTerminalPlanID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Includes the Triggers to automatically apply a data plan. */
      class DataPlanTriggerType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataPlanTriggerType()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanTriggerType
           *
           * @return This accessor's type constant, i.e. DataPlanTriggerType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanTriggerType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanTriggerType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a unique ID for a mission. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionID_Type, uci::type::accessorType::missionID_Type> MissionID;

         /** Indicates a unique ID assigned to the MissionPlan. Updates to the mission plan for a given system and time period,
           * which go beyond the guidelines for using MissionPlanRevisionID, would generally result in a new unique ID indicating
           * that the new MissionPlan superseded the previous MissionPlan. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** Indicates a unique ID assigned to a CommTerminalPlan. When set indicates this data plan is triggered by the
           * activation of this communication plan. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommTerminalPlanID_Type, uci::type::accessorType::commTerminalPlanID_Type> CommPlanID;

         /** Indicates time windows for which this data plan should be active. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> EffectiveTimeSpans;

         /** This element represents a unique identifier of a zone for which this Data Plan should be active. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::OpZoneID_Type, uci::type::accessorType::opZoneID_Type> OpZoneID;

         /** Returns the bounded list that is identified by the MissionID.
           *
           * @return The bounded list identified by MissionID.
           */
         virtual const uci::type::DataPlanTriggerType::MissionID& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionID.
           *
           * @return The bounded list identified by MissionID.
           */
         virtual uci::type::DataPlanTriggerType::MissionID& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionID(const uci::type::DataPlanTriggerType::MissionID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::DataPlanTriggerType::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::DataPlanTriggerType::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::DataPlanTriggerType::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommPlanID.
           *
           * @return The bounded list identified by CommPlanID.
           */
         virtual const uci::type::DataPlanTriggerType::CommPlanID& getCommPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommPlanID.
           *
           * @return The bounded list identified by CommPlanID.
           */
         virtual uci::type::DataPlanTriggerType::CommPlanID& getCommPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommPlanID(const uci::type::DataPlanTriggerType::CommPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectiveTimeSpans.
           *
           * @return The bounded list identified by EffectiveTimeSpans.
           */
         virtual const uci::type::DataPlanTriggerType::EffectiveTimeSpans& getEffectiveTimeSpans() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectiveTimeSpans.
           *
           * @return The bounded list identified by EffectiveTimeSpans.
           */
         virtual uci::type::DataPlanTriggerType::EffectiveTimeSpans& getEffectiveTimeSpans()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EffectiveTimeSpans.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEffectiveTimeSpans(const uci::type::DataPlanTriggerType::EffectiveTimeSpans& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OpZoneID.
           *
           * @return The bounded list identified by OpZoneID.
           */
         virtual const uci::type::DataPlanTriggerType::OpZoneID& getOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OpZoneID.
           *
           * @return The bounded list identified by OpZoneID.
           */
         virtual uci::type::DataPlanTriggerType::OpZoneID& getOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OpZoneID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOpZoneID(const uci::type::DataPlanTriggerType::OpZoneID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanTriggerType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanTriggerType to copy from
           */
         DataPlanTriggerType(const DataPlanTriggerType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanTriggerType to the contents of the DataPlanTriggerType on
           * the right hand side (rhs) of the assignment operator.DataPlanTriggerType [only available to derived classes]
           *
           * @param rhs The DataPlanTriggerType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DataPlanTriggerType
           * @return A constant reference to this DataPlanTriggerType.
           */
         const DataPlanTriggerType& operator=(const DataPlanTriggerType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanTriggerType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanTriggerType_h

