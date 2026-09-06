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
#ifndef Uci__Type__CommLink16NetSelectionType_h
#define Uci__Type__CommLink16NetSelectionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommLink16NetSelectionType sequence accessor class */
      class CommLink16NetSelectionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16NetSelectionType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16NetSelectionType
           *
           * @return This accessor's type constant, i.e. CommLink16NetSelectionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16NetSelectionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16NetSelectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Net number request for net selection. Used with the NetSelectionStatusPG_Index. Range 0-127, default 0.; [Minimum
           * inclusive: 0] [Maximum inclusive: 127]
           */
         typedef uci::base::IntAccessor NetSelectionStatusNet;
         //  The type of primitive data accessed by NetSelectionStatusNet
         typedef xs::Int NetSelectionStatusNetValue;

         /** PG index number for net selection. Range 0, 4 to 511 except 9, 12, 13, default 0.; [Minimum inclusive: 0] [Maximum
           * inclusive: 511]
           */
         typedef uci::base::IntAccessor NetSelectionStatusPG_Index;
         //  The type of primitive data accessed by NetSelectionStatusPG_Index
         typedef xs::Int NetSelectionStatusPG_IndexValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the NetSelectionStatusNet.
           *
           * @return The value of the simple primitive data type identified by NetSelectionStatusNet.
           */
         virtual uci::type::CommLink16NetSelectionType::NetSelectionStatusNetValue getNetSelectionStatusNet() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetSelectionStatusNet.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetSelectionStatusNet(uci::type::CommLink16NetSelectionType::NetSelectionStatusNetValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NetSelectionStatusPG_Index.
           *
           * @return The value of the simple primitive data type identified by NetSelectionStatusPG_Index.
           */
         virtual uci::type::CommLink16NetSelectionType::NetSelectionStatusPG_IndexValue getNetSelectionStatusPG_Index() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetSelectionStatusPG_Index.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetSelectionStatusPG_Index(uci::type::CommLink16NetSelectionType::NetSelectionStatusPG_IndexValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16NetSelectionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16NetSelectionType to copy from
           */
         CommLink16NetSelectionType(const CommLink16NetSelectionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16NetSelectionType to the contents of the
           * CommLink16NetSelectionType on the right hand side (rhs) of the assignment operator.CommLink16NetSelectionType [only
           * available to derived classes]
           *
           * @param rhs The CommLink16NetSelectionType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommLink16NetSelectionType
           * @return A constant reference to this CommLink16NetSelectionType.
           */
         const CommLink16NetSelectionType& operator=(const CommLink16NetSelectionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16NetSelectionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16NetSelectionType_h

