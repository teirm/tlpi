/*
 * log_dir.c
 *
 * A program to log directories using inotify.
 */

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ftw.h>
#include <limits.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/inotify.h>

#define BUF_LEN             1024 
#define INOTIFY_TRACK_MAX   100

#define LOG_DIR_DEBUG()       1

static char *concat_path(char *dirname, char *basename);
static char *find_watched_path(int wd);
static int track_dir(const char *fpath, const struct stat *sb, int flag, struct FTW *ftwbuf);
static int end_watch(char *fpath);
static int start_watch(char *fpath);

static void log_dir();
static void parse_event(struct inotify_event *i);

static int handle_create_dir(struct inotify_event *i);
static int handle_delete_dir(struct inotify_event *i);

static int watch_descriptors[INOTIFY_TRACK_MAX];
static char *watch_paths[INOTIFY_TRACK_MAX];

struct log_node_s {
    int wd;
    char path[PATH_MAX];
    struct log_node_s *next;
};
typedef struct log_node_s log_node_t;

struct log_list_s {
    int length;
    log_node_t *head;
};
typedef struct log_list_s log_list_t;

static int inotify_fd;
static int total_watched; 
static log_list_t *watch_list;

static log_list_t*
watch_list_init()
{
    assert(watch_list == NULL);
    
    watch_list = malloc(sizeof(log_list_t));
    if (watch_list == NULL) {
        return NULL;
    }

    watch_list->head = NULL;
    watch_list->length = 0;

    return watch_list;
}

static void
watch_list_destroy()
{
    log_node_t *curr_node = NULL; 
    assert(watch_list != NULL);
    
    curr_node = watch_list->head;
    while (curr_node != NULL) {
        watch_list->head = curr_node->next;
        free(curr_node);
        curr_node = watch_list->head;
        watch_list->length--;
    }
    
    assert(watch_list->length == 0);

    free(watch_list);
    watch_list = NULL;
    return;
}

static int
log_list_add(int wd, const char *fpath)
{
    log_node_t *new_node = NULL;
    
    assert(watch_list != NULL);
    new_node = calloc(1, sizeof(log_node_t));
    
    if (new_node == NULL) {
        return -1;
    }
   
    new_node->wd = wd;
    memcpy(new_node->path, fpath, strlen(fpath));

    new_node->next = watch_list->head;
    watch_list->head = new_node;
    watch_list->length++;

#if LOG_DIR_DEBUG() > 0
    printf("%s: added (%d, %s)\n", __func__, wd, fpath);
#endif /* LOG_DIR_DEBUG() > 0 */

    return 0;
}

static log_node_t*
log_list_remove(int wd)
{
    log_node_t *curr_node = NULL;
    log_node_t *prev_node = NULL;
    
    assert(watch_list != NULL);
   
    if (watch_list->head == NULL) {
        return NULL;
    }

    if (watch_list->head->wd == wd) {
        curr_node = watch_list->head;
        watch_list->head = curr_node->next;
        watch_list->length--;
        return curr_node;
    }
   
    prev_node = watch_list->head;
     
    for (curr_node = prev_node->next; curr_node; curr_node = curr_node->next) {
        if (curr_node->wd == wd) {
            prev_node->next = curr_node->next;
            watch_list->length--;
            return curr_node;
        }
        prev_node = curr_node;
    }

#if LOG_DIR_DEBUG() > 0
    printf("%s: removed (%d, %s)\n", __func__, curr_node->wd, curr_node->path);
#endif /* LOG_DIR_DEBUG() > 0 */

    return NULL;
}

static char *
log_list_find_path(int wd)
{
    log_node_t *curr_node = NULL;
    
    assert(watch_list != NULL);

    if (watch_list->head == NULL) {
        return NULL;
    }

    for (curr_node = watch_list->head; curr_node; curr_node = curr_node->next) {
        if (curr_node->wd == wd) {
            return curr_node->path;
        }
    }

    return NULL;
}

static void
log_list_dump()
{
    log_node_t *curr_node = NULL;

    assert(watch_list != NULL);

    for (curr_node = watch_list->head; curr_node; curr_node = curr_node->next) {
        printf("(%d, %s)\n", curr_node->wd, curr_node->path);
    }
    return;
}

static int
track_dir(const char *fpath, const struct stat *sb, int flag, struct FTW *ftwbuf)
{
    int rc; 
    int wd;

    wd = inotify_add_watch(inotify_fd, fpath, IN_ALL_EVENTS);

    if (wd == -1) {
        fprintf(stderr, "ERROR: failed to add %s to watch list -- %s\n", 
                fpath, strerror(errno));
        return -1; 
    }
    
    rc = log_list_add(wd, fpath);
    if (rc == -1) {
        fprintf(stderr, "ERROR: failed to add %s into list -- %s\n",
                fpath, strerror(errno));
        return -1;
    }

    return 0;
}

static int
handle_create_dir(struct inotify_event *i)
{
    int rc; 
    int new_wd; 
    char *dir_path; 
    char new_dir[PATH_MAX];     

    memset(new_dir, '\0', PATH_MAX);
    if (i->len == 0) {
        return 0;
    }
    
    dir_path = log_list_find_path(i->wd);
    if (dir_path == NULL) {
        return -1;
    }
    
    strncpy(new_dir, dir_path, strlen(dir_path));
    strncat(new_dir, "/", 1);
    strncat(new_dir, i->name, i->len);
    
    new_wd = inotify_add_watch(inotify_fd, new_dir, IN_ALL_EVENTS); 
    if (new_wd == -1) {
        fprintf(stderr, "ERROR: inotify_add_watch failed to add %s -- %s\n",
                new_dir, strerror(errno));
        return -1;
    }

    rc = log_list_add(new_wd, new_dir);
    if (rc == -1) {
        fprintf(stderr, "ERROR: failed to add (%d, %s) to watch list -- %s\n",
                new_wd, dir_path, strerror(errno));
        return -1;
    }

    return 0;
}

static int
handle_delete_dir(struct inotify_event *i)
{
    int rc;
    log_node_t *del_node;
   
    rc = inotify_rm_watch(inotify_fd, i->wd);
    if (rc == -1) {
        fprintf(stderr, "ERROR: inotify_rm_watch failed to remove %d -- %s\n",
                i->wd, strerror(errno));
        return -1;
    }

    del_node = log_list_remove(i->wd);
    if (del_node == NULL) {
        fprintf(stderr, "ERROR: failed to remove %d from watch list\n",
                i->wd);
        return -1;
    }
    
    free(del_node);
    del_node = NULL;

    return 0;
}

static void
parse_event(struct inotify_event *i)
{
    int rc;

    printf("    wd =%d; ", i->wd);
    if (i->cookie > 0) {
        printf("cookie =%4d; ", i->cookie);
    }

    printf("mask = ");
    if (i->mask & IN_ACCESS)        
        printf("IN_ACCESS ");
    
    if (i->mask & IN_ATTRIB)        
        printf("IN_ATTRIB ");
    
    if (i->mask & IN_CLOSE_NOWRITE) 
        printf("IN_CLOSE_NOWRITE ");
    
    if (i->mask & IN_CLOSE_WRITE)   
        printf("IN_CLOSE_WRITE ");
    
    if (i->mask & IN_CREATE)        
        printf("IN_CREATE ");
    
    if (i->mask & IN_DELETE)        
        printf("IN_DELETE ");
    
    if (i->mask & IN_DELETE_SELF) 
        printf("IN_DELTE_SELF ");
    
    if (i->mask & IN_IGNORED)       
        printf("IN_IGNORED ");
    
    if (i->mask & IN_ISDIR)         
        printf("IN_ISDIR ");
    
    if (i->mask & IN_MODIFY)        
        printf("IN_MODIFY ");
    
    if (i->mask & IN_MOVE_SELF)     
        printf("IN_MOVE_SELF ");
    
    if (i->mask & IN_MOVED_FROM)    
        printf("IN_MOVED_FROM ");
    
    if (i->mask & IN_MOVED_TO)      
        printf("IN_MOVED_TO ");
    
    if (i->mask & IN_OPEN)          
        printf("IN_OPEN");
    
    if (i->mask & IN_Q_OVERFLOW)    
        printf("IN_Q_OVERFLOW");
    
    if (i->mask & IN_UNMOUNT)       
        printf("IN_UNMOUNT");
    printf("\n");

    if (i->len > 0) {
        printf("        name = %s\n", i->name);
    }

    if (i->mask & IN_ISDIR && i->mask & IN_CREATE) {
        rc = handle_create_dir(i);
#if LOG_DIR_DEBUG() > 0
        if (rc) {
            fprintf(stderr, "ERROR: handle_create_dir\n");
        }
#endif /* LOG_DIR_DEBUG() > 0 */
    }
    
    if (i->mask & IN_ISDIR && i->mask & IN_MOVED_TO) {
        rc = handle_create_dir(i);
#if LOG_DIR_DEBUG() > 0
        if (rc) {
            fprintf(stderr, "ERROR: handle_create_dir\n");
        }
#endif /* LOG_DIR_DEBUG() > 0 */
    }

    if (i->mask & IN_ISDIR && i->mask & IN_DELETE_SELF) {
        rc = handle_delete_dir(i);
#if LOG_DIR_DEBUG() > 0
        if (rc) {
            fprintf(stderr, "ERROR: handle_delete_dir\n");
        }
#endif /* LOG_DIR_DEBUG() > 0 */
    }

    if (i->mask & IN_ISDIR && i->mask & IN_MOVED_FROM) {
        rc = handle_delete_dir(i);
#if LOG_DIR_DEBUG() > 0
        if (rc) {
            fprintf(stderr, "ERROR: handle_delete_dir\n");
        }
#endif /* LOG_DIR_DEBUG() > 0 */
    }
}

static void
log_dir()
{
    ssize_t num_read;
    char *p;
    struct inotify_event *event;
    
    char buf[BUF_LEN] __attribute__((aligned(8)));

#if LOG_DIR_DEBUG() > 0
    log_list_dump();
#endif /* LOG_DIR_DEBUG() > 0 */

    for (;;) {
        num_read = read(inotify_fd, buf, BUF_LEN);
        if (num_read == -1) {
            fprintf(stderr, "ERROR: failed to read from inotify_fd -- %s\n",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }

        for (p = buf; p < buf + num_read; ) {
            event = (struct inotify_event *)p;
            parse_event(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }
}

int
main(int argc, char *argv[])
{
    if (argc > 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: ./%s <dir_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    watch_list_init(); 
    if (watch_list == NULL) {
        fprintf(stderr, "ERROR: unable to initialize watch list -- %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        fprintf(stderr, "ERROR: unable to initialize inotify -- %s\n", 
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if (nftw(argv[1], track_dir, 10, 0) == -1) {
        fprintf(stderr, "ERROR: unable to start nftw -- %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    log_dir();

    if (close(inotify_fd) == -1) {
        fprintf(stderr, "ERROR: failed to close inotify_fd -- %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
