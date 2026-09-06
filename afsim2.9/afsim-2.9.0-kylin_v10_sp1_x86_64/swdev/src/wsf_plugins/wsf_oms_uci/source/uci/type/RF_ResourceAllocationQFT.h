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
#ifndef Uci__Type__RF_ResourceAllocationQFT_h
#define Uci__Type__RF_ResourceAllocationQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ResourceRequestID_Type_h)
# include "uci/type/ResourceRequestID_Type.h"
#endif

#if !defined(Uci__Type__ResourceAllocationStateEnum_h)
# include "uci/type/ResourceAllocationStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ResourceAllocationQFT sequence accessor class */
      class RF_ResourceAllocationQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~RF_ResourceAllocationQFT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ResourceAllocationQFT
           *
           * @return This accessor's type constant, i.e. RF_ResourceAllocationQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ResourceAllocationQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ResourceAllocationQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ResourceRequestID_Type, uci::type::accessorType::resourceRequestID_Type> ResourceRequestID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::ResourceAllocationStateEnum, uci::type::accessorType::resourceAllocationStateEnum> ResourceAllocationState;

         /** Returns the bounded list that is identified by the ResourceRequestID.
           *
           * @return The bounded list identified by ResourceRequestID.
           */
         virtual const uci::type::RF_ResourceAllocationQFT::ResourceRequestID& getResourceRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequestID.
           *
           * @return The bounded list identified by ResourceRequestID.
           */
         virtual uci::type::RF_ResourceAllocationQFT::ResourceRequestID& getResourceRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceRequestID(const uci::type::RF_ResourceAllocationQFT::ResourceRequestID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceAllocationState.
           *
           * @return The bounded list identified by ResourceAllocationState.
           */
         virtual const uci::type::RF_ResourceAllocationQFT::ResourceAllocationState& getResourceAllocationState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceAllocationState.
           *
           * @return The bounded list identified by ResourceAllocationState.
           */
         virtual uci::type::RF_ResourceAllocationQFT::ResourceAllocationState& getResourceAllocationState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceAllocationState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceAllocationState(const uci::type::RF_ResourceAllocationQFT::ResourceAllocationState& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ResourceAllocationQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ResourceAllocationQFT to copy from
           */
         RF_ResourceAllocationQFT(const RF_ResourceAllocationQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ResourceAllocationQFT to the contents of the
           * RF_ResourceAllocationQFT on the right hand side (rhs) of the assignment operator.RF_ResourceAllocationQFT [only
           * available to derived classes]
           *
           * @param rhs The RF_ResourceAllocationQFT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RF_ResourceAllocationQFT
           * @return A constant reference to this RF_ResourceAllocationQFT.
           */
         const RF_ResourceAllocationQFT& operator=(const RF_ResourceAllocationQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ResourceAllocationQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ResourceAllocationQFT_h

