#pragma once
namespace dnxr { class SignalSinkProbe { public: void record(float v){ last_=v; count_++; } float last() const { return last_; } unsigned count() const { return count_; } private: float last_=0.0f; unsigned count_=0; }; }
