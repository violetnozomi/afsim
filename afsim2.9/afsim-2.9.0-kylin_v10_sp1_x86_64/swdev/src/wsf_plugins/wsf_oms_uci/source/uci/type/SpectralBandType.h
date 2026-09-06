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
#ifndef Uci__Type__SpectralBandType_h
#define Uci__Type__SpectralBandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Xs__Type__BinaryXmlSchemaPrimitives_h)
# include "xs/type/binaryXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SpectralBandType sequence accessor class */
      class SpectralBandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SpectralBandType()
         { }

         /** Returns this accessor's type constant, i.e. SpectralBandType
           *
           * @return This accessor's type constant, i.e. SpectralBandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::spectralBandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SpectralBandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the energy spectral density within the sibling Frequency band. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MilliwattPowerRatioType, uci::type::accessorType::milliwattPowerRatioType> SpectralDensity;

         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual const uci::type::ActivityID_Type& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual uci::type::ActivityID_Type& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityID
           */
         virtual void setActivityID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActivityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActivityID is emabled or not
           */
         virtual bool hasActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActivityID
           *
           * @param type = uci::type::accessorType::activityID_Type This Accessor's accessor type
           */
         virtual void enableActivityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::activityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActivityID */
         virtual void clearActivityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual const uci::type::FrequencyRangeType& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual uci::type::FrequencyRangeType& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Frequency to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Frequency
           */
         virtual void setFrequency(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpectralDensity.
           *
           * @return The bounded list identified by SpectralDensity.
           */
         virtual const uci::type::SpectralBandType::SpectralDensity& getSpectralDensity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpectralDensity.
           *
           * @return The bounded list identified by SpectralDensity.
           */
         virtual uci::type::SpectralBandType::SpectralDensity& getSpectralDensity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpectralDensity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpectralDensity(const uci::type::SpectralBandType::SpectralDensity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SpectralImage field contained in this message fragment
           *
           * @return The reference to the accessor to the SpectralImage field.
           */
         virtual const xs::HexBinary& getSpectralImage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SpectralImage field contained in this message fragment
           *
           * @return The reference to the accessor to the SpectralImage field.
           */
         virtual xs::HexBinary& getSpectralImage()
            throw(uci::base::UCIException) = 0;


         /** Set the contents of the message fragment that this accesssor provides access to to the contents the specified
           * accessor provides access to. A deep copy will be used by this method in setting the contents
           *
           * @param value The accessor to use to set the contents of the message fragment that this accessor provides access to.
           */
         virtual void setSpectralImage(const xs::HexBinary& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SpectralImageis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSpectralImage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSpectralImage will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = xs::accessorType::hexBinary This Accessor's accessor type
           */
         virtual void enableSpectralImage(uci::base::accessorType::AccessorType type = xs::accessorType::hexBinary)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSpectralImage()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SpectralBandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SpectralBandType to copy from
           */
         SpectralBandType(const SpectralBandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SpectralBandType to the contents of the SpectralBandType on the
           * right hand side (rhs) of the assignment operator.SpectralBandType [only available to derived classes]
           *
           * @param rhs The SpectralBandType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SpectralBandType
           * @return A constant reference to this SpectralBandType.
           */
         const SpectralBandType& operator=(const SpectralBandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SpectralBandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SpectralBandType_h

