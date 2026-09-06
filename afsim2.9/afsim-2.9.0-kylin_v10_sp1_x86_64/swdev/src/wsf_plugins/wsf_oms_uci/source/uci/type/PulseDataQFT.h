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
#ifndef Uci__Type__PulseDataQFT_h
#define Uci__Type__PulseDataQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PulseDataID_Type_h)
# include "uci/type/PulseDataID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityCommandID_Type_h)
# include "uci/type/CapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PulseDataQFT sequence accessor class */
      class PulseDataQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~PulseDataQFT()
         { }

         /** Returns this accessor's type constant, i.e. PulseDataQFT
           *
           * @return This accessor's type constant, i.e. PulseDataQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pulseDataQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PulseDataQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::PulseDataID_Type, uci::type::accessorType::pulseDataID_Type> PulseDataID;

         /** Indicates the ID of the Capability used to collect the PulseData. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates the ID of the Capability command which initiated the collection of the PulseData. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityCommandID_Type, uci::type::accessorType::capabilityCommandID_Type> CapabilityCommandID;

         /** Indicates the originating Task. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Returns the bounded list that is identified by the PulseDataID.
           *
           * @return The bounded list identified by PulseDataID.
           */
         virtual const uci::type::PulseDataQFT::PulseDataID& getPulseDataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseDataID.
           *
           * @return The bounded list identified by PulseDataID.
           */
         virtual uci::type::PulseDataQFT::PulseDataID& getPulseDataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PulseDataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPulseDataID(const uci::type::PulseDataQFT::PulseDataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::PulseDataQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::PulseDataQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::PulseDataQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommandID.
           *
           * @return The bounded list identified by CapabilityCommandID.
           */
         virtual const uci::type::PulseDataQFT::CapabilityCommandID& getCapabilityCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommandID.
           *
           * @return The bounded list identified by CapabilityCommandID.
           */
         virtual uci::type::PulseDataQFT::CapabilityCommandID& getCapabilityCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityCommandID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityCommandID(const uci::type::PulseDataQFT::CapabilityCommandID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::PulseDataQFT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::PulseDataQFT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::PulseDataQFT::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalFilter.
           */
         virtual const uci::type::DateTimeRangeType& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalFilter.
           */
         virtual uci::type::DateTimeRangeType& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TemporalFilter to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TemporalFilter
           */
         virtual void setTemporalFilter(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TemporalFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TemporalFilter is emabled or not
           */
         virtual bool hasTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TemporalFilter
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableTemporalFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TemporalFilter */
         virtual void clearTemporalFilter()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PulseDataQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PulseDataQFT to copy from
           */
         PulseDataQFT(const PulseDataQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PulseDataQFT to the contents of the PulseDataQFT on the right hand
           * side (rhs) of the assignment operator.PulseDataQFT [only available to derived classes]
           *
           * @param rhs The PulseDataQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::PulseDataQFT
           * @return A constant reference to this PulseDataQFT.
           */
         const PulseDataQFT& operator=(const PulseDataQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PulseDataQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PulseDataQFT_h

