// Slimmer impl of wine.c for possible consumption on windows targets
// DLW: Brought over from wine.c impls (todo: setup preproc?)
#define COBJMACROS

#include <ntstatus.h>
#define WIN32_NO_STATUS

#include <oleauto.h>
#include <winternl.h>
#include <winuser.h>
#include <rpcproxy.h>
#include "wine/debug.h"

#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "vbscript.h"

#include <dirent.h>
#ifndef __MSYSWINE__
#include <fnmatch.h>
#else
typedef int WINBOOL;
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

#undef wcsncpy

HRESULT external_open_storage(const OLECHAR* pwcsName, IStorage* pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstgOpen);

HINSTANCE hProxyDll = 0;

const WCHAR* system_dir = L".";

const ExtendedProxyFileInfo oleaut32_oaidl_ProxyFileInfo = { 0 };

struct debug_info
{
   unsigned int str_pos;      /* current position in strings buffer */
   unsigned int out_pos;      /* current position in output buffer */
   char       strings[1020]; /* buffer for temporary strings */
   char       output[1020];  /* current output line */
} g_debug_info;

char dbg_buffer[4096] = { 0 };

int WINAPI __wine_dbg_write(const char *str, unsigned int len)
{
   size_t dest_len = strlen(dbg_buffer);
   size_t remaining_len = sizeof(dbg_buffer) - dest_len - 1;

   if (len < remaining_len)
      strncat(dbg_buffer, str, len);
   else {
      strncat(dbg_buffer, str, remaining_len);
      dbg_buffer[sizeof(dbg_buffer) - 1] = '\n';
   }

   char* end = strrchr(dbg_buffer, '\n');

   if (end) {
      *end = '\0';

#ifdef _DEBUG
      external_log_debug("%s", dbg_buffer);
#endif

      *dbg_buffer = '\0';
   }

   return 0;
}

static int append_output(struct debug_info *info, const char *str, size_t len)
{
   if (len >= sizeof(info->output) - info->out_pos) {
      __wine_dbg_write(info->output, info->out_pos);
      info->out_pos = 0;
      __wine_dbg_write(str, len);
   }
   memcpy(info->output + info->out_pos, str, len);
   info->out_pos += len;
   return len;
}

const char * __cdecl __wine_dbg_strdup(const char *str)
{
   struct debug_info *info = &g_debug_info;
   unsigned int pos = info->str_pos;
   size_t n = strlen(str) + 1;

   if (pos + n > sizeof(info->strings)) pos = 0;
   info->str_pos = pos + n;
   return memcpy(info->strings + pos, str, n);
}

int __cdecl __wine_dbg_output(const char *str)
{
   struct debug_info *info = &g_debug_info;
   const char *end = strrchr(str, '\n');
   int ret = 0;

   if (end) {
      ret += append_output(info, str, end + 1 - str);
      __wine_dbg_write(info->output, info->out_pos);
      info->out_pos = 0;
      str = end + 1;
   }
   if (*str) ret += append_output(info, str, strlen(str));
   return ret;
}

int __cdecl __wine_dbg_header(enum __wine_debug_class cls, struct __wine_debug_channel *channel,
                        const char *function)
{
   static const char * const classes[] = { "fixme", "err", "warn", "trace" };
   struct debug_info *info = &g_debug_info;
   char *pos = info->output;

   if (!(__wine_dbg_get_channel_flags(channel) & (1 << cls))) return -1;

   if (info->out_pos) return 0;

   if (function && cls < ARRAY_SIZE(classes))
      pos += snprintf(pos, sizeof(info->output) - (pos - info->output), "%s:%s:%s ",
                   classes[cls], channel->name, function);
   info->out_pos = pos - info->output;
   return info->out_pos;
}

unsigned char __cdecl __wine_dbg_get_channel_flags(struct __wine_debug_channel *channel)
{
   //if (!strcmp(channel->name, "vbscript"))
   //{
   //   return (1 << __WINE_DBCL_ERR) | (1 << __WINE_DBCL_TRACE) | (1 << __WINE_DBCL_FIXME);
   //}

   //if (!strcmp(channel->name, "variant"))
   //{
   //   return (1 << __WINE_DBCL_ERR) | (1 << __WINE_DBCL_TRACE) | (1 << __WINE_DBCL_FIXME);
   //}

   //if (!strcmp(channel->name, "scrrun"))
   //{
   //   return (1 << __WINE_DBCL_ERR) | (1 << __WINE_DBCL_TRACE) | (1 << __WINE_DBCL_FIXME);
   //}

   return 0;
}

// HACK: Need to figure out where this is actually defined, broken for now
void external_log_info(const char* format, ...)
{
   return;
}
