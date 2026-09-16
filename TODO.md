# TODO — Codexion

## 1. Estado actual
- [x] `priority.c`, `scheduler.c`, `routine.c`, `actions.c`, `pthreads.c`,
      `args_parse.c`, `coders_parse.c`, `main.c`: el proyecto compila.
- [ ] **`monitor.c` NO existe** → bloque principal.
- [ ] `main.c` no crea ni une el hilo monitor.
- [ ] `Makefile` no compila `monitor.c`.
- [ ] **Caso N=1 no cumple el subject** (hoy "compila" con un solo dongle físico).

## 2. Nuevo archivo: monitor.c
Tercer actor: un hilo que no compite por dongles; observa a todos desde fuera y decide
cuándo se detiene la simulación. Los coders dormidos en acquiere no pueden comprobarse
a sí mismos → es el monitor quien detecta el agotamiento en su lugar.

Flujo conceptual del hilo:
- Mientras la simulación esté viva, repetir: dormir ~1 ms (`usleep(1000)`) y barrer
  a todos los coders.
- **Agotamiento**: para cada coder sin dongles en la mano (`has_dongles == 0`), si
  el tiempo desde su última compilación supera `time_to_burnout`, marcarle con
  `burn_out` (log + `simulation_over = 1`). El log debe salir ≤10 ms del agotamiento
  real → de ahí el ciclo de 1 ms.
- **Finalización**: si todos los coders tienen `compile_count >=
  number_of_compiles_required`, poner `simulation_over = 1`.
- **Despertar a los dormidos**: tras poner `simulation_over`, `broadcast` en
  `sched_cond` (con mutex tomado) para que los que esperan en `acquire_dongles`
  salgan por su camino de aborto (0) y terminen sus hilos → `join_pthreads` retorna.

Prototipos:
- Header (lo usa main): `void	*monitor_routine(void *arg);` — recibe el array
  NULL-terminado de coders (`t_coder **`).
- Helper interna `static` (opcional) para decidir agotamiento y otra para el fin
  global. Ej.: `static int	coder_burned_out(t_coder *coder);`
      `static int	all_have_compiled(t_coder **coders);`

## 3. main.c — integración
- Declarar un `pthread_t` para el monitor.
- Antes de `initialize_pthreads`, crear el hilo monitor pasándole el array de coders.
- Tras `join_pthreads`, `pthread_join` del monitor.
- Razón: el monitor debe estar vivo desde el primer ms o un coder podría agotarse
  sin que nadie lo detecte.

## 4. Caso N=1 — cumplir el subject (es.md:271: 1 persona = 1 dongle; es.md:145:
   compilar exige DOS dongles; entonces la persona nunca compila y el monitor la agota)
- Hoy `left_dongle == right_dongle` (el mismo dongle físico) y `is_grantable` lo da
  por bueno con un solo dongle → la persona "compila". Incumple el subject.
- **Fijación de plan**: impedir la concesión cuando ambos dongles son el mismo.
  Ej. de cómo encaja sin tocar el resto: al inicio de `is_grantable`, si
  `l_dongle == r_dongle`, responder "no concedible" (0). Con eso el coder nunca
  es concedido, espera en acquiere (timedwait ya re-chequea cada ms), y el monitor
  lo agota al superar `time_to_burnout` → simulación termina por burnout. (El doble
  push al mismo heap es inofensivo en este caso, no hay más coders.)
- Decidir también si `acquire` debe evitar el doble push cuando izq == der (limpieza).

## 5. ft_codexion.h — prototipos que faltan
- [ ] `long long	get_priority(t_coder *coder);` (coherencia; hoy solo vive en scheduler.c).
- [ ] `void	*monitor_routine(void *arg);` (tras crear monitor.c).

## 6. Makefile
- [ ] Añadir `monitor.c` a `SOURCES`.

## 7. Retoques (no bloquean compilación)
- [ ] **Doble log de burnout**: al despertar por fin de simulación, el self-check de
      la rutina (routine.c:56-57) puede disparar un segundo "burned out". Saltarse el
      self-check si `simulation_over` ya está activo (el monitor ya se encargó).
- [ ] `free_coder_list`: `heap_destroy` sobre cada dongle antes de liberarlo
      (requisito del subject: sin fugas).
- [ ] `validate_args`: `i != 1` excluye erróneamente a `number_of_coders` de la
      validación de negativos; incluir el índice 1.
- [ ] Confirmar que `put_args_in_simul_data` rechaza también `number_of_compiles_required`
      y demás no-enteros (solo `atoi` al día → "3abc" pasaría; `atoi("3abc") == 3`).

## 8. Norma
- [ ] `norminette` en todo.
- [ ] Candidatos a >80 cols: routine.c:45-47, 53-57; scheduler.c:56-58 y 77-78.
- [ ] `get_priority` al header (o `static` si queda local).

## 9. Verificación final
- [ ] `make` limpio.
- [ ] Caso completable (burnout generoso): todos terminan, el monitor pone fin, sin cuelgues.
- [ ] Caso agotamiento (burnout corto): log ≤10 ms, simulación se detiene.
- [ ] **N=1**: con 1 solo dongle, la persona se agota y el programa termina.
- [ ] fifo y edf.
- [ ] Valgrind sin fugas.