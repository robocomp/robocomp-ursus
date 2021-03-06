/*
 *    Copyright (C) 2017 by YOUR NAME HERE
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

#include "config.h"
#include <QtGui>
#include <stdint.h>
#include <qlog/qlog.h>

#include <ui_mainUI.h>

#include <CommonBehavior.h>

#include <TrajectoryRobot2D.h>
#include <Logger.h>
#include <OmniRobot.h>
#include <GenericBase.h>
#include <agm.h>

#define CHECK_PERIOD 5000
#define BASIC_PERIOD 100

typedef map <string,::IceProxy::Ice::Object*> MapPrx;

using namespace std;

using namespace RoboCompTrajectoryRobot2D;
using namespace RoboCompAGMWorldModel;
using namespace RoboCompOmniRobot;
using namespace RoboCompGenericBase;
using namespace RoboCompAGMExecutive;
using namespace RoboCompPlanning;
using namespace RoboCompLogger;
using namespace RoboCompAGMCommonBehavior;


struct BehaviorParameters
{
	RoboCompPlanning::Action action;
	std::vector< std::vector <std::string> > plan;
};



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
	QMutex *mutex;
	bool activate(const BehaviorParameters& parameters);
	bool deactivate();
	bool isActive() { return active; }


	LoggerPrx logger_proxy;
	TrajectoryRobot2DPrx trajectoryrobot2d_proxy;
	OmniRobotPrx omnirobot_proxy;
	AGMExecutivePrx agmexecutive_proxy;

	virtual bool reloadConfigAgent() = 0;
	virtual bool activateAgent(const ParameterMap &prs) = 0;
	virtual bool setAgentParameters(const ParameterMap &prs) = 0;
	virtual ParameterMap getAgentParameters() = 0;
	virtual void killAgent() = 0;
	virtual int uptimeAgent() = 0;
	virtual bool deactivateAgent() = 0;
	virtual StateStruct getAgentState() = 0;
	virtual void structuralChange(const RoboCompAGMWorldModel::World &w) = 0;
	virtual void edgesUpdated(const RoboCompAGMWorldModel::EdgeSequence &modifications) = 0;
	virtual void edgeUpdated(const RoboCompAGMWorldModel::Edge &modification) = 0;
	virtual void symbolUpdated(const RoboCompAGMWorldModel::Node &modification) = 0;
	virtual void symbolsUpdated(const RoboCompAGMWorldModel::NodeSequence &modifications) = 0;

protected:
	QTimer timer;
	int Period;
	bool active;
	AGMModel::SPtr worldModel;
	BehaviorParameters p;
	ParameterMap params;
	int iter;
	bool setParametersAndPossibleActivation(const ParameterMap &prs, bool &reactivated);
	RoboCompPlanning::Action createAction(std::string s);

private:
	std::map<std::string, std::vector<std::string> > agm_types;

public slots:
	virtual void compute() = 0;
signals:
	void kill();
};

#endif

