// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "logger.hpp"

il::logger::eHCL_LOG_LEVEL il::logger::GlobalLogger::getLevel()
{
   std::lock_guard<std::mutex> lock(instanceMutex);
   return level;
}

void il::logger::GlobalLogger::setLevel(il::logger::eHCL_LOG_LEVEL aLevel)
{
   std::lock_guard<std::mutex> lock(instanceMutex);
   level = aLevel;
}

void il::logger::GlobalLogger::setFormat(const il::logger::LogFormat& aFormat)
{
   std::lock_guard<std::mutex> lock(instanceMutex);
   format = aFormat;
}

static const std::string logLevelToString(il::logger::eHCL_LOG_LEVEL aLevel)
{
   std::string logLevelString = "";
   switch (aLevel)
   {
      case il::logger::eHCL_LOG_LEVEL::TRACE:
         logLevelString = "trace";
         break;
      case il::logger::eHCL_LOG_LEVEL::DEBUG:
         logLevelString = "debug";
         break;
      case il::logger::eHCL_LOG_LEVEL::INFO:
         logLevelString = "info";
         break;
      case il::logger::eHCL_LOG_LEVEL::WARN:
         logLevelString = "warning";
         break;
      case il::logger::eHCL_LOG_LEVEL::ERROR:
         logLevelString = "error";
         break;
      case il::logger::eHCL_LOG_LEVEL::FATAL:
         logLevelString = "fatal";
         break;
      case il::logger::eHCL_LOG_LEVEL::OFF:
         break;
      default:
         assert(false);
   }
   return logLevelString;
}

static void writeRecordToStream(const il::logger::LogFormat& aFormat,
                                std::ostream& aStream,
                                il::logger::eHCL_LOG_LEVEL aLevel,
                                const std::string& aMessage)
{
   const char* constantStringBegin = aFormat.constantStrings.data();
   for (size_t i = 0; i < aFormat.items.size(); ++i)
   {
      il::logger::LoggerItem item = aFormat.items[i];
      switch (item.type)
      {
         case il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_CONSTANT_STRING:
         {
            size_t strLength = item.constantStringLength;
            aStream.write(constantStringBegin, strLength);
            constantStringBegin += strLength;
         }
         break;
         case il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_MESSAGE:
            aStream << aMessage;
            break;
         case il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_SEVERITY:
            aStream << logLevelToString(aLevel);
            break;
         default:
            assert(false);
      }
   }
   aStream << '\n';
   aStream.flush();
}

void il::logger::GlobalLogger::writeRecord(il::logger::eHCL_LOG_LEVEL aLevel,
                                           const std::string& aMessage)
{
   std::lock_guard<std::mutex> lock(instanceMutex);
   writeRecordToStream(format, std::cout, aLevel, aMessage);
}

il::logger::macrotype_LogRecord::macrotype_LogRecord(il::logger::eHCL_LOG_LEVEL aLevel, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger) :
   level(aLevel),
   mGlobalLogger(aGlobalLogger)
{
   i = 0;
   messageBuffer.setf(std::ios::boolalpha | std::ios::fixed);
   messageBuffer.precision(2);
}

il::logger::macrotype_LogRecord::~macrotype_LogRecord()
{
   // i will equal zero if the log item is filtered. If we do not
   // check for this case, a blank line will be printed.
   if (i != 0)
   {
      // stringstream::str copies its data. This is wasteful since
      // this string is just copied again to an output stream.
      std::string message = messageBuffer.str();
      mGlobalLogger.lock()->writeRecord(level, message);
   }
}

static inline void addConstantString(il::logger::LogFormat& aFormat, const std::string& aStr)
{
   il::logger::LoggerItem item = {};
   item.type = il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_CONSTANT_STRING;
   item.constantStringLength = aStr.size();
   aFormat.items.push_back(item);
   aFormat.constantStrings += aStr;
}

void il::logger::Set_Global_Log_Level(eHCL_LOG_LEVEL aLevel, il::logger::GlobalLogger* aGlobalLogger)
{
   aGlobalLogger->setLevel(aLevel);
}

static il::logger::LogFormat parseFormatString(const std::string& aStr)
{
   il::logger::LogFormat format;
   if (aStr.empty())
   {
      return format;
   }

   format.constantStrings.reserve(aStr.size());
   format.items.reserve(10);

   size_t strBegin = 0;

   std::string constantString;
   constantString.reserve(aStr.size());

   bool inItem = false;
   std::string itemName;
   itemName.reserve(aStr.size());
   size_t i = 0;
   while (i < aStr.size())
   {
      char c = aStr[i];
      ++i;

      if (inItem)
      {
         if (c == '%')
         {
            size_t strEnd = i - 1;
            // check that there is at least one character in the item name
            assert(strEnd != strBegin);

            il::logger::LoggerItem item = {};

            // set the item type based on the item's string name
            itemName.assign(aStr, strBegin, strEnd - strBegin);
            if (itemName == "Message")
            {
               item.type = il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_MESSAGE;
            }
            else if (itemName == "Severity")
            {
               item.type = il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_SEVERITY;
            }
            else
            {
               //TODO error handling
               assert(false);
               item.type = il::logger::eHCL_LOG_ITEM_TYPE::LOG_ITEM_CONSTANT_STRING;
               item.constantStringLength = 0;
            }

            format.items.push_back(item);

            inItem = false;
            strBegin = i;
         }
      }
      else if (c == '%')
      {
         // append constant string characters up to and excluding the '%'
         size_t strEnd = i - 1;
         constantString.append(aStr, strBegin, strEnd - strBegin);

         // test for the sequence '%%', which escapes the '%' character
         if (i < aStr.size() && aStr[i] == '%')
         {
            constantString += '%';
            ++i;
            strBegin = i;
         }
         else
         {
            // append the constant string item to the item list
            if (!constantString.empty())
            {
               addConstantString(format, constantString);
               constantString.clear();
            }

            // begin reading an item name
            inItem = true;
            strBegin = i;
         }
      }
   }

   if (inItem)
   {
      //TODO error
      assert(false);
   }

   // append the final constant string
   constantString.append(aStr, strBegin, std::string::npos);
   if (!constantString.empty())
   {
      addConstantString(format, constantString);
   }

   return format;
}

void il::logger::Init_Loggers(const std::string& aFormatStr, il::logger::GlobalLogger* aGlobalLogger)
{
   il::logger::LogFormat format = parseFormatString(aFormatStr);
   aGlobalLogger->setFormat(format);
}
