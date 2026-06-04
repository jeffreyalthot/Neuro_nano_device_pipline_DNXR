#include "RuntimeJitterAnalyzer.h"
#include <cmath>
namespace dnxr { void RuntimeJitterAnalyzer::recordPeriod(double ms){periods_.push_back(ms);} double RuntimeJitterAnalyzer::maxJitterMs() const{ if(periods_.size()<2) return 0.0; double sum=0; for(double v:periods_) sum+=v; double avg=sum/static_cast<double>(periods_.size()); double max=0; for(double v:periods_){double j=std::fabs(v-avg); if(j>max) max=j;} return max;} bool RuntimeJitterAnalyzer::within(double budgetMs) const{return maxJitterMs()<=budgetMs;} }
