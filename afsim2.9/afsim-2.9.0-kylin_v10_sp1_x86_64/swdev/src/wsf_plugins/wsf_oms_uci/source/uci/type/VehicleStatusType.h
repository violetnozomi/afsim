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
#ifndef Uci__Type__VehicleStatusType_h
#define Uci__Type__VehicleStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SegmentCompletedType_h)
# include "uci/type/SegmentCompletedType.h"
#endif

#if !defined(Uci__Type__SegmentEstimateType_h)
# include "uci/type/SegmentEstimateType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the VehicleStatusType sequence accessor class */
      class VehicleStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleStatusType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleStatusType
           *
           * @return This accessor's type constant, i.e. VehicleStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of the most recently completed Task. Multiple IDs are allowed when Systems performed multiple
           * Tasks simultaneously. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> PreviousTaskID;

         /** Indicates the unique ID of a Task the System is currently in route to execute or is in the process of executing.
           * Multiple IDs are allowed when Systems perform multiple Tasks simultaneously. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> CurrentTaskID;

         /** Indicates the unique ID of the Task that the System will execute after completing the task that it is currently in
           * route to execute or is in the process of executing. Multiple IDs are allowed when Systems perform multiple Tasks
           * simultaneously. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> NextTaskID;

         /** Returns the accessor that provides access to the complex content that is identified by the PreviousSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by PreviousSegment.
           */
         virtual const uci::type::SegmentCompletedType& getPreviousSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PreviousSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by PreviousSegment.
           */
         virtual uci::type::SegmentCompletedType& getPreviousSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PreviousSegment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PreviousSegment
           */
         virtual void setPreviousSegment(const uci::type::SegmentCompletedType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PreviousSegment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PreviousSegment is emabled or not
           */
         virtual bool hasPreviousSegment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PreviousSegment
           *
           * @param type = uci::type::accessorType::segmentCompletedType This Accessor's accessor type
           */
         virtual void enablePreviousSegment(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentCompletedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PreviousSegment */
         virtual void clearPreviousSegment()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentSegment.
           */
         virtual const uci::type::SegmentEstimateType& getCurrentSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentSegment.
           */
         virtual uci::type::SegmentEstimateType& getCurrentSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentSegment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentSegment
           */
         virtual void setCurrentSegment(const uci::type::SegmentEstimateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextSegment.
           */
         virtual const uci::type::SegmentEstimateType& getNextSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextSegment.
           */
         virtual uci::type::SegmentEstimateType& getNextSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NextSegment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NextSegment
           */
         virtual void setNextSegment(const uci::type::SegmentEstimateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NextSegment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NextSegment is emabled or not
           */
         virtual bool hasNextSegment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NextSegment
           *
           * @param type = uci::type::accessorType::segmentEstimateType This Accessor's accessor type
           */
         virtual void enableNextSegment(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentEstimateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NextSegment */
         virtual void clearNextSegment()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PreviousTaskID.
           *
           * @return The bounded list identified by PreviousTaskID.
           */
         virtual const uci::type::VehicleStatusType::PreviousTaskID& getPreviousTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PreviousTaskID.
           *
           * @return The bounded list identified by PreviousTaskID.
           */
         virtual uci::type::VehicleStatusType::PreviousTaskID& getPreviousTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PreviousTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPreviousTaskID(const uci::type::VehicleStatusType::PreviousTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CurrentTaskID.
           *
           * @return The bounded list identified by CurrentTaskID.
           */
         virtual const uci::type::VehicleStatusType::CurrentTaskID& getCurrentTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CurrentTaskID.
           *
           * @return The bounded list identified by CurrentTaskID.
           */
         virtual uci::type::VehicleStatusType::CurrentTaskID& getCurrentTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CurrentTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCurrentTaskID(const uci::type::VehicleStatusType::CurrentTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NextTaskID.
           *
           * @return The bounded list identified by NextTaskID.
           */
         virtual const uci::type::VehicleStatusType::NextTaskID& getNextTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NextTaskID.
           *
           * @return The bounded list identified by NextTaskID.
           */
         virtual uci::type::VehicleStatusType::NextTaskID& getNextTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the NextTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setNextTaskID(const uci::type::VehicleStatusType::NextTaskID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleStatusType to copy from
           */
         VehicleStatusType(const VehicleStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleStatusType to the contents of the VehicleStatusType on the
           * right hand side (rhs) of the assignment operator.VehicleStatusType [only available to derived classes]
           *
           * @param rhs The VehicleStatusType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::VehicleStatusType
           * @return A constant reference to this VehicleStatusType.
           */
         const VehicleStatusType& operator=(const VehicleStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleStatusType_h

