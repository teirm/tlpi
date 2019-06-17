/*
 * my_pstree
 *
 * Print out the current process graph in a 
 * slightly less pretty manner than the pstree command
 */
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "tree_print/tree_print.h"

#define MAX_CHILDREN 100
#define MAX_TREES    100
#define MAX_LINE     1024

/* BUFFER_SIZE CALCULATION:
 *   Maximum entry size + 
 *   "/proc/" (6) + 
 *   "/status" (7)
 *   + "\0" (1)
 */
#define BUF_SIZE (PATH_MAX + 14)

#define DEBUG 0

struct process_info_s {
    pid_t pid;
    pid_t ppid;
    char command[MAX_LINE];
    struct process_info_s *children[MAX_CHILDREN];
};
typedef struct process_info_s process_info_t;

struct process_list_node_s {
    process_info_t *info;
    struct process_list_node_s *next;
};
typedef struct process_list_node_s process_list_node_t;

struct process_list_head_s {
    int length;
    process_list_node_t *first;
};
typedef struct process_list_head_s process_list_head_t;

/* NOTE: setup a linked-list of processes and 
 * fill it up in an ordered manner. The tree
 * can then be built safely based on pid / ppid
 * relationships.
 *
 * What if a pid is read but the ppid is never
 * added to the list because it exited -- the pid
 * is assumed to be a zombie or inherited by init and 
 * reaped -- so can probably discard in all cases.
 */
static process_info_t *process_proc_status(const char *d_name);
static void build_procfs_tree(DIR *procfs);

/* list related functions */
static void add_node_in_order(process_list_head_t *head, process_info_t *value);
static process_list_head_t *init_process_list();
/* note: do not destroy node values as a tree is over-laid on the list */
static void destroy_process_list(process_list_head_t *head);
static void dump_proc_list(process_list_head_t *head);
static void proc_list_to_proc_tree(process_list_head_t *head);

/* tree related functions */
static void destroy_node(tree_node_t *node);
static int parent_child_comparator(void *a, void *b);
static void print_node(tree_node_t *node);

static void 
destroy_node(tree_node_t *node)
{
    assert(node != NULL);
    free(node->value);
    node->value = NULL;
}

static int 
parent_child_comparator(void *parent, void *child)
{
    process_info_t *proc_a = (process_info_t *)parent;
    process_info_t *proc_b = (process_info_t *)child;

    return proc_a->pid == proc_b->ppid;
}

static void 
print_node(tree_node_t *node)
{
    process_info_t *proc_info = NULL;

    assert(node != NULL);

    proc_info = (process_info_t *)node->value;

    printf("Pid: %d PPid: %d Command: %s\n", 
            proc_info->pid, proc_info->ppid, proc_info->command);

}

static process_list_head_t*
init_process_list()
{
    process_list_head_t *head = NULL;

    head = malloc(sizeof(process_list_head_t));
    
    if (head == NULL) {
        return NULL;
    }
    
    head->length = 0;
    head->first = NULL;
    
    return head;
}

static void
destroy_process_list(process_list_head_t *head)
{
    process_list_node_t *curr_node = NULL;
    process_list_node_t *next_node = NULL;

    curr_node = head->first;

    while (curr_node != NULL) {
        next_node = curr_node->next;
        free(curr_node);
        curr_node = next_node;
    }

    free(head);
}

static void
add_node_in_order(process_list_head_t *head, process_info_t *value)
{
    pid_t node_pid; 
    process_list_node_t *curr_node = NULL;
    process_list_node_t *prev_node = NULL;
    process_list_node_t *new_node = NULL; 

    assert(head != NULL);
    
    new_node = malloc(sizeof(process_list_node_t));
    if (new_node == NULL) {
        fprintf(stderr, "Failed to allocate new node.\n");
        return;
    }

    new_node->info = value;
    new_node->next = NULL;
    
    /* case 1: no nodes in the list -- add to list head as first */
    if (head->first == NULL) {
        head->first = new_node;
        return;
    }

    /* case 2: pid is less than first -- new becomes first and old first becomes
     *                                   second.
     */
    if (value->pid < head->first->info->pid) {
        new_node->next = head->first;
        head->first = new_node;
        return;
    }

    /* case 3: pid lies somewhere in the list */
    for (curr_node = head->first; curr_node != NULL; curr_node = curr_node->next) {
        if (value->pid < curr_node->info->pid) {
            new_node->next = curr_node;
            prev_node->next = new_node;
            return;
        }
        prev_node = curr_node;
    } 
    
    /* case 4: pid lies at the end of the list */
    prev_node->next = new_node;
    return;
}

static void
dump_proc_list(process_list_head_t *head)
{
    process_list_node_t *curr_node = NULL;
    process_info_t *curr_proc = NULL; 

    for (curr_node = head->first; curr_node != NULL; curr_node = curr_node->next) {
        curr_proc = curr_node->info;
        printf("Pid: %d PPid: %d Command: %s\n",
                curr_proc->pid, curr_proc->ppid, curr_proc->command);
    }
}

static void
proc_list_to_proc_tree(process_list_head_t *head)
{
    int rc = 0;
    tree_node_t *root = NULL;
    tree_node_t *parent_tree_node = NULL;
    process_list_node_t *curr_node = NULL;
    process_info_t *pid_zero = NULL;

    assert(head != NULL);

    /* pid 0 starts init and kthread and is not in procfs. 
     * Account for this by making it the root */

    pid_zero = malloc(sizeof(process_info_t));
    if (pid_zero == NULL) {
        fprintf(stderr, "Unable to allocate pid 0\n");
        exit(EXIT_FAILURE);
    }
    
    memset(pid_zero, '\0', sizeof(process_info_t));
    pid_zero->pid = 0;
    pid_zero->ppid = 0;
    strncpy(pid_zero->command, "-", 1);

    root = init_tree(pid_zero);

    for (curr_node = head->first; curr_node != NULL; curr_node = curr_node->next) {
        parent_tree_node = find_node(root, curr_node->info, parent_child_comparator);
        if (parent_tree_node == NULL) {
            fprintf(stderr, "Unable to find parent tree node: %d\n",
                    curr_node->info->ppid);
        } else {
            rc = add_child(parent_tree_node, curr_node->info);
        }
    }
    
    print_tree(root, print_node);
    destroy_tree(root, destroy_node);
}

static process_info_t*
process_proc_status(const char *d_name)
{
    pid_t pid;
    pid_t ppid;
    
    char label[MAX_LINE];
    char command_name[MAX_LINE];
    char abs_proc_path[BUF_SIZE] = "/proc/";    
   
    process_info_t *info = NULL;
    FILE *status_file = NULL;
    
    strncat(abs_proc_path, d_name, strlen(d_name));
    strncat(abs_proc_path, "/status", strlen("/status")+1);

    status_file = fopen(abs_proc_path, "r");
    if (status_file == NULL) {
        printf("status file was NULL\n");
        return NULL;
    }
    
    while (fscanf(status_file, "%s", label) != EOF) {
        if (strcmp(label, "Name:") == 0) {
            fscanf(status_file, "%s", command_name);
        } else if (strcmp(label, "Pid:") == 0) {
            fscanf(status_file, "%d", (int *) &pid);
        } else if (strcmp(label, "PPid:") == 0) {
            fscanf(status_file, "%d", (int *) &ppid);
        }
    }
    
    if (fclose(status_file) == EOF) {
        fprintf(stderr, "Error closing status_file\n");
        return NULL;
    }
    
    info = malloc(sizeof(process_info_t));
    if (info == NULL) {
        fprintf(stderr, "Unable to allocate process info.\n"); 
        return NULL;
    }

    memset(info, '\0', sizeof(process_info_t));
    info->pid = pid;
    info->ppid = ppid;
    strncpy(info->command, command_name, strlen(command_name)+1);
    
    return info;
}

static void
build_procfs_tree(DIR *procfs)
{
    struct dirent *curr_dir = NULL;
    
    process_list_head_t *head = NULL; 
    process_info_t *current_process = NULL; 

    head = init_process_list(); 
    if (head == NULL) {
        fprintf(stderr, "Failed to initialize process list.\n");
        exit(EXIT_FAILURE);
    }

    while ((curr_dir = readdir(procfs)) != NULL) {
        if (strtol(curr_dir->d_name, NULL, 10) != 0) {
            current_process = process_proc_status(curr_dir->d_name);
            if (current_process != NULL) {
#if DEBUG 
            printf("Pid: %d PPid: %d Command: %s\n",
                    current_process->pid, current_process->ppid,
                    current_process->command);
#endif /* DEBUG */
            (void)add_node_in_order(head, current_process);
            }
        }
    }
#if DEBUG
    (void)dump_proc_list(head);
#endif /* DEBUG */
    
    proc_list_to_proc_tree(head);
    destroy_process_list(head); 

}

int
main()
{
    DIR *procfs = NULL; 

    procfs = opendir("/proc/");

    if (procfs == NULL) {
        fprintf(stderr, "Unable to open procfs.\n");
        exit(EXIT_FAILURE);
    }
    
    build_procfs_tree(procfs);

    if (closedir(procfs)) {
        fprintf(stderr, "Unable to close procfs.\n");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
