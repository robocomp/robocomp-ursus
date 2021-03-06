/*
 *    Copyright (C) 2006-2010 by RoboLab - University of Extremadura
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GENERICWORKER_H
#define GENERICWORKER_H

// #include <ipp.h>
#include "config.h"
#include <QtGui>
#include <stdint.h>
#include <qlog/qlog.h>
#include <CommonBehavior.h>
#include <ui_guiDlg.h>
#include "config.h"
#include <JointMotor.h>
#include <JointMotor.h>

#define CHECK_PERIOD 5000
#define BASIC_PERIOD 200

typedef map <string,::IceProxy::Ice::Object*> MapPrx;

using namespace std;

/**
       \brief
       @author authorname
*/
using namespace RoboCompJointMotor;
using namespace RoboCompJointMotor;
class GenericWorker :
#ifdef USE_QTGUI
public QWidget, public Ui_guiDlg
#else
public QObject
#endif
{
Q_OBJECT
public:
	GenericWorker(MapPrx& mprx);
	virtual ~GenericWorker();
	virtual void killYourSelf();
	virtual void setPeriod(int p);
	
	virtual bool setParams(RoboCompCommonBehavior::ParameterList params) = 0;
	QMutex *mutex;                //Shared mutex with servant

	JointMotorPrx jointmotor0_proxy;
	JointMotorPrx jointmotor1_proxy;
	virtual void  setPosition(const MotorGoalPosition& goal) = 0;
	virtual void  setVelocity(const MotorGoalVelocity& goal) = 0;
	virtual void  setZeroPos(const string& name) = 0;
	virtual void  setSyncPosition(const MotorGoalPositionList& listGoals) = 0;
	virtual void  setSyncVelocity(const MotorGoalVelocityList& listGoals) = 0;
	virtual void  setSyncZeroPos() = 0;
	virtual MotorParams getMotorParams(const string& motor) = 0;
	virtual MotorState getMotorState(const string& motor) = 0;
	virtual MotorStateMap getMotorStateMap(const MotorList& mList) = 0;
	virtual void  getAllMotorState(MotorStateMap& mstateMap) = 0;
	virtual MotorParamsList getAllMotorParams() = 0;
	virtual BusParams getBusParams() = 0;
protected:
	QTimer timer;
	int Period;
public slots:
	virtual void compute() = 0;
signals:
	void kill();
};

#endif