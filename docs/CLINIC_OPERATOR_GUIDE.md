# Clinic operator guide

1. Build with `DNXR_CLINICAL_TEST_BUILD=ON` only for authorized clinical investigations.
2. Place signed QMS, regulatory, ethics, protocol, site, and role artifacts in the expected paths.
3. Run `tools/clinical/dnxr_clinic_unlock_check.py` before connecting a human subject.
4. Confirm the status panel shows `CLINICAL_TEST_UNLOCKED` before human connection and `CLINICAL_STIMULUS_DISPATCH_UNLOCKED` before any protocol-authorized dispatch.
5. Stop immediately if monitoring is lost, the session expires, a signature is revoked, or either emergency stop is activated.
