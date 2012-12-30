/*
 * PieChart.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2012 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
# Last Update : 24-05-2012                                                 #
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


#include "PieChart.hpp"

PieChart::PieChart(const QRectF & _bounding_rect, QWidget * _parent)
: QWidget( _parent ), boundingRect( _bounding_rect ),
  legend(new StatsLegend(QPoint(_bounding_rect.width() + 25, 10), this))
{
	resize(legend->size().width() + 10,  boundingRect.topLeft().y() + boundingRect.height()) ;
	setStyleSheet("background:transparent") ;
}

PieChart::~PieChart()
{
	delete legend ;
	slices.clear() ;
}

QString PieChart::update(const CheckStatusCountT & _check_status_count, const qint32 _check_count)
{
	qint32 ok_count, warning_count, critical_count, unknown_count ;
	qreal ok_ratio, warning_ratio, critical_ratio, unknown_ratio ;


	critical_count = _check_status_count[MonitorBroker::CRITICAL] ;
	warning_count = _check_status_count[MonitorBroker::WARNING] ;
	unknown_count = _check_status_count[MonitorBroker::UNKNOWN] ;
	ok_count =  _check_status_count[MonitorBroker::OK] ;

	critical_ratio= ( 100.0 * critical_count ) / _check_count ;
	warning_ratio = ( 100.0 * warning_count ) / _check_count ;
	unknown_ratio = ( 100.0 * unknown_count ) / _check_count ;
	ok_ratio = ( 100.0 * ok_count ) / _check_count ;

	slices[MonitorBroker::CRITICAL] = new PieChartItem(
			boundingRect, 0, 3.6 * critical_ratio, StatsLegend::CRITICAL_COLOR, this) ;
	slices[MonitorBroker::WARNING] = new PieChartItem(
			boundingRect, 3.6 * critical_ratio, 3.6 * warning_ratio, StatsLegend::WARNING_COLOR, this) ;
	slices[MonitorBroker::UNKNOWN] = new PieChartItem(
			boundingRect, 3.6 * (critical_ratio + warning_ratio), 3.6 * unknown_ratio, StatsLegend::UNKNOWN_COLOR, this) ;
	slices[MonitorBroker::OK] = new PieChartItem(
			boundingRect, 3.6 * (unknown_ratio + warning_ratio + critical_ratio), 3.6 * ok_ratio, StatsLegend::OK_COLOR, this) ;

    QString info =  tr("Critical")%": "%QString::number(critical_count)%"/"
            %QString::number(_check_count)%" ("%QString::number(critical_ratio, 'f', 0) %
            "%)"%"\n"%tr("Warning")%": "%QString::number(warning_count)%
            "/"%QString::number(_check_count)%" ("%QString::number(warning_ratio, 'f', 0)%
            "%)"%"\n"%tr("Unknown")%": "%QString::number(unknown_count)%
            "/"%QString::number(_check_count)%" ("%QString::number(unknown_ratio, 'f', 0)%
            "%)"%"\n"%tr("Normal")%": "%QString::number(ok_count)%
            "/"%QString::number(_check_count)%" ("%QString::number(ok_ratio, 'f', 0)%
			"%)" ;

	return info ;
}