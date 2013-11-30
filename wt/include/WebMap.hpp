/*
 * WebServiceMap.hpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2012 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
# Last Update : 19-09-2013                                                 #
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

#ifndef WEBSERVICEMAP_HPP
#define WEBSERVICEMAP_HPP

#include "Base.hpp"
#include <Wt/WPaintedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WPainter>
#include <Wt/WObject>
#include <Wt/WLength>
#include <Wt/WSignal>
#include <Wt/WScrollArea>

class WebPieMap : public Wt::WPaintedWidget
{
public:
  WebPieMap(CoreDataT* _cdata);
  virtual ~WebPieMap();
  void drawMap(void);

  Wt::WScrollArea* get(void) const {return m_scrollArea;}
  void updateNode(const NodeT& _node, const QString& _toolTip);
  void scaleMap(double factor);

protected:
  void paintEvent(Wt::WPaintDevice *paintDevice);
  void layoutSizeChanged(int width, int height );

private:
  CoreDataT* m_cdata;
  double m_scaleX;
  double m_scaleY;
  Wt::WPainter* m_painter;
  IconMapT m_icons;
  Wt::WScrollArea* m_scrollArea;
  bool m_firstUpdate;

  void drawNode(const NodeT& _node);
  void drawEdge(const QString& _parentId, const QString& _childId);
  void createLink(const NodeT& _node);
  void setPreferredMethod(void);
  void setJavaScriptMember(void);
};

#endif /* WEBSERVICEMAP_HPP */
