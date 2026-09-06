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
#ifndef Uci__Type__IO_PortStatusType_h
#define Uci__Type__IO_PortStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommUserIO_ID_Type_h)
# include "uci/type/CommUserIO_ID_Type.h"
#endif

#if !defined(Uci__Type__IO_PortTypeEnum_h)
# include "uci/type/IO_PortTypeEnum.h"
#endif

#if !defined(Uci__Type__IO_PortConfigurationStatusType_h)
# include "uci/type/IO_PortConfigurationStatusType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the IO_PortStatusType sequence accessor class */
      class IO_PortStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IO_PortStatusType()
         { }

         /** Returns this accessor's type constant, i.e. IO_PortStatusType
           *
           * @return This accessor's type constant, i.e. IO_PortStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::iO_PortStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IO_PortStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique IDs of the Activities currently assigned to this IO port. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivityID_Type, uci::type::accessorType::activityID_Type> ActivityID;

         /** Returns the accessor that provides access to the complex content that is identified by the PortID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PortID.
           */
         virtual const uci::type::CommUserIO_ID_Type& getPortID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PortID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PortID.
           */
         virtual uci::type::CommUserIO_ID_Type& getPortID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PortID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PortID
           */
         virtual void setPortID(const uci::type::CommUserIO_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PortType.
           *
           * @return The value of the enumeration identified by PortType.
           */
         virtual const uci::type::IO_PortTypeEnum& getPortType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PortType.
           *
           * @return The value of the enumeration identified by PortType.
           */
         virtual uci::type::IO_PortTypeEnum& getPortType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PortType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPortType(const uci::type::IO_PortTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PortType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPortType(uci::type::IO_PortTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Configuration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Configuration.
           */
         virtual const uci::type::IO_PortConfigurationStatusType& getConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Configuration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Configuration.
           */
         virtual uci::type::IO_PortConfigurationStatusType& getConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Configuration to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Configuration
           */
         virtual void setConfiguration(const uci::type::IO_PortConfigurationStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityID.
           *
           * @return The bounded list identified by ActivityID.
           */
         virtual const uci::type::IO_PortStatusType::ActivityID& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityID.
           *
           * @return The bounded list identified by ActivityID.
           */
         virtual uci::type::IO_PortStatusType::ActivityID& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityID(const uci::type::IO_PortStatusType::ActivityID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IO_PortStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IO_PortStatusType to copy from
           */
         IO_PortStatusType(const IO_PortStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IO_PortStatusType to the contents of the IO_PortStatusType on the
           * right hand side (rhs) of the assignment operator.IO_PortStatusType [only available to derived classes]
           *
           * @param rhs The IO_PortStatusType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::IO_PortStatusType
           * @return A constant reference to this IO_PortStatusType.
           */
         const IO_PortStatusType& operator=(const IO_PortStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IO_PortStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IO_PortStatusType_h

