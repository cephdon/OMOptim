// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file InfoSender.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#if !defined(_INFOSENDER_H)
#define _INFOSENDER_H

#include "Info.h"
#include <QTextStream>
#include <QTime>

/** InfoSender is used to send information to GUI or to console.
It contains a single instance shared within entire project*/

class InfoSender : public QObject
{
	Q_OBJECT


public:
    static InfoSender* instance();
    void setLogStream(QTextStream* logStream);
    ~InfoSender(void);

public slots :
    void send(Info);
    void debug(QString msg){send(Info(msg,ListInfo::INFODEBUG));}
    void sendWarning(QString msg){send(Info(msg,ListInfo::WARNING2));}
    void sendError(QString msg){send(Info(msg,ListInfo::ERROR2));}
    void sendNormal(QString msg){send(Info(msg,ListInfo::NORMAL2));}


signals :
    void sent(Info);
    void setCurrentTask(QString);
    void increaseTaskProgress();
    void noCurrentTask();

private :
    QTextStream* _logStream;
    InfoSender();
    static InfoSender* _instance;
};
//Q_DECLARE_METATYPE(InfoSender)
//Q_DECLARE_INTERFACE(InfoSender,"com.OMOptim.InfoSender/1.0")

#endif