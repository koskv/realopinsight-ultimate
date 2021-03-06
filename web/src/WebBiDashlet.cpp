/*
 * WebBiDashlet.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2015 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
# Creation: 17-07-2015                                                     #
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

#include "WebBiDashlet.hpp"
#include "WebBiRawChart.hpp"
#include <ctime>


WebBiDashlet::WebBiDashlet()
{
  setLayout(m_layout = new Wt::WGridLayout());
  addEvent();

}

WebBiDashlet::~WebBiDashlet()
{
  // just remove m_filter from the layout, since not a pointer
  m_layout->removeWidget(&m_filterHeader);
  m_layout->clear(); // delete all contents
  clear(); // delete m_layout
}


void WebBiDashlet::addEvent(void)
{
  QObject::connect(&m_filterHeader, SIGNAL(reportPeriodChanged(long, long)),
                   this, SLOT(handleReportPeriodChanged(long, long)));
}

void WebBiDashlet::initialize(const DbViewsT& viewList)
{
  int row = 0;
  m_layout->addWidget(&m_filterHeader, row, 0, 1, 2, Wt::AlignRight);
  for (const auto& view : viewList) {
    //FIXME: dont use pointer for chart widgets
    // or think of deleting explicitely chart objects
    m_slaChartTitleMap.insert(view.name, createTitleWidget(view.name));
    m_csvExportLinkMap.insert(view.name, new WebCsvExportIcon());
    m_itProblemChartMap.insert(view.name, new WebBiRawChart(view.name));
    m_slaPieChartMap.insert(view.name, new WebPieChart(ChartBase::SLAData));

    ++row;
    m_layout->addWidget(m_slaChartTitleMap[view.name], row, 0);
    m_layout->addWidget(m_csvExportLinkMap[view.name], row, 1, Wt::AlignRight);

    ++row;
    m_layout->addWidget(m_itProblemChartMap[view.name], row, 0);
    m_layout->addWidget(m_slaPieChartMap[view.name], row, 1);
  }
}


Wt::WText* WebBiDashlet::createTitleWidget(const std::string& viewName)
{
  Wt::WString title = Wt::WString("<h5>{1}</h5>").arg(viewName);
  return new Wt::WText(title, Wt::XHTMLText);
}


void WebBiDashlet::updateViewCharts(const std::string& viewName, const QosDataListMapT& qosDataMap)
{
  QosDataListMapT::ConstIterator iterQosDataSet = qosDataMap.find(viewName);
  if (iterQosDataSet ==  qosDataMap.end())
    return; // stop process

  // Here we have: iterQosDataSet !=  qosDataMap.end()
  // So update qos chart when applicable
  QMap<std::string, WebPieChart*>::iterator iterSlaPiechart = m_slaPieChartMap.find(viewName);
  if (iterSlaPiechart != m_slaPieChartMap.end()) {
    WebBiSlaDataAggregator slaData(*iterQosDataSet);
    (*iterSlaPiechart)->setSeverityData(slaData.normalDuration(),
                                        slaData.minorDuration(),
                                        slaData.majorDuration(),
                                        slaData.criticalDuration(),
                                        slaData.totalDuration());
    (*iterSlaPiechart)->repaint();
  }

  // update IT problem chart when applicable
  QMap<std::string, WebBiRawChart*>::iterator iterProblemTrendsChart = m_itProblemChartMap.find(viewName);
  if (iterProblemTrendsChart != m_itProblemChartMap.end()) {
    (*iterProblemTrendsChart)->updateData(*iterQosDataSet);
  }

  // update QoS data for export
  QMap<std::string, WebCsvExportIcon*>::iterator iterCsvExportItem = m_csvExportLinkMap.find(viewName);
  if (iterCsvExportItem != m_csvExportLinkMap.end()) {
    (*iterCsvExportItem)->updateData(viewName, *iterQosDataSet);
  }
}

