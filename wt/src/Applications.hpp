/*
 * RealOpInsightQApp.hpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2012 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
# Last Update : 06-10-2014                                                 #
#                                                                          #
# This file is part of RealOpInsight (http://RealOpInsight.com) authored   #
# by Rodrigue Chakode <rodrigue.chakode@gmail.com>                         #
#                                                                          #
# RealOpInsight is free software: you can redistribute it and/or modify    #
# it under the terms of the GNU General Public License as published by     #
# the Free Software Foundation, either version 3 of the License, or        #
# (at your option) any later version.                                      #
#                                                                          #
# The Software is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
# GNU General Public License for more details.                             #
#                                                                          #
# You should have received a copy of the GNU General Public License        #
# along with RealOpInsight.  If not, see <http://www.gnu.org/licenses/>.   #
#--------------------------------------------------------------------------#
 */

#ifndef REALOPINSIGHTQAPP_HPP
#define REALOPINSIGHTQAPP_HPP
#include <QApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include "WQApplication"
#include "WebUtils.hpp"
#include "DbSession.hpp"
#include "AuthManager.hpp"
#include "QosCollector.hpp"


class RealOpInsightQApp : public QCoreApplication
{
public:
  RealOpInsightQApp(int& argc, char ** argv) : QCoreApplication(argc, argv) { }
  virtual ~RealOpInsightQApp() { }

  virtual bool notify(QObject * receiver, QEvent * event) {
    try {
      return QCoreApplication::notify(receiver, event);
    } catch(std::exception& ex) {
      LOG("fatal", ex.what());
    }
    return false;
  }
};

class WebApp : public Wt::WQApplication
{
public:
  WebApp(const Wt::WEnvironment& env)
    : WQApplication(env, true) {}

protected:
  virtual void create()
  {
#ifdef REALOPINSIGHT_WEB_FASTCGI
    m_dirroot = "";
    m_docroot = "";
#else
    m_dirroot = "/";
    m_docroot = docRoot() +  m_dirroot;
#endif

    setTwoPhaseRenderingThreshold(0);
    useStyleSheet(m_dirroot+"resources/css/ngrt4n.css");
    useStyleSheet(m_dirroot+"resources/css/font-awesome.min.css");
    messageResourceBundle().use(m_docroot+"resources/i18n/messages");
    setTheme(new Wt::WBootstrapTheme());
    requireJQuery(m_dirroot+"resources/js/jquery-1.10.2.min.js");

#ifdef ENABLE_ANALYTICS
    require(m_dirroot+"resources/js/ga.js");
#endif

    m_dbSession = new DbSession();
    root()->setId("wrapper");
    root()->addWidget(new AuthManager(m_dbSession));
  }

  virtual void destroy()
  {
    delete m_dbSession;
  }

private:
  DbSession* m_dbSession;
  std::string m_dirroot;
  std::string m_docroot;
};

class ReportCollectorApp : public Wt::WQApplication
{
public:
  ReportCollectorApp(const Wt::WEnvironment& env)
    : WQApplication(env, true) {}

  DbSession* dbSession(void) {return m_dbSession;}

protected:
  virtual void create()
  {
    m_dbSession = new DbSession();

    // only accept request from local loop
    std::string clientIp = environment().clientAddress();
    if (clientIp != "127.0.0.1" && clientIp != "localhost") {
      LOG("warn", "External QoS sampling request rejected");
      return ;
    }

    WebPreferences* preferences = new WebPreferences();
    m_dbSession->updateUserList();

    long now = time(NULL);
    std::vector<QosCollector*> mycollectors;
    for (auto view: m_dbSession->viewList()) {
      QosCollector* collector = new QosCollector(view.path.c_str());
      collector->initialize(preferences);
      mycollectors.push_back(collector);
    }

    LOG("notice", Q_TR("Collecting QoS data..."));
    for (auto collector: mycollectors) {
      collector->runMonitor();
      DbQosInfoT qosInfo = collector->qosInfo();
      qosInfo.timestamp = now;
      m_dbSession->addQosInfo(qosInfo);
    }

    // free up resources
    for (auto collector: mycollectors) {
      delete collector;
    }
  }

  virtual void destroy()
  {
    delete m_dbSession;
  }

private:
  DbSession* m_dbSession;
};


#endif // REALOPINSIGHTQAPP_HPP