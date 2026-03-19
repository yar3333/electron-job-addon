#include "JobAddon.h"
#include <stdexcept>

// ============================================================================
// JobObject class implementation
// ============================================================================

JobObject::JobObject() : handle_(INVALID_HANDLE_VALUE), disposed_(false)
{
}

JobObject::~JobObject()
{
  Dispose();
}

bool JobObject::Create(const std::wstring& name)
{
  if (handle_ != INVALID_HANDLE_VALUE)
  {
    Close();
  }

  // Create Job Object
  // First parameter - security attributes (nullptr = default)
  // Second parameter - object name (nullptr = unnamed)
  handle_ = CreateJobObjectW(nullptr, name.empty() ? nullptr : name.c_str());

  if (handle_ == nullptr || handle_ == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  return true;
}

bool JobObject::Close()
{
  if (handle_ != INVALID_HANDLE_VALUE && handle_ != nullptr)
  {
    BOOL result = CloseHandle(handle_);
    handle_ = INVALID_HANDLE_VALUE;
    return result != FALSE;
  }
  return true;
}

bool JobObject::IsValid() const
{
  return handle_ != INVALID_HANDLE_VALUE && handle_ != nullptr;
}

bool JobObject::AddProcess(HANDLE processHandle)
{
  if (!IsValid())
  {
    return false;
  }

  // AssignProcessToJobObject adds a process to Job Object
  // Returns TRUE on success, FALSE on error
  BOOL result = AssignProcessToJobObject(handle_, processHandle);
  return result != FALSE;
}

bool JobObject::AddProcessById(DWORD processId)
{
  if (!IsValid())
  {
    return false;
  }

  // Open process with rights to add to Job
  HANDLE processHandle = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, processId);
  if (processHandle == nullptr)
  {
    return false;
  }

  bool result = AddProcess(processHandle);
  CloseHandle(processHandle);
  return result;
}

bool JobObject::SetExtendedLimitInformation(const JOBOBJECT_EXTENDED_LIMIT_INFORMATION& info)
{
  if (!IsValid())
  {
    return false;
  }

  // SetInformationJobObject sets extended limit information
  BOOL result = SetInformationJobObject(
    handle_,
    static_cast<JOBOBJECTINFOCLASS>(JobObjectInfoType::ExtendedLimitInformation),
    const_cast<JOBOBJECT_EXTENDED_LIMIT_INFORMATION*>(&info),
    sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION)
  );

  return result != FALSE;
}

bool JobObject::SetLimitFlags(DWORD limitFlags)
{
  if (!IsValid())
  {
    return false;
  }

  // Get current information
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};
  DWORD returnLength = 0;

  BOOL getResult = QueryInformationJobObject(
    handle_,
    static_cast<JOBOBJECTINFOCLASS>(JobObjectInfoType::ExtendedLimitInformation),
    &info,
    sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION),
    &returnLength
  );

  if (!getResult)
  {
    return false;
  }

  // Set new flags
  info.BasicLimitInformation.LimitFlags = limitFlags;

  return SetExtendedLimitInformation(info);
}

bool JobObject::SetKillOnJobClose(bool enable)
{
  DWORD currentFlags = 0;

  // Get current flags
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};
  DWORD returnLength = 0;

  BOOL getResult = QueryInformationJobObject(
    handle_,
    static_cast<JOBOBJECTINFOCLASS>(JobObjectInfoType::ExtendedLimitInformation),
    &info,
    sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION),
    &returnLength
  );

  if (!getResult)
  {
    return false;
  }

  currentFlags = info.BasicLimitInformation.LimitFlags;

  // Set or clear JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE flag
  if (enable)
  {
    currentFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  }
  else
  {
    currentFlags &= ~JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  }

  return SetLimitFlags(currentFlags);
}

bool JobObject::SetSilentBreakaway(bool enable)
{
  DWORD currentFlags = 0;

  JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};
  DWORD returnLength = 0;

  BOOL getResult = QueryInformationJobObject(
    handle_,
    static_cast<JOBOBJECTINFOCLASS>(JobObjectInfoType::ExtendedLimitInformation),
    &info,
    sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION),
    &returnLength
  );

  if (!getResult)
  {
    return false;
  }

  currentFlags = info.BasicLimitInformation.LimitFlags;

  // Set or clear JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK flag
  if (enable)
  {
    currentFlags |= JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
  }
  else
  {
    currentFlags &= ~JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
  }

  return SetLimitFlags(currentFlags);
}

void JobObject::Dispose()
{
  if (disposed_)
  {
    return;
  }

  Close();
  disposed_ = true;
}

// ============================================================================
// Factory functions
// ============================================================================

namespace job_addon
{
  JobObject* CreateJob()
  {
    JobObject* job = new JobObject();
    if (!job->Create())
    {
      delete job;
      return nullptr;
    }
    return job;
  }

  void DestroyJob(JobObject* job)
  {
    if (job != nullptr)
    {
      delete job;
    }
  }
}
