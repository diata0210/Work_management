#ifndef TASK_H
#define TASK_H

typedef struct {
    int task_id;
    char name[256];
    char description[512];
    char status[32];
    int assignee_id;
} Task;

typedef struct {
    Task *tasks;
    int count;
} TaskArray;

#endif // TASK_H
