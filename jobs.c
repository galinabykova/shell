#include "jobs.h"

const int N = 10; 
int step = 5; 
const int stepToReduceStep = 2; 
const int stepFirst=5; 
const int nameSize = 10; 
const int criticalSize = 100; 
int crit = 0; 
char unknownStr[10]="unknown"; 

int newTaskStruct ()
{ 
        ta.headFreeTask = 0;
        ta.lastInOrder = -1; 

        ta.zero = malloc(sizeof(struct Task) * (unsigned int)N); 
        ta.order = malloc(sizeof(int) * (unsigned int)N); 
        if (!ta.zero) {
                fprintf(stderr, "didn't allocate memory to TaskStruct");
                return -1; 
        }
        if (!ta.order) {
                fprintf(stderr, "didn't allocate memory to order");
                free(ta.zero);
                return -1; 
        }
        ta.size = N;
        for (int i = 0;i < N;++i) {
                ta.zero[i].next = i + 1;
                ta.zero[i].name = NULL;
                ta.zero[i].cnt = 0;
                ta.zero[i].pid = 0;
                ta.zero[i].placeInOrder=-1;
        }
        ta.tailFreeTask = ta.size - 1;
        ta.headWorkTask = -1; 
        for (int i = 0; i < N; ++i) {
                ta.order[i] = -1; 
        }
        return 0;
}

void deleteTaskStruct()
{
        if (ta.size == 0) return;    
        int now = ta.headWorkTask;
        while (now != -1) {
                if (ta.zero[now].pid > 0) kill(-ta.zero[now].pid, SIGKILL); //
                if (ta.zero[now].name && strcmp(ta.zero[now].name,unknownStr)) free(ta.zero[now].name);
                now = ta.zero[now].next;
        }
        free(ta.zero);
        free(ta.order);
}

int increaseTaskStruct()
{
        if (ta.size == 0) {
                return newTaskStruct();
        }

        struct Task *new;
        new = realloc(ta.zero, sizeof(struct Task) * ((unsigned int)(ta.size + step)));
        while (!new && step > stepToReduceStep) {
                step = step - stepToReduceStep;
                new = realloc(ta.zero, sizeof(struct Task) * ((unsigned int)(ta.size + step)));
        }
        while (!new && step > 0) {
                --step;
                new = realloc(ta.zero, sizeof(struct Task) * ((unsigned int)(ta.size + step)));
        }
        if (!step) {
                step=stepFirst;
                fprintf(stderr,"There is no memory to increase the size of arr.");
                return -1;
        }
        int *newOrder = realloc(ta.order, sizeof(int) * ((unsigned int)(ta.size + step)));
        if (newOrder) {
                ta.order = newOrder;
        }

        int newsize = (ta.size) + step;
        for (int i = ta.size;i < newsize;++i) {
                new[i].next = i+1;
                ta.order[i] = -1;
                new[i].name = NULL;
                new[i].cnt = 0;
                new[i].pid = 0;
                new[i].placeInOrder=-1;
        }
        ta.zero = new;
        ta.size = newsize;
        if (ta.size > criticalSize) {
                crit = 1;
        }
        ta.tailFreeTask = ta.size - 1;
        return 0;
}

int pushTask(int pid, char *n, int cnt)
{
        if (!ta.zero) newTaskStruct();

        int place = ta.headFreeTask;
        if (place == ta.size) {
                int f = 1;
                if (crit) {
                        f = !findExited();
                }
                if (f) {
                        increaseTaskStruct();
                }
        }
        place = ta.headFreeTask;

        ta.headFreeTask = ta.zero[place].next;
        ta.zero[place].pid = getpgid(pid);
        ta.zero[place].cnt = cnt;
        ta.zero[place].next = ta.headWorkTask;
        ta.headWorkTask = place;

        ta.zero[place].name = malloc(sizeof(char) * (unsigned int)nameSize);//
        if (ta.zero[place].name) {
                strncpy(ta.zero[place].name, n, (unsigned int)nameSize);
                ta.zero[place].name[nameSize - 1] = '\0';
        } else {
                ta.zero[place].name = unknownStr;
        }

        ta.lastInOrder = (ta.lastInOrder + 1) % ta.size;
        ta.zero[place].placeInOrder = ta.lastInOrder;
        ta.order[ta.lastInOrder] = place;

        return place + 1;
}

int changeTaskCnt(int index, int cnt)
{
        if (index < 1) return -1;
        if (cnt <= 0) {
                ta.zero[index - 1].pid = -1; 
        }
        ta.zero[index - 1].cnt = cnt;
        return 0;
}

int pushToOrder(int index)
{
        --index;
        if (index < 0) return -1;
        ta.order[ta.zero[index].placeInOrder] = -1;
        ta.lastInOrder=(ta.lastInOrder + 1) % ta.size;
        ta.order[ta.lastInOrder] = index;
        ta.zero[index].placeInOrder = ta.lastInOrder;
        return 0;
}

int popToOrder(int index)
{
        if (index < 1) return -1;
        --index;
        ta.order[ta.zero[index].placeInOrder] = -1;
        return 0;
}

int findExited()
{
        if (!ta.size) return 0;
        int now = ta.headWorkTask;
        int last = -1;
        int everythingIsDeleted = 0;
        while (now != -1) {
                int pidN = ta.zero[now].pid;
                int nnext = ta.zero[now].next;
                while (waitpid(-pidN, 0, WNOHANG)>0 && ta.zero[now].cnt>0) {
                    --(ta.zero[now].cnt);
                }
                if (pidN == -1 || ta.zero[now].cnt <= 0) {
                        everythingIsDeleted = 1;
                        ta.order[ta.zero[now].placeInOrder] = -1;
                        if (pidN != -1) printf("[%d] Completed %s\n", now + 1, ta.zero[now].name);
                        ta.zero[now].pid = 0;
                        if (strcmp(ta.zero[now].name,unknownStr)) free(ta.zero[now].name);

                        if (last != -1) ta.zero[last].next = ta.zero[now].next;
                        else ta.headWorkTask = ta.zero[now].next;

                        if (ta.headFreeTask == ta.size) {
                                ta.headFreeTask = now;
                        } else {
                                ta.zero[ta.tailFreeTask].next = now;
                        }
                        ta.tailFreeTask = now;

                        ta.zero[now].next = ta.size;
                } else {
                        last = now;
                }
                now = nnext;
        }
        return everythingIsDeleted;
}

int getTaskPid(int index)
{
        if (index > ta.size || index < 1) return 0;
        return ta.zero[index - 1].pid;
}

int getTaskCnt(int index)  //
{
        if (index > ta.size || index < 1) return 0;
        return ta.zero[index - 1].cnt;
}

int peekOrder()
{
    if (ta.lastInOrder == -1) return -1;
        int j = 0;
        while (ta.order[ta.lastInOrder]==-1) {
                if (j >= ta.size) return -1;
                ta.lastInOrder = (ta.lastInOrder + ta.size - 1) % ta.size;
                ++j;
        }
        return ta.order[ta.lastInOrder] + 1;
}

void printTaskStruct()
{
        printf("Head %d\n", ta.headFreeTask);
        printf("Tail %d\n", ta.tailFreeTask);
        printf("HeadW %d\n", ta.headWorkTask);
        for (int i = 0; i < ta.size; ++i) {
                printf("%d %d %d %d\n", i, ta.zero[i].next, ta.zero[i].pid, ta.zero[i].placeInOrder);
        }
        printf("LastInOrder %d\n", ta.lastInOrder);
        for (int i = 0; i < ta.size; ++i) {
                printf("%d %d\n", i, ta.order[i]);
        }
        printf("\n");
}

void nearJobs()
{
        if (crit) findExited();
        if (ta.size == 0) return;
        if (ta.lastInOrder == -1) return;

        int j = 0;
        while (ta.order[ta.lastInOrder] == -1 && j!=ta.size) {
                ta.lastInOrder = (ta.lastInOrder + ta.size - 1) % ta.size;
                ++j;
        }

        j = 0;
        int lastlast = (ta.lastInOrder + ta.size - 1) % ta.size;
        while (ta.order[lastlast] == -1 && j != ta.size) {
                lastlast = (lastlast + ta.size - 1) % ta.size;
                ++j;
        }
        
        for (int i = 0; i < ta.size; ++i) {
                int pidN = ta.zero[i].pid;
                if (!pidN) continue;
                if (ta.zero[i].placeInOrder == ta.lastInOrder) {
                        printf("[%d]+ %d %s\n", i + 1, ta.zero[i].pid, ta.zero[i].name);
                } else {
                        if (ta.zero[i].placeInOrder == lastlast) printf("[%d]- %d %s\n", i+1, ta.zero[i].pid, ta.zero[i].name);
                        else printf("[%d] %d %s\n", i + 1, ta.zero[i].pid, ta.zero[i].name);
                }
        }
}

inline void printExitedJobs(int now)
{
        --now;
        printf("[%d] %d %s\n", now + 1, ta.zero[now].pid, ta.zero[now].name);
}
