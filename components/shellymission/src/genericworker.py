#
# Copyright (C) 2017 by YOUR NAME HERE
#
#    This file is part of RoboComp
#
#    RoboComp is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    RoboComp is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.

import sys, Ice, os
from PySide import QtGui, QtCore

ROBOCOMP = ''
try:
	ROBOCOMP = os.environ['ROBOCOMP']
except KeyError:
	print '$ROBOCOMP environment variable not set, using the default value /opt/robocomp'
	ROBOCOMP = '/opt/robocomp'

preStr = "-I/opt/robocomp/interfaces/ -I"+ROBOCOMP+"/interfaces/ --all /opt/robocomp/interfaces/"
Ice.loadSlice(preStr+"CommonBehavior.ice")
import RoboCompCommonBehavior

additionalPathStr = ''
icePaths = [ '/opt/robocomp/interfaces' ]
try:
	SLICE_PATH = os.environ['SLICE_PATH'].split(':')
	for p in SLICE_PATH:
		icePaths.append(p)
		additionalPathStr += ' -I' + p + ' '
	icePaths.append('/opt/robocomp/interfaces')
except:
	print 'SLICE_PATH environment variable was not exported. Using only the default paths'
	pass

ice_AGMExecutive = False
for p in icePaths:
	if os.path.isfile(p+'/AGMExecutive.ice'):
		preStr = "-I/opt/robocomp/interfaces/ -I"+ROBOCOMP+"/interfaces/ " + additionalPathStr + " --all "+p+'/'
		wholeStr = preStr+"AGMExecutive.ice"
		Ice.loadSlice(wholeStr)
		ice_AGMExecutive = True
		break
if not ice_AGMExecutive:
	print 'Couln\'t load AGMExecutive'
	sys.exit(-1)
from RoboCompAGMExecutive import *
ice_AGMWorldModel = False
for p in icePaths:
	if os.path.isfile(p+'/AGMWorldModel.ice'):
		preStr = "-I/opt/robocomp/interfaces/ -I"+ROBOCOMP+"/interfaces/ " + additionalPathStr + " --all "+p+'/'
		wholeStr = preStr+"AGMWorldModel.ice"
		Ice.loadSlice(wholeStr)
		ice_AGMWorldModel = True
		break
if not ice_AGMWorldModel:
	print 'Couln\'t load AGMWorldModel'
	sys.exit(-1)
from RoboCompAGMWorldModel import *
ice_Planning = False
for p in icePaths:
	if os.path.isfile(p+'/Planning.ice'):
		preStr = "-I/opt/robocomp/interfaces/ -I"+ROBOCOMP+"/interfaces/ " + additionalPathStr + " --all "+p+'/'
		wholeStr = preStr+"Planning.ice"
		Ice.loadSlice(wholeStr)
		ice_Planning = True
		break
if not ice_Planning:
	print 'Couln\'t load Planning'
	sys.exit(-1)
from RoboCompPlanning import *
ice_ShellyMission = False
for p in icePaths:
	if os.path.isfile(p+'/ShellyMission.ice'):
		preStr = "-I/opt/robocomp/interfaces/ -I"+ROBOCOMP+"/interfaces/ " + additionalPathStr + " --all "+p+'/'
		wholeStr = preStr+"ShellyMission.ice"
		Ice.loadSlice(wholeStr)
		ice_ShellyMission = True
		break
if not ice_ShellyMission:
	print 'Couln\'t load ShellyMission'
	sys.exit(-1)
from RoboCompShellyMission import *


from shellymissionI import *

try:
	from ui_mainUI import *
except:
	print "Can't import UI file. Did you run 'make'?"
	sys.exit(-1)


class GenericWorker(QtGui.QWidget):
	kill = QtCore.Signal()


	def __init__(self, mprx):
		super(GenericWorker, self).__init__()


		self.agmexecutive_proxy = mprx["AGMExecutiveProxy"]
		self.ui = Ui_guiDlg()
		self.ui.setupUi(self)
		self.show()


		self.mutex = QtCore.QMutex(QtCore.QMutex.Recursive)
		self.Period = 30
		self.timer = QtCore.QTimer(self)


	@QtCore.Slot()
	def killYourSelf(self):
		rDebug("Killing myself")
		self.kill.emit()

	# \brief Change compute period
	# @param per Period in ms
	@QtCore.Slot(int)
	def setPeriod(self, p):
		print "Period changed", p
		Period = p
		timer.start(Period)
