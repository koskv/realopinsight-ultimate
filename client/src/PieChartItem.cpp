/*
 * PieChartItem.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2014 Rodrigue Chakode (rodrigue.chakode@gmail.com)    #
# Last Update : 23-03-2014                                                 #
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


#include "PieChartItem.hpp"

PieChartItem::PieChartItem(const QRectF & _bounding_rect,
                           const qreal & _start_angle,
                           const qreal & _proportion,
                           const QColor & _color,
                           QWidget * _parent)
  : QWidget(_parent),
    pieChartBoundingRect(_bounding_rect),
    startAngle(_start_angle),
    proportion(_proportion),
    fillColor1(_color),
    fillColor2(_color)
{
  path.moveTo( pieChartBoundingRect.center() );
  path.arcTo(pieChartBoundingRect, startAngle, proportion);
  path.closeSubpath();
}

QSize PieChartItem::minimumSizeHint() const
{
  return QSize(350, 200) ;
}

QSize PieChartItem::sizeHint() const
{
  return QSize(500, 300) ;
}

void PieChartItem::setFillRule(Qt::FillRule rule)
{
  path.setFillRule(rule) ;
  update() ;
}

void PieChartItem::setFillGradient(const QColor &color1, const QColor &color2)
{
  fillColor1 = color1;
  fillColor2 = color2;
  update() ;
}

void PieChartItem::setPath(void)
{
  path.moveTo( pieChartBoundingRect.center() );
  path.arcTo(pieChartBoundingRect, startAngle, proportion);
  path.closeSubpath();
}

void PieChartItem::paintEvent(QPaintEvent *)
{
  QPainter painter(this) ;
  painter.setRenderHint(QPainter::Antialiasing) ;
  QLinearGradient gradient(0, 0, 0, 100) ;
  gradient.setColorAt(0.0, fillColor1) ;
  gradient.setColorAt(1.0, fillColor2) ;
  painter.setPen(QColor(Qt::transparent)) ;
  painter.setBrush(gradient) ;
  painter.drawPath(path) ;
}
