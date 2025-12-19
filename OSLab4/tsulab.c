#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/timekeeping.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Kernel module with /proc output");

#define procfs_name "tsulab"
static struct proc_dir_entry *out_proc_file = NULL;

// Время подписания Декларации независимости США:
// 04.07.1776 14:22 UTC
#define USA_BIRTH_TIMESTAMP -6106069320LL

static void calculate_usa_age_seconds(char* buffer, size_t max_len)
{
    u64 now_sec = ktime_get_real_seconds();
    s64 seconds_existed = now_sec - USA_BIRTH_TIMESTAMP;
    
    if (seconds_existed < 0) {
        snprintf(buffer, max_len, "Ошибка: США ещё не существует!\n");
        return;
    }
    snprintf(buffer, max_len, 
             "США существует: %llu секунд\n"
             "Декларация независимости подписана: 4 июля 1776 года, 14:22\n",
             seconds_existed);
}

static ssize_t procfile_read(
    struct file *file_pointer, char __user *buffer, 
    size_t buffer_length, loff_t *offset)
{
    char msg[256];
    int msg_len;
    
    if (*offset > 0)
        return 0;
    
    calculate_usa_age_seconds(msg, sizeof(msg));
    msg_len = strlen(msg);
    
    if (copy_to_user(buffer, msg, msg_len))
        return -EFAULT;
    
    *offset = msg_len;
    return msg_len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops proc_file_ops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_ops = {
    .read = procfile_read,
};
#endif

static int __init tsu_init(void)
{
    pr_info("Welcome to the Tomsk State University\n");
    
    out_proc_file = proc_create(procfs_name, 0444, NULL, &proc_file_ops);
    
    if (out_proc_file == NULL) {
        pr_err("Could not initialize /proc/%s\n", procfs_name);
        return -ENOMEM;
    }
    
    pr_info("/proc/%s created\n", procfs_name);
    return 0;
}

static void __exit tsu_exit(void)
{
    proc_remove(out_proc_file);
    pr_info("/proc/%s removed\n", procfs_name);
    pr_info("Tomsk State University forever!\n");
}

module_init(tsu_init);
module_exit(tsu_exit);