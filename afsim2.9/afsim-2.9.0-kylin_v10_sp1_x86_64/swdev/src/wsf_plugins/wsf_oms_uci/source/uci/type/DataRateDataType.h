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
#ifndef Uci__Type__DataRateDataType_h
#define Uci__Type__DataRateDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DataRateDataType sequence accessor class */
      class DataRateDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataRateDataType()
         { }

         /** Returns this accessor's type constant, i.e. DataRateDataType
           *
           * @return This accessor's type constant, i.e. DataRateDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataRateDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataRateDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataRateAverage.
           *
           * @return The value of the simple primitive data type identified by DataRateAverage.
           */
         virtual uci::type::DataRateTypeValue getDataRateAverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataRateAverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataRateAverage(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataRateAverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataRateAverage is emabled or not
           */
         virtual bool hasDataRateAverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataRateAverage
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableDataRateAverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataRateAverage */
         virtual void clearDataRateAverage()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataRateMin.
           *
           * @return The value of the simple primitive data type identified by DataRateMin.
           */
         virtual uci::type::DataRateTypeValue getDataRateMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataRateMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataRateMin(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataRateMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataRateMin is emabled or not
           */
         virtual bool hasDataRateMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataRateMin
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableDataRateMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataRateMin */
         virtual void clearDataRateMin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataRateMax.
           *
           * @return The value of the simple primitive data type identified by DataRateMax.
           */
         virtual uci::type::DataRateTypeValue getDataRateMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataRateMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataRateMax(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataRateMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataRateMax is emabled or not
           */
         virtual bool hasDataRateMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataRateMax
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableDataRateMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataRateMax */
         virtual void clearDataRateMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataRateDeviation.
           *
           * @return The value of the simple primitive data type identified by DataRateDeviation.
           */
         virtual uci::type::DataRateTypeValue getDataRateDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataRateDeviation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataRateDeviation(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataRateDeviation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataRateDeviation is emabled or not
           */
         virtual bool hasDataRateDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataRateDeviation
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableDataRateDeviation(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataRateDeviation */
         virtual void clearDataRateDeviation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataRateDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataRateDataType to copy from
           */
         DataRateDataType(const DataRateDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataRateDataType to the contents of the DataRateDataType on the
           * right hand side (rhs) of the assignment operator.DataRateDataType [only available to derived classes]
           *
           * @param rhs The DataRateDataType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DataRateDataType
           * @return A constant reference to this DataRateDataType.
           */
         const DataRateDataType& operator=(const DataRateDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataRateDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataRateDataType_h

