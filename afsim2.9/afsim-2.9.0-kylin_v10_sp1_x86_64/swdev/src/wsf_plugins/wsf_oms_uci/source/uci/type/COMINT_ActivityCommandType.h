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
#ifndef Uci__Type__COMINT_ActivityCommandType_h
#define Uci__Type__COMINT_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__COMINT_ChangeDwellType_h)
# include "uci/type/COMINT_ChangeDwellType.h"
#endif

#if !defined(Uci__Type__COMINT_ChangeGeolocationType_h)
# include "uci/type/COMINT_ChangeGeolocationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_ActivityCommandType sequence accessor class */
      class COMINT_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~COMINT_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. COMINT_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies the specific dwell that the Subsystem should enable, disable, unallocate, or delete. The dwell's identity
           * is originally defined in the optional ReceiveDwell structure of the Activity message. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::COMINT_ChangeDwellType, uci::type::accessorType::cOMINT_ChangeDwellType> COMINT_ChangeDwell;

         /** Returns the bounded list that is identified by the COMINT_ChangeDwell.
           *
           * @return The bounded list identified by COMINT_ChangeDwell.
           */
         virtual const uci::type::COMINT_ActivityCommandType::COMINT_ChangeDwell& getCOMINT_ChangeDwell() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the COMINT_ChangeDwell.
           *
           * @return The bounded list identified by COMINT_ChangeDwell.
           */
         virtual uci::type::COMINT_ActivityCommandType::COMINT_ChangeDwell& getCOMINT_ChangeDwell()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the COMINT_ChangeDwell.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCOMINT_ChangeDwell(const uci::type::COMINT_ActivityCommandType::COMINT_ChangeDwell& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the COMINT_ChangeGeolocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by COMINT_ChangeGeolocation.
           */
         virtual const uci::type::COMINT_ChangeGeolocationType& getCOMINT_ChangeGeolocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the COMINT_ChangeGeolocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by COMINT_ChangeGeolocation.
           */
         virtual uci::type::COMINT_ChangeGeolocationType& getCOMINT_ChangeGeolocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the COMINT_ChangeGeolocation to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by COMINT_ChangeGeolocation
           */
         virtual void setCOMINT_ChangeGeolocation(const uci::type::COMINT_ChangeGeolocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by COMINT_ChangeGeolocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by COMINT_ChangeGeolocation is emabled or not
           */
         virtual bool hasCOMINT_ChangeGeolocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by COMINT_ChangeGeolocation
           *
           * @param type = uci::type::accessorType::cOMINT_ChangeGeolocationType This Accessor's accessor type
           */
         virtual void enableCOMINT_ChangeGeolocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::cOMINT_ChangeGeolocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by COMINT_ChangeGeolocation */
         virtual void clearCOMINT_ChangeGeolocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @return The value of the simple primitive data type identified by SpectralDensityReporting.
           */
         virtual xs::Boolean getSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpectralDensityReporting(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpectralDensityReporting exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpectralDensityReporting is emabled or not
           */
         virtual bool hasSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpectralDensityReporting
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSpectralDensityReporting(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpectralDensityReporting */
         virtual void clearSpectralDensityReporting()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_ActivityCommandType to copy from
           */
         COMINT_ActivityCommandType(const COMINT_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_ActivityCommandType to the contents of the
           * COMINT_ActivityCommandType on the right hand side (rhs) of the assignment operator.COMINT_ActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The COMINT_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::COMINT_ActivityCommandType
           * @return A constant reference to this COMINT_ActivityCommandType.
           */
         const COMINT_ActivityCommandType& operator=(const COMINT_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_ActivityCommandType_h

