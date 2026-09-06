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
#ifndef Uci__Type__CommLink16DelayReductionType_h
#define Uci__Type__CommLink16DelayReductionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The Delay Reduction Participation Group (PG) Index Number specifies which message(s) are subject to Delay Reduction. */
      class CommLink16DelayReductionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16DelayReductionType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16DelayReductionType
           *
           * @return This accessor's type constant, i.e. CommLink16DelayReductionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16DelayReductionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16DelayReductionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** J-Message Label; [Minimum inclusive: 0] [Maximum inclusive: 31] */
         typedef uci::base::IntAccessor MessageLabel;
         //  The type of primitive data accessed by MessageLabel
         typedef xs::Int MessageLabelValue;

         /** J-Message Sub-Label; [Minimum inclusive: 0] [Maximum inclusive: 7] */
         typedef uci::base::IntAccessor MessageSubLabel;
         //  The type of primitive data accessed by MessageSubLabel
         typedef xs::Int MessageSubLabelValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the UseMIDS_DelayReduction.
           *
           * @return The value of the simple primitive data type identified by UseMIDS_DelayReduction.
           */
         virtual xs::Boolean getUseMIDS_DelayReduction() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UseMIDS_DelayReduction.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUseMIDS_DelayReduction(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MessageLabel.
           *
           * @return The value of the simple primitive data type identified by MessageLabel.
           */
         virtual uci::type::CommLink16DelayReductionType::MessageLabelValue getMessageLabel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MessageLabel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMessageLabel(uci::type::CommLink16DelayReductionType::MessageLabelValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MessageSubLabel.
           *
           * @return The value of the simple primitive data type identified by MessageSubLabel.
           */
         virtual uci::type::CommLink16DelayReductionType::MessageSubLabelValue getMessageSubLabel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MessageSubLabel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMessageSubLabel(uci::type::CommLink16DelayReductionType::MessageSubLabelValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16DelayReductionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16DelayReductionType to copy from
           */
         CommLink16DelayReductionType(const CommLink16DelayReductionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16DelayReductionType to the contents of the
           * CommLink16DelayReductionType on the right hand side (rhs) of the assignment operator.CommLink16DelayReductionType
           * [only available to derived classes]
           *
           * @param rhs The CommLink16DelayReductionType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommLink16DelayReductionType
           * @return A constant reference to this CommLink16DelayReductionType.
           */
         const CommLink16DelayReductionType& operator=(const CommLink16DelayReductionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16DelayReductionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16DelayReductionType_h

