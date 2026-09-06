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
#ifndef Uci__Type__SpectralDensityReportMDT_h
#define Uci__Type__SpectralDensityReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SpectralBandType_h)
# include "uci/type/SpectralBandType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SpectralDensityReportMDT sequence accessor class */
      class SpectralDensityReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SpectralDensityReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. SpectralDensityReportMDT
           *
           * @return This accessor's type constant, i.e. SpectralDensityReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::spectralDensityReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SpectralDensityReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the spectral density measurement data [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SpectralBandType, uci::type::accessorType::spectralBandType> SpectralBand;

         /** Returns the value of the SimplePrimitive data type that is identified by the StartTimestamp.
           *
           * @return The value of the simple primitive data type identified by StartTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getStartTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EndTimestamp.
           *
           * @return The value of the simple primitive data type identified by EndTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getEndTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EndTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEndTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EndTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EndTimestamp is emabled or not
           */
         virtual bool hasEndTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EndTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEndTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EndTimestamp */
         virtual void clearEndTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpectralBand.
           *
           * @return The bounded list identified by SpectralBand.
           */
         virtual const uci::type::SpectralDensityReportMDT::SpectralBand& getSpectralBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpectralBand.
           *
           * @return The bounded list identified by SpectralBand.
           */
         virtual uci::type::SpectralDensityReportMDT::SpectralBand& getSpectralBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpectralBand.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpectralBand(const uci::type::SpectralDensityReportMDT::SpectralBand& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SpectralDensityReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SpectralDensityReportMDT to copy from
           */
         SpectralDensityReportMDT(const SpectralDensityReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SpectralDensityReportMDT to the contents of the
           * SpectralDensityReportMDT on the right hand side (rhs) of the assignment operator.SpectralDensityReportMDT [only
           * available to derived classes]
           *
           * @param rhs The SpectralDensityReportMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SpectralDensityReportMDT
           * @return A constant reference to this SpectralDensityReportMDT.
           */
         const SpectralDensityReportMDT& operator=(const SpectralDensityReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SpectralDensityReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SpectralDensityReportMDT_h

