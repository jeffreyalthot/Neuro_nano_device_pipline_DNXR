#include "NeuralSignalEmulator.h"
namespace dnxr { std::vector<double> NeuralSignalEmulator::generate(std::size_t channels, double amplitude) const { std::vector<double> signal(channels); for(std::size_t i=0;i<channels;++i) signal[i]=amplitude*(static_cast<double>((i%5)+1)/5.0); return signal; } }
