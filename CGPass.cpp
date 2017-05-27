#include "CGPass.h"

#include <iostream>

#include <fstream>

#include <deque>




void CallGraph::printNode(std::string dotString) const {
	for (Function* f : valueList) {
		//dotString += "Node" + f;
	}
}



void CallGraph::dump() const {

	std::error_code error;
	enum sys::fs::OpenFlags F_None;
	StringRef fileName("CG.dot");
	raw_fd_ostream file(fileName, error, F_None);

	std::string dotString = "";

	file << "digraph \"Call Graph\"{\n";
	file << "label=\"Call Graph\";\n";

	for (Function* f : valueList) {
		file << "Node" << f << " [shape=record, label=\"{" << f->getName() << "}\"];\n";
	}

	for (auto pairs : m_map) {
		for (Function* second : pairs.second) {
			file << "Node" << pairs.first << " -> " << "Node" << second << ";\n";
		}

	}

	file <<  "}\n";
	file.close();

}



bool CGPass::runOnModule(Module &M) {

	Function *main = M.getFunction("main");

	G = new CallGraph(main);
	G->valueList.push_back(main);
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
						//errs() <<"instruction2\n";

						G->AddEdge(func, called);

						if (!G->hasFunction(called))

						{
							list.push_back(called);
							G->valueList.push_back(called);
						}

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

						{
							list.push_back(called);
							G->valueList.push_back(called);
						}

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
