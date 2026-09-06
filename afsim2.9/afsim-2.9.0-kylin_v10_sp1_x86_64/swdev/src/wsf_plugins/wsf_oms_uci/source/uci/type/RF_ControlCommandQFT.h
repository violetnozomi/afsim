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
#ifndef Uci__Type__RF_ControlCommandQFT_h
#define Uci__Type__RF_ControlCommandQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__RF_ProfileID_Type_h)
# include "uci/type/RF_ProfileID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Query Filter for RF_ControlCommand. Filter elements with cardinality greater than 1 are logically ORed. Sibling
        * filter elements are logically ANDed.
        */
      class RF_ControlCommandQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~RF_ControlCommandQFT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ControlCommandQFT
           *
           * @return This accessor's type constant, i.e. RF_ControlCommandQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ControlCommandQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ControlCommandQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the Applicable Capability ID's for which the query applies [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> ApplicableCapabilityID;

         /** Indicates the RF Profile IDs for which the query applies [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_ProfileID_Type, uci::type::accessorType::rF_ProfileID_Type> RF_ProfileID;

         /** Returns the bounded list that is identified by the ApplicableCapabilityID.
           *
           * @return The bounded list identified by ApplicableCapabilityID.
           */
         virtual const uci::type::RF_ControlCommandQFT::ApplicableCapabilityID& getApplicableCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableCapabilityID.
           *
           * @return The bounded list identified by ApplicableCapabilityID.
           */
         virtual uci::type::RF_ControlCommandQFT::ApplicableCapabilityID& getApplicableCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableCapabilityID(const uci::type::RF_ControlCommandQFT::ApplicableCapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_ProfileID.
           *
           * @return The bounded list identified by RF_ProfileID.
           */
         virtual const uci::type::RF_ControlCommandQFT::RF_ProfileID& getRF_ProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_ProfileID.
           *
           * @return The bounded list identified by RF_ProfileID.
           */
         virtual uci::type::RF_ControlCommandQFT::RF_ProfileID& getRF_ProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_ProfileID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_ProfileID(const uci::type::RF_ControlCommandQFT::RF_ProfileID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ControlCommandQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ControlCommandQFT to copy from
           */
         RF_ControlCommandQFT(const RF_ControlCommandQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ControlCommandQFT to the contents of the RF_ControlCommandQFT
           * on the right hand side (rhs) of the assignment operator.RF_ControlCommandQFT [only available to derived classes]
           *
           * @param rhs The RF_ControlCommandQFT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RF_ControlCommandQFT
           * @return A constant reference to this RF_ControlCommandQFT.
           */
         const RF_ControlCommandQFT& operator=(const RF_ControlCommandQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ControlCommandQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ControlCommandQFT_h

