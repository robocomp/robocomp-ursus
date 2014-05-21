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
 
 #include "specificworker.h"
#include <specificworker.h>

/**
* \brief Default constructor
*/

SpecificWorker::SpecificWorker(MapPrx& mprx) : GenericWorker(mprx)	
{	
	connect(ejecutarButton,SIGNAL(clicked()), this, SLOT(ejecutar()));
	connect(camareroButton,SIGNAL(clicked()), this, SLOT(moveCamarero()));
	connect(homeButton,SIGNAL(clicked()), this, SLOT(goHome()));
	connect(closeButton,SIGNAL(clicked()), this, SLOT(closeFingers()));
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{

}
void SpecificWorker::compute( )
{
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
	// ¡CUIDADO CON EL INNERMODEL! Debe ser el mismo que LOKIARM!!!
	try
	{
		RoboCompCommonBehavior::Parameter par = params.at("BIK.InnerModel") ;
		if( QFile(QString::fromStdString(par.value)).exists() == true)
		{
			qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << "Reading Innermodel file " << QString::fromStdString(par.value);
			innerModel = new InnerModel(par.value);
			qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << "Innermodel file read OK!" ;		
		}
		else
		{	qDebug() << __FILE__ << __FUNCTION__ << __LINE__ << "Innermodel file " << QString::fromStdString(par.value) << " does not exists";
			qFatal("Exiting now.");
		}
	}
	catch(std::exception e)
	{
		qFatal("Error reading config params");
	}

	timer.start(Period);
	return true;
};

/**
 * @brief SLOT ejecutar. Crea el target que el usuario ha especificado en la interfaz y se
 * lo pasa a lokiArm para que lo ejecute.
 * @return void
 * */
void SpecificWorker::ejecutar()
{
	
	// Sacamos la POSE de la GUI:
	QVec pose (6);
	pose[0] = poseTx->value();
	pose[1] = poseTy->value();
	pose[2] = poseTz->value();
	pose[3] = poseRx->value();
	pose[4] = poseRy->value();
	pose[5] = poseRz->value();
		
	//Movemos el target en RCIS
	QVec nPose = pose;
	nPose[0] = pose[0]/1000;
	nPose[1] = pose[1]/1000;
	nPose[2] = pose[2]/1000;
	moverTargetEnRCIS(nPose);
	
	QString tip = tipBox->currentText();
	
	QVec pesos(6); pesos.set((T)0);
	if(WTx->isChecked()) pesos[0] = 1;
	if(WTy->isChecked()) pesos[1] = 1;
	if(WTz->isChecked()) pesos[2] = 1;
	if(WRx->isChecked()) pesos[3] = 1;
	if(WRy->isChecked()) pesos[4] = 1;
	if(WRz->isChecked()) pesos[5] = 1;
	
	QString type = typeBox->currentText();		
	Target::TargetType tt;
	if(type == "POSE6D")
		tt = Target::POSE6D;
	if(type == "ALIGNAXIS")
		tt = Target::ALIGNAXIS;
	
	std::string part = partBox->currentText().toStdString();
	
	// 		void  setTargetPose6D(const string& bodyPart, const Pose6D& target, const WeightVector& weights);
	Target t(innerModel, pose, tip, pesos, tt); //no me deja poner el tipo de target...
	
	RoboCompBodyInverseKinematics::Pose6D pose6D;
	pose6D.x = pose[0];		pose6D.y = pose[1];		pose6D.z = pose[2];
	pose6D.rx = pose[3];	pose6D.ry = pose[4];	pose6D.rz = pose[5];
	
	RoboCompBodyInverseKinematics::WeightVector weights;
	weights.x = pesos[0];	weights.y = pesos[1]; 	weights.z = pesos[2];	weights.rx = pesos[3];	weights.ry = pesos[4];	weights.rz = pesos[5];
	
	qDebug()<<"POSE: "<<pose;
	qDebug()<<"POSE 6D: "<<pose6D.x<<pose6D.y<<pose6D.z<<pose6D.rx<<pose6D.ry<<pose6D.rz;
	qDebug()<<"TIP: "<<tip;
	qDebug()<<"PESOS: "<<pesos;
	qDebug()<<"TYPE: "<< type;
	qDebug()<<"PART: "<< QString::fromStdString(part);

	if(type == "POSE6D")	
	{
		try
		{
			bodyinversekinematics_proxy->setTargetPose6D(part, pose6D, weights );
		}
		catch(Ice::Exception ex){
			std::cout<<"Error al pasar el target: "<<ex<<endl;
		}
	}
}

/*
 * Método moverTarget versión 2.
 * Mueve el target a una posición que se le pasa como parámetro de entrada. 
 * Crea una pose3D a cero y actualiza sus traslaciones tx, ty y tz y sus 
 * rotaciones rx, ry y rz con los datos del parámetro de entrada.
 * Sirve para colocar el target en el innerModel. Para nada más.
 */ 
void SpecificWorker::moverTargetEnRCIS(const QVec &pose)
{
	try
	{
		RoboCompInnerModelManager::Pose3D p;
		p.x=p.y=p.z=p.rx=p.ry=p.rz=0.0; //Primero inicializamos a cero.

		p.x = pose[0]; p.y = pose[1]; p.z = pose[2];
		p.rx = pose[3]; p.ry = pose[4]; p.rz = pose[5];
			
		innermodelmanager_proxy->setPoseFromParent("target",p);
		innerModel->updateTransformValues("target",p.x,p.y,p.z,p.rx,p.ry,p.rz);
		}
	catch (const Ice::Exception &ex) 
	{
		cout<<"Excepción en moverTarget: "<<ex<<endl;
	}
}



/**
 * @brief Create a sequence of targets simulating a person handling a tray
 * 
 * @return void
 */
void SpecificWorker::moveCamarero()
{
    QVec pesos(6);
    QVec pose = QVec::zeros(6);
    float xAux, yAux;
	QQueue<QVec> poses;
   
    // lado inferior en X 1:
	for(float i=-0.15; i<=0.15; i=i+0.01)
	{
		pose[0] = i; pose[1] = 0.9; pose[2] = 0.350;
		poses.enqueue(pose);
		xAux = i;
	}

	// Y 1:
	for(float j=0.9; j<1.10; j=j+0.01)
	{
		pose[0] = xAux; pose[1] = j; pose[2] = 0.350;
		poses.enqueue(pose);
		yAux = j;
	}

	// X 2:
	for(float i=xAux; i>=-0.15; i=i-0.01)
	{
		pose[0] = i; pose[1] = yAux; pose[2] = 0.35;
		poses.enqueue(pose);
		xAux=i;
	}
	// Y 2:
	for(float j=yAux; j>=0.9; j=j-0.01)
	{
		pose[0] = xAux; pose[1] = j; pose[2] = 0.35;
		poses.enqueue(pose);
		yAux = j;
	}
	
    foreach(QVec p, poses)
    {
		try
        {
			moverTargetEnRCIS(p);
            pose[0] = p[0] * (T)1000;
            pose[1] = p[1] * (T)1000;
            pose[2] = p[2] * (T)1000;
			
            RoboCompBodyInverseKinematics::Pose6D pose6D;
            pose6D.x = pose[0];     pose6D.y = pose[1];     pose6D.z = pose[2];
            pose6D.rx = pose[3];    pose6D.ry = pose[4];    pose6D.rz = pose[5];
			
			pesos.set((T)0);
			if(WTx->isChecked()) pesos[0] = 1;	if(WTy->isChecked()) pesos[1] = 1;	if(WTz->isChecked()) pesos[2] = 1;
			if(WRx->isChecked()) pesos[3] = 1;	if(WRy->isChecked()) pesos[4] = 1;	if(WRz->isChecked()) pesos[5] = 1;
            RoboCompBodyInverseKinematics::WeightVector weights;
            weights.x = pesos[0];    weights.y = pesos[1];     weights.z = pesos[2];    weights.rx = pesos[3];    weights.ry = pesos[4];    weights.rz = pesos[5];
   
			std::string part = partBox->currentText().toStdString();
			QString type = typeBox->currentText();		
			
			if(part == "HEAD")
				bodyinversekinematics_proxy->pointAxisTowardsTarget(part, pose6D, "z", false, 0 );
			else if(type == "POSE6D") 				
				bodyinversekinematics_proxy->setTargetPose6D(part, pose6D, weights );
// 			else if(type == "ADVANCEALONGAXIS")
// 				bodyinversekinematics_proxy->advanceAlongAxis(part, axis, dist);
			
            usleep(50000);
        }
        catch(Ice::Exception ex)
        {
            std::cout<<"Error al pasar el target: "<<ex<<endl;
        }
    }
}

void SpecificWorker::goHome()
{
	try 
	{	
		std::string part = partBox->currentText().toStdString();
		qDebug() << "Go gome" << QString::fromStdString(part);
		bodyinversekinematics_proxy->goHome(part);
	} 
	catch (Ice::Exception ex) 
	{
		cout << ex << endl;
	}
}


void SpecificWorker::closeFingers()
{
	try 
	{	
		qDebug() << "Close/Open fingers";
		if(closeButton->isChecked())
			bodyinversekinematics_proxy->setFingers(0);
		else
			bodyinversekinematics_proxy->setFingers(1);
	} 
	catch (Ice::Exception ex) 
	{
		cout << ex << endl;
	}
}










/////CODIGO DE LOKIARM PARA INICIALIZAR DESDE EL GENERADOR

//CREA LA LISTA DE TARGETS PARA CADA PARTE DEL CUERPO: necesita innerModel y el bodypart
	/*listaTargetsBrazoDerecho = generador.generarListaTargets(innerModel, bodyParts.value("RIGHTARM"));
	bodyParts["RIGHTARM"].addListaTarget(listaTargetsBrazoDerecho);
	
	listaTargetsBrazoIzquierdo = generador.generarListaTargets(innerModel, bodyParts.value("LEFTARM")); //añadido
	bodyParts["LEFTARM"].addListaTarget(listaTargetsBrazoIzquierdo);*/
	
	
	//USAMOS LA INTERFAZ DE ICE
// 	QVec w(6);
// 	w.set(1.f);
	
// 	listaTargetsBrazoDerecho = generador.generarListaTargets(innerModel, bodyParts.value("RIGHTARM"), w);
// 	bodyParts["RIGHTARM"].addListaTarget(listaTargetsBrazoDerecho);
// 	
//  	listaTargetsBrazoIzquierdo = generador.generarListaTargets(innerModel, bodyParts.value("LEFTARM"), w); 
// 	foreach(Target t, listaTargetsBrazoIzquierdo)
// 	{
// 		RoboCompBodyInverseKinematics::Pose6D pose;
// 		RoboCompBodyInverseKinematics::WeightVector weights;
// 		pose.x = t.getPose().x();pose.y = t.getPose().y();pose.z = t.getPose().z();
// 		pose.rx = t.getPose().rx();pose.ry = t.getPose().ry();pose.rz = t.getPose().rz();
// 		weights.x = t.getWeights().x();	weights.y = t.getWeights().y();	weights.z = t.getWeights().z();
// 		weights.rx = t.getWeights().rx();	weights.ry = t.getWeights().ry(); weights.rz = t.getWeights().rz();
// 		this->setTargetPose6D("LEFTARM", pose, weights);
// 	}
// 	qDebug() << "Size lista  " << bodyParts.value("LEFTARM").getListaTargets().size();
	
  //bodyParts["LEFTARM"].addListaTarget(listaTargetsBrazoIzquierdo);
// 	listaTargetsCabeza = generador.generarListaTargets(innerModel, bodyParts.value("HEAD"),w); //añadido
// 	foreach(Target t, listaTargetsCabeza)
// 	{
// 		RoboCompBodyInverseKinematics::Pose6D pose;
// 		pose.x = t.getPose().x();pose.y = t.getPose().y();pose.z = t.getPose().z();
// 		pose.rx = t.getPose().rx();pose.ry = t.getPose().ry();pose.rz = t.getPose().rz();
// 		this->pointAxisTowardsTarget("HEAD", pose, "z", true, 0.f);		
// 	}
	
	//bodyParts["HEAD"].addListaTarget(listaTargetsCabeza);
