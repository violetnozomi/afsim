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
#ifndef Uci__Type__PixelReferenceCoordinateType_h
#define Uci__Type__PixelReferenceCoordinateType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PixelDataType_h)
# include "uci/type/PixelDataType.h"
#endif

#if !defined(Uci__Type__FileNameType_h)
# include "uci/type/FileNameType.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Define a pixel coordinate and a reference image given as a File, a ProductMetadataID and/or a reference ForeignKey. */
      class PixelReferenceCoordinateType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PixelReferenceCoordinateType()
         { }

         /** Returns this accessor's type constant, i.e. PixelReferenceCoordinateType
           *
           * @return This accessor's type constant, i.e. PixelReferenceCoordinateType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pixelReferenceCoordinateType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PixelReferenceCoordinateType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelCoordinate.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelCoordinate.
           */
         virtual const uci::type::PixelDataType& getPixelCoordinate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelCoordinate.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelCoordinate.
           */
         virtual uci::type::PixelDataType& getPixelCoordinate()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelCoordinate to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelCoordinate
           */
         virtual void setPixelCoordinate(const uci::type::PixelDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the File.
           *
           * @return The value of the string data type identified by File.
           */
         virtual const uci::type::FileNameType& getFile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the File.
           *
           * @return The value of the string data type identified by File.
           */
         virtual uci::type::FileNameType& getFile()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the File to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFile(const uci::type::FileNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the File to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFile(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the File to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFile(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by File exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by File is emabled or not
           */
         virtual bool hasFile() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by File
           *
           * @param type = uci::type::accessorType::fileNameType This Accessor's accessor type
           */
         virtual void enableFile(uci::base::accessorType::AccessorType type = uci::type::accessorType::fileNameType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by File */
         virtual void clearFile()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductID.
           */
         virtual const uci::type::ProductMetadataID_Type& getProductID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductID.
           */
         virtual uci::type::ProductMetadataID_Type& getProductID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductID
           */
         virtual void setProductID(const uci::type::ProductMetadataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductID is emabled or not
           */
         virtual bool hasProductID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductID
           *
           * @param type = uci::type::accessorType::productMetadataID_Type This Accessor's accessor type
           */
         virtual void enableProductID(uci::base::accessorType::AccessorType type = uci::type::accessorType::productMetadataID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductID */
         virtual void clearProductID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reference.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reference.
           */
         virtual const uci::type::ForeignKeyType& getReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reference.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reference.
           */
         virtual uci::type::ForeignKeyType& getReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Reference to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Reference
           */
         virtual void setReference(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Reference exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Reference is emabled or not
           */
         virtual bool hasReference() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Reference
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableReference(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Reference */
         virtual void clearReference()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PixelReferenceCoordinateType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PixelReferenceCoordinateType to copy from
           */
         PixelReferenceCoordinateType(const PixelReferenceCoordinateType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PixelReferenceCoordinateType to the contents of the
           * PixelReferenceCoordinateType on the right hand side (rhs) of the assignment operator.PixelReferenceCoordinateType
           * [only available to derived classes]
           *
           * @param rhs The PixelReferenceCoordinateType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::PixelReferenceCoordinateType
           * @return A constant reference to this PixelReferenceCoordinateType.
           */
         const PixelReferenceCoordinateType& operator=(const PixelReferenceCoordinateType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PixelReferenceCoordinateType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PixelReferenceCoordinateType_h

