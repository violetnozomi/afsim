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
#ifndef Uci__Type__CommMADL_CommWaveformActivityType_h
#define Uci__Type__CommMADL_CommWaveformActivityType_h 1

#if !defined(Uci__Type__CommWaveformActivityPET_h)
# include "uci/type/CommWaveformActivityPET.h"
#endif

#if !defined(Uci__Type__CommMADL_ControlType_h)
# include "uci/type/CommMADL_ControlType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_AircraftTypeEnum_h)
# include "uci/type/CommMADL_AircraftTypeEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_LinkStatusListType_h)
# include "uci/type/CommMADL_LinkStatusListType.h"
#endif

#if !defined(Uci__Type__CommMADL_NetChangeStatusEnum_h)
# include "uci/type/CommMADL_NetChangeStatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformActivityPET for communication
        * MADL parameters.
        */
      class CommMADL_CommWaveformActivityType : public virtual uci::type::CommWaveformActivityPET {
      public:

         /** The destructor */
         virtual ~CommMADL_CommWaveformActivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_CommWaveformActivityType
           *
           * @return This accessor's type constant, i.e. CommMADL_CommWaveformActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_CommWaveformActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_CommWaveformActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Uniquely identifies the ownship platform within the MADL world. Read from MDF associated with selected profile.;
           * [Minimum inclusive: 0] [Maximum inclusive: 65535]
           */
         typedef uci::base::UnsignedIntAccessor MADL_UniqueID;
         //  The type of primitive data accessed by MADL_UniqueID
         typedef xs::UnsignedInt MADL_UniqueIDValue;

         /** Identifies the day of year used.; [Minimum inclusive: 1] [Maximum inclusive: 366] */
         typedef uci::base::UnsignedIntAccessor MADL_DayOfYearApplied;
         //  The type of primitive data accessed by MADL_DayOfYearApplied
         typedef xs::UnsignedInt MADL_DayOfYearAppliedValue;

         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual const uci::type::CommMADL_ControlType& getSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual uci::type::CommMADL_ControlType& getSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SettingsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SettingsStatus
           */
         virtual void setSettingsStatus(const uci::type::CommMADL_ControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MADL_UniqueID.
           *
           * @return The value of the simple primitive data type identified by MADL_UniqueID.
           */
         virtual uci::type::CommMADL_CommWaveformActivityType::MADL_UniqueIDValue getMADL_UniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MADL_UniqueID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMADL_UniqueID(uci::type::CommMADL_CommWaveformActivityType::MADL_UniqueIDValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AircraftType.
           *
           * @return The value of the enumeration identified by AircraftType.
           */
         virtual const uci::type::CommMADL_AircraftTypeEnum& getAircraftType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AircraftType.
           *
           * @return The value of the enumeration identified by AircraftType.
           */
         virtual uci::type::CommMADL_AircraftTypeEnum& getAircraftType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AircraftType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAircraftType(const uci::type::CommMADL_AircraftTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AircraftType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAircraftType(uci::type::CommMADL_AircraftTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MADL_DayOfYearApplied.
           *
           * @return The value of the simple primitive data type identified by MADL_DayOfYearApplied.
           */
         virtual uci::type::CommMADL_CommWaveformActivityType::MADL_DayOfYearAppliedValue getMADL_DayOfYearApplied() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MADL_DayOfYearApplied.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMADL_DayOfYearApplied(uci::type::CommMADL_CommWaveformActivityType::MADL_DayOfYearAppliedValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceCapable.
           *
           * @return The value of the simple primitive data type identified by VoiceCapable.
           */
         virtual xs::Boolean getVoiceCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkStatus.
           */
         virtual const uci::type::CommMADL_LinkStatusListType& getLinkStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkStatus.
           */
         virtual uci::type::CommMADL_LinkStatusListType& getLinkStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LinkStatus to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LinkStatus
           */
         virtual void setLinkStatus(const uci::type::CommMADL_LinkStatusListType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NetChangeStatus.
           *
           * @return The value of the enumeration identified by NetChangeStatus.
           */
         virtual const uci::type::CommMADL_NetChangeStatusEnum& getNetChangeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NetChangeStatus.
           *
           * @return The value of the enumeration identified by NetChangeStatus.
           */
         virtual uci::type::CommMADL_NetChangeStatusEnum& getNetChangeStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NetChangeStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNetChangeStatus(const uci::type::CommMADL_NetChangeStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NetChangeStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNetChangeStatus(uci::type::CommMADL_NetChangeStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield NetChangeStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasNetChangeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getNetChangeStatus will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commMADL_NetChangeStatusEnum This Accessor's accessor type
           */
         virtual void enableNetChangeStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_NetChangeStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearNetChangeStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_CommWaveformActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_CommWaveformActivityType to copy from
           */
         CommMADL_CommWaveformActivityType(const CommMADL_CommWaveformActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_CommWaveformActivityType to the contents of the
           * CommMADL_CommWaveformActivityType on the right hand side (rhs) of the assignment
           * operator.CommMADL_CommWaveformActivityType [only available to derived classes]
           *
           * @param rhs The CommMADL_CommWaveformActivityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommMADL_CommWaveformActivityType
           * @return A constant reference to this CommMADL_CommWaveformActivityType.
           */
         const CommMADL_CommWaveformActivityType& operator=(const CommMADL_CommWaveformActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_CommWaveformActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_CommWaveformActivityType_h

