

theLink/tests/README
====================


The scripts used in this directory are used to verify the libmsgque
library including the the other language bindings. 
Testing is done using the tcltest feature of the tcl distribution.

- using the test feature of the build environment with 4 parallel tasks

    + cd NHI1-X.X
    + make -j 4 check 

- invoke an individual test script for language "python" and "perl"

    + cd NHI1-X.X/theLink/tests
    + Nhi1Exec int.test --only-python-perl

- invoke all tests for language "go" using "threads" and only "binary" data

    + cd NHI1-X.X/theLink/tests
    + Nhi1Exec ./all.tcl --only-go --only-threads --only-binary

- Example: start "tcl" server using "tcp" and listing on port "7777" and "spawn"
   for every new connection a new server

    + cd NHI1-X.X/theLink/tests
    + Nhi1Exec server.tcl --tcp --port 7777 --spawn

Help is available with the following commands:

- test specific help

    + cd NHI1-X.X/theLink/tests
    + Nhi1Exec ./all.tcl --help

   or

    + Nhi1Exec ./all.tcl --help-msgque

- tcltest specific help

    + man tcltest

