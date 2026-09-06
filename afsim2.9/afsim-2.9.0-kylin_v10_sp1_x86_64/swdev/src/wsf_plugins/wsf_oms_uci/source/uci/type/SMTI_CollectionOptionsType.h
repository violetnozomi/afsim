// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SMTI_CollectionOptionsType_h
#define Uci__Type__SMTI_CollectionOptionsType_h 1

#if !defined(Uci__Type__RadarCollectionOptionsType_h)
# include "uci/type/RadarCollectionOptionsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SMTI_ResolutionType_h)
# include "uci/type/SMTI_ResolutionType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__HRR_OptionsType_h)
# include "uci/type/HRR_OptionsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SMTI_CollectionOptionsType sequence accessor class */
      class SMTI_CollectionOptionsType : public virtual uci::type::RadarCollectionOptionsType {
      public:

         /** The destructor */
         virtual ~SMTI_CollectionOptionsType()
         { }

         /** Returns this accessor's type constant, i.e. SMTI_CollectionOptionsType
           *
           * @return This accessor's type constant, i.e. SMTI_CollectionOptionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sMTI_CollectionOptionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SMTI_CollectionOptionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Tracking.
           *
           * @return The value of the simple primitive data type identified by Tracking.
           */
         virtual xs::Boolean getTracking() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Tracking.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTracking(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Tracking exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Tracking is emabled or not
           */
         virtual bool hasTracking() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Tracking
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableTracking(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Tracking */
         virtual void clearTracking()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FTI.
           *
           * @return The value of the simple primitive data type identified by FTI.
           */
         virtual xs::Boolean getFTI() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FTI.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFTI(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FTI exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FTI is emabled or not
           */
         virtual bool hasFTI() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FTI
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableFTI(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FTI */
         virtual void clearFTI()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Resolution.
           *
           * @return The acecssor that provides access to the complex content that is identified by Resolution.
           */
         virtual const uci::type::SMTI_ResolutionType& getResolution() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Resolution.
           *
           * @return The acecssor that provides access to the complex content that is identified by Resolution.
           */
         virtual uci::type::SMTI_ResolutionType& getResolution()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Resolution to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Resolution
           */
         virtual void setResolution(const uci::type::SMTI_ResolutionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Resolution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Resolution is emabled or not
           */
         virtual bool hasResolution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Resolution
           *
           * @param type = uci::type::accessorType::sMTI_ResolutionType This Accessor's accessor type
           */
         virtual void enableResolution(uci::base::accessorType::AccessorType type = uci::type::accessorType::sMTI_ResolutionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Resolution */
         virtual void clearResolution()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SignalToNoiseRatio.
           *
           * @return The value of the simple primitive data type identified by SignalToNoiseRatio.
           */
         virtual uci::type::DecibelTypeValue getSignalToNoiseRatio() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SignalToNoiseRatio.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSignalToNoiseRatio(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalToNoiseRatio exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalToNoiseRatio is emabled or not
           */
         virtual bool hasSignalToNoiseRatio() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalToNoiseRatio
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableSignalToNoiseRatio(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalToNoiseRatio */
         virtual void clearSignalToNoiseRatio()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the HRR_Options.
           *
           * @return The acecssor that provides access to the complex content that is identified by HRR_Options.
           */
         virtual const uci::type::HRR_OptionsType& getHRR_Options() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the HRR_Options.
           *
           * @return The acecssor that provides access to the complex content that is identified by HRR_Options.
           */
         virtual uci::type::HRR_OptionsType& getHRR_Options()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the HRR_Options to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by HRR_Options
           */
         virtual void setHRR_Options(const uci::type::HRR_OptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HRR_Options exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HRR_Options is emabled or not
           */
         virtual bool hasHRR_Options() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HRR_Options
           *
           * @param type = uci::type::accessorType::hRR_OptionsType This Accessor's accessor type
           */
         virtual void enableHRR_Options(uci::base::accessorType::AccessorType type = uci::type::accessorType::hRR_OptionsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HRR_Options */
         virtual void clearHRR_Options()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SMTI_CollectionOptionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SMTI_CollectionOptionsType to copy from
           */
         SMTI_CollectionOptionsType(const SMTI_CollectionOptionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SMTI_CollectionOptionsType to the contents of the
           * SMTI_CollectionOptionsType on the right hand side (rhs) of the assignment operator.SMTI_CollectionOptionsType [only
           * available to derived classes]
           *
           * @param rhs The SMTI_CollectionOptionsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SMTI_CollectionOptionsType
           * @return A constant reference to this SMTI_CollectionOptionsType.
           */
         const SMTI_CollectionOptionsType& operator=(const SMTI_CollectionOptionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SMTI_CollectionOptionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SMTI_CollectionOptionsType_h

