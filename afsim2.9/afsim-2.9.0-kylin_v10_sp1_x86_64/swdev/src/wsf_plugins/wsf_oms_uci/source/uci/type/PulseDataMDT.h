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
#ifndef Uci__Type__PulseDataMDT_h
#define Uci__Type__PulseDataMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PulseDataID_Type_h)
# include "uci/type/PulseDataID_Type.h"
#endif

#if !defined(Uci__Type__PulseDataSourceType_h)
# include "uci/type/PulseDataSourceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SignalNavDataType_h)
# include "uci/type/SignalNavDataType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__PulseType_h)
# include "uci/type/PulseType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PulseDataMDT sequence accessor class */
      class PulseDataMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PulseDataMDT()
         { }

         /** Returns this accessor's type constant, i.e. PulseDataMDT
           *
           * @return This accessor's type constant, i.e. PulseDataMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pulseDataMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PulseDataMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Data describing the state of the platform and antenna at the time of pulse collection. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SignalNavDataType, uci::type::accessorType::signalNavDataType> NavData;

         /** Indicates a signal intelligence Pulse. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::PulseType, uci::type::accessorType::pulseType> Pulse;

         /** Returns the accessor that provides access to the complex content that is identified by the PulseDataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseDataID.
           */
         virtual const uci::type::PulseDataID_Type& getPulseDataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseDataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseDataID.
           */
         virtual uci::type::PulseDataID_Type& getPulseDataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PulseDataID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PulseDataID
           */
         virtual void setPulseDataID(const uci::type::PulseDataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::PulseDataSourceType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::PulseDataSourceType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::PulseDataSourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Source exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Source is emabled or not
           */
         virtual bool hasSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Source
           *
           * @param type = uci::type::accessorType::pulseDataSourceType This Accessor's accessor type
           */
         virtual void enableSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::pulseDataSourceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Source */
         virtual void clearSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NavData.
           *
           * @return The bounded list identified by NavData.
           */
         virtual const uci::type::PulseDataMDT::NavData& getNavData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NavData.
           *
           * @return The bounded list identified by NavData.
           */
         virtual uci::type::PulseDataMDT::NavData& getNavData()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the NavData.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setNavData(const uci::type::PulseDataMDT::NavData& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PhaseAndAmplitudeCalibrated.
           *
           * @return The value of the simple primitive data type identified by PhaseAndAmplitudeCalibrated.
           */
         virtual xs::Boolean getPhaseAndAmplitudeCalibrated() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PhaseAndAmplitudeCalibrated.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPhaseAndAmplitudeCalibrated(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PhaseAndAmplitudeCalibrated exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PhaseAndAmplitudeCalibrated is emabled or not
           */
         virtual bool hasPhaseAndAmplitudeCalibrated() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PhaseAndAmplitudeCalibrated
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePhaseAndAmplitudeCalibrated(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PhaseAndAmplitudeCalibrated */
         virtual void clearPhaseAndAmplitudeCalibrated()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Pulse.
           *
           * @return The bounded list identified by Pulse.
           */
         virtual const uci::type::PulseDataMDT::Pulse& getPulse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Pulse.
           *
           * @return The bounded list identified by Pulse.
           */
         virtual uci::type::PulseDataMDT::Pulse& getPulse()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Pulse.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPulse(const uci::type::PulseDataMDT::Pulse& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PulseDataMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PulseDataMDT to copy from
           */
         PulseDataMDT(const PulseDataMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PulseDataMDT to the contents of the PulseDataMDT on the right hand
           * side (rhs) of the assignment operator.PulseDataMDT [only available to derived classes]
           *
           * @param rhs The PulseDataMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::PulseDataMDT
           * @return A constant reference to this PulseDataMDT.
           */
         const PulseDataMDT& operator=(const PulseDataMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PulseDataMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PulseDataMDT_h

