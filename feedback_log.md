# Workflow Feedback Log
Structured record of quality complaints and prompt fixes.

## 2026-09-02
- **Node:** Agent Behavior / Communication
- **Complaint:** "i think you are stupid? like why do we have a webcall? why dont you try to talk with me first"
- **Root Cause:** Rushed into running background system checks and network queries instead of having a collaborative architectural conversation with the user first.
- **Resolution:** Codified "Discuss Before Acting" into `AGENTS.md`. Mandated that all requirements, approaches, and trade-offs must be discussed and agreed upon with the user before executing code or tool calls.

## 2026-09-03
- **Node:** Agent Behavior / Code Delivery
- **Complaint:** "that is not very small and i meant effective for agents.md"
- **Root Cause:** Provided a 35-line block of code handling multiple concerns at once instead of truly small, step-by-step increments for learning.
- **Resolution:** Codified "Very Small Code Steps" in `AGENTS.md`. Strictly limited code presentations to tiny 3–6 line focused increments, explaining what each line does in Rust before moving on.

## 2026-09-26
- **Node:** Agent Behavior / Bite-Sized Increments
- **Complaint:** "i think you vialolted our coding principel count mhow many adjustmenst you did"
- **Root Cause:** In a single turn, performed 9 separate file modifications across `vendor/`, `src/render.hpp`, `src/render.cpp`, `.clangd`, `Makefile`, `src/main.cpp`, `test.md`, and `plan.md` without pausing, explaining each one individually, or getting user confirmation between steps.
- **Resolution:** Strictly enforce single-action, bite-sized steps: present only one file or small change at a time, explain it, and wait for explicit user approval before moving to the next.

## 2026-10-10
- **Node:** Agent Behavior / Build Turn Efficiency
- **Complaint:** "okay whyt do you rebuilt or use make when we updat ethe log? ... i want to change that, make is okay after changign code but nwo aftewr changign log files"
- **Root Cause:** Executed the check command after editing markdown plan/log files even though non-code edits cannot break C++ compilation.
- **Resolution:** Updated `.agents/rules/core.md:35` to explicitly skip the check command for documentation and log edits, saving turns while keeping builds mandatory for code edits.

