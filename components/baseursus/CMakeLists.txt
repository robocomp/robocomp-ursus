cmake_minimum_required( VERSION 2.6 )

ADD_DEFINITIONS( -std=c++11 )
SUBDIRS( src )

INSTALL(FILES etc/config DESTINATION /opt/robocomp/etc-default/ RENAME baseursuscomp.conf )

ADD_CUSTOM_TARGET(doc doxygen Doxyfile)
ADD_CUSTOM_TARGET(installdoc mkdir -p /opt/robocomp/doc COMMAND cp -R doc/html /opt/robocomp/doc/baseursuscomp )
