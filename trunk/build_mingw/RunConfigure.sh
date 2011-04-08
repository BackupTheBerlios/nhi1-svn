set -x

# Java -------------------------------------------------------------------------------------------

export JAVA_HOME=/cygdrive/c/Programme/Java/jdk1.6.0_24/
export PATH="$JAVA_HOME/bin:$PATH"

# CSharp -----------------------------------------------------------------------------------------

CSDIR=/cygdrive/c/Windows/Microsoft.NET/Framework64
FrameworkDir=$(cygpath -w $CSDIR)
FrameworkVersion=v4.0.30319
PATH=$CSDIR/$FrameworkVersion:'/cygdrive/c/Program Files/Microsoft SDKs/Windows/v7.1/Bin/x64':$PATH

# Main -------------------------------------------------------------------------------------------

export CC="ccache x86_64-w64-mingw32-gcc"
export CXX="ccache x86_64-w64-mingw32-g++"

rm -fr /tmp/libmsgque-install

if [[ $ENVHOME == *nothread* ]] ; then
  ARGS=""
else
  ARGS=--enable-threads
fi

bash ../configure   --prefix=/tmp/libmsgque-install \
		    --build=i686-pc-cygwin \
		    --host=x86_64-w64-mingw32 \
		    --enable-tcl \
		    --enable-cxx \
		    --enable-java \
		    --enable-csharp \
		    --enable-vb \
\
		    $ARGS $@

