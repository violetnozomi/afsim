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
#ifndef Uci__Type__DataPlanMDT_h
#define Uci__Type__DataPlanMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DataPlanID_Type_h)
# include "uci/type/DataPlanID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanApprovalEnum_h)
# include "uci/type/DataPlanApprovalEnum.h"
#endif

#if !defined(Uci__Type__DataPlanDetailsType_h)
# include "uci/type/DataPlanDetailsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class DataPlanMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataPlanMDT()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanMDT
           *
           * @return This accessor's type constant, i.e. DataPlanMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanID.
           */
         virtual const uci::type::DataPlanID_Type& getDataPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanID.
           */
         virtual uci::type::DataPlanID_Type& getDataPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataPlanID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataPlanID
           */
         virtual void setDataPlanID(const uci::type::DataPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalState.
           *
           * @return The value of the enumeration identified by ApprovalState.
           */
         virtual const uci::type::DataPlanApprovalEnum& getApprovalState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalState.
           *
           * @return The value of the enumeration identified by ApprovalState.
           */
         virtual uci::type::DataPlanApprovalEnum& getApprovalState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalState(const uci::type::DataPlanApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalState(uci::type::DataPlanApprovalEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by Details.
           */
         virtual const uci::type::DataPlanDetailsType& getDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by Details.
           */
         virtual uci::type::DataPlanDetailsType& getDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Details to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Details
           */
         virtual void setDetails(const uci::type::DataPlanDetailsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanMDT to copy from
           */
         DataPlanMDT(const DataPlanMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanMDT to the contents of the DataPlanMDT on the right hand
           * side (rhs) of the assignment operator.DataPlanMDT [only available to derived classes]
           *
           * @param rhs The DataPlanMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::DataPlanMDT
           * @return A constant reference to this DataPlanMDT.
           */
         const DataPlanMDT& operator=(const DataPlanMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanMDT_h

