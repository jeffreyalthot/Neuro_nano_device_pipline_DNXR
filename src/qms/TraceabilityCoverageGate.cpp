#include "qms/TraceabilityCoverageGate.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
namespace dnxr {
namespace { std::string norm(std::string s){ s.erase(std::remove_if(s.begin(),s.end(),[](unsigned char c){return c==' '||c=='_'||c=='-'||c=='/';}),s.end()); std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));}); return s;} std::vector<std::string> split(const std::string& line){ std::vector<std::string> out; std::string cur; std::stringstream ss(line); while(std::getline(ss,cur,',')) out.push_back(cur); return out;} }
TraceabilityCoverageGate::TraceabilityCoverageGate(std::string p):matrixPath_(std::move(p)){}
TraceabilityCoverageDecision TraceabilityCoverageGate::evaluate() const { TraceabilityCoverageDecision d; std::ifstream in(matrixPath_); if(!in){d.gaps.push_back("missing traceability matrix"); return d;} std::string header; if(!std::getline(in,header)){d.gaps.push_back("empty traceability matrix"); return d;} auto cols=split(header); std::vector<std::string> req={"requirement","hazard","riskcontrol","sourcefile","test","evidenceartifact","verificationmethod","status","reviewersignoff"}; std::vector<int> idx; for(const auto& r:req){ auto it=std::find_if(cols.begin(),cols.end(),[&](const std::string& c){return norm(c)==r || norm(c).find(r)!=std::string::npos;}); if(it==cols.end()){ d.gaps.push_back("missing column: "+r); idx.push_back(-1);} else idx.push_back(static_cast<int>(std::distance(cols.begin(),it))); } std::string line; int row=1; while(std::getline(in,line)){ ++row; if(line.empty()) continue; auto cells=split(line); ++d.rowsChecked; for(std::size_t i=0;i<idx.size();++i){ if(idx[i]<0) continue; if(static_cast<std::size_t>(idx[i])>=cells.size() || cells[idx[i]].empty()) d.gaps.push_back("row "+std::to_string(row)+" missing "+req[i]); } } if(d.rowsChecked==0) d.gaps.push_back("no requirement rows"); d.complete=d.gaps.empty(); return d; }
}
