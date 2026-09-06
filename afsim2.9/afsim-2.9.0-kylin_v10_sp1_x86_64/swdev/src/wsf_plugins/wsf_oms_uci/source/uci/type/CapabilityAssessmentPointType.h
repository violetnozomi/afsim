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
#ifndef Uci__Type__CapabilityAssessmentPointType_h
#define Uci__Type__CapabilityAssessmentPointType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__InertialStateType_h)
# include "uci/type/InertialStateType.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__PathID_Type_h)
# include "uci/type/PathID_Type.h"
#endif

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityAssessmentActionType_h)
# include "uci/type/CapabilityAssessmentActionType.h"
#endif

#if !defined(Uci__Type__CapabilityAssessmentActivityDetailsType_h)
# include "uci/type/CapabilityAssessmentActivityDetailsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A point along a Mission Plan at which a capability is expected to be used by a system. */
      class CapabilityAssessmentPointType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityAssessmentPointType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityAssessmentPointType
           *
           * @return This accessor's type constant, i.e. CapabilityAssessmentPointType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityAssessmentPointType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityAssessmentPointType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a planned Task or estimated [Capability]Command assessed to occur at the sibling Position. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityAssessmentActionType, uci::type::accessorType::capabilityAssessmentActionType> Action;

         /** Contains a list of estimated activities. Each activity contains a flag to indicate whether this activity has been
           * approved vs a not yet approved activity being included in the assessment. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityAssessmentActivityDetailsType, uci::type::accessorType::capabilityAssessmentActivityDetailsType> ActivityDetails;

         /** Returns the accessor that provides access to the complex content that is identified by the InertialState.
           *
           * @return The acecssor that provides access to the complex content that is identified by InertialState.
           */
         virtual const uci::type::InertialStateType& getInertialState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InertialState.
           *
           * @return The acecssor that provides access to the complex content that is identified by InertialState.
           */
         virtual uci::type::InertialStateType& getInertialState()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InertialState to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InertialState
           */
         virtual void setInertialState(const uci::type::InertialStateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlanID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanID is emabled or not
           */
         virtual bool hasMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanID
           *
           * @param type = uci::type::accessorType::missionPlanID_Type This Accessor's accessor type
           */
         virtual void enableMissionPlanID(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanID */
         virtual void clearMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathID.
           */
         virtual const uci::type::PathID_Type& getPathID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathID.
           */
         virtual uci::type::PathID_Type& getPathID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PathID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PathID
           */
         virtual void setPathID(const uci::type::PathID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PathID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PathID is emabled or not
           */
         virtual bool hasPathID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PathID
           *
           * @param type = uci::type::accessorType::pathID_Type This Accessor's accessor type
           */
         virtual void enablePathID(uci::base::accessorType::AccessorType type = uci::type::accessorType::pathID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PathID */
         virtual void clearPathID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SegmentID.
           */
         virtual const uci::type::SegmentID_Type& getSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SegmentID.
           */
         virtual uci::type::SegmentID_Type& getSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SegmentID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SegmentID
           */
         virtual void setSegmentID(const uci::type::SegmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SegmentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SegmentID is emabled or not
           */
         virtual bool hasSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SegmentID
           *
           * @param type = uci::type::accessorType::segmentID_Type This Accessor's accessor type
           */
         virtual void enableSegmentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SegmentID */
         virtual void clearSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Action.
           *
           * @return The bounded list identified by Action.
           */
         virtual const uci::type::CapabilityAssessmentPointType::Action& getAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Action.
           *
           * @return The bounded list identified by Action.
           */
         virtual uci::type::CapabilityAssessmentPointType::Action& getAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Action.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAction(const uci::type::CapabilityAssessmentPointType::Action& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityDetails.
           *
           * @return The bounded list identified by ActivityDetails.
           */
         virtual const uci::type::CapabilityAssessmentPointType::ActivityDetails& getActivityDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityDetails.
           *
           * @return The bounded list identified by ActivityDetails.
           */
         virtual uci::type::CapabilityAssessmentPointType::ActivityDetails& getActivityDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityDetails.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityDetails(const uci::type::CapabilityAssessmentPointType::ActivityDetails& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityAssessmentPointType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityAssessmentPointType to copy from
           */
         CapabilityAssessmentPointType(const CapabilityAssessmentPointType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityAssessmentPointType to the contents of the
           * CapabilityAssessmentPointType on the right hand side (rhs) of the assignment operator.CapabilityAssessmentPointType
           * [only available to derived classes]
           *
           * @param rhs The CapabilityAssessmentPointType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CapabilityAssessmentPointType
           * @return A constant reference to this CapabilityAssessmentPointType.
           */
         const CapabilityAssessmentPointType& operator=(const CapabilityAssessmentPointType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityAssessmentPointType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityAssessmentPointType_h

