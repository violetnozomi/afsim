// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: logger.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract: Logging facilities for the battle manager port to AFSIM.
*    The HCL acronym below stands for HELIOS C2 Logger.
*
*******************************************************************************/

#ifndef HELIOS_C2_LOGGER
#define HELIOS_C2_LOGGER

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

#include "iadslib_export.h"

namespace std
{
   class mutex;
}

namespace il
{
   namespace logger
   {
      // The fields in this enum should be ordered from most-specific level
      // to least-specific level. This allows the log level filter to work
      // correctly. Also, the last field should be OFF.
      enum eHCL_LOG_LEVEL
      {
         TRACE = 0,
         DEBUG,
         INFO,
         WARN,
         ERROR,
         FATAL,
         OFF
      };

      enum eHCL_LOG_ITEM_TYPE
      {
         LOG_ITEM_CONSTANT_STRING,
         LOG_ITEM_MESSAGE,
         LOG_ITEM_SEVERITY
      };

      struct LoggerItem
      {
         eHCL_LOG_ITEM_TYPE type;
         // this field is only relevant to constant string items,
         // but it is convenient to place it here
         size_t constantStringLength;
      };

      struct LogFormat
      {
         // pack all constant strings into a single string for cache coherency
         std::string constantStrings;
         std::vector<LoggerItem> items;
      };

      // A class for accessing a global logger in a thread-safe manner.
      class IADSLIB_EXPORT GlobalLogger
      {
         public:
            // Gets the global logging level.
            eHCL_LOG_LEVEL getLevel();

            // Sets the global logging level.
            void setLevel(eHCL_LOG_LEVEL);

            // Sets the global logging format.
            void setFormat(const LogFormat& format);

            // Writes a record to the logger output stream
            void writeRecord(il::logger::eHCL_LOG_LEVEL level,
                                    const std::string& message);

            GlobalLogger()
            {
               LoggerItem item = {};
               item.type = LOG_ITEM_MESSAGE;
               format.items.push_back(item);
            }

         private:
            GlobalLogger(const GlobalLogger& other);
            GlobalLogger& operator=(const GlobalLogger& other) = delete;

            std::mutex instanceMutex{};

            eHCL_LOG_LEVEL level{WARN};
            LogFormat format;
      };

      // This class stores state used for a single log record. It is not intended for
      // use outside the LOG_WITH_STREAM macro.
      class IADSLIB_EXPORT macrotype_LogRecord
      {
         public:
            macrotype_LogRecord(eHCL_LOG_LEVEL level, std::weak_ptr<GlobalLogger> aGlobalLogger);
            ~macrotype_LogRecord();

            int i;
            eHCL_LOG_LEVEL level;
            //TODO The capacity of std::stringstream cannot be set. This could result
            // in many reallocations. Look for a different approach for formatted output
            // that allows preallocating space.
            std::ostringstream messageBuffer;

         private:
            macrotype_LogRecord(const macrotype_LogRecord& other);
            macrotype_LogRecord& operator=(const macrotype_LogRecord& other) = delete;

            std::weak_ptr<GlobalLogger> mGlobalLogger;
      };

      // This macro conditionally logs a record using the '<<' syntax for writing to
      // a stream using some syntactical and language tricks. It declares per-record
      // state in a for loop variable, and relies on this variable's destructor to
      // format the record and flush it to the output stream. The for loop is
      // braceless so that any '<<' expressions following the macro will be attached
      // to the streaming statement inside the loop. Finally, the for loop is
      // designed such that it will execute for one iteration if the given filter
      // passes, and zero iterations otherwise.
      //
      // Example usage:
      //
      //   HCL_LOG_WITH_FILTER(false, eHCL_LOG_LEVEL::INFO, aGlobalLoggerPtr) << "My favorite number is: " << 42 << '\n';
#define HCL_LOG_WITH_FILTER(filter, lvl, aGlobLogPtr) \
         for (il::logger::macrotype_LogRecord macrovar_record(lvl, aGlobLogPtr); \
               !(filter) && macrovar_record.i == 0; \
               ++macrovar_record.i) \
          macrovar_record.messageBuffer

#define HCL_LOG_WITH_LEVEL_FILTER(lvl, aGlobLogPtr) HCL_LOG_WITH_FILTER(lvl < aGlobLogPtr.lock()->getLevel(), lvl, aGlobLogPtr)

#ifdef LOG_ALL_CONTEXT
#   define HCL_TRACE_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::TRACE, aGlobLogPtr) << __LINE__ << ":" << __FUNCTION__ << ":"
#   define HCL_DEBUG_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::DEBUG, aGlobLogPtr) << __LINE__ << ":" << __FUNCTION__ << ":"
#   define HCL_INFO_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::INFO, aGlobLogPtr) << __LINE__ << ":" << __FUNCTION__ << ":"
#   define HCL_WARN_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::WARN, aGlobLogPtr) << __LINE__ << ":" << __FUNCTION__ << ":"
#   define HCL_ERROR_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::ERROR, aGlobLogPtr) << __LINE__ << ":" << __FUNCTION__ << ":"
#   define HCL_FATAL_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::FATAL, aGlobLogPtr) << __LINE__ << ":" << __FUNCTION__ << ":"
#else
#   define HCL_TRACE_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::TRACE, aGlobLogPtr)
#   define HCL_DEBUG_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::DEBUG, aGlobLogPtr)
#   define HCL_INFO_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::INFO, aGlobLogPtr)
#   define HCL_WARN_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::WARN, aGlobLogPtr)
#   define HCL_ERROR_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::ERROR, aGlobLogPtr)
#   define HCL_FATAL_LOGGER(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::FATAL, aGlobLogPtr)
#endif

      // for stifling context info for places where it doesn't make sense - even when compiling with it on
#define HCL_TRACE_LOGGER_NOCONTEXT(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::TRACE, aGlobLogPtr)
#define HCL_DEBUG_LOGGER_NOCONTEXT(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::DEBUG, aGlobLogPtr)
#define HCL_INFO_LOGGER_NOCONTEXT(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::INFO, aGlobLogPtr)
#define HCL_WARN_LOGGER_NOCONTEXT(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::WARN, aGlobLogPtr)
#define HCL_ERROR_LOGGER_NOCONTEXT(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::ERROR, aGlobLogPtr)
#define HCL_FATAL_LOGGER_NOCONTEXT(aGlobLogPtr) HCL_LOG_WITH_LEVEL_FILTER(il::logger::FATAL, aGlobLogPtr)

      IADSLIB_EXPORT void Set_Global_Log_Level(eHCL_LOG_LEVEL level, GlobalLogger* aGlobalLogger);

      IADSLIB_EXPORT void Init_Loggers(const std::string& format, GlobalLogger* aGlobalLogger);
   } // namespace logger
} // namespace il (iadsLib)

#endif

