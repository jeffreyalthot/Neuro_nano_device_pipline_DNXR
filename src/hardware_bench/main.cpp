#include "hardware_bench/HardwareBenchRuntime.h"
#include <iostream>
#include <string>
namespace {
std::string arg(int argc,char** argv,const std::string& name,const std::string& d=""){ for(int i=1;i+1<argc;++i) if(argv[i]==name) return argv[i+1]; return d; }
}
int main(int argc,char** argv){
    const auto manifestPath=arg(argc,argv,"--manifest");
    const auto device=arg(argc,argv,"--device","loopback-bench-device");
    const auto mode=arg(argc,argv,"--mode","dry-run");
    (void)arg(argc,argv,"--transport","loopback");
    try {
        auto manifest = manifestPath.empty()?dnxr::HardwareBenchSession::dryRunManifest():dnxr::HardwareBenchSession::loadManifest(manifestPath);
        manifest.hardwareSerial=device;
        dnxr::RealNeuralChipHAL hal;
        dnxr::HardwareBenchRuntime runtime;
        if(mode=="active-dummy"){
            manifest.activeStimulationEnabled=true;
            manifest.maxOutputCeiling=0.01f;
            dnxr::DummyLoadAdapter dummy;
            dummy.attach();
            dnxr::BoundedStimulusCommand cmd;
            cmd.activeOutputEnabled=true; cmd.maxOutputCeiling=0.01f; cmd.normalizedCeiling=0.01f; cmd.maxDeltaPerFrame=0.001f; cmd.cumulativeBudget=0.1f; cmd.validatedBySafetyKernel=true;
            auto r=runtime.sendActiveToDummyLoad(hal,manifest,dummy,cmd);
            std::cout<<r.reason<<"\n";
            return r.activeOutputSent?0:2;
        }
        auto r=runtime.startDryRun(hal,manifest);
        std::cout<<r.reason<<"\n";
        return r.started?0:2;
    } catch(const std::exception& e){ std::cerr<<e.what()<<"\n"; return 1; }
}
