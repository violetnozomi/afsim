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
#ifndef Uci__Type__ReadinessStatusQFT_h
#define Uci__Type__ReadinessStatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityTypeEnum_h)
# include "uci/type/CapabilityTypeEnum.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityTypeEnum_h)
# include "uci/type/SupportCapabilityTypeEnum.h"
#endif

#if !defined(Uci__Type__TimedZoneType_h)
# include "uci/type/TimedZoneType.h"
#endif

#if !defined(Uci__Type__TailNumberType_h)
# include "uci/type/TailNumberType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__CommSystemID_Type_h)
# include "uci/type/CommSystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ReadinessStatusQFT sequence accessor class */
      class ReadinessStatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ReadinessStatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. ReadinessStatusQFT
           *
           * @return This accessor's type constant, i.e. ReadinessStatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::readinessStatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ReadinessStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter by the System ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Filter by the Capability. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Filter on the Capability Type [Minimum occurrences: 0] [Maximum occurrences: 14] */
         typedef uci::base::BoundedList<uci::type::CapabilityTypeEnum, uci::type::accessorType::capabilityTypeEnum> CapabilityType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityID_Type, uci::type::accessorType::supportCapabilityID_Type> SupportCapabilityID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityTypeEnum, uci::type::accessorType::supportCapabilityTypeEnum> SupportCapabilityType;

         /** Filter by timed zone [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TimedZoneType, uci::type::accessorType::timedZoneType> TimedZone;

         /** Allows for specification of the tail number in the query [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TailNumberType, uci::type::accessorType::tailNumberType> TailNumber;

         /** Allows for specification of the call sign in the query [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> CallSign;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommSystemID_Type, uci::type::accessorType::commSystemID_Type> CommSystemID;

         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::ReadinessStatusQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::ReadinessStatusQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::ReadinessStatusQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::ReadinessStatusQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::ReadinessStatusQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::ReadinessStatusQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityType.
           *
           * @return The bounded list identified by CapabilityType.
           */
         virtual const uci::type::ReadinessStatusQFT::CapabilityType& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityType.
           *
           * @return The bounded list identified by CapabilityType.
           */
         virtual uci::type::ReadinessStatusQFT::CapabilityType& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityType(const uci::type::ReadinessStatusQFT::CapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual const uci::type::ReadinessStatusQFT::SupportCapabilityID& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual uci::type::ReadinessStatusQFT::SupportCapabilityID& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityID(const uci::type::ReadinessStatusQFT::SupportCapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityType.
           *
           * @return The bounded list identified by SupportCapabilityType.
           */
         virtual const uci::type::ReadinessStatusQFT::SupportCapabilityType& getSupportCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityType.
           *
           * @return The bounded list identified by SupportCapabilityType.
           */
         virtual uci::type::ReadinessStatusQFT::SupportCapabilityType& getSupportCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityType(const uci::type::ReadinessStatusQFT::SupportCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimedZone.
           *
           * @return The bounded list identified by TimedZone.
           */
         virtual const uci::type::ReadinessStatusQFT::TimedZone& getTimedZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimedZone.
           *
           * @return The bounded list identified by TimedZone.
           */
         virtual uci::type::ReadinessStatusQFT::TimedZone& getTimedZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TimedZone.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTimedZone(const uci::type::ReadinessStatusQFT::TimedZone& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TailNumber.
           *
           * @return The bounded list identified by TailNumber.
           */
         virtual const uci::type::ReadinessStatusQFT::TailNumber& getTailNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TailNumber.
           *
           * @return The bounded list identified by TailNumber.
           */
         virtual uci::type::ReadinessStatusQFT::TailNumber& getTailNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TailNumber.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTailNumber(const uci::type::ReadinessStatusQFT::TailNumber& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CallSign.
           *
           * @return The bounded list identified by CallSign.
           */
         virtual const uci::type::ReadinessStatusQFT::CallSign& getCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CallSign.
           *
           * @return The bounded list identified by CallSign.
           */
         virtual uci::type::ReadinessStatusQFT::CallSign& getCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CallSign.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCallSign(const uci::type::ReadinessStatusQFT::CallSign& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSystemID.
           *
           * @return The bounded list identified by CommSystemID.
           */
         virtual const uci::type::ReadinessStatusQFT::CommSystemID& getCommSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSystemID.
           *
           * @return The bounded list identified by CommSystemID.
           */
         virtual uci::type::ReadinessStatusQFT::CommSystemID& getCommSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSystemID(const uci::type::ReadinessStatusQFT::CommSystemID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ReadinessStatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ReadinessStatusQFT to copy from
           */
         ReadinessStatusQFT(const ReadinessStatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ReadinessStatusQFT to the contents of the ReadinessStatusQFT on
           * the right hand side (rhs) of the assignment operator.ReadinessStatusQFT [only available to derived classes]
           *
           * @param rhs The ReadinessStatusQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ReadinessStatusQFT
           * @return A constant reference to this ReadinessStatusQFT.
           */
         const ReadinessStatusQFT& operator=(const ReadinessStatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ReadinessStatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ReadinessStatusQFT_h

