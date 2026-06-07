#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
int main(){ std::ifstream in("../src/runtime/RuntimeStimulusStage.cpp"); if(!in) in.open("src/runtime/RuntimeStimulusStage.cpp"); std::stringstream ss; ss<<in.rdbuf(); const auto s=ss.str(); const auto marker=s.find("RuntimePipelineFrame RuntimeStimulusStage::execute(RuntimePipelineFrame frame, RuntimeStageServices& services)"); assert(marker!=std::string::npos); const auto modern=s.substr(marker); assert(modern.find("services.nodes.writeNeuralStimulus")==std::string::npos); return 0; }
