// Syntax:
// 		variable name <var>: [A-Z]
// 		constants <cte>: [0-9]+ | "%e" | "%pi"
// 		unary operators <un>: "-" | "ln" | "abs" | "sin" | "cos" | "tan" | "arcsin" | "arccos" | "arctan"
// 		binary operators <bi>: [+-*/^]
//
//	- <un> (<exp>)
//	- <exp> <bi> <exp>
//	- (<exp> <bi> <exp>)

#ifndef FUNCTION_H
#define FUNCTION_H

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <limits>
#include <stack>
#include <string>

class Function {
	private:
		class node {
			private:
				node *child[2];
				union {
					double (*f1)(double const&);
					double (*f2)(double const&,double const&);
					double value;
				};
				int var;
				char type;

				friend class Function;
			public:
				node() { child[0] = child[1] = 0; var = -1; value = 0; type = 0; }
				double v() {
					if (type == 1)
						return f1(child[0]->v());
					else if (type == 2)
						return f2(child[0]->v(), child[1]->v());
					else
						return value;
				}
		};

		static void make_child(node*& a, node*& b) {
			swap_node(a,b);
			b->child[0] = a->child[0];
			b->child[1] = a->child[1];
			a->child[0] = b;
			a->child[1] = 0;
		}
		static void swap_node(node* a, node* b) {
			node *tmp = new node();
			tmp->value = a->value;
			tmp->type = a->type;
			
			a->value = b->value;
			a->type = b->type;
			
			b->value = tmp->value;
			b->type = tmp->type;
			delete tmp;
		}
		static void change_value(node* a, int var, double val) {
			if (a->type == 4 && a->var == var)
				a->value = val;
			if (a->child[0]) change_value(a->child[0],var,val);
			if (a->child[1]) change_value(a->child[1],var,val);
		}

		node *tree;
	protected:
		static double add(double const& a, double const& b) { return a + b; }
		static double sub(double const& a, double const& b) { return a - b; }
		static double mul(double const& a, double const& b) { return a * b; }
		static double div(double const& a, double const& b) { return a / b; }
		static double e(double const& a) { return exp(a); }
		static double pw(double const& a, double const& b) { return  pow(a,b); }

	private:
		enum TOKEN {
			VAR,
			UN,
			BI,
			OPT_BI,
			EXP,
			OPEN_P,
			CLOSE_P,
			FUNCT
		};

		void buildTree(char*& c);
		void deleteNode(node*& n);
	public:
		Function(char *input);
		~Function() { deleteNode(tree); }
		double operator()(int n, double inputs[]);
		double operator()(double x);
		double operator()();

	private:
		class node {
			private:
				node *child[2];
				union {
					double (*f1)(double const&);
					double (*f2)(double const&,double const&);
					double value;
				};
				int var;
				char type;

				friend class Function;
			public:
				node();
				double v();
		};
};

#endif