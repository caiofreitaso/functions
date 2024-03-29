#include "Function.hpp"

Function::node::node() {
	 child[0] = child[1] = 0;
	 var = -1;
	 value = 0;
	 type = 0;
}

double Function::node::v(){
	if (type == 1)
		return f1(child[0]->v());
	else if (type == 2)
		return f2(child[0]->v(), child[1]->v());
	else
		return value;
}

void Function::make_child(Function::node*& a, Function::node*& b) {
	swap_node(a,b);
	b->child[0] = a->child[0];
	b->child[1] = a->child[1];
	a->child[0] = b;
	a->child[1] = 0;
}
void Function::swap_node(Function::node* a, Function::node* b) {
	node *tmp = new node();
	tmp->value = a->value;
	tmp->type = a->type;
	
	a->value = b->value;
	a->type = b->type;
	
	b->value = tmp->value;
	b->type = tmp->type;
	delete tmp;
}
void Function::change_value(Function::node* a, int var, double val) {
	if (a->type == 4 && a->var == var)
		a->value = val;
	if (a->child[0]) change_value(a->child[0],var,val);
	if (a->child[1]) change_value(a->child[1],var,val);
}

void Function::buildTree(char*& c) {
	std::stack<TOKEN> t;
	t.push(FUNCT);

	std::string vars = "";
	
	node *current = tree;
	std::stack<node*> open_p;

	bool ok = true, as_child = false;
	while (!t.empty() && ok)
		switch (t.top()) {
			case FUNCT:
				if (*c == 0)
					t.pop();
				else if (strchr("+-*/^",*c))
					t.push(OPT_BI);
				else
					t.push(EXP);
				break;
			case OPEN_P:
				if (*c == '(') {
					t.pop();
					if (!tree) {
						tree = new node();
						current = tree;
						open_p.push(tree);
					} else {
						node *tmp = new node();
						if (current->child[0])
							current->child[1] = tmp;
						else
							current->child[0] = tmp;
						current = tmp;
						open_p.push(tmp);
					}
					c++;
				}
				else
					ok = false;
				break;
			case CLOSE_P:
				if (*c == ')') {
					t.pop();
					node *x = open_p.top()->child[0];
					open_p.top()->value = x->value;
					open_p.top()->type = x->type;
					open_p.top()->child[0] = x->child[0];
					open_p.top()->child[1] = x->child[1];
					current = open_p.top();
					open_p.pop();
					delete x;
					as_child = true;
					c++;
				} else if (strchr("+-*/^",*c)) {
					t.push(EXP);
					t.push(BI);
				}
				else
					ok = false;
				break;
			case BI:
				if (strchr("+-*/^",*c)) {
					t.pop();
					
					node *tmp = new node();

					switch(*c) {
						case '+':tmp->f2 = add; break;
						case '-':tmp->f2 = sub; break;
						case '*':tmp->f2 = mul; break;
						case '/':tmp->f2 = Function::div; break;
						case '^':tmp->f2 = pw; break;
					}
					tmp->type = 2;
					
					if (*c == '+' || *c == '-' || current->type == 3 || as_child)
						make_child(current, tmp);
					else {
						if (current->child[0])
							if (current->child[1]) {
								make_child(current->child[1], tmp);
								current = current->child[1];
							} else
								current->child[1] = tmp;
						else
							current->child[0] = tmp;
					}
					as_child = false;
					c++;
				} else
					ok = false;
				break;
			case UN:
				break;
			case EXP:
				if (*c == 'a' || *c == 's' || *c == '-' ||
					*c == 'c' || *c == 'l' || *c == 't') {
					t.pop();
					t.push(CLOSE_P);
					t.push(EXP);
					t.push(OPEN_P);
					t.push(UN);
				} else if (*c == '(') {
					t.pop();
					t.push(CLOSE_P);
					t.push(EXP);
					t.push(OPEN_P);
				} else if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789%",*c)) {
					t.pop();
					t.push(VAR);
				} else
					ok = false;
				break;
			case VAR:
				{
					t.pop();
					
					node *tmp = new node();
					tmp->type = 3;

					if (!tree)
						current = tree = tmp;
					else {
						if (current->child[0])
							current->child[1] = tmp;
						else
							current->child[0] = tmp;
						if (!open_p.empty())
							if (current == open_p.top())
								current = tmp;
					}

					if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ",*c)) {
						tmp->type = 4;
						tmp->var = vars.find(*c);
						if (tmp->var == -1) {
							vars += *c;
							tmp->var = numVar;
							numVar++;									
						}
						c++;
						t.push(OPT_BI);
					} else if (strchr("0123456789",*c)) {
						std::string v;
						for (; strchr("0123456789",*c); c++)
							if (*c == 0)
								break;
							else
								v += *c;
						tmp->value = atof(v.c_str());
					} else if (*c == '%') {
						c++;
						if (*c == 'p'){
							c++;
							if (*c == 'i') {
								current->value = M_PI;
								c++;
							} else
								ok = false;
						}
						else if (*c == 'e') {
							current->value = M_E;
							c++;
						} else
							ok = false;
					} else
						ok = false;
				}
				break;
			case OPT_BI:
				if (*c == 0)
					t.pop();
				else if (strchr("+-*/^",*c)) {
					t.pop();
					t.push(EXP);
					t.push(BI);
				}
				else
					ok = false;
		}

	if (!ok)
		deleteNode(tree);
}
void Function::deleteNode(Function::node*& n) {
	if (n->child[1])
		deleteNode(n->child[1]);
	if (n->child[0])
		deleteNode(n->child[0]);
	delete n;
}

double Function::operator()() {
	if (numVar)
		return std::numeric_limits<double>::quiet_NaN();
	return tree->v();
}

double Function::operator()(double x) {
	if (!numVar)
		return std::numeric_limits<double>::quiet_NaN();
	change_value(tree, 0, x);
	return tree->v();
}

double Function::operator()(int n, double inputs[]) {
	if (n != numVar + 1)
		return std::numeric_limits<double>::quiet_NaN();
	for (int i = 0; i < n; i++)
		change_value(tree, i, inputs[i]);

	return tree->v();
}

Function::Function(char *input) {
	tree = 0;
	numVar = 0;
	buildTree(input);
}