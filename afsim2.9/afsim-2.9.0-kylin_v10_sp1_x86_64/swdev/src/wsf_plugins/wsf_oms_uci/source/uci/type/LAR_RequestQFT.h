// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__LAR_RequestQFT_h
#define Uci__Type__LAR_RequestQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__LAR_RequestID_Type_h)
# include "uci/type/LAR_RequestID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LAR_RequestQFT sequence accessor class */
      class LAR_RequestQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~LAR_RequestQFT()
         { }

         /** Returns this accessor's type constant, i.e. LAR_RequestQFT
           *
           * @return This accessor's type constant, i.e. LAR_RequestQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lAR_RequestQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LAR_RequestQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::LAR_RequestID_Type, uci::type::accessorType::lAR_RequestID_Type> LAR_RequestID;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @return The value of the simple primitive data type identified by IncludeStatus.
           */
         virtual xs::Boolean getIncludeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LAR_RequestID.
           *
           * @return The bounded list identified by LAR_RequestID.
           */
         virtual const uci::type::LAR_RequestQFT::LAR_RequestID& getLAR_RequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LAR_RequestID.
           *
           * @return The bounded list identified by LAR_RequestID.
           */
         virtual uci::type::LAR_RequestQFT::LAR_RequestID& getLAR_RequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LAR_RequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLAR_RequestID(const uci::type::LAR_RequestQFT::LAR_RequestID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LAR_RequestQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_RequestQFT to copy from
           */
         LAR_RequestQFT(const LAR_RequestQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_RequestQFT to the contents of the LAR_RequestQFT on the right
           * hand side (rhs) of the assignment operator.LAR_RequestQFT [only available to derived classes]
           *
           * @param rhs The LAR_RequestQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LAR_RequestQFT
           * @return A constant reference to this LAR_RequestQFT.
           */
         const LAR_RequestQFT& operator=(const LAR_RequestQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_RequestQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LAR_RequestQFT_h

