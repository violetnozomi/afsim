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

export DISPLAY="${DISPLAY:-:${NRM_VNC_DISPLAY_NUMBER}}"
export XAUTHORITY="${XAUTHORITY:-${NRM_RUNTIME_DIR}/Xauthority}"
export SOURCE_ROOT="${AFSIM_SOURCE}"
export RESOURCE_PATH="${AFSIM_RESOURCE_ROOT}"
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
install -m 0755 "${AFSIM_BUILD}/warlock" "${AFSIM_REMOTE_BIN}/warlock"
ln -sfn "${AFSIM_BUILD}/warlock_plugins" "${AFSIM_REMOTE_BIN}/warlock_plugins"
ln -sfn "${AFSIM_BUILD}/wsf_plugins" "${AFSIM_REMOTE_BIN}/wsf_plugins"
ln -sfn "${AFSIM_BUILD}/wkf_plugins" "${AFSIM_REMOTE_BIN}/wkf_plugins"
ln -sfn "${AFSIM_RESOURCE_ROOT}" "${AFSIM_REMOTE_ROOT}/resources"
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
      set -- "${NRM_SOURCE}/test_mission/framework_smoke.txt"
   fi
   exec gdb --args "${AFSIM_REMOTE_BIN}/warlock" "$@"
fi

if [[ "$#" -eq 0 ]]
then
   set -- "${NRM_SOURCE}/test_mission/framework_smoke.txt"
fi

exec "${AFSIM_REMOTE_BIN}/warlock" "$@"
