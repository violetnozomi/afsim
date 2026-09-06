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
#ifndef Uci__Type__MTI_ActivityStatusDetailType_h
#define Uci__Type__MTI_ActivityStatusDetailType_h 1

#if !defined(Uci__Type__MTI_ActivityStatusBaseType_h)
# include "uci/type/MTI_ActivityStatusBaseType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EstimatedPerformanceType_h)
# include "uci/type/EstimatedPerformanceType.h"
#endif

#if !defined(Uci__Type__AreaChoiceType_h)
# include "uci/type/AreaChoiceType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Collection metrics used for describing the progress of an activity (sensor collection) being performed by a MTI
        * sensor.
        */
      class MTI_ActivityStatusDetailType : public virtual uci::type::MTI_ActivityStatusBaseType {
      public:

         /** The destructor */
         virtual ~MTI_ActivityStatusDetailType()
         { }

         /** Returns this accessor's type constant, i.e. MTI_ActivityStatusDetailType
           *
           * @return This accessor's type constant, i.e. MTI_ActivityStatusDetailType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mTI_ActivityStatusDetailType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MTI_ActivityStatusDetailType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Estimated performance of the activity [Minimum occurrences: 0] [Maximum occurrences: 5] */
         typedef uci::base::BoundedList<uci::type::EstimatedPerformanceType, uci::type::accessorType::estimatedPerformanceType> EstimatedPerformance;

         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedERP.
           *
           * @return The value of the simple primitive data type identified by EstimatedERP.
           */
         virtual uci::type::DecibelTypeValue getEstimatedERP() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedERP.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedERP(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedERP exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedERP is emabled or not
           */
         virtual bool hasEstimatedERP() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedERP
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableEstimatedERP(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedERP */
         virtual void clearEstimatedERP()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ProductsProduced.
           *
           * @return The value of the simple primitive data type identified by ProductsProduced.
           */
         virtual xs::UnsignedInt getProductsProduced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProductsProduced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProductsProduced(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductsProduced exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductsProduced is emabled or not
           */
         virtual bool hasProductsProduced() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductsProduced
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableProductsProduced(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductsProduced */
         virtual void clearProductsProduced()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HitsReported.
           *
           * @return The value of the simple primitive data type identified by HitsReported.
           */
         virtual xs::UnsignedInt getHitsReported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HitsReported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHitsReported(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HitsReported exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HitsReported is emabled or not
           */
         virtual bool hasHitsReported() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HitsReported
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableHitsReported(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HitsReported */
         virtual void clearHitsReported()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EstimatedPerformance.
           *
           * @return The bounded list identified by EstimatedPerformance.
           */
         virtual const uci::type::MTI_ActivityStatusDetailType::EstimatedPerformance& getEstimatedPerformance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EstimatedPerformance.
           *
           * @return The bounded list identified by EstimatedPerformance.
           */
         virtual uci::type::MTI_ActivityStatusDetailType::EstimatedPerformance& getEstimatedPerformance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EstimatedPerformance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEstimatedPerformance(const uci::type::MTI_ActivityStatusDetailType::EstimatedPerformance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetAreaScanned.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetAreaScanned.
           */
         virtual const uci::type::AreaChoiceType& getTargetAreaScanned() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetAreaScanned.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetAreaScanned.
           */
         virtual uci::type::AreaChoiceType& getTargetAreaScanned()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TargetAreaScanned to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TargetAreaScanned
           */
         virtual void setTargetAreaScanned(const uci::type::AreaChoiceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TargetAreaScanned exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TargetAreaScanned is emabled or not
           */
         virtual bool hasTargetAreaScanned() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TargetAreaScanned
           *
           * @param type = uci::type::accessorType::areaChoiceType This Accessor's accessor type
           */
         virtual void enableTargetAreaScanned(uci::base::accessorType::AccessorType type = uci::type::accessorType::areaChoiceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TargetAreaScanned */
         virtual void clearTargetAreaScanned()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeToService.
           *
           * @return The value of the simple primitive data type identified by TimeToService.
           */
         virtual uci::type::DurationTypeValue getTimeToService() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeToService.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeToService(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeToService exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeToService is emabled or not
           */
         virtual bool hasTimeToService() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeToService
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTimeToService(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeToService */
         virtual void clearTimeToService()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AverageTimeToService.
           *
           * @return The value of the simple primitive data type identified by AverageTimeToService.
           */
         virtual uci::type::DurationTypeValue getAverageTimeToService() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AverageTimeToService.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAverageTimeToService(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AverageTimeToService exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AverageTimeToService is emabled or not
           */
         virtual bool hasAverageTimeToService() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AverageTimeToService
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableAverageTimeToService(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AverageTimeToService */
         virtual void clearAverageTimeToService()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MTI_ActivityStatusDetailType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MTI_ActivityStatusDetailType to copy from
           */
         MTI_ActivityStatusDetailType(const MTI_ActivityStatusDetailType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MTI_ActivityStatusDetailType to the contents of the
           * MTI_ActivityStatusDetailType on the right hand side (rhs) of the assignment operator.MTI_ActivityStatusDetailType
           * [only available to derived classes]
           *
           * @param rhs The MTI_ActivityStatusDetailType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MTI_ActivityStatusDetailType
           * @return A constant reference to this MTI_ActivityStatusDetailType.
           */
         const MTI_ActivityStatusDetailType& operator=(const MTI_ActivityStatusDetailType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MTI_ActivityStatusDetailType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MTI_ActivityStatusDetailType_h

