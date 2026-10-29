# myshell

A basic Unix shell in C, built in stages as a learning project (processes, fork/exec/wait, file descriptors, pipes, dup2, signals). It will later get an optional AI layer that turns natural-language requests into commands using a local Ollama model.

## How we work

- **The user plans, Claude implements.** For each feature, first agree on a plan: which files change, which data structures, which syscalls, which edge cases. Claude helps by raising options and pitfalls, but the user makes the decisions. Write code only after the user approves the plan.
- **Explain while implementing.** For each change, explain the systems concepts it uses: what each syscall does, why it is called in that order, what breaks if a step is skipped (for example, an unclosed pipe end makes the reader hang). Point to the relevant man page (`man 2 pipe`). Keep code and explanation in small steps the user can follow; don't drop a whole module at once.
- **Stay in the current stage.** Don't implement features from later stages early (see Status). If a later stage will need a change, say so instead of making it.
- **Language.** Talk to the user in Hebrew and wrap each reply in `<div dir="rtl">…</div>` so it renders right-to-left. Write code, comments, commit messages and docs in English.

## Environment

The repo lives on Windows, but the shell is POSIX-only. Build, run and test **inside WSL (Ubuntu)**, for example `wsl make` or `wsl ./myshell`. Never try to build with a Windows toolchain.

## Build

```bash
make            # gcc -Wall -Wextra -g (default dialect, gnu17)
make asan       # same, plus -fsanitize=address,undefined (use when chasing memory bugs)
make valgrind   # build without ASan, then run valgrind --leak-check=full --track-fds=yes
make test       # tests/run.sh
make clean
make AI=0       # build without the AI module and libcurl (once the AI module exists)
```

Code must compile without warnings.

## Architecture

One input line flows through:

```
input -> [ai_translate, optional] -> lex -> parse -> execute
```

```c
token    *lex(const char *line);
pipeline *parse(token *tokens);                        // NULL on syntax error
void      execute(pipeline *p, shell_state *st);
char     *ai_translate(const char *request, shell_state *st); // NULL = cancelled
```

Rules:
- Each stage is a separate module that knows nothing about the others. The AI layer only outputs a string; the parser and executor don't know AI exists.
- Parsing and execution are strictly separate. The executor never looks at tokens or raw text.
- `main.c` contains only the REPL loop and should read like the flow above.
- The parser owns the AST structs (`redirect`, `command`, `pipeline`), defined in `parser.h`.
- Each stage frees the memory of the input it consumed.
- Helper functions are `static`. Headers expose only the interface.
- Even the naive stage-1 parser returns a `pipeline` with one command, so the executor doesn't change when the parser is replaced.
- The AI module is compiled only when `AI=1`; `ollama.c` is HTTP transport only (libcurl + cJSON) and must stay swappable.

Layout: `include/` (`shell.h lexer.h parser.h executor.h builtins.h ai.h`), `src/` (`main.c input.c lexer.c parser.c executor.c builtins.c signals.c`, `ai/ai.c`, `ai/ollama.c`), `tests/`.

## Conventions

- Use standard C and POSIX APIs only; avoid Linux- or GNU-specific calls. Read input with `getline` (no readline).
- `snake_case` everywhere. Structs are `typedef`'d without a `_t` suffix (POSIX reserves `_t`). Macros are `UPPER_CASE`. Header guards look like `MYSHELL_PARSER_H`.
- 4-space indent, K&R braces.
- Errors:
  - Check every syscall.
  - Print errors to stderr as `myshell: <context>: <strerror>`, e.g. `myshell: cd: No such file or directory`.
  - A child whose `exec` fails calls `_exit(127)`, or `_exit(126)` for permission errors. Never `exit()` in a child after `fork`.
  - Allocation failures go through `xmalloc`/`xstrdup`, which print a message and exit.
- When behavior is unclear, match bash/POSIX sh.

## Testing

- `tests/run.sh` feeds commands into `./myshell` on stdin and compares the output with `bash` running the same commands.
- From stage 5, add small C unit tests for the lexer and parser (plain `assert`, no framework).
- A feature is done when tests pass, `make valgrind` reports no leaks and no leaked file descriptors, and the user has reviewed it.

## Git

- One branch per feature, created from `main`: `feat/<short-name>` (e.g. `feat/redirections`).
- Make small commits on the branch with messages like `redirections: open target and dup2 in child`.
- Push after every commit (`git push -u origin <branch>` the first time).
- Merge into `main` with `git merge --no-ff` only after the user approves, push `main`, then delete the branch locally and on `origin`.
- Never commit directly to `main`.

## Status

- [ ] 1. REPL loop, naive tokenizer, fork/execvp/waitpid
- [ ] 2. Builtins: `cd`, `exit`, `export` (run in the shell process, no fork)
- [ ] 3. Redirections `<` `>` `>>` (open + dup2 in the child)
- [ ] 4. Pipes (close every unused pipe end)
- [ ] AI layer (design still to be discussed; don't start without a plan)
- [ ] 5. Real lexer and parser: quotes, escaping, `$VAR`, `$?`, `;` `&&` `||`
- [ ] 6. Signals: the shell ignores SIGINT, children restore the default
- [ ] 7. Background jobs (`&`) and reaping zombies
- [ ] 8. (Optional) job control: `fg`, `bg`, Ctrl-Z, process groups
