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
#ifndef Uci__Type__CommUtilizationRequestMDT_h
#define Uci__Type__CommUtilizationRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommUtilizationRequestID_Type_h)
# include "uci/type/CommUtilizationRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__CommDataType_h)
# include "uci/type/CommDataType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommUtilizationRequestMDT sequence accessor class */
      class CommUtilizationRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommUtilizationRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommUtilizationRequestMDT
           *
           * @return This accessor's type constant, i.e. CommUtilizationRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commUtilizationRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommUtilizationRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommUtilizationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommUtilizationRequestID.
           */
         virtual const uci::type::CommUtilizationRequestID_Type& getCommUtilizationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommUtilizationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommUtilizationRequestID.
           */
         virtual uci::type::CommUtilizationRequestID_Type& getCommUtilizationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommUtilizationRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommUtilizationRequestID
           */
         virtual void setCommUtilizationRequestID(const uci::type::CommUtilizationRequestID_Type& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the CommUtilizationData.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommUtilizationData.
           */
         virtual const uci::type::CommDataType& getCommUtilizationData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommUtilizationData.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommUtilizationData.
           */
         virtual uci::type::CommDataType& getCommUtilizationData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommUtilizationData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommUtilizationData
           */
         virtual void setCommUtilizationData(const uci::type::CommDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeSpan.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeSpan.
           */
         virtual const uci::type::DateTimeRangeType& getTimeSpan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeSpan.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeSpan.
           */
         virtual uci::type::DateTimeRangeType& getTimeSpan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimeSpan to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimeSpan
           */
         virtual void setTimeSpan(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::Point2D_Type& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::Point2D_Type& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativePriority.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativePriority.
           */
         virtual const uci::type::RankingType& getRelativePriority() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativePriority.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativePriority.
           */
         virtual uci::type::RankingType& getRelativePriority()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RelativePriority to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RelativePriority
           */
         virtual void setRelativePriority(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommUtilizationRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommUtilizationRequestMDT to copy from
           */
         CommUtilizationRequestMDT(const CommUtilizationRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommUtilizationRequestMDT to the contents of the
           * CommUtilizationRequestMDT on the right hand side (rhs) of the assignment operator.CommUtilizationRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The CommUtilizationRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommUtilizationRequestMDT
           * @return A constant reference to this CommUtilizationRequestMDT.
           */
         const CommUtilizationRequestMDT& operator=(const CommUtilizationRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommUtilizationRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommUtilizationRequestMDT_h

