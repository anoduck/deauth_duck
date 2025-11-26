#!/usr/bin/env bash
# vim: set sw=4 sts=4 et ft=sh :
# -*- mode:bash; -*-
# -------------------------------------------------------
# Copyright (C) 2025 by Anoduck, The Anonymous Duck
# -------------------------------------------------------
# https://anoduck.mit-license.org
# -------------------------------------------------------
#  Used to push changes from Arduino's Sketchbook to it's repo.
# =============================================================
NOW=$(date --rfc-3339=s)
REPOBASE="$HOME/Sandbox/deauth_duck"
REPO="$REPOBASE/arduino"
SKETCHBOOK="$HOME/Arduino/sketch_nov25a"

if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: pushgit.sh"
		echo " --------------- "
		echo "Push changes to git from sketchbook."
    exit 0
fi

skgit () {
    git -C "$REPO" "$@"
}

check_repo () {
	test -d "$REPO" && return
	mkdir -p "$REPO"
}

move_file () {
	rsync -rul "$SKETCHBOOK" "$REPO"
}

commit_changes () {
	pushd "$REPOBASE" || exit 1
	skgit pull
  skgit add -A
  skgit commit -am "Pushing newest changes $NOW"
	skgit pull
  skgit push
	pushd -1 || exit 1
}

check_repo
move_file
commit_changes
