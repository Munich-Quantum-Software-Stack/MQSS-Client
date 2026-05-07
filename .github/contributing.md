# Contributing {#contributing}

<!-- [DOXYGEN] -->

Thank you for your interest in contributing to **MQSS Client**. We value contributions from people
with all levels of experience.

We use GitHub to [host code](https://github.com/Munich-Quantum-Software-Stack/MQSS Client), to
[track issues and feature requests][issues], as well as accept [pull
requests](https://github.com/Munich-Quantum-Software-Stack/MQSS Client/pulls). See
<https://docs.github.com/en/get-started/quickstart> for a general introduction to working with
GitHub and contributing to projects.

## Types of Contributions {#types-of-contributions}

Pick the path that fits your time and interests:

- 🐛 Report bugs:

  Use the _🐛 Bug report_ template at
  <https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/issues>. Include steps to reproduce,
  expected vs. actual behavior, environment, and a minimal example.

- 🛠️ Fix bugs:

  Browse [issues][issues], especially those labeled "bug", "help wanted", or "good first issue".
  Open a draft PR early to get feedback.

- 💡 Propose features:

  Use the _✨ Feature request_ template at
  <https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/issues>. Describe the motivation,
  alternatives considered, and (optionally) a small API sketch.

- ✨ Implement features:

  Pick items labeled "feature" or "enhancement". Coordinate in the issue first if the change is
  substantial; start with a draft PR.

- 📝 Improve documentation:

  Add or refine docstrings, tutorials, and examples; fix typos; clarify explanations. Small
  documentation-only PRs are very welcome.

- ⚡️ Performance and reliability:

  Profile hot paths, add benchmarks, reduce allocations, deflake tests, and improve error messages.

- 📦 Packaging and tooling:

  Improve build configuration, type hints/stubs, CI workflows, and platform wheels. Incremental
  tooling fixes have a big impact.

- 🙌 Community support:

  Triage issues, reproduce reports, and answer questions in Discussions:
  <https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/discussions>.

## Guidelines {#guidelines}

Please adhere to the following guidelines to help the project grow sustainably. Contributions that
do not comply with these guidelines or violate our [AI Usage Guidelines][ai_usage] may be rejected
without further review.

### Core Guidelines {#core-guidelines}

- ["Commit early and push often"](https://www.worklytics.co/blog/commit-early-push-often).
- Write meaningful commit messages, preferably using [gitmoji](https://gitmoji.dev) for additional
  context.
- Focus on a single feature or bug at a time and only touch relevant files. Split multiple features
  into separate contributions.
- Add tests for new features to ensure they work as intended.
- Document new features. For user-facing changes, add a changelog entry; for breaking changes,
  update the upgrade guide.
- Add tests for bug fixes to demonstrate the fix.
- Document your code thoroughly and ensure it is readable.
- Keep your code clean by removing debug statements, leftover comments, and unrelated code.
- Check your code for style and linting errors before committing.
- Follow the project's coding standards and conventions.
- Be open to feedback and willing to make necessary changes based on code reviews.

### AI-assisted contributions {#ai-assisted-contributions}

We acknowledge the utility of AI-based coding assistants (e.g., GitHub Copilot, ChatGPT) in modern
software development. However, their use requires a high degree of responsibility and transparency
to maintain code quality and licensing compliance.

Please carefully read and follow our dedicated [AI Usage Guidelines][ai_usage] before submitting any
AI-assisted contribution. In short: **You are responsible for every line of code you submit**, and a
**human must always be in the loop**. We require disclosure of AI tool usage in your PR description.

### Pull Request Workflow {#pull-request-workflow}

- Create PRs early. Work-in-progress PRs are welcome; mark them as drafts on GitHub.
- Use a clear title, reference related issues by number, and describe the changes. Follow the PR
  template; only omit the issue reference if not applicable.
- CI runs on all supported platforms and Python versions to build, test, format, and lint. All
  checks must pass before merging.
- When ready, convert the draft to a regular PR and request a review from a maintainer. If unsure,
  ask in PR comments. If you are a first-time contributor, mention a maintainer in a comment to
  request a review.
- If your PR gets a "Changes requested" review, address the feedback and push updates to the same
  branch. Do not close and reopen a new PR. Respond to comments to signal that you have addressed
  the feedback. Do not resolve review comments yourself; the reviewer will do so once satisfied.
- If the reviewer suggested changes with explicit code suggestions as part of the comments, apply
  these directly using the GitHub UI. This attributes the changes to the reviewer and automatically
  resolves the respective comments (this is an exception to the rule above). If there are multiple
  suggestions that you want to apply at once, you can batch them into a single commit. Go to the
  "Files changed" tab of the PR, and then click "Add suggestion to batch" for each suggestion you
  want to include. Once you are done selecting suggestions, click "Commit suggestions". Only apply
  suggestions manually if using the GitHub UI is not feasible.
- Re-request a review after pushing changes that address feedback.
- Do not squash commits locally; maintainers typically squash on merge. Avoid rebasing or
  force-pushing before reviews; you may rebase after addressing feedback if desired.

<!-- [DOXYGEN] -->
