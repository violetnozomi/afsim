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
#ifndef Uci__Type__SAR_TwoColorMultiViewParametersType_h
#define Uci__Type__SAR_TwoColorMultiViewParametersType_h 1

#if !defined(Uci__Type__SAR_ProcessingParametersType_h)
# include "uci/type/SAR_ProcessingParametersType.h"
#endif

#if !defined(Uci__Type__ImagePixelDepthEnum_h)
# include "uci/type/ImagePixelDepthEnum.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type extends SAR_ProcessingParametersType to provide processing parameters needed for Two Color Multi-View
        * products between two SAR image products.
        */
      class SAR_TwoColorMultiViewParametersType : public virtual uci::type::SAR_ProcessingParametersType {
      public:

         /** The destructor */
         virtual ~SAR_TwoColorMultiViewParametersType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_TwoColorMultiViewParametersType
           *
           * @return This accessor's type constant, i.e. SAR_TwoColorMultiViewParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_TwoColorMultiViewParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_TwoColorMultiViewParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PixelDepth.
           *
           * @return The value of the enumeration identified by PixelDepth.
           */
         virtual const uci::type::ImagePixelDepthEnum& getPixelDepth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PixelDepth.
           *
           * @return The value of the enumeration identified by PixelDepth.
           */
         virtual uci::type::ImagePixelDepthEnum& getPixelDepth()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PixelDepth.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPixelDepth(const uci::type::ImagePixelDepthEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PixelDepth.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPixelDepth(uci::type::ImagePixelDepthEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceID.
           */
         virtual const uci::type::ProductMetadataID_Type& getReferenceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceID.
           */
         virtual uci::type::ProductMetadataID_Type& getReferenceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferenceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferenceID
           */
         virtual void setReferenceID(const uci::type::ProductMetadataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MatchID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MatchID.
           */
         virtual const uci::type::ProductMetadataID_Type& getMatchID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MatchID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MatchID.
           */
         virtual uci::type::ProductMetadataID_Type& getMatchID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MatchID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MatchID
           */
         virtual void setMatchID(const uci::type::ProductMetadataID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_TwoColorMultiViewParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_TwoColorMultiViewParametersType to copy from
           */
         SAR_TwoColorMultiViewParametersType(const SAR_TwoColorMultiViewParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_TwoColorMultiViewParametersType to the contents of the
           * SAR_TwoColorMultiViewParametersType on the right hand side (rhs) of the assignment
           * operator.SAR_TwoColorMultiViewParametersType [only available to derived classes]
           *
           * @param rhs The SAR_TwoColorMultiViewParametersType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SAR_TwoColorMultiViewParametersType
           * @return A constant reference to this SAR_TwoColorMultiViewParametersType.
           */
         const SAR_TwoColorMultiViewParametersType& operator=(const SAR_TwoColorMultiViewParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_TwoColorMultiViewParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_TwoColorMultiViewParametersType_h

