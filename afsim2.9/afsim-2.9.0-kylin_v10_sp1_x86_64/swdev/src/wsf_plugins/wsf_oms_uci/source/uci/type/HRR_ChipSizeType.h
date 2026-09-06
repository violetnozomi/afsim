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
#ifndef Uci__Type__HRR_ChipSizeType_h
#define Uci__Type__HRR_ChipSizeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the HRR_ChipSizeType sequence accessor class */
      class HRR_ChipSizeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~HRR_ChipSizeType()
         { }

         /** Returns this accessor's type constant, i.e. HRR_ChipSizeType
           *
           * @return This accessor's type constant, i.e. HRR_ChipSizeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::hRR_ChipSizeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const HRR_ChipSizeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RangeSize.
           *
           * @return The value of the simple primitive data type identified by RangeSize.
           */
         virtual xs::UnsignedInt getRangeSize() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RangeSize.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRangeSize(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DopplerSize.
           *
           * @return The value of the simple primitive data type identified by DopplerSize.
           */
         virtual xs::UnsignedInt getDopplerSize() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DopplerSize.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDopplerSize(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         HRR_ChipSizeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The HRR_ChipSizeType to copy from
           */
         HRR_ChipSizeType(const HRR_ChipSizeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this HRR_ChipSizeType to the contents of the HRR_ChipSizeType on the
           * right hand side (rhs) of the assignment operator.HRR_ChipSizeType [only available to derived classes]
           *
           * @param rhs The HRR_ChipSizeType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::HRR_ChipSizeType
           * @return A constant reference to this HRR_ChipSizeType.
           */
         const HRR_ChipSizeType& operator=(const HRR_ChipSizeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // HRR_ChipSizeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__HRR_ChipSizeType_h

