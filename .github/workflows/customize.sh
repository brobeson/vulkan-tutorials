#!/bin/bash

set -eu
shopt -s globstar

# Remove this workflow so it doesn't run again after this.
(
  cd .github/workflows
  rm template_customization.yaml customize.sh
)

# Remove template content from the README file.
sed --in-place '4,$d' README.md

# Update the project name and owner in the remaining files.
sed --in-place \
  -e "s/CppStarter/${PROJECT_NAME}/g" \
  -e "s/brobeson/${OWNER}/g" \
  LICENSE ./**/*.md .github/**/*.yaml .vscode/*.json ./**/CMakeLists.txt

# Update the CMake project(DESCRIPTION ...)
DESCRIPTION=$(gh repo view --json description --jq .description)
sed --in-place "s/DESCRIPTION .*/DESCRIPTION ${DESCRIPTION}/" CMakeLists.txt

# That sed catches the static analysis workflow, but nothing should change in
# that file. Just `git restore` it.
git restore .github/workflows/static_analysis.yaml

# Create the "Next Release" milestone
gh api \
  --method POST \
  -H "Accept: application/vnd.github.v3+json" \
  "/repos/${OWNER}/${PROJECT_NAME}/milestones" \
  --raw-field title='Next Release' \
  --raw-field state='open' \
  --raw-field description='Task planning for the next release'
