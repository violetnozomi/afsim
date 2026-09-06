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
#ifndef Uci__Type__RF_ResourceRequestQFT_h
#define Uci__Type__RF_ResourceRequestQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ResourceRequestID_Type_h)
# include "uci/type/ResourceRequestID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class RF_ResourceRequestQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~RF_ResourceRequestQFT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ResourceRequestQFT
           *
           * @return This accessor's type constant, i.e. RF_ResourceRequestQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ResourceRequestQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ResourceRequestQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ResourceRequestID_Type, uci::type::accessorType::resourceRequestID_Type> ResourceRequestID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 3] */
         typedef uci::base::BoundedList<uci::type::MessageStateEnum, uci::type::accessorType::messageStateEnum> ResourceRequestState;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityID_Type, uci::type::accessorType::supportCapabilityID_Type> AntennaID;

         /** Returns the bounded list that is identified by the ResourceRequestID.
           *
           * @return The bounded list identified by ResourceRequestID.
           */
         virtual const uci::type::RF_ResourceRequestQFT::ResourceRequestID& getResourceRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequestID.
           *
           * @return The bounded list identified by ResourceRequestID.
           */
         virtual uci::type::RF_ResourceRequestQFT::ResourceRequestID& getResourceRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceRequestID(const uci::type::RF_ResourceRequestQFT::ResourceRequestID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequestState.
           *
           * @return The bounded list identified by ResourceRequestState.
           */
         virtual const uci::type::RF_ResourceRequestQFT::ResourceRequestState& getResourceRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequestState.
           *
           * @return The bounded list identified by ResourceRequestState.
           */
         virtual uci::type::RF_ResourceRequestQFT::ResourceRequestState& getResourceRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceRequestState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceRequestState(const uci::type::RF_ResourceRequestQFT::ResourceRequestState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaID.
           *
           * @return The bounded list identified by AntennaID.
           */
         virtual const uci::type::RF_ResourceRequestQFT::AntennaID& getAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaID.
           *
           * @return The bounded list identified by AntennaID.
           */
         virtual uci::type::RF_ResourceRequestQFT::AntennaID& getAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaID(const uci::type::RF_ResourceRequestQFT::AntennaID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ResourceRequestQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ResourceRequestQFT to copy from
           */
         RF_ResourceRequestQFT(const RF_ResourceRequestQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ResourceRequestQFT to the contents of the RF_ResourceRequestQFT
           * on the right hand side (rhs) of the assignment operator.RF_ResourceRequestQFT [only available to derived classes]
           *
           * @param rhs The RF_ResourceRequestQFT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RF_ResourceRequestQFT
           * @return A constant reference to this RF_ResourceRequestQFT.
           */
         const RF_ResourceRequestQFT& operator=(const RF_ResourceRequestQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ResourceRequestQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ResourceRequestQFT_h

