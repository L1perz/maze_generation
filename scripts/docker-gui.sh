#!/usr/bin/env bash
set -e

export DISPLAY=:99

Xvfb "$DISPLAY" -screen 0 1366x768x24 +extension GLX +render -noreset &
XVFB_PID=$!

sleep 1
fluxbox >/tmp/fluxbox.log 2>&1 &
x11vnc -display "$DISPLAY" -forever -shared -nopw -rfbport 5900 >/tmp/x11vnc.log 2>&1 &
websockify --web=/usr/share/novnc/ 0.0.0.0:6080 localhost:5900 >/tmp/websockify.log 2>&1 &

./MazeExplorer --windowed &
APP_PID=$!

wait "$APP_PID"
kill "$XVFB_PID" 2>/dev/null || true
