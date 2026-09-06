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
#ifndef Uci__Type__CommMADL_LinkCommandListType_h
#define Uci__Type__CommMADL_LinkCommandListType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommMADL_LinkCommandType_h)
# include "uci/type/CommMADL_LinkCommandType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_LinkCommandListType sequence accessor class */
      class CommMADL_LinkCommandListType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_LinkCommandListType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_LinkCommandListType
           *
           * @return This accessor's type constant, i.e. CommMADL_LinkCommandListType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_LinkCommandListType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_LinkCommandListType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Separate entries are included for each of a possible 2 links. [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::CommMADL_LinkCommandType, uci::type::accessorType::commMADL_LinkCommandType> LinkCmdState;

         /** Returns the bounded list that is identified by the LinkCmdState.
           *
           * @return The bounded list identified by LinkCmdState.
           */
         virtual const uci::type::CommMADL_LinkCommandListType::LinkCmdState& getLinkCmdState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkCmdState.
           *
           * @return The bounded list identified by LinkCmdState.
           */
         virtual uci::type::CommMADL_LinkCommandListType::LinkCmdState& getLinkCmdState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LinkCmdState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLinkCmdState(const uci::type::CommMADL_LinkCommandListType::LinkCmdState& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_LinkCommandListType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_LinkCommandListType to copy from
           */
         CommMADL_LinkCommandListType(const CommMADL_LinkCommandListType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_LinkCommandListType to the contents of the
           * CommMADL_LinkCommandListType on the right hand side (rhs) of the assignment operator.CommMADL_LinkCommandListType
           * [only available to derived classes]
           *
           * @param rhs The CommMADL_LinkCommandListType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommMADL_LinkCommandListType
           * @return A constant reference to this CommMADL_LinkCommandListType.
           */
         const CommMADL_LinkCommandListType& operator=(const CommMADL_LinkCommandListType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_LinkCommandListType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_LinkCommandListType_h

