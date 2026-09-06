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
#ifndef Uci__Type__CapabilityCoverageAreaRequestQFT_h
#define Uci__Type__CapabilityCoverageAreaRequestQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityCoverageAreaRequestID_Type_h)
# include "uci/type/CapabilityCoverageAreaRequestID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityCoverageAreaRequestQFT sequence accessor class */
      class CapabilityCoverageAreaRequestQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CapabilityCoverageAreaRequestQFT()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityCoverageAreaRequestQFT
           *
           * @return This accessor's type constant, i.e. CapabilityCoverageAreaRequestQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityCoverageAreaRequestQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityCoverageAreaRequestQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityCoverageAreaRequestID_Type, uci::type::accessorType::capabilityCoverageAreaRequestID_Type> CapabilityCoverageAreaRequestID;

         /** Returns the bounded list that is identified by the CapabilityCoverageAreaRequestID.
           *
           * @return The bounded list identified by CapabilityCoverageAreaRequestID.
           */
         virtual const uci::type::CapabilityCoverageAreaRequestQFT::CapabilityCoverageAreaRequestID& getCapabilityCoverageAreaRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCoverageAreaRequestID.
           *
           * @return The bounded list identified by CapabilityCoverageAreaRequestID.
           */
         virtual uci::type::CapabilityCoverageAreaRequestQFT::CapabilityCoverageAreaRequestID& getCapabilityCoverageAreaRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityCoverageAreaRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityCoverageAreaRequestID(const uci::type::CapabilityCoverageAreaRequestQFT::CapabilityCoverageAreaRequestID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCoverageAreaRequestQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCoverageAreaRequestQFT to copy from
           */
         CapabilityCoverageAreaRequestQFT(const CapabilityCoverageAreaRequestQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCoverageAreaRequestQFT to the contents of the
           * CapabilityCoverageAreaRequestQFT on the right hand side (rhs) of the assignment
           * operator.CapabilityCoverageAreaRequestQFT [only available to derived classes]
           *
           * @param rhs The CapabilityCoverageAreaRequestQFT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CapabilityCoverageAreaRequestQFT
           * @return A constant reference to this CapabilityCoverageAreaRequestQFT.
           */
         const CapabilityCoverageAreaRequestQFT& operator=(const CapabilityCoverageAreaRequestQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCoverageAreaRequestQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityCoverageAreaRequestQFT_h

