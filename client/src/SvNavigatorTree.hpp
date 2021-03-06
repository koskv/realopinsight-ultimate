/*
 * SvNavigatorTree.hpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2014 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
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

#ifndef SNAVSVNAVIGATORTREE_H_
#define SNAVSVNAVIGATORTREE_H_

#include "Base.hpp"
#include "GuiUtils.hpp"
#include "utilsCore.hpp"

class SvNavigatorTree : public QTreeWidget
{
  Q_OBJECT

public:
  SvNavigatorTree(CoreDataT* _cdata, const bool& _enableDrag=false, QWidget* = 0 );

  void update(void);

  void clearTree(void) {m_items.clear();}
  void build(void);
  QTreeWidgetItem* rootItem(void) {return m_items[ngrt4n::ROOT_ID];}
  QTreeWidgetItem* addNode(const NodeT &, const bool & = false );
  QTreeWidgetItem* findNodeItem(const QString& _nodeId);
  void removeNodeItem(const QString& _nodeId) {m_items.remove(_nodeId);}
  void updateNodeItem(const NodeT& _node, const QString& _tip);
  void resetData(CoreDataT* cdata) {m_cdata = cdata;}

Q_SIGNALS:
  void treeNodeMoved( QString _nodeId );

protected:
  void dropEvent(QDropEvent *);
  void startDrag(Qt::DropActions) ;

private:
  QString m_selectedNode;
  CoreDataT* m_cdata;
  GuiTreeItemListT m_items;
  QTreeWidgetItem * createItem(const NodeT & _node);
};

#endif /* SNAVSVNAVIGATORTREE_H_ */
