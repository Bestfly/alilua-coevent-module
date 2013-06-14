#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <time.h>
#include <math.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/ioctls.h>
#include <sys/un.h>
#include <inttypes.h>
#include <zlib.h>
#include <errno.h>
#include <openssl/sha.h>
#include <lua.h>
#include <lauxlib.h>

#define free(p) do { if (p) { free(p); p = NULL; } } while (0)
#define close(fd) do { if (fd >= 0) { close(fd); fd = -1; } } while (0)
//printf("closeat %d %s:%d\n",fd,__FILE__,__LINE__);

#ifndef _COTEST_H
	#define _COTEST_H
	
	#define large_malloc(s) (malloc(((int)(s/4096)+1)*4096))
	#define A_C_R     "\x1b[31m"
	#define A_C_G   "\x1b[32m"
	#define A_C_Y  "\x1b[33m"
	#define A_C_B    "\x1b[34m"
	#define A_C_M "\x1b[35m"
	#define A_C_C    "\x1b[36m"
	#define A_C__   "\x1b[0m"
	
	#define cr_printf(fmt, ...) printf("%s" fmt "%s", A_C_R, ##__VA_ARGS__, A_C__)
	#define cg_printf(fmt, ...) printf("%s" fmt "%s", A_C_G, ##__VA_ARGS__, A_C__)
	#define cy_printf(fmt, ...) printf("%s" fmt "%s", A_C_Y, ##__VA_ARGS__, A_C__)
	#define cb_printf(fmt, ...) printf("%s" fmt "%s", A_C_B, ##__VA_ARGS__, A_C__)
	#define cm_printf(fmt, ...) printf("%s" fmt "%s", A_C_M, ##__VA_ARGS__, A_C__)
	#define cc_printf(fmt, ...) printf("%s" fmt "%s", A_C_C, ##__VA_ARGS__, A_C__)
	
	time_t timer;

	typedef struct{
		lua_State *L;
		//int ref;
		void *next;
		char z[12]; /// size align
	} cosocket_swop_t;

	typedef struct{
		char *buf;
		void *next;
		int buf_size;
		int buf_len;
		char z[2]; /// size align
	} cosocket_link_buf_t;

	typedef struct{
		uint32_t	key1;
		uint32_t	key2;
		struct in_addr addr;
		int		recached;
		void *		next;
	} dns_cache_item_t;

	typedef struct{
		void *next;
		void *uper;
		int fd;
		int recached;
		unsigned long pool_key;
	} cosocket_connect_pool_t;

	typedef struct {
		int type;
		int fd;
		int status;
		void *ptr;
		lua_State *L;
		const u_char *send_buf;
		u_char _send_buf[3872];// with size align / 60
		size_t send_buf_len;
		size_t send_buf_ed;
		u_char *send_buf_need_free;
		
		int in_read_action;
		cosocket_link_buf_t *read_buf;
		cosocket_link_buf_t *last_buf;
		size_t total_buf_len;
		size_t buf_read_len;
		
		size_t readed;
		
		int timeout;
		int dns_tid;
		int dns_query_fd;
		char dns_query_name[60];// with size align / 60
		struct sockaddr_in addr;
		int pool_size;
		unsigned long pool_key;
		int reusedtimes;
		
		int ref;
	} cosocket_t;

	typedef struct{
		cosocket_t *cok;
		void *uper;
		void *next;
		long timeout;
	} timeout_link_t;

	typedef struct{
		uint16_t	tid;			/* Transaction ID */
		uint16_t	flags;			/* Flags */
		uint16_t	nqueries;		/* Questions */
		uint16_t	nanswers;		/* Answers */
		uint16_t	nauth;			/* Authority PRs */
		uint16_t	nother;			/* Other PRs */
		unsigned char	data[1];	/* Data, variable length */
	} dns_query_header_t;

	#define NULL32 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	int coevent_setnonblocking(int fd);
	int add_to_timeout_link(cosocket_t *cok, int timeout);
	int del_in_timeout_link(cosocket_t *cok);
	int lua_f_coroutine_resume_waiting(lua_State *L);
	int chk_do_timeout_link(int epoll_fd);
	void add_dns_cache(const char *name, struct in_addr addr, int do_recache);
	int do_dns_query(int epoll_fd, cosocket_t *cok, const char *name);
	void parse_dns_result(int epoll_fd, int fd, cosocket_t *cok, const unsigned char *pkt, int len);
	long longtime();
	int lua_f_time(lua_State *L);
	int lua_f_longtime(lua_State *L);
	int tcp_connect(const char *host, int port, cosocket_t *cok, int epoll_fd, int *ret);
	size_t lua_calc_strlen_in_table(lua_State *L, int index, int arg_i, unsigned strict);
	unsigned char *lua_copy_str_in_table(lua_State *L, int index, u_char *dst);
	int lua_f_sha1bin(lua_State *L);
	int lua_f_base64encode(lua_State *L);
	int lua_f_base64decode(lua_State *L);
	int lua_f_escape_uri(lua_State *L);
	int lua_f_unescape_uri(lua_State *L);
#endif
