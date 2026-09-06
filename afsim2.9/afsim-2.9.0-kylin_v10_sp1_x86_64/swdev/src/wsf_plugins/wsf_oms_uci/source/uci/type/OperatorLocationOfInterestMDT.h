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
#ifndef Uci__Type__OperatorLocationOfInterestMDT_h
#define Uci__Type__OperatorLocationOfInterestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OperatorLocationOfInterestID_Type_h)
# include "uci/type/OperatorLocationOfInterestID_Type.h"
#endif

#if !defined(Uci__Type__EntityPositionType_h)
# include "uci/type/EntityPositionType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorLocationOfInterestMDT sequence accessor class */
      class OperatorLocationOfInterestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorLocationOfInterestMDT()
         { }

         /** Returns this accessor's type constant, i.e. OperatorLocationOfInterestMDT
           *
           * @return This accessor's type constant, i.e. OperatorLocationOfInterestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorLocationOfInterestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorLocationOfInterestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * OperatorLocationOfInterestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorLocationOfInterestID.
           */
         virtual const uci::type::OperatorLocationOfInterestID_Type& getOperatorLocationOfInterestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * OperatorLocationOfInterestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorLocationOfInterestID.
           */
         virtual uci::type::OperatorLocationOfInterestID_Type& getOperatorLocationOfInterestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorLocationOfInterestID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorLocationOfInterestID
           */
         virtual void setOperatorLocationOfInterestID(const uci::type::OperatorLocationOfInterestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::EntityPositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::EntityPositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::EntityPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual const uci::type::VisibleString1024Type& getRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual uci::type::VisibleString1024Type& getRemarks()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRemarks(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Remarks exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Remarks is emabled or not
           */
         virtual bool hasRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Remarks
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableRemarks(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Remarks */
         virtual void clearRemarks()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorLocationOfInterestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorLocationOfInterestMDT to copy from
           */
         OperatorLocationOfInterestMDT(const OperatorLocationOfInterestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorLocationOfInterestMDT to the contents of the
           * OperatorLocationOfInterestMDT on the right hand side (rhs) of the assignment operator.OperatorLocationOfInterestMDT
           * [only available to derived classes]
           *
           * @param rhs The OperatorLocationOfInterestMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::OperatorLocationOfInterestMDT
           * @return A constant reference to this OperatorLocationOfInterestMDT.
           */
         const OperatorLocationOfInterestMDT& operator=(const OperatorLocationOfInterestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorLocationOfInterestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorLocationOfInterestMDT_h

