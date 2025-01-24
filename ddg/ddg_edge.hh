#ifndef _DDG_EDGE_HH_
#define _DDG_EDGE_HH_

class DDG_Node;

class DDG_Edge {
public:
  DDG_Node *src;
  DDG_Node *dst;

  // DDG_Edge(DDG_Node *src, DDG_Node *dst);
  // ~DDG_Edge();
};

#endif
