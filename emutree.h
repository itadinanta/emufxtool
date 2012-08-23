#ifndef __TREE_H
#define __TREE_H

#include "strdsp.h"
namespace emu {

	public class Node {
		Node *parent;
		int refCount;
		int id;
		emuString name;
	      protected:
		void detach() {
			refCount--;
			if (!refCount)
				killChildren();
		} virtual Node *killChildren() {
		} virtual void attach() {
			refCount++;
		}
		virtual Node *remove(Node * child) {
		}
	      public:
		Node(int id, emuString name) {
			this->parent = 0;
			refCount = 1;
			this->id = id;
			this->name = name;
		};
		Node(Node * parent, int id, emuString name) {
			this->parent = parent;
			this->name = name;
			this->id = id;
			if (parent)
				parent->attach();
			refCount = 1;
		}
		virtual int getId() {
			return 0;
		}
		virtual int getFlags() {
			return 0;
		}
		virtual emuString getStringValue() {
			return 0;
		}
		virtual emuString getName() {
			return 0;
		}
		virtual int getNumberValue() {
			return 0;
		}
		virtual int getChildrenCount() {
			return 0;
		}
		virtual Node *getChildByName(const String & id) {
			return 0;
		}
		virtual Node *getChildById(int id) {
			return 0;
		}
		virtual Node *getParent() {
			return parent;
		}
		virtual ~ Node() {
			if (parent) {
				parent->remove(this);
				parent->detach();
			}
			killChildren();
		}
	}

	public class Leaf:public Node {
		int intValue;
		emuString stringValue;
	      public:
		  Leaf(Node * parent):Node(parent) {

		} Leaf(Node * parent, int id);
		virtual emuString getString() {
			return stringValue;
		}
		virtual int getNumber() {
			return intValue;
		}
	}

	public class Branch:public Node {
	      public:
		Leaf();
		Leaf(int id);
		virtual int getId();
		virtual int getFlags();
		virtual emuString getString();
		virtual int getNumber();
		virtual Node *getProperty(const String & id);
		virtual Node *getProperty(int id) = 0;
		virtual Node *getParent();
	} public class Address:public Leaf {

	} public class Value:public Leaf {

	} public class Name:public Leaf {
	      public:
		Name(emuString name) {

	}} public class Control:public Branch {

	} public class Variable:public Branch {

	} public class Constant:public Branch {

	} public class Tram:public Branch {

	} public class ITram:public Tram {

	} public class ETram:public Tram {

	} public class PCM:public Branch {

	} public class Line:public Branch {

	} public class LRead:public Branch {

	} public class LWrite:public Branch {

	}
/**
 * Code Hierarchy
 * Code
 *   Line
 *       Operator
 *       Operand
 *           
 *       Operand
 *       Operand
 *       Operand
 *   Line...
 *     
 */ public class Code:public Branch {

	} public class Line:public Branch {

	} public class Operator:public Leaf {

	} public class Operand:public Leaf {

	}
#endif
