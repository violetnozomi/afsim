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
#ifndef Uci__Type__DataPlanQFT_h
#define Uci__Type__DataPlanQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanID_Type_h)
# include "uci/type/DataPlanID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanApprovalEnum_h)
# include "uci/type/DataPlanApprovalEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Query Filter Type to query for Data Plans. */
      class DataPlanQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~DataPlanQFT()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanQFT
           *
           * @return This accessor's type constant, i.e. DataPlanQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on who the Data Plan is allocated to [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> AllocatedToID;

         /** Filter on the Data Plan ID. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DataPlanID_Type, uci::type::accessorType::dataPlanID_Type> DataPlanID;

         /** Filter on the state of the Data Plan [Minimum occurrences: 0] [Maximum occurrences: 3] */
         typedef uci::base::BoundedList<uci::type::DataPlanApprovalEnum, uci::type::accessorType::dataPlanApprovalEnum> DataPlanState;

         /** Returns the bounded list that is identified by the AllocatedToID.
           *
           * @return The bounded list identified by AllocatedToID.
           */
         virtual const uci::type::DataPlanQFT::AllocatedToID& getAllocatedToID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AllocatedToID.
           *
           * @return The bounded list identified by AllocatedToID.
           */
         virtual uci::type::DataPlanQFT::AllocatedToID& getAllocatedToID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AllocatedToID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllocatedToID(const uci::type::DataPlanQFT::AllocatedToID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataPlanID.
           *
           * @return The bounded list identified by DataPlanID.
           */
         virtual const uci::type::DataPlanQFT::DataPlanID& getDataPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataPlanID.
           *
           * @return The bounded list identified by DataPlanID.
           */
         virtual uci::type::DataPlanQFT::DataPlanID& getDataPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataPlanID(const uci::type::DataPlanQFT::DataPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataPlanState.
           *
           * @return The bounded list identified by DataPlanState.
           */
         virtual const uci::type::DataPlanQFT::DataPlanState& getDataPlanState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataPlanState.
           *
           * @return The bounded list identified by DataPlanState.
           */
         virtual uci::type::DataPlanQFT::DataPlanState& getDataPlanState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataPlanState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataPlanState(const uci::type::DataPlanQFT::DataPlanState& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanQFT to copy from
           */
         DataPlanQFT(const DataPlanQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanQFT to the contents of the DataPlanQFT on the right hand
           * side (rhs) of the assignment operator.DataPlanQFT [only available to derived classes]
           *
           * @param rhs The DataPlanQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::DataPlanQFT
           * @return A constant reference to this DataPlanQFT.
           */
         const DataPlanQFT& operator=(const DataPlanQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanQFT_h

