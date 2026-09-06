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
#ifndef Uci__Type__MissionPlanDetailsType_h
#define Uci__Type__MissionPlanDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ReplanReasonType_h)
# include "uci/type/ReplanReasonType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AllocatedTaskType_h)
# include "uci/type/AllocatedTaskType.h"
#endif

#if !defined(Uci__Type__RouteID_Type_h)
# include "uci/type/RouteID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanDetailsType sequence accessor class */
      class MissionPlanDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanDetailsType
           *
           * @return This accessor's type constant, i.e. MissionPlanDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a list of allocated/accepted Tasks whose meaning varies somewhat depending on the ApprovalState. Once the
           * allocation is approved this list indicates those Tasks which have been accepted by the AllocatedTo System and
           * approved by an ApprovalAuthority if required by the ApprovalPolicy. In the post-allocation ApprovalState. the list
           * generally isn't expected to change. Allocated tasks must be present unless no tasks were accepted by or commanded to
           * the System. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AllocatedTaskType, uci::type::accessorType::allocatedTaskType> AllocatedTask;

         /** Returns the accessor that provides access to the complex content that is identified by the AllocatedToID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocatedToID.
           */
         virtual const uci::type::SystemID_Type& getAllocatedToID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AllocatedToID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocatedToID.
           */
         virtual uci::type::SystemID_Type& getAllocatedToID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AllocatedToID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AllocatedToID
           */
         virtual void setAllocatedToID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReplanReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReplanReason.
           */
         virtual const uci::type::ReplanReasonType& getReplanReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReplanReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReplanReason.
           */
         virtual uci::type::ReplanReasonType& getReplanReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReplanReason to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReplanReason
           */
         virtual void setReplanReason(const uci::type::ReplanReasonType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReplanReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReplanReason is emabled or not
           */
         virtual bool hasReplanReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReplanReason
           *
           * @param type = uci::type::accessorType::replanReasonType This Accessor's accessor type
           */
         virtual void enableReplanReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::replanReasonType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReplanReason */
         virtual void clearReplanReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AllocatedTask.
           *
           * @return The bounded list identified by AllocatedTask.
           */
         virtual const uci::type::MissionPlanDetailsType::AllocatedTask& getAllocatedTask() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AllocatedTask.
           *
           * @return The bounded list identified by AllocatedTask.
           */
         virtual uci::type::MissionPlanDetailsType::AllocatedTask& getAllocatedTask()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AllocatedTask.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllocatedTask(const uci::type::MissionPlanDetailsType::AllocatedTask& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteID.
           */
         virtual const uci::type::RouteID_Type& getRouteID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteID.
           */
         virtual uci::type::RouteID_Type& getRouteID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteID
           */
         virtual void setRouteID(const uci::type::RouteID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RouteID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RouteID is emabled or not
           */
         virtual bool hasRouteID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RouteID
           *
           * @param type = uci::type::accessorType::routeID_Type This Accessor's accessor type
           */
         virtual void enableRouteID(uci::base::accessorType::AccessorType type = uci::type::accessorType::routeID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RouteID */
         virtual void clearRouteID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanDetailsType to copy from
           */
         MissionPlanDetailsType(const MissionPlanDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanDetailsType to the contents of the
           * MissionPlanDetailsType on the right hand side (rhs) of the assignment operator.MissionPlanDetailsType [only available
           * to derived classes]
           *
           * @param rhs The MissionPlanDetailsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MissionPlanDetailsType
           * @return A constant reference to this MissionPlanDetailsType.
           */
         const MissionPlanDetailsType& operator=(const MissionPlanDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanDetailsType_h

