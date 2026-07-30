#!/usr/bin/env bash

set -euo pipefail

readonly NRM_REMOTE_ROOT="${NRM_REMOTE_ROOT:-/home/pyh/.local/nrm-remote}"
readonly NRM_VNC_DISPLAY_NUMBER="${NRM_VNC_DISPLAY_NUMBER:-1}"
readonly NRM_VNC_DISPLAY=":${NRM_VNC_DISPLAY_NUMBER}"
readonly NRM_VNC_PORT="$((5900 + NRM_VNC_DISPLAY_NUMBER))"
readonly NRM_VNC_GEOMETRY="${NRM_VNC_GEOMETRY:-1920x1080}"
readonly NRM_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}/nrm-vnc"
readonly NRM_XAUTHORITY="${NRM_RUNTIME_DIR}/Xauthority"

export PATH="${NRM_REMOTE_ROOT}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${NRM_REMOTE_ROOT}/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH:-}"
export XDG_DATA_DIRS="${NRM_REMOTE_ROOT}/usr/share:${XDG_DATA_DIRS:-/usr/local/share:/usr/share}"
export DISPLAY="${NRM_VNC_DISPLAY}"
export XAUTHORITY="${NRM_XAUTHORITY}"

mkdir -p "${NRM_RUNTIME_DIR}"
chmod 700 "${NRM_RUNTIME_DIR}"

if xdpyinfo -display "${NRM_VNC_DISPLAY}" >/dev/null 2>&1
then
   echo "Display ${NRM_VNC_DISPLAY} is already active." >&2
   exit 1
fi

touch "${NRM_XAUTHORITY}"
chmod 600 "${NRM_XAUTHORITY}"
xauth -f "${NRM_XAUTHORITY}" remove "${NRM_VNC_DISPLAY}" >/dev/null 2>&1 || true
xauth -f "${NRM_XAUTHORITY}" add "${NRM_VNC_DISPLAY}" . "$(mcookie)"

vnc_pid=""
desktop_pid=""

cleanup()
{
   trap - EXIT INT TERM
   if [[ -n "${desktop_pid}" ]] && kill -0 "${desktop_pid}" >/dev/null 2>&1
   then
      kill "${desktop_pid}" >/dev/null 2>&1 || true
      wait "${desktop_pid}" >/dev/null 2>&1 || true
   fi
   if [[ -n "${vnc_pid}" ]] && kill -0 "${vnc_pid}" >/dev/null 2>&1
   then
      kill "${vnc_pid}" >/dev/null 2>&1 || true
      wait "${vnc_pid}" >/dev/null 2>&1 || true
   fi
}
trap cleanup EXIT INT TERM

"${NRM_REMOTE_ROOT}/usr/bin/Xtigervnc" "${NRM_VNC_DISPLAY}" \
   -localhost \
   -SecurityTypes None \
   -rfbport "${NRM_VNC_PORT}" \
   -geometry "${NRM_VNC_GEOMETRY}" \
   -depth 24 \
   -AlwaysShared \
   -DisconnectClients=0 \
   -desktop "AFSIM-NRM" \
   -auth "${NRM_XAUTHORITY}" &
vnc_pid="$!"

for _ in $(seq 1 100)
do
   if xdpyinfo -display "${NRM_VNC_DISPLAY}" >/dev/null 2>&1
   then
      break
   fi
   if ! kill -0 "${vnc_pid}" >/dev/null 2>&1
   then
      wait "${vnc_pid}"
   fi
   sleep 0.1
done

if ! xdpyinfo -display "${NRM_VNC_DISPLAY}" >/dev/null 2>&1
then
   echo "Display ${NRM_VNC_DISPLAY} failed to start." >&2
   exit 1
fi

dbus-run-session -- "${NRM_REMOTE_ROOT}/usr/bin/openbox" &
desktop_pid="$!"

echo "NRM VNC desktop is listening on 127.0.0.1:${NRM_VNC_PORT}."
wait "${vnc_pid}"
