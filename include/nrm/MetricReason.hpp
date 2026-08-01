#ifndef NRM_METRIC_REASON_HPP
#define NRM_METRIC_REASON_HPP

namespace nrm
{
enum class MetricReason
{
   cNONE,
   cNO_SAMPLES,
   cMISSING_TRANSMIT_DENOMINATOR,
   cMISSING_LIFECYCLE_CORRELATION,
   cMISSING_STATE_HISTORY,
   cMISSING_ESTABLISHMENT_EVENTS,
   cPROFILE_NOT_CONFIGURED,
   cPROFILE_INVALID,
   cOUT_OF_ORDER_EVENT,
   cOUTPUT_OPEN_FAILED,
   cOUTPUT_WRITE_FAILED
};

inline const char* ToString(MetricReason aReason)
{
   switch (aReason)
   {
   case MetricReason::cNONE: return "NONE";
   case MetricReason::cNO_SAMPLES: return "NO_SAMPLES";
   case MetricReason::cMISSING_TRANSMIT_DENOMINATOR: return "MISSING_TRANSMIT_DENOMINATOR";
   case MetricReason::cMISSING_LIFECYCLE_CORRELATION: return "MISSING_LIFECYCLE_CORRELATION";
   case MetricReason::cMISSING_STATE_HISTORY: return "MISSING_STATE_HISTORY";
   case MetricReason::cMISSING_ESTABLISHMENT_EVENTS: return "MISSING_ESTABLISHMENT_EVENTS";
   case MetricReason::cPROFILE_NOT_CONFIGURED: return "PROFILE_NOT_CONFIGURED";
   case MetricReason::cPROFILE_INVALID: return "PROFILE_INVALID";
   case MetricReason::cOUT_OF_ORDER_EVENT: return "OUT_OF_ORDER_EVENT";
   case MetricReason::cOUTPUT_OPEN_FAILED: return "OUTPUT_OPEN_FAILED";
   case MetricReason::cOUTPUT_WRITE_FAILED: return "OUTPUT_WRITE_FAILED";
   }
   return "NO_SAMPLES";
}
} // namespace nrm

#endif
