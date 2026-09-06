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
#ifndef Uci__Type__ApprovalRequestItemReferenceType_h
#define Uci__Type__ApprovalRequestItemReferenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__ApprovalRequestTaskItemType_h)
# include "uci/type/ApprovalRequestTaskItemType.h"
#endif

#if !defined(Uci__Type__ApprovalRequestActivationItemType_h)
# include "uci/type/ApprovalRequestActivationItemType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ApprovalRequestItemReferenceType choice accessor class */
      class ApprovalRequestItemReferenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ApprovalRequestItemReferenceType()
         { }

         /** Returns this accessor's type constant, i.e. ApprovalRequestItemReferenceType
           *
           * @return This accessor's type constant, i.e. ApprovalRequestItemReferenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::approvalRequestItemReferenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ApprovalRequestItemReferenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum ApprovalRequestItemReferenceTypeChoice {
            APPROVALREQUESTITEMREFERENCETYPE_CHOICE_NONE,
            APPROVALREQUESTITEMREFERENCETYPE_CHOICE_ALLOCATIONMISSIONPLANID,
            APPROVALREQUESTITEMREFERENCETYPE_CHOICE_ROUTEMISSIONPLANID,
            APPROVALREQUESTITEMREFERENCETYPE_CHOICE_TASKAPPROVAL,
            APPROVALREQUESTITEMREFERENCETYPE_CHOICE_MISSIONPLANACTIVATION,
            APPROVALREQUESTITEMREFERENCETYPE_CHOICE_UPLOADMISSIONPLANID
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual ApprovalRequestItemReferenceTypeChoice getApprovalRequestItemReferenceTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getApprovalRequestItemReferenceTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setApprovalRequestItemReferenceTypeChoiceOrdinal(ApprovalRequestItemReferenceTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AllocationMissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationMissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getAllocationMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AllocationMissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationMissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getAllocationMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AllocationMissionPlanID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AllocationMissionPlanID
           */
         virtual void setAllocationMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteMissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteMissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getRouteMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteMissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteMissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getRouteMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteMissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteMissionPlanID
           */
         virtual void setRouteMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskApproval.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskApproval.
           */
         virtual const uci::type::ApprovalRequestTaskItemType& getTaskApproval() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskApproval.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskApproval.
           */
         virtual uci::type::ApprovalRequestTaskItemType& getTaskApproval()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskApproval to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskApproval
           */
         virtual void setTaskApproval(const uci::type::ApprovalRequestTaskItemType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanActivation.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanActivation.
           */
         virtual const uci::type::ApprovalRequestActivationItemType& getMissionPlanActivation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanActivation.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanActivation.
           */
         virtual uci::type::ApprovalRequestActivationItemType& getMissionPlanActivation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanActivation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanActivation
           */
         virtual void setMissionPlanActivation(const uci::type::ApprovalRequestActivationItemType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UploadMissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UploadMissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getUploadMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UploadMissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UploadMissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getUploadMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UploadMissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UploadMissionPlanID
           */
         virtual void setUploadMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ApprovalRequestItemReferenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ApprovalRequestItemReferenceType to copy from
           */
         ApprovalRequestItemReferenceType(const ApprovalRequestItemReferenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ApprovalRequestItemReferenceType to the contents of the
           * ApprovalRequestItemReferenceType on the right hand side (rhs) of the assignment
           * operator.ApprovalRequestItemReferenceType [only available to derived classes]
           *
           * @param rhs The ApprovalRequestItemReferenceType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ApprovalRequestItemReferenceType
           * @return A constant reference to this ApprovalRequestItemReferenceType.
           */
         const ApprovalRequestItemReferenceType& operator=(const ApprovalRequestItemReferenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ApprovalRequestItemReferenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ApprovalRequestItemReferenceType_h

