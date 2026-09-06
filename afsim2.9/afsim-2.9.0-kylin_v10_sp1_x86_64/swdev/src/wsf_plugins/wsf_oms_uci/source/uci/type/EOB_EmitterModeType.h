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
#ifndef Uci__Type__EOB_EmitterModeType_h
#define Uci__Type__EOB_EmitterModeType_h 1

#if !defined(Uci__Type__EOB_SignalType_h)
# include "uci/type/EOB_SignalType.h"
#endif

#if !defined(Uci__Type__EOB_EmitterModeID_Type_h)
# include "uci/type/EOB_EmitterModeID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DataRateDataType_h)
# include "uci/type/DataRateDataType.h"
#endif

#if !defined(Uci__Type__ModRateDataType_h)
# include "uci/type/ModRateDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Emitter mode parametrics. Modes/Parametrics are loaded provided in part by the Global Command and Control System
        * Integrated Imagery and Intelligence (GCCS-I3) with more detail found in the Application Program Interface Reference
        * Manual (APIRM) for MIDB Data Access Layer (MDAL)
        */
      class EOB_EmitterModeType : public virtual uci::type::EOB_SignalType {
      public:

         /** The destructor */
         virtual ~EOB_EmitterModeType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_EmitterModeType
           *
           * @return This accessor's type constant, i.e. EOB_EmitterModeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_EmitterModeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_EmitterModeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterModeID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterModeID.
           */
         virtual const uci::type::EOB_EmitterModeID_Type& getEmitterModeID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterModeID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterModeID.
           */
         virtual uci::type::EOB_EmitterModeID_Type& getEmitterModeID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterModeID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterModeID
           */
         virtual void setEmitterModeID(const uci::type::EOB_EmitterModeID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InterceptCount.
           *
           * @return The value of the simple primitive data type identified by InterceptCount.
           */
         virtual xs::UnsignedInt getInterceptCount() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InterceptCount.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInterceptCount(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InterceptCount exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InterceptCount is emabled or not
           */
         virtual bool hasInterceptCount() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InterceptCount
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableInterceptCount(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InterceptCount */
         virtual void clearInterceptCount()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataRate.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataRate.
           */
         virtual const uci::type::DataRateDataType& getDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataRate.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataRate.
           */
         virtual uci::type::DataRateDataType& getDataRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataRate to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataRate
           */
         virtual void setDataRate(const uci::type::DataRateDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataRate is emabled or not
           */
         virtual bool hasDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataRate
           *
           * @param type = uci::type::accessorType::dataRateDataType This Accessor's accessor type
           */
         virtual void enableDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataRate */
         virtual void clearDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModulationRate.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModulationRate.
           */
         virtual const uci::type::ModRateDataType& getModulationRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModulationRate.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModulationRate.
           */
         virtual uci::type::ModRateDataType& getModulationRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ModulationRate to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ModulationRate
           */
         virtual void setModulationRate(const uci::type::ModRateDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ModulationRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ModulationRate is emabled or not
           */
         virtual bool hasModulationRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ModulationRate
           *
           * @param type = uci::type::accessorType::modRateDataType This Accessor's accessor type
           */
         virtual void enableModulationRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::modRateDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ModulationRate */
         virtual void clearModulationRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_EmitterModeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_EmitterModeType to copy from
           */
         EOB_EmitterModeType(const EOB_EmitterModeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_EmitterModeType to the contents of the EOB_EmitterModeType on
           * the right hand side (rhs) of the assignment operator.EOB_EmitterModeType [only available to derived classes]
           *
           * @param rhs The EOB_EmitterModeType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EOB_EmitterModeType
           * @return A constant reference to this EOB_EmitterModeType.
           */
         const EOB_EmitterModeType& operator=(const EOB_EmitterModeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_EmitterModeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_EmitterModeType_h

