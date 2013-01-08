/*
 * ngrt4n.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2012 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
# Last Update: 24-05-2012                                                 #
#                                                                          #
# This file is part of NGRT4N (http://ngrt4n.com).                         #
#                                                                          #
# NGRT4N is free software: you can redistribute it and/or modify           #
# it under the terms of the GNU General Public License as published by     #
# the Free Software Foundation, either version 3 of the License, or        #
# (at your option) any later version.                                      #
#                                                                          #
# NGRT4N is distributed in the hope that it will be useful,                #
# but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
# GNU General Public License for more details.                             #
#                                                                          #
# You should have received a copy of the GNU General Public License        #
# along with NGRT4N.  If not, see <http://www.gnu.org/licenses/>.          #
#--------------------------------------------------------------------------#
 */

#include "core/ns.hpp"
#include "client/utilsClient.hpp"
#include "client/Auth.hpp"
#include "client/SvNavigator.hpp"
#include "client/SvConfigCreator.hpp"
#include "client/ZbxHelper.hpp"
#include <sstream>
#include <getopt.h>
#include <QTranslator>
#include <QObject>

QString  usage = "usage: %1 [OPTION] [view_config]\n"
    "Options: \n"
    "	-c\n"
    "	   Launch the configuration utility\n"
    "	-e [view_config]\n"
    "	   Run the VE utility and load the file view_config if specified\n"
    "	-d view_config\n"
    "	   Run the OC utility and load the file view_config\n"
    "	-v\n"
    "	  Print the version and license information.\n"
    "	-h \n"
    "	   Print this help.\n";


int main(int argc, char **argv)
{
  QApplication* app = new QApplication(argc, argv);
  app->setWindowIcon(QIcon(":images/built-in/icon.png"));
  app->setApplicationName(appName.toUpper() );
  app->setStyleSheet(Preferences::style());

  QString cmdName = basename(argv[0]);
  ostringstream versionMsg(QObject::tr("%1\nVersion %2 (%3)\n").arg(appName).arg(packageVersion).arg(releaseName).toStdString()
                           +QObject::tr("Copyright (c) 2010-%1 by NGRT4N Project. All rights reserved.\n").arg(releaseYear).toStdString()
                           +QObject::tr("%1").arg(packageUrl).toStdString());

  QString module = "config";
  QString file = (argc >= 2)? argv[1] : "";
  int opt;
  if ( (opt = getopt(argc, argv, "chvd:e:") ) != -1) {
      switch (opt) {
        case 'c':
          module = "config";
          break;
        case 'd':
          module = "dashboard";
          file = optarg;
          break;
        case 'e':
          module = "editor";
          file = optarg;
          break;
        case 'v':
          cout << versionMsg.str()<<endl;
          exit(0);
        case 'h': {
            cout << usage.arg(cmdName).toStdString();
            exit(0);
          }
        default:
          cout << usage.arg(cmdName).toStdString();
          exit(1);
          break;
        }
    }
  cout <<"Launching "<<versionMsg.str()<<endl;
  Auth authentication;
  int userRole = authentication.exec();
  if( userRole != Auth::ADM_USER_ROLE && userRole != Auth::OP_USER_ROLE )
    exit(1);
  if(module == "dashboard") {
      QSplashScreen* info = Preferences::infoScreen(QObject::tr("Loading...\n\n%1")
                                                    .arg(QString::fromStdString(versionMsg.str())));
      utils::delay(1);
      if(file == "") {
          info->clearMessage();
          info->showMessage(QObject::tr("You need to select a configuration file!"), Qt::AlignCenter|Qt::AlignCenter);
          utils::delay(1); info->finish(0);
          file = QFileDialog::getOpenFileName(0,
                                              QObject::tr("%1 | Select a configuration file").arg(appName),
                                              ".",
                                              QObject::tr("Xml files (*.xml);;All files (*)"));

          if(!file.length()){
              utils::alert(QObject::tr("No configuration file has been selected and the program will exit!"));
              exit (1);
            }

        }
      info->finish(0);
      SvNavigator *console= new SvNavigator(userRole);
      console->load(file);
      console->startMonitor();
    } else if(module == "editor") {
      SvCreator* editor = new SvCreator(userRole);
      editor->load(file);
    } else if(module == "config") {
      Preferences* update_settings = new Preferences(userRole, Preferences::ChangeMonitoringSettings);
      Preferences* change_passwd = new Preferences(userRole, Preferences::ChangePassword);
      update_settings->exec();
      change_passwd->exec();
      exit(0);
    }
  return app->exec();
}