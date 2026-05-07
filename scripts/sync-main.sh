#!/usr/bin/env bash
set -euo pipefail

git fetch origin main

current_branch="$(git branch --show-current)"
if [[ "$current_branch" != "main" ]]; then
  git switch main
fi

git pull --ff-only origin main
git status --short --branch
