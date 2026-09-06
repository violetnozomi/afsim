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
#ifndef Uci__Type__CommAssetQFT_h
#define Uci__Type__CommAssetQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommSystemID_Type_h)
# include "uci/type/CommSystemID_Type.h"
#endif

#if !defined(Uci__Type__CommAssetID_Type_h)
# include "uci/type/CommAssetID_Type.h"
#endif

#if !defined(Uci__Type__CommAssetCategoryEnum_h)
# include "uci/type/CommAssetCategoryEnum.h"
#endif

#if !defined(Uci__Type__AntennaID_Type_h)
# include "uci/type/AntennaID_Type.h"
#endif

#if !defined(Uci__Type__AntennaTypeEnum_h)
# include "uci/type/AntennaTypeEnum.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

#if !defined(Uci__Type__IntegerMinMaxType_h)
# include "uci/type/IntegerMinMaxType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommAssetQFT sequence accessor class */
      class CommAssetQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CommAssetQFT()
         { }

         /** Returns this accessor's type constant, i.e. CommAssetQFT
           *
           * @return This accessor's type constant, i.e. CommAssetQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commAssetQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommAssetQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommSystemID_Type, uci::type::accessorType::commSystemID_Type> CommSystemID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommAssetID_Type, uci::type::accessorType::commAssetID_Type> CommAssetID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::CommAssetCategoryEnum, uci::type::accessorType::commAssetCategoryEnum> Category;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::AntennaID_Type, uci::type::accessorType::antennaID_Type> AntennaID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::AntennaTypeEnum, uci::type::accessorType::antennaTypeEnum> AntennaType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::base::IntAccessor, uci::base::accessorType::intAccessor> SupportedMode;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DataRateType, uci::type::accessorType::dataRateType> UplinkDataRate;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DataRateType, uci::type::accessorType::dataRateType> DownlinkDataRate;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::IntegerMinMaxType, uci::type::accessorType::integerMinMaxType> ConcurrentUserRange;

         /** Returns the bounded list that is identified by the CommSystemID.
           *
           * @return The bounded list identified by CommSystemID.
           */
         virtual const uci::type::CommAssetQFT::CommSystemID& getCommSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSystemID.
           *
           * @return The bounded list identified by CommSystemID.
           */
         virtual uci::type::CommAssetQFT::CommSystemID& getCommSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSystemID(const uci::type::CommAssetQFT::CommSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommAssetID.
           *
           * @return The bounded list identified by CommAssetID.
           */
         virtual const uci::type::CommAssetQFT::CommAssetID& getCommAssetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommAssetID.
           *
           * @return The bounded list identified by CommAssetID.
           */
         virtual uci::type::CommAssetQFT::CommAssetID& getCommAssetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommAssetID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommAssetID(const uci::type::CommAssetQFT::CommAssetID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual const uci::type::CommAssetQFT::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual uci::type::CommAssetQFT::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Category.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCategory(const uci::type::CommAssetQFT::Category& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaID.
           *
           * @return The bounded list identified by AntennaID.
           */
         virtual const uci::type::CommAssetQFT::AntennaID& getAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaID.
           *
           * @return The bounded list identified by AntennaID.
           */
         virtual uci::type::CommAssetQFT::AntennaID& getAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaID(const uci::type::CommAssetQFT::AntennaID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaType.
           *
           * @return The bounded list identified by AntennaType.
           */
         virtual const uci::type::CommAssetQFT::AntennaType& getAntennaType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaType.
           *
           * @return The bounded list identified by AntennaType.
           */
         virtual uci::type::CommAssetQFT::AntennaType& getAntennaType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaType(const uci::type::CommAssetQFT::AntennaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedMode.
           *
           * @return The bounded list identified by SupportedMode.
           */
         virtual const uci::type::CommAssetQFT::SupportedMode& getSupportedMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedMode.
           *
           * @return The bounded list identified by SupportedMode.
           */
         virtual uci::type::CommAssetQFT::SupportedMode& getSupportedMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedMode.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedMode(const uci::type::CommAssetQFT::SupportedMode& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UplinkDataRate.
           *
           * @return The bounded list identified by UplinkDataRate.
           */
         virtual const uci::type::CommAssetQFT::UplinkDataRate& getUplinkDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UplinkDataRate.
           *
           * @return The bounded list identified by UplinkDataRate.
           */
         virtual uci::type::CommAssetQFT::UplinkDataRate& getUplinkDataRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the UplinkDataRate.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setUplinkDataRate(const uci::type::CommAssetQFT::UplinkDataRate& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DownlinkDataRate.
           *
           * @return The bounded list identified by DownlinkDataRate.
           */
         virtual const uci::type::CommAssetQFT::DownlinkDataRate& getDownlinkDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DownlinkDataRate.
           *
           * @return The bounded list identified by DownlinkDataRate.
           */
         virtual uci::type::CommAssetQFT::DownlinkDataRate& getDownlinkDataRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DownlinkDataRate.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDownlinkDataRate(const uci::type::CommAssetQFT::DownlinkDataRate& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentUserRange.
           *
           * @return The bounded list identified by ConcurrentUserRange.
           */
         virtual const uci::type::CommAssetQFT::ConcurrentUserRange& getConcurrentUserRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentUserRange.
           *
           * @return The bounded list identified by ConcurrentUserRange.
           */
         virtual uci::type::CommAssetQFT::ConcurrentUserRange& getConcurrentUserRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConcurrentUserRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConcurrentUserRange(const uci::type::CommAssetQFT::ConcurrentUserRange& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommAssetQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommAssetQFT to copy from
           */
         CommAssetQFT(const CommAssetQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommAssetQFT to the contents of the CommAssetQFT on the right hand
           * side (rhs) of the assignment operator.CommAssetQFT [only available to derived classes]
           *
           * @param rhs The CommAssetQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CommAssetQFT
           * @return A constant reference to this CommAssetQFT.
           */
         const CommAssetQFT& operator=(const CommAssetQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommAssetQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommAssetQFT_h

