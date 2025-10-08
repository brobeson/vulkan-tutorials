# CppStarter

[![Static Analysis](https://github.com/brobeson/CppStarter/actions/workflows/static_analysis.yaml/badge.svg)](https://github.com/brobeson/CppStarter/actions/workflows/static_analysis.yaml)

This is a template repository to start a new C++ project.

## Using This Template

1. In GitHub, click **Use this template**, then click **Create a new repository**.
1. Fill out the form for your new repository.
   The values shouldn't affect the template's behavior.
1. Wait for GitHub to create your new repository.
1. After GitHub creates your repository, run the [New Project Setup workflow](/.github/workflows/template_customization.yaml).
   1. Click the **Actions** tab.
   1. Click the **New Project Setup** workflow.
   1. Click the **Run workflow** combobox.
   1. Set **Use workflow from** to `Branch: main`.
   1. Click the **Run workflow** button.

### New Project Setup Workflow

The New Project Setup workflow customizes your new project.
The workflow performs these steps:

1. Clone the repository
1. Change all instances of `CppStarter` to the new repository's name
1. Remove the workflow file and shell script
1. Remove these instructions from README.md
1. Create a `Next Release` milestone
1. Commit and push the changes to `main`

## What You Get From This Template

This template repository provides all the boiler plate to get started with a high quality C++ project.

### A C++ Project

The template provides a complete C++ project that builds and runs.

- CMake infrastructure to generate a build system.
- A simple unit test that builds and runs.

### GitHub Workflows

The template includes GitHub workflows that run right out of the box.

- [Support File Quality](https://github.com/brobeson/SupportFileQuality)  
  This workflow ensures there are no mistakes in project support files such as Markdown documentation and tool configuration files.
- [C++ Build](https://github.com/brobeson/CppWorkflow)  
  Runs debug and release builds on Ubuntu, MacOS, and Windows.
- [C++ Static Analysis](https://github.com/CppWorkflow)  
  Runs static analysis tools on the C++ code.

### Visual Studio Code Configuration

I use [Visual Studio Code](https://code.visualstudio.com/), so this template includes settings, tasks, and extension recommendations for that IDE.

#### Tasks

The VS Code configuration includes a set of tasks to emulate the GitHub workflows.
These can help you check the entire code base of your project before pushing.
Each task has the prefix `ProjectName:`, where "ProjectName" is the name of the repo you create from the template.
The available tasks are:

- `ProjectName: Spell Check` - Run CSpell on all files in the repository.
  Spelling mistakes are captured in the Problems panel.
- `ProjectName: Markdownlint` - Run Markdownlint on all Markdown files in the repository.
  Markdown mistakes are captured in the Problems panel.
- `ProjectName: Prettier Format` - Run Prettier on all files in the repository.
  This doesn't report formatting errors; it reformats the files in-place.
- `ProjectName: Pipeline` - Run all the other tasks.
  This is a shortcut to invoke all the other tasks with a single command.
