#!/usr/bin/env bash
set -euo pipefail

current_branch="$(git branch --show-current)"
if [[ "$current_branch" != "main" ]]; then
  echo "Not on main. Switch to main first."
  exit 1
fi

if [[ -n "$(git status --porcelain)" ]]; then
  echo "Working tree is not clean. Commit or stash changes before pushing."
  exit 1
fi

git push origin main
