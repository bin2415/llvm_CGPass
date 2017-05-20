#include "CGPass.h"

#include <iostream>

#include <fstream>

#include <deque>





std::string getFunctionName(Function *f)

{

	//DISubprogram *disubprogram = f->getSubprogram();

	//if (disubprogram)

	//{

	//	return disubprogram->getName().str();

	//}

	std::string result = f->getName().str();

	return result;



	//return "";

}

void CallGraph::recur(Function *func, std::set<Function *> &done) const

{

	//如果已经遍历过，就返回

	if (done.count(func) == 1)

		return;

	if (m_map.find(func) != m_map.end())

	{

		for (Function *f : m_map.find(func)->second)

		{

			errs() << getFunctionName(func) << "->" << getFunctionName(f) << "\n";

			recur(f, done);

		}

	}

	

}



void CallGraph::recurDot(Function *func, std::set<Function *> &done, std::string &s) const

{

	if (done.count(func) == 1) return;

	done.insert(func);

	if (m_map.find(func) != m_map.end())

	{

		for (Function *f : m_map.find(func)->second)

		{

			s += getFunctionName(func) + "->" + getFunctionName(f) + ";\n";

			recurDot(f, done, s);

		}

	}

}



void CallGraph::print() const {

	errs() << "print the Call Graph" << "\n";

	std::set<Function*> done;

	recur(mMain, done);

}



void CallGraph::dump() const {

	std::string dotString = "";

	dotString += "digraph {\n";

	std::set<Function *>done;

	recurDot(mMain, done, dotString);

	dotString += "}\n";



	std::ofstream os;

	os.open("CG.dot");

	if (os.is_open())

	{

		os << dotString << "\n";

		os.close();

		errs() << "Call Graph Done to CG.dot\n";

	}

	else

	{

		errs() << "Error: Can't Open the CG.dot\n";

	}

}



bool CGPass::runOnModule(Module &M){

	Function *main = M.getFunction("main");

	G = new CallGraph(main);

	if (!main) return false;

	std::deque<Function*> list;

	list.push_back(main);

	while (!list.empty()) {

		Function* func = list.front();

		list.pop_front();

		for (Function::iterator iter = func->begin(); iter != func->end(); ++iter)

		{

			

			for (BasicBlock::iterator Biter = iter->begin(); Biter != iter->end(); ++Biter)

			{

				Instruction *I = &*Biter;

				if (CallInst *inst = dyn_cast<CallInst>(I))

				{

					//errs() <<"instruction\n";

					Function* called = inst->getCalledFunction();

					if (called)

					{

						//errs() <<"instruction1\n";

						//DISubprogram *dissubprogram = called->getSubprogram();

						//if (dissubprogram)

						//{

							//errs() <<"instruction2\n";

							G->AddEdge(func, called);

							if (!G->hasFunction(called))

								list.push_back(called);

						//}

					}

				}

				if (InvokeInst *inst = dyn_cast<InvokeInst>(I))

				{

					Function* called = inst->getCalledFunction();

					errs() << "hello\n";

					if (called)

					{

							G->AddEdge(func, called);

							if (!G->hasFunction(called))

								list.push_back(called);

					}

				}

			}

		}

	}

	//G->print();

	G->dump();

}



//注册Pass

static RegisterPass<CGPass> X("CGPass", "Call Graph Pass", false, false);
