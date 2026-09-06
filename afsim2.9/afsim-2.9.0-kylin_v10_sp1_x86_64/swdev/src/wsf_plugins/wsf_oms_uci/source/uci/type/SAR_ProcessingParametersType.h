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
#ifndef Uci__Type__SAR_ProcessingParametersType_h
#define Uci__Type__SAR_ProcessingParametersType_h 1

#if !defined(Uci__Type__ProcessingParametersPET_h)
# include "uci/type/ProcessingParametersPET.h"
#endif

#if !defined(Uci__Type__ImageWeightEnum_h)
# include "uci/type/ImageWeightEnum.h"
#endif

#if !defined(Uci__Type__ImageRemapEnum_h)
# include "uci/type/ImageRemapEnum.h"
#endif

#if !defined(Uci__Type__ImageRegionOfInterestPET_h)
# include "uci/type/ImageRegionOfInterestPET.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base ProcessingParametersPET. It represents the
        * base processing parameters needed for SAR processing. Specific algorithms will have an extended type for additional
        * parameters. Available extension types include: SAR_AutomaticTargetDetectionParametersType,
        * SAR_CoherentChangeDetectionParametersType, SAR_DynamicImageParametersType, SAR_TwoColorMultiViewParametersType.
        */
      class SAR_ProcessingParametersType : public virtual uci::type::ProcessingParametersPET {
      public:

         /** The destructor */
         virtual ~SAR_ProcessingParametersType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_ProcessingParametersType
           *
           * @return This accessor's type constant, i.e. SAR_ProcessingParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_ProcessingParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_ProcessingParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Weight.
           *
           * @return The value of the enumeration identified by Weight.
           */
         virtual const uci::type::ImageWeightEnum& getWeight() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Weight.
           *
           * @return The value of the enumeration identified by Weight.
           */
         virtual uci::type::ImageWeightEnum& getWeight()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Weight.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeight(const uci::type::ImageWeightEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Weight.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeight(uci::type::ImageWeightEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Weightis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasWeight() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getWeight will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::imageWeightEnum This Accessor's accessor type
           */
         virtual void enableWeight(uci::base::accessorType::AccessorType type = uci::type::accessorType::imageWeightEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearWeight()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Remap.
           *
           * @return The value of the enumeration identified by Remap.
           */
         virtual const uci::type::ImageRemapEnum& getRemap() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Remap.
           *
           * @return The value of the enumeration identified by Remap.
           */
         virtual uci::type::ImageRemapEnum& getRemap()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Remap.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRemap(const uci::type::ImageRemapEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Remap.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRemap(uci::type::ImageRemapEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Remapis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRemap() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRemap will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::imageRemapEnum This Accessor's accessor type
           */
         virtual void enableRemap(uci::base::accessorType::AccessorType type = uci::type::accessorType::imageRemapEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRemap()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RegionOfInterest.
           *
           * @return The acecssor that provides access to the complex content that is identified by RegionOfInterest.
           */
         virtual const uci::type::ImageRegionOfInterestPET& getRegionOfInterest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RegionOfInterest.
           *
           * @return The acecssor that provides access to the complex content that is identified by RegionOfInterest.
           */
         virtual uci::type::ImageRegionOfInterestPET& getRegionOfInterest()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RegionOfInterest to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RegionOfInterest
           */
         virtual void setRegionOfInterest(const uci::type::ImageRegionOfInterestPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RegionOfInterest exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RegionOfInterest is emabled or not
           */
         virtual bool hasRegionOfInterest() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RegionOfInterest
           *
           * @param type = uci::type::accessorType::imageRegionOfInterestPET This Accessor's accessor type
           */
         virtual void enableRegionOfInterest(uci::base::accessorType::AccessorType type = uci::type::accessorType::imageRegionOfInterestPET)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RegionOfInterest */
         virtual void clearRegionOfInterest()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_ProcessingParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_ProcessingParametersType to copy from
           */
         SAR_ProcessingParametersType(const SAR_ProcessingParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_ProcessingParametersType to the contents of the
           * SAR_ProcessingParametersType on the right hand side (rhs) of the assignment operator.SAR_ProcessingParametersType
           * [only available to derived classes]
           *
           * @param rhs The SAR_ProcessingParametersType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SAR_ProcessingParametersType
           * @return A constant reference to this SAR_ProcessingParametersType.
           */
         const SAR_ProcessingParametersType& operator=(const SAR_ProcessingParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_ProcessingParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_ProcessingParametersType_h

