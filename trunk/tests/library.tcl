source ../env.tcl

package require tcltest
namespace import -force ::tcltest::*
verbose {pass body error}

set testdir [file normalize [file dirname [info script]]]

