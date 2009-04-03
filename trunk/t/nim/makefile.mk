ctests                 += t/nim/ai.t \
                          t/nim/basic.t

ptests                 += $(srcdir)/t/nim/trace.t
phelpers               += t/nim/trace 

# c tests
t_nim_ai_t_SOURCES      = t/nim/ai.c
t_nim_ai_t_LDFLAGS      = -lnim -ltap

t_nim_basic_t_SOURCES   = t/nim/basic.c
t_nim_basic_t_LDFLAGS   = -lnim -ltap

# helpers
t_nim_trace_SOURCES     = t/nim/trace.c
t_nim_trace_LDFLAGS     = -lnim 
