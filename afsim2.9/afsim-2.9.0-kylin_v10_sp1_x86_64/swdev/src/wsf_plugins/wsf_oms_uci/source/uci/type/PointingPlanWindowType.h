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
#ifndef Uci__Type__PointingPlanWindowType_h
#define Uci__Type__PointingPlanWindowType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommScheduleAllocationID_Type_h)
# include "uci/type/CommScheduleAllocationID_Type.h"
#endif

#if !defined(Uci__Type__CommSystemID_Type_h)
# include "uci/type/CommSystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__CommPointingType_h)
# include "uci/type/CommPointingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A Time bounded list of Pointing Vector commands that are to be used to support an external communication allocation. */
      class PointingPlanWindowType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PointingPlanWindowType()
         { }

         /** Returns this accessor's type constant, i.e. PointingPlanWindowType
           *
           * @return This accessor's type constant, i.e. PointingPlanWindowType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pointingPlanWindowType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PointingPlanWindowType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The List of Mission Plans supported by this Pointing Plan. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** Represents the list of time bounded Pointing Vectors that will need to be commanded to the communication system to
           * support a set of mission plans. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommPointingType, uci::type::accessorType::commPointingType> CommPointing;

         /** Returns the accessor that provides access to the complex content that is identified by the AllocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationID.
           */
         virtual const uci::type::CommScheduleAllocationID_Type& getAllocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AllocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationID.
           */
         virtual uci::type::CommScheduleAllocationID_Type& getAllocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AllocationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AllocationID
           */
         virtual void setAllocationID(const uci::type::CommScheduleAllocationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AllocationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AllocationID is emabled or not
           */
         virtual bool hasAllocationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AllocationID
           *
           * @param type = uci::type::accessorType::commScheduleAllocationID_Type This Accessor's accessor type
           */
         virtual void enableAllocationID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commScheduleAllocationID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AllocationID */
         virtual void clearAllocationID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemID.
           */
         virtual const uci::type::CommSystemID_Type& getCommSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemID.
           */
         virtual uci::type::CommSystemID_Type& getCommSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSystemID
           */
         virtual void setCommSystemID(const uci::type::CommSystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommSystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommSystemID is emabled or not
           */
         virtual bool hasCommSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommSystemID
           *
           * @param type = uci::type::accessorType::commSystemID_Type This Accessor's accessor type
           */
         virtual void enableCommSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commSystemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommSystemID */
         virtual void clearCommSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::PointingPlanWindowType::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::PointingPlanWindowType::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::PointingPlanWindowType::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommPointing.
           *
           * @return The bounded list identified by CommPointing.
           */
         virtual const uci::type::PointingPlanWindowType::CommPointing& getCommPointing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommPointing.
           *
           * @return The bounded list identified by CommPointing.
           */
         virtual uci::type::PointingPlanWindowType::CommPointing& getCommPointing()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommPointing.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommPointing(const uci::type::PointingPlanWindowType::CommPointing& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PointingPlanWindowType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PointingPlanWindowType to copy from
           */
         PointingPlanWindowType(const PointingPlanWindowType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PointingPlanWindowType to the contents of the
           * PointingPlanWindowType on the right hand side (rhs) of the assignment operator.PointingPlanWindowType [only available
           * to derived classes]
           *
           * @param rhs The PointingPlanWindowType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::PointingPlanWindowType
           * @return A constant reference to this PointingPlanWindowType.
           */
         const PointingPlanWindowType& operator=(const PointingPlanWindowType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PointingPlanWindowType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PointingPlanWindowType_h

