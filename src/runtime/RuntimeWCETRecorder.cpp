#include "RuntimeWCETRecorder.h"
namespace dnxr { void RuntimeWCETRecorder::observe(double ms){if(ms>worst_) worst_=ms;} double RuntimeWCETRecorder::worstCaseMs() const{return worst_;} bool RuntimeWCETRecorder::within(double budgetMs) const{return worst_<=budgetMs;} }
