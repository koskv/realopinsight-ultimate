/*
 * LsHelper.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2014 Rodrigue Chakode (rodrigue.chakode@gmail.com)    #
# Last Update: 23-03-2014                                                  #
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

#include "LsHelper.hpp"
#include "utilsCore.hpp"
#include "utilsCore.hpp"
#include "RawSocket.hpp"
#include "JsonHelper.hpp"
#include <iostream>
#include <QDir>
#include <QScriptValueIterator>

LsHelper::LsHelper(const QString& host, int port)
  : m_socketHandler(new RawSocket(host, port))
{
}

LsHelper::~LsHelper()
{
  delete m_socketHandler;
}

int LsHelper::setupSocket(void)
{
  if (m_socketHandler->setupSocket()) {
    m_lastError = QString("%1: %2").arg(Q_FUNC_INFO, m_socketHandler->lastError());
    return -1;
  }
  return 0;
}

QByteArray LsHelper::prepareRequestData(const QString& hostgroupFilter, ReqTypeT requestType)
{
  QString data = "";
  switch(requestType) {
  case LsHelper::Host:
    data = "GET hosts\n"
           "Columns: name state last_state_change check_command plugin_output groups\n"
           "OutputFormat: json\n";
    break;
  case LsHelper::Service:
    data = "GET services\n"
           "Columns: host_name service_description state last_state_change check_command plugin_output host_groups\n"
           "OutputFormat: json\n";
    break;
  default:
    break;
  }

  if (! hostgroupFilter.isEmpty()) {
    QString filterPattern;
    switch(requestType) {
    case LsHelper::Host:
      filterPattern =
          "Filter: name = %1\n"
          "Filter: host_groups ~ %1\n"
          "Or: 2\n";
      break;
    case LsHelper::Service:
      filterPattern =
          "Filter: host_name = %1\n"
          "Filter: host_groups ~ %1\n"
          "Or: 2\n";
      break;
    default:
      break;
    }
    data.append(filterPattern.arg(hostgroupFilter));
  }
  return ngrt4n::toByteArray(data.append("\n"));
}

int LsHelper::loadChecks(const QString& hostgroupFilter, ChecksT& checks)
{
  checks.clear();

  if (makeRequest(prepareRequestData(hostgroupFilter, LsHelper::Host), checks) != 0)
    return -1;

  return makeRequest(prepareRequestData(hostgroupFilter, LsHelper::Service), checks);
}


int LsHelper::makeRequest(const QByteArray& data, ChecksT& checks)
{
  if (m_socketHandler->makeRequest(data) != 0) {
    m_lastError = QString("%1: %2").arg(Q_FUNC_INFO, m_socketHandler->lastError());
    return -1;
  }
  parseResult(checks);
  return 0;
}


void LsHelper::parseResult(ChecksT& checks)
{
  JsonHelper json(m_socketHandler->lastResult());

  QScriptValueIterator entryIter(json.data());
  while (entryIter.hasNext()) {
    entryIter.next();
    if (entryIter.flags() & QScriptValue::SkipInEnumeration) continue;
    QScriptValueIterator fieldIter(entryIter.value());

    QStringList fields;
    fields.clear();
    while (fieldIter.hasNext()) {
      fieldIter.next();
      if (fieldIter.flags() & QScriptValue::SkipInEnumeration) continue;
      fields.push_back(fieldIter.value().toString());
    }

    CheckT check;
    switch( fields.size() ) {
    case 6: // host
      check.id = check.host = fields[0].toStdString();
      check.status = fields[1].toInt();
      check.last_state_change = fields[2].toStdString();
      check.check_command = fields[3].toStdString();
      check.alarm_msg = fields[4].toStdString();
      check.host_groups = fields[5].toStdString();
      break;

    case 7: // service
      check.host = fields[0].toStdString();
      check.id = ID_PATTERN.arg(check.host.c_str(), fields[1]).toLower().toStdString();
      check.status = fields[2].toInt();
      check.last_state_change = fields[3].toStdString();
      check.check_command = fields[4].toStdString();
      check.alarm_msg = fields[5].toStdString();
      check.host_groups = fields[6].toStdString();
      break;

    default:
      qDebug()<< "unexpected entry: "<< entryIter.value().toString();
      continue;
      break;
    }

    checks.insert(check.id, check);
  }
}
