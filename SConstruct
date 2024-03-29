import sys
import os
import multiprocessing
import SCons.Util

## Helper Functions

def CheckPkgConfig(context):
    context.Message('Checking for pkg-config... ')
    ret = context.TryAction('pkg-config --version')[0]
    context.Result(ret)
    return ret

def CheckPkg(context, name):
    context.Message('Checking for %s... ' % name)
    ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
    context.Result(ret)
    return ret

def CheckPkgMinVersion(context, name, version):
    context.Message('Checking %s-%s or greater... ' % (name, version))
    ret = context.TryAction('pkg-config --atleast-version \'%s\' \'%s\'' % (version, name))[0]
    context.Result(ret)
    return ret

## Configuration

opts = Variables('Local.sc')

opts.AddVariables(
    ("CC", "C Compiler", "cc"),
    ("CXX", "C++ Compiler", "clang++"),
    ("AS", "Assembler"),
    ("NUMCPUS", "Number of CPUs to use for build (0 means auto)", 0, None, int),
    EnumVariable("BUILDTYPE", "Build type", "RELEASE", ["RELEASE", "DEBUG", "PERF"]),
    BoolVariable("VERBOSE", "Show full build information", 0),
    BoolVariable("WITH_GPROF", "Include gprof profiling", 0),
    BoolVariable("WITH_GOOGLEHEAP", "Link to Google Heap Cheker", 0),
    BoolVariable("WITH_GOOGLEPROF", "Link to Google CPU Profiler", 0),
    BoolVariable("WITH_TSAN", "Enable Clang ThreadSanitizer", 0),
    BoolVariable("WITH_ASAN", "Enable Clang AddressSanitizer", 0),
    BoolVariable("WITH_USAN", "Enable Clang UndefinedBehaviorSanitizer", 0),
    BoolVariable("BUILD_BINARIES", "Build binaries", 1),
    BoolVariable("CROSSCOMPILE", "Cross compile", 0),
    PathVariable("PREFIX", "Installation target directory", "/usr/local/bin/", PathVariable.PathAccept),
    PathVariable("DESTDIR", "The root directory to install into. Useful mainly for binary package building", "", PathVariable.PathAccept),
)

env = Environment(options = opts,
                  tools = ['default'],
                  ENV = os.environ)

env["BUILDTYPE"] = "DEBUG"

Help("""TARGETS:
scons               Build\n""")
Help(opts.GenerateHelpText(env))

# Copy environment variables
if os.environ.has_key('CC'):
    env["CC"] = os.getenv('CC')
if os.environ.has_key('CXX'):
    env["CXX"] = os.getenv('CXX')
if os.environ.has_key('AS'):
    env["AS"] = os.getenv('AS')
if os.environ.has_key('LD'):
    env["LINK"] = os.getenv('LD')
if os.environ.has_key('CFLAGS'):
    env.Append(CCFLAGS = SCons.Util.CLVar(os.environ['CFLAGS']))
if os.environ.has_key('CPPFLAGS'):
    env.Append(CPPFLAGS = SCons.Util.CLVar(os.environ['CPPFLAGS']))
if os.environ.has_key('CXXFLAGS'):
    env.Append(CXXFLAGS = SCons.Util.CLVar(os.environ['CXXFLAGS']))
if os.environ.has_key('LDFLAGS'):
    env.Append(LINKFLAGS = SCons.Util.CLVar(os.environ['LDFLAGS']))
env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

#env.Append(CPPFLAGS = [ "-Wall", "-Wformat=2", "-Wextra", "-Wwrite-strings",
#                        "-Wno-unused-parameter", "-Wmissing-format-attribute",
#                        "-Werror" ])
#env.Append(CFLAGS = [ "-Wmissing-prototypes", "-Wmissing-declarations",
#                      "-Wshadow", "-Wbad-function-cast", "-Werror" ])
#env.Append(CXXFLAGS = [ "-Wno-non-template-friend", "-Woverloaded-virtual",
#                        "-Wcast-qual", "-Wcast-align", "-Wconversion",
#                        "-Weffc++", "-std=c++0x", "-Werror" ])

if env["WITH_GPROF"]:
    env.Append(CPPFLAGS = [ "-pg" ])
    env.Append(LINKFLAGS = [ "-pg" ])

env.Append(CFLAGS = ["-std=c11"])
env.Append(CXXFLAGS = ["-std=c++17"])
env.Append(CPPFLAGS = ["-Wall"])
env.Append(CPPFLAGS = ["-Wno-potentially-evaluated-expression" ])
 # "-Wsign-compare", "-Wsign-conversion", "-Wcast-align"

if env["BUILDTYPE"] == "DEBUG":
    env.Append(CPPFLAGS = [ "-g", "-DCELESTIS_DEBUG", "-O0" ])
    env.Append(LINKFLAGS = [ "-g", "-rdynamic" ])
elif env["BUILDTYPE"] == "PERF":
    env.Append(CPPFLAGS = [ "-g", "-DNDEBUG", "-DCELESTIS_PERF", "-O2"])
    env.Append(LDFLAGS = [ "-g", "-rdynamic" ])
elif env["BUILDTYPE"] == "RELEASE":
    env.Append(CPPFLAGS = ["-DNDEBUG", "-DCELESTIS_RELEASE", "-O2"])
else:
    print "Error BUILDTYPE must be RELEASE or DEBUG"
    sys.exit(-1)

if not env["VERBOSE"]:
    env["CCCOMSTR"] = "Compiling $SOURCE"
    env["CXXCOMSTR"] = "Compiling $SOURCE"
    env["SHCCCOMSTR"] = "Compiling $SOURCE"
    env["SHCXXCOMSTR"] = "Compiling $SOURCE"
    env["ARCOMSTR"] = "Creating library $TARGET"
    env["RANLIBCOMSTR"] = "Indexing library $TARGET"
    env["LINKCOMSTR"] = "Linking $TARGET"
    env["SHLINKCOMSTR"] = "Linking $TARGET"
    env["ASCOMSTR"] = "Assembling $TARGET"
    env["ASPPCOMSTR"] = "Assembling $TARGET"
    env["ARCOMSTR"] = "Creating library $TARGET"
    env["RANLIBCOMSTR"] = "Indexing library $TARGET"

def GetNumCPUs(env):
    if env["NUMCPUS"] > 0:
        return int(env["NUMCPUS"])
    return 2*multiprocessing.cpu_count()

env.SetOption('num_jobs', GetNumCPUs(env))

# Modify CPPPATH and LIBPATH
env.Append(CPPPATH = [ "/usr/local/include" ])
env.Append(LIBPATH = [ "$LIBPATH", "/usr/local/lib" ])

# XXX: Hack to support clang static analyzer
def CheckFailed():
    if os.getenv('CCC_ANALYZER_OUTPUT_FORMAT') != None:
        return
    Exit(1)

# Configuration
conf = env.Configure(custom_tests = { 'CheckPkgConfig' : CheckPkgConfig,
                                      'CheckPkg' : CheckPkg,
                                      'CheckPkgMinVersion' : CheckPkgMinVersion })

if not conf.CheckCC():
    print 'Your C compiler and/or environment is incorrectly configured.'
    CheckFailed()

env.AppendUnique(CXXFLAGS = ['-std=c++17'])
if not conf.CheckCXX():
    print 'Your C++ compiler and/or environment is incorrectly configured.'
    CheckFailed()

if (sys.platform == "win32") or env["CROSSCOMPILE"]:
    env["HAS_PKGCONFIG"] = False
else:
    env["HAS_PKGCONFIG"] = True
    if not conf.CheckPkgConfig():
        print 'pkg-config not found!'
        Exit(1)

conf.Finish()

Export('env')

# Set compile options for binaries
env.Append(CPPPATH = ['#include', '#.'])
env.Append(LIBPATH = ['#build/lib/'])
env.Append(LIBS = ["ncurses", "panel", "menu"])
if sys.platform != "win32" and sys.platform != "darwin":
    env.Append(CPPFLAGS = ['-pthread'])
    env.Append(LIBS = ["pthread"])

# Debugging Tools
if env["WITH_GOOGLEHEAP"]:
    env.Append(LIBS = ["tcmalloc"])
if env["WITH_GOOGLEPROF"]:
    env.Append(LIBS = ["profiler"])
if env["WITH_TSAN"]:
    env.Append(CPPFLAGS = ["-fsanitize=thread", "-fPIE"])
    env.Append(LINKFLAGS = ["-fsanitize=thread", "-pie"])
if env["WITH_ASAN"]:
    env.Append(CPPFLAGS = ["-fsanitize=address"])
    env.Append(LINKFLAGS = ["-fsanitize=address"])
if env["WITH_USAN"]:
    env.Append(CPPFLAGS = ["-fsanitize=undefined"])
    env.Append(LINKFLAGS = ["-fsanitize=undefined"])
if env["WITH_TSAN"] and env["WITH_ASAN"]:
    print "Cannot set both WITH_TSAN and WITH_ASAN!"
    sys.exit(-1)

# Libraries
SConscript('lib/SConscript', variant_dir='build/lib')

# Tests
SConscript('tests/SConscript', variant_dir='build/tests')

