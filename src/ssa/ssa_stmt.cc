#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

SSA_Stmt::SSA_Stmt(SSA_StmtType type, string op, SSA_Opd *lopd, SSA_Opd *ropd1, SSA_Opd *ropd2) {
  CHECK_INVARIANT(type == SSA_AssignStmt, "SSA_AssignStmt expected");

  this->type = type;

  this->op = op;
  this->lopd = lopd;
  this->ropd1 = ropd1;
  this->ropd2 = ropd2;
  this->ropds = NULL;

  if (ropd2 == NULL) {
    CHECK_INVARIANT(op == "=", "Assignment operator expected");
    this->stmt = lopd->str() + " = " + ropd1->str();
  } else {
    this->stmt = lopd->str() + " = " + ropd1->str() + " " + op + " " + ropd2->str();
  }
}

SSA_Stmt::SSA_Stmt(SSA_StmtType type, SSA_Opd *lopd, std::list<SSA_Opd *> *ropds) {
  CHECK_INVARIANT(type == SSA_PhiStmt, "SSA_PhiStmt expected");
  CHECK_INVARIANT(lopd->get_type() == SSA_PhiOpd, "SSA_PhiOpd expected");

  this->type = type;

  this->op = "phi";
  this->lopd = lopd;
  this->ropd1 = NULL;
  this->ropd2 = NULL;
  this->ropds = ropds;

  this->stmt = lopd->str() + " = PHI(";
  for (list<SSA_Opd *>::iterator it = ropds->begin(); it != ropds->end(); ++it) {
    this->stmt += (*it)->str();
    if (it != --ropds->end())
      this->stmt += ", ";
  }
  this->stmt += ")";
}

SSA_Stmt::~SSA_Stmt() {
  if (this->lopd != NULL)
    delete this->lopd;

  if (this->ropd1 != NULL)
    delete this->ropd1;

  if (this->ropd2 != NULL)
    delete this->ropd2;

  if (this->ropds != NULL) {
    for (list<SSA_Opd *>::iterator it = this->ropds->begin(); it != this->ropds->end(); ++it)
      delete *it;

    delete this->ropds;
  }
}

SSA_StmtType SSA_Stmt::get_type() { return this->type; }

void SSA_Stmt::dump() {
  lopd->dump();
  *dot_fd << " = ";
  if (this->type == SSA_AssignStmt) {
    ropd1->dump();
    if (this->ropd2 != NULL) {
      *dot_fd << ' ' << op << ' ';
      ropd2->dump();
    }
    return;
  }

  *dot_fd << "PHI(";
  bool first = true;
  for (SSA_Opd *ropd : *ropds) {
    if (first) {
      first = false;
    } else {
      *dot_fd << ", ";
    }
    ropd->dump();
  }
  *dot_fd << ')';
}

std::string &SSA_Stmt::get_stmt() { return this->stmt; }

std::string &SSA_Stmt::get_op() { return op; }

SSA_Opd *SSA_Stmt::get_lhs() { return lopd; }

std::vector<SSA_Opd *> SSA_Stmt::get_rhs() {
  CHECK_INVARIANT(type == SSA_AssignStmt, "Can't get RHS of non assign stmt");
  std::vector<SSA_Opd *> res;
  if (ropd1 != nullptr) {
    res.push_back(ropd1);
  }
  if (ropd2 != nullptr) {
    res.push_back(ropd2);
  }
  return res;
}

std::list<SSA_Opd *> *SSA_Stmt::get_phi_uses() {
  CHECK_INVARIANT(type == SSA_PhiStmt, "Can't get PHI uses of non PHI stmt");
  return ropds;
}
