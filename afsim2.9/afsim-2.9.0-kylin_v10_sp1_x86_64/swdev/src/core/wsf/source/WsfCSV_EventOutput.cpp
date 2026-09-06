// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfCSV_EventOutput.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfSimulation.hpp"

void WSF_EXPORT Register_csv_event_output(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("csv_event_output"))
   {
      using ScenarioExtension = wsf::event::output::ScenarioExtension<WsfCSV_EventOutputData, WsfCSV_EventOutput>;
      aApplication.RegisterExtension("csv_event_output",
                                     ut::make_unique<WsfDefaultApplicationExtension<ScenarioExtension>>());
   }
}

std::mutex WsfCSV_EventOutputData::sCriticalSection;

// static
void WsfCSV_EventOutputData::AddDataTags(const std::string& aEventName, const std::vector<std::string>& aDataTags)
{
   std::lock_guard<std::mutex> lock(sCriticalSection);
   mDataTags[aEventName] = aDataTags;
}

// static
void WsfCSV_EventOutputData::AppendDataTags(std::vector<std::string>& aLHS, const std::vector<std::string>& aRHS)
{
   aLHS.insert(aLHS.end(), aRHS.begin(), aRHS.end());
}

const std::vector<std::string> WsfCSV_EventOutputData::mMessageDataTags = {
   // standard message header
   "time<time>",
   "event<string>",
   "platform<string>",
   "side<string>",
   "comm<string>",
   "message_serial_number<int>",
   "data_tag<double>",
   "message_type<string>",
   "message_size<int>",
   "queue_size<int>",
   "comment<string>",
   // Status Message
   "message_status<string>",
   "request_id<string>",
   "system<string>",
   "message_platform<string>",
   // Track Id
   "track_id<string>"};

const std::vector<std::string> WsfCSV_EventOutputData::mTrackDataBriefTags = {"start_time<time>",
                                                                              "update_time<time>",
                                                                              "update_count<int>",
                                                                              "quality<double>",
                                                                              "domain<string>",
                                                                              "track_type<string>",
                                                                              "is_candidate<string>",
                                                                              "is_false_target<string>"};

const std::vector<std::string> WsfCSV_EventOutputData::mTrackDataTags = {
   "start_time<time>",
   "update_time<time>",
   "update_count<int>",
   "quality<double>",
   "domain<string>",
   "track_type<string>",
   "is_candidate<string>",
   "is_false_target<string>",
   "target_platform<string>",
   "target_type<string>",
   "target_side<string>",
   "originator_lat<lat>",
   "originator_lon<lon>",
   "originator_alt<double>",
   "originator_x<double>",
   "originator_y<double>",
   "originator_z<double>",
   "track_lat<lat>",
   "track_lon<lon>",
   "track_alt<double>",
   "track_x<double>",
   "track_y<double>",
   "track_z<double>",
   "location_valid<string>",
   "is_3D<double>",
   "range_valid<string>",
   "bearing_valid<string>",
   "elevation_valid<string>",
   "truth_lat<lat>",
   "truth_lon<lon>",
   "truth_alt<double>",
   "truth_x<double>",
   "truth_y<double>",
   "truth_z<double>",
   "difference_distance<double>",
   "track_speed<double>",
   "track_heading<angle>",
   "track_truth_speed<double>",
   "track_truth_heading<angle>",
   "track_range<double>",
   "track_bearing<angle>",
   "track_elevation<angle>",
   "track_truth_range<double>",
   "track_truth_bearing<angle>",
   "track_truth_elevation<angle>",
   "range_error<double>",
   "bearing_error<double>",
   "elevation_error<double>",
   "track_type_id<string>",
   "track_side<string>",
   "signal_to_noise<double>",
   "pixel_count<int>",
   "frequency_count<int>",
   "frequency_low_#<double>",
   "frequency_high_#<double>", /*"measurement_covariance<double>",
                                  "state_covariance<double>",
                                  "residual_covariance<double>",*/
   "aux_data<string>"};

const std::vector<std::string> WsfCSV_EventOutputData::mEM_InteractionTags = {
   // PrintEM_AbsXmtrRcvrData (Xmtr) (Rcvr)
   "xmtr_type<string>",
   "xmtr_lat<lat>",
   "xmtr_lon<lon>",
   "xmtr_alt<double>",
   "xmtr_x<double>",
   "xmtr_y<double>",
   "xmtr_z<double>",
   "xmtr_heading<angle>",
   "xmtr_pitch<angle>",
   "xmtr_roll<angle>",
   "xmtr_speed<double>",
   "rcvr_type<string>",
   "rcvr_lat<lat>",
   "rcvr_lon<lon>",
   "rcvr_alt<double>",
   "rcvr_x<double>",
   "rcvr_y<double>",
   "rcvr_z<double>",
   "rcvr_heading<angle>",
   "rcvr_pitch<angle>",
   "rcvr_roll<angle>",
   "rcvr_speed<double>",
   // PrintEM_AbsTargetData (tgt)
   "target_type<string>",
   "target_lat<lat>",
   "target_lon_loc<lon>",
   "target_alt<double>",
   "target_x<double>",
   "target_y<double>",
   "target_z<double>",
   "target_heading<angle>",
   "target_pitch<angle>",
   "target_roll<angle>",
   "target_speed<double>",
   // PrintEM_RelXmtrRcvrData (Xmtr->Rcvr) (Rcvr->Xmtr)
   "xmtr->rcvr_range<double>",
   "xmtr->rcvr_bearing<angle>",
   "xmtr->rcvr_elev<angle>",
   "xmtr->rcvr_apparent_bearing<angle>",
   "xmtr->rcvr_apparent_elev<angle>",
   "rcvr->xmtr_range<double>",
   "rcvr->xmtr_bearing<angle>",
   "rcvr->xmtr_elev<angle>",
   "rcvr->xmtr_apparent_bearing<angle>",
   "rcvr->Xmtr_apparent_elev<angle>",
   // PrintEM_RelXmtrRcvrData (Xmtr->Tgt)
   "xmtr->tgt_range<double>",
   "xmtr->tgt_bearing<angle>",
   "xmtr->tgt_elev<angle>",
   "xmtr->tgt_apparent_bearing<angle>",
   "xmtr->tgt_apparent_elev<angle>",
   // PrintEM_RelTargetData (Tgt->Xmtr)
   "tgt->xmtr_range<double>",
   "tgt->xmtr_bearing<angle>",
   "tgt->xmtr_elev<angle>",
   "tgt->xmtr_apparent_bearing<angle>",
   "tgt->xmtr_apparent_elev<angle>",
   // PrintEM_RelXmtrRcvrData (Rcvr->Tgt)
   "rcvr->tgt_range<double>",
   "rcvr->tgt_bearing<angle>",
   "rcvr->tgt_elev<angle>",
   "rcvr->tgt_apparent_bearing<angle>",
   "rcvr->tgt_apparent_elev<angle>",
   // PrintEM_RelTargetData (Tgt->Rcvr)
   "tgt->rcvr_range<double>",
   "tgt->rcvr_bearing<angle>",
   "tgt->rcvr_elev<angle>",
   "tgt->rcvr_apparent_bearing<angle>",
   "tgt->rcvr_apparent_elev<angle>",
   // PrintEM_BeamData (Xmtr) (Rcvr)
   "xmtr_beam_bearing<angle>",
   "xmtr_beam_elevation<angle>",
   "xmtr_rel_az<angle>",
   "xmtr_rel_el<angle>",
   "xmtr_EBS_Az<angle>",
   "xmtr_EBS_El<angle>",
   "xmtr_Omega<angle>",
   "xmtr_gain<double>",
   "rcvr_beam_bearing<angle>",
   "rcvr_beam_elevation<angle>",
   "rcvr_rel_az<angle>",
   "rcvr_rel_el<angle>",
   "rcvr_EBS_Az<angle>",
   "rcvr_EBS_El<angle>",
   "rcvr_Omega<angle>",
   "rcvr_gain<double>",
   "RCS<double>",
   "RCS_(linear)<double>",
   "RCS_azimuth<angle>",
   "RCS_elev<angle>",
   "Optical_Sig<double>",
   "Optical_Sig_(linear)<double>",
   "Optical_Sig_azimuth<angle>",
   "Optical_Sig_elev<angle>",
   "Optical_Sig_Reflectivity<double>",
   "IR_Sig<double>",
   "IR_Sig_azimuth<angle>",
   "IR_Sig_elev<angle>",
   "BackgroundRadiantIntensity<double>",
   "ContrastRadiantIntensity<double>",
   "Transmittance<double>",
   "AbsorptionFactor<double>",
   "AbsorptionFactor(linear)<double>",
   "PropagationFactor_F^4<double>",
   "PropagationFactor(linear)<double>",
   "PropagationFactor^0.25<double>",
   "masking_factor<double>",
   "TransmittedPower<double>",
   "ReceivedPower<double>",
   "ReceiverNoisePower<double>",
   "ClutterPower<double>",
   "InterferencePower(linear)<double>",
   "empty_1<string>",
   "empty_2<string>",
   "empty_3<string>",
   "pixel_count<int>",
   "SignalToNoise<double>",
   "DetectionThreshold<double>",
   "SignalPower/noisePower<double>",
   "SignalPower/(noisePower+clutterPower)<double>",
   "SignalPower/(noisePower+clutterPower+interferencePower)<double>",
   "empty_4<string>",
   "failure_reason<string>"};

std::map<std::string, std::vector<std::string>> WsfCSV_EventOutputData::mDataTags;

// =================================================================================================
WsfCSV_EventOutput* WsfCSV_EventOutput::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfCSV_EventOutput*>(aSimulation.FindExtension("csv_event_output"));
}

// ===================================================================================================
void WsfCSV_EventOutput::WriteHeader(const std::string& aEventName)
{
   if (mCSV_Data.mDataTags.find(aEventName) != mCSV_Data.mDataTags.end())
   {
      StreamRef() << "! " << aEventName;
      for (const auto& tag : mCSV_Data.mDataTags[aEventName])
      {
         StreamRef() << ',' << tag;
      }
      StreamRef() << '\n';
   }
   else if (GetSimulation().GetApplication().IsTestingEnabled())
   {
      // In the future, the inclusion of the "-FAIL-" tag should be automatic.
      ut::log::info("test_fail") << "-FAIL- WsfCSV_EventOutput::WriteHeader: Missing CSV header for event "
                                 << aEventName << ".";
   }
}

void WsfCSV_EventOutput::EnableOrDisableEventP(const std::string& aEventName, bool aEnable)
{
   if (mCSV_Data.mInsertDataTags && aEnable)
   {
      WriteHeader(aEventName);
   }
}

void WsfCSV_EventOutput::PrintEvent(const wsf::event::Result& aResult) const
{
   aResult.PrintCSV(StreamRef());
}

// =================================================================================================
WsfCSV_EventOutputData::WsfCSV_EventOutputData()
   : wsf::event::output::Data()
{
   // Modify default values
   mSettings.SetTimeFormat(UtTime::FmtS + 5 + UtTime::FmtShowPoint + UtTime::FmtNoSuffix);
   mSettings.SetLatLonFormat(UtAngle::FmtD + 6);

   std::vector<std::string> systemOffOnEvent = {"time<time>",
                                                "event<string>",
                                                "platform<string>",
                                                "side<string>",
                                                "type<string>",
                                                "system<string>",
                                                "system_type<string>",
                                                "lat<lat>",
                                                "lon<lon>",
                                                "alt<double>",
                                                "x<double>",
                                                "y<double>",
                                                "z<double>",
                                                "heading<double>",
                                                "pitch<double>",
                                                "roll<double>",
                                                "ned_speed<double>",
                                                "ned_velocity_n<double>",
                                                "ned_velocity_e<double>",
                                                "ned_velocity_d<double>",
                                                "eci_speed<double>",
                                                "eci_velocity_x<double>",
                                                "eci_velocity_y<double>",
                                                "eci_velocity_z<double>",
                                                "ned_acceleration<double>",
                                                "ned_acceleration_n<double>",
                                                "ned_acceleration_e<double>",
                                                "ned_acceleration_d<double>",
                                                "eci_acceleration<double>",
                                                "eci_acceleration_x<double>",
                                                "eci_acceleration_y<double>",
                                                "eci_acceleration_z<double>"};

   AddDataTags("BTREE_NODE_CHILDREN",
               {"time<time>",
                "event<string>",
                "file_path<string>",
                "platform<string>",
                "node_id<string>",
                "node_type<string>",
                "children_count<int>",
                "child_id_#<int>"});
   AddDataTags("BTREE_NODE_EXEC",
               {"time<time>",
                "event<string>",
                "platform<string>",
                "side<string>",
                "node_id<string>",
                "node_type<string>",
                "exec_state<bool>",
                "failure_reason<string>"});

   AddDataTags("COMM_FREQUENCY_CHANGED",
               {"time<time>",
                "event<string>",
                "platform<string>",
                "side<string>",
                "comm_network<string>",
                "lat<lat>",
                "lon<lon>",
                "alt<double>",
                "x<double>",
                "y<double>",
                "z<double>",
                "frequency_id<int>",
                "frequency<double>"});
   {
      // COMM_STATUS
      AddDataTags("COMM_BROKEN", systemOffOnEvent);
      AddDataTags("COMM_NON_OPERATIONAL", systemOffOnEvent);
      AddDataTags("COMM_OPERATIONAL", systemOffOnEvent);
      AddDataTags("COMM_TURNED_OFF", systemOffOnEvent);
      AddDataTags("COMM_TURNED_ON", systemOffOnEvent);
   }
   AddDataTags("COMMENT", {"time<time>", "event<string>", "platform<string>", "comment<string>"});
   AddDataTags("COMM_ADDED_TO_MANAGER", {"platform<string>", "comm<string>"});
   AddDataTags("COMM_REMOVED_FROM_MANAGER", {"platform<string>", "comm<string>"});
   AddDataTags("COMM_ADDED_TO_LOCAL",
               {"local_platform<string>", "local_router<string>", "protocol_type<string>", "address<string>"});
   AddDataTags("COMM_REMOVED_FROM_LOCAL",
               {"local_platform<string>", "local_router<string>", "protocol_type<string>", "address<string>"});

   AddDataTags("CRASHED_INTO_GROUND", {"time<time>", "event<string>", "platform<string>", "side<string>"});

   AddDataTags("EXCHANGE_COMPLETED",
               {"time<time>",
                "event<string>",
                "event_id<string>",
                "caller<string>",
                "requester<string>",
                "responder<double>",
                "request_id<string>",
                "item_id<string>",
                "desired_amount<double>",
                "offered_amount<double>",
                "transferred_amount<double>",
                "container<string>",
                "qty_amount<double>"});

   AddDataTags("EXCHANGE_QUERIED",
               {"time<time>",
                "event<string>",
                "query_type<string>",
                "caller<string>",
                "requester<string>",
                "responder<double>",
                "serial_request_id<string>",
                "item_id<string>",
                "desired_amount<double>",
                "offered_amount<double>",
                "negotiated_amount<double>"});
   AddDataTags("EXCHANGE_NEGOTIATED",
               {"time<time>",
                "event<string>",
                "negotiation_type<string>",
                "caller<string>",
                "requester<string>",
                "responder<double>",
                "serial_request_id<string>",
                "item_id<string>",
                "desired_amount<double>",
                " offered_amount<double>",
                "negotiated_amount<double>"});
   AddDataTags("EXCHANGE_REQUEST_FAILED",
               {"time<time>",
                "event<string>",
                "event_id<string>",
                "requester<double>",
                "responder<double>",
                "request_id<string>",
                "item_id<string>",
                "desired_qty<double>"});

   AddDataTags("EXECUTE_CALLBACK", {"time<time>", "event<string>", "platform<string>", "side<string>", "callback<string>"});

   AddDataTags("FUEL_EVENT", {"time<time>", "event<string>", "platform<string>", "side<string>", "event_type<string>"});

   {
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       "side<string>",
                                       "local_track_id<string>",
                                       "non_local_track_id<string>"};
      AddDataTags("LOCAL_TRACK_CORRELATION", temp);
      AddDataTags("LOCAL_TRACK_DECORRELATION", temp);
   }

   AddDataTags("LINK_ADDED_TO_MANAGER",
               {"source_platform<string>",
                "source_comm<string>",
                "source_address<string>",
                "destination_platform<string>",
                "destination_comm<string>",
                "destination_address<string>"});
   AddDataTags("LINK_REMOVED_FROM_MANAGER",
               {"source_platform<string>",
                "source_comm<string>",
                "source_address<string>",
                "destination_platform<string>",
                "destination_comm<string>",
                "destination_address<string>"});
   AddDataTags("LINK_ENABLED_ON_MANAGER",
               {"source_platform<string>",
                "source_comm<string>",
                "source_address<string>",
                "destination_platform<string>",
                "destination_comm<string>",
                "destination_address<string>"});
   AddDataTags("LINK_DISABLED_ON_MANAGER",
               {"source_platform<string>",
                "source_comm<string>",
                "source_address<string>",
                "destination_platform<string>",
                "destination_comm<string>",
                "destination_address<string>"});
   AddDataTags("LINK_ADDED_TO_LOCAL",
               {"local_platform<string>",
                "local_router<string>",
                "protocol_type<string>",
                "source_address<string>",
                "destination_address<string>"});
   AddDataTags("LINK_REMOVED_FROM_LOCAL",
               {"local_platform<string>",
                "local_router<string>",
                "protocol_type<string>",
                "source_address<string>",
                "destination_address<string>"});
   AddDataTags("LINK_ENABLED_ON_LOCAL",
               {"local_platform<string>",
                "local_router<string>",
                "protocol_type<string>",
                "source_address<string>",
                "destination_address<string>"});
   AddDataTags("LINK_DISABLED_ON_LOCAL",
               {"local_platform<string>",
                "local_router<string>",
                "protocol_type<string>",
                "source_address<string>",
                "destination_address<string>"});

   {
      // LOCAL_TRACK_INITDROPUPDATE
      std::vector<std::string> temp = {"time<time>", "event<string>", "platform<string>", "side<string>", "track_id<string>"};
      std::vector<std::string> localTrackDropped = temp;
      AppendDataTags(localTrackDropped, {"target_platform<string>", "target_type<string>"});
      AddDataTags("LOCAL_TRACK_DROPPED", localTrackDropped);

      AppendDataTags(temp, mTrackDataTags);
      AppendDataTags(temp, {"sensor_track_id<string>", "sensor<string>", "sensor_type<string>", "sensor_mode<string>"});
      AddDataTags("LOCAL_TRACK_INITIATED", temp);
      AddDataTags("LOCAL_TRACK_UPDATED", temp);
   }
   //{
   //   // LOCAL_TRACK_STATUS
   //   temp = { "time<time>", "track_action<double>", "platform<string>", "side<string>", "target<string>",
   //   "track_id<string>" }; AppendDataTags(temp, mTrackDataTags); AddDataTags("LOCAL_TRACK_STATUS", temp);
   //}

   {
      // MESSAGE_DELIVERY_ATTEMPT
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "transmitter<string>",
                                       "transmitter_side<string>",
                                       "receiver<string>",
                                       "receiver_system<string>",
                                       "message_serial_number<int>",
                                       "data_tag<double>",
                                       "message_type<string>",
                                       "message_size<int>",
                                       "data_rate<double>",
                                       "noise_density<double>",
                                       "bit_error_rate<double>"};
      AppendDataTags(temp, mEM_InteractionTags);
      AppendDataTags(temp, {"success_failure<double>"});
      AddDataTags("MESSAGE_DELIVERY_ATTEMPT", temp);
   }
   AddDataTags("MESSAGE_FAILED_ROUTING",
               {"time<time>",
                "event<string>",
                "transmitter<string>",
                "transmitter_side<string>",
                "comm<string>",
                "message_serial_number<int>",
                "data_tag<double>",
                "message_type<string>",
                "message_size<double>",
                "receiver<string>",
                /*"queue_size<double>",*/ "comment<string>",
                "message_status<string>",
                "request_id<string>",
                "system<string>",
                "message_platform<string>",
                "track_id<string>",
                "update_time<time>",
                "update_count<int>",
                "track_quality<double>",
                "track_type<string>"});
   { // MESSAGE_HOP
      std::vector<std::string> tempHop = {"time<time>",
                                          "event<string>",
                                          "receiver<string>",
                                          "receiver_side<string>",
                                          "receiver_system<string>",
                                          "message_serial_number<int>",
                                          "data_tag<double>",
                                          "message_type<string>",
                                          "message_size<int>",
                                          "destination<string>",
                                          "destination_system<string>"};
      AppendDataTags(tempHop, mMessageDataTags);
      AddDataTags("MESSAGE_HOP", tempHop);
   }
   {
      // MESSAGE_XMTD_RCVD_QUEUED_DISCARDED
      std::vector<std::string> temp = {};
      AppendDataTags(temp, mMessageDataTags);
      AppendDataTags(temp, mTrackDataTags);
      AddDataTags("MESSAGE_DISCARDED", temp);
      AddDataTags("MESSAGE_QUEUED", temp);
      AddDataTags("MESSAGE_RECEIVED", temp);
      AddDataTags("MESSAGE_TRANSMITTED", temp);
      AddDataTags("MESSAGE_TRANSMITTED_HEARTBEAT", temp);
      AddDataTags("MESSAGE_TRANSMIT_ENDED", temp);
   }
   {
      // MESSAGE_UPDATED
      std::vector<std::string> temp = {};
      AppendDataTags(temp, mMessageDataTags);
      AppendDataTags(temp, mTrackDataBriefTags);
      // just about a copy of mMessageDataTags with prefixes
      AppendDataTags(temp,
                     {"new_serial_number<int>",
                      "new_data_tag<double>",
                      "new_message_type<string>",
                      "new_message_size<int>",
                      "new_queue_size<int>",
                      "new_comment<string>",
                      "new_message_status<string>",
                      "new_request_id<string>",
                      "new_system<string>",
                      "new_message_platform<string>"});
      // mTrackDataBriefTags with prefixes
      AppendDataTags(temp,
                     {"new_start_time<time>",
                      "new_update_time<time>",
                      "new_update_count<int>",
                      "new_quality<double>",
                      "new_domain<string>",
                      "new_track_type<string>",
                      "new_is_candidate<string>",
                      "new_is_false_target<string>"});
      AddDataTags("MESSAGE_UPDATED", temp);
   }

   {
      // MOVER_STATUS
      AddDataTags("MOVER_BROKEN", systemOffOnEvent);
      AddDataTags("MOVER_BURNED_OUT", systemOffOnEvent);
      AddDataTags("MOVER_NON_OPERATIONAL", systemOffOnEvent);
      AddDataTags("MOVER_OPERATIONAL", systemOffOnEvent);
      AddDataTags("MOVER_TURNED_OFF", systemOffOnEvent);
      AddDataTags("MOVER_TURNED_ON", systemOffOnEvent);
   }
   AddDataTags("MOVER_STAGED", systemOffOnEvent);
   // AddDataTags("MOVER_STATUS", { "time", "event", "platform", "side" });

   AddDataTags("NAVIGATION_STATUS_CHANGED",
               {"time<time>", "event<string>", "platform<string>", "side<string>", "status<string>"});

   AddDataTags("NETWORK_ADDED", {"time<time>", "event<string>", "network<string>", "address<string>"});

   AddDataTags("NETWORK_REMOVED", {"time<time>", "event<string>", "network<string>", "address<string>"});

   AddDataTags("OPERATING_LEVEL_CHANGED",
               {"time<time>",
                "event<string>",
                "platform<string>",
                "side<string>",
                "processor<string>",
                "processor_name<string>",
                "level<int>"});

   AddDataTags("PLATFORM_APPEARANCE_CHANGED",
               {"time<time>",
                "event<string>",
                "platform<string>",
                "side<string>",
                "item_type<string>",
                "concealment_factor<double>",
                "class_name<string>",
                "signature_state<string>",
                "scale_factor<double>"});
   AddDataTags("PLATFORM_CAPABILITY_CHANGED",
               {"time<time>", "event<string>", "platform<string>", "side<string>", "item_type<string>", "toggle<bool>"});
   {
      // PLATFORM_STATUS
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       "side<string>",
                                       "type<string>",
                                       "ps<double>",
                                       "lat<lat>",
                                       "lon<lon>",
                                       "alt<double>",
                                       "x<double>",
                                       "y<double>",
                                       "z<double>",
                                       "heading<angle>",
                                       "pitch<angle>",
                                       "roll<angle>",
                                       "ned_speed<double>",
                                       "ned_velocity_n<double>",
                                       "ned_velocity_e<double>",
                                       "ned_velocity_d<double>",
                                       "eci_speed<double>",
                                       "eci_velocity_x<double>",
                                       "eci_velocity_y<double>",
                                       "eci_velocity_z<double>",
                                       "ned_acceleration<double>",
                                       "ned_acceleration_n<double>",
                                       "ned_acceleration_e<double>",
                                       "ned_acceleration_d<double>",
                                       "eci_acceleration<double>",
                                       "eci_acceleration_x<double>",
                                       "eci_acceleration_y<double>",
                                       "eci_acceleration_z<double>"};
      AddDataTags("PLATFORM_ADDED", temp);
      AddDataTags("PLATFORM_INITIALIZED", temp);
      AddDataTags("PLATFORM_BROKEN", temp);
      AddDataTags("PLATFORM_KILLED", temp);
      AddDataTags("PLATFORM_DELETED", temp);
      AddDataTags("PLATFORM_OMITTED", temp);
   }

   {
      // PROCESSOR_STATUS
      AddDataTags("PROCESSOR_BROKEN", systemOffOnEvent);
      AddDataTags("PROCESSOR_NON_OPERATIONAL", systemOffOnEvent);
      AddDataTags("PROCESSOR_OPERATIONAL", systemOffOnEvent);
      AddDataTags("PROCESSOR_TURNED_OFF", systemOffOnEvent);
      AddDataTags("PROCESSOR_TURNED_ON", systemOffOnEvent);
   }

   {
      // ROUTER_STATUS
      AddDataTags("ROUTER_BROKEN", systemOffOnEvent);
      AddDataTags("ROUTER_NON_OPERATIONAL", systemOffOnEvent);
      AddDataTags("ROUTER_OPERATIONAL", systemOffOnEvent);
      AddDataTags("ROUTER_TURNED_OFF", systemOffOnEvent);
      AddDataTags("ROUTER_TURNED_ON", systemOffOnEvent);
   }

   {
      // SENSOR_DETECTION
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       /*"sensor_platform<string>",*/ "side<string>",
                                       "target_platform<string>",
                                       "sensor<string>",
                                       "mode<string>",
                                       "beam_index<double>"};
      AppendDataTags(temp, mEM_InteractionTags);
      AppendDataTags(temp, {"failures<string>", "Pd<double>", "required_Pd<double>", "detected<bool>"});
      AddDataTags("SENSOR_DETECTION_ATTEMPT", temp);
      AddDataTags("SENSOR_DETECTION_CHANGED", temp);
   }
   {
      // SENSOR_STATUS
      AddDataTags("SENSOR_BROKEN", systemOffOnEvent);
      AddDataTags("SENSOR_NON_OPERATIONAL", systemOffOnEvent);
      AddDataTags("SENSOR_OPERATIONAL", systemOffOnEvent);
      AddDataTags("SENSOR_TURNED_OFF", systemOffOnEvent);
      AddDataTags("SENSOR_TURNED_ON", systemOffOnEvent);
   }
   AddDataTags("SENSOR_FREQUENCY_CHANGED",
               {"time<time>",
                "event<string>",
                "platform<string>",
                "side<string>",
                "sensor<string>",
                "mode<string>",
                "lat<lat>",
                "lon<lon>",
                "alt<double>",
                "x<double>",
                "y<double>",
                "z<double>",
                "transmitter_count<int>",
                "beam_number_#<int>",
                "frequency_id_#<int>",
                "frequency_#<double>"});
   {
      // SENSOR_MODE_STATUS
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       "side<string>",
                                       "name<string>",
                                       "mode<string>",
                                       "lat<lat>",
                                       "lon<lon>",
                                       "alt<double>",
                                       "x<double>",
                                       "y<double>",
                                       "z<double>"};
      AddDataTags("SENSOR_MODE_ACTIVATED", temp);
      AddDataTags("SENSOR_MODE_DEACTIVATED", temp);
   }
   {
      // SENSOR_REQUEST_STATUS
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       "side<string>",
                                       "sensor<string>",
                                       "mode<string>",
                                       "track_id<string>"};
      AppendDataTags(temp, mTrackDataTags);
      AddDataTags("SENSOR_REQUEST_CANCELED", temp);
      AddDataTags("SENSOR_REQUEST_INITIATED", temp);
      AddDataTags("SENSOR_REQUEST_UPDATED", temp);
   }
   // AddDataTags("SENSOR_STATUS", { "time", "event", "platform", "name", "type" });
   {
      // SENSOR_TRACK_STATUS
      std::vector<std::string> temp =
         {"time<time>", "event<string>", "platform<string>", "side<string>", "sensor<string>", "track_id<string>"};
      std::vector<std::string> sensorTrackDropped = temp;
      AppendDataTags(sensorTrackDropped, {"target_platform<string>", "target_type<string>"});
      AddDataTags("SENSOR_TRACK_DROPPED", sensorTrackDropped);

      AppendDataTags(temp, mTrackDataTags);
      AddDataTags("SENSOR_TRACK_COASTED", temp);
      AddDataTags("SENSOR_TRACK_INITIATED", temp);
      AddDataTags("SENSOR_TRACK_UPDATED", temp);
   }
   {
      // STATE_STATUS
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       "side<string>",
                                       "target_platform<string>",
                                       "state<string>",
                                       "processor<string>",
                                       "lat<lat>",
                                       "lon<lon>",
                                       "alt<double>",
                                       "x<double>",
                                       "y<double>",
                                       "z<double>"};
      AddDataTags("STATE_ENTRY", temp);
      AddDataTags("STATE_EXIT", temp);
   }

   // SIMULATION
   {
      std::vector<std::string> temp =
         {"time<time>", "event<string>", "year<int>", "month<int>", "day<int>", "hour<int>", "minute<int>", "second<int>"};
      AddDataTags("SIMULATION_STARTING", temp);
      AddDataTags("SIMULATION_COMPLETE", temp);
   }

   AddDataTags(
      "TANKING_EVENT",
      {"time<time>", "event<string>", "tanker<string>", "tanker_side<string>", "receiver<string>", "event_type<string>"});

   {
      // TASK_STATUS
      std::vector<std::string> temp = {"time<time>",
                                       "event<string>",
                                       "platform<string>",
                                       "assigner_platform<string>",
                                       "task_type<string>",
                                       "task_status<string>",
                                       "task_assign_time<time>",
                                       "task_update_time<time>",
                                       "resource_name<string>",
                                       "task_target<string>",
                                       "track_id<string>"};
      AppendDataTags(temp, mTrackDataTags);
      AppendDataTags(temp, {"assignee_speed<double>", "assignee_bearing<double>"});
      AddDataTags("TASK_CANCELED", temp);
      AddDataTags("TASK_COMPLETED", temp);
      AddDataTags("TASK_ASSIGNED", temp);
   }

   AddDataTags("TEAM_NAME_DEFINITION", {"time<time>", "event<string>", "platform<string>", "side<string>"});
}

// =================================================================================================
bool WsfCSV_EventOutputData::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command;
   aInput.GetCommand(command);
   if (command == "insert_data_tags")
   {
      aInput.ReadValue(mInsertDataTags);
   }
   else
   {
      myCommand = wsf::event::output::Data::ProcessInput(aInput);
   }
   return myCommand;
}
