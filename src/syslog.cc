#include <errno.h>
#include <string.h>
#include <syslog.h>

#include <napi.h>

using namespace Napi;

// -------------------------
// Helper Macros

#define THROW_IF_ARGS_EMPTY(info) \
  if (info.Length() == 0) { \
    Napi::TypeError::New(info.Env(), "Missing arguments").ThrowAsJavaScriptException(); \
    return info.Env().Undefined(); \
  }

#define REQUIRE_INT_ARG(info, I, VAR) \
  if (info.Length() <= (I) || !info[I].IsNumber()) { \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be an integer").ThrowAsJavaScriptException(); \
    return info.Env().Undefined(); \
  } \
  int VAR = info[I].As<Number>().Int32Value();

#define REQUIRE_STRING_ARG(info, I, VAR) \
  if (info.Length() <= (I) || !info[I].IsString()) { \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be a string").ThrowAsJavaScriptException(); \
    return info.Env().Undefined(); \
  } \
  std::string VAR = info[I].As<String>().Utf8Value();


// -------------------------
// N-API bindings

Value Open(const CallbackInfo& info) {
  Env env = info.Env();
  THROW_IF_ARGS_EMPTY(info);

  REQUIRE_STRING_ARG(info, 0, ident);
  REQUIRE_INT_ARG(info, 1, logopt);
  REQUIRE_INT_ARG(info, 2, facility);

  openlog(strdup(ident.c_str()), logopt, facility);
  return env.Undefined();
}

Value Log(const CallbackInfo& info) {
  Env env = info.Env();
  THROW_IF_ARGS_EMPTY(info);

  REQUIRE_INT_ARG(info, 0, priority);
  REQUIRE_STRING_ARG(info, 1, message);

  syslog(priority, "%s", message.c_str());
  return env.Undefined();
}

Value Close(const CallbackInfo& info) {
  Env env = info.Env();
  closelog();
  return env.Undefined();
}

Value Mask(const CallbackInfo& info) {
  Env env = info.Env();
  REQUIRE_INT_ARG(info, 0, maskpri);

  int mask = setlogmask(LOG_UPTO(maskpri));
  return Number::New(env, mask);
}

// -------------------------
// Module initialization

Object Init(Env env, Object exports) {
  exports.Set("openlog", Function::New(env, Open));
  exports.Set("syslog", Function::New(env, Log));
  exports.Set("closelog", Function::New(env, Close));
  exports.Set("setlogmask", Function::New(env, Mask));

  // Priorities
  exports.Set("LOG_EMERG", Number::New(env, LOG_EMERG));
  exports.Set("LOG_ALERT", Number::New(env, LOG_ALERT));
  exports.Set("LOG_ERR", Number::New(env, LOG_ERR));
  exports.Set("LOG_WARNING", Number::New(env, LOG_WARNING));
  exports.Set("LOG_NOTICE", Number::New(env, LOG_NOTICE));
  exports.Set("LOG_INFO", Number::New(env, LOG_INFO));
  exports.Set("LOG_DEBUG", Number::New(env, LOG_DEBUG));

  // Facilities
  exports.Set("LOG_KERN", Number::New(env, LOG_KERN));
  exports.Set("LOG_USER", Number::New(env, LOG_USER));
  exports.Set("LOG_MAIL", Number::New(env, LOG_MAIL));
  exports.Set("LOG_DAEMON", Number::New(env, LOG_DAEMON));
  exports.Set("LOG_AUTH", Number::New(env, LOG_AUTH));
  exports.Set("LOG_LPR", Number::New(env, LOG_LPR));
  exports.Set("LOG_NEWS", Number::New(env, LOG_NEWS));
  exports.Set("LOG_UUCP", Number::New(env, LOG_UUCP));
  exports.Set("LOG_CRON", Number::New(env, LOG_CRON));
  exports.Set("LOG_LOCAL0", Number::New(env, LOG_LOCAL0));
  exports.Set("LOG_LOCAL1", Number::New(env, LOG_LOCAL1));
  exports.Set("LOG_LOCAL2", Number::New(env, LOG_LOCAL2));
  exports.Set("LOG_LOCAL3", Number::New(env, LOG_LOCAL3));
  exports.Set("LOG_LOCAL4", Number::New(env, LOG_LOCAL4));
  exports.Set("LOG_LOCAL5", Number::New(env, LOG_LOCAL5));
  exports.Set("LOG_LOCAL6", Number::New(env, LOG_LOCAL6));
  exports.Set("LOG_LOCAL7", Number::New(env, LOG_LOCAL7));

  // Options
  exports.Set("LOG_PID", Number::New(env, LOG_PID));
  exports.Set("LOG_CONS", Number::New(env, LOG_CONS));
  exports.Set("LOG_NDELAY", Number::New(env, LOG_NDELAY));

  return exports;
}

NODE_API_MODULE(syslog, Init)
