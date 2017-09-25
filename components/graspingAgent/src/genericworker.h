/*
 *    Copyright (C)2017 by YOUR NAME HERE
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

#include <JointMotor.h>
#include <InverseKinematics.h>
#include <Logger.h>
#include <agm.h>

#define CHECK_PERIOD 5000
#define BASIC_PERIOD 100

typedef map <string,::IceProxy::Ice::Object*> MapPrx;

using namespace std;

using namespace RoboCompInverseKinematics;
using namespace RoboCompAGMWorldModel;
using namespace RoboCompAGMExecutive;
using namespace RoboCompPlanning;
using namespace RoboCompJointMotor;
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


	InverseKinematicsPrx inversekinematics_proxy;
	JointMotorPrx jointmotor_proxy;
	LoggerPrx logger_proxy;
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
	std::string action, backAction;
	ParameterMap previousParams;
	int iter;
	bool setParametersAndPossibleActivation(const ParameterMap &prs, bool &reactivated);
	RoboCompPlanning::Action createAction(std::string s);

	bool isNodeType(const AGMModelSymbol::SPtr &node, const std::string &query) const
	{
		return isNodeType(node->symbolType, query);
	}
	bool isNodeType(const std::string &t, const std::string &query) const
	{
		if (t == query) return true;
		try
		{
			for (auto &e : agm_types.at(query))
			{
				if (e == t)
					return true;
			}
		}
		catch (...)
		{
			printf("no %s in types\n", query.c_str());
			exit(1);
			return false;
		}
		return false;
	}

protected:
	std::map<std::string, std::vector<std::string> > agm_types;


public slots:
	virtual void compute() = 0;
signals:
	void kill();
};

#endif
