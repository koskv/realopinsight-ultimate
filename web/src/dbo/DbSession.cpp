/*
# DbSession.cpp
# ------------------------------------------------------------------------ #
# Copyright (c) 2010-2014 Rodrigue Chakode (rodrigue.chakode@ngrt4n.com)   #
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
#include "WebUtils.hpp"
#include "DbSession.hpp"
#include "WebPreferences.hpp"
#include <QFile>
#include <Wt/Auth/HashFunction>
#include <Wt/Auth/Identity>
#include <Wt/Auth/PasswordStrengthValidator>

DbSession::DbSession(void)
  : m_dbPath(ngrt4n::sqliteDbPath())
{
  m_dbUsers = new UserDatabase(*this);
  m_passAuthService = new Wt::Auth::PasswordService(m_basicAuthService);
  m_sqlite3Db = new Wt::Dbo::backend::Sqlite3(m_dbPath);
  m_sqlite3Db->setProperty("show-queries", "false");
  setConnection(*m_sqlite3Db);

  // do this before anything
  configureAuth();

  setupDb();
  updateUserList();
  updateViewList();
}

DbSession::~DbSession()
{
  delete m_dbUsers;
  delete m_sqlite3Db;
  delete m_passAuthService;
}

void DbSession::setupDb(void)
{
  mapClass<DbUserT>("user");
  mapClass<DbViewT>("view");
  mapClass<AuthInfo>("auth_info");
  mapClass<DbLoginSession>("login_session");
  mapClass<DbQosDataT>("qosdata");
  mapClass<AuthInfo::AuthIdentityType>("auth_identity");
  mapClass<AuthInfo::AuthTokenType>("auth_token");
  initDb();
}


int DbSession::addUser(const DbUserT& user)
{
  int retCode = -1;
  dbo::Transaction transaction(*this);
  try {
    UserCollectionT users = find<DbUserT>().where("name=?").bind(user.username);
    if (users.size() > 0) {
      m_lastError = "Failed: a user with the same username already exist.";
      LOG("error", m_lastError);
      retCode = 1;
    } else {
      Wt::Auth::User dbuser = m_dbUsers->registerNew();
      dbo::ptr<AuthInfo> info = m_dbUsers->find(dbuser);
      info.modify()->setEmail(user.email);
      m_passAuthService->updatePassword(dbuser, user.password);
      DbUserT* userTmpPtr(new DbUserT());
      *userTmpPtr = user;
      info.modify()->setUser(add(userTmpPtr));
      dbuser.addIdentity(Wt::Auth::Identity::LoginName, user.username);
      retCode = 0;
    }
  } catch (const dbo::Exception& ex) {
    m_lastError = "Failed to add the user. More details in log.";
    LOG("error", ex.what());
  }
  transaction.commit();
  updateUserList();
  return retCode;
}

int DbSession::updateUser(DbUserT user)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    dbo::ptr<AuthInfo> authInfo = find<AuthInfo>().where("user_name=?").bind(user.username);
    dbo::ptr<DbUserT> userPtr = authInfo.modify()->user();
    userPtr.modify()->username = user.username;
    userPtr.modify()->lastname = user.lastname;
    userPtr.modify()->firstname = user.firstname;
    userPtr.modify()->email = user.email;
    userPtr.modify()->role = user.role;
    authInfo.modify()->setEmail(user.email);
    retCode = 0;
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    m_lastError = "Failed to update the user. More details in log.";
    LOG("error", ex.what());
  }
  updateUserList();
  return retCode;
}

int DbSession::updatePassword(const std::string& uname, const std::string& currentPass, const std::string& newpass)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    Wt::Auth::User dbuser = m_dbUsers->findWithIdentity(Wt::Auth::Identity::LoginName, uname);
    switch (m_passAuthService->verifyPassword(dbuser, currentPass)) {
      case Wt::Auth::PasswordValid:
        m_passAuthService->updatePassword(dbuser, newpass);
        retCode = 0;
        break;
      case Wt::Auth::PasswordInvalid:
        m_lastError = "Your current password doesn't match";
        break;
      case Wt::Auth::LoginThrottling:
        m_lastError = "The account has been blocked. Retry later or contact your administrator";
        break;
      default:m_lastError = "Unknown error concerning your current password";
        break;
    }
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    retCode = -1;
    LOG("error", ex.what());
  }
  updateUserList();
  return retCode;
}

int DbSession::deleteUser(std::string uname)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    dbo::ptr<DbUserT> usr = find<DbUserT>().where("name=?").bind(uname);
    usr.remove();
    retCode = 0;
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    retCode = 1;
    LOG("error", ex.what());
  }
  updateUserList();
  return retCode;
}


int DbSession::deleteAuthSystemUsers(int authSystem)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    dbo::ptr<DbUserT> usr = find<DbUserT>().where("authsystem=?").bind(authSystem);
    usr.remove();
    retCode = 0;
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    retCode = 1;
    LOG("error", ex.what());
  }
  updateUserList();
  return retCode;
}


bool DbSession::findUser(const std::string& username, DbUserT& user)
{
  DbUsersT::const_iterator it = std::find_if(m_userList.cbegin(),
                                             m_userList.cend(),
                                             [&username](const DbUserT& u){return u.username == username;});
  bool found = false;
  if (it != m_userList.end()) {
    found = true;
    user = *it;
  }
  return found;
}


std::string DbSession::hashPassword(const std::string& pass)
{
  Wt::Auth::BCryptHashFunction h;
  return h.compute(pass, "$ngrt4n$salt");
}

Wt::Auth::AuthService& DbSession::auth()
{
  return m_basicAuthService;
}

Wt::Auth::PasswordService* DbSession::passwordAuthentificator(void)
{
  return m_passAuthService;
}

Wt::Auth::Login& DbSession::loginObject(void)
{
  rereadAll();
  return m_loginObj;
}

void DbSession::configureAuth(void)
{
  m_basicAuthService.setAuthTokensEnabled(true, "realopinsightcookie");
  m_basicAuthService.setEmailVerificationEnabled(true);
  Wt::Auth::PasswordVerifier* verifier = new Wt::Auth::PasswordVerifier();
  verifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));
  m_passAuthService->setVerifier(verifier);
  m_passAuthService->setStrengthValidator(new Wt::Auth::PasswordStrengthValidator());
  m_passAuthService->setAttemptThrottlingEnabled(true);
}

void DbSession::setLoggedUser(void)
{
  try {
    std::string dbUserId = loginObject().user().id();
    dbo::Transaction transaction(*this);
    dbo::ptr<AuthInfo> info = find<AuthInfo>().where("id=?").bind(dbUserId);
    m_loggedUser = *(info.modify()->user());
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    LOG("error", ex.what());
  }
}

void DbSession::updateUserList(void)
{
  try {
    m_userList.clear();
    dbo::Transaction transaction(*this);
    UserCollectionT users = find<DbUserT>();
    for (auto &user : users) {
      m_userList.push_back(*user);
    }
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    LOG("error", ex.what());
  }
}

void DbSession::updateViewList(void)
{
  try {
    m_viewList.clear();
    dbo::Transaction transaction(*this);
    ViewCollectionT views = find<DbViewT>();
    for (auto& view :views) {
      m_viewList.push_back(*view);
    }
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    LOG("error", ex.what());
  }
}

void DbSession::updateViewList(const std::string& uname)
{
  try {
    m_viewList.clear();
    dbo::Transaction transaction(*this);
    dbo::ptr<DbUserT> userDboPtr = find<DbUserT>().where("name=?").bind(uname);
    for (auto& view : userDboPtr.modify()->views) {
      m_viewList.push_back(*view);
    }
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    LOG("error", ex.what());
  }
}

bool DbSession::findView(const std::string& vname, DbViewT& view)
{
  DbViewsT::const_iterator it = std::find_if(m_viewList.cbegin(),
                                             m_viewList.cend(),
                                             [&vname](const DbViewT& v){return v.name == vname;});
  bool found = false;
  if (it != m_viewList.end()) {
    found = true;
    view = *it;
  }
  return found;
}

void DbSession::initDb(void)
{
  try {
    WebPreferencesBase pref;
    if (pref.getDbState() != 1) {
      createTables();
      DbUserT adm;
      adm.username = "admin";
      adm.password = "password";
      adm.firstname = "Default";
      adm.lastname = "Administrator";
      adm.role = DbUserT::AdmRole;
      adm.registrationDate = Wt::WDateTime::currentDateTime().toString().toUTF8();
      addUser(adm);
      pref.setDbState(1);
      LOG("info", Q_TR("Database created: ")+m_dbPath);
    }
  } catch (dbo::Exception& ex) {
    LOG("error", "Failed initializing the database");
    LOG("error", ex.what());
  }
}

int DbSession::addView(const DbViewT& view)
{
  int retCode = -1;
  dbo::Transaction transaction(*this);
  try {
    ViewCollectionT views = find<DbViewT>().where("name=?").bind(view.name);
    if (views.size() > 0) {
      m_lastError = "Failed: a view with the same name already exist.";
      LOG("error", m_lastError);
      retCode = 1;
    } else {
      DbViewT* viewTmpPtr(new DbViewT());
      *viewTmpPtr =  view;
      add(viewTmpPtr);
      retCode = 0;
    }
  } catch (const dbo::Exception& ex) {
    m_lastError = "Failed to add the view. More details in log.";
    LOG("error", ex.what());
  }
  transaction.commit();
  updateViewList();
  return retCode;
}


int DbSession::deleteView(std::string vname)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    execute("DELETE FROM user_view WHERE view_name=?;").bind(vname);
    execute("DELETE FROM view WHERE name=?;").bind(vname);
    retCode = 0;
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    retCode = 1;
    m_lastError = ex.what();
    LOG("error", ex.what());
  }
  updateViewList();
  return retCode;
}


void DbSession::updateUserViewList(void)
{
  m_userViewList.clear();
  dbo::Transaction transaction(*this);
  UserCollectionT users = find<DbUserT>();
  for (auto& user : users) {
    for (const auto& view: user->views) {
      m_userViewList.insert(user->username+":"+view->name);
    }
  }
  transaction.commit();
}



int DbSession::assignView(const std::string& uname, const std::string& vname)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    dbo::ptr<DbUserT> dboUserPtr = find<DbUserT>().where("name=?").bind(uname);
    dbo::ptr<DbViewT> dboViewPtr = find<DbViewT>().where("name=?").bind(vname);
    dboUserPtr.modify()->views.insert(dboViewPtr);
    retCode = 0;
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    LOG("error", ex.what());
  }
  return retCode;
}


int DbSession::revokeView(const std::string& uname, const std::string& vname)
{
  int retCode = -1;
  try {
    dbo::Transaction transaction(*this);
    dbo::ptr<DbUserT> dboUserPtr = find<DbUserT>().where("name=?").bind(uname);
    dbo::ptr<DbViewT> dboViewPtr = find<DbViewT>().where("name=?").bind(vname);
    dboUserPtr.modify()->views.erase(dboViewPtr);
    retCode = 0;
    transaction.commit();
  } catch (const dbo::Exception& ex) {
    LOG("error", ex.what());
  }
  return retCode;
}


int DbSession::addSession(const DbLoginSession& session)
{
  int retCode = -1;
  dbo::Transaction transaction(*this);
  try {
    if (checkUserCookie(session) != DbLoginSession::ActiveCookie) {
      DbLoginSession* sessionPtr(new DbLoginSession());
      *sessionPtr = session;
      add(sessionPtr);
      retCode = 0;
    } else {
      m_lastError = "Already active session";
      LOG("error", m_lastError);
      retCode = 1;
    }
  } catch (const dbo::Exception& ex) {
    m_lastError = "Failed to add the session. More details in log.";
    LOG("error", ex.what());
  }
  transaction.commit();
  return retCode;
}



int DbSession::checkUserCookie(const DbLoginSession& session)
{
  int retCode = -1;

  dbo::Transaction transaction(*this);
  try {
    LoginSessionCollectionT sessions = find<DbLoginSession>()
        .where("username=? AND session_id=? AND status = ?")
        .bind(session.username)
        .bind(session.sessionId)
        .bind(DbLoginSession::ExpiredCookie);
    retCode = sessions.size()? DbLoginSession::ActiveCookie : DbLoginSession::InvalidSession;
  } catch (const dbo::Exception& ex) {
    m_lastError = "Error checking the session. More details in log.";
    LOG("error", ex.what());
  }
  transaction.commit();

  return retCode;
}


int DbSession::addQosData(const DbQosDataT& qosData)
{
  int retCode = -1;
  dbo::Transaction transaction(*this);
  try {
    DbQosDataT* qosDboPtr = new DbQosDataT();
    *qosDboPtr = qosData;
    qosDboPtr->view = find<DbViewT>().where("name=?").bind(qosData.viewname);;
    add(qosDboPtr);
    retCode = 0;
    m_lastError = Q_TR("QoS entry added: ") + qosData.toString();
  } catch (const dbo::Exception& ex) {
    m_lastError = "Failed to add QoS entry. More details in log.";
    LOG("error", ex.what());
  }
  transaction.commit();
  return retCode;
}



int DbSession::fetchQosData(ViewQosDataMapT& qosDataMap,
                             const std::string& viewName,
                             long fromDate,
                             long toDate)
{
  int retCode = -1;
  dbo::Transaction transaction(*this);
  try {
    QosInfoCollectionT entries;
    if (viewName.empty()) {
      entries = find<DbQosDataT>()
          .where("timestamp >= ? AND timestamp <= ?")
          .orderBy("timestamp")
          .bind(fromDate).bind(toDate);
    } else {
      entries = find<DbQosDataT>()
          .where("view_name = ? AND timestamp >= ? AND timestamp <= ?")
          .orderBy("timestamp")
          .bind(viewName).bind(fromDate).bind(toDate);
    }

    qosDataMap.clear();
    for (auto &entry : entries) {
      entry.modify()->viewname = entry->view->name;
      qosDataMap[entry->viewname].push_back(*entry);
    }

    retCode = 0;
  } catch (const dbo::Exception& ex) {
    m_lastError = "Failed to fetch QoS entries. More details in log.";
    qDebug()<< ex.what();
    LOG("error", ex.what());
  }
  transaction.commit();
  return retCode;
}