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
#ifndef Uci__Type__CargoStatusQFT_h
#define Uci__Type__CargoStatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CargoID_Type_h)
# include "uci/type/CargoID_Type.h"
#endif

#if !defined(Uci__Type__CargoPackagingEnum_h)
# include "uci/type/CargoPackagingEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CargoStatusQFT sequence accessor class */
      class CargoStatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CargoStatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. CargoStatusQFT
           *
           * @return This accessor's type constant, i.e. CargoStatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cargoStatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CargoStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CargoID_Type, uci::type::accessorType::cargoID_Type> CargoID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 5] */
         typedef uci::base::BoundedList<uci::type::CargoPackagingEnum, uci::type::accessorType::cargoPackagingEnum> CargoType;

         /** Returns the bounded list that is identified by the CargoID.
           *
           * @return The bounded list identified by CargoID.
           */
         virtual const uci::type::CargoStatusQFT::CargoID& getCargoID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CargoID.
           *
           * @return The bounded list identified by CargoID.
           */
         virtual uci::type::CargoStatusQFT::CargoID& getCargoID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CargoID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCargoID(const uci::type::CargoStatusQFT::CargoID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CargoType.
           *
           * @return The bounded list identified by CargoType.
           */
         virtual const uci::type::CargoStatusQFT::CargoType& getCargoType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CargoType.
           *
           * @return The bounded list identified by CargoType.
           */
         virtual uci::type::CargoStatusQFT::CargoType& getCargoType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CargoType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCargoType(const uci::type::CargoStatusQFT::CargoType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CargoStatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CargoStatusQFT to copy from
           */
         CargoStatusQFT(const CargoStatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CargoStatusQFT to the contents of the CargoStatusQFT on the right
           * hand side (rhs) of the assignment operator.CargoStatusQFT [only available to derived classes]
           *
           * @param rhs The CargoStatusQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CargoStatusQFT
           * @return A constant reference to this CargoStatusQFT.
           */
         const CargoStatusQFT& operator=(const CargoStatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CargoStatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CargoStatusQFT_h

