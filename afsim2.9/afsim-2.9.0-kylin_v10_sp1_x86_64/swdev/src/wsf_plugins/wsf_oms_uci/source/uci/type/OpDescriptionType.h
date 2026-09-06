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
#ifndef Uci__Type__OpDescriptionType_h
#define Uci__Type__OpDescriptionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Markup for OpPoint and OpZone. */
      class OpDescriptionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpDescriptionType()
         { }

         /** Returns this accessor's type constant, i.e. OpDescriptionType
           *
           * @return This accessor's type constant, i.e. OpDescriptionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opDescriptionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpDescriptionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual const uci::type::VisibleString256Type& getName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual uci::type::VisibleString256Type& getName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setName(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Name exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Name is emabled or not
           */
         virtual bool hasName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Name
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableName(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Name */
         virtual void clearName()
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
         OpDescriptionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpDescriptionType to copy from
           */
         OpDescriptionType(const OpDescriptionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpDescriptionType to the contents of the OpDescriptionType on the
           * right hand side (rhs) of the assignment operator.OpDescriptionType [only available to derived classes]
           *
           * @param rhs The OpDescriptionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OpDescriptionType
           * @return A constant reference to this OpDescriptionType.
           */
         const OpDescriptionType& operator=(const OpDescriptionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpDescriptionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpDescriptionType_h

