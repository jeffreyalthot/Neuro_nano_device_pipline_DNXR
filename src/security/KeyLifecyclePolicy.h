#pragma once
namespace dnxr { struct KeyLifecyclePolicy { int rotationDays=90; bool hardwareBackedStorageRequired=true; bool revokedKeysRejected=true; bool acceptable() const { return rotationDays > 0 && rotationDays <= 180 && revokedKeysRejected; } }; }
