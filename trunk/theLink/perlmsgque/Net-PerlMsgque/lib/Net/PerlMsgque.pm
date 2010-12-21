package Net::PerlMsgque;

use 5.010000;
use strict;
use warnings;

require Exporter;
use AutoLoader qw(AUTOLOAD);

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Net::PerlMsgque ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '4.8';

require XSLoader;
XSLoader::load('Net::PerlMsgque', $VERSION);

# Initialize 'Sys'
Net::PerlMsgque::InitializeSys();

# Preloaded methods go here.
Net::PerlMsgque::Init($^X, "-w", $0);

# set default Factory
Net::PerlMsgque::FactoryDefault("perlmsgque", "Net::PerlMsgque::MqS");

# Autoload methods go after =cut, and are processed by the autosplit program.

package Net::PerlMsgque;
use constant {
  START_DEFAULT	  => 0,
  START_FORK	  => 1,
  START_THREAD	  => 2,
  START_SPAWN	  => 3,
  TIMEOUT_DEFAULT => -1,
  TIMEOUT_USER	  => -2,
  TIMEOUT_MAX	  => -3,
  WAIT_NO	  => 0,
  WAIT_ONCE	  => 1,
  WAIT_FOREVER	  => 2,
};

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

Net::PerlMsgque - Perl extension to setup a perl application server

=head1 SYNOPSIS

  use Net::PerlMsgque;
  package MyServer;
  use base qw(Net::PerlMsgque::MqS);

=head1 DESCRIPTION

The msgque project is an infrastructure to link software together to act like a single software. To link mean distributing work
from one software to an other software an wait or not wait for an answer. The linking is done using unix or inet domain sockets
and is based on packages send from one software to an other software and back. The msgque project is used to handle all the
different aspects for setup and maintain the link and is responsible for:

 * starting and stopping the server application
 * starting and stopping the communication interface
 * sending and receiving package data
 * reading and writing data from or into a package
 * setup and maintain the event-handling for an asynchronous transfer
 * propagate the error messages from the server to the client

=head2 EXPORT

None by default.

=head1 SEE ALSO

The original documentation is available with :

  > perlmsgque(n)

or on the internet at:

  http://libmsgque.sourceforge.net/

In addition other languages are supported also:

  C        > libmsgque(n)
  C++      > ccmsgque(n)
  C#       > csmsgque(n)
  JAVA     > javamsgque(n)
  PYTHON   > pymsgque(n)
  TCL      > tclmsgque(n)
  VB.NET   > vbmsgque(n)

=head1 AUTHOR

Andreas Otto, E<lt>aotto1968@users.sourceforge.net<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2009 by Andreas Otto

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.10.1 or,
at your option, any later version of Perl 5 you may have available.


=cut
