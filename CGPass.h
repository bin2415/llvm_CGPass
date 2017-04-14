#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <string>
#include <set>
using namespace llvm;

namespace {
	class CallGraph {
	public:
		CallGraph(Function *mainFunc) : mMain(mainFunc) {}
		~CallGraph() {}
		void AddEdge(Function * f, Function * t) {
			m_map[f].insert(t);
		}

		//打印CallGraph及其信息
		void print() const;

		//将Callgraph输出到dot图中
		void dump() const;
		bool hasFunction(Function *func) { std::map<Function*, std::set<Function*> >::iterator iter = m_map.find(func);
		if (iter == m_map.end())
			return false;
		return true;

		}
		std::map<Function*, std::set<Function*> > m_map;
		//std::string dotString;
		//std::string getFunctionName(Function* f);
	private:
		Function* mMain; //主函数
		
		unsigned int edge_count = 0;
		
		void recur(Function *func, std::set<Function*> &done) const;
		void recurDot(Function* func, std::set<Function*> &done, std::string &s) const;

	};

	
	class CGPass : public ModulePass
	{
	public:
		static char ID;
		CGPass() : ModulePass(ID) {}
		virtual bool runOnModule(Module &M) override;
	private:
		CallGraph *G;
	};
	char CGPass::ID = 0;
}

