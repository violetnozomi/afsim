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
#ifndef Uci__Type__ChannelDescriptionType_h
#define Uci__Type__ChannelDescriptionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__GainRangeType_h)
# include "uci/type/GainRangeType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Identifies the parameters of the RF channel. This information is typically used to decide if the channel has
        * sufficient resources to support an application.
        */
      class ChannelDescriptionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ChannelDescriptionType()
         { }

         /** Returns this accessor's type constant, i.e. ChannelDescriptionType
           *
           * @return This accessor's type constant, i.e. ChannelDescriptionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::channelDescriptionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ChannelDescriptionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element identifies the RF bands that are supported by this RF channel, using integer indices [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::base::IntAccessor, uci::base::accessorType::intAccessor> BandsSupported;

         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelFreq.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelFreq.
           */
         virtual const uci::type::FrequencyRangeType& getChannelFreq() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelFreq.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelFreq.
           */
         virtual uci::type::FrequencyRangeType& getChannelFreq()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChannelFreq to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChannelFreq
           */
         virtual void setChannelFreq(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Gain.
           *
           * @return The acecssor that provides access to the complex content that is identified by Gain.
           */
         virtual const uci::type::GainRangeType& getGain() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Gain.
           *
           * @return The acecssor that provides access to the complex content that is identified by Gain.
           */
         virtual uci::type::GainRangeType& getGain()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Gain to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Gain
           */
         virtual void setGain(const uci::type::GainRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Gain exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Gain is emabled or not
           */
         virtual bool hasGain() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Gain
           *
           * @param type = uci::type::accessorType::gainRangeType This Accessor's accessor type
           */
         virtual void enableGain(uci::base::accessorType::AccessorType type = uci::type::accessorType::gainRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Gain */
         virtual void clearGain()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GainStepSize.
           *
           * @return The value of the simple primitive data type identified by GainStepSize.
           */
         virtual uci::type::DecibelTypeValue getGainStepSize() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GainStepSize.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGainStepSize(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GainStepSize exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GainStepSize is emabled or not
           */
         virtual bool hasGainStepSize() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GainStepSize
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableGainStepSize(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GainStepSize */
         virtual void clearGainStepSize()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BandsSupported.
           *
           * @return The bounded list identified by BandsSupported.
           */
         virtual const uci::type::ChannelDescriptionType::BandsSupported& getBandsSupported() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BandsSupported.
           *
           * @return The bounded list identified by BandsSupported.
           */
         virtual uci::type::ChannelDescriptionType::BandsSupported& getBandsSupported()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BandsSupported.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBandsSupported(const uci::type::ChannelDescriptionType::BandsSupported& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ChannelDescriptionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ChannelDescriptionType to copy from
           */
         ChannelDescriptionType(const ChannelDescriptionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ChannelDescriptionType to the contents of the
           * ChannelDescriptionType on the right hand side (rhs) of the assignment operator.ChannelDescriptionType [only available
           * to derived classes]
           *
           * @param rhs The ChannelDescriptionType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ChannelDescriptionType
           * @return A constant reference to this ChannelDescriptionType.
           */
         const ChannelDescriptionType& operator=(const ChannelDescriptionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ChannelDescriptionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ChannelDescriptionType_h

