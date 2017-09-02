/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2008, Linkopings University,
 * Department of Computer and Information Science,
 * SE-58183 Linkoping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
 * LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
 * THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
 * PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linkopings University, either from the above address,
 * from the URL: http://www.ida.liu.se/projects/OpenModelica
 * and in the OpenModelica distribution.
 *
 * This program is distributed  WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * For more information about the Qt-library visit TrollTech's webpage
 * regarding the Qt licence: http://www.trolltech.com/products/qt/licensing.html
 */

#include "omc_communicator.h"

// STD includes
#include <cmath>
#include <iostream>
#include <stdlib.h> //This should be cstdlib, if it even should be used!!
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>

// Windows includes
#ifdef WIN32
#include "windows.h"
#endif

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QDir>
#endif

//#include "MOQtmico.h"
using namespace std;

/**
* \brief
* Creates and initializes the Omc communicator.
*/
OmcCommunicator::OmcCommunicator()
: QObject(),
omc_(0)
{
}

/**
* \brief
* Destroys the Omc communicator.
*/
OmcCommunicator::~OmcCommunicator()
{
}

/**
* \brief
* Returns a reference to the Omc communicator.
*/
OmcCommunicator& OmcCommunicator::getInstance()
{
    static OmcCommunicator instance;
    return instance;
}

/**
* \brief
* Attempts to establish a connection to Omc.
*
* \return true if a connection was established successfully or if a connection already exists,
* false otherwise.
*/
bool OmcCommunicator::establishConnection()
{
    if (omc_)
    {
        return true;
    }

    // ORB initialization.
    int argc(0); char* argv = new char[1];
    CORBA::ORB_var orb;
    orb = CORBA::ORB_init(argc, &argv);



    QFile objectRefFile;

#ifdef WIN32 // Win32
        objectRefFile.setFileName(QString(QDir::tempPath()).append(QDir::separator()).append("openmodelica.objid"));
#else // UNIX environment
        char *user = getenv("USER");
        if (!user) { user = "nobody"; }
        objectRefFile.setFileName(QString(QDir::tempPath()).append(QDir::separator()).append("openmodelica.").append(*(new QString(user))).append(".objid"));
#endif

    if (!objectRefFile.exists())
        return false;

    objectRefFile.open(QIODevice::ReadOnly);

    char buf[1024];
    objectRefFile.readLine( buf, sizeof(buf) );
    QString uri( (const char*)buf );

    CORBA::Object_var obj = orb->string_to_object( uri.trimmed().toLatin1() );

    omc_ = OmcCommunication::_narrow(obj);


    // Test if we have a connection.
    try {
        omc_->sendExpression("getClassNames()");
    }
    catch (CORBA::Exception&) {
        omc_ = 0;
        return false;
    }

    return true;
}

/**
* \brief
* Returns true if a connection has been established to Omc, false otherwise.
*/
bool OmcCommunicator::isConnected() const
{
    return omc_ != 0;
}

/**
* \brief
* Closes the connection to Omc. Omc is not terminated by closing the connection.
*/
void OmcCommunicator::closeConnection()
{
    // 2006-02-02 AF, added this code:
    omc_ = 0;
}


/**
* \brief Executes the specified Omc function and returns the return
* value received from Omc.
*
* \throw OmcError if the Omc function call fails.
* \throw OmcConnectionLost if the connection to Omc is lost.
*/
QString OmcCommunicator::callOmc(const QString& fnCall)
{
    if (!omc_) {
        //throw OmcError(fnCall);

        // 2006-02-02 AF, Added throw exception
        string msg = string("OMC-ERROR in function call: ") + fnCall.toStdString();
        throw runtime_error( msg.c_str() );
    }

    QString returnString;
    while (true) {
        try {
            returnString = omc_->sendExpression( fnCall.toLatin1() );
            break;
        }
        catch (CORBA::Exception&)
        {
            if( fnCall != "quit()" && fnCall != "quit();" )
            {
                throw runtime_error("NOT RESPONDING");
            }
            else
                break;
        }
    }

    // PORT >> returnString = returnString.stripWhiteSpace();
    returnString = returnString.trimmed();
    if (fnCall.startsWith("list(")) {
        emit omcOutput("...");
    } else {
        emit omcOutput(returnString);
    }

    if (returnString == "-1") {
        string tmp = "[Internal Error] OmcCommunicator::callOmc():\nOmc call \""
            + fnCall.toStdString() + "\" failed!\n\n";

        qWarning( tmp.c_str() );
        //throw OmcError(fnCall, returnString);
        cerr << "OmcError(" << fnCall.toStdString() << ", " << returnString.toStdString() << ")" << endl;
    }

    return returnString;
}




