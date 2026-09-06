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
#ifndef Uci__Type__COMINT_CapabilityOptionsType_h
#define Uci__Type__COMINT_CapabilityOptionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MIME_Type_h)
# include "uci/type/MIME_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_CapabilityOptionsType sequence accessor class */
      class COMINT_CapabilityOptionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_CapabilityOptionsType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_CapabilityOptionsType
           *
           * @return This accessor's type constant, i.e. COMINT_CapabilityOptionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_CapabilityOptionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_CapabilityOptionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This indicates what formats are used by the subsystem when outputting audio data. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MIME_Type, uci::type::accessorType::mIME_Type> AudioFormats;

         /** This indicates what data compression types are used by the subsystem when outputting audio data. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> CompressionType;

         /** Returns the value of the SimplePrimitive data type that is identified by the InterruptOtherActivities.
           *
           * @return The value of the simple primitive data type identified by InterruptOtherActivities.
           */
         virtual xs::Boolean getInterruptOtherActivities() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InterruptOtherActivities.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInterruptOtherActivities(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfChannels.
           *
           * @return The value of the simple primitive data type identified by NumberOfChannels.
           */
         virtual xs::Int getNumberOfChannels() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfChannels.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfChannels(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumberOfChannels exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumberOfChannels is emabled or not
           */
         virtual bool hasNumberOfChannels() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumberOfChannels
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableNumberOfChannels(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumberOfChannels */
         virtual void clearNumberOfChannels()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AGC_Supported.
           *
           * @return The value of the simple primitive data type identified by AGC_Supported.
           */
         virtual xs::Boolean getAGC_Supported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AGC_Supported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAGC_Supported(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AGC_Supported exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AGC_Supported is emabled or not
           */
         virtual bool hasAGC_Supported() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AGC_Supported
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAGC_Supported(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AGC_Supported */
         virtual void clearAGC_Supported()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ArchiveAvailable.
           *
           * @return The value of the simple primitive data type identified by ArchiveAvailable.
           */
         virtual xs::Boolean getArchiveAvailable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ArchiveAvailable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setArchiveAvailable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ArchiveAvailable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ArchiveAvailable is emabled or not
           */
         virtual bool hasArchiveAvailable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ArchiveAvailable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableArchiveAvailable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ArchiveAvailable */
         virtual void clearArchiveAvailable()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityDependencyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityDependencyID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityDependencyID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityDependencyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityDependencyID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityDependencyID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityDependencyID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityDependencyID
           */
         virtual void setCapabilityDependencyID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityDependencyID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityDependencyID is emabled or not
           */
         virtual bool hasCapabilityDependencyID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityDependencyID
           *
           * @param type = uci::type::accessorType::capabilityID_Type This Accessor's accessor type
           */
         virtual void enableCapabilityDependencyID(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityDependencyID */
         virtual void clearCapabilityDependencyID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ScanSupport.
           *
           * @return The value of the simple primitive data type identified by ScanSupport.
           */
         virtual xs::Boolean getScanSupport() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ScanSupport.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setScanSupport(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ScanSupport exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ScanSupport is emabled or not
           */
         virtual bool hasScanSupport() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ScanSupport
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableScanSupport(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ScanSupport */
         virtual void clearScanSupport()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedModes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedModes.
           */
         virtual const uci::type::ForeignKeyType& getSupportedModes() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedModes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedModes.
           */
         virtual uci::type::ForeignKeyType& getSupportedModes()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportedModes to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportedModes
           */
         virtual void setSupportedModes(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SupportedModes exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SupportedModes is emabled or not
           */
         virtual bool hasSupportedModes() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SupportedModes
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableSupportedModes(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SupportedModes */
         virtual void clearSupportedModes()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AudioFormats.
           *
           * @return The bounded list identified by AudioFormats.
           */
         virtual const uci::type::COMINT_CapabilityOptionsType::AudioFormats& getAudioFormats() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AudioFormats.
           *
           * @return The bounded list identified by AudioFormats.
           */
         virtual uci::type::COMINT_CapabilityOptionsType::AudioFormats& getAudioFormats()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AudioFormats.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAudioFormats(const uci::type::COMINT_CapabilityOptionsType::AudioFormats& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CompressionType.
           *
           * @return The bounded list identified by CompressionType.
           */
         virtual const uci::type::COMINT_CapabilityOptionsType::CompressionType& getCompressionType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CompressionType.
           *
           * @return The bounded list identified by CompressionType.
           */
         virtual uci::type::COMINT_CapabilityOptionsType::CompressionType& getCompressionType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CompressionType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCompressionType(const uci::type::COMINT_CapabilityOptionsType::CompressionType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_CapabilityOptionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_CapabilityOptionsType to copy from
           */
         COMINT_CapabilityOptionsType(const COMINT_CapabilityOptionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_CapabilityOptionsType to the contents of the
           * COMINT_CapabilityOptionsType on the right hand side (rhs) of the assignment operator.COMINT_CapabilityOptionsType
           * [only available to derived classes]
           *
           * @param rhs The COMINT_CapabilityOptionsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::COMINT_CapabilityOptionsType
           * @return A constant reference to this COMINT_CapabilityOptionsType.
           */
         const COMINT_CapabilityOptionsType& operator=(const COMINT_CapabilityOptionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_CapabilityOptionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_CapabilityOptionsType_h

