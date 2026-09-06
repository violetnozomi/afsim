// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTINPUT_HPP
#define UTINPUT_HPP

#include "ut_export.h"

#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "UtCast.hpp"
#include "UtCloneablePtr.hpp"
#include "UtInputBuffer.hpp"
#include "UtLatPos.hpp"
#include "UtLexicalCast.hpp"
#include "UtLonPos.hpp"
#include "UtStringUtil.hpp"
#include "UtUnits.hpp"

class UtAngle;
class UtAngleR;
class UtCalendar;
class UtColor;
#include "UtException.hpp"
class UtInputFile;
class UtInputPreprocessor;
class UtLatPos;
class UtLonPos;
class UtSpeed;
class UtStringId;
class UtReal;
class UtTime;

#if defined(_WIN32)
#if defined(GetMessage)
#undef GetMessage
#endif
#endif

/**
   An input processing object which can operate on multiple UtInputBuffer objects.
   Provides reading multiple input types, including automatic unit conversions.
   Automatically handles 'include's, allowing users to string together many files.
*/

class UT_EXPORT UtInput
{
public:
   //! Stores a location in the input stream for later use
   class UT_EXPORT StoredLocation
   {
   public:
      friend class UtInput;

      StoredLocation()                           = default;
      ~StoredLocation()                          = default;
      StoredLocation(const StoredLocation& aSrc) = default;
      StoredLocation& operator=(const StoredLocation& aRhs) = default;
      std::string     GetLocation() const;
      UtInputBuffer*  GetBuffer() { return mInputPtr.Pointer(); }

   private:
      ut::CloneablePtr<UtInputBuffer> mInputPtr{nullptr};

   public:
      size_t      mOffset{0};
      std::string mCurrentCommand;
   };

   // Exception classes.

   class UT_EXPORT ExceptionBase : public UtException
   {
   public:
      ExceptionBase(UtInput& aInput, const std::string& aWhat);
      ~ExceptionBase() override = default;

      const std::string& GetLocation() const { return mLocation; }
      const std::string& GetMessage() const { return mMessage; }

   private:
      std::string mMessage;
      std::string mLocation;
   };

   /** An exception that is thrown when a bad value is encountered in the input. */
   class UT_EXPORT BadValue : public ExceptionBase
   {
   public:
      BadValue(UtInput& aInput);
      BadValue(UtInput& aInput, const std::string& aMessage);
   };

   /** An exception that is thrown when the end of data is encountered. */
   class UT_EXPORT EndOfData : public ExceptionBase
   {
   public:
      EndOfData(UtInput& aInput);
   };

   /** An exception that is thrown when data are processed out of context. */
   class UT_EXPORT OutOfContext : public ExceptionBase
   {
   public:
      OutOfContext(UtInput& aInput);
      OutOfContext(UtInput& aInput, const std::string& aMessage);
   };

   /** An exception that is thrown by an input processor when it doesn't recognize a command. */
   class UT_EXPORT UnknownCommand : public ExceptionBase
   {
   public:
      UnknownCommand(UtInput& aInput);
   };

   /** An exception that is thrown when an unknown unit is encountered */
   class UT_EXPORT UnknownUnits : public ExceptionBase
   {
   public:
      UnknownUnits(UtInput& aInput);
   };

   //! Value types for ReadValueOfType.
   enum ValueType
   {
      // Data type              --- values are returned in units of... ---
      cLENGTH               = UtUnits::cLENGTH,               //!< meters
      cAREA                 = UtUnits::cAREA,                 //!< meters^2
      cAREA_DB              = UtUnits::cAREA_DB,              //!< meters^2, but also allows 'dBsm'
      cVOLUME               = UtUnits::cVOLUME,               //!< meters^3
      cSPEED                = UtUnits::cSPEED,                //!< meters/second
      cACCELERATION         = UtUnits::cACCELERATION,         //!< meters/second^2
      cTIME                 = UtUnits::cTIME,                 //!< seconds
      cFREQUENCY            = UtUnits::cFREQUENCY,            //!< Hertz
      cANGLE                = UtUnits::cANGLE,                //!< radians
      cANGULAR_RATE         = UtUnits::cANGULAR_RATE,         //!< radians/second
      cANGULAR_ACCELERATION = UtUnits::cANGULAR_ACCELERATION, //!< radians/second^2
      cDATA_RATE            = UtUnits::cDATA_RATE,            //!< bits/second
      cDATA_SIZE            = UtUnits::cDATA_SIZE,            //!< bits
      cPOWER                = UtUnits::cPOWER,                //!< watts
      cPOWER_DB             = UtUnits::cPOWER_DB,             //!< watts, but also allows  'dBw' and 'dBm'
      cSOLID_ANGLE          = UtUnits::cSOLID_ANGLE,          //!< steradians
      cMASS                 = UtUnits::cMASS,                 //!< kg
      cMASS_DENSITY         = UtUnits::cMASS_DENSITY,         //!< kg/m^3
      cMASS_TRANSFER        = UtUnits::cMASS_TRANSFER,        //!< kg/s
      cFORCE                = UtUnits::cFORCE,                //!< Newtons
      cENERGY               = UtUnits::cENERGY,               //!< joules
      cIRRADIANCE           = UtUnits::cIRRADIANCE,           //!< watts/meters^2
      cSPECTRAL_IRRADIANCE  = UtUnits::cSPECTRAL_IRRADIANCE,  //!< watts/meters^2/meter
      cRADIANCE             = UtUnits::cRADIANCE,             //!< watts/meters^2/steradian
      cSPECTRAL_RADIANCE    = UtUnits::cSPECTRAL_RADIANCE,    //!< watts/meters^2/steradian/meter
      cFLUENCE              = UtUnits::cFLUENCE,              //!< joules/meters^2
      cRATIO                = UtUnits::cRATIO,                //!< absolute ratio
      cNOISE_PRESSURE       = UtUnits::cNOISE_PRESSURE,       //!< dB - 20 micro-Pa
      cPRESSURE             = UtUnits::cPRESSURE,             //!< Pascal (Newtons/meters^2)
      cTEMPERATURE          = UtUnits::cTEMPERATURE,          //!< Kelvin
      cSPECIFIC_RANGE       = UtUnits::cSPECIFIC_RANGE,       //!< meters/kg
      cANGULAR_INERTIA      = UtUnits::cANGULAR_INERTIA,      //!< kg*meters^2
      cTORQUE               = UtUnits::cTORQUE,               //!< nt*m
      cVOLTAGE              = UtUnits::cVOLTAGE,              //!< volts
      cCURRENT              = UtUnits::cCURRENT,              //!< amps
      cCURRENT_DENSITY      = UtUnits::cCURRENT_DENSITY,      //!< amps/meters^2
      cRESISTANCE           = UtUnits::cRESISTANCE,           //!< ohms
      cCAPACITANCE          = UtUnits::cCAPACITANCE,          //!< farads
      cRESPONSIVITY         = UtUnits::cRESPONSIVITY,         //!< amps/watt
      cIMPULSE              = UtUnits::cIMPULSE,              //!< Newton * second

      // These value types are not 'unitary'
      cFIRST_NON_UNITARY = 100,
      cLATITUDE          = 100, //!< degrees (+ = north, - = south)
      cLONGITUDE,               //!< degrees (+ = east , - = west)
                                //--------------------------------------------------------------------
      cNON_DIMENSIONAL = 7777,  //!< non-dimensional
      cDEFERRED        = 8888,  //!< (placeholder for evaluation
                                //                      //!<  of a complex unit)
      cINVALID = 9999           //!< Invalid or not set
   };

   UtInput();
   UtInput(const UtInput& aSrc) = delete;
   UtInput(UtInput&&)           = default;
   UtInput& operator=(const UtInput&) = delete;
   UtInput& operator=(UtInput&&) = default;
   virtual ~UtInput()            = default;

   //@{
   static bool StringToValueType(const std::string& aTypeName, ValueType& aValueType);

   static ValueType GetValueTypeOf(const std::string& aUnits);

   ValueType GetValueType(const std::string& aUnits);

   //@}

   //@{
   std::string GetCurrentFileName() const;

   std::string GetFileName() const;
   //@}

   //@{
   bool AddToPathList(const std::string& aPathName);

   void ResetPathList();
   //@}

   //@{
   void DefinePathVariable(const std::string& aMacroName, const std::string& aMacroValue);

   void UndefinePathVariable(const std::string& aMacroName);

   void ResetPathVariableList();
   //@}

   std::string LocateFile(const std::string& aFileName);

   std::string ConvertRelativeInputFileName(const std::string& aFileName);

   double ConvertValue(double aValue, const std::string& aUnits, ValueType aValueType);

   static double ConvertValueFrom(double aValue, const std::string& aFromUnits, ValueType aValueType);

   static double ConvertValueTo(double aValue, const std::string& aDesiredUnits, ValueType aValueType);

   static bool ValidateUnits(const std::string& aUnits, ValueType aValueType);

   bool ConvertStringToBool(const std::string& aValue);

   const std::string& GetCommand();

   void GetCommand(std::string& aCommand);

   void PushBack(const std::string& aString);

   bool ReadCommand(std::string& aCommand);

   bool TryReadCommand(std::string& aCommand);

   void ReadLine(std::string& aValue, bool aIgnoreCurrentLine = true);

   bool ReadBool()
   {
      bool value;
      ReadValue(value);
      return value;
   }
   void ReadValue(bool& aValue);
   void ReadValue(std::string& aValue);
   void ReadValueQuoted(std::string& aValue);
   void ReadValue(UtStringId& aStringId);
   void ReadValue(UtCalendar& aCalendar);

   //! Read the next word from the input stream.
   //! @param  aValue The value read from the input stream.
   //! @throws EndOfData if there is no data left to read.
   //! @throws BadValue if the value was improperly formatted.
   template<typename T>
   void ReadValue(T& aValue)
   {
      std::string strValue;
      ReadValue(strValue);
      if /*constexpr*/ (std::is_unsigned<T>::value) // C++17 constexpr
      {
         if (strValue[0] == '-')
         {
            throw BadValue(*this);
         }
      }
      try
      {
         aValue = ut::lexical_cast<T>(strValue);
      }
      catch (ut::bad_lexical_cast&)
      {
         throw BadValue(*this);
      }
   }

   // Reads any unit value type from UtUnitTypes.hpp
   template<typename UNIT_VALUE_TYPE>
   void ReadUnitValue(UNIT_VALUE_TYPE& aUnitValue)
   {
      double number;
      ReadValue(number);
      std::string unit;
      ReadValue(unit);

      using UnitType = typename UNIT_VALUE_TYPE::UnitType;
      int unitId     = UnitType::ReadUnit(unit);
      if (unitId != -1)
      {
         aUnitValue.Set(number, unitId);
      }
      else
      {
         throw BadValue(*this);
      }
   }

   void ReadValue(UtColor& aColor);

   //! Read a value of given type from the input stream.
   //! @param  aValue [output] The resulting value converted to standard units.
   //! @param  aValueType The type of units to be read.
   //! @throws EndOfData if there is no data left to read.
   //! @throws BadValue if the value was improperly formatted or bad units were specified.
   template<typename T>
   void ReadValueOfType(T& aValue, ValueType aValueType)
   {
      static_assert(std::is_arithmetic<T>::value,
                    "UtInput::ReadValueOfType only valid for integral or floating point types");
      try
      {
         switch (aValueType)
         {
         case cLATITUDE:
         {
            UtLatPos lat;
            ReadValue(lat);
            aValue = ut::safe_cast<T, double>(lat);
            break;
         }
         case cLONGITUDE:
         {
            UtLonPos lon;
            ReadValue(lon);
            aValue = ut::safe_cast<T, double>(lon);
            break;
         }
         case cNON_DIMENSIONAL:
         {
            ReadValue(aValue);
            break;
         }
         default:
         {
            double      value;
            std::string units;
            ReadValue(value);
            ReadValue(units);
            UtStringUtil::ToLower(units);
            aValue = ut::safe_cast<T>(ConvertValue(value, units, aValueType));
            break;
         }
         }
      }
      catch (std::out_of_range& e)
      {
         throw BadValue(*this, e.what());
      }
   }

   void StringEqual(const std::string& aValue, const std::string& aExpectedValue);

   /** Determine if a value is in a closed range.

       @param aValue    Value to be tested.
       @param aMinValue Lower end of the closed range.
       @param aMaxValue Upper end of the closed range.
       @throws BadValue if aValue < aMinValue or aValue > aMaxValue
   */
   template<class T>
   void ValueInClosedRange(T aValue, T aMinValue, T aMaxValue)
   {
      ValueGreaterOrEqual(aValue, aMinValue);
      ValueLessOrEqual(aValue, aMaxValue);
   }

   /** Determine if a value is greater than a specified value.

       @param aValue    Value to be tested.
       @param aMinValue Lower value.
       @throws BadValue if aValue <= aMinValue
   */
   template<class T>
   void ValueGreater(T aValue, T aMinValue)
   {
      if (aValue <= aMinValue)
      {
         throw BadValue(*this,
                        "Expected value '" + UtStringUtil::ToString(aValue) + "' to be > " +
                           UtStringUtil::ToString(aMinValue));
      }
   }

   /** Determine if a value is greater than a specified value.

       @param aValue    Value to be tested.
       @param aMinValue Lower value.
       @throws BadValue if aValue < aMinValue
   */
   template<class T>
   void ValueGreaterOrEqual(T aValue, T aMinValue)
   {
      if (aValue < aMinValue)
      {
         throw BadValue(*this,
                        "Expected value '" + UtStringUtil::ToString(aValue) +
                           "' to be >= " + UtStringUtil::ToString(aMinValue));
      }
   }

   /** Determine if a value is greater than a specified value.

       @param aValue    Value to be tested.
       @param aMaxValue Lower value
       @throws BadValue if aValue >= aMaxValue
   */
   template<class T>
   void ValueLess(T aValue, T aMaxValue)
   {
      if (aValue >= aMaxValue)
      {
         throw BadValue(*this,
                        "Expected value '" + UtStringUtil::ToString(aValue) + " to be < " +
                           UtStringUtil::ToString(aMaxValue));
      }
   }

   /** Determine if a value is less than or equal to a specified value.

       @param aValue    Value to be tested.
       @param aMaxValue Upper value
       @throws BadValue if aValue > aMaxValue
   */
   template<class T>
   void ValueLessOrEqual(T aValue, T aMaxValue)
   {
      if (aValue > aMaxValue)
      {
         throw BadValue(*this,
                        "Expected value '" + UtStringUtil::ToString(aValue) +
                           " to be <= " + UtStringUtil::ToString(aMaxValue));
      }
   }

   std::string GetSource();

   std::string GetLocation();

   static bool BreakComplexUnit(const std::string& aComplexUnit, std::string& aSubject, std::string& aPredicate);

   static bool BreakComplexUnit(const std::string& aUnits, std::string& aSubject, std::string& aPredicate, char& aOperator);

   std::string SubstitutePathVariables(const std::string& aString);

   //! @name File open callback registration.
   //! The user may register a callback function that is called whenever UtInput opens an include file.
   //! It is also available to external users of UtInput that themselves open a file in response to some input.
   //! (e.g.: UtTable, etc.)
   //@{
   using FileOpenCallback = std::function<void(const std::string&)>;

   //! Specify a callback to be invoked when a nested include file is opened.
   void SetFileOpenCallback(const FileOpenCallback& aCallback) { mFileOpenCallback = aCallback; }

   //! Invoke the file open callback if it is defined.
   void DoFileOpenCallback(const std::string& aFileName)
   {
      if ((!aFileName.empty()) && mFileOpenCallback)
      {
         mFileOpenCallback(aFileName);
      }
   }
   //@}

   void SetDebug(bool aValue) { mDebug = aValue; }


   UtInputBuffer* GetCurrentInput();

   const std::queue<std::string>& GetPushedBackStrings() const { return mPushedBackStrings; }

   void ClearPushedBackStrings();

   StoredLocation StoreLocation();

   void RestoreLocation(const StoredLocation& aLocation);

   double GetCurrentOrdering() const { return mCurrentOrdering; }

   void PushInput(std::unique_ptr<UtInputBuffer> aInputPtr);

   void PushInputString(const std::string& aString);

   void LinkInputs(UtInput& aOtherInput);

   void LinkBuffers(UtInput& aOtherInput);

   void CopyAuxData(const UtInput& aOtherInput);

   void* GetAuxData(int aIndex);

   void SetAuxData(int aIndex, void* aAuxData);

   UtInputPreprocessor* GetPreprocessor() const;

protected:
   void PopInput();

   //! Uniquely identify a file
   class FileID
   {
   public:
      FileID(const std::string& aRelativeFileName);

      bool operator==(const FileID& aEq) const;
      bool operator<(const FileID& aRhs) const;

      const std::string& GetFileName() const { return mFileName; }

      bool IsValid() const;

   private:
      std::string mFileName;
      unsigned    mDeviceId;
      unsigned    mINodeId;
      std::string mWindowsFullPathName;
   };

   static bool BreakComplexUnit(const std::string& aFraction,
                                const char&        aOperator,
                                std::string&       aNumerator,
                                std::string&       aDenominator);

   static bool ConvertValueP(double& aValue, const std::string& aUnits, ValueType aValueType);

   static bool EvaluateComplexUnit(double&            aValue,
                                   const std::string& aUnits,
                                   const char&        aOperator,
                                   ValueType          aSubjectType,
                                   ValueType          aPredicateType);

   std::string LocateFile(const std::string& aFileName, const std::string& aCurrentFileName);


   std::string             mCommand;
   std::queue<std::string> mPushedBackStrings;
   // The current input buffer
   ut::CloneablePtr<UtInputBuffer> mInputPtr;

   std::shared_ptr<UtInputPreprocessor> mPreprocessor;

   // The input buffers to be processed after mInputPtr (usually due to an include statement)
   std::deque<ut::CloneablePtr<UtInputBuffer>> mInputStack;
   FileOpenCallback                            mFileOpenCallback;

   struct BufferCache
   {
      using FileBufferMap = std::map<FileID, ut::CloneablePtr<UtInputBuffer>>;
      using FileID_Set    = std::set<FileID>;

      FileBufferMap mFileBuffers;
      // For include_once check
      FileID_Set mIncludedFiles;
   };

   std::shared_ptr<BufferCache> mBufferCache;
   double                       mCurrentOrdering;
   bool                         mDebug;
   std::vector<void*>           mAuxData;

   //! The list of file path entries.
   std::list<std::string> mPathList;

   //! The list of path variable definitions.
   std::map<std::string, std::string> mPathVariables;
};

#endif
