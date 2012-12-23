/*
 * SvNavigator.hpp
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

#ifndef SVNAVIGATOR_HPP
#define SVNAVIGATOR_HPP

#include <QString>
#include "Base.hpp"
#include "Stats.hpp"
#include "Parser.hpp"
#include "WebKit.hpp"
#include "MsgPanel.hpp"
#include "GraphView.hpp"
#include "SvNavigatorTree.hpp"
#include "Preferences.hpp"
#include "Socket.hpp"
#include "ZbxHelper.hpp"
#include "ZnsHelper.hpp"
#include <QScriptValueIterator>

class SvNavigator : public QMainWindow
{
    Q_OBJECT

public:
    SvNavigator(const qint32 & _userRole = Auth::OP_USER_ROLE,
                const QString & _config = "",
                QWidget* = 0);
    virtual ~SvNavigator();
    void load(const QString & _file = "");

    void resize(void);

    static ComboBoxItemsT propRules();
    static ComboBoxItemsT calcRules();

    static QString getNodeToolTip(const NodeT & _node);

public slots:
    void startMonitor();
    int runNagiosMonitor(void);
    void resetStatData(void);
    void updateBpNode(QString);
    void expandNode(const QString & _nodeId,
                    const bool & _expand,
                    const qint32 & _level);
    void centerGraphOnNode(const QString & _node_id = "");
    void filterNodeRelatedMsg(void);
    void filterNodeRelatedMsg(const QString &);
    void acknowledge(void);
    void tabChanged(int);
    void hideChart(void);
    void centerGraphOnNode(QTreeWidgetItem *);
    void handleChangePasswordAction(void);
    void handleChangeMonitoringSettingsAction(void);
    void handleShowOnlineResources(void);
    void handleShowAbout(void);
    void processZabbixReply(QNetworkReply* reply);
    void processPostError(QNetworkReply::NetworkError code);
    void processZenossReply(QNetworkReply* reply);

signals:
    void hasToBeUpdate(QString);
    void sortEventConsole(void);

protected:
    void closeEvent(QCloseEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void timerEvent(QTimerEvent *);


private:
    Struct* coreData;
    QString configFile;
    QString openedFile;
    QString monitorBaseUrl;
    QString selectedNodeId;
    QString statsPanelTooltip;
    qint32 userRole;
    qint32 updateInterval;
    qint32 timerId;
    Settings* settings;
    Stats* statsPanel;
    MsgPanel* filteredMsgPanel;
    QSplitter* mainSplitter;
    QSplitter* rightSplitter;
    QTabWidget * topRightPanel;
    QTabWidget* bottomRightPanel;
    WebKit* browser;
    GraphView* map;
    SvNavigatorTree* tree;
    Preferences* monPrefWindow;
    Preferences* changePasswdWindow;
    MsgPanel* msgPanel;
    QMenu* nodeContextMenu;
    QSize msgPanelSize;
    MenuListT menuList;
    SubMenuListT subMenuList;
    SubMenuListT contextMenuList;
    QString serverAddr;
    QString serverPort;
    QString serverUrl;
    QString serverAuthChain;
    Socket* msocket;
    ZbxHelper* zxHelper;
    QString zbxAuthToken;
    qint32 hLeft;
    qint32 iter;
    bool updateSucceed;
    ZnsHelper* znsHelper;
    bool isLoggedOnZenoss;


    void addEvents(void);
    void loadMenus(void);
    void unloadMenus(void);
    void updateMonitoringSettings();
    void updateNavTreeItemStatus(const NodeListT::iterator & _node, const QString & _tip);
    void updateAlarmMsg(NodeListT::iterator & _node);
    void updateCNode(NodeListT::iterator & _node);
    void updateStats(void);
    void updateStatusBar(const QString & msg);
    void openZabbixSession(void);
    void closeZabbixSession(void);
    void requestZabbixTriggers(void);
    void retrieveZabbixHostTriggers(const QString & host);
    void openZenossSession(void);
    void requestZenossEvents(void);
};

#endif /* SVNAVIGATOR_HPP */
