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
#ifndef Uci__Type__ProductClassificationRequestMDT_h
#define Uci__Type__ProductClassificationRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductClassificationRequestID_Type_h)
# include "uci/type/ProductClassificationRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ProductClassificationRequestEnum_h)
# include "uci/type/ProductClassificationRequestEnum.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Type__DownloadPriorityEnum_h)
# include "uci/type/DownloadPriorityEnum.h"
#endif

#if !defined(Uci__Type__ClassificationRequirementsType_h)
# include "uci/type/ClassificationRequirementsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductClassificationRequestMDT sequence accessor class */
      class ProductClassificationRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductClassificationRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductClassificationRequestMDT
           *
           * @return This accessor's type constant, i.e. ProductClassificationRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productClassificationRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductClassificationRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductClassificationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ProductClassificationRequestID.
           */
         virtual const uci::type::ProductClassificationRequestID_Type& getProductClassificationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductClassificationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ProductClassificationRequestID.
           */
         virtual uci::type::ProductClassificationRequestID_Type& getProductClassificationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductClassificationRequestID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductClassificationRequestID
           */
         virtual void setProductClassificationRequestID(const uci::type::ProductClassificationRequestID_Type& value)
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


         /** Returns the value of the enumeration that is identified by the RequestType.
           *
           * @return The value of the enumeration identified by RequestType.
           */
         virtual const uci::type::ProductClassificationRequestEnum& getRequestType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestType.
           *
           * @return The value of the enumeration identified by RequestType.
           */
         virtual uci::type::ProductClassificationRequestEnum& getRequestType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestType(const uci::type::ProductClassificationRequestEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestType(uci::type::ProductClassificationRequestEnum::EnumerationItem value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ClassificationRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClassificationRequirements.
           */
         virtual const uci::type::ClassificationRequirementsType& getClassificationRequirements() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ClassificationRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClassificationRequirements.
           */
         virtual uci::type::ClassificationRequirementsType& getClassificationRequirements()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ClassificationRequirements to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ClassificationRequirements
           */
         virtual void setClassificationRequirements(const uci::type::ClassificationRequirementsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductClassificationRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductClassificationRequestMDT to copy from
           */
         ProductClassificationRequestMDT(const ProductClassificationRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductClassificationRequestMDT to the contents of the
           * ProductClassificationRequestMDT on the right hand side (rhs) of the assignment
           * operator.ProductClassificationRequestMDT [only available to derived classes]
           *
           * @param rhs The ProductClassificationRequestMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductClassificationRequestMDT
           * @return A constant reference to this ProductClassificationRequestMDT.
           */
         const ProductClassificationRequestMDT& operator=(const ProductClassificationRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductClassificationRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductClassificationRequestMDT_h

