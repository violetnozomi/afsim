// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__GainRangeType_h
#define Uci__Type__GainRangeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the GainRangeType sequence accessor class */
      class GainRangeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GainRangeType()
         { }

         /** Returns this accessor's type constant, i.e. GainRangeType
           *
           * @return This accessor's type constant, i.e. GainRangeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gainRangeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GainRangeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GainMax.
           *
           * @return The value of the simple primitive data type identified by GainMax.
           */
         virtual uci::type::DecibelTypeValue getGainMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GainMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGainMax(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GainMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GainMax is emabled or not
           */
         virtual bool hasGainMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GainMax
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableGainMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GainMax */
         virtual void clearGainMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GainMin.
           *
           * @return The value of the simple primitive data type identified by GainMin.
           */
         virtual uci::type::DecibelTypeValue getGainMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GainMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGainMin(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GainMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GainMin is emabled or not
           */
         virtual bool hasGainMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GainMin
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableGainMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GainMin */
         virtual void clearGainMin()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GainRangeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GainRangeType to copy from
           */
         GainRangeType(const GainRangeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GainRangeType to the contents of the GainRangeType on the right
           * hand side (rhs) of the assignment operator.GainRangeType [only available to derived classes]
           *
           * @param rhs The GainRangeType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::GainRangeType
           * @return A constant reference to this GainRangeType.
           */
         const GainRangeType& operator=(const GainRangeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GainRangeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GainRangeType_h

