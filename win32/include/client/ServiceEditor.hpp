/*
 * SNAVServiceEditor.hpp
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


#ifndef SNAVSERVICEEDITOR_H_
#define SNAVSERVICEEDITOR_H_

#include "Base.hpp"
#include "Parser.hpp"


const qint32 MAX_NODE_NAME = 24 ;
const QString NAME_FIELD = "name";
const QString TYPE_FIELD = "type";
const QString STATUS_CALC_RULE_FIELD = "StatusCalcRules";
const QString STATUS_PROP_RULE_FIELD = "StatusPropRules";
const QString ICON_FIELD = "icon";
const QString DESCRIPTION_FIELD = "description";
const QString ALARM_MSG_FIELD = "alarmMsg" ;
const QString NOTIFICATION_MSG_FIELD = "notificationMsg";
const QString CHECK_LIST_FIELD = "ChecksField";
const QString CHECK_FIELD = "CheckField";
const QString HIGH_CRITICITY_CALC_RULE_FIELD = "Higher Criticity";
const QString WEIGHTED_CALC_RULE_FIELD = "Equal-weighted Criticity";

class ServiceEditor : public QWidget
{
	Q_OBJECT

public:

	typedef QMap<QString, QWidget*> ItemsList ;

	ServiceEditor( QWidget* = 0);
	virtual ~ServiceEditor();

	void layoutEditorComponents(void) ;
	void setContent( const NodeListT &, const QString & ) ;
	void setContent( NodeListT::const_iterator ) ;
	bool updateNode( NodeListT &, const QString& ) ;
	bool updateNode(NodeListT::iterator & _node_it) ;
	void setCheckListField( const MonitorBroker::NagiosChecksT& );
	void setEnableFields( const bool& );
	void loadStatusFile(const QString &) ;
	void setLowLevelAlarmComponentEnabled(bool enable){editorItemsList[CHECK_LIST_FIELD]->setEnabled(enable);}

	ItemsList* itemList( void ) {return &editorItemsList;}
	QLineEdit* nameField(void){return dynamic_cast<QLineEdit*>(editorItemsList[NAME_FIELD]);}
	QComboBox* typeField(void) const {return dynamic_cast<QComboBox*>(editorItemsList[TYPE_FIELD]);}
	QComboBox* statusCalcRuleField(void) const {return dynamic_cast<QComboBox*>(editorItemsList[STATUS_CALC_RULE_FIELD]);}
	QComboBox* statusPropRuleField(void) const {return dynamic_cast<QComboBox*>(editorItemsList[STATUS_PROP_RULE_FIELD]);}
	inline QComboBox* iconField(void) const {return dynamic_cast<QComboBox*>(editorItemsList[ICON_FIELD]);}
	inline QTextEdit* descriptionField(void) const {return dynamic_cast<QTextEdit*>(editorItemsList[DESCRIPTION_FIELD]);}
	inline QTextEdit* alarmMsgField(void){return dynamic_cast<QTextEdit*>(editorItemsList[ALARM_MSG_FIELD]) ;}
	inline QTextEdit* notificationMsgField(void){return dynamic_cast<QTextEdit*>(editorItemsList[NOTIFICATION_MSG_FIELD]);}
	inline QComboBox* checkField(void){return dynamic_cast<QComboBox*>(editorItemsList[CHECK_FIELD]) ;}
	inline QListWidget* checkListField(void){return dynamic_cast<QListWidget*>(editorItemsList[CHECK_LIST_FIELD]);}

public slots:
	void handleSaveClick(void) ;
	void handleCloseClick(void ) ;
	void handleReturnPressed(void) ;
	void handleNodeTypeChanged(const QString &) ;
	void handleNodeTypeActivated( const QString & _text) ;

	signals:
	void saveClicked(void) ;
	void closeClicked(void) ;
	void returnPressed(void) ;
	void nodeTypeActivated(qint32) ;

private:
	Settings* settings ;
	qint32 editorLayoutRowCount;
	qint32 editorLayoutColumnCount;
	qint16 currentLine;
	ItemsList editorItemsList;
	QGridLayout* layout;
	QDialogButtonBox* buttonBox;

	void addEvent(void) ;
	void loadLabelFields(void);
	void loadDescriptionFields(void);
	void loadTypeFields(void);
	void loadStatusHandlingFields(void);
	void loadAlarmMsgFields(void);
	void loadNotificationMsgFields(void);
	void loadIconFields(void);
	void loadCheckField(void);
	void loadButtonBox(void);
};

#endif /* SNAVSERVICEEDITOR_H_ */