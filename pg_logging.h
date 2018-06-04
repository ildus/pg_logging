#ifndef PG_LOGGING_H
#define PG_LOGGING_H

#include "postgres.h"
#include "pg_config.h"
#include "port/atomics.h"
#include "storage/lwlock.h"

#define CHECK_DATA

/* CollectedItem contains offsets in saved block */
typedef struct CollectedItem
{
#ifdef CHECK_DATA
	int			magic;
#endif
	int			totallen;		/* size of this block */
	int			saved_errno;			/* errno at entry */
	char		elevel;			/* error level */

	/* text offsets in data block */
	int			message_len;
	int			detail_len;			/* detail error message */
	int			hint_len;			/* hint message */

	/* texts are contained here */
	char		data[FLEXIBLE_ARRAY_MEMBER];
} CollectedItem;

#define ITEM_HDR_LEN (offsetof(CollectedItem, data))

typedef struct LoggingShmemHdr
{
	char			   *data;
	pg_atomic_uint32	endpos;
	volatile uint32		readpos;
	int					buffer_size;	/* total size of buffer */
	LWLock				hdr_lock;
	bool				wraparound;
} LoggingShmemHdr;

struct ErrorLevel {
	char   *text;
	int		code;
};

#define	PG_LOGGING_MAGIC	0xAABBCCDD
#define	PG_ITEM_MAGIC		0x06054AB5

// views
#define Natts_pg_logging_data		6
#define Anum_pg_logging_level		1
#define Anum_pg_logging_errno		2
#define Anum_pg_logging_message		3
#define Anum_pg_logging_detail		4
#define Anum_pg_logging_hint		5
#define Anum_pg_logging_position	6

extern struct ErrorLevel errlevel_wordlist[];
extern LoggingShmemHdr	*hdr;

void reset_counters_in_shmem(void);
struct ErrorLevel *get_errlevel (register const char *str, register size_t len);

#endif
