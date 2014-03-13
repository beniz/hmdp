/*
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
#include <config.h>
#include <iostream>
#include <stdlib.h>
#if HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
namespace std {
  typedef std::ostrstream ostringstream;
}
#endif
#include "strxml.h"

std::string next_token(std::istream& is)
{
  static char last_char = 0;

  std::string res;
  if (last_char)
    res += last_char;

  if (last_char == '<') {
    last_char = 0;
    return res;
  }

  if (last_char == '>') {
    last_char = 0;
    return res;
  }

  int next_char;
  while (1) {
    next_char = is.get();
    if (next_char == 1 || next_char == -1 || next_char == 0) {
      return std::string("");
    }
    if (next_char == '>' || next_char == '<') {
      if (res.length() == 0) {
        res += next_char;
        last_char = 0;
        return res;
      }
      last_char = next_char;
      break;
    }
    res += next_char;
  }
  return res;
}

int token_type(char c)
{
  if (c == '=')
    return 2;
  if (c == '"')
    return 3;
  if (c == '/')
    return 4;
  if (isspace(c))
    return 0;
  return 1;
}

str_vec tokenize_string(std::string str)
{
  str_vec v;
  std::string last;
  int t_type = 0;
  for (const char *s = str.c_str(); *s; s++) {
    int n_type = token_type(*s);

    if (t_type != n_type && last.length() != 0) {
      if (t_type)
        v.push_back(last);
      last.erase();
    }
    last += *s;
    t_type = n_type;
  }
  if (last.length())
    v.push_back(last);
  return v;
}

int do_node(std::string token, PSink& ps)
{
  str_vec node_tokens = tokenize_string(token);

  //cout << token << ":" << endl;
  //for (int i=0; i<node_tokens.size(); i++)
  //  cout << node_tokens[i] << endl;

  if (node_tokens.size() == 0) {
    //cerr << "e1a" << endl;
    return -2;
  }
    
  if (node_tokens[0] == "/") {
    ps.popNode(node_tokens[1]);
    return -1;
  }
  std::string name = node_tokens[0];
  str_pair_vec v;
  for (size_t i=1; i<node_tokens.size(); i+=5) {
    if (node_tokens[i] == "/") {
      ps.pushNode(name, v);
      ps.popNode(name);
      return 0;
    }
    if (i+5 > node_tokens.size() ||
        node_tokens[i+1] != "=" ||
        node_tokens[i+2] != "\"" ||
        node_tokens[i+4] != "\"") {
      //cerr << "e1b" << endl;
      return -2;
    }
    str_pair p(node_tokens[i], node_tokens[i+3]);
    v.push_back(p);
  }
  ps.pushNode(name, v);
  return 1;
}

int parseStream(std::istream& is, PSink& ps)
{
  std::string token = next_token(is);
  int depth = 0;
  while (token.length() != 0) {
    if (token == "<") {
      int delta = do_node(next_token(is), ps);
      if (delta == -2) {
	//cerr << "e1" << endl;
	ps.formaterror();
	return 1;
      }
      depth += delta;
      token = next_token(is);
      if (token != ">") {
	//cerr << "e2" << endl;
	ps.formaterror();
	return 1;
      }
      if (depth == 0)
	return 0;
    }
    else
      ps.pushText(token);
    token = next_token(is);
  }
  ps.streamerror();
  return 1;
}

void PSink::pushNode(std::string name, str_pair_vec params)
{
  //cout << "push: " << name << endl;
  XMLParent *p = new XMLParent;
  p->type = 2;
  p->name = name;
  for (size_t i=0; i<params.size(); i++)
    p->params[params[i].first] = params[i].second;
  if (s.size() == 0)
    top = p;
  else
    ((XMLParent*)(s.top()))->children.push_back(p);
  s.push(p);
}

void PSink::popNode(std::string name)
{
  //cout << "pop: " << name << endl;
  s.pop();
}

void PSink::pushText(std::string text)
{
  XMLText *t = new XMLText;
  t->type = 1;
  t->text = text;
  if (s.size() == 0)
    top = t;
  else
    ((XMLParent*)(s.top()))->children.push_back(t);
}

XMLText::XMLText()
{
  type = 2;
}

void XMLText::print(std::ostream& os)
{
  os << text;
}

XMLParent::XMLParent()
{
  type = 2;
}

void XMLParent::print(std::ostream& os)
{
  os << "<" << name;
  for (str_str_map::iterator itr = params.begin(); itr != params.end(); itr++)
    os << " " << itr->first << "=\"" << itr->second << "\"";
  os << ">";
  for (size_t i=0; i<children.size(); i++)
    children[i]->print(os);
  os << "</" << name << ">";
}

XMLNodePtr XMLParent::getChild(int i)
{
  return children[i];
}

XMLNodePtr XMLParent::getChild(std::string s)
{
  for (size_t i=0; i<children.size(); i++) {
    if (children[i]->type == 2) {
      XMLParent *p = (XMLParent*)children[i];
      if (p->name == s)
	return p;
    }
  }
  return 0;
}

int XMLParent::size()
{
  return children.size();
}

std::string XMLText::getText()
{
  return text;
}

std::string XMLText::getName()
{
  return text;
}

std::string XMLText::getParam(std::string s)
{
  return "";
}

std::string XMLParent::getText()
{
  std::ostringstream os;
  for (size_t i=0; i<children.size(); i++)
    children[i]->print(os);
#if !HAVE_SSTREAM
  os << '\0';
#endif
  std::string s = os.str();
  return s;
}

std::string XMLParent::getName()
{
  return name;
}

std::string XMLParent::getParam(std::string s)
{
  return params[s];
}

XMLParent::~XMLParent()
{
  for (size_t i=0; i<children.size(); i++)
    delete children[i];
}

int dissectNode(XMLNodePtr p, std::string child, std::string& destination)
{
  if (!p)
    return 0;
  XMLNodePtr c = p->getChild(child);
  if (!c)
    return 0;
  destination = c->getText();
  return 1;
}

XMLNodePtr getNodeFromStream(std::istream& is)
{
  PSink ps;
  int ret = parseStream(is, ps);
  if (ret) {
    return 0;
  }
  return ps.top;
}

std::ostream& operator << (std::ostream& os, XMLNodePtr& xn)
{
  xn->print(os);
  return os;
}

std::istream& operator >> (std::istream& is, XMLNodePtr& xn)
{
  if (xn)
    delete xn;
  xn = getNodeFromStream(is);
  return is;
}

XMLNode* XMLNode::getChild(int i)
{
  return 0;
}
XMLNode* XMLNode::getChild(std::string s)
{
  return 0;
}
int XMLNode::size()
{
  return 0;
}



/*
int main(int argc, char **argv)
{
  XMLNodePtr node = 0;
  cin >> node;
  cout << node->getChild("name")->getText() << endl;
  return 0;
}
*/



