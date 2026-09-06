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
#ifndef Uci__Type__EstimationStartingSegmentID_Type_h
#define Uci__Type__EstimationStartingSegmentID_Type_h 1

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EstimationStartingSegmentID_Type sequence accessor class */
      class EstimationStartingSegmentID_Type : public virtual uci::type::SegmentID_Type {
      public:

         /** The destructor */
         virtual ~EstimationStartingSegmentID_Type()
         { }

         /** Returns this accessor's type constant, i.e. EstimationStartingSegmentID_Type
           *
           * @return This accessor's type constant, i.e. EstimationStartingSegmentID_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::estimationStartingSegmentID_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EstimationStartingSegmentID_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SegmentCaptureTime.
           *
           * @return The value of the simple primitive data type identified by SegmentCaptureTime.
           */
         virtual uci::type::DateTimeTypeValue getSegmentCaptureTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SegmentCaptureTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSegmentCaptureTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SegmentCaptureTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SegmentCaptureTime is emabled or not
           */
         virtual bool hasSegmentCaptureTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SegmentCaptureTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableSegmentCaptureTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SegmentCaptureTime */
         virtual void clearSegmentCaptureTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EstimationStartingSegmentID_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EstimationStartingSegmentID_Type to copy from
           */
         EstimationStartingSegmentID_Type(const EstimationStartingSegmentID_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EstimationStartingSegmentID_Type to the contents of the
           * EstimationStartingSegmentID_Type on the right hand side (rhs) of the assignment
           * operator.EstimationStartingSegmentID_Type [only available to derived classes]
           *
           * @param rhs The EstimationStartingSegmentID_Type on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::EstimationStartingSegmentID_Type
           * @return A constant reference to this EstimationStartingSegmentID_Type.
           */
         const EstimationStartingSegmentID_Type& operator=(const EstimationStartingSegmentID_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EstimationStartingSegmentID_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EstimationStartingSegmentID_Type_h

