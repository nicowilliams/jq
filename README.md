jq
==

jq is a command-line JSON processor.

NOTE: This branch aims to have no mutable globals outside main.c and the
      test program(s)), thus making jq suitable for building a libjq.  A
      libjq would rock my world.  We start by analyzing mutable globals
      using "nm !(main|jq_test).o|grep -i ' [bcds] '", then we figure
      out how to get rid of them.  See the ./globals file.

If you want to learn to use jq, read the documentation at
[http://stedolan.github.com/jq](http://stedolan.github.com/jq). This
documentation is generated from the docs/ folder of this repository.

If you want to hack on jq, feel free, but be warned that its internals
are not well-documented at the moment. Bring a hard hat and a shovel.

You can find some basic build instructions at
[http://stedolan.github.com/jq/download](http://stedolan.github.com/jq/download)
