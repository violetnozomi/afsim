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
#ifndef Uci__Type__TraceabilityType_h
#define Uci__Type__TraceabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityCommandID_Type_h)
# include "uci/type/CapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__CollectionDeckTraceabilityType_h)
# include "uci/type/CollectionDeckTraceabilityType.h"
#endif

#if !defined(Uci__Type__ACTDF_TraceabilityType_h)
# include "uci/type/ACTDF_TraceabilityType.h"
#endif

#if !defined(Uci__Type__EEI_ID_Type_h)
# include "uci/type/EEI_ID_Type.h"
#endif

#if !defined(Uci__Type__ATO_TraceabilityType_h)
# include "uci/type/ATO_TraceabilityType.h"
#endif

#if !defined(Uci__Type__AOCO_TraceabilityType_h)
# include "uci/type/AOCO_TraceabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Tasking sources work at different levels of detail and abstraction, progressing from what to do to how specifically
        * to do it. Inputs from abstract tasking sources and Tasks can be decomposed into discrete, actionable Tasks. This type
        * provides traceability to the external tasking or Task from which this Task was derived.
        */
      class TraceabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TraceabilityType()
         { }

         /** Returns this accessor's type constant, i.e. TraceabilityType
           *
           * @return This accessor's type constant, i.e. TraceabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::traceabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TraceabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the TaskID of a Task that this Task originated from. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Indicates a reference to an external (non-UCI) tasking source: the collection deck. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CollectionDeckTraceabilityType, uci::type::accessorType::collectionDeckTraceabilityType> CollectionDeckTraceability;

         /** Indicates a reference to an external (non-UCI) tasking source: Airborne Collection Tasking Message. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ACTDF_TraceabilityType, uci::type::accessorType::aCTDF_TraceabilityType> ACTDF_Traceability;

         /** Indicates a reference to an external (non-UCI) tasking source: Essential Element of Information. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EEI_ID_Type, uci::type::accessorType::eEI_ID_Type> EEI_ID;

         /** Indicates a reference to an external (non-UCI) tasking source: the Air Tasking Order. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ATO_TraceabilityType, uci::type::accessorType::aTO_TraceabilityType> ATO_Traceability;

         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual const uci::type::CapabilityCommandID_Type& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual uci::type::CapabilityCommandID_Type& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandID
           */
         virtual void setCommandID(const uci::type::CapabilityCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommandID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommandID is emabled or not
           */
         virtual bool hasCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommandID
           *
           * @param type = uci::type::accessorType::capabilityCommandID_Type This Accessor's accessor type
           */
         virtual void enableCommandID(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommandID */
         virtual void clearCommandID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::TraceabilityType::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::TraceabilityType::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::TraceabilityType::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectionDeckTraceability.
           *
           * @return The bounded list identified by CollectionDeckTraceability.
           */
         virtual const uci::type::TraceabilityType::CollectionDeckTraceability& getCollectionDeckTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectionDeckTraceability.
           *
           * @return The bounded list identified by CollectionDeckTraceability.
           */
         virtual uci::type::TraceabilityType::CollectionDeckTraceability& getCollectionDeckTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CollectionDeckTraceability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCollectionDeckTraceability(const uci::type::TraceabilityType::CollectionDeckTraceability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ACTDF_Traceability.
           *
           * @return The bounded list identified by ACTDF_Traceability.
           */
         virtual const uci::type::TraceabilityType::ACTDF_Traceability& getACTDF_Traceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ACTDF_Traceability.
           *
           * @return The bounded list identified by ACTDF_Traceability.
           */
         virtual uci::type::TraceabilityType::ACTDF_Traceability& getACTDF_Traceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ACTDF_Traceability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setACTDF_Traceability(const uci::type::TraceabilityType::ACTDF_Traceability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EEI_ID.
           *
           * @return The bounded list identified by EEI_ID.
           */
         virtual const uci::type::TraceabilityType::EEI_ID& getEEI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EEI_ID.
           *
           * @return The bounded list identified by EEI_ID.
           */
         virtual uci::type::TraceabilityType::EEI_ID& getEEI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EEI_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEEI_ID(const uci::type::TraceabilityType::EEI_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ATO_Traceability.
           *
           * @return The bounded list identified by ATO_Traceability.
           */
         virtual const uci::type::TraceabilityType::ATO_Traceability& getATO_Traceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ATO_Traceability.
           *
           * @return The bounded list identified by ATO_Traceability.
           */
         virtual uci::type::TraceabilityType::ATO_Traceability& getATO_Traceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ATO_Traceability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setATO_Traceability(const uci::type::TraceabilityType::ATO_Traceability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AOCO_Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by AOCO_Traceability.
           */
         virtual const uci::type::AOCO_TraceabilityType& getAOCO_Traceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AOCO_Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by AOCO_Traceability.
           */
         virtual uci::type::AOCO_TraceabilityType& getAOCO_Traceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AOCO_Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AOCO_Traceability
           */
         virtual void setAOCO_Traceability(const uci::type::AOCO_TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AOCO_Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AOCO_Traceability is emabled or not
           */
         virtual bool hasAOCO_Traceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AOCO_Traceability
           *
           * @param type = uci::type::accessorType::aOCO_TraceabilityType This Accessor's accessor type
           */
         virtual void enableAOCO_Traceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::aOCO_TraceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AOCO_Traceability */
         virtual void clearAOCO_Traceability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TraceabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TraceabilityType to copy from
           */
         TraceabilityType(const TraceabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TraceabilityType to the contents of the TraceabilityType on the
           * right hand side (rhs) of the assignment operator.TraceabilityType [only available to derived classes]
           *
           * @param rhs The TraceabilityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::TraceabilityType
           * @return A constant reference to this TraceabilityType.
           */
         const TraceabilityType& operator=(const TraceabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TraceabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TraceabilityType_h

