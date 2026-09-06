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
#ifndef Uci__Type__CommMADL_NetworkLayoutType_h
#define Uci__Type__CommMADL_NetworkLayoutType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NodeRangeType_h)
# include "uci/type/CommMADL_NodeRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_NetworkLayoutType sequence accessor class */
      class CommMADL_NetworkLayoutType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_NetworkLayoutType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_NetworkLayoutType
           *
           * @return This accessor's type constant, i.e. CommMADL_NetworkLayoutType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_NetworkLayoutType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_NetworkLayoutType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfNodes.
           *
           * @return The value of the simple primitive data type identified by NumberOfNodes.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getNumberOfNodes() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfNodes.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfNodes(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FirstNodeID.
           *
           * @return The value of the simple primitive data type identified by FirstNodeID.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getFirstNodeID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FirstNodeID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFirstNodeID(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_NetworkLayoutType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_NetworkLayoutType to copy from
           */
         CommMADL_NetworkLayoutType(const CommMADL_NetworkLayoutType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_NetworkLayoutType to the contents of the
           * CommMADL_NetworkLayoutType on the right hand side (rhs) of the assignment operator.CommMADL_NetworkLayoutType [only
           * available to derived classes]
           *
           * @param rhs The CommMADL_NetworkLayoutType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommMADL_NetworkLayoutType
           * @return A constant reference to this CommMADL_NetworkLayoutType.
           */
         const CommMADL_NetworkLayoutType& operator=(const CommMADL_NetworkLayoutType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_NetworkLayoutType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_NetworkLayoutType_h

