#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
VENV_DIR="$SCRIPT_DIR/.venv"
PYTHON="$VENV_DIR/bin/python"

if [ ! -x "$PYTHON" ]; then
    python3 -m venv "$VENV_DIR"
    "$PYTHON" -m pip install -r "$SCRIPT_DIR/requirements.txt"
fi

exec "$PYTHON" "$SCRIPT_DIR/LogDecrypt.py" "$@"
