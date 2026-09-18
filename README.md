*Este proyecto ha sido creado como parte del currículo de 42 por caperale.*

# Codexion

A concurrent C simulation of the classic **Dining Philosophers** problem, reimagined as a team of *coders* sharing *dongles* to compile their code. It implements a thread-safe scheduler with two policies (FIFO and EDF), a dedicated monitor thread, cooldown management and precise burnout detection.

## Description

**Codexion** models a group of `N` coders sitting around a table. Between each coder there is a single shared *dongle* (a hardware license token), so every coder needs to hold **both** its left and right dongles to perform a *compilation*. The goal of each coder is to complete a required number of compilations before *burning out*.

The rest of the lifecycle mimics the classic problem:

- **Compile**: only possible while holding both dongles.
- **Debug**: a free activity done after compiling.
- **Refactor**: a free activity done after compiling.
- **Burn out**: if a coder does not compile within `time_to_burnout` milliseconds, it burns out — immediately ending the simulation.

A **monitor thread** watches the coders for burnout and termination conditions, while the coders themselves coordinate dongle allocation through a centralized scheduler. The scheduler grants *both* dongles at once using per-dongle priority heaps, which avoids the race and deadlock pitfalls of classic dining-philosophers solutions.

Two scheduling policies are available:

- **`fifo`** – First In, First Out. Coders are granted dongles in arrival order (each dongle keeps an `arrival_counter`).
- **`edf`** – Earliest Deadline First. The coder whose projected burnout deadline (`last_compilation_time + time_to_burnout`) is soonest is granted the dongles first.

## Instructions

### Compiling

The project includes a `Makefile`:

```bash
make -C coders
```

The binary `codexion` is produced inside the `coders/` directory. The Makefile also provides:

- `make clean` / `make fclean` – remove object files and/or the binary.
- `make re` – rebuild from scratch.
- `make sanitize` – rebuild with `-fsanitize=thread` (ThreadSanitizer) for race detection.

### Running

```bash
./coders/codexion <number_of_coders> <time_to_burnout> \
    <time_to_compile> <time_to_debug> <time_to_refactor> \
    <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

| Argument | Meaning |
| --- | --- |
| `number_of_coders` | Amount of coder threads (≥ 1). |
| `time_to_burnout` (ms) | Max time without compiling before a coder burns out. |
| `time_to_compile` (ms) | Duration of a compilation. |
| `time_to_debug` (ms) | Duration of a debugging session. |
| `time_to_refactor` (ms) | Duration of a refactoring session. |
| `number_of_compiles_required` | Compilations each coder must finish. |
| `dongle_cooldown` (ms) | Minimum idle time required for a dongle after each release. |
| `scheduler` | `fifo` or `edf`. |

### Example

```bash
./coders/codexion 5 800 200 200 200 5 100 fifo
```

Runs 5 coders that burn out after 800 ms without compiling. Each compilation, debugging and refactoring session takes 200 ms, each coder must compile 5 times, dongles rest 100 ms between uses, and dongles are granted FIFO.

### Output format

Each line logs a state change with a relative timestamp:

```
<ms> <coder_index> <message>
```

e.g.

```
100 2 has taken a dongle
100 2 has taken a dongle
100 2 is compiling
...
320 4 has burned out
```

## Blocking cases handled

- **Deadlock prevention (Coffman conditions).** The classic deadlock arises from *hold-and-wait* plus *circular wait*: a coder holding one dongle while waiting for another. Here that is impossible: a coder never holds a partial allocation. It registers its request in the heaps of *both* dongles atomically (under the scheduler mutex) and only proceeds when the scheduler can grant **both** dongles simultaneously. Since dongles are always acquired as a set and released together, no cycle of waiting can form.

- **Starvation prevention.** Every conditional wait on a dongle is ordered by the per-dongle priority heap, which guarantees that requests are served in priority order — arrival order (`fifo`) or burnout-deadline order (`edf`). A coder waiting at the head of both heaps is always granted the dongles as soon as they become available, so no coder can be indefinitely postponed behind newer requests.

- **Cooldown management.** Each dongle records `last_release_ms`. A request is only grantable when the elapsed time since the dongle was last released is at least `dongle_cooldown`. This ensures a minimum resting period between uses and prevents the same dongle from being hot-swapped between coders faster than allowed.

- **Accurate burnout detection.** A coder can only burn out if it is **not** currently holding dongles (`has_dongles == 0`). If the dongles have already been granted, the coder is guaranteed to compile, so counting a deadline violation at that point would be a false positive. The monitor reads these flags as a consistent snapshot under the scheduler and coder mutexes, and, in the `edf` policy, the deadline starts from `start_time` before the first compilation (`last_compilation_time == 0`). A coder holding the dongles is never reported as burned out.

- **Log serialization.** All output goes through a single `log_mutex`, and timestamps are computed against the shared `start_time`, so logs from concurrent threads never interleave and share a coherent time base. The single-coder case (where one coder would hold both dongles and deadlock) is handled specially: it simply burns out after `time_to_burnout`.

- **Early termination.** When a coder burns out, `sim_end` is set and all waiters are woken up via the scheduler condition variable; `sleep_ms` also polls `sim_is_over()` so long sleeps are cut short as soon as the simulation ends.

## Thread synchronization mechanisms

The solution relies exclusively on POSIX threads primitives and a small custom event mechanism built on top of them.

### Primitives used

- **`pthread_mutex_t`** protects every shared resource:
  - `log_mutex` serializes logging so state-change lines never interleave.
  - `sched_mutex` guards the scheduler: the dongle heaps, `is_being_used`, `last_release_ms`, `sim_end` and `has_dongles`.
  - `start_mutex` guards the arrival/ready barrier between coders and the monitor.
  - a per-coder `mutex` protects each coder's state (`is_compiling`, `compile_count`, `last_compilation_time`, `has_burnout`, `ready`).
  - a per-dongle `mutex` protects the dongle wait-heap during pop operations (`use_dongles`).

- **`pthread_cond_t`** implements the event-based signaling:
  - `start_cond`: the last coder to arrive broadcasts when all coders have reached the barrier; the monitor, after setting `start_time`, broadcasts again to release everyone. This gives all threads a common clock reference before any compilation can start (a data race on `start_time` would otherwise skew timestamps and burnout deadlines).
  - `sched_cond`: coders waiting for a dongle grant call `pthread_cond_timedwait` on it; `release_dongles()` and `burn_out()` broadcast it to wake all waiters. It is the notification channel of the scheduler.

- **Custom event waiting (spurious-wakeup-safe loops).** There is no naked sleep-based polling for grants. `acquire_dongles()` registers the coder in both heaps *first* (holding `sched_mutex`), then waits inside a loop that re-evaluates the predicate `is_grantable()` with `pthread_cond_timedwait`. The timed wait acts as a watchdog so a lost or spurious wakeup can never leave a coder stuck forever: every iteration re-checks the real state of the dongles.

### How data races are avoided

Every field that crosses threads is only written and read while holding its associated mutex — e.g. the monitor reads `compile_count`, `last_compilation_time` and `has_burnout` under the coder mutex, and `has_dongles` under `sched_mutex`, while the coders update those same fields under the same locks. The mutual exclusion on the scheduler mutex also confirms the two invariants behind the approximate model: (1) only one coder is ever granted at a time, and (2) the grant decision plus the dongle `use`/`release` transitions are atomic operations. This removes the window in the classic problem where two coders each take one fork simultaneously and then wait forever.

### Thread-safe coder ↔ monitor communication

Coders and the monitor never talk directly; they share only the guarded state and the condition variables described above:

1. Coders announce their arrival by incrementing `arrived` under `start_mutex`; the monitor waits on `start_cond` until all have arrived, then stamps `start_time` and broadcasts `start_cond` to release every coder.
2. During the simulation the monitor runs a polling loop (1 ms resolution): it locks each coder mutex, evaluates `burned_out_aux`, and — on the first burnout — calls `burn_out()`, which sets `sim_end` under `sched_mutex` and broadcasts `sched_cond` so any coder blocked waiting for dongles wakes up and exits.
3. The coder threads themselves check the same flags (`sim_end`, `has_burnout`) at the top of each iteration, so they stop promptly without spinning into a data race.

The combination of mutexes (for exclusive state access), condition variables (for blocking wakes and broadcasts) and predicate-rechecking loops (as a custom event layer) makes every inter-thread communication lock-free-to-observe but race-free in practice.

## Resources

### Classic references

- **Dining Philosophers Problem** — original problem statement and classic solutions: E. W. Dijkstra / C. A. R. Hoare; introductory material on Wikipedia: <https://en.wikipedia.org/wiki/Dining_philosophers_problem>
- **Deadlock and Coffman conditions** — the four necessary conditions for deadlock: mutual exclusion, hold-and-wait, no preemption and circular wait: <https://en.wikipedia.org/wiki/Deadlock>
- **POSIX Threads Programming** — official tutorial on `pthread_create`, mutexes and condition variables from LLNL: <https://hpc-tutorials.llnl.gov/posix/>
- **`pthread_mutex` / `pthread_cond` manual pages** — canonical API reference: `man 3 pthread_mutex_lock`, `man 3 pthread_cond_timedwait`.
- **Earliest Deadline First (EDF) scheduling** — the real-time scheduling theory that inspired the `edf` policy: <https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling>
- **ThreadSanitizer** — dynamic data-race detector used to validate the design (`make sanitize`): <https://clang.llvm.org/docs/ThreadSanitizer.html>

### Use of AI

An AI assistant (opencode) was used during development to:

- **Conceptual support**: explain the theoretical background (Coffman conditions, EDF, dining philosophers variants) and reason about the correct shape of the scheduler.
- **Design and review**: help design the per-dongle priority-heap scheduler, the arrival/ready barrier, the cooldown logic and the grant predicate `is_grantable`.
- **Debugging**: interpret ThreadSanitizer reports and pinpoint the data races that motivated the per-coder mutexes and the locking order between `sched_mutex` and the coder mutexes.
- **Documentation**: produce this README.

The final implementation, including the synchronization primitives and scheduling logic, was reviewed and validated with `-fsanitize=thread` builds and manual testing before submission.
