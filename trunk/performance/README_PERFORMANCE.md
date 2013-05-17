
performance/README
==================

The performance test ist used to check the IO performance of
LibMsgque together with a various programming languages.
The results are placed into the __doc__ subdirectory.

The results are available at @ref performance
    
    usage: make (thread|nothread|perftest) ?arg=...|ALL?

    thread ..... build thread code
    nothread ... build no-thread code
    perftest ... make the performance test


build_performance_env.sh
------------------------

build and install the libmsgque library in the subdirectory "nothread"
and "thread". This step is REQUIRED before doing the test with 
"performance.sh"

performance.sh
--------------

do the performance test, no additional argument is equal to do all tests.

only do perl tests:

    > ./performance.sh perl

only do java thread tests:

    > ./performance.sh "java*thread"

get all tests accepted by the command-line:

    > ./performance.sh --help-tests

