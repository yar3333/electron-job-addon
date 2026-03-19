#pragma once

#include <windows.h>
#include <string>

// Structures are already defined in Windows SDK (winnt.h)
// Use them directly

enum class JobObjectInfoType
{
  AssociateCompletionPortInformation = 7,
  BasicLimitInformation = 2,
  BasicUIRestrictions = 4,
  EndOfJobTimeInformation = 6,
  ExtendedLimitInformation = 9,
  SecurityLimitInformation = 5,
  GroupInformation = 11
};

// Limit flags (JOB_OBJECT_LIMIT_*)
enum class JobLimitFlags : DWORD
{
  None = 0x00000000,
  KillOnJobClose = 0x00000001,
  SilentBreakaway = 0x00000002,
  SchedulingClass = 0x00000004,
  CpuRateControlByCap = 0x00000008,
  Affinity = 0x00000010,
  PriorityClass = 0x00000020,
  WorkingSetSize = 0x00000040,
  ProcessMemory = 0x00000100,
  JobMemory = 0x00000200,
  DieOnUnhandledException = 0x00000400,
  BreakawayOnJobClose = 0x00000800,
  PostJobInformation = 0x00001000,
  ActiveProcess = 0x00002000,
  PerProcessUserTime = 0x00004000,
  EndOfJobTime = 0x00008000,
  KillOnJobCloseAll = 0x00010000,
  UiRestrictionsClass = 0x00020000,
  SecurityLimitInformation = 0x00040000,
  GroupAffinity = 0x00080000,
  CpuRateControlByWeight = 0x00100000,
  CpuRateControlByEffectiveCpu = 0x00200000,
  CpuRateControlByTime = 0x00400000,
  CpuRateControlByCycleTime = 0x00800000,
  CpuRateControlByPaceOutput = 0x01000000,
  NetRateControlByCap = 0x02000000,
  NetRateControlByEffectiveCpu = 0x04000000,
  NetRateControlByTime = 0x08000000,
  IoRateControlByCap = 0x10000000,
  IoRateControlByEffectiveCpu = 0x20000000,
  IoRateControlByTime = 0x40000000,
  IoRateControlByIops = 0x80000000
};

// Class for managing Job Object
class JobObject
{
public:
  JobObject();
  ~JobObject();

  bool Create(const std::wstring& name = L"");
  bool Close();
  bool IsValid() const;

  bool AddProcess(HANDLE processHandle);
  bool AddProcessById(DWORD processId);

  bool SetExtendedLimitInformation(const JOBOBJECT_EXTENDED_LIMIT_INFORMATION& info);
  bool SetLimitFlags(DWORD limitFlags);
  bool SetKillOnJobClose(bool enable);
  bool SetSilentBreakaway(bool enable);

  HANDLE GetHandle() const { return handle_; }

private:
  HANDLE handle_;
  bool disposed_;

  void Dispose();
};

// Factory functions for creating Job Object
namespace job_addon
{
  JobObject* CreateJob();
  void DestroyJob(JobObject* job);
}
