/* The Win32 file calls the FractumSeraph/OpenCE fork's game units make
 * (its recordings, network_recording.c; its map families, map_families.c),
 * which its desktop builds take from port/linux/src/xbox_files.c: here over
 * the C library, on the SD card. A path's backslashes are slashes; a
 * search's pattern is its last part's * and ? (as Win32's). Only files and
 * searches are handles here (CloseHandle closes either). */
/* (a native unit, over newlib: the SDK's names and types written out here,
 * the same on the 3DS's 32-bit ARM, where __stdcall is nothing; the find
 * data is the SDK's struct _WIN32_FIND_DATAA, port/n3ds/xdk/xdk_pdb.h) */
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
typedef void *HANDLE;
typedef const char *LPCSTR;
typedef void *LPVOID;
typedef const void *LPCVOID;
typedef DWORD *LPDWORD;
typedef LONG *PLONG;
typedef void *LPSECURITY_ATTRIBUTES;
typedef void *LPOVERLAPPED;
#define WINAPI
#define TRUE 1
#define FALSE 0
#define GENERIC_READ 0x80000000UL
#define GENERIC_WRITE 0x40000000UL
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG)-1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)

struct n3ds_win32_filetime { DWORD low, high; };
typedef struct n3ds_win32_find_data
{
	DWORD dwFileAttributes;
	struct n3ds_win32_filetime ftCreationTime;
	struct n3ds_win32_filetime ftLastAccessTime;
	struct n3ds_win32_filetime ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	char cFileName[260];
	char cAlternateFileName[14];
} *LPWIN32_FIND_DATAA;
_Static_assert(sizeof(struct n3ds_win32_find_data) == 320, "the SDK's WIN32_FIND_DATAA");

#define N3DS_WIN32_FILE 0x46494C45UL /* 'FILE' */
#define N3DS_WIN32_FIND 0x46494E44UL /* 'FIND' */

struct n3ds_win32_handle
{
	unsigned long kind;
	FILE *file;
	DIR *folder;
	char folder_path[260];
	char pattern[128];
};

static void n3ds_win32_path(const char *in, char *out, size_t size)
{
	size_t i;

	for (i = 0; in && in[i] && i + 1 < size; i++)
		out[i] = in[i] == '\\' ? '/' : in[i];
	out[i] = 0;
}

static int n3ds_win32_match(const char *pattern, const char *name)
{
	for (; *pattern; pattern++, name++)
	{
		if (*pattern == '*')
		{
			while (*pattern == '*')
				pattern++;
			if (!*pattern)
				return 1;
			for (; *name; name++)
				if (n3ds_win32_match(pattern, name))
					return 1;
			return 0;
		}
		if (!*name)
			return 0;
		if (*pattern != '?')
		{
			char a = *pattern >= 'A' && *pattern <= 'Z' ? *pattern - 'A' + 'a' : *pattern;
			char b = *name >= 'A' && *name <= 'Z' ? *name - 'A' + 'a' : *name;

			if (a != b)
				return 0;
		}
	}
	return !*name;
}

HANDLE WINAPI CreateFileA(LPCSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES attributes,
	DWORD disposition, DWORD flags, HANDLE template_file)
{
	char path[260];
	struct stat status;
	int exists;
	const char *mode;
	struct n3ds_win32_handle *handle;
	FILE *file;

	(void)share;
	(void)attributes;
	(void)flags;
	(void)template_file;
	n3ds_win32_path(name, path, sizeof(path));
	exists = stat(path, &status) == 0;
	if (exists && S_ISDIR(status.st_mode))
		return INVALID_HANDLE_VALUE;
	switch (disposition)
	{
	case CREATE_NEW:
		if (exists)
			return INVALID_HANDLE_VALUE;
		mode = (access & GENERIC_READ) ? "w+b" : "wb";
		break;
	case CREATE_ALWAYS:
		mode = (access & GENERIC_READ) ? "w+b" : "wb";
		break;
	case OPEN_EXISTING:
		if (!exists)
			return INVALID_HANDLE_VALUE;
		mode = (access & GENERIC_WRITE) ? "r+b" : "rb";
		break;
	case OPEN_ALWAYS:
		mode = exists ? ((access & GENERIC_WRITE) ? "r+b" : "rb") : "w+b";
		break;
	case TRUNCATE_EXISTING:
		if (!exists)
			return INVALID_HANDLE_VALUE;
		mode = "w+b";
		break;
	default:
		return INVALID_HANDLE_VALUE;
	}
	file = fopen(path, mode);
	if (!file)
		return INVALID_HANDLE_VALUE;
	handle = calloc(1, sizeof(*handle));
	if (!handle)
	{
		fclose(file);
		return INVALID_HANDLE_VALUE;
	}
	handle->kind = N3DS_WIN32_FILE;
	handle->file = file;
	return (HANDLE)handle;
}

static struct n3ds_win32_handle *n3ds_win32_file(HANDLE handle)
{
	struct n3ds_win32_handle *h = (struct n3ds_win32_handle *)handle;

	return h && handle != INVALID_HANDLE_VALUE && h->kind == N3DS_WIN32_FILE ? h : NULL;
}

BOOL WINAPI ReadFile(HANDLE handle, LPVOID buffer, DWORD size, LPDWORD read, LPOVERLAPPED overlapped)
{
	struct n3ds_win32_handle *h = n3ds_win32_file(handle);
	size_t count;

	(void)overlapped;
	if (read)
		*read = 0;
	if (!h)
		return FALSE;
	count = fread(buffer, 1, size, h->file);
	if (read)
		*read = (DWORD)count;
	return count == size || !ferror(h->file);
}

BOOL WINAPI WriteFile(HANDLE handle, LPCVOID buffer, DWORD size, LPDWORD written, LPOVERLAPPED overlapped)
{
	struct n3ds_win32_handle *h = n3ds_win32_file(handle);
	size_t count;

	(void)overlapped;
	if (written)
		*written = 0;
	if (!h)
		return FALSE;
	count = fwrite(buffer, 1, size, h->file);
	if (written)
		*written = (DWORD)count;
	return count == size;
}

DWORD WINAPI SetFilePointer(HANDLE handle, LONG distance, PLONG distance_high, DWORD method)
{
	struct n3ds_win32_handle *h = n3ds_win32_file(handle);
	int whence = method == FILE_END ? SEEK_END : method == FILE_CURRENT ? SEEK_CUR : SEEK_SET;
	long position;

	if (distance_high)
		*distance_high = 0;
	if (!h || fseek(h->file, distance, whence) != 0 || (position = ftell(h->file)) < 0)
		return INVALID_SET_FILE_POINTER;
	return (DWORD)position;
}

DWORD WINAPI GetFileSize(HANDLE handle, LPDWORD size_high)
{
	struct n3ds_win32_handle *h = n3ds_win32_file(handle);
	long here, end;

	if (size_high)
		*size_high = 0;
	if (!h || (here = ftell(h->file)) < 0 || fseek(h->file, 0, SEEK_END) != 0)
		return INVALID_FILE_SIZE;
	end = ftell(h->file);
	fseek(h->file, here, SEEK_SET);
	return end < 0 ? INVALID_FILE_SIZE : (DWORD)end;
}

static BOOL n3ds_win32_find_fill(struct n3ds_win32_handle *h, LPWIN32_FIND_DATAA data)
{
	struct dirent *entry;

	while ((entry = readdir(h->folder)) != NULL)
	{
		char path[400];
		struct stat status;

		if (!n3ds_win32_match(h->pattern, entry->d_name))
			continue;
		memset(data, 0, sizeof(*data));
		snprintf(data->cFileName, sizeof(data->cFileName), "%s", entry->d_name);
		snprintf(path, sizeof(path), "%s/%s", h->folder_path, entry->d_name);
		if (stat(path, &status) == 0)
		{
			data->dwFileAttributes = S_ISDIR(status.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
			data->nFileSizeLow = S_ISDIR(status.st_mode) ? 0 : (DWORD)status.st_size;
		}
		return TRUE;
	}
	return FALSE;
}

HANDLE WINAPI FindFirstFileA(LPCSTR pattern, LPWIN32_FIND_DATAA data)
{
	char path[260];
	char *slash;
	struct n3ds_win32_handle *h;

	n3ds_win32_path(pattern, path, sizeof(path));
	h = calloc(1, sizeof(*h));
	if (!h || !data)
	{
		free(h);
		return INVALID_HANDLE_VALUE;
	}
	slash = strrchr(path, '/');
	if (slash)
	{
		snprintf(h->pattern, sizeof(h->pattern), "%s", slash + 1);
		*slash = 0;
		snprintf(h->folder_path, sizeof(h->folder_path), "%s", path[0] ? path : "/");
	}
	else
	{
		snprintf(h->pattern, sizeof(h->pattern), "%s", path);
		snprintf(h->folder_path, sizeof(h->folder_path), ".");
	}
	h->kind = N3DS_WIN32_FIND;
	h->folder = opendir(h->folder_path);
	if (!h->folder || !n3ds_win32_find_fill(h, data))
	{
		if (h->folder)
			closedir(h->folder);
		free(h);
		return INVALID_HANDLE_VALUE;
	}
	return (HANDLE)h;
}

BOOL WINAPI FindNextFileA(HANDLE handle, LPWIN32_FIND_DATAA data)
{
	struct n3ds_win32_handle *h = (struct n3ds_win32_handle *)handle;

	if (!h || handle == INVALID_HANDLE_VALUE || h->kind != N3DS_WIN32_FIND || !data)
		return FALSE;
	return n3ds_win32_find_fill(h, data);
}

BOOL WINAPI CloseHandle(HANDLE handle)
{
	struct n3ds_win32_handle *h = (struct n3ds_win32_handle *)handle;

	if (!h || handle == INVALID_HANDLE_VALUE)
		return FALSE;
	if (h->kind == N3DS_WIN32_FILE)
		fclose(h->file);
	else if (h->kind == N3DS_WIN32_FIND)
		closedir(h->folder);
	else
		return FALSE;
	h->kind = 0;
	free(h);
	return TRUE;
}

BOOL WINAPI CreateDirectoryA(LPCSTR name, LPSECURITY_ATTRIBUTES attributes)
{
	char path[260];

	(void)attributes;
	n3ds_win32_path(name, path, sizeof(path));
	return mkdir(path, 0777) == 0;
}

BOOL WINAPI DeleteFileA(LPCSTR name)
{
	char path[260];

	n3ds_win32_path(name, path, sizeof(path));
	return remove(path) == 0;
}

BOOL WINAPI MoveFileA(LPCSTR from, LPCSTR to)
{
	char a[260], b[260];

	n3ds_win32_path(from, a, sizeof(a));
	n3ds_win32_path(to, b, sizeof(b));
	return rename(a, b) == 0;
}
