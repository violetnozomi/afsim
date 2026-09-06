.. ****************************************************************************
.. CUI//REL TO USA ONLY
..
.. The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
..
.. The use, dissemination or disclosure of data in this file is subject to
.. limitation or restriction. See accompanying README and LICENSE for details.
.. ****************************************************************************

.. _ag_processors_helios_processors:

IADS C2 Processors
==================

Overview
^^^^^^^^^
	IADS C2 processors can be used to quickly put together a flexible, configurable command and control (C2) system for use in an integrated air defense system (IADS). Five main types of processor make up the IADS C2 model set: asset managers, C2 disseminate processors, battle managers, weapons managers, and sensors managers. A complete system will need to deploy all five of these processors in their respective roles. Asset managers are primarily responsible for handling the perceived state of its own asset as well as that of other assets in its command chain. By default, asset managers are also responsible for commanding all other IADS C2 processors on board a platform to execute their behaviors. C2 dissemination processors are responsible for routing IADS C2 messages to other platforms. Battle managers are responsible for creating and monitoring assignments. Sensors managers are responsible for cuing sensors to produce weapon quality tracks for assigned targets. Weapons managers are responsible for handling engagements against assigned targets.

	.. note:: All five types of IADS C2 processors are meant to be deployed in unison in an IADS system. IADS C2 processors are meant to work together, and a solitary IADS C2 processor will likely not function as intended.

Example IADS C2 Setup
^^^^^^^^^^^^^^^^^^^^^^

		.. _ag_processors_fig1:

	.. figure:: /docs/images/processors_images/processor_image1.png
		 :width: 1500px
		 :align: center
		 :height: 1125px
		 :figclass: align-center

		 **Figure 12.1: IADS C2 processor placement in an example IADS.**

	This graphic depicts an example IADS and an example setup of IADS C2 processors on each platform. In this example, the High Level Command Post (HLCP) has a Battle Manager (BM) configured with Commit Authority (CA). The Intermediate Level Command Post (ILCP) has a BM configured to not have commit authority. The Battalion has a Sensors Manager (SM), and each of the SAMs has a Weapons Manager (WM). All the platforms on the weapons side of the IADS have an Asset Manager (AM) and a Disseminate C2 (DC2) processor. Importantly, all the platforms also have a Track Processor which maintains a master track list.

		.. _ag_processors_fig2:

	.. figure:: /docs/images/processors_images/processor_image2.png
		 :width: 3036px
		 :align: center
		 :height: 1124px
		 :figclass: align-center

		 **Figure 12.2 IADS C2 message flow in an example IADS.**

	This graphic depicts the IADS system outfitted with IADS C2 managers shown in Figure 12.1 with the flow of IADS C2 messages. The EW platform sends tracks to the HLCP via the onboard **Track Processor**, from which **assignments** may be generated if the BM has CA. The HLCP BM with CA creates engagement assignments from the received tracks, which the DC2 forwards with the associated **assignment track updates** along the command chain to the assigned subordinates. The BM (without CA) on the ILCP re-evaluates the assignments it receives (for its subordinates) from the HLCP and reassigns **delegate assignments**, if necessary; the ILCP's DC2 sends the delegate assignment messages along its command chain to the assigned subordinates. The SM on the Battalion **cues** the TTR sensors to obtain higher-quality tracks for targets assigned to weapon platforms, and the DC2 passes the assignments on to the assigned SAMs. Once the TTRs have established tracks, the updated tracks are passed as **assignment track updates** to the SAMs via the Battalion DC2. Once a track of sufficient quality has been established, the WM on the assigned SAM executes the engagement against the assigned target. **Combat status messages** and **assignment status messages** are not pictured, but are also sent between IADS C2 system assets.

	.. note:: All platforms employing a battle manager, sensors manager, and/or weapons manager, and all platforms in a command chain between platforms deploying any of these managers should be "C2 capable". Pertaining to IADS C2, "C2 capable" refers to a platform which deploys at least an asset manager, a C2 dissemination processor, and a track processor which maintains a master track list.

.. _ag_processors_helios_processors_wsf_battle_manager:

Battle Management C2 - WSF_BATTLE_MANAGER
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	IADS C2 battle managers are responsible for creating and adjusting assignments on tracks to be carried out by an IADS system. Specifically, a top-level battle manager, with commit authority, is responsible for making assignments on tracks according to a specific weapon allocation scheme. Battle managers without commit authority are responsible for reassessing and reassigning assignments that pass through them to their subordinates. Battle managers are also responsible for dealing with assignment status updates and monitoring assignments they have created. Each IADS C2 command chain structure should have at least a top-level battle manager with commit authority, and the surveillance side of an IADS should be configured to report tracks to the top-level battle manager. See WSF_UNCLASS_BM for an implementation of WSF_BATTLE_MANAGER.

	.. note:: WSF_BATTLE_MANAGER is a base class from which HELIOS-based battle managers inherit. It is not meant to be used as a processor on a platform in it's own right; instead, other battle manager processors inherit from it to provide common functionality found in all IADS C2 battle managers.

Battle Management Commands
"""""""""""""""""""""""""""

	The **commit_authority** command allows the user to specify whether or not a battle manager has the ability to create an assignment from a track. If an IADS C2 battle manager has commit authority, it is considered a top-level battle manager and whenever it receives a new track, it will assess its subordinate weapons to determine if there are any suitable to be assigned against the target track. If there are one or more weapons suitable for the track, the battle manager will create an assignment message and send it to the assigned weapon along with the track which triggered the assignment. If an IADS C2 battle manager does not have commit authority, it is considered a delegate battle manager and will reassess and reassign assignments which pass through it on the way to its subordinate weapons but cannot create new assignments from tracks.

	.. note:: There should be exactly one battle manager with commit authority for each IADS C2 command chain structure.

	The **project_tracks_by_delays** command allows the user to specify whether or not tracks are projected by assignment delay prior to assessment for assignment creation. In essence, by projecting tracks forward by delays, the battle manager is able to anticipate and counteract some delays from the weapon and intermediate C2 nodes when making assessments and creating assignments.

	The **project_targets_forward** **by** commands allow the user to configure forward projection times for weapon assessment and assignment purposes. The project_targets_forward command sets the maximum forward time projection, not including projection due to anticipated delays, by which tracks will be projected forward in a weapon assessment to find an intercept solution. The by command sets the increment by which projection times are increased.

	The **engage_iff_permissions** block containing the **unknowns**, **neutrals**, **friendlies**, and **hostiles** commands allow the user to specify which types of tracks the battle manager will be allowed to engage by assigning against. Any is friend or foe (IFF) type with a true value will allow the battle manager to engage the corresponding category of tracks.

	Example usage of WSF_BATTLE_MANAGER commands:

	.. _ag_processors_snippet_36:
 
	.. code-block::	rest	

		platform iads_commander WSF_PLATFORM
		   commander SELF
		   ...
		   add processor battle_manager WSF_UNCLASS_BM
		      ...
		      commit_authority          enable
		      project_tracks_by_delays  true
		      project_tracks_forward    1 minute
		      by                        10 seconds
		
		      engage_iff_permissions
		         unknowns               false
		         neutrals               false
		         friendlies             false
		         hostiles               true
		      end_engage_iff_permissions
		      ...
		   end_processor
		   ...
		end_platform
		
		platform delegate_commander WSF_PLATFORM
		   commander iads_commander
		   ...
		   add processor battle_manager WSF_UNCLASS_BM
		      ...
		      commit_authority          disable
		      project_tracks_by_delays  true
		      project_tracks_forward    1 minute by 10 seconds
		
		      engage_iff_permissions
		         hostiles               true
		      end_engage_iff_permissions
		      ...
		   end_processor
		   ...
		end_platform

	Snippet 12.36: Battle Management Commands

Required and Default Values
""""""""""""""""""""""""""""
	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - commit_authority
		  - No*
		  - <boolean>
		  - false
		  
		* - project_tracks_by_delays
		  - No
		  - <boolean>
		  - true
		  
		* - project_targets_forward
		  - No
		  - <time-value>
		  - 60 seconds
		  
		* - by
		  - No
		  - <time-value>
		  - 10 seconds

	Engage IFF Permissions Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - unknowns
		  - No
		  - <boolean>
		  - false
		  
		* - neutrals
		  - No
		  - <boolean>
		  - false
		  
		* - friendlies
		  - No
		  - <boolean>
		  - false
		  
		* - hostiles
		  - No
		  - <boolean>
		  - false

	.. note:: * At least one battle manager must have commit authority for assignments to be made.

.. _ag_processors_helios_processors_wsf_unclass_bm:

WSF_UNCLASS_BM
^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_UNCLASS_BM is an unclassified implementation of the WSF_BATTLE_MANAGER base class. This is the generic unclassified battle manager for use in scenarios. WSF_UNCLASS_BM will perform the required roles of an IADS C2 battle manager, assessing tracks (if the BM has commit authority) or assignments (if the BM does not have commit authority), handling assignment status messages, and monitoring assignments. WSF_UNCLASS_BM also allows user configuration of shot doctrine, max firing time, stale asset time, and print settings, as well as weapon allocation schemes via weapon rows.

Battle Manager Processing
""""""""""""""""""""""""""
	The WSF_UNCLASS_BM has a specific process for assigning weapons. First, combat ready (status green) subordinate weapons are organized into prioritization groups which favor direct subordinate weapons and weapons utilizing engagement zones. The processor then iterates through the weapon arrays to find the highest scoring weapon which matches a BM's weapon row for the target type and subtype in the most highly prioritized weapon array with a successful predicted intercept according to the weapon's weapons manager. The weapon score is calculated according to the weapon engagement's predicted workload, intercept range, intercept time, workload, and probability of kill, according to the scoring factors specified in the BM's weapon rows for matching target type and subtype. The WSF_UNCLASS_BM also manages existing assignments by canceling assignments which have surpassed the max firing time prior to firing and CANTCOing allocations which cannot be properly resolved as assignments. For more details as to the exact process that goes into making assignments, see the wiki entry for this processor.

Unclass Battle Manager Commands
""""""""""""""""""""""""""""""""

	The **shot_doctrine** command allows the user to specify the shot doctrine to be used when creating assignments. Options are Shoot-1, Shoot-2, and Shoot-Look-Shoot. Shoot-1 calls for a single shot to be fired. Shoot-2 calls for two shots to be fired in immediate succession. Shoot-Look-Shoot calls for a single shot, then after the first has detonated, if the target remains, a second shot to be fired.

	The **max_firing_time** command allows the user to specify the maximum firing time allowed for a weapon to fire on an assigned target prior to the battle manager canceling the assignment.

	The **stale_asset_time** command allows the user to specify the amount of time after which the battle manager will consider a subordinate stale, and no longer consider for assignment until updated. For example, if the stale asset time is 60 seconds, a weapon must have sent a status update to the battle manager within the past 60 seconds to be considered for assignment.

	The **print_settings** block containing the **valid_units**, **valid_unit_details**, and **invalid_unit_details** commands allow the user to configure the logging of weapon evaluations. Valid_units controls logging of basic evaluation information for evaluations of valid weapons; valid_unit_details controls logging of detailed information for evaluations of valid weapons; invalid_unit_details controls logging of detailed information for evaluations of invalid weapons.

	Example usage of WSF_UNCLASS_BM commands:

	.. _ag_processors_snippet_37:
 
	.. code-block::	rest	

		platform iads_commander WSF_PLATFORM
		   commander SELF
		   ...
		   add processor battle_manager WSF_UNCLASS_BM
		      ...
		      commit_authority          true
		      project_tracks_by_delays  true
		      project_tracks_forward    1 minute
		      by                        10 seconds
		      shot_doctrine             Shoot-2
		      max_firing_time           360 seconds
		      stale_asset_time          60 seconds
		
		      engage_iff_permissions
		         unknowns               false
		         neutrals               false
		         friendlies             false
		         hostiles               true
		      end_engage_iff_permissions
		
		      print_settings
		         valid_units            true
		         valid_unit_details     true
		         invalid_unit_details   false
		      end_print_settings
		      ...
		   end_processor
		   ...
		end_platform

	Snippet 12.37: WSF_UNCLASS_BM Commands

	.. note:: WSF_UNCLASS_BM inherits all commands available to WSF_BATTLE_MANAGER.

Weapon Table and Weapon Scoring Commands
"""""""""""""""""""""""""""""""""""""""""
	The WSF_UNCLASS_BM **weapon_table** block allows user configuration of **weapon_rows**, which define priorities for assignment of SAMs, AIs, and other weapons, as well as minimum probability of kill for targets of specified type and subtype which fall into user configurable bins of speed and altitude attributes.

	Inside the **weapon_table** block, the **speed_attributes** and **altitude_attributes** blocks and the command **attrib** inside those blocks allow for user definition of speed and altitude ranges for use in weapon rows, such that tracks of the same type and subtype may be handled differently based on altitude and speed.

	The WSF_UNCLASS_BM **scoring_factors** block allows user configuration of scoring factor weights in weapon scoring evaluations. The factors are **weapon_type**, **weapon_subtype**, **intercept_range**, **intercept_time**, **pk**, and **workload** and the factor values range from 0 to 10. Higher factor weights result in greater weight being applied to the corresponding scoring factor.

	Example usage of WSF_UNCLASS_BM weapon_table and scoring factors commands:

	.. _ag_processors_snippet_38:
 
	.. code-block::	rest	

		platform iads_commander WSF_PLATFORM
		   ...
		   add processor battle_manager WSF_UNCLASS_BM
		      ...
		      weapon_table
		         speed_attributes
		            attrib slow 0 kts < 500 kts
		            attrib fast 500 kts < 1000 kts
		         end_speed_attributes
		
		         altitude_attributes
		            attrib low 0 ft < 2000 ft
		            attrib high 2000 ft < 10000 ft
		         end_altitude_attributes
		
		         weapon_rows
		            target_pref priority 1 "UCAV" "Any" fast low  0.5 1 0 0
		            target_pref priority 2 "UCAV" "Any" fast high 0.7 1 1 0
		            target_pref priority 3 "UCAV" "Any" slow low  0.7 1 0 0
		            target_pref priority 4 "UCAV" "Any" slow high 0.9 0 1 0
		         end_weapon_rows
		      end_weapon_table
		
		      scoring_factors
		         weapon_type     10
		         weapon_subtype   7
		         intercept_range  7
		         intercept_time   3
		         pk              10
		         workload         5
		      end_scoring_factors
		      ...
		   end_processor
		...
		end_platform

	Snippet 12.38: WSF_UNCLASS_BM Weapon Table and Weapon Scoring Commands

Required and Default Values
""""""""""""""""""""""""""""

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - shot_doctrine
		  - No
		  - [Shoot-1 | Shoot-2 | Shoot-Look-Shoot]
		  - Shoot-Look-Shoot
		  
		* - max_firing_time
		  - No
		  - <time-value>
		  - 360 seconds
		  
		* - stale_asset_time
		  - No
		  - <time-value>
		  - 60 seconds

	Print Settings Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - valid_units
		  - No
		  - <boolean>
		  - false
		  
		* - valid_unit_details
		  - No
		  - <boolean>
		  - false
		  
		* - invalid_unit_details
		  - No
		  - <boolean>
		  - false

	Weapon Table Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - speed_attributes attrib
		  - Yes
		  - <speed-value> [ < <= ] <speed-value>
		  - none
		  
		* - altitude_attributes attrib
		  - Yes
		  - <length-value> [ < <= ] <length-value>
		  - none
		  
		* - weapon_rows target_pref
		  - Yes
		  - priority <integer> <string> <string> [speed attrib] [altitude attrib] <real> <integer> <integer> <integer>
		  - none

	Scoring Factors Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - weapon_type
		  - No
		  - <integer>
		  - 1
		  
		* - weapon_subtype
		  - No
		  - <integer>
		  - 1
		  
		* - intercept_range
		  - No
		  - <integer>
		  - 1
		  
		* - intercept_time
		  - No
		  - <integer>
		  - 1
		  
		* - pk
		  - No
		  - <integer>
		  - 1
		  
		* - workload
		  - No
		  - <integer>
		  - 1

.. _ag_processors_helios_processors_wsf_asset_manager:

WSF_ASSET_MANAGER
^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	IADS C2 asset managers are responsible for providing common IADS status, track, and assignment interfaces. An asset manager is responsible for reporting its own status, receiving the status of its subordinate units, and reporting an aggregate status, as well as creating, managing, and logging a map of fellow asset status perceptions. In addition, asset managers (and subsystems) perform weapon-to-target assignment for tracks stored by the unit, and  monitor and assess existing assignments. Asset managers can be configured to add assignment and decision update delays. Asset managers are also responsible for initiating the periodic processing of other IADS C2 processors on the asset manager's platform. Each C2-capable platform in an IADS C2 system should have an asset manager. See WSF_UNCLASS_ASSET_MANAGER for an implementation of WSF_ASSET_MANAGER.

	.. note:: WSF_ASSET_MANAGER is a base class from which HELIOS-based asset managers inherit. It is not meant to be used as a processor on a platform in it's own right; instead, other asset manager processors inherit from it to provide common functionality found in all IADS C2 asset managers.

Asset Manager Commands
"""""""""""""""""""""""

	The **filter_dead_tracks** command allows the user to specify whether or not to use truth data to ensure track targets are not dead prior to inclusion in the asset manager's threat array. 

	The **max_track_grouping_distance** command allows the user to specify grouping distance for track strength determinations. Tracks are not merged if grouped, solely incremented in strength count.

	The **max_assignments** command allows the user to specify the maximum total number of active assignments for this asset and its subordinates.

	The **assignment_delay** command allows the user to specify a delay to be added prior to processing assignments.

	The **decision_update_delay** command allows the user to specify a delay to be added prior to routing C2 messages.

	The **log_status** command allows the user to specify whether or not the asset manager will attempt to log its and its subordinates' statuses every time step.

	The **status_settings** block contains the following commands, which pertain to asset status.

	The **subord_yellow_timeout** command allows the user to specify the amount of time without a status update to wait prior to setting a subordinate status to yellow.

	The **subord_red_timeout** command allows the user to specify the amount of time without a status update to wait prior to setting a subordinate status to red.

	The **report_position_every** **or_every** commands allow the user to specify how often position messages should be transmitted by units of distance traveled or time passed.

	The **report_status_every** command allows the user to specify how often status messages should be transmitted.

	The **aggregate_unit_status** command allows the user to specify whether or not to consider subordinates for meeting the requirements of status green.

	The **stationary_opns_only** command allows the user to require the asset to be stationary in order to be status green.

	The **weapon_required** command allows the user to specify whether the asset requires a weapon with munitions on its platform (or a subordinate platform if aggregating unit status) for status green.

	The **require_all_weapons** command allows the user to specify whether the asset requires all its weapons (or all its subordinate weapons if aggregating unit status) to have munitions for status green.

	The **ew_required** command allows the user to specify whether a sensor of category EW is required on the asset (or a subordinate if aggregating unit status) for status green.

	The **tar_required** command allows the user to specify whether a sensor of category TAR is required on the asset (or a subordinate if aggregating unit status) for status green.

	The **ttr_required** command allows the user to specify whether a sensor of category TTR is required on the asset (or a subordinate if aggregating unit status) for status green.

	Example usage of WSF_ASSET_MANAGER commands:

	.. _ag_processors_snippet_39:
 
	.. code-block::	rest	

		platform iads_commander WSF_PLATFORM
		   ...
		   add processor asset_manager WSF_UNCLASS_ASSET_MANAGER
		      update_interval 1 second
		
		      max_assignments              100
		      assignment_delay             1 second
		      decision_update_delay        1 second
		      filter_dead_tracks           false
		      max_track_grouping_distance  50 meters
		      log_status                   true
		
		      status_settings
		         subord_yellow_timeout  120 seconds
		         subord_red_timeout     180 seconds
		         report_position_every  100 meters
		         or_every               300 seconds
		         report_status_every    10 seconds
		         aggregate_unit_status  true
		         stationary_opns_only   false
		         weapon_required        true
		         require_all_weapons    false
		         ew_required            false
		         tar_required           true
		         ttr_required           true
		      end_status_settings
		   end_processor
		   ...
		end_platform

	Snippet 12.39: WSF_ASSET_MANAGER Commands

	.. note:: The update interval of the asset manager must be set for any IADS C2 processors to function correctly.

Required and Default Values
""""""""""""""""""""""""""""

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - filter_dead_tracks
		  - No
		  - <boolean>
		  - false
		  
		* - max_track_grouping_distance
		  - No
		  - <length-value>
		  - 50 meters
		  
		* - max_assignments
		  - No*
		  - <integer>
		  - 0
		  
		* - assignment_delay
		  - No
		  - <time-value>
		  - 0 seconds
		  
		* - decision_update_delay
		  - No
		  - <time-value>
		  - 0 seconds
		  
		* - log_status
		  - No
		  - <boolean>
		  - true

	Status Settings Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - subord_yellow_timeout
		  - No
		  - <time-value>
		  - 15 seconds
		  
		* - subord_red_timeout
		  - No
		  - <time-value>
		  - 30 seconds
		  
		* - report_position_every
		  - No
		  - <length-value>
		  - 100 meters
		  
		* - or_every
		  - No
		  - <time-value>
		  - 300 seconds
		  
		* - report_status_every
		  - No
		  - <time-value>
		  - 10 seconds
		  
		* - aggregate_unit_status
		  - No
		  - <boolean>
		  - false
		  
		* - stationary_opns_only
		  - No
		  - <boolean>
		  - false
		  
		* - weapon_required
		  - No
		  - <boolean>
		  - false
		  
		* - require_all_weapons
		  - No
		  - <boolean>
		  - false
		  
		* - ew_required
		  - No
		  - <boolean>
		  - false
		  
		* - tar_required
		  - No
		  - <boolean>
		  - false
		  
		* - ttr_required
		  - No
		  - <boolean>
		  - false

.. _ag_processors_helios_processors_wsf_unclass_asset_manager:

WSF_UNCLASS_ASSET_MANAGER
^^^^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_UNCLASS_ASSET_MANAGER is an unclassified implementation of the WSF_ASSET_MANAGER base class. This is the generic unclassified asset manager for use in scenarios. See the entry for WSF_ASSET_MANAGER for commands.

.. _ag_processors_helios_processors_wsf_disseminate_c2:

WSF_DISSEMINATE_C2
^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	IADS C2 disseminate C2 processors disseminate assignment, status, position, and track messages to connected units based on the criteria specified by the user. It is meant to be used in concert with the other IADS C2 processors for sending messages outbound from those processors. Each C2-capable platform in an IADS C2 system should have a disseminate C2 processor. See WSF_UNCLASS_DISSEMINATE_C2 for an implementation of WSF_DISSEMINATE_C2.

	.. note:: WSF_DISSEMINATE_C2 is a base class from which HELIOS-based C2 dissemination processors inherit. It is not meant to be used as a processor on a platform in it's own right; instead, other C2 dissemination processors inherit from it to provide common functionality found in all IADS C2 C2 dissemination processors.

Disseminate C2 Commands
""""""""""""""""""""""""

	The **routing_style** command allows the user to specify how the processor should route dynamically-routed directed messages. next_unit causes messages to be sent to the next unit on the way to their destination. next_c2 causes messages to be sent to the next C2 capable unit on the way to their destination. direct causes messages to be sent directly to their destination.

	The **routing_table** block allows the user to configure the sending of outbound messages by the processor per message type. Each command in the routing table corresponds to a single type of message and adds a set of intended recipients for that message type. Options for each message type are peer, commander, dynamic, and none. Peers are fellow subordinate assets of the asset's commander along the default command chain. The commander is the asset's direct commander along the default command chain. Dynamic routing causes directed messages to be sent according to the specified routing style toward the destination asset; dynamic routing has no effect in the case of non-directed messages. None causes that type of message to not be sent out by the disseminate C2 processor. The recognized message types are detailed below.

	**track_updates** are non-directed messages sent to update track information. Track message handling should generally be handled by the track processing systems, not IADS C2 systems.

	**assign_track_updates** are directed messages sent to update track information pertaining to an assignment. Assignment track messages should generally be sent dynamically.

	**assignments** are directed messages sent to command a weapon to engage a target. Assignment messages should generally be sent dynamically.

	**assignment_status** messages are directed messages sent to update assets about the status of an assignment. Assignment status messages should generally be sent dynamically.

	**assignment_cancel** messages are directed messages sent to inform assets about a decision to cancel an assignment. Assignment cancel messages should generally be sent dynamically.

	**sensor_cue** messages are directed messages sent to cue a sensor to start tracking a target. Sensor cue messages should generally be sent dynamically.

	**status** messages are non-directed messages sent to update assets about another asset's status. Status messages should generally be sent to commanders.

	Example usage of WSF_DISSEMINATE_C2 commands:

	.. _ag_processors_snippet_40:
 
	.. code-block::	rest	

		platform battalion WSF_PLATFORM
		   ...
		   add processor dissemination_manager WSF_UNCLASS_DISSEMINATE_C2
		      routing_style next_c2
		
		      routing_table
		         track_updates         none
		         assign_track_updates  dynamic
		         assignments           dynamic
		         assignment_status     dynamic
		         assignment_cancel     dynamic
		         sensor_cue            dynamic
		         status                commander
		      end_routing_table
		   end_processor
		   ...
		end_platform

	Snippet 12.40: WSF_DISSEMINATE_C2 Commands

Required and Default Values
""""""""""""""""""""""""""""

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - routing_style
		  - No
		  - [ next_unit | next_c2 | direct ]
		  - next_c2

	Routing Table Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - track_updates
		  - No
		  - [ peer | commander | dynamic | none ]
		  - none
		  
		* - assign_track_updates
		  - No
		  - [ peer | commander | dynamic | none ]
		  - dynamic
		  
		* - assignments
		  - No
		  - [ peer | commander | dynamic | none ]
		  - dynamic
		  
		* - assignment_status
		  - No
		  - [ peer | commander | dynamic | none ]
		  - dynamic
		  
		* - assignment_cancel
		  - No
		  - [ peer | commander | dynamic | none ]
		  - dynamic
		  
		* - sensor_cue
		  - No
		  - [ peer | commander | dynamic | none ]
		  - dynamic
		  
		* - status
		  - No
		  - [ peer | commander | dynamic | none ]
		  - none

.. _ag_processors_helios_processors_wsf_unclass_disseminate_c2:

WSF_UNCLASS_DISSEMINATE_C2
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_UNCLASS_DISSEMINATE_C2 is an unclassified implementation of the WSF_DISSEMINATE_C2 base class. This is the generic unclassified C2 dissemination processor for use in scenarios. See the entry for WSF_DISSEMINATE_C2 for commands.

.. _ag_processors_helios_processors_wsf_sensors_manager:

WSF_SENSORS_MANAGER
^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	IADS C2 sensor managers are responsible for managing Target Acquisition Radar (TAR) and Target Tracking Radar (TTR) sensors either located on the asset employing the sensor manager or on subordinate assets. Sensor managers are responsible for turning sensors on and off at appropriate times, cuing sensors to track targets which have been assigned to be engaged, canceling cues at appropriate times, and CANTCOing assignments when cued sensors are unable to track the assigned target within an acceptable time frame. Sensor managers should generally be placed on the lowest level platform which hosts on board or is commander to both the weapons manager of the weapon and the sensors used to establish tracks for the weapon. Sensors managers should not have subordinates that also deploy sensors managers unless the subordinate sensor managers are on a platform which employs on board both a weapons manager and the sensors used to establish tracks for that weapon. Some example valid and invalid configurations are shown below in Figure 12.3. See WSF_SIMPLE_SENSORS_MANAGER and WSF_SENSORS_MANAGER_FOV for implementations of WSF_SENSORS_MANAGER.

		.. _ag_processors_fig3:

	.. figure:: /docs/images/processors_images/processor_image3.png
		 :width: 742px
		 :align: center
		 :height: 771px
		 :figclass: align-center

		 **Figure 12.3: IADS C2 sensors manager valid and invalid deployment schemes**

	.. note:: WSF_SENSORS_MANAGER is a base class from which HELIOS-based sensors managers inherit. It is not meant to be used as a processor on a platform in it's own right; instead, other sensors manager processors inherit from it to provide common functionality found in all IADS C2 sensors managers.

	.. note:: Sensors managed by a sensors manager should be configured with the command "category TAR" or "category TTR", but should not be in more than one of the following categories: EW, TAR, TTR, RWR.

	.. note:: Subordinate sensors managed by a sensors manager should be configured to report tracks to the platform with a sensors manager.

Sensors Manager Processing
"""""""""""""""""""""""""""
	When a platform with a sensors manager receives an assignment message for an onboard or subordinate weapon, the following process occurs:
	
		1. The sensor manager ensures it has a handle to a master track that matches the target of the assignment.
		2. If a master track is found, it is checked for source/quality. The next sensor type cued for the assignment is dependent on the current track quality. The order of track quality is as follows in ascending order: EW, TAR, TTR. Therefore, if the track's highest quality contributor is an EW track, the sensor manager will attempt to cue a TAR on the target. If there is no TAR available to cue, a ttr will then be attempted.
		3. Once the desired sensor type is determined, the sensors manager will attempt to find and cue a sensor of the desired type using the following algorithm:
			a. First attempt to find an available sensor of the desired type on the assigned platform.
			b. Next attempt to find an available sensor of the desired type on the sensor manager's platform and subordinate platforms which do not deploy weapons managers.
		4. If the sensors manager was able to cue a radar, the sensors manager will then add a responsibility for that assignment to its own list of responsible assignments.
		5. The sensors manager will then periodically check the current state of the assignment to determine if the cued sensor has located the target, and if so, repeat the above process to determine if a higher quality sensor can be cued to track the target. If so, the sensors manager will also determine if it is time to turn off the sensor last active for the assignment, and will do so once a higher quality sensor has established its own track.
		6. Once the sensor of the highest available quality has been cued, the sensors manager simply monitors the assignment and waits for completion. Once the assignment has been completed, the sensors manager removes its responsibility for the assignment. The sensors manager also determines if it is time to turn off the sensor last active for the assignment.
		
	The algorithm also cancels cue assignments if the maximum acquisition time configured for the corresponding sensor time has expired prior to the cued sensor establishing a track. If this occurs, the assignment is CANTCO'd and the cue is canceled. An example of sensors manager processing is shown below in Figure 12.4.

		.. _ag_processors_fig4:

	.. figure:: /docs/images/processors_images/processor_image4.png
		 :width: 872px
		 :align: center
		 :height: 2335px
		 :scale: 29%
		 :figclass: align-center

		 **Figure 12.4: Example IADS C2 sensors manager processing**

	.. note:: If the assigned unit for the assignment happened to be the "SAM" unit instead of "SAM 2", the same process would apply, except the SAM Controller would instead use the TAR and the TTR on the "SAM" and would leave the standalone TAR and TTR off for this assignment.

Sensors Manager Commands
"""""""""""""""""""""""""

	The **max_tar_acquisition_time** command allows the user to specify the maximum amount of time to allow a TAR to unsuccessfully search for a target prior to CANTCOing the assignment.

	The **max_ttr_acquisition_time** command allows the user to specify the maximum amount of time to allow a TTR to unsuccessfully search for a target prior to CANTCOing the assignment.

	The **turn_off_tar_if_no_tracks** command allows the user to specify whether or not the sensors manager should turn off a TAR when it is no longer cued to track any targets.

	The **turn_off_ttr_if_no_tracks** command allows the user to specify whether or not the sensors manager should turn off a TTR when it is no longer cued to track any targets.

	The **ttr_tracking_mode_name** command allows the user to specify the name of the mode the TTR will be commanded to use when cued to start tracking a target. If not specified, the first mode of the TTR will be used to start tracking.

	Example usage of WSF_SENSORS_MANAGER commands:

	.. _ag_processors_snippet_41:
 
	.. code-block::	rest	

		platform battalion WSF_PLATFORM
		   ...
		   add processor sensors_manager WSF_SIMPLE_SENSORS_MANAGER
		      max_tar_acquisition_time   60 seconds
		      max_ttr_acquisition_time   60 seconds
		      turn_off_tar_if_no_tracks  false
		      turn_off_ttr_if_no_tracks  true
		      ttr_tracking_mode_name     "ACQUIRE"
		   end_processor
		   ...
		end_platform

	Snippet 12.41: WSF_SENSORS_MANAGER Commands

Required and Default Values
""""""""""""""""""""""""""""

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - max_tar_acquisition_time
		  - No
		  - <time-value>
		  - 60 seconds
		  
		* - max_ttr_acquisition_time
		  - No
		  - <time-value>
		  - 60 seconds
		  
		* - turn_off_tar_if_no_tracks
		  - No
		  - <boolean>
		  - false
		  
		* - turn_off_ttr_if_no_tracks
		  - No
		  - <boolean>
		  - true
		  
		* - ttr_tracking_mode_name
		  - No
		  - <string>
		  - ""

.. _ag_processors_helios_processors_wsf_simple_sensors_manager:

WSF_SIMPLE_SENSORS_MANAGER
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_SIMPLE_SENSORS_MANAGER is an implementation of the WSF_SENSORS_MANAGER base class. This is a generic sensors manager processor for use in scenarios. See the entry for WSF_SENSORS_MANAGER for commands.

.. _ag_processors_helios_processors_wsf_sensors_manager_fov:

WSF_SENSORS_MANAGER_FOV
^^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_SENSORS_MANAGER_FOV is an implementation of the WSF_SENSORS_MANAGER base class and extends WSF_SIMPLE_SENSORS_MANAGER to provide more sophisticated slewing control. It is identical in all other respects. While the simple sensor manager points a TTR directly at assigned targets in an undefined order, the sensor manager field of view slews a TTR to the region of space in which it could see the most targets.

Sensors Manager FOV Processing
"""""""""""""""""""""""""""""""
	The major additional responsibility of the sensors manager field of view is to compute the direction in which to point a TTR by considering its current assignments. This section will refer to an assignment given to a TTR as a target for which a track has not yet been established by the TTR. In computing the cue azimuth, the manager considers the azimuth of the target relative to the sensor, and whether or not the sensor is currently tracking the target. The image in Figure 12.5 is a top-down view of a set of targets around a TTR.

.. _ag_processors_helios_processors_sensors_fov_image:

	.. figure:: /docs/images/processors_images/processor_image5.png
		 :width: 456px
		 :align: center
		 :height: 350px
		 :figclass: align-center

		 **Figure 12.5: TTR managed by a sensors manager FOV with cued targets**

	The wedge containing the tracked targets T8 and T9 represents one possible azimuth of the sensor, where the angle of the wedge represents the width of the sensor beam. For radar sensors, this is typically the half-power beam width; for geometric sensors, it is the azimuth field of view; other sensor types may define this width differently. The cue azimuth is determined by finding the wedge that contains the most tracked targets. Given that two wedges contain the same number of tracked targets, the one with more targets (tracked or not) is chosen. Given that two wedges contain the same number of targets, the one with the denser target grouping (in azimuth) is chosen. Within this wedge, the manager cues the TTR to the middle of the target grouping (more precisely, the mean of all the target azimuths).

	In the image above, T3, T4, and T5 form a dense group that fit into an azimuth wedge; however, the sensor is already busy tracking two targets, T8 and T9, so the TTR is cued such that its beam contains these two targets instead.

	This strategy prevents the TTR from abandoning targets it is already tracking, prioritizes groups with more targets, and favors dense groups over sparse ones.

	The sensors manager FOV performs all duties of the simple sensors manager plus some additional actions for each subordinate TTR. The sensors manager FOV checks if a TTR has any cues. If not, the manager ensures the TTR is turned off, sets its slew rate to the coarse slew rate, and cues it to its resting azimuth. Otherwise, the manager computes the cue azimuth for the TTR, and cues the TTR to this azimuth. If the cued azimuth is within the sensor's current field of view, the manager makes sure the TTR is on, and sets its slew rate to the fine slew rate; otherwise, the manager makes sure the TTR is off, and sets its slew rate to the coarse slew rate.

TTR Requirements
"""""""""""""""""
	The sensors manager field of view takes control of some aspects of its subordinate TTRs. This includes the on/off state; the azimuth_slew_rate and azimuth_cue_rate; and the cued location/angle/target. These aspects should not be modified from script. Doing so will interfere with the operation of the manager and produce odd results. The manager also requires that TTR sensors use a default sensor scheduler; using another scheduler will also produce odd results.

	The sensors manager field of view only cues a TTR sensor to a single location, so it will not operate correctly for sensors with multiple antennas or beams if the antennas/beams can point in multiple directions at the same time. However, it is able to manager multiple sensors present on the same platform.

	The sensors manager field of view queries aux_data defined in subordinate TTRs to discover additional sensor parameters. Note that these parameters are not defined in a sensor manager's aux_data, but in that of a TTR sensor. If an auxiliary parameter is not specified, it will be set to a default value. The following aux_data should be configured on TTRs managed by a sensors manager FOV.

	**unitary RESTING_AZIMUTH =** <angle-value>
		The azimuth to which the TTR will return when it has no more cues. This azimuth is defined in the PCS frame of the TTR (thus, setting this value to 0 degrees points the resting azimuth directly in front of the sensor, 90 degrees points it to the right, -90 to the left, and 180 degrees directly behind). Since this is an azimuth, it should be in the range [-180, 180] degrees. **Default:** 0 degrees

	**unitary COARSE_SLEW_RATE_AZIMUTH =** <angular-speed-value>
		The speed at which the TTR will slew in azimuth when it does not have targets in its field of view. **Default:** Infinity

	**unitary FINE_SLEW_RATE_AZIMUTH =** <angular-speed-value>
		The speed at which the TTR will slew in azimuth when it has targets in its field of view. **Default:** Infinity

	Example TTR aux_data for use with sensors manager FOV:

	.. _ag_processors_snippet_42:
 
	.. code-block::	rest	

		sensor TTR_SENSOR WSF_RADAR_SENSOR
		   ...
		   aux_data
		      unitary RESTING_AZIMUTH = 0.0 deg
		      unitary COARSE_SLEW_RATE_AZIMUTH = 10.0 deg/sec
		      unitary FINE_SLEW_RATE_AZIMUTH = 1.0 deg/sec
		   end_aux_data
		   ...
		end_sensor

	Snippet 12.42: TTR Aux_Data for use with Sensors Manager FOV

.. _ag_processors_helios_processors_wsf_weapons_manager:

WSF_WEAPONS_MANAGER
^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	IADS C2 weapons managers are responsible for managing how weapons prosecute assignments. Weapons managers periodically assess their weapons' assignments to ensure continued suitability (intercept occurs within zones, if required) and capability (weapon has munitions and weapon asset is status green), as well as verifying assignment tracks are of sufficient quality prior to firing. Weapons managers are responsible for handling their managed weapons' engagements, including commanding their managed weapons to fire according to the specified doctrine of the assignment (including shot doctrine) and the configuration of the weapons manager (including applying delays). Weapons managers are also responsible for issuing HAVCO (have completed) and CANTCO (cannot complete) messages when their managed weapons' have completed an assignment or have been found to be incapable of completing an assignment as assigned, respectively. Weapons managers should be placed on C2 capable platforms which employ weapons. See WSF_WEAPONS_MANAGER_SAM and WSF_WEAPONS_MANAGER_AI for implementations of WSF_WEAPONS_MANAGER.

	.. note:: WSF_WEAPONS_MANAGER is a base class from which HELIOS-based weapons managers inherit. It is not meant to be used as a processor on a platform in it's own right; instead, other weapons manager processors inherit from it to provide common functionality found in all IADS C2 weapons managers.

Weapons Manager Commands
"""""""""""""""""""""""""

	The **haveco_reporting_strategy** command allows the user to specify when to consider an engagement complete and assess it to report HAVCO success or failure. On_launch will cause engagements to be considered complete as soon as the shot doctrine of the assignment has been fulfilled; this behavior can cause assignments to be considered complete while shots are still in flight, and therefore may cause multiple unnecessary assignments and shots against a target. On_detonate and on_kill will cause engagements to be considered complete as soon as all shots fired in the engagement have been resolved. On_kill will consider an engagement successful if the target has been killed; on_detonate will consider an engagement successful if the shot doctrine has been fulfilled.

	The **engagement_settings** block contains the following four commands, which pertain to requirements to engage an assigned target.

	The **ew_targets** command allows the user to specify whether the weapons manager will fire at EW quality tracks.

	The **tar_targets** command allows the user to specify whether the weapons manager will fire at TAR quality tracks.

	The **ttr_targets** command allows the user to specify whether the weapons manager will fire at TTR quality tracks.

	The **engage_local_ttr_targets_only** command allows the user to specify whether the weapons manager will fire only at targets being tracked by a local TTR.

	The **delays** block contains the following two commands, which pertain to weapons manager delays.

	The **time_between_engagements** command allows the user to specify a delay between engaging different assignments.

	The **expected_sensor_acquisition** command allows the user to specify an anticipated sensor acquisition time. This allows battle managers to anticipate the time it takes sensors to acquire a track of sufficient quality to engage a target, which allows battle managers to optimize shot timelines around such delays. This delay is more important for SAM weapons managers than AI weapons managers.

	The **wez** block contains the following two commands for defining the weapons manager WEZ library usage.

	The **library** command allows the user to specify the name of the WEZ library.

	The **tactical_range** command allows the user to specify which range specification doctrine to use. Aero represents the intercept range given the target heading and velocity. Max 1 represents the maximum kinematic range of a missile at a given target altitude. Max 2 represents the maximum doctrinal engagement range at a given altitude or the maximum range of the missile, whichever is smaller.

	The **self_defense** block contains commands pertaining to self defense, which has not been implemented.

	Example usage of WSF_WEAPONS_MANAGER commands:

	.. _ag_processors_snippet_43:
 
	.. code-block::	rest	

		platform sam WSF_PLATFORM
		   ...
		   add processor weapons_manager WSF_WEAPONS_MANAGER_SAM
		      haveco_reporting_strategy    on_detonate
		
		      engagement_settings
		         ew_targets                     false
		         tar_targets                    false
		         ttr_targets                    true
		         engage_local_ttr_targets_only  true
		      end_engagement_settings
		
		      delays
		         time_between_engagements     0 seconds
		         expected_sensor_acquisition  30 seconds
		      end_delays
		
		      wez
		         library         none
		         tactical_range  aero
		      end_wez
		
		      self_defense
		         disable
		      end_self_defense
		   end_processor
		   ...
		end_platform

	Snippet 12.43: WSF_WEAPONS_MANAGER Commands

Required and Default Values
""""""""""""""""""""""""""""

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - haveco_reporting_strategy
		  - No
		  - [ on_launch | on_detonate | on_kill ]
		  - on_kill

	Engagement Settings Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - ew_targets
		  - No
		  - <boolean>
		  - false
		  
		* - tar_targets
		  - No
		  - <boolean>
		  - false
		  
		* - ttr_targets
		  - No
		  - <boolean>
		  - true
		  
		* - engage_local_ttr_targets_only
		  - No
		  - <boolean>
		  - true

	Delays Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - time_between_engagements
		  - No
		  - <time-value>
		  - 0 seconds
		  
		* - expected_sensor_acquisition
		  - No
		  - <time-value>
		  - 0 seconds

	WEZ Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - library
		  - No
		  - <string>
		  - ""
		  
		* - tactical_range
		  - No
		  - [ aero | max1 | max2 ]
		  - aero

	Self Defense Commands **Self defense is not currently implemented.**

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - enable
		  - No
		  - 
		  - Disabled
		  
		* - disable
		  - No
		  - 
		  - Disabled
		  
		* - range
		  - No
		  - <length-value>
		  - 0 meters
		  
		* - shot_doctrine
		  - No
		  - [ Shoot-1 | Shoot-2 | Shoot-Look-Shoot ]
		  - Shoot-1
		  
		* - shot_expiry
		  - No
		  - <time-value>
		  - 10 seconds

.. _ag_processors_helios_processors_wsf_weapons_manager_sam:

WSF_WEAPONS_MANAGER_SAM
^^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_WEAPONS_MANAGER_SAM is an implementation of the WSF_WEAPONS_MANAGER base class. This is a generic weapons manager processor for use with SAM platforms in scenarios. See the entry for WSF_WEAPONS_MANAGER for commands.

Intercept Calculators
""""""""""""""""""""""
	IADS C2 battle managers and weapons managers expect SAM systems to have an intercept calculator in AFSIM script matching the signature shown in the example below. The intercept calculator is used by the battle manager for evaluating the weapon for assignment, as well as by the weapons manager to assess the ongoing validity of the assignment.

	Example script intercept calculator on example SAM launcher platform employing a weapons manager:

	.. _ag_processors_snippet_44:
 
	.. code-block::	rest	

		platform sam_launcher WSF_PLATFORM
		   ...
		   add weapon sam WSF_EXPLICIT_WEAPON
		      ...
		   end_weapon
		
		   add processor weapons_manager WSF_WEAPONS_MANAGER_SAM
		   end_processor
		
		   script WsfBMInterceptCalcResult can_intercept(WsfWeapon weapon, WsfTrack track)
		      WsfBMInterceptCalcResult result = WsfBMInterceptCalcResult();
		      result.SetCantIntercept();
		
		      bool canIntercept;
		      double timeToIntercept;
		      double maxWeaponRange;
		      double estimatedProbabilityOfKill;
		      WsfGeoPoint interceptPoint;
		
		      ...
		
		      if (canIntercept)
		      {
		         result.SetCanIntercept(true);
		         result.SetTTI(timeToIntercept);
		         result.SetPredPoint(interceptPoint);
		         result.SetWeaponMaxRange(maxWeaponRange);
		         result.SetEstimatedPK(estimatedProbabilityOfKill);
		      }
		
		      return result;
		   end_script
		   ...
		end_platform

	Snippet 12.44: Example script intercept calculator on example SAM launcher platform employing a weapons manager

.. _ag_processors_helios_processors_wsf_weapons_manager_ai:

WSF_WEAPONS_MANAGER_AI
^^^^^^^^^^^^^^^^^^^^^^^

Overview
"""""""""
	WSF_WEAPONS_MANAGER_AI is an implementation of the WSF_WEAPONS_MANAGER base class. This is a generic weapons manager processor for use with air interceptor platforms in scenarios. See the entry for WSF_WEAPONS_MANAGER for more commands. Air intercept weapons managers are responsible for engagement handling, scrambling, evasion- and engagement-related maneuvers and the use of engagement zones, as well as egressing and the use of egress corridors.

Method of Operation
""""""""""""""""""""
	A Weapons Manager Air Interceptor (WMAI) begins by checking if the user has enabled evasion behaviors. If evasion behaviors have been enabled and the platform has a Radar Warning Receiver (RWR), the WMAI selects the highest priority threat sensed by the RWR that is within the response range and evaluates whether to evade, using either a beam response or drag response, relative to that threat. If the Air Interceptor (AI) is currently performing a beam response (an evasion maneuver that attempts to put the threat on the platform's "beam"), the WMAI checks whether the beam time has elapsed. If so, the WMAI begins a drag response (flying away from the threat at the highest possible speed) relative to the highest priority threat. If not, the current beam response is adjusted, if necessary, so that it is relative to the highest priority threat. If the AI is currently performing a drag response, the drag response is adjusted, if necessary, so that it is being performed relative to the highest priority threat. If the AI is not currently evading, the WMAI begins evading the highest priority threat with a beam response unless the AI is in the process of engaging the unit or the unit is believed dead.

	If the WMAI does not continue or begin an evasion behavior, and if the user has enabled "take action" behaviors, the WMAI next considers whether to take action. "Taking action" involves breaking left, breaking right, or continuing to fly straight for a specific duration. The user can specify the direction and duration for each of four available actions. Which of the four actions will be taken depends on the AI's place in formation, which can also be set by the user. The WMAI will only take action if the AI is currently pursuing or intercepting an assignment. Additionally, during the time that the AI intercepts and then pursues an assignment, it can only take action once. (If the WMAI later receives another assignment against the same target and engages it with a new intercept or pursue behavior, it may take action again relative to that same target.) So, the WMAI will decide to take action relative to the current assignment if: (1) the AI is currently intercepting or pursuing an assignment, (2) the intercept-pursue chain of behaviors has not yet generated a "take action," and (3) the AI is not currently engaging the assignment.

	If the WMAI is not evading or taking action, it takes the following steps for targets that it has been assigned: If the current assignment is complete, the WMAI reports HAVCO and considers the next assignment. If the asset is not combat ready or if it does not have a valid master track for the assignment, the WMAI reports CANTCO and considers the next assignment. If the WMAI has fired fewer salvos than the number requested for the assignment, the algorithm ensures that the weapon assigned for the engagement can be located, sufficient munitions remain, and the assignment references a valid weapon record. (The WMAI reports CANTCO and considers the next assignment if any of those conditions fails). The WMAI then reports WILCO for the assignment if it has not yet been reported.

	After reporting WILCO, if the AI has not fired at the target for the assignment yet, or there are shots remaining to be fired for the assignment, the WMAI decides how to further handle the assignment. Otherwise, the WMAI moves on to the next assignment. How the WMAI responds depends on the behavior that it is currently executing: If the WMAI is not executing any behavior, it will engage the assigned target by pursuing it if the target is within the pursuit range, and intercepting (flying to the point where it expects the target to arrive at a future time) if it is not. If the WMAI is executing a "take action" behavior, it will continue on to evaluate the next assignment. If the WMAI is intercepting the assigned target and it has closed the distance so that the target is now within the pursuit range, it will stop intercepting and begin pursuing. If the WMAI is evading by performing a beam response or drag response, it will continue on to the next assignment without firing on the target.

	The WMAI will only fire on the assigned target if it is engaging the target, either by intercepting or pursuing. If it is currently intercepting or pursuing the target that corresponds to the assignment, the WMAI will then determine whether or not it should fire. First, the WMAI ensures that the track is of sufficiently high quality, i.e. the reporting sensor for the track is of a type that the user has said should be engaged. If the user has required that only "local" TTR targets should be engaged, the WMAI checks that the track contains at least one raw track originating from a TTR that is on the AI, on a peer platform, or on the AI's direct commander. Second, the WMAI verifies that the target falls within the weapon's range. If either of these checks fails, the WMAI moves on to the next assignment without firing on the target. Finally, the WMAI verifies that it can intercept the track. If it can, the WMAI attempts to fire a salvo at the target using the shot doctrine for the assignment. If it can't, it reports CANTCO and continues to the next assignment. The WMAI's attempt to fire a salvo at the target will fail if: (1) the weapon has no munitions left, (2) the maximum number of salvos for the weapon is already active, or (3) the weapon is reloading and is unable to shoot while reloading. Otherwise, the attempt to fire will succeed, meaning that the request to fire at the target will be added to the weapon's salvo request list.

Evasion Behavior Requirements
""""""""""""""""""""""""""""""
	Weapons Manager AIs depend on an RWR to detect and evade threats. To activate evasion behaviors, the user must both (1) enable the behaviors in the "rwr_response" block (see Commands section) and (2) place a WSF_ESM_SENSOR on the platform with its category set to "RWR" The RWR must be wired to the track_manager with an internal_link. The RWR detects other sensors that are transmitting within the RWR's frequency band, which can be changed using the sensor's "frequency_band" command. As a result, the RWR will not detect - and the weapons manager AI will not evade - threats that do not have a sensor on the platform that is transmitting within the RWR's frequency band.

	Example Platform configured for WSF_WEAPONS_MANAGER_AI evasion behaviors:

	.. _ag_processors_snippet_45:
 
	.. code-block::	rest	

		platform ai WSF_PLATFORM
		   ...
		   add processor track_manager WSF_TRACK_PROCESSOR
		      ...
		   end_processor
		
		   add sensor rwr WSF_ESM_SENSOR
		      on
		      category RWR
		      internal_link track_manager
		      frequency_band  0.1  ghz 20 ghz
		      ...
		      reports_location
		      ignore_same_side
		   end_sensor
		
		   add processor weapons_manager WSF_WEAPONS_MANAGER_AI
		      ...
		      rwr_response
		         enable
		         ...
		      end_rwr_response
		      ...
		   end_processor
		   ...
		end_platform

	Snippet 12.45: Platform Configured for WSF_WEAPONS_MANAGER_AI Evasion Behaviors

Weapons Manager AI Commands
""""""""""""""""""""""""""""

	The **alert_time** command allows the user to specify a delay time for parked air interceptors to wait prior to launching, in order to simulate scramble times.

	The **assess_engage_via** command allows the user to specify the weapons manager's assessment strategy. Collision-Intercept-PT-Inside-Zone assesses engagements by evaluating if the AI can intercept the track, should it continue moving at the current velocity, within a FEZ, if one exists for the AI. Munition-Intercept-PT-Ignore-Zone assesses engagements by evaluating if an AI's weapon can currently intercept the target. Munition-Intercept-PT-Inside-Zone evaluates assessments the same way as Munition-Intercept-PT-Ignore-Zone, except for the assessment to be successful, the munition's intercept point must be within a FEZ, if one exists for the AI.

	The **debug** command allows the user to turn on debug logging.

	The **home_base_position** command allows the user to specify the home base position for the AI. The AI will land at its home base position at the end of an egress if it is low on fuel, has no munitions, or is not on combat air patrol.

	The **intercept_speed** command allows the user to specify the speed at which the AI will move (or the mover's max speed, whichever is lower) as the AI intercepts a track.

	The **my_place_in_formation** command allows the user to specify the AI's place in formation for purposes of taking action. The take action and duration number will match the AI's configured place in formation.

	The **pursuit_range** command allows the user to specify the range within which the AI will engage assignments via the pursue behavior instead of the intercept behavior.

	The **take_action** block contains the following eleven commands, which pertain to the take action maneuvers, which, if enabled, are employed as an AI approaches a target it has been assigned.

	The **enable** command enables take_action behaviors.

	The **disable** command disables take_action behaviors.

	The **action_distance** command allows the user to specify the distance between AI and target at which the AI will take its specified action when approaching to engage an assigned target.

	The **ai_action_1** command allows the user to specify the action to take during initial approach of an assigned target at action_distance for an AI configured with my_place_in_formation 1.

	The **duration_1** command allows the user to specify the duration of the action taken by an AI with my_place_in_formation 1.

	The **ai_action_2** command allows the user to specify the action to take during initial approach of an assigned target at action_distance for an AI configured with my_place_in_formation 2.

	The **duration_2** command allows the user to specify the duration of the action taken by an AI with my_place_in_formation 2.

	The **ai_action_3** command allows the user to specify the action to take during initial approach of an assigned target at action_distance for an AI configured with my_place_in_formation 3.

	The **duration_3** command allows the user to specify the duration of the action taken by an AI with my_place_in_formation 3.

	The **ai_action_4** command allows the user to specify the action to take during initial approach of an assigned target at action_distance for an AI configured with my_place_in_formation 4.

	The **duration_4** command allows the user to specify the duration of the action taken by an AI with my_place_in_formation 4.

	The **rwr_response** block contains the following seven commands, which pertain to the AI attempting to detect threats and perform evasion behaviors.

	The **enable** command enables rwr_response behaviors. A WSF_ESM_SENSOR of category RWR is necessary on platform for RWR response evasion behaviors to occur.

	The **disable** command disables rwr_response behaviors.

	The **beam_duration** command allows the user to specify the maximum duration of a beam maneuver. Weapons manager AIs begin evading by performing a beam, moving along a vector 90 degrees left or right of the threat's velocity on a horizontal plane, and then reassess to determine if a drag maneuver should then be performed after the beam_duration has expired.

	The **sam_response_range** command allows the user to specify the maximum range from the AI at which a threat in the spatial domain of land or surface will cause an RWR response evasion behavior.

	The **ai_response_range** command allows the user to specify the maximum range from the AI at which a threat in the spatial domain of air or space will cause an RWR response evasion behavior.

	The **restore_after_break_lock** command allows the user to specify the amount of time since the last detection update that is required to consider a lock broken and complete RWR response evasion behaviors.

	The **priority** command allows the user to specify the methodology for choosing which threat to prioritize in the case of multiple concurrent locks detected by the RWR. Default prioritization responds to the first threat in the RWR track list. Choose-Closest-Threat prioritizes the closest threat.

	Example usage of WSF_WEAPONS_MANAGER_AI commands:

	.. _ag_processors_snippet_46:
 
	.. code-block::	rest	

		platform ai WSF_PLATFORM
		   ...
		   add processor weapons_manager WSF_WEAPONS_MANAGER_AI
		      ...
		      debug
		      alert_time             5 minutes
		      assess_engage_via      Collision-Intercept-PT-Inside-Zone
		      home_base_position     39.769106n 84.069864w
		      intercept_speed        600 knots
		      my_place_in_formation  3
		      pursuit_range          60 kilometers
		
		      rwr_response
		         enable
		         beam_duration             5 seconds
		         sam_response_range        95 kilometers
		         ai_response_range         45 kilometers
		         restore_after_break_lock  3 seconds
		         priority                  Choose-Closest-Threat
		      end_rwr_response
		
		      take_action
		         enable
		         action_distance  30 kilometers
		         ai_action_1      Break-Right
		         duration_1       7 seconds
		         ai_action_2      Break-Left
		         duration_2       7 seconds
		         ai_action_3      Straight
		         duration_3       2 seconds
		         ai_action_4      Break-Right
		         duration_4       7 seconds
		      end_take_action
		      ...
		   end_processor
		   ...
		end_platform

	Snippet 12.46: WSF_WEAPONS_MANAGER Commands

Required and Default Values
""""""""""""""""""""""""""""

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - alert_time
		  - No
		  - <time-value>
		  - 60 seconds
		  
		* - assess_engage_via
		  - No
		  - [ Collision-Intercept-PT-Inside-Zone | Munition-Intercept-PT-Inside-Zone | Munition-Intercept-PT-Ignore-Zone ]
		  - Munition-Intercept-PT-Ignore-Zone
		  
		* - debug
		  - No
		  - 
		  - Off
		  
		* - home_base_position
		  - No
		  - <latitude-value>, <longitude-value>
		  - 0n,0w
		  
		* - intercept_speed
		  - No
		  - <speed-value>
		  - 500 knots (257.222 meters/second)
		  
		* - my_place_in_formation
		  - No
		  - [ 1 | 2 | 3 | 4 ]
		  - 60 seconds
		  
		* - pursuit_range
		  - No
		  - <length-value>
		  - 60 kilometers

	Take Action Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - enable
		  - No
		  - 
		  - disable
		  
		* - disable
		  - No
		  - 
		  - disable
		  
		* - action_distance
		  - No
		  - <length-value>
		  - 45 kilometers
		  
		* - ai_action_1
		  - No
		  - [ Straight | Break-Left | Break-Right ]
		  - Break-Left
		  
		* - duration_1
		  - No
		  - <time-value>
		  - 3 seconds
		  
		* - ai_action_2
		  - No
		  - [ Straight | Break-Left | Break-Right ]
		  - Break-Right
		  
		* - duration_2
		  - No
		  - <time-value>
		  - 3 seconds
		  
		* - ai_action_3
		  - No
		  - [ Straight | Break-Left | Break-Right ]
		  - Straight
		  
		* - duration_3
		  - No
		  - <time-value>
		  - 0 seconds
		  
		* - ai_action_4
		  - No
		  - [ Straight | Break-Left | Break-Right ]
		  - Straight
		  
		* - duration_4
		  - No
		  - <time-value>
		  - 10 seconds

	RWR Response Commands

	.. list-table::
		:header-rows: 1

		* - Command
		  - Required?
		  - Input
		  - Default Value
		  
		* - enable
		  - No
		  - 
		  - disable
		  
		* - disable
		  - No
		  - 
		  - disable
		  
		* - beam_duration
		  - No
		  - <time-value>
		  - 3 seconds
		  
		* - sam_response_range
		  - No
		  - <length-value>
		  - 150 kilometers
		  
		* - ai_response_range
		  - No
		  - <length-value>
		  - 40 kilometers
		  
		* - restore_after_break_lock
		  - No
		  - <time-value>
		  - 3 seconds
		  
		* - priority
		  - No
		  - [ Choose-Closest-Threat | Default ]
		  - Default
