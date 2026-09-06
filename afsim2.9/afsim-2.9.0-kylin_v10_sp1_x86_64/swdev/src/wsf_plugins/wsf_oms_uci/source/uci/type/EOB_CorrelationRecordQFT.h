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
#ifndef Uci__Type__EOB_CorrelationRecordQFT_h
#define Uci__Type__EOB_CorrelationRecordQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EOB_CorrelationRecordID_Type_h)
# include "uci/type/EOB_CorrelationRecordID_Type.h"
#endif

#if !defined(Uci__Type__EOB_CorrelationTypeEnum_h)
# include "uci/type/EOB_CorrelationTypeEnum.h"
#endif

#if !defined(Uci__Type__EOB_CorrelatedToType_h)
# include "uci/type/EOB_CorrelatedToType.h"
#endif

#if !defined(Uci__Type__SignalParametricQueryType_h)
# include "uci/type/SignalParametricQueryType.h"
#endif

#if !defined(Uci__Type__SignalIdentityQueryType_h)
# include "uci/type/SignalIdentityQueryType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Type__FMOP_Enum_h)
# include "uci/type/FMOP_Enum.h"
#endif

#if !defined(Uci__Type__PMOP_Enum_h)
# include "uci/type/PMOP_Enum.h"
#endif

#if !defined(Uci__Type__ScanTypeEnum_h)
# include "uci/type/ScanTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This QFT is intended to support analyzing data for which was previously processed and potentially correlated to an
        * EOB record.
        */
      class EOB_CorrelationRecordQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~EOB_CorrelationRecordQFT()
         { }

         /** Returns this accessor's type constant, i.e. EOB_CorrelationRecordQFT
           *
           * @return This accessor's type constant, i.e. EOB_CorrelationRecordQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_CorrelationRecordQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_CorrelationRecordQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::EOB_CorrelationRecordID_Type, uci::type::accessorType::eOB_CorrelationRecordID_Type> EOB_CorrelationRecordID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::EOB_CorrelationTypeEnum, uci::type::accessorType::eOB_CorrelationTypeEnum> CorrelationType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::EOB_CorrelatedToType, uci::type::accessorType::eOB_CorrelatedToType> CorrelatedTo;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> SignalLastHeardTimestamp;

         /** [Minimum occurrences: 0] [Maximum occurrences: 59] */
         typedef uci::base::BoundedList<uci::type::ModulationEnum, uci::type::accessorType::modulationEnum> MOP_Type;

         /** [Minimum occurrences: 0] [Maximum occurrences: 3] */
         typedef uci::base::BoundedList<uci::type::FMOP_Enum, uci::type::accessorType::fMOP_Enum> FMOP_Type;

         /** [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::PMOP_Enum, uci::type::accessorType::pMOP_Enum> PMOP_Type;

         /** [Minimum occurrences: 0] [Maximum occurrences: 27] */
         typedef uci::base::BoundedList<uci::type::ScanTypeEnum, uci::type::accessorType::scanTypeEnum> ScanType;

         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual const uci::type::OpZoneID_Type& getOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual uci::type::OpZoneID_Type& getOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpZoneID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpZoneID
           */
         virtual void setOpZoneID(const uci::type::OpZoneID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpZoneID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpZoneID is emabled or not
           */
         virtual bool hasOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpZoneID
           *
           * @param type = uci::type::accessorType::opZoneID_Type This Accessor's accessor type
           */
         virtual void enableOpZoneID(uci::base::accessorType::AccessorType type = uci::type::accessorType::opZoneID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpZoneID */
         virtual void clearOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EOB_CorrelationRecordID.
           *
           * @return The bounded list identified by EOB_CorrelationRecordID.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::EOB_CorrelationRecordID& getEOB_CorrelationRecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EOB_CorrelationRecordID.
           *
           * @return The bounded list identified by EOB_CorrelationRecordID.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::EOB_CorrelationRecordID& getEOB_CorrelationRecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EOB_CorrelationRecordID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEOB_CorrelationRecordID(const uci::type::EOB_CorrelationRecordQFT::EOB_CorrelationRecordID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelationType.
           *
           * @return The bounded list identified by CorrelationType.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::CorrelationType& getCorrelationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelationType.
           *
           * @return The bounded list identified by CorrelationType.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::CorrelationType& getCorrelationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CorrelationType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCorrelationType(const uci::type::EOB_CorrelationRecordQFT::CorrelationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelatedTo.
           *
           * @return The bounded list identified by CorrelatedTo.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::CorrelatedTo& getCorrelatedTo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelatedTo.
           *
           * @return The bounded list identified by CorrelatedTo.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::CorrelatedTo& getCorrelatedTo()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CorrelatedTo.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCorrelatedTo(const uci::type::EOB_CorrelationRecordQFT::CorrelatedTo& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalParametrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalParametrics.
           */
         virtual const uci::type::SignalParametricQueryType& getSignalParametrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalParametrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalParametrics.
           */
         virtual uci::type::SignalParametricQueryType& getSignalParametrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalParametrics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalParametrics
           */
         virtual void setSignalParametrics(const uci::type::SignalParametricQueryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalParametrics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalParametrics is emabled or not
           */
         virtual bool hasSignalParametrics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalParametrics
           *
           * @param type = uci::type::accessorType::signalParametricQueryType This Accessor's accessor type
           */
         virtual void enableSignalParametrics(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalParametricQueryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalParametrics */
         virtual void clearSignalParametrics()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalIdentities.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalIdentities.
           */
         virtual const uci::type::SignalIdentityQueryType& getSignalIdentities() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalIdentities.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalIdentities.
           */
         virtual uci::type::SignalIdentityQueryType& getSignalIdentities()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalIdentities to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalIdentities
           */
         virtual void setSignalIdentities(const uci::type::SignalIdentityQueryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalIdentities exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalIdentities is emabled or not
           */
         virtual bool hasSignalIdentities() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalIdentities
           *
           * @param type = uci::type::accessorType::signalIdentityQueryType This Accessor's accessor type
           */
         virtual void enableSignalIdentities(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalIdentityQueryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalIdentities */
         virtual void clearSignalIdentities()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SignalLastHeardTimestamp.
           *
           * @return The bounded list identified by SignalLastHeardTimestamp.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::SignalLastHeardTimestamp& getSignalLastHeardTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SignalLastHeardTimestamp.
           *
           * @return The bounded list identified by SignalLastHeardTimestamp.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::SignalLastHeardTimestamp& getSignalLastHeardTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SignalLastHeardTimestamp.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSignalLastHeardTimestamp(const uci::type::EOB_CorrelationRecordQFT::SignalLastHeardTimestamp& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MOP_Type.
           *
           * @return The bounded list identified by MOP_Type.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::MOP_Type& getMOP_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MOP_Type.
           *
           * @return The bounded list identified by MOP_Type.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::MOP_Type& getMOP_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MOP_Type.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMOP_Type(const uci::type::EOB_CorrelationRecordQFT::MOP_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FMOP_Type.
           *
           * @return The bounded list identified by FMOP_Type.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::FMOP_Type& getFMOP_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FMOP_Type.
           *
           * @return The bounded list identified by FMOP_Type.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::FMOP_Type& getFMOP_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FMOP_Type.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFMOP_Type(const uci::type::EOB_CorrelationRecordQFT::FMOP_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PMOP_Type.
           *
           * @return The bounded list identified by PMOP_Type.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::PMOP_Type& getPMOP_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PMOP_Type.
           *
           * @return The bounded list identified by PMOP_Type.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::PMOP_Type& getPMOP_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PMOP_Type.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPMOP_Type(const uci::type::EOB_CorrelationRecordQFT::PMOP_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanType.
           *
           * @return The bounded list identified by ScanType.
           */
         virtual const uci::type::EOB_CorrelationRecordQFT::ScanType& getScanType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanType.
           *
           * @return The bounded list identified by ScanType.
           */
         virtual uci::type::EOB_CorrelationRecordQFT::ScanType& getScanType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScanType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScanType(const uci::type::EOB_CorrelationRecordQFT::ScanType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_CorrelationRecordQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_CorrelationRecordQFT to copy from
           */
         EOB_CorrelationRecordQFT(const EOB_CorrelationRecordQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_CorrelationRecordQFT to the contents of the
           * EOB_CorrelationRecordQFT on the right hand side (rhs) of the assignment operator.EOB_CorrelationRecordQFT [only
           * available to derived classes]
           *
           * @param rhs The EOB_CorrelationRecordQFT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EOB_CorrelationRecordQFT
           * @return A constant reference to this EOB_CorrelationRecordQFT.
           */
         const EOB_CorrelationRecordQFT& operator=(const EOB_CorrelationRecordQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_CorrelationRecordQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_CorrelationRecordQFT_h

