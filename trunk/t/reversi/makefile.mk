ctests                 += t/reversi/get_moves.t \
                          t/reversi/eval.t \
                          t/reversi/iterative.t \
                          t/reversi/time.t

ptests                 += $(srcdir)/t/reversi/move.t \
                          $(srcdir)/t/reversi/trace.t

phelpers               += t/reversi/move

# c tests
t_reversi_get_moves_t_SOURCES     = t/reversi/get_moves.c
t_reversi_get_moves_t_LDFLAGS     = -lreversi -ltap

t_reversi_eval_t_SOURCES          = t/reversi/eval.c
t_reversi_eval_t_LDFLAGS          = -lreversi -ltap

t_reversi_iterative_t_SOURCES     = t/reversi/iterative.c
t_reversi_iterative_t_LDFLAGS     = -lreversi -ltap

t_reversi_time_t_SOURCES          = t/reversi/time.c
t_reversi_time_t_LDFLAGS          = -lreversi -ltap

# helpers
t_reversi_move_SOURCES            = t/reversi/move.c
t_reversi_move_LDFLAGS            = -lreversi 
