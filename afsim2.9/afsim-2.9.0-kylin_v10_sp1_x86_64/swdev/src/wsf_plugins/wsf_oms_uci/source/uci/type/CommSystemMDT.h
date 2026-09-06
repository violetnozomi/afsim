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
#ifndef Uci__Type__CommSystemMDT_h
#define Uci__Type__CommSystemMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommSystemID_Type_h)
# include "uci/type/CommSystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommType_h)
# include "uci/type/CommType.h"
#endif

#if !defined(Uci__Type__CommSchedulingScopeEnum_h)
# include "uci/type/CommSchedulingScopeEnum.h"
#endif

#if !defined(Uci__Type__CommAssetID_Type_h)
# include "uci/type/CommAssetID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommSystemMDT sequence accessor class */
      class CommSystemMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommSystemMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommSystemMDT
           *
           * @return This accessor's type constant, i.e. CommSystemMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commSystemMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommSystemMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a type of communication ssupported by the CommSystem. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommType, uci::type::accessorType::commType> CommType;

         /** Indicates a particular CommAsset that provides the actual communications for use that is part of this communication
           * system. If no assets are defined, the scheduling scope should indicate SYSTEM, NONE, or MANUAL. This would also
           * indicate that this system is scheduled as a whole, is always available, or is coordinated outside the scope of UCI
           * and cannot be represented as individual assets. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommAssetID_Type, uci::type::accessorType::commAssetID_Type> CommAssetID;

         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemID.
           */
         virtual const uci::type::CommSystemID_Type& getCommSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemID.
           */
         virtual uci::type::CommSystemID_Type& getCommSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSystemID
           */
         virtual void setCommSystemID(const uci::type::CommSystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommType.
           *
           * @return The bounded list identified by CommType.
           */
         virtual const uci::type::CommSystemMDT::CommType& getCommType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommType.
           *
           * @return The bounded list identified by CommType.
           */
         virtual uci::type::CommSystemMDT::CommType& getCommType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommType(const uci::type::CommSystemMDT::CommType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SchedulingScope.
           *
           * @return The value of the enumeration identified by SchedulingScope.
           */
         virtual const uci::type::CommSchedulingScopeEnum& getSchedulingScope() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SchedulingScope.
           *
           * @return The value of the enumeration identified by SchedulingScope.
           */
         virtual uci::type::CommSchedulingScopeEnum& getSchedulingScope()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SchedulingScope.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSchedulingScope(const uci::type::CommSchedulingScopeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SchedulingScope.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSchedulingScope(uci::type::CommSchedulingScopeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommAssetID.
           *
           * @return The bounded list identified by CommAssetID.
           */
         virtual const uci::type::CommSystemMDT::CommAssetID& getCommAssetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommAssetID.
           *
           * @return The bounded list identified by CommAssetID.
           */
         virtual uci::type::CommSystemMDT::CommAssetID& getCommAssetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommAssetID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommAssetID(const uci::type::CommSystemMDT::CommAssetID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommSystemMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommSystemMDT to copy from
           */
         CommSystemMDT(const CommSystemMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommSystemMDT to the contents of the CommSystemMDT on the right
           * hand side (rhs) of the assignment operator.CommSystemMDT [only available to derived classes]
           *
           * @param rhs The CommSystemMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CommSystemMDT
           * @return A constant reference to this CommSystemMDT.
           */
         const CommSystemMDT& operator=(const CommSystemMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommSystemMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommSystemMDT_h

