/* -*-C++-*- */
/*
 * XML.
 *
 * Copyright (C) 2003 Carnegie Mellon University and Rutgers University
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 */
#ifndef _STRXML_H
#define _STRXML_H

#include <vector>
#include <string>
#include <map>
#include <stack>

class XMLNode
{
 public:
  int type;
  virtual XMLNode* getChild(int i);
  virtual XMLNode* getChild(std::string s);
  virtual int size();
  virtual void print(std::ostream& os) = 0;
  virtual std::string getText() = 0;
  virtual std::string getName() = 0;
  virtual std::string getParam(std::string s) = 0;
};

typedef XMLNode* XMLNodePtr;
typedef std::pair<std::string, std::string> str_pair;
typedef std::vector<str_pair> str_pair_vec;
typedef std::vector<std::string> str_vec;
typedef std::map<std::string, std::string> str_str_map;
typedef std::vector<XMLNodePtr> node_vec;
typedef std::stack<XMLNodePtr> node_stk;

class XMLText : public XMLNode
{
 public:
  std::string text;
  XMLText();
  virtual ~XMLText() {}
  virtual void print(std::ostream& os);
  virtual std::string getText();
  virtual std::string getName();
  virtual std::string getParam(std::string s);
};

class XMLParent : public XMLNode {
 public:
  std::string name;
  str_str_map params;
  node_vec children;
  XMLParent();
  virtual ~XMLParent();
  virtual void print(std::ostream& os);
  XMLNodePtr getChild(int i);
  XMLNodePtr getChild(std::string s);
  virtual int size();
  virtual std::string getText();
  virtual std::string getName();
  virtual std::string getParam(std::string s);

};

class PSink
{
  node_stk s;
 public:
  int error;
  XMLNode *top;
  virtual void pushNode(std::string name, str_pair_vec params);
  virtual void popNode(std::string name);
  virtual void pushText(std::string text);

  virtual void formaterror() {error=1;}
  virtual void streamerror() {error=2;}

  PSink() {top=0;error=0;}
  virtual ~PSink() {}
};

int dissectNode(XMLNodePtr p, std::string child, std::string& destination);

int parseStream(std::istream& is, PSink& ps);
XMLNodePtr getNodeFromStream(std::istream& is);

std::ostream& operator << (std::ostream& os, XMLNodePtr& xn);
std::istream& operator >> (std::istream& is, XMLNodePtr& xn);

#endif
