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
#ifndef Uci__Type__CommMADL_LinkCommandType_h
#define Uci__Type__CommMADL_LinkCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NetLinkOpStateEnum_h)
# include "uci/type/CommMADL_NetLinkOpStateEnum.h"
#endif

#if !defined(Uci__Type__CommMADL_NetLinkSearchModeEnum_h)
# include "uci/type/CommMADL_NetLinkSearchModeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_LinkCommandType sequence accessor class */
      class CommMADL_LinkCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_LinkCommandType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_LinkCommandType
           *
           * @return This accessor's type constant, i.e. CommMADL_LinkCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_LinkCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_LinkCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalState.
           *
           * @return The value of the enumeration identified by OperationalState.
           */
         virtual const uci::type::CommMADL_NetLinkOpStateEnum& getOperationalState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalState.
           *
           * @return The value of the enumeration identified by OperationalState.
           */
         virtual uci::type::CommMADL_NetLinkOpStateEnum& getOperationalState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalState(const uci::type::CommMADL_NetLinkOpStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalState(uci::type::CommMADL_NetLinkOpStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SearchMode.
           *
           * @return The value of the enumeration identified by SearchMode.
           */
         virtual const uci::type::CommMADL_NetLinkSearchModeEnum& getSearchMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SearchMode.
           *
           * @return The value of the enumeration identified by SearchMode.
           */
         virtual uci::type::CommMADL_NetLinkSearchModeEnum& getSearchMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SearchMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSearchMode(const uci::type::CommMADL_NetLinkSearchModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SearchMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSearchMode(uci::type::CommMADL_NetLinkSearchModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_LinkCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_LinkCommandType to copy from
           */
         CommMADL_LinkCommandType(const CommMADL_LinkCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_LinkCommandType to the contents of the
           * CommMADL_LinkCommandType on the right hand side (rhs) of the assignment operator.CommMADL_LinkCommandType [only
           * available to derived classes]
           *
           * @param rhs The CommMADL_LinkCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommMADL_LinkCommandType
           * @return A constant reference to this CommMADL_LinkCommandType.
           */
         const CommMADL_LinkCommandType& operator=(const CommMADL_LinkCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_LinkCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_LinkCommandType_h

