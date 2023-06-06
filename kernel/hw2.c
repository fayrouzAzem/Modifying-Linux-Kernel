#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/limits.h>

asmlinkage long sys_hello(void) {
	printk("Hello, World!\n");
	return 0;
}

asmlinkage long sys_set_weight(int weight){
    if(weight < 0) return -EINVAL;
    else {
    	current->weight = weight;
    	return 0;
    }
}

asmlinkage long sys_get_weight(void) {
    return current->weight;
}

asmlinkage long sys_get_ancestor_sum(void) {
    struct task_struct *task;
    int sum = 0;

    for(task = current; task->pid != 0; task = task->parent) {
        sum += task->weight;
    }

    return sum;
}


void get_heaviest_descendant_recursive(struct task_struct* curr, long* max_weight, pid_t* max_pid, bool is_root){
    struct list_head *iter;
    struct task_struct *task;
    
    if(!is_root && (curr->weight > *max_weight || (curr->weight == *max_weight && curr->pid < *max_pid))) {
	*max_weight = curr->weight;
	*max_pid = curr->pid;
     }

    list_for_each(iter, &(curr -> children)){
	task = list_entry(iter,struct task_struct, sibling);
        get_heaviest_descendant_recursive(task, max_weight, max_pid, false); 
	
    }
}

asmlinkage long sys_get_heaviest_descendant(void) {
    long max_weight = -1;
    pid_t max_pid = INT_MAX;

    if (list_empty(&(current->children))) {
        return -ECHILD;
    }

    get_heaviest_descendant_recursive(current, &max_weight, &max_pid, true);
    return max_pid;
}



