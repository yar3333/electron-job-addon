#include <napi.h>
#include <windows.h>
#include "JobAddon.h"

// ============================================================================
// JobWrapper class for wrapping JobObject in N-API
// ============================================================================

class JobWrapper : public Napi::ObjectWrap<JobWrapper>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  JobWrapper(const Napi::CallbackInfo& info);
  ~JobWrapper();

private:
  JobObject* job_;

  // Methods accessible from JavaScript
  Napi::Value Create(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value IsValid(const Napi::CallbackInfo& info);
  Napi::Value AddProcess(const Napi::CallbackInfo& info);
  Napi::Value AddProcessById(const Napi::CallbackInfo& info);
  Napi::Value SetKillOnJobClose(const Napi::CallbackInfo& info);
  Napi::Value SetSilentBreakaway(const Napi::CallbackInfo& info);
  Napi::Value SetLimitFlags(const Napi::CallbackInfo& info);
};

// ============================================================================
// JobWrapper class initialization
// ============================================================================

Napi::Object JobWrapper::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Job", {
      InstanceMethod("create", &JobWrapper::Create),
      InstanceMethod("close", &JobWrapper::Close),
      InstanceMethod("isValid", &JobWrapper::IsValid),
      InstanceMethod("addProcess", &JobWrapper::AddProcess),
      InstanceMethod("addProcessById", &JobWrapper::AddProcessById),
      InstanceMethod("setKillOnJobClose", &JobWrapper::SetKillOnJobClose),
      InstanceMethod("setSilentBreakaway", &JobWrapper::SetSilentBreakaway),
      InstanceMethod("setLimitFlags", &JobWrapper::SetLimitFlags),
    });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("Job", func);
  return exports;
}

JobWrapper::JobWrapper(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<JobWrapper>(info), job_(nullptr)
{
  // Constructor does not create Job automatically
  // Need to explicitly call create()
}

JobWrapper::~JobWrapper()
{
  if (job_ != nullptr)
  {
    job_addon::DestroyJob(job_);
    job_ = nullptr;
  }
}

// -----------------------------------------------------------------------------
// create([name]) method - creates Job Object
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::Create(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ != nullptr)
  {
    // Already created
    return Napi::Boolean::New(env, true);
  }

  std::wstring name;
  if (info.Length() > 0 && info[0].IsString())
  {
    Napi::String nameStr = info[0].As<Napi::String>();
    std::u16string nameUtf16 = nameStr.Utf16Value();
    name = std::wstring(reinterpret_cast<const wchar_t*>(nameUtf16.c_str()), nameUtf16.length());
  }

  job_ = job_addon::CreateJob();
  if (job_ == nullptr)
  {
    Napi::Error::New(env, "Failed to create Job Object. Error: " +
      std::to_string(GetLastError())).ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  return Napi::Boolean::New(env, true);
}

// -----------------------------------------------------------------------------
// close() method - closes Job Object
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::Close(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr)
  {
    return Napi::Boolean::New(env, false);
  }

  bool result = job_->Close();
  job_addon::DestroyJob(job_);
  job_ = nullptr;

  return Napi::Boolean::New(env, result);
}

// -----------------------------------------------------------------------------
// isValid() method - checks if Job Object is valid
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::IsValid(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr)
  {
    return Napi::Boolean::New(env, false);
  }

  return Napi::Boolean::New(env, job_->IsValid());
}

// -----------------------------------------------------------------------------
// addProcess(handle) method - adds process by HANDLE
// handle can be a number (PID) or Buffer with handle
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::AddProcess(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr || !job_->IsValid())
  {
    Napi::Error::New(env, "Job Object is not initialized").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "Expected process handle or PID").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  HANDLE processHandle = nullptr;

  if (info[0].IsNumber())
  {
    // If PID is passed, use addProcessById
    DWORD pid = info[0].As<Napi::Number>().Uint32Value();
    bool result = job_->AddProcessById(pid);
    return Napi::Boolean::New(env, result);
  }
  else if (info[0].IsBuffer())
  {
    // If Buffer with handle is passed
    Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();
    if (buffer.Length() >= sizeof(HANDLE))
    {
      processHandle = *reinterpret_cast<HANDLE*>(buffer.Data());
    }
    else
    {
      Napi::TypeError::New(env, "Invalid buffer size for handle").ThrowAsJavaScriptException();
      return Napi::Boolean::New(env, false);
    }
  }
  else if (info[0].IsExternal())
  {
    // If External with handle is passed
    Napi::External<void> external = info[0].As<Napi::External<void>>();
    processHandle = reinterpret_cast<HANDLE>(external.Data());
  }
  else
  {
    Napi::TypeError::New(env, "Expected number (PID), Buffer, or External for process handle").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  bool result = job_->AddProcess(processHandle);
  if (!result)
  {
    Napi::Error::New(env, "Failed to add process to Job. Error: " +
      std::to_string(GetLastError())).ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(env, result);
}

// -----------------------------------------------------------------------------
// addProcessById(pid) method - adds process by PID
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::AddProcessById(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr || !job_->IsValid())
  {
    Napi::Error::New(env, "Job Object is not initialized").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "Expected process ID (number)").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  DWORD pid = info[0].As<Napi::Number>().Uint32Value();
  bool result = job_->AddProcessById(pid);

  if (!result)
  {
    Napi::Error::New(env, "Failed to add process to Job. Error: " +
      std::to_string(GetLastError())).ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(env, result);
}

// -----------------------------------------------------------------------------
// setKillOnJobClose(enable) method - sets kill on job close flag
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::SetKillOnJobClose(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr || !job_->IsValid())
  {
    Napi::Error::New(env, "Job Object is not initialized").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (info.Length() < 1 || !info[0].IsBoolean())
  {
    Napi::TypeError::New(env, "Expected boolean value").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  bool enable = info[0].As<Napi::Boolean>().Value();
  bool result = job_->SetKillOnJobClose(enable);

  if (!result)
  {
    Napi::Error::New(env, "Failed to set KillOnJobClose flag. Error: " +
      std::to_string(GetLastError())).ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(env, result);
}

// -----------------------------------------------------------------------------
// setSilentBreakaway(enable) method - sets silent breakaway flag
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::SetSilentBreakaway(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr || !job_->IsValid())
  {
    Napi::Error::New(env, "Job Object is not initialized").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (info.Length() < 1 || !info[0].IsBoolean())
  {
    Napi::TypeError::New(env, "Expected boolean value").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  bool enable = info[0].As<Napi::Boolean>().Value();
  bool result = job_->SetSilentBreakaway(enable);

  if (!result)
  {
    Napi::Error::New(env, "Failed to set SilentBreakaway flag. Error: " +
      std::to_string(GetLastError())).ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(env, result);
}

// -----------------------------------------------------------------------------
// setLimitFlags(flags) method - sets limit flags
// -----------------------------------------------------------------------------
Napi::Value JobWrapper::SetLimitFlags(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  if (job_ == nullptr || !job_->IsValid())
  {
    Napi::Error::New(env, "Job Object is not initialized").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (info.Length() < 1 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "Expected flags value (number)").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  DWORD flags = info[0].As<Napi::Number>().Uint32Value();
  bool result = job_->SetLimitFlags(flags);

  if (!result)
  {
    Napi::Error::New(env, "Failed to set limit flags. Error: " +
      std::to_string(GetLastError())).ThrowAsJavaScriptException();
  }

  return Napi::Boolean::New(env, result);
}

// ============================================================================
// Module initialization
// ============================================================================

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  // Initialize Job class
  JobWrapper::Init(env, exports);

  // Add flag constants
  exports.Set("JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE)));
  exports.Set("JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK)));
  exports.Set("JOB_OBJECT_LIMIT_BREAKAWAY_OK",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_BREAKAWAY_OK)));
  exports.Set("JOB_OBJECT_LIMIT_PROCESS_MEMORY",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_PROCESS_MEMORY)));
  exports.Set("JOB_OBJECT_LIMIT_JOB_MEMORY",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_JOB_MEMORY)));
  exports.Set("JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION)));
  exports.Set("JOB_OBJECT_LIMIT_ACTIVE_PROCESS",
    Napi::Number::New(env, static_cast<uint32_t>(JOB_OBJECT_LIMIT_ACTIVE_PROCESS)));

  return exports;
}

NODE_API_MODULE(job_addon, Init)
