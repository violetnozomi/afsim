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
#ifndef Uci__Type__CommLink16CommWaveformActivityType_h
#define Uci__Type__CommLink16CommWaveformActivityType_h 1

#if !defined(Uci__Type__CommWaveformActivityPET_h)
# include "uci/type/CommWaveformActivityPET.h"
#endif

#if !defined(Uci__Type__CommLink16ControlType_h)
# include "uci/type/CommLink16ControlType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformActivityPET for communication
        * Link16 parameters.
        */
      class CommLink16CommWaveformActivityType : public virtual uci::type::CommWaveformActivityPET {
      public:

         /** The destructor */
         virtual ~CommLink16CommWaveformActivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16CommWaveformActivityType
           *
           * @return This accessor's type constant, i.e. CommLink16CommWaveformActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16CommWaveformActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16CommWaveformActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 511] */
         typedef uci::base::IntAccessor PPLI_MsgGenPPLI_PG;
         //  The type of primitive data accessed by PPLI_MsgGenPPLI_PG
         typedef xs::Int PPLI_MsgGenPPLI_PGValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 15] */
         typedef uci::base::IntAccessor PPLI_MsgGenTxRate;
         //  The type of primitive data accessed by PPLI_MsgGenTxRate
         typedef xs::Int PPLI_MsgGenTxRateValue;

         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual const uci::type::CommLink16ControlType& getSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual uci::type::CommLink16ControlType& getSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SettingsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SettingsStatus
           */
         virtual void setSettingsStatus(const uci::type::CommLink16ControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SettingsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SettingsStatus is emabled or not
           */
         virtual bool hasSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SettingsStatus
           *
           * @param type = uci::type::accessorType::commLink16ControlType This Accessor's accessor type
           */
         virtual void enableSettingsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16ControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SettingsStatus */
         virtual void clearSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PPLI_MsgGenPPLI_PG.
           *
           * @return The value of the simple primitive data type identified by PPLI_MsgGenPPLI_PG.
           */
         virtual uci::type::CommLink16CommWaveformActivityType::PPLI_MsgGenPPLI_PGValue getPPLI_MsgGenPPLI_PG() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PPLI_MsgGenPPLI_PG.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPPLI_MsgGenPPLI_PG(uci::type::CommLink16CommWaveformActivityType::PPLI_MsgGenPPLI_PGValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PPLI_MsgGenPPLI_PG exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PPLI_MsgGenPPLI_PG is emabled or not
           */
         virtual bool hasPPLI_MsgGenPPLI_PG() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PPLI_MsgGenPPLI_PG
           *
           * @param type = uci::type::commLink16CommWaveformActivityType::accessorType::pPLI_MsgGenPPLI_PG This Accessor's
           *      accessor type
           */
         virtual void enablePPLI_MsgGenPPLI_PG(uci::base::accessorType::AccessorType type = uci::type::commLink16CommWaveformActivityType::accessorType::pPLI_MsgGenPPLI_PG)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PPLI_MsgGenPPLI_PG */
         virtual void clearPPLI_MsgGenPPLI_PG()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PPLI_MsgGenTxRate.
           *
           * @return The value of the simple primitive data type identified by PPLI_MsgGenTxRate.
           */
         virtual uci::type::CommLink16CommWaveformActivityType::PPLI_MsgGenTxRateValue getPPLI_MsgGenTxRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PPLI_MsgGenTxRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPPLI_MsgGenTxRate(uci::type::CommLink16CommWaveformActivityType::PPLI_MsgGenTxRateValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PPLI_MsgGenTxRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PPLI_MsgGenTxRate is emabled or not
           */
         virtual bool hasPPLI_MsgGenTxRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PPLI_MsgGenTxRate
           *
           * @param type = uci::type::commLink16CommWaveformActivityType::accessorType::pPLI_MsgGenTxRate This Accessor's accessor
           *      type
           */
         virtual void enablePPLI_MsgGenTxRate(uci::base::accessorType::AccessorType type = uci::type::commLink16CommWaveformActivityType::accessorType::pPLI_MsgGenTxRate)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PPLI_MsgGenTxRate */
         virtual void clearPPLI_MsgGenTxRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16CommWaveformActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16CommWaveformActivityType to copy from
           */
         CommLink16CommWaveformActivityType(const CommLink16CommWaveformActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16CommWaveformActivityType to the contents of the
           * CommLink16CommWaveformActivityType on the right hand side (rhs) of the assignment
           * operator.CommLink16CommWaveformActivityType [only available to derived classes]
           *
           * @param rhs The CommLink16CommWaveformActivityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommLink16CommWaveformActivityType
           * @return A constant reference to this CommLink16CommWaveformActivityType.
           */
         const CommLink16CommWaveformActivityType& operator=(const CommLink16CommWaveformActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16CommWaveformActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16CommWaveformActivityType_h

