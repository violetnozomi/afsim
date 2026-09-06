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
#ifndef Uci__Type__CommAssetOrbitQFT_h
#define Uci__Type__CommAssetOrbitQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommAssetID_Type_h)
# include "uci/type/CommAssetID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommAssetOrbitQFT sequence accessor class */
      class CommAssetOrbitQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CommAssetOrbitQFT()
         { }

         /** Returns this accessor's type constant, i.e. CommAssetOrbitQFT
           *
           * @return This accessor's type constant, i.e. CommAssetOrbitQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commAssetOrbitQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommAssetOrbitQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommAssetID_Type, uci::type::accessorType::commAssetID_Type> CommAssetID;

         /** Returns the bounded list that is identified by the CommAssetID.
           *
           * @return The bounded list identified by CommAssetID.
           */
         virtual const uci::type::CommAssetOrbitQFT::CommAssetID& getCommAssetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommAssetID.
           *
           * @return The bounded list identified by CommAssetID.
           */
         virtual uci::type::CommAssetOrbitQFT::CommAssetID& getCommAssetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommAssetID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommAssetID(const uci::type::CommAssetOrbitQFT::CommAssetID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommAssetOrbitQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommAssetOrbitQFT to copy from
           */
         CommAssetOrbitQFT(const CommAssetOrbitQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommAssetOrbitQFT to the contents of the CommAssetOrbitQFT on the
           * right hand side (rhs) of the assignment operator.CommAssetOrbitQFT [only available to derived classes]
           *
           * @param rhs The CommAssetOrbitQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommAssetOrbitQFT
           * @return A constant reference to this CommAssetOrbitQFT.
           */
         const CommAssetOrbitQFT& operator=(const CommAssetOrbitQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommAssetOrbitQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommAssetOrbitQFT_h

