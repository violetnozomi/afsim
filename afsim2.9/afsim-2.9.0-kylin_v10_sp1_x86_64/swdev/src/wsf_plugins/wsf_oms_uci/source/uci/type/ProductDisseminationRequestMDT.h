// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ProductDisseminationRequestMDT_h
#define Uci__Type__ProductDisseminationRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductDisseminationRequestID_Type_h)
# include "uci/type/ProductDisseminationRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Type__DownloadPriorityEnum_h)
# include "uci/type/DownloadPriorityEnum.h"
#endif

#if !defined(Uci__Type__DisseminationRequirementsType_h)
# include "uci/type/DisseminationRequirementsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductDisseminationRequestMDT sequence accessor class */
      class ProductDisseminationRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductDisseminationRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationRequestMDT
           *
           * @return This accessor's type constant, i.e. ProductDisseminationRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductDisseminationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductDisseminationRequestID.
           */
         virtual const uci::type::ProductDisseminationRequestID_Type& getProductDisseminationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductDisseminationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductDisseminationRequestID.
           */
         virtual uci::type::ProductDisseminationRequestID_Type& getProductDisseminationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductDisseminationRequestID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductDisseminationRequestID
           */
         virtual void setProductDisseminationRequestID(const uci::type::ProductDisseminationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Rank.
           *
           * @return The acecssor that provides access to the complex content that is identified by Rank.
           */
         virtual const uci::type::RankingType& getRank() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Rank.
           *
           * @return The acecssor that provides access to the complex content that is identified by Rank.
           */
         virtual uci::type::RankingType& getRank()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Rank to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Rank
           */
         virtual void setRank(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PreemptLowerRank.
           *
           * @return The value of the enumeration identified by PreemptLowerRank.
           */
         virtual const uci::type::DownloadPriorityEnum& getPreemptLowerRank() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PreemptLowerRank.
           *
           * @return The value of the enumeration identified by PreemptLowerRank.
           */
         virtual uci::type::DownloadPriorityEnum& getPreemptLowerRank()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PreemptLowerRank.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPreemptLowerRank(const uci::type::DownloadPriorityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PreemptLowerRank.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPreemptLowerRank(uci::type::DownloadPriorityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationRequirements.
           */
         virtual const uci::type::DisseminationRequirementsType& getDisseminationRequirements() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationRequirements.
           */
         virtual uci::type::DisseminationRequirementsType& getDisseminationRequirements()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DisseminationRequirements to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DisseminationRequirements
           */
         virtual void setDisseminationRequirements(const uci::type::DisseminationRequirementsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationRequestMDT to copy from
           */
         ProductDisseminationRequestMDT(const ProductDisseminationRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationRequestMDT to the contents of the
           * ProductDisseminationRequestMDT on the right hand side (rhs) of the assignment operator.ProductDisseminationRequestMDT
           * [only available to derived classes]
           *
           * @param rhs The ProductDisseminationRequestMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductDisseminationRequestMDT
           * @return A constant reference to this ProductDisseminationRequestMDT.
           */
         const ProductDisseminationRequestMDT& operator=(const ProductDisseminationRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationRequestMDT_h

