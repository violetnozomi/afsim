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
#ifndef Uci__Type__DMPI_StatusQFT_h
#define Uci__Type__DMPI_StatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__DMPI_StoreStateEnum_h)
# include "uci/type/DMPI_StoreStateEnum.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_StatusQFT sequence accessor class */
      class DMPI_StatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~DMPI_StatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_StatusQFT
           *
           * @return This accessor's type constant, i.e. DMPI_StatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_StatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_StatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The DMPI IDs to query. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DMPI_ID_Type, uci::type::accessorType::dMPI_ID_Type> DMPI_ID;

         /** The Capability states to query. [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::DMPI_StoreStateEnum, uci::type::accessorType::dMPI_StoreStateEnum> CapabilityState;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeDMPI.
           *
           * @return The value of the simple primitive data type identified by IncludeDMPI.
           */
         virtual xs::Boolean getIncludeDMPI() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeDMPI.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeDMPI(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_ID.
           *
           * @return The bounded list identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_StatusQFT::DMPI_ID& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_ID.
           *
           * @return The bounded list identified by DMPI_ID.
           */
         virtual uci::type::DMPI_StatusQFT::DMPI_ID& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DMPI_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDMPI_ID(const uci::type::DMPI_StatusQFT::DMPI_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityState.
           *
           * @return The bounded list identified by CapabilityState.
           */
         virtual const uci::type::DMPI_StatusQFT::CapabilityState& getCapabilityState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityState.
           *
           * @return The bounded list identified by CapabilityState.
           */
         virtual uci::type::DMPI_StatusQFT::CapabilityState& getCapabilityState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityState(const uci::type::DMPI_StatusQFT::CapabilityState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::DMPI_StatusQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::DMPI_StatusQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::DMPI_StatusQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::DMPI_StatusQFT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::DMPI_StatusQFT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::DMPI_StatusQFT::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::DMPI_StatusQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::DMPI_StatusQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::DMPI_StatusQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_StatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_StatusQFT to copy from
           */
         DMPI_StatusQFT(const DMPI_StatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_StatusQFT to the contents of the DMPI_StatusQFT on the right
           * hand side (rhs) of the assignment operator.DMPI_StatusQFT [only available to derived classes]
           *
           * @param rhs The DMPI_StatusQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::DMPI_StatusQFT
           * @return A constant reference to this DMPI_StatusQFT.
           */
         const DMPI_StatusQFT& operator=(const DMPI_StatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_StatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_StatusQFT_h

