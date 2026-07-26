# Repository publishing policy

The official upstream repository `schombert/Project-Alice` is read-only for
agents working in this checkout.

- Never create or modify pull requests, issues, comments, reviews, releases,
  branches, tags, labels, or files in `schombert/Project-Alice`.
- Never push to the `upstream` remote.
- Push branches and commits only to `origin`
  (`tippered1-debug/Project-Alice`).
- When asked to create a pull request, create it only inside
  `tippered1-debug/Project-Alice` unless the user explicitly names a different
  repository and authorizes that exact external write in the same request.
- Fetching, comparing, and reading from upstream are allowed.
- If a requested publishing action could target upstream implicitly, stop and
  ask the user to name the destination repository.
