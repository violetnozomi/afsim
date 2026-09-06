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
#ifndef Uci__Type__CommScheduleAllocationMDT_h
#define Uci__Type__CommScheduleAllocationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommScheduleAllocationID_Type_h)
# include "uci/type/CommScheduleAllocationID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequirementID_Type_h)
# include "uci/type/CommSchedulingRequirementID_Type.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequestID_Type_h)
# include "uci/type/CommSchedulingRequestID_Type.h"
#endif

#if !defined(Uci__Type__CommCoverageUserDataType_h)
# include "uci/type/CommCoverageUserDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommScheduleAllocationMDT sequence accessor class */
      class CommScheduleAllocationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommScheduleAllocationMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommScheduleAllocationMDT
           *
           * @return This accessor's type constant, i.e. CommScheduleAllocationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commScheduleAllocationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommScheduleAllocationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The list of CommSchedulingRequirements that the allocation is intended to fulfill. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequirementID_Type, uci::type::accessorType::commSchedulingRequirementID_Type> CommSchedulingRequirementID;

         /** Represents a list of communication allocation times that are being allocated from approved CommSchedulingRequests.
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommCoverageUserDataType, uci::type::accessorType::commCoverageUserDataType> AllocatedComm;

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


         /** Returns the accessor that provides access to the complex content that is identified by the ControllingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControllingSystemID.
           */
         virtual const uci::type::SystemID_Type& getControllingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControllingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControllingSystemID.
           */
         virtual uci::type::SystemID_Type& getControllingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControllingSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControllingSystemID
           */
         virtual void setControllingSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DesignatedSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DesignatedSystemID.
           */
         virtual const uci::type::SystemID_Type& getDesignatedSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DesignatedSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DesignatedSystemID.
           */
         virtual uci::type::SystemID_Type& getDesignatedSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DesignatedSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DesignatedSystemID
           */
         virtual void setDesignatedSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DesignatedSystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DesignatedSystemID is emabled or not
           */
         virtual bool hasDesignatedSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DesignatedSystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableDesignatedSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DesignatedSystemID */
         virtual void clearDesignatedSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @return The bounded list identified by CommSchedulingRequirementID.
           */
         virtual const uci::type::CommScheduleAllocationMDT::CommSchedulingRequirementID& getCommSchedulingRequirementID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @return The bounded list identified by CommSchedulingRequirementID.
           */
         virtual uci::type::CommScheduleAllocationMDT::CommSchedulingRequirementID& getCommSchedulingRequirementID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSchedulingRequirementID(const uci::type::CommScheduleAllocationMDT::CommSchedulingRequirementID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSchedulingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSchedulingRequestID.
           */
         virtual const uci::type::CommSchedulingRequestID_Type& getCommSchedulingRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSchedulingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSchedulingRequestID.
           */
         virtual uci::type::CommSchedulingRequestID_Type& getCommSchedulingRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSchedulingRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSchedulingRequestID
           */
         virtual void setCommSchedulingRequestID(const uci::type::CommSchedulingRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommSchedulingRequestID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommSchedulingRequestID is emabled or not
           */
         virtual bool hasCommSchedulingRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommSchedulingRequestID
           *
           * @param type = uci::type::accessorType::commSchedulingRequestID_Type This Accessor's accessor type
           */
         virtual void enableCommSchedulingRequestID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commSchedulingRequestID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommSchedulingRequestID */
         virtual void clearCommSchedulingRequestID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AllocatedComm.
           *
           * @return The bounded list identified by AllocatedComm.
           */
         virtual const uci::type::CommScheduleAllocationMDT::AllocatedComm& getAllocatedComm() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AllocatedComm.
           *
           * @return The bounded list identified by AllocatedComm.
           */
         virtual uci::type::CommScheduleAllocationMDT::AllocatedComm& getAllocatedComm()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AllocatedComm.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllocatedComm(const uci::type::CommScheduleAllocationMDT::AllocatedComm& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommScheduleAllocationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommScheduleAllocationMDT to copy from
           */
         CommScheduleAllocationMDT(const CommScheduleAllocationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommScheduleAllocationMDT to the contents of the
           * CommScheduleAllocationMDT on the right hand side (rhs) of the assignment operator.CommScheduleAllocationMDT [only
           * available to derived classes]
           *
           * @param rhs The CommScheduleAllocationMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommScheduleAllocationMDT
           * @return A constant reference to this CommScheduleAllocationMDT.
           */
         const CommScheduleAllocationMDT& operator=(const CommScheduleAllocationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommScheduleAllocationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommScheduleAllocationMDT_h

