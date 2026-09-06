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
#ifndef Uci__Type__DataPlanActivationStateQFT_h
#define Uci__Type__DataPlanActivationStateQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DataPlanID_Type_h)
# include "uci/type/DataPlanID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanActivationStateEnum_h)
# include "uci/type/DataPlanActivationStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Query Filter Type to query for Data Plan Activation State messages. */
      class DataPlanActivationStateQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~DataPlanActivationStateQFT()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanActivationStateQFT
           *
           * @return This accessor's type constant, i.e. DataPlanActivationStateQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanActivationStateQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanActivationStateQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the DataPlan ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DataPlanID_Type, uci::type::accessorType::dataPlanID_Type> DataPlanID;

         /** Filter on the state of the data plan. [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::DataPlanActivationStateEnum, uci::type::accessorType::dataPlanActivationStateEnum> State;

         /** Returns the bounded list that is identified by the DataPlanID.
           *
           * @return The bounded list identified by DataPlanID.
           */
         virtual const uci::type::DataPlanActivationStateQFT::DataPlanID& getDataPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataPlanID.
           *
           * @return The bounded list identified by DataPlanID.
           */
         virtual uci::type::DataPlanActivationStateQFT::DataPlanID& getDataPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataPlanID(const uci::type::DataPlanActivationStateQFT::DataPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the State.
           *
           * @return The bounded list identified by State.
           */
         virtual const uci::type::DataPlanActivationStateQFT::State& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the State.
           *
           * @return The bounded list identified by State.
           */
         virtual uci::type::DataPlanActivationStateQFT::State& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the State.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setState(const uci::type::DataPlanActivationStateQFT::State& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanActivationStateQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanActivationStateQFT to copy from
           */
         DataPlanActivationStateQFT(const DataPlanActivationStateQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanActivationStateQFT to the contents of the
           * DataPlanActivationStateQFT on the right hand side (rhs) of the assignment operator.DataPlanActivationStateQFT [only
           * available to derived classes]
           *
           * @param rhs The DataPlanActivationStateQFT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DataPlanActivationStateQFT
           * @return A constant reference to this DataPlanActivationStateQFT.
           */
         const DataPlanActivationStateQFT& operator=(const DataPlanActivationStateQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanActivationStateQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanActivationStateQFT_h

