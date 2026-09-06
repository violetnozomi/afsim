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
#ifndef Uci__Type__RF_BandType_h
#define Uci__Type__RF_BandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__RFD_GainSettingType_h)
# include "uci/type/RFD_GainSettingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_BandType sequence accessor class */
      class RF_BandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_BandType()
         { }

         /** Returns this accessor's type constant, i.e. RF_BandType
           *
           * @return This accessor's type constant, i.e. RF_BandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_BandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_BandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BandID.
           *
           * @return The value of the simple primitive data type identified by BandID.
           */
         virtual xs::UnsignedInt getBandID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BandID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBandID(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Bandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by Bandwidth.
           */
         virtual const uci::type::FrequencyRangeType& getBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Bandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by Bandwidth.
           */
         virtual uci::type::FrequencyRangeType& getBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Bandwidth to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Bandwidth
           */
         virtual void setBandwidth(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RFD_GainSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RFD_GainSettings.
           */
         virtual const uci::type::RFD_GainSettingType& getRFD_GainSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RFD_GainSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RFD_GainSettings.
           */
         virtual uci::type::RFD_GainSettingType& getRFD_GainSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RFD_GainSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RFD_GainSettings
           */
         virtual void setRFD_GainSettings(const uci::type::RFD_GainSettingType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_BandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_BandType to copy from
           */
         RF_BandType(const RF_BandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_BandType to the contents of the RF_BandType on the right hand
           * side (rhs) of the assignment operator.RF_BandType [only available to derived classes]
           *
           * @param rhs The RF_BandType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::RF_BandType
           * @return A constant reference to this RF_BandType.
           */
         const RF_BandType& operator=(const RF_BandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_BandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_BandType_h

