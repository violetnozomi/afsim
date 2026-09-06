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
#ifndef Uci__Type__EA_BeamGroupType_h
#define Uci__Type__EA_BeamGroupType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TxBeamType_h)
# include "uci/type/TxBeamType.h"
#endif

#if !defined(Uci__Type__RxBeamType_h)
# include "uci/type/RxBeamType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_BeamGroupType sequence accessor class */
      class EA_BeamGroupType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_BeamGroupType()
         { }

         /** Returns this accessor's type constant, i.e. EA_BeamGroupType
           *
           * @return This accessor's type constant, i.e. EA_BeamGroupType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_BeamGroupType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_BeamGroupType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates current jamming being performed by a specific Beam. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TxBeamType, uci::type::accessorType::txBeamType> TxBeam;

         /** Indicates a receive beam in use by EA to perform tracking. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RxBeamType, uci::type::accessorType::rxBeamType> RxBeam;

         /** Returns the bounded list that is identified by the TxBeam.
           *
           * @return The bounded list identified by TxBeam.
           */
         virtual const uci::type::EA_BeamGroupType::TxBeam& getTxBeam() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TxBeam.
           *
           * @return The bounded list identified by TxBeam.
           */
         virtual uci::type::EA_BeamGroupType::TxBeam& getTxBeam()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TxBeam.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTxBeam(const uci::type::EA_BeamGroupType::TxBeam& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RxBeam.
           *
           * @return The bounded list identified by RxBeam.
           */
         virtual const uci::type::EA_BeamGroupType::RxBeam& getRxBeam() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RxBeam.
           *
           * @return The bounded list identified by RxBeam.
           */
         virtual uci::type::EA_BeamGroupType::RxBeam& getRxBeam()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RxBeam.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRxBeam(const uci::type::EA_BeamGroupType::RxBeam& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_BeamGroupType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_BeamGroupType to copy from
           */
         EA_BeamGroupType(const EA_BeamGroupType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_BeamGroupType to the contents of the EA_BeamGroupType on the
           * right hand side (rhs) of the assignment operator.EA_BeamGroupType [only available to derived classes]
           *
           * @param rhs The EA_BeamGroupType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EA_BeamGroupType
           * @return A constant reference to this EA_BeamGroupType.
           */
         const EA_BeamGroupType& operator=(const EA_BeamGroupType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_BeamGroupType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_BeamGroupType_h

