/*
 * Parser.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2012 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
# Last Update : 13-05-2012												   #
#																		   #
# This file is part of NGRT4N (http://ngrt4n.com).	  					   #
#																		   #
# NGRT4N is free software: you can redistribute it and/or modify		   #
# it under the terms of the GNU General Public License as published by     #
# the Free Software Foundation, either version 3 of the License, or        #
# (at your option) any later version.									   #
#																		   #
# NGRT4N is distributed in the hope that it will be useful,                #
# but WITHOUT ANY WARRANTY; without even the implied warranty of		   #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	           #
# GNU General Public License for more details.							   #
#																		   #
# You should have received a copy of the GNU General Public License		   #
# along with NGRT4N.  If not, see <http://www.gnu.org/licenses/>.		   #
#--------------------------------------------------------------------------#
 */

#include "core/ns.hpp"
#include "Parser.hpp"
#include "GraphView.hpp"

const QString Parser::CHILD_NODES_SEP = "," ;

const QString Parser::dotFileHeader = "strict graph   \n{\n//Nodes declaration\n\tnode[shape=plaintext]\n\n";
const QString Parser::dotFileFooter = "}";


Parser::Parser(){}

Parser::~Parser()
{
	QFile dotFile;

	if(dotFile.exists(graphFilename))
		dotFile.remove(graphFilename);

	if(dotFile.exists(graphFilename + ".plain"))
		dotFile.remove(graphFilename + ".plain");
}

bool Parser::parseSvConfig(const QString & _sv_config_file, Struct & _snav_struct)
{
	QString graph_content;
	QDomDocument sv_config_doc;
	QDomElement rootElt;
	QDomNodeList xml_service_node_list;
	QFile file(_sv_config_file);
	NodeT node;
	qint32 s, xml_service_node_count ;

	if ( ! file.open(QIODevice::ReadOnly) )
	{
		qDebug() << "Unable to open the file " << _sv_config_file ;
		return false;
	}

	if (! sv_config_doc.setContent(&file) )
	{
		file.close();
		qDebug() << "Error when parsing the file " << _sv_config_file ;
		return false;
	}

	rootElt = sv_config_doc.documentElement();
	xml_service_node_list = rootElt.elementsByTagName("Service");

	node.id = _snav_struct.root_id = rootElt.attribute("id");
	node.name = rootElt.firstChildElement("Name").text();
	node.child_nodes = rootElt.firstChildElement("SubServices").text();
	node.status = MonitorBroker::UNSET_STATUS ;  // TODO Acknowledge status,
	node.icon = GraphView::DEFAULT_ICON ;
	node.type = SERVICE_NODE ;
	node.parent = "NULL" ;  // By convention

	_snav_struct.node_list[node.id] = node;

	xml_service_node_count = xml_service_node_list.length();
	for (s = 0; s < xml_service_node_count; s++)
	{
		QDomElement service = xml_service_node_list.item(s).toElement();

		node.id = service.attribute("id").trimmed() ;
		node.type = service.attribute("type").toInt() ;
		node.status_calc_rule = service.attribute("statusCalcRule").toInt() ;
		node.icon = service.firstChildElement("Icon").text().trimmed() ;
		node.name = service.firstChildElement("Name").text().trimmed() ;
		node.description = service.firstChildElement("Description").text().trimmed() ;
		node.propagation_rule = service.firstChildElement("PropagationRule").text().trimmed() ;
		node.alarm_msg = service.firstChildElement("AlarmMsg").text().trimmed() ;
		node.notification_msg = service.firstChildElement("NotificationMsg").text().trimmed() ;
		node.child_nodes = service.firstChildElement("SubServices").text().trimmed() ;

		if( node.type == ALARM_NODE )
		{
			_snav_struct.check_list << node.id;
		}

		_snav_struct.node_list[node.id] = node ;
	}
	file.close();

	updateNodeHierachy( _snav_struct.node_list, graph_content ) ;
	buildNodeTree(_snav_struct.node_list, _snav_struct.tree_item_list) ;

	graph_content = dotFileHeader + graph_content ;
	graph_content += dotFileFooter;

	saveCoordinatesDotFile( graph_content );

	return true;
}

void Parser::updateNodeHierachy( NodeListT & _nodes_list, QString & _graph_content )
{
	QString child_node_id ;
	NodeListT::iterator node_it, child_node_it ;
	QStringList node_ids_list ;
	QStringList::iterator node_id_it ;
	QString g_node_label ;

	_graph_content = "\n//Edges delcaration\n" ;

	for( node_it = _nodes_list.begin(); node_it != _nodes_list.end(); node_it ++)
	{
		g_node_label = node_it->name ;
		_graph_content = "\t" + node_it->id + "[label=\"" + g_node_label.replace(' ', '#') + "\"];\n" + _graph_content;

		if( node_it->child_nodes != "" )
		{
			node_ids_list = node_it->child_nodes.split( CHILD_NODES_SEP ) ;

			for(node_id_it = node_ids_list.begin(); node_id_it != node_ids_list.end(); node_id_it ++)
			{
				child_node_it = _nodes_list.find( (*node_id_it).trimmed() );
				if( child_node_it != _nodes_list.end() )
				{
					child_node_it->parent = node_it->id ;
					_graph_content += "\t" + node_it->id + "--" + child_node_it->id + "\n";
				}
			}
		}
	}
}

void Parser::buildNodeTree( NodeListT & _nodes_list, TreeNodeItemListT & _tree)
{
	NodeListT::iterator node_it ;

	for( node_it = _nodes_list.begin(); node_it != _nodes_list.end(); node_it ++)
	{
		if(node_it->parent != "" && node_it->id != "")
		{
			SvNavigatorTree::addNode(_tree, (*node_it));
		}
	}
}

void Parser::saveCoordinatesDotFile(const QString& _graph_content)
{
	QFile file;

	graphFilename = "/tmp/" + QString(ngrt4n::APP_NAME.c_str()) + "-bv-" + QTime().currentTime().toString("hhmmsszzz") + ".dot";
	file.setFileName(graphFilename);
	if(! file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Unable to write the file " << graphFilename ;
		exit (1);
	}

	QTextStream file_stream(&file);
	file_stream << _graph_content;
	file.close();
}
