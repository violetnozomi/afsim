#!/usr/bin/env bash

set -euo pipefail

readonly AFSIM_SOURCE="${AFSIM_SOURCE:-/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src}"
readonly AFSIM_BUILD="${AFSIM_BUILD:-${AFSIM_SOURCE}/build-ubuntu24}"
readonly AFSIM_RESOURCE_ROOT="${AFSIM_RESOURCE_ROOT:-/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/resources}"
readonly NRM_SOURCE="${NRM_SOURCE:-/home/pyh/afsim/network_resource_manager}"
readonly NRM_REMOTE_ROOT="${NRM_REMOTE_ROOT:-/home/pyh/.local/nrm-remote}"
readonly AFSIM_REMOTE_ROOT="${AFSIM_REMOTE_ROOT:-${NRM_REMOTE_ROOT}/opt/afsim}"
readonly AFSIM_REMOTE_BIN="${AFSIM_REMOTE_ROOT}/bin"
readonly QT_COMPAT_SOURCE="${NRM_SOURCE}/scripts/remote/qt512_readlink_compat.c"
readonly QT_COMPAT_LIBRARY="${AFSIM_REMOTE_ROOT}/lib/libnrm_qt512_readlink_compat.so"
readonly NRM_VNC_DISPLAY_NUMBER="${NRM_VNC_DISPLAY_NUMBER:-1}"
readonly NRM_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}/nrm-vnc"
readonly -a WARLOCK_VISUAL_PLUGINS=(
   CommVis
)
readonly -a DISABLED_REMOTE_VISUAL_PLUGINS=(
   MapDisplay
   MapHoverInfo
   Interactions
   PlatformBrowser
   PlatformData
   PlatformMovement
   Tracks
)

export DISPLAY="${DISPLAY:-:${NRM_VNC_DISPLAY_NUMBER}}"
export XAUTHORITY="${XAUTHORITY:-${NRM_RUNTIME_DIR}/Xauthority}"
export SOURCE_ROOT="${AFSIM_SOURCE}"
# WKF MapDisplay profiles expand paths such as
# $RESOURCE_PATH/bluemarble_db/bmng.earth, so this variable must point to the
# map catalog rather than the resources parent directory.
export RESOURCE_PATH="${AFSIM_RESOURCE_ROOT}/maps"
export NRM_OUTPUT_DIR="${NRM_OUTPUT_DIR:-${NRM_SOURCE}/output}"
export XDG_DATA_DIRS="${NRM_REMOTE_ROOT}/usr/share:${XDG_DATA_DIRS:-/usr/local/share:/usr/share}"
export LIBGL_ALWAYS_SOFTWARE="${LIBGL_ALWAYS_SOFTWARE:-1}"
export QT_X11_NO_MITSHM=1
unset QT_QPA_PLATFORM

export LD_LIBRARY_PATH="${NRM_REMOTE_ROOT}/usr/lib/x86_64-linux-gnu:\
${AFSIM_BUILD}:\
${AFSIM_BUILD}/3rd_party/osg-3.6.3-x64-lnx/lib64:\
${AFSIM_BUILD}/3rd_party/osgEarth-2.10.1-x64-lnx/lib64:\
${AFSIM_BUILD}/3rd_party/gdal-3.3.2-x64-lnx/lib:\
${AFSIM_BUILD}/3rd_party/geos-3.5.1-x64-lnx/lib:\
${AFSIM_BUILD}/3rd_party/proj-8.1.1-x64-lnx/lib:\
${AFSIM_BUILD}/3rd_party/qt-5.12.11-x64-lnx/lib:\
${LD_LIBRARY_PATH:-}"

if ! xdpyinfo -display "${DISPLAY}" >/dev/null 2>&1
then
   echo "Display ${DISPLAY} is unavailable. Start nrm-vnc.service first." >&2
   exit 1
fi

# Qt 5.12's QLockFile helper in this AFSIM distribution cannot safely inspect
# the original, very long build-tree executable path on current glibc. Keep a
# short user-local runtime path and refresh its executable on every launch.
mkdir -p "${AFSIM_REMOTE_BIN}" "${AFSIM_REMOTE_ROOT}/lib" \
   "${AFSIM_RESOURCE_ROOT}/site/mil-std2525d"
for plugin_dir in wkf_plugins warlock_plugins
do
   if [[ -L "${AFSIM_REMOTE_BIN}/${plugin_dir}" ]]
   then
      unlink "${AFSIM_REMOTE_BIN}/${plugin_dir}"
   fi
   mkdir -p "${AFSIM_REMOTE_BIN}/${plugin_dir}"
done
for plugin_name in "${DISABLED_REMOTE_VISUAL_PLUGINS[@]}"
do
   for plugin_dir in wkf_plugins warlock_plugins
   do
      disabled_library="${AFSIM_REMOTE_BIN}/${plugin_dir}/lib${plugin_name}_ln13m64.so"
      if [[ -L "${disabled_library}" ]]
      then
         unlink "${disabled_library}"
      fi
   done
done
install -m 0755 "${AFSIM_BUILD}/warlock" "${AFSIM_REMOTE_BIN}/warlock"
ln -sfn "${AFSIM_BUILD}/wsf_plugins" "${AFSIM_REMOTE_BIN}/wsf_plugins"
ln -sfn "${AFSIM_RESOURCE_ROOT}" "${AFSIM_REMOTE_ROOT}/resources"

# The short-path executable must have the standard visualization plugins beside
# it.  Without these links the custom dock loads, but Warlock has no central map,
# platform symbology, movement, tracks, or communication visualization.
for plugin_name in "${WARLOCK_VISUAL_PLUGINS[@]}"
do
   plugin_library="${AFSIM_BUILD}/lib${plugin_name}_ln13m64.so"
   if [[ -f "${plugin_library}" ]]
   then
      ln -sfn "${plugin_library}" \
         "${AFSIM_REMOTE_BIN}/warlock_plugins/$(basename "${plugin_library}")"
   fi
done

for plugin_library in "${AFSIM_BUILD}/warlock_plugins/"libNetworkResourceManager_*.so
do
   if [[ -f "${plugin_library}" ]]
   then
      ln -sfn "${plugin_library}" \
         "${AFSIM_REMOTE_BIN}/warlock_plugins/$(basename "${plugin_library}")"
   fi
done
if [[ ! -f "${QT_COMPAT_LIBRARY}" || "${QT_COMPAT_SOURCE}" -nt "${QT_COMPAT_LIBRARY}" ]]
then
   cc -shared -fPIC -O2 -Wall -Wextra -o "${QT_COMPAT_LIBRARY}" "${QT_COMPAT_SOURCE}"
fi
export LD_PRELOAD="${QT_COMPAT_LIBRARY}${LD_PRELOAD:+:${LD_PRELOAD}}"

if [[ "${1:-}" == "--gdb" ]]
then
   shift
   if [[ "$#" -eq 0 ]]
   then
      set -- "${NRM_SOURCE}/test_mission/four_network_overview.txt"
   fi
   exec gdb --args "${AFSIM_REMOTE_BIN}/warlock" "$@"
fi

if [[ "$#" -eq 0 ]]
then
   set -- "${NRM_SOURCE}/test_mission/four_network_overview.txt"
fi

# Resolve project-relative mission paths before Warlock starts.  Warlock may
# change its working directory to the short runtime tree, where a relative
# test_mission/... path no longer identifies the source-tree file.
if [[ "${1}" != /* && -f "${NRM_SOURCE}/${1}" ]]
then
   set -- "${NRM_SOURCE}/${1}" "${@:2}"
fi

exec "${AFSIM_REMOTE_BIN}/warlock" "$@"
