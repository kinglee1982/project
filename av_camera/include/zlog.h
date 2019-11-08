#if 1
/*
 * This file is part of the zlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * The zlog Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The zlog Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the zlog Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __zlog_h
#define __zlog_h


typedef struct zlog_category_s zlog_category_t;
extern zlog_category_t *g_pZlog;

HI_S32 zlog_init(const HI_CHAR *confpath);
HI_S32 zlog_reload(const HI_CHAR *confpath);
HI_VOID zlog_fini(HI_VOID);

HI_VOID zlog_profile(HI_VOID);

zlog_category_t *zlog_get_category(const HI_CHAR *cname);

HI_S32 zlog_put_mdc(const HI_CHAR *key, const HI_CHAR *value);
HI_CHAR *zlog_get_mdc(const HI_CHAR *key);
HI_VOID zlog_remove_mdc(const HI_CHAR *key);
HI_VOID zlog_clean_mdc(HI_VOID);

HI_VOID zlog(zlog_category_t * category,
	const HI_CHAR *file, size_t filelen,
	const HI_CHAR *func, size_t funclen,
	long line, HI_S32 level,
	const HI_CHAR *format, ...);
HI_VOID vzlog(zlog_category_t * category,
	const HI_CHAR *file, size_t filelen,
	const HI_CHAR *func, size_t funclen,
	long line, HI_S32 level,
	const HI_CHAR *format, va_list args);
HI_VOID hzlog(zlog_category_t * category,
	const HI_CHAR *file, size_t filelen,
	const HI_CHAR *func, size_t funclen,
	long line, HI_S32 level,
	const HI_VOID *buf, size_t buflen);

HI_S32 dzlog_init(const HI_CHAR *confpath, const HI_CHAR *cname);
HI_S32 dzlog_set_category(const HI_CHAR *cname);

HI_VOID dzlog(const HI_CHAR *file, size_t filelen,
	const HI_CHAR *func, size_t funclen,
	long line, HI_S32 level,
	const HI_CHAR *format, ...);
HI_VOID vdzlog(const HI_CHAR *file, size_t filelen,
	const HI_CHAR *func, size_t funclen,
	long line, HI_S32 level,
	const HI_CHAR *format, va_list args);
HI_VOID hdzlog(const HI_CHAR *file, size_t filelen,
	const HI_CHAR *func, size_t funclen,
	long line, HI_S32 level,
	const HI_VOID *buf, size_t buflen);

typedef struct zlog_msg_s {
	HI_CHAR *buf;
	size_t len;
	HI_CHAR *path;
} zlog_msg_t;

typedef HI_S32 (*zlog_record_fn)(zlog_msg_t *msg);
HI_S32 zlog_set_record(const HI_CHAR *rname, zlog_record_fn record);

/******* useful macros, can be redefined at user's h file **********/

typedef enum {
	ZLOG_LEVEL_DEBUG = 20,
	ZLOG_LEVEL_INFO = 40,
	ZLOG_LEVEL_NOTICE = 60,
	ZLOG_LEVEL_WARN = 80,
	ZLOG_LEVEL_ERROR = 100,
	ZLOG_LEVEL_FATAL = 120
} zlog_level;

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
# if defined __GNUC__ && __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
/* zlog macros */
#define zlog_fatal(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define zlog_error(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define zlog_warn(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)
#define zlog_notice(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define zlog_info(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define zlog_debug(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)
/* dzlog macros */
#define dzlog_fatal(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define dzlog_error(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define dzlog_warn(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)
#define dzlog_notice(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define dzlog_info(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define dzlog_debug(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)
#elif defined __GNUC__
/* zlog macros */
#if 1
#define zlog_fatal(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define zlog_error(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#endif
//#define zlog_fatal(cat, format, args...)
//#define zlog_error(cat, format, args...)
#define zlog_warn(cat, format, args...)
#define zlog_notice(cat, format, args...)
#define zlog_info(cat, format, args...)
#define zlog_debug(cat, format, args...)

#if  0
#define zlog_fatal(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define zlog_error(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define zlog_warn(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)
#define zlog_notice(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define zlog_info(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define zlog_debug(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#endif /* #if 0 */
/* dzlog macros */

#define dzlog_fatal(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define dzlog_error(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define dzlog_warn(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)
#define dzlog_notice(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define dzlog_info(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define dzlog_debug(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#endif

/* vzlog macros */
#define vzlog_fatal(cat, format, args) \
	vzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, args)
#define vzlog_error(cat, format, args) \
	vzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, args)
#define vzlog_warn(cat, format, args) \
	vzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, args)
#define vzlog_notice(cat, format, args) \
	vzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, args)
#define vzlog_info(cat, format, args) \
	vzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, args)
#define vzlog_debug(cat, format, args) \
	vzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, args)

/* hzlog macros */
#define hzlog_fatal(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, buf, buf_len)
#define hzlog_error(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, buf, buf_len)
#define hzlog_warn(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, buf, buf_len)
#define hzlog_notice(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, buf, buf_len)
#define hzlog_info(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, buf, buf_len)
#define hzlog_debug(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, buf, buf_len)


/* vdzlog macros */
#define vdzlog_fatal(format, args) \
	vdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, args)
#define vdzlog_error(format, args) \
	vdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, args)
#define vdzlog_warn(format, args) \
	vdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, args)
#define vdzlog_notice(format, args) \
	vdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, args)
#define vdzlog_info(format, args) \
	vdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, args)
#define vdzlog_debug(format, args) \
	vdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, args)

/* hdzlog macros */
#define hdzlog_fatal(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, buf, buf_len)
#define hdzlog_error(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, buf, buf_len)
#define hdzlog_warn(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, buf, buf_len)
#define hdzlog_notice(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, buf, buf_len)
#define hdzlog_info(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, buf, buf_len)
#define hdzlog_debug(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, buf, buf_len)


#endif
#endif
