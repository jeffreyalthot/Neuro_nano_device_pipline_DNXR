#!/usr/bin/env python3
import argparse, json, sys
from datetime import datetime, timezone
from pathlib import Path

def load(path):
    if not path: return None, ["not provided"]
    p=Path(path)
    if not p.exists(): return None, [f"missing {path}"]
    try: return json.loads(p.read_text()), []
    except Exception as e: return None, [f"invalid json {path}: {e}"]

def iso_expired(value):
    if not value: return False
    try:
        return datetime.fromisoformat(value.replace('Z','+00:00')) <= datetime.now(timezone.utc)
    except Exception:
        return True

def signed(obj):
    sig=(obj or {}).get('signature',{})
    return sig.get('algorithm')=='Ed25519' and bool(sig.get('key_id')) and bool(sig.get('value'))

def yes(v): return 'yes' if v else 'no'

ap=argparse.ArgumentParser(description='DNXR clinical unlock status panel')
ap.add_argument('--clinic-unlock', required=True); ap.add_argument('--session-unlock')
ap.add_argument('--trust-store'); ap.add_argument('--revocations'); ap.add_argument('--source-root', default='.')
args=ap.parse_args()
clinic, blockers = load(args.clinic_unlock)
session, session_blockers = load(args.session_unlock) if args.session_unlock else (None, ['patient session not loaded yet'])
revoked=set()
if args.revocations and Path(args.revocations).exists():
    data=json.loads(Path(args.revocations).read_text()); revoked=set(data.get('revoked_key_ids', data if isinstance(data,list) else []))

caps=dict(clinical_review=False,human_connection=False,signal_recording=False,calibration=False,stimulus_dispatch=False)
reasons=[]; status='BLOCKED'
if clinic and signed(clinic):
    key=clinic.get('signature',{}).get('key_id')
    if key in revoked: blockers.append('clinic unlock signing key revoked')
    if iso_expired(clinic.get('signature',{}).get('expires_at')) or iso_expired(clinic.get('protocol',{}).get('expires_at')): blockers.append('clinic unlock or protocol expired')
    qms=clinic.get('qms',{}); reg=clinic.get('regulatory',{}); roles=clinic.get('clinical_roles',{}); site=clinic.get('clinical_site',{})
    if all(qms.get(k) for k in ('qms_signed','risk_file_signed','verification_package_signed','validation_package_signed')):
        caps['clinical_review']=True; status='CLINICAL_REVIEW_UNLOCKED'; reasons.append('QMS package valid')
    else: blockers.append('QMS package incomplete')
    regulatory = (reg.get('reb_approval') and reg.get('health_canada_ita')) or (reg.get('irb_approval') and reg.get('fda_ide'))
    human = caps['clinical_review'] and regulatory and clinic.get('protocol',{}).get('signed') and site.get('authorized') and roles.get('principal_investigator_signed') and roles.get('operator_authorized', True)
    if human:
        status='CLINICAL_TEST_UNLOCKED'; caps.update(human_connection=True, signal_recording=True, calibration=True); reasons += ['clinical site authorized','protocol signed','REB/IRB and ITA/IDE path present','physician investigator signed']
    elif caps['clinical_review']:
        if not regulatory: blockers.append('missing REB/IRB plus ITA/IDE regulatory path')
        if not clinic.get('protocol',{}).get('signed'): blockers.append('clinical protocol not signed')
        if not site.get('authorized'): blockers.append('clinical site not authorized')
        if not roles.get('principal_investigator_signed'): blockers.append('physician investigator missing')
else:
    blockers.append('clinic unlock signature invalid or missing')

if session:
    if not signed(session): session_blockers.append('session signature invalid or missing')
    if session.get('signature',{}).get('key_id') in revoked: session_blockers.append('session signing key revoked')
    if iso_expired(session.get('session_limits',{}).get('expires_at')): session_blockers.append('session unlock file expired')
    needed=['patient_consent_signed','patient_calibration_signed','session_prescription_signed','monitoring_active','physical_emergency_stop_verified','software_emergency_stop_verified','clinician_present']
    for k in needed:
        if not session.get(k): session_blockers.append(k.replace('_',' ')+' missing')
    if status=='CLINICAL_TEST_UNLOCKED' and not session_blockers:
        caps['stimulus_dispatch']=True; status='CLINICAL_STIMULUS_DISPATCH_UNLOCKED'; reasons.append('patient session signed and within protocol limits')
else:
    blockers += session_blockers
if session and status!='CLINICAL_STIMULUS_DISPATCH_UNLOCKED': blockers += session_blockers

print('DNXR CLINICAL UNLOCK CHECK')
print(f'Status: {status}\n')
print('Capabilities:')
for k,v in caps.items(): print(f'- {k}: {yes(v)}')
print('\nCLINICAL UNLOCK STATUS')
print(f"Clinical protocol            : {'OK' if clinic and clinic.get('protocol',{}).get('signed') else 'Missing'}")
print(f"Emergency stop               : {'OK' if session and session.get('physical_emergency_stop_verified') and session.get('software_emergency_stop_verified') else 'Missing/Not active'}")
if clinic: print(f"Protocol: {clinic.get('protocol',{}).get('protocol_id','unknown')}\nSite: {clinic.get('clinical_site',{}).get('site_id','unknown')}")
if session: print(f"Session: {session.get('session_id','unknown')} subject_id: pseudonymized expires_at: {session.get('session_limits',{}).get('expires_at','unknown')}")
if reasons:
    print('\nReason:'); [print(f'- {r}') for r in reasons]
if blockers:
    print('\nBlockers:'); [print(f'- {b}') for b in blockers]
sys.exit(0 if status!='BLOCKED' else 2)
