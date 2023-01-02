project('cadical', 'cpp',
  version : '1.5.2',
  default_options : ['warning_level=3'])

src = [
  'src/analyze.cpp',
  'src/arena.cpp',
  'src/assume.cpp',
  'src/averages.cpp',
  'src/backtrack.cpp',
  'src/backward.cpp',
  'src/bins.cpp',
  'src/block.cpp',
  # 'src/cadical.cpp',
  'src/ccadical.cpp',
  'src/checker.cpp',
  'src/clause.cpp',
  'src/collect.cpp',
  'src/compact.cpp',
  'src/condition.cpp',
  'src/config.cpp',
  'src/constrain.cpp',
  'src/contract.cpp',
  'src/cover.cpp',
  'src/decide.cpp',
  'src/decompose.cpp',
  'src/deduplicate.cpp',
  'src/elim.cpp',
  'src/ema.cpp',
  'src/extend.cpp',
  'src/external.cpp',
  'src/file.cpp',
  'src/flags.cpp',
  'src/format.cpp',
  'src/gates.cpp',
  'src/instantiate.cpp',
  'src/internal.cpp',
  'src/ipasir.cpp',
  'src/limit.cpp',
  'src/logging.cpp',
  'src/lookahead.cpp',
  'src/lucky.cpp',
  'src/message.cpp',
  'src/minimize.cpp',
  # 'src/mobical.cpp',
  'src/occs.cpp',
  'src/options.cpp',
  'src/parse.cpp',
  'src/phases.cpp',
  'src/probe.cpp',
  'src/profile.cpp',
  'src/proof.cpp',
  'src/propagate.cpp',
  'src/queue.cpp',
  'src/random.cpp',
  'src/reap.cpp',
  'src/reduce.cpp',
  'src/rephase.cpp',
  'src/report.cpp',
  'src/resources.cpp',
  'src/restart.cpp',
  'src/restore.cpp',
  'src/score.cpp',
  'src/shrink.cpp',
  'src/signal.cpp',
  'src/solution.cpp',
  'src/solver.cpp',
  'src/stats.cpp',
  'src/subsume.cpp',
  'src/terminal.cpp',
  'src/ternary.cpp',
  'src/tracer.cpp',
  'src/transred.cpp',
  'src/util.cpp',
  'src/var.cpp',
  'src/version.cpp',
  'src/vivify.cpp',
  'src/walk.cpp',
  'src/watch.cpp',
]

# Generate 'build.hpp' using script:
build_hpp = custom_target(
  output : 'build.hpp',
  command : 'scripts/make-build-header.sh',
  capture : true,
)

if get_option('buildtype').startswith('release')
    add_project_arguments('-DNDEBUG', language : 'cpp')
endif

code = '''
#include <cstdio>
int main () {
  const char * path = "$feature.log";
  FILE * file = fopen (path, "w");
  if (!file) return 1;
  if (putc_unlocked (42, file) != 42) return 1;
  if (fclose (file)) return 1;
  file = fopen (path, "r");
  if (!file) return 1;
  if (getc_unlocked (file) != 42) return 1;
  if (fclose (file)) return 1;
  return 0;
}
'''
compiler = meson.get_compiler('cpp')
result = compiler.run(code, name : 'unlocked io')
if result.compiled() and result.returncode() == 0
  add_project_arguments('-DUNLOCKED', language : 'cpp')
else
  add_project_arguments('-DNUNLOCKED', language : 'cpp')
endif

link_args = []
# Link 'psapi' on Windows:
if host_machine.system() == 'windows'
  link_args += '-lpsapi'
endif
lib = library('cadical', src, build_hpp,
  install : true, link_args : link_args)

exe_cadical = executable('cadical', 'src/cadical.cpp', build_hpp,
  install : true, link_with : lib)

# Exclude 'mobical' on Windows:
if host_machine.system() != 'windows'
exe_mobical = executable('mobical', 'src/mobical.cpp', build_hpp,
  install : true, link_with : lib)
endif

install_headers(
  'src/ccadical.h',
  'src/ipasir.h',
  'src/arena.hpp',
  'src/averages.hpp',
  'src/bins.hpp',
  'src/block.hpp',
  'src/cadical.hpp',
  'src/checker.hpp',
  'src/clause.hpp',
  'src/config.hpp',
  'src/contract.hpp',
  'src/cover.hpp',
  'src/elim.hpp',
  'src/ema.hpp',
  'src/external.hpp',
  'src/file.hpp',
  'src/flags.hpp',
  'src/format.hpp',
  'src/heap.hpp',
  'src/instantiate.hpp',
  'src/internal.hpp',
  'src/inttypes.hpp',
  'src/level.hpp',
  'src/limit.hpp',
  'src/logging.hpp',
  'src/message.hpp',
  'src/observer.hpp',
  'src/occs.hpp',
  'src/options.hpp',
  'src/parse.hpp',
  'src/phases.hpp',
  'src/profile.hpp',
  'src/proof.hpp',
  'src/queue.hpp',
  'src/radix.hpp',
  'src/random.hpp',
  'src/range.hpp',
  'src/reap.hpp',
  'src/reluctant.hpp',
  'src/resources.hpp',
  'src/score.hpp',
  'src/signal.hpp',
  'src/stats.hpp',
  'src/terminal.hpp',
  'src/tracer.hpp',
  'src/util.hpp',
  'src/var.hpp',
  'src/version.hpp',
  'src/vivify.hpp',
  'src/watch.hpp',
  subdir : 'cadical',
)
